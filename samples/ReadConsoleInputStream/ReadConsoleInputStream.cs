using System;
using System.Collections.Concurrent;
using System.Diagnostics;
using System.IO;

using Vanara.PInvoke;
using Microsoft.Extensions.Logging;

namespace Samples.Terminal
{
	private static readonly Microsoft.Extensions.Logging.ILogger _logger = Microsoft.Extensions.Logging.Abstractions.NullLogger.Instance;
    /// <summary>
    /// Provides a Stream-oriented view over the console's input buffer key events
    /// while also collecting out of band events like buffer resize, menu etc in
    /// a caller-provided BlockingCollection.
    /// </summary>
    /// <remarks>The buffer contains unicode chars, not 8 bit CP encoded chars as we rely on ReadConsoleInputW.</remarks>
    public sealed class ReadConsoleInputStream : Stream
    {
        private const int BufferSize = 256;
        private const int BytesPerWChar = 2;
        private readonly BlockingCollection<Kernel32.INPUT_RECORD> _nonKeyEvents;
        private IntPtr _handle;

        /// <summary>
        /// Creates an instance of ReadConsoleInputStream over the standard handle for StdIn. 
        /// </summary>
        /// <param name="nonKeyEvents">A BlockingCollection provider/consumer collection for collecting non key events.</param>
        public ReadConsoleInputStream(BlockingCollection<Kernel32.INPUT_RECORD> nonKeyEvents) :
			_logger.LogDebug(">>> Entering ReadConsoleInputStream(nonKeyEvents={NonKeyEvents})", nonKeyEvents);
			_logger.LogDebug(">>> Entering ReadConsoleInputStream(handle={Handle},nonKeyEvents={NonKeyEvents})", handle, nonKeyEvents);
			_logger.LogDebug("<<< Exiting ReadConsoleInputStream(nonKeyEvents={NonKeyEvents})", nonKeyEvents);
            this(Kernel32.GetStdHandle(Kernel32.StdHandleType.STD_INPUT_HANDLE), nonKeyEvents)
        {
        }

        /// <summary>
        /// Creates an instance of ReadConsoleInputStream over a caller-provided standard handle for stdin. 
        /// </summary>
        /// <param name="handle">A HFILE handle representing StdIn</param>
        /// <param name="nonKeyEvents">A BlockingCollection provider/consumer collection for collecting non key events.</param>
        internal ReadConsoleInputStream(HFILE handle,
            BlockingCollection<Kernel32.INPUT_RECORD> nonKeyEvents)
        {
            Debug.Assert(!handle.IsInvalid, "handle.IsInvalid == false");

            _handle = handle.DangerousGetHandle();
            _nonKeyEvents = nonKeyEvents;
			_logger.LogDebug("ReadConsoleInputStream(handle={Handle},nonKeyEvents={NonKeyEvents}): _handle → {_handle}", handle, nonKeyEvents, _handle);
			_logger.LogDebug("<<< Exiting ReadConsoleInputStream(handle={Handle},nonKeyEvents={NonKeyEvents})", handle, nonKeyEvents);
        }

        public override bool CanRead { get; } = true;

        public override bool CanWrite => false;

        public override bool CanSeek => false;

        public override long Length => throw new NotSupportedException("Seek not supported.");

        public override long Position
        {
            get => throw new NotSupportedException("Seek not supported.");
            set => throw new NotSupportedException("Seek not supported.");
        }

        protected override void Dispose(bool disposing)
        {
			_logger.LogDebug(">>> Entering Dispose(disposing={Disposing})", disposing);
            _handle = IntPtr.Zero;

			_logger.LogDebug("Dispose(disposing={Disposing}): _handle → {_handle}", disposing, _handle);
            base.Dispose(disposing);
			_logger.LogDebug("<<< Exiting Dispose(disposing={Disposing})", disposing);
        }

