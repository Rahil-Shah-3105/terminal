//----------------------------------------------------------------------------------------------------------------------
// <copyright file="TerminalApp.cs" company="Microsoft">
//     Copyright (c) Microsoft Corporation.  All rights reserved.
// </copyright>
// <summary>Helper and wrapper for generating the base test application and its UI Root node.</summary>
//----------------------------------------------------------------------------------------------------------------------
namespace WindowsTerminal.UIA.Tests.Elements
{
    using System;
    using System.IO;

    using WindowsTerminal.UIA.Tests.Common;
    using WindowsTerminal.UIA.Tests.Common.NativeMethods;

    using OpenQA.Selenium.Remote;
    using OpenQA.Selenium.Appium;
    using OpenQA.Selenium.Appium.iOS;
    using OpenQA.Selenium.Interactions;

    using WEX.Logging.Interop;
    using WEX.TestExecution;
    using WEX.TestExecution.Markup;

    using System.Runtime.InteropServices;
    using System.Security.Principal;
    using OpenQA.Selenium;
	using Microsoft.Extensions.Logging;

    public class TerminalApp : IDisposable
    {
		private static readonly Microsoft.Extensions.Logging.ILogger _logger = Microsoft.Extensions.Logging.Abstractions.NullLogger.Instance;
        protected const string AppDriverUrl = "http://127.0.0.1:4723";

        private IntPtr job;

        public IOSDriver<IOSElement> Session { get; private set; }
        public Actions Actions { get; private set; }
        public AppiumWebElement UIRoot { get; private set; }

        private bool isDisposed = false;

        private TestContext context;

        public string ContentPath { get; private set; }
        public string GetFullTestContentPath(string filename)
        {
			_logger.LogDebug(">>> Entering GetFullTestContentPath(filename={Filename})", filename);
			_logger.LogDebug("<<< Exiting GetFullTestContentPath(filename={Filename})", filename);
            return Path.GetFullPath(Path.Combine(ContentPath, filename));
        }

        public TerminalApp(TestContext context, string shellToLaunch = "powershell.exe")
        {
			_logger.LogDebug(">>> Entering TerminalApp(context={Context},shellToLaunch={ShellToLaunch})", context, shellToLaunch);
            this.context = context;

			_logger.LogDebug("TerminalApp(context={Context},shellToLaunch={ShellToLaunch}): this.context → {Arg2}", context, shellToLaunch, this.context);
			_logger.LogDebug("TerminalApp(context={Context},shellToLaunch={ShellToLaunch}): context → {Context1}", context, shellToLaunch, context);
            // If running locally, set WTPath to where we can find a loose
            // deployment of Windows Terminal. That means you'll need to build
            // the Terminal appx, then use
            // New-UnpackagedTerminalDistribution.ps1 to build an unpackaged
            // layout that can successfully launch. Then, point the tests at
            // that WindowsTerminal.exe like so:
            //
            //   te.exe WindowsTerminal.UIA.Tests.dll /p:WTPath=C:\the\path\to\the\unpackaged\layout\WindowsTerminal.exe
            //
            // On the build machines, the scripts lay it out at the terminal-0.0.1.0\ subfolder of the test deployment directory
            string path = Path.GetFullPath(Path.Combine(context.TestDeploymentDir, @"terminal-0.0.1.0\WindowsTerminal.exe"));
			_logger.LogDebug("TerminalApp(context={Context},shellToLaunch={ShellToLaunch}): path → {Path}", context, shellToLaunch, path);
            if (context.Properties.Contains("WTPath"))
            {
                path = (string)context.Properties["WTPath"];
            }
            Log.Comment($"Windows Terminal will be launched from '{path}'");

            // Same goes for the content directory. Set WTTestContent for where the content files are
            // for running tests.
            // On the build machines, the scripts lay it out at the content\ subfolder.
            ContentPath = @"content";
            if (context.Properties.Contains("WTTestContent"))
            {
                ContentPath = (string)context.Properties["WTTestContent"];
            }
            Log.Comment($"Test Content will be loaded from '{Path.GetFullPath(ContentPath)}'");

            this.CreateProcess(path, shellToLaunch);
			_logger.LogDebug("<<< Exiting TerminalApp(context={Context},shellToLaunch={ShellToLaunch})", context, shellToLaunch);
        }

        ~TerminalApp()
        {
            this.Dispose(false);
        }

        public void Dispose()
        {
			_logger.LogDebug(">>> Entering Dispose()");
            this.Dispose(true);
            GC.SuppressFinalize(this);
			_logger.LogDebug("<<< Exiting Dispose()");
        }

        public AppiumWebElement GetRoot()
        {
			_logger.LogDebug(">>> Entering GetRoot()");
			_logger.LogDebug("<<< Exiting GetRoot()");
            return this.UIRoot;
        }

        protected virtual void Dispose(bool disposing)
        {
			_logger.LogDebug(">>> Entering Dispose(disposing={Disposing})", disposing);
            if (!this.isDisposed)
            {
                // ensure we're exited when this is destroyed or disposed of explicitly
                this.ExitProcess();

                this.isDisposed = true;
				_logger.LogDebug("Dispose(disposing={Disposing}): this.isDisposed → {Arg1}", disposing, this.isDisposed);
            }
			_logger.LogDebug("<<< Exiting Dispose(disposing={Disposing})", disposing);
        }

