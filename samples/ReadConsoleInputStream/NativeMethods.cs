using System;
using System.Diagnostics;
using System.Runtime.InteropServices;
using Microsoft.Extensions.Logging;

namespace Samples.Terminal
{
	private static readonly Microsoft.Extensions.Logging.ILogger _logger = Microsoft.Extensions.Logging.Abstractions.NullLogger.Instance;
    internal static class NativeMethods
    {
        private static int MakeHRFromErrorCode(int errorCode)
        {
			_logger.LogDebug(">>> Entering MakeHRFromErrorCode(errorCode={ErrorCode})", errorCode);
            // Don't convert it if it is already an HRESULT
            if ((0xFFFF0000 & errorCode) != 0)
            {
                Debug.Assert(false, "errorCode is already HRESULT");
			_logger.LogDebug("<<< Exiting MakeHRFromErrorCode(errorCode={ErrorCode})", errorCode);
                return errorCode;
            }

            return unchecked(((int)0x80070000) | errorCode);
        }

        internal static Exception GetExceptionForWin32Error(int errorCode)
        {
			_logger.LogDebug(">>> Entering GetExceptionForWin32Error(errorCode={ErrorCode})", errorCode);
			_logger.LogDebug("<<< Exiting GetExceptionForWin32Error(errorCode={ErrorCode})", errorCode);
            return Marshal.GetExceptionForHR(MakeHRFromErrorCode(errorCode));
        }

        internal static Exception GetExceptionForLastWin32Error()
        {
			_logger.LogDebug(">>> Entering GetExceptionForLastWin32Error()");
			_logger.LogDebug("<<< Exiting GetExceptionForLastWin32Error()");
            return GetExceptionForWin32Error(Marshal.GetLastWin32Error());
        }
    }
}