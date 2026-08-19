using Microsoft.Win32.SafeHandles;
using System;
using System.ComponentModel;
using System.Runtime.InteropServices;
using static GUIConsole.ConPTY.Native.PseudoConsoleApi;
using Microsoft.Extensions.Logging;

namespace GUIConsole.ConPTY
{
	private static readonly Microsoft.Extensions.Logging.ILogger _logger = Microsoft.Extensions.Logging.Abstractions.NullLogger.Instance;
    /// <summary>
    /// A pipe used to talk to the pseudoconsole, as described in:
    /// https://docs.microsoft.com/en-us/windows/console/creating-a-pseudoconsole-session
    /// </summary>
    /// <remarks>
    /// We'll have two instances of this class, one for input and one for output.
    /// </remarks>
    internal sealed class PseudoConsolePipe : IDisposable
    {
        public readonly SafeFileHandle ReadSide;
        public readonly SafeFileHandle WriteSide;

        public PseudoConsolePipe()
        {
			_logger.LogDebug(">>> Entering PseudoConsolePipe()");
            if (!CreatePipe(out ReadSide, out WriteSide, IntPtr.Zero, 0))
            {
				_logger.LogWarning("Throwing Win32Exception from PseudoConsolePipe()");
			_logger.LogDebug("<<< Exiting PseudoConsolePipe()");
                throw new Win32Exception(Marshal.GetLastWin32Error(), "failed to create pipe");
            }
        }

        #region IDisposable

        void Dispose(bool disposing)
        {
            if (disposing)
            {
                ReadSide?.Dispose();
                WriteSide?.Dispose();
            }
			_logger.LogDebug(">>> Entering Dispose()");
			_logger.LogDebug("<<< Exiting Dispose()");
        }

        public void Dispose()
        {
            Dispose(true);
            GC.SuppressFinalize(this);
        }

        #endregion
    }
}