        private void CreateProcess(string path, string shellToLaunch)
        {
			_logger.LogDebug(">>> Entering CreateProcess(path={Path},shellToLaunch={ShellToLaunch})", path, shellToLaunch);
            string WindowTitleToFind = "WindowsTerminal.UIA.Tests";

            job = WinBase.CreateJobObject(IntPtr.Zero, IntPtr.Zero);
            NativeMethods.Win32NullHelper(job, "Creating job object to hold binaries under test.");
			_logger.LogDebug("CreateProcess(path={Path},shellToLaunch={ShellToLaunch}): job → {Job}", path, shellToLaunch, job);

            Log.Comment("Attempting to launch command-line application at '{0}'", path);

            string binaryToRunPath = path;
            string args = $"new-tab --title \"{WindowTitleToFind}\" --suppressApplicationTitle \"{shellToLaunch}\"";

            string launchArgs = $"{binaryToRunPath} {args}";

            WinBase.STARTUPINFO si = new WinBase.STARTUPINFO();
			_logger.LogDebug("CreateProcess(path={Path},shellToLaunch={ShellToLaunch}): si → {Si}", path, shellToLaunch, si);
            si.cb = Marshal.SizeOf(si);

            WinBase.PROCESS_INFORMATION pi = new WinBase.PROCESS_INFORMATION();
			_logger.LogDebug("CreateProcess(path={Path},shellToLaunch={ShellToLaunch}): pi → {Pi}", path, shellToLaunch, pi);

            NativeMethods.Win32BoolHelper(WinBase.CreateProcess(null,
                launchArgs,
                IntPtr.Zero,
                IntPtr.Zero,
                false,
                WinBase.CP_CreationFlags.CREATE_SUSPENDED,
                IntPtr.Zero,
                null,
                ref si,
                out pi),
                "Attempting to create child host window process.");

            Log.Comment($"Host window PID: {pi.dwProcessId}");

            NativeMethods.Win32BoolHelper(WinBase.AssignProcessToJobObject(job, pi.hProcess), "Assigning new host window (suspended) to job object.");
            NativeMethods.Win32BoolHelper(-1 != WinBase.ResumeThread(pi.hThread), "Resume host window process now that it is attached and its launch of the child application will be caught in the job object.");

            Globals.WaitForTimeout();

            DesiredCapabilities appCapabilities = new DesiredCapabilities();
			_logger.LogDebug("CreateProcess(path={Path},shellToLaunch={ShellToLaunch}): appCapabilities → {AppCapabilities}", path, shellToLaunch, appCapabilities);
            appCapabilities.SetCapability("app", @"Root");
            Session = new IOSDriver<IOSElement>(new Uri(AppDriverUrl), appCapabilities);

			_logger.LogDebug("CreateProcess(path={Path},shellToLaunch={ShellToLaunch}): Session → {Session}", path, shellToLaunch, Session);
            Verify.IsNotNull(Session);
            Actions = new Actions(Session);
            Verify.IsNotNull(Session);
			_logger.LogDebug("CreateProcess(path={Path},shellToLaunch={ShellToLaunch}): Actions → {Actions}", path, shellToLaunch, Actions);

            Session.Manage().Timeouts().ImplicitWait = TimeSpan.FromSeconds(15);
            UIRoot = Session.FindElementByName(WindowTitleToFind);
			_logger.LogDebug("CreateProcess(path={Path},shellToLaunch={ShellToLaunch}): UIRoot → {UIRoot}", path, shellToLaunch, UIRoot);
			_logger.LogDebug("<<< Exiting CreateProcess(path={Path},shellToLaunch={ShellToLaunch})", path, shellToLaunch);
        }

        private bool IsRunningAsAdmin()
        {
			_logger.LogDebug(">>> Entering IsRunningAsAdmin()");
			_logger.LogDebug("<<< Exiting IsRunningAsAdmin()");
            return new WindowsPrincipal(WindowsIdentity.GetCurrent()).IsInRole(WindowsBuiltInRole.Administrator);
        }

        private void ExitProcess()
        {
			_logger.LogDebug(">>> Entering ExitProcess()");
            Globals.SweepAllModules(this.context);

            // Release attachment to the child process console.
            WinCon.FreeConsole();

            this.UIRoot = null;
			_logger.LogDebug("ExitProcess(): UIRoot → {UIRoot}", UIRoot);
			_logger.LogDebug("ExitProcess(): this.UIRoot → {Arg0}", this.UIRoot);

            if (this.job != IntPtr.Zero)
            {
                WinBase.TerminateJobObject(this.job, 0);
                this.job = IntPtr.Zero;
				_logger.LogDebug("ExitProcess(): job → {Job}", job);
				_logger.LogDebug("ExitProcess(): this.job → {Arg0}", this.job);
            }
			_logger.LogDebug("<<< Exiting ExitProcess()");
        }
    }
}
