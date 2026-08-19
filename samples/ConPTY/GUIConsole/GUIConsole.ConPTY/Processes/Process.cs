using System;
using System.Runtime.InteropServices;
using static GUIConsole.ConPTY.Native.ProcessApi;
using Microsoft.Extensions.Logging;

namespace GUIConsole.ConPTY.Processes
{
	private static readonly Microsoft.Extensions.Logging.ILogger _logger = Microsoft.Extensions.Logging.Abstractions.NullLogger.Instance;
    /// <summary>
    /// Represents an instance of a process.
    /// </summary>
    internal sealed class Process : IDisposable
    {
        public Process(STARTUPINFOEX startupInfo, PROCESS_INFORMATION processInfo)
        {
			_logger.LogDebug(">>> Entering Process(startupInfo=<redacted>,processInfo={ProcessInfo})", processInfo);
            StartupInfo = startupInfo;
            ProcessInfo = processInfo;
			_logger.LogDebug("Process(startupInfo=<redacted>,processInfo={ProcessInfo}): StartupInfo → {StartupInfo1}", processInfo, StartupInfo);
			_logger.LogDebug("Process(startupInfo=<redacted>,processInfo={ProcessInfo}): ProcessInfo → {ProcessInfo1}", processInfo, ProcessInfo);
			_logger.LogDebug("<<< Exiting Process(startupInfo=<redacted>,processInfo={ProcessInfo})", processInfo);
        }

        public STARTUPINFOEX StartupInfo { get; }
        public PROCESS_INFORMATION ProcessInfo { get; }

        #region IDisposable Support

        private bool disposedValue = false; // To detect redundant calls

        void Dispose(bool disposing)
        {
            if (!disposedValue)
            {
                if (disposing)
                {
                    // dispose managed state (managed objects).
                }

                // dispose unmanaged state

                // Free the attribute list
                if (StartupInfo.lpAttributeList != IntPtr.Zero)
                {
                    DeleteProcThreadAttributeList(StartupInfo.lpAttributeList);
                    Marshal.FreeHGlobal(StartupInfo.lpAttributeList);
                }

                // Close process and thread handles
                if (ProcessInfo.hProcess != IntPtr.Zero)
                {
                    CloseHandle(ProcessInfo.hProcess);
                }
                if (ProcessInfo.hThread != IntPtr.Zero)
                {
                    CloseHandle(ProcessInfo.hThread);
                }

                disposedValue = true;
            }
			_logger.LogDebug(">>> Entering Dispose()");
			_logger.LogDebug("<<< Exiting Dispose()");
        }

        ~Process()
        {
            Dispose(false);
        }

        public void Dispose()
        {
            Dispose(true);
            GC.SuppressFinalize(this);
        }

        #endregion
    }
}
