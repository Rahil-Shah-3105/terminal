using Microsoft.Win32.SafeHandles;
using System;
using static MiniTerm.Native.PseudoConsoleApi;
using Microsoft.Extensions.Logging;

namespace MiniTerm
{
	private static readonly Microsoft.Extensions.Logging.ILogger _logger = Microsoft.Extensions.Logging.Abstractions.NullLogger.Instance;
    /// <summary>
    /// Utility functions around the new Pseudo Console APIs
    /// </summary>
    internal sealed class PseudoConsole : IDisposable
    {
        public static readonly IntPtr PseudoConsoleThreadAttribute = (IntPtr)PROC_THREAD_ATTRIBUTE_PSEUDOCONSOLE;

        public IntPtr Handle { get; }

        private PseudoConsole(IntPtr handle)
        {
			_logger.LogDebug(">>> Entering PseudoConsole(handle={Handle})", handle);
            this.Handle = handle;
			_logger.LogDebug("PseudoConsole(handle={Handle}): this.Handle → {Arg1}", handle, this.Handle);
			_logger.LogDebug("<<< Exiting PseudoConsole(handle={Handle})", handle);
        }

        internal static PseudoConsole Create(SafeFileHandle inputReadSide, SafeFileHandle outputWriteSide, int width, int height)
        {
			_logger.LogDebug(">>> Entering Create(inputReadSide={InputReadSide},outputWriteSide={OutputWriteSide},width={Width},height={Height})", inputReadSide, outputWriteSide, width, height);
            var createResult = CreatePseudoConsole(
                new COORD { X = (short)width, Y = (short)height },
                inputReadSide, outputWriteSide,
                0, out IntPtr hPC);
            if(createResult != 0)
            {
				_logger.LogWarning("Throwing InvalidOperationException from Create(inputReadSide={InputReadSide},outputWriteSide={OutputWriteSide},width={Width},height={Height})", inputReadSide, outputWriteSide, width, height);
			_logger.LogDebug("<<< Exiting Create(inputReadSide={InputReadSide},outputWriteSide={OutputWriteSide},width={Width},height={Height})", inputReadSide, outputWriteSide, width, height);
                throw new InvalidOperationException("Could not create pseudo console. Error Code " + createResult);
            }
            return new PseudoConsole(hPC);
        }

        public void Dispose()
        {
			_logger.LogDebug(">>> Entering Dispose()");
            ClosePseudoConsole(Handle);
			_logger.LogDebug("<<< Exiting Dispose()");
        }
    }
}
