using System;
using System.Runtime.InteropServices;
using static MiniTerm.Native.ProcessApi;
using Microsoft.Extensions.Logging;

namespace MiniTerm
{
	private static readonly Microsoft.Extensions.Logging.ILogger _logger = Microsoft.Extensions.Logging.Abstractions.NullLogger.Instance;
    /// <summary>
    /// Support for starting and configuring processes.
    /// </summary>
    /// <remarks>
    /// Possible to replace with managed code? The key is being able to provide the PROC_THREAD_ATTRIBUTE_PSEUDOCONSOLE attribute
    /// </remarks>
    static class ProcessFactory
    {
        /// <summary>
        /// Start and configure a process. The return value represents the process and should be disposed.
        /// </summary>
        internal static Process Start(string command, IntPtr attributes, IntPtr hPC)
        {
			_logger.LogDebug(">>> Entering Start(command={Command},attributes={Attributes},hPC={HPC})", command, attributes, hPC);
            var startupInfo = ConfigureProcessThread(hPC, attributes);
            var processInfo = RunProcess(ref startupInfo, command);
			_logger.LogDebug("Start(command={Command},attributes={Attributes},hPC={HPC}): processInfo → {ProcessInfo}", command, attributes, hPC, processInfo);
			_logger.LogDebug("Start(command={Command},attributes={Attributes},hPC={HPC}): startupInfo → {StartupInfo}", command, attributes, hPC, startupInfo);
			_logger.LogDebug("<<< Exiting Start(command={Command},attributes={Attributes},hPC={HPC})", command, attributes, hPC);
            return new Process(startupInfo, processInfo);
        }

        private static STARTUPINFOEX ConfigureProcessThread(IntPtr hPC, IntPtr attributes)
        {
			_logger.LogDebug(">>> Entering ConfigureProcessThread(hPC={HPC},attributes={Attributes})", hPC, attributes);
            // this method implements the behavior described in https://docs.microsoft.com/en-us/windows/console/creating-a-pseudoconsole-session#preparing-for-creation-of-the-child-process

            var lpSize = IntPtr.Zero;
            var success = InitializeProcThreadAttributeList(
                lpAttributeList: IntPtr.Zero,
                dwAttributeCount: 1,
                dwFlags: 0,
                lpSize: ref lpSize
            );
            if (success || lpSize == IntPtr.Zero) // we're not expecting `success` here, we just want to get the calculated lpSize
            {
				_logger.LogWarning("Throwing InvalidOperationException from ConfigureProcessThread(hPC={HPC},attributes={Attributes})", hPC, attributes);
                throw new InvalidOperationException("Could not calculate the number of bytes for the attribute list. " + Marshal.GetLastWin32Error());
            }

            var startupInfo = new STARTUPINFOEX();
			_logger.LogDebug("ConfigureProcessThread(hPC={HPC},attributes={Attributes}): startupInfo → {StartupInfo}", hPC, attributes, startupInfo);
            startupInfo.StartupInfo.cb = Marshal.SizeOf<STARTUPINFOEX>();
            startupInfo.lpAttributeList = Marshal.AllocHGlobal(lpSize);

            success = InitializeProcThreadAttributeList(
                lpAttributeList: startupInfo.lpAttributeList,
                dwAttributeCount: 1,
                dwFlags: 0,
                lpSize: ref lpSize
            );
            if (!success)
            {
                throw new InvalidOperationException("Could not set up attribute list. " + Marshal.GetLastWin32Error());
            }

            success = UpdateProcThreadAttribute(
                lpAttributeList: startupInfo.lpAttributeList,
                dwFlags: 0,
                attribute: attributes,
                lpValue: hPC,
                cbSize: (IntPtr)IntPtr.Size,
                lpPreviousValue: IntPtr.Zero,
                lpReturnSize: IntPtr.Zero
            );
            if (!success)
            {
			_logger.LogDebug("<<< Exiting ConfigureProcessThread(hPC={HPC},attributes={Attributes})", hPC, attributes);
                throw new InvalidOperationException("Could not set pseudoconsole thread attribute. " + Marshal.GetLastWin32Error());
            }

            return startupInfo;
        }

        private static PROCESS_INFORMATION RunProcess(ref STARTUPINFOEX sInfoEx, string commandLine)
        {
			_logger.LogDebug(">>> Entering RunProcess(sInfoEx={SInfoEx},commandLine={CommandLine})", sInfoEx, commandLine);
            int securityAttributeSize = Marshal.SizeOf<SECURITY_ATTRIBUTES>();
            var pSec = new SECURITY_ATTRIBUTES { nLength = securityAttributeSize };
			_logger.LogDebug("RunProcess(sInfoEx={SInfoEx},commandLine={CommandLine}): securityAttributeSize → {SecurityAttributeSize}", sInfoEx, commandLine, securityAttributeSize);
            var tSec = new SECURITY_ATTRIBUTES { nLength = securityAttributeSize };
            var success = CreateProcess(
                lpApplicationName: null,
                lpCommandLine: commandLine,
                lpProcessAttributes: ref pSec,
                lpThreadAttributes: ref tSec,
                bInheritHandles: false,
                dwCreationFlags: EXTENDED_STARTUPINFO_PRESENT,
                lpEnvironment: IntPtr.Zero,
                lpCurrentDirectory: null,
                lpStartupInfo: ref sInfoEx,
                lpProcessInformation: out PROCESS_INFORMATION pInfo
            );
            if (!success)
            {
				_logger.LogWarning("Throwing InvalidOperationException from RunProcess(sInfoEx={SInfoEx},commandLine={CommandLine})", sInfoEx, commandLine);
			_logger.LogDebug("<<< Exiting RunProcess(sInfoEx={SInfoEx},commandLine={CommandLine})", sInfoEx, commandLine);
                throw new InvalidOperationException("Could not create process. " + Marshal.GetLastWin32Error());
            }

            return pInfo;
        }
    }
}