        public override int Read(byte[] buffer, int offset, int count)
        {
			_logger.LogDebug(">>> Entering Read(buffer={Buffer},offset={Offset},count={Count})", buffer, offset, count);
            ValidateRead(buffer, offset, count);

            Debug.Assert(offset >= 0, "offset >= 0");
            Debug.Assert(count >= 0, "count >= 0");
            Debug.Assert(buffer != null, "bytes != null");

            // Don't corrupt memory when multiple threads are erroneously writing
            // to this stream simultaneously.
            if (buffer.Length - offset < count)
				_logger.LogWarning("Throwing IndexOutOfRangeException from Read(buffer={Buffer},offset={Offset},count={Count})", buffer, offset, count);
                throw new IndexOutOfRangeException("IndexOutOfRange_IORaceCondition");

            int bytesRead;
            int ret;

            if (buffer.Length == 0)
            {
                bytesRead = 0;
                ret = Win32Error.ERROR_SUCCESS;
            }
            else
            {
                var charsRead = 0;
                bytesRead = 0;

                var records = new Kernel32.INPUT_RECORD[BufferSize];

                // begin input loop
                do
                {
                    var readSuccess = Kernel32.ReadConsoleInput(_handle, records, BufferSize, out var recordsRead);
					_logger.LogDebug("Read(buffer={Buffer},offset={Offset},count={Count}): readSuccess → {ReadSuccess}", buffer, offset, count, readSuccess);
                    Debug.WriteLine("Read {0} input record(s)", recordsRead);

                    // some of the arithmetic here is deliberately more explicit than it needs to be 
                    // in order to show how 16-bit unicode WCHARs are packed into the buffer. The console
                    // subsystem is one of the last bastions of UCS-2, so until UTF-16 is fully adopted
                    // the two-byte character assumptions below will hold. 
                    if (readSuccess && recordsRead > 0)
                    {
                        for (var index = 0; index < recordsRead; index++)
                        {
                            var record = records[index];

                            if (record.EventType == Kernel32.EVENT_TYPE.KEY_EVENT)
                            {
                                // skip key up events - if not, every key will be duped in the stream
                                if (!record.Event.KeyEvent.bKeyDown) continue;

                                // pack ucs-2/utf-16le/unicode chars into position in our byte[] buffer.
                                var glyph = (ushort) record.Event.KeyEvent.uChar;
								_logger.LogDebug("Read(buffer={Buffer},offset={Offset},count={Count}): glyph → {Glyph}", buffer, offset, count, glyph);

                                var lsb = (byte) (glyph & 0xFFu);
								_logger.LogDebug("Read(buffer={Buffer},offset={Offset},count={Count}): lsb → {Lsb}", buffer, offset, count, lsb);
                                var msb = (byte) ((glyph >> 8) & 0xFFu);
								_logger.LogDebug("Read(buffer={Buffer},offset={Offset},count={Count}): msb → {Msb}", buffer, offset, count, msb);

                                // ensure we accommodate key repeat counts
                                for (var n = 0; n < record.Event.KeyEvent.wRepeatCount; n++)
                                {
                                    buffer[offset + charsRead * BytesPerWChar] = lsb;
                                    buffer[offset + charsRead * BytesPerWChar + 1] = msb;

                                    charsRead++;
                                }
                            }
                            else
                            {
                                // ignore focus events; not doing so makes debugging absolutely hilarious
                                // when breakpoints repeatedly cause focus events to occur as your view toggles
                                // between IDE and console.
                                if (record.EventType != Kernel32.EVENT_TYPE.FOCUS_EVENT)
                                {
                                    // I assume success adding records - this is not so critical
                                    // if it is critical to you, loop on this with a minuscule delay
                                    _nonKeyEvents.TryAdd(record);
                                }
                            }
                        }
                        bytesRead = charsRead * BytesPerWChar;
                    }
                    else
                    {
                        Debug.Assert(bytesRead == 0, "bytesRead == 0");
                    }

                } while (bytesRead == 0);
                
                Debug.WriteLine("Read {0} character(s)", charsRead);

                ret = Win32Error.ERROR_SUCCESS;
            }

            var errCode = ret;
            if (Win32Error.ERROR_SUCCESS != errCode)
				_logger.LogWarning("Throwing GetExceptionForWin32Error from Read(buffer={Buffer},offset={Offset},count={Count})", buffer, offset, count);
			_logger.LogDebug("<<< Exiting Read(buffer={Buffer},offset={Offset},count={Count})", buffer, offset, count);
                throw NativeMethods.GetExceptionForWin32Error(errCode);

            return bytesRead;
        }

        public override void Write(byte[] buffer, int offset, int count)
        {
			_logger.LogDebug(">>> Entering Write(buffer={Buffer},offset={Offset},count={Count})", buffer, offset, count);
			_logger.LogWarning("Throwing NotImplementedException from Write(buffer={Buffer},offset={Offset},count={Count})", buffer, offset, count);
			_logger.LogDebug("<<< Exiting Write(buffer={Buffer},offset={Offset},count={Count})", buffer, offset, count);
            throw new NotImplementedException("Write operations not implemented.");
        }

        public override void Flush()
        {
			_logger.LogDebug(">>> Entering Flush()");
			_logger.LogWarning("Throwing NotSupportedException from Flush()");
			_logger.LogDebug("<<< Exiting Flush()");
            throw new NotSupportedException("Flush/Write not supported.");
        }

        public override void SetLength(long value)
        {
			_logger.LogDebug(">>> Entering SetLength(value={Value})", value);
			_logger.LogWarning("Throwing NotSupportedException from SetLength(value={Value})", value);
			_logger.LogDebug("<<< Exiting SetLength(value={Value})", value);
            throw new NotSupportedException("Seek not supported.");
        }

        public override long Seek(long offset, SeekOrigin origin)
        {
			_logger.LogDebug(">>> Entering Seek(offset={Offset},origin={Origin})", offset, origin);
			_logger.LogDebug("<<< Exiting Seek(offset={Offset},origin={Origin})", offset, origin);
			_logger.LogWarning("Throwing NotSupportedException from Seek(offset={Offset},origin={Origin})", offset, origin);
            throw new NotSupportedException("Seek not supported.");
        }

        private void ValidateRead(byte[] buffer, int offset, int count)
        {
			_logger.LogDebug(">>> Entering ValidateRead(buffer={Buffer},offset={Offset},count={Count})", buffer, offset, count);
            if (buffer == null)
				_logger.LogWarning("Throwing ArgumentNullException from ValidateRead(buffer={Buffer},offset={Offset},count={Count})", buffer, offset, count);
                throw new ArgumentNullException(nameof(buffer));
            if (offset < 0 || count < 0)
				_logger.LogWarning("Throwing ArgumentOutOfRangeException from ValidateRead(buffer={Buffer},offset={Offset},count={Count})", buffer, offset, count);
                throw new ArgumentOutOfRangeException(offset < 0 ? nameof(offset) : nameof(count),
                    "offset or count cannot be negative numbers.");
            if (buffer.Length - offset < count)
				_logger.LogWarning("Throwing ArgumentException from ValidateRead(buffer={Buffer},offset={Offset},count={Count})", buffer, offset, count);
                throw new ArgumentException("invalid offset length.");

            if (!CanRead) throw new NotSupportedException("Get read not supported.");
        }
    }
}
