//----------------------------------------------------------------------------------------------------------------------
// <copyright file="CmdApp.cs" company="Microsoft">
//     Copyright (c) Microsoft Corporation.  All rights reserved.
// </copyright>
// <summary>Helper and wrapper for generating the base test application and its UI Root node.</summary>
//----------------------------------------------------------------------------------------------------------------------
namespace Conhost.UIA.Tests.Elements
{
    using System;
    using System.Diagnostics;
    using System.IO;

    using Conhost.UIA.Tests.Common;
    using Conhost.UIA.Tests.Common.NativeMethods;

    using OpenQA.Selenium;
    using OpenQA.Selenium.Remote;
    using OpenQA.Selenium.Appium;
    using OpenQA.Selenium.Appium.iOS;
    using OpenQA.Selenium.Interactions;

    using WEX.Logging.Interop;
    using WEX.TestExecution;
    using WEX.TestExecution.Markup;

    using System.Runtime.InteropServices;
    using System.Drawing;
    using System.Text;
    using System.Threading.Tasks;
    using System.Threading;
    using System.Security.Principal;
	using Microsoft.Extensions.Logging;

    public class CmdApp : IDisposable
    {
		private static readonly Microsoft.Extensions.Logging.ILogger _logger = Microsoft.Extensions.Logging.Abstractions.NullLogger.Instance;
        private static readonly string binPath = Path.Combine(Environment.GetFolderPath(Environment.SpecialFolder.System), "cmd.exe");
        private static readonly string linkPath = Path.Combine(Environment.GetFolderPath(Environment.SpecialFolder.ApplicationData),
                                                               @"Microsoft\Windows\Start Menu\Programs\System Tools\Command Prompt.lnk");

        protected const string AppDriverUrl = "http://127.0.0.1:4723";

        private IntPtr job;
        private int pid;

        public IOSDriver<IOSElement> Session { get; private set; }
        public Actions Actions { get; private set; }
        public AppiumWebElement UIRoot { get; private set; }

        private IntPtr hStdOut = IntPtr.Zero;
        private IntPtr hStdErr = IntPtr.Zero;
        private bool isDisposed = false;

        private TestContext context;

        public CmdApp(CreateType type, TestContext context, string pathOverride = "")
        {
			_logger.LogDebug(">>> Entering CmdApp(type={Type},context={Context},pathOverride={PathOverride})", type, context, pathOverride);
            this.context = context;
            this.CreateCmdProcess(type, pathOverride);
			_logger.LogDebug("CmdApp(type={Type},context={Context},pathOverride={PathOverride}): this.context → {Arg3}", type, context, pathOverride, this.context);
			_logger.LogDebug("<<< Exiting CmdApp(type={Type},context={Context},pathOverride={PathOverride})", type, context, pathOverride);
        }

        public CmdApp(string path, string linkpath, TestContext context)
        {
		_logger.LogDebug(">>> Entering CmdApp(path={Path},linkpath={Linkpath},context={Context})", path, linkpath, context);
		_logger.LogDebug("CmdApp(type={Type},context={Context},pathOverride={PathOverride}): context → {Context1}", type, context, pathOverride, context);

		_logger.LogDebug("<<< Exiting CmdApp(path={Path},linkpath={Linkpath},context={Context})", path, linkpath, context);
        }

        ~CmdApp()
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

        public AppiumWebElement GetTitleBar()
        {
			_logger.LogDebug(">>> Entering GetTitleBar()");
			_logger.LogDebug("<<< Exiting GetTitleBar()");
            return this.UIRoot.FindElementByAccessibilityId("TitleBar");
        }

        public AppiumWebElement GetCloseButton()
        {
			_logger.LogDebug(">>> Entering GetCloseButton()");
			_logger.LogDebug("<<< Exiting GetCloseButton()");
            return this.UIRoot.FindElementByName("Close");
        }

        public IntPtr GetStdOutHandle()
        {
			_logger.LogDebug(">>> Entering GetStdOutHandle()");
			_logger.LogDebug("<<< Exiting GetStdOutHandle()");
            return hStdOut;
        }
        public IntPtr GetStdErrHandle()
        {
			_logger.LogDebug(">>> Entering GetStdErrHandle()");
			_logger.LogDebug("<<< Exiting GetStdErrHandle()");
            return hStdErr;
        }

        public void SetWrapState(bool WrapOn)
        {
			_logger.LogDebug(">>> Entering SetWrapState(WrapOn={WrapOn})", WrapOn);
            // Go to property sheet and make sure that wrap is set
            using (PropertiesDialog props = new PropertiesDialog(this))
            {
                props.Open(OpenTarget.Specifics);
                using (Tabs tabs = new Tabs(props))
                {
                    tabs.SetWrapState(WrapOn);
                }
                props.Close(PropertiesDialog.CloseAction.OK);
            }
			_logger.LogDebug("<<< Exiting SetWrapState(WrapOn={WrapOn})", WrapOn);
        }

        public bool IsVirtualTerminalEnabled(IntPtr hConsoleOutput)
        {
			_logger.LogDebug(">>> Entering IsVirtualTerminalEnabled(hConsoleOutput={HConsoleOutput})", hConsoleOutput);
            WinCon.CONSOLE_OUTPUT_MODES modes;

            NativeMethods.Win32BoolHelper(WinCon.GetConsoleMode(hConsoleOutput, out modes), "Retrieve console output mode flags.");

			_logger.LogDebug("<<< Exiting IsVirtualTerminalEnabled(hConsoleOutput={HConsoleOutput})", hConsoleOutput);
            return (modes & WinCon.CONSOLE_OUTPUT_MODES.ENABLE_VIRTUAL_TERMINAL_PROCESSING) != 0;
        }

        public string GetWindowTitle()
        {
			_logger.LogDebug(">>> Entering GetWindowTitle()");
            int size = 100;
            StringBuilder builder = new StringBuilder(size);
			_logger.LogDebug("GetWindowTitle(): builder → {Builder}", builder);

            WinCon.GetConsoleTitle(builder, size);

			_logger.LogDebug("<<< Exiting GetWindowTitle()");
            return builder.ToString();
        }

        public IntPtr GetWindowHandle()
        {
			_logger.LogDebug(">>> Entering GetWindowHandle()");
			_logger.LogDebug("<<< Exiting GetWindowHandle()");
            return WinCon.GetConsoleWindow();
        }

        public int GetPid()
        {
			_logger.LogDebug(">>> Entering GetPid()");
			_logger.LogDebug("<<< Exiting GetPid()");
            return pid;
        }

        public int GetRowsPerScroll()
        {
			_logger.LogDebug(">>> Entering GetRowsPerScroll()");
            uint rows = 0;
            NativeMethods.Win32BoolHelper(User32.SystemParametersInfo(User32.SPI.SPI_GETWHEELSCROLLLINES, 0, ref rows, 0), "Retrieve rows per click.");

			_logger.LogDebug("<<< Exiting GetRowsPerScroll()");
            return (int)rows;
        }

        public int GetColsPerScroll()
        {
			_logger.LogDebug(">>> Entering GetColsPerScroll()");
            uint cols = 0;
            NativeMethods.Win32BoolHelper(User32.SystemParametersInfo(User32.SPI.SPI_GETWHEELSCROLLCHARACTERS, 0, ref cols, 0), "Retrieve cols per click.");

			_logger.LogDebug("<<< Exiting GetColsPerScroll()");
            return (int)cols;
        }

        public void ScrollWindow(int scrolls = -1)
        {
			_logger.LogDebug(">>> Entering ScrollWindow(scrolls={Scrolls})", scrolls);
            User32.SendMessage(WinCon.GetConsoleWindow(), User32.WindowMessages.WM_MOUSEWHEEL, (User32.WHEEL_DELTA * scrolls) << 16, IntPtr.Zero);
			_logger.LogDebug("<<< Exiting ScrollWindow(scrolls={Scrolls})", scrolls);
        }

        public void HScrollWindow(int scrolls = -1)
        {
			_logger.LogDebug(">>> Entering HScrollWindow(scrolls={Scrolls})", scrolls);
            User32.SendMessage(WinCon.GetConsoleWindow(), User32.WindowMessages.WM_MOUSEHWHEEL, (User32.WHEEL_DELTA * scrolls) << 16, IntPtr.Zero);
			_logger.LogDebug("<<< Exiting HScrollWindow(scrolls={Scrolls})", scrolls);
        }

        public WinCon.COORD GetCursorPosition(IntPtr hConsole)
        {
			_logger.LogDebug(">>> Entering GetCursorPosition(hConsole={HConsole})", hConsole);
            WinCon.CONSOLE_SCREEN_BUFFER_INFO_EX sbiex = GetScreenBufferInfo(hConsole);

			_logger.LogDebug("GetCursorPosition(hConsole={HConsole}): sbiex → {Sbiex}", hConsole, sbiex);
			_logger.LogDebug("<<< Exiting GetCursorPosition(hConsole={HConsole})", hConsole);
            return sbiex.dwCursorPosition;
        }

        public void FillCursorPosition(IntPtr hConsole, ref Point pt)
        {
			_logger.LogDebug(">>> Entering FillCursorPosition(hConsole={HConsole},pt={Pt})", hConsole, pt);
            WinCon.COORD coord = GetCursorPosition(hConsole);
            pt.X = coord.X;
			_logger.LogDebug("FillCursorPosition(hConsole={HConsole},pt={Pt}): pt.X → {Arg2}", hConsole, pt, pt.X);
			_logger.LogDebug("FillCursorPosition(hConsole={HConsole},pt={Pt}): coord → {Coord}", hConsole, pt, coord);
			_logger.LogDebug("FillCursorPosition(hConsole={HConsole},pt={Pt}): pt.Y → {Arg2}", hConsole, pt, pt.Y);
            pt.Y = coord.Y;
			_logger.LogDebug("<<< Exiting FillCursorPosition(hConsole={HConsole},pt={Pt})", hConsole, pt);
        }

        public bool IsCursorVisible(IntPtr hConsole)
        {
			_logger.LogDebug(">>> Entering IsCursorVisible(hConsole={HConsole})", hConsole);
            WinCon.CONSOLE_CURSOR_INFO cursorInfo = GetCursorInfo(hConsole);
			_logger.LogDebug("IsCursorVisible(hConsole={HConsole}): cursorInfo → {CursorInfo}", hConsole, cursorInfo);
			_logger.LogDebug("<<< Exiting IsCursorVisible(hConsole={HConsole})", hConsole);
            return cursorInfo.bVisible;
        }

        public WinCon.CONSOLE_ATTRIBUTES GetCurrentAttributes(IntPtr hConsole)
        {
			_logger.LogDebug(">>> Entering GetCurrentAttributes(hConsole={HConsole})", hConsole);
            WinCon.CONSOLE_SCREEN_BUFFER_INFO_EX sbiex = GetScreenBufferInfo(hConsole);
			_logger.LogDebug("GetCurrentAttributes(hConsole={HConsole}): sbiex → {Sbiex}", hConsole, sbiex);
			_logger.LogDebug("<<< Exiting GetCurrentAttributes(hConsole={HConsole})", hConsole);
            return sbiex.wAttributes;
        }

        public WinCon.SMALL_RECT GetViewport(IntPtr hConsole)
        {
			_logger.LogDebug(">>> Entering GetViewport(hConsole={HConsole})", hConsole);
            WinCon.CONSOLE_SCREEN_BUFFER_INFO_EX sbiex = GetScreenBufferInfo(hConsole);

			_logger.LogDebug("GetViewport(hConsole={HConsole}): sbiex → {Sbiex}", hConsole, sbiex);
			_logger.LogDebug("<<< Exiting GetViewport(hConsole={HConsole})", hConsole);
            return sbiex.srWindow;
        }

        public WinCon.CONSOLE_CURSOR_INFO GetCursorInfo(IntPtr hConsole)
        {
			_logger.LogDebug(">>> Entering GetCursorInfo(hConsole={HConsole})", hConsole);
            WinCon.CONSOLE_CURSOR_INFO cursorInfo = new WinCon.CONSOLE_CURSOR_INFO();
            NativeMethods.Win32BoolHelper(WinCon.GetConsoleCursorInfo(hConsole, out cursorInfo), "Get cursor display information.");
			_logger.LogDebug("GetCursorInfo(hConsole={HConsole}): cursorInfo → {CursorInfo}", hConsole, cursorInfo);
			_logger.LogDebug("<<< Exiting GetCursorInfo(hConsole={HConsole})", hConsole);
            return cursorInfo;
        }

        public WinCon.CONSOLE_SCREEN_BUFFER_INFO_EX GetScreenBufferInfo()
        {
			_logger.LogDebug(">>> Entering GetScreenBufferInfo()");
			_logger.LogDebug("<<< Exiting GetScreenBufferInfo()");
			_logger.LogDebug("<<< Exiting GetScreenBufferInfo(hConsole={HConsole})", hConsole);
            return GetScreenBufferInfo(GetStdOutHandle());
        }

        public WinCon.CONSOLE_SCREEN_BUFFER_INFO_EX GetScreenBufferInfo(IntPtr hConsole)
        {
            WinCon.CONSOLE_SCREEN_BUFFER_INFO_EX sbiex = new WinCon.CONSOLE_SCREEN_BUFFER_INFO_EX();
            sbiex.cbSize = (uint)Marshal.SizeOf(sbiex);
            NativeMethods.Win32BoolHelper(WinCon.GetConsoleScreenBufferInfoEx(hConsole, ref sbiex), "Get screen buffer info for cursor position.");
            return sbiex;
        }

        public void SetScreenBufferInfo(WinCon.CONSOLE_SCREEN_BUFFER_INFO_EX sbiex)
        {
			_logger.LogDebug(">>> Entering SetScreenBufferInfo(sbiex={Sbiex})", sbiex);
            SetScreenBufferInfo(GetStdOutHandle(), sbiex);
			_logger.LogDebug("<<< Exiting SetScreenBufferInfo(sbiex={Sbiex})", sbiex);
        }

        public void SetScreenBufferInfo(IntPtr hConsole, WinCon.CONSOLE_SCREEN_BUFFER_INFO_EX sbiex)
        {
			_logger.LogDebug(">>> Entering SetScreenBufferInfo(hConsole={HConsole},sbiex={Sbiex})", hConsole, sbiex);
            NativeMethods.Win32BoolHelper(WinCon.SetConsoleScreenBufferInfoEx(hConsole, ref sbiex), "Set screen buffer info with given data.");
			_logger.LogDebug("<<< Exiting SetScreenBufferInfo(hConsole={HConsole},sbiex={Sbiex})", hConsole, sbiex);
        }


        protected virtual void Dispose(bool disposing)
        {
			_logger.LogDebug(">>> Entering Dispose(disposing={Disposing})", disposing);
            if (!this.isDisposed)
            {
                // ensure we're exited when this is destroyed or disposed of explicitly
                this.ExitCmdProcess();

                this.isDisposed = true;
				_logger.LogDebug("Dispose(disposing={Disposing}): this.isDisposed → {Arg1}", disposing, this.isDisposed);
            }
			_logger.LogDebug("<<< Exiting Dispose(disposing={Disposing})", disposing);
        }

        private void CreateCmdProcess(string path, string link = "")
        {
			_logger.LogDebug(">>> Entering CreateCmdProcess(path={Path},link={Link})", path, link);
            //string AdminPrefix = "Administrator: ";
            string WindowTitleToFind = "Host.Tests.UIA window under test";

            job = WinBase.CreateJobObject(IntPtr.Zero, IntPtr.Zero);
            NativeMethods.Win32NullHelper(job, "Creating job object to hold binaries under test.");
			_logger.LogDebug("CreateCmdProcess(path={Path},link={Link}): job → {Job}", path, link, job);

            Log.Comment("Attempting to launch command-line application at '{0}'", path);

            string binaryToRunPath = path;

#if __INSIDE_WINDOWS
            string launchArgs = binaryToRunPath;
#else
            string openConsolePath = Path.Combine(this.context.TestDeploymentDir, "OpenConsole.exe");

            string launchArgs = $"{openConsolePath}  {binaryToRunPath}";
#endif

            WinBase.STARTUPINFO si = new WinBase.STARTUPINFO();
			_logger.LogDebug("CreateCmdProcess(path={Path},link={Link}): si → {Si}", path, link, si);
            si.cb = Marshal.SizeOf(si);

            // If we were given a LNK file to startup with, set the STARTUPINFO structure to pass that information in to the console host.
            if (!string.IsNullOrEmpty(link))
            {
                si.dwFlags |= WinBase.STARTF.STARTF_TITLEISLINKNAME;
                si.lpTitle = link;
				_logger.LogDebug("CreateCmdProcess(path={Path},link={Link}): si.lpTitle → {Arg2}", path, link, si.lpTitle);
            }

            WinBase.PROCESS_INFORMATION pi = new WinBase.PROCESS_INFORMATION();
			_logger.LogDebug("CreateCmdProcess(path={Path},link={Link}): pi → {Pi}", path, link, pi);

            NativeMethods.Win32BoolHelper(WinBase.CreateProcess(null,
                launchArgs,
                IntPtr.Zero,
                IntPtr.Zero,
                false,
                WinBase.CP_CreationFlags.CREATE_NEW_CONSOLE | WinBase.CP_CreationFlags.CREATE_SUSPENDED,
                IntPtr.Zero,
                null,
                ref si,
                out pi),
                "Attempting to create child host window process.");

            Log.Comment($"Host window PID: {pi.dwProcessId}");

            NativeMethods.Win32BoolHelper(WinBase.AssignProcessToJobObject(job, pi.hProcess), "Assigning new host window (suspended) to job object.");
            NativeMethods.Win32BoolHelper(-1 != WinBase.ResumeThread(pi.hThread), "Resume host window process now that it is attached and its launch of the child application will be caught in the job object.");

            Globals.WaitForTimeout();

            WinBase.JOBOBJECT_BASIC_PROCESS_ID_LIST list = new WinBase.JOBOBJECT_BASIC_PROCESS_ID_LIST();
			_logger.LogDebug("CreateCmdProcess(path={Path},link={Link}): list → {List}", path, link, list);
			_logger.LogDebug("CreateCmdProcess(path={Path},link={Link}): list.NumberOfAssignedProcesses → {Arg2}", path, link, list.NumberOfAssignedProcesses);
            list.NumberOfAssignedProcesses = 2;

            int listptrsize = Marshal.SizeOf(list);
			_logger.LogDebug("CreateCmdProcess(path={Path},link={Link}): listptrsize → {Listptrsize}", path, link, listptrsize);
            IntPtr listptr = Marshal.AllocHGlobal(listptrsize);
			_logger.LogDebug("CreateCmdProcess(path={Path},link={Link}): listptr → {Listptr}", path, link, listptr);
            Marshal.StructureToPtr(list, listptr, false);

            TimeSpan totalWait = TimeSpan.Zero;
            TimeSpan waitLimit = TimeSpan.FromSeconds(30);
			_logger.LogDebug("CreateCmdProcess(path={Path},link={Link}): waitLimit → {WaitLimit}", path, link, waitLimit);
            TimeSpan pollInterval = TimeSpan.FromMilliseconds(500);
			_logger.LogDebug("CreateCmdProcess(path={Path},link={Link}): pollInterval → {PollInterval}", path, link, pollInterval);
            while (totalWait < waitLimit)
            {
                WinBase.QueryInformationJobObject(job, WinBase.JOBOBJECTINFOCLASS.JobObjectBasicProcessIdList, listptr, listptrsize, IntPtr.Zero);
                list = (WinBase.JOBOBJECT_BASIC_PROCESS_ID_LIST)Marshal.PtrToStructure(listptr, typeof(WinBase.JOBOBJECT_BASIC_PROCESS_ID_LIST));

                if (list.NumberOfAssignedProcesses > 1)
                {
                    break;
                }
                else if (list.NumberOfAssignedProcesses < 1)
                {
                    Verify.Fail("Somehow we lost the one console host process in the job already.");
                }

                Thread.Sleep(pollInterval);
                totalWait += pollInterval;
            }
            Verify.IsLessThan(totalWait, waitLimit);

            WinBase.QueryInformationJobObject(job, WinBase.JOBOBJECTINFOCLASS.JobObjectBasicProcessIdList, listptr, listptrsize, IntPtr.Zero);
            list = (WinBase.JOBOBJECT_BASIC_PROCESS_ID_LIST)Marshal.PtrToStructure(listptr, typeof(WinBase.JOBOBJECT_BASIC_PROCESS_ID_LIST));

            Verify.AreEqual(list.NumberOfAssignedProcesses, list.NumberOfProcessIdsInList);

#if __INSIDE_WINDOWS
            pid = pi.dwProcessId;
#else
			_logger.LogDebug("CreateCmdProcess(path={Path},link={Link}): pid → {Pid}", path, link, pid);
            // Take whichever PID isn't the host window's PID as the child.
            pid = pi.dwProcessId == (int)list.ProcessId ? (int)list.ProcessId2 : (int)list.ProcessId;
            Log.Comment($"Child command app PID: {pid}");
#endif

            // Free any attached consoles and attach to the console we just created.
            // The driver will bind our calls to the Console APIs into the child process.
            // This will allow us to use the APIs to get/set the console state of the test window.
            NativeMethods.Win32BoolHelper(WinCon.FreeConsole(), "Free existing console bindings.");
            // need to wait a bit or we might not be able to reliably attach
            System.Threading.Thread.Sleep(Globals.Timeout);
            NativeMethods.Win32BoolHelper(WinCon.AttachConsole((uint)pid), "Bind to the new PID for console APIs.");

            // we need to wait here for a bit or else
            // setting the console window title will fail.
            System.Threading.Thread.Sleep(Globals.Timeout * 5);
            NativeMethods.Win32BoolHelper(WinCon.SetConsoleTitle(WindowTitleToFind), "Set the window title so AppDriver can find it.");

            DesiredCapabilities appCapabilities = new DesiredCapabilities();
			_logger.LogDebug("CreateCmdProcess(path={Path},link={Link}): appCapabilities → {AppCapabilities}", path, link, appCapabilities);
            appCapabilities.SetCapability("app", @"Root");
            Session = new IOSDriver<IOSElement>(new Uri(AppDriverUrl), appCapabilities);

			_logger.LogDebug("CreateCmdProcess(path={Path},link={Link}): Session → {Session}", path, link, Session);
            Verify.IsNotNull(Session);
            Actions = new Actions(Session);
            Verify.IsNotNull(Session);
			_logger.LogDebug("CreateCmdProcess(path={Path},link={Link}): Actions → {Actions}", path, link, Actions);

            Globals.WaitForTimeout();

            // If we are running as admin, the child window title will have a prefix appended as it will also run as admin.
            //if (IsRunningAsAdmin())
            //{
            //    WindowTitleToFind = $"{AdminPrefix}{WindowTitleToFind}";
            //}

            Log.Comment($"Searching for window title '{WindowTitleToFind}'");
            this.UIRoot = Session.FindElementByName(WindowTitleToFind);
            this.hStdOut = WinCon.GetStdHandle(WinCon.CONSOLE_STD_HANDLE.STD_OUTPUT_HANDLE);
			_logger.LogDebug("CreateCmdProcess(path={Path},link={Link}): hStdOut → {HStdOut}", path, link, hStdOut);
            Verify.IsNotNull(this.hStdOut, "Ensure output handle is valid.");
            this.hStdErr = WinCon.GetStdHandle(WinCon.CONSOLE_STD_HANDLE.STD_ERROR_HANDLE);
			_logger.LogDebug("CreateCmdProcess(path={Path},link={Link}): hStdErr → {HStdErr}", path, link, hStdErr);
            Verify.IsNotNull(this.hStdErr, "Ensure error handle is valid.");

            // Set the timeout to 15 seconds after we found the initial window.
            Session.Manage().Timeouts().ImplicitWait = TimeSpan.FromSeconds(15);
			_logger.LogDebug("<<< Exiting CreateCmdProcess(path={Path},link={Link})", path, link);
        }

        private bool IsRunningAsAdmin()
        {
			_logger.LogDebug(">>> Entering IsRunningAsAdmin()");
			_logger.LogDebug("<<< Exiting IsRunningAsAdmin()");
            return new WindowsPrincipal(WindowsIdentity.GetCurrent()).IsInRole(WindowsBuiltInRole.Administrator);
        }

        private void CreateCmdProcess(CreateType type, string pathOverride = "")
        {
			_logger.LogDebug(">>> Entering CreateCmdProcess(type={Type},pathOverride={PathOverride})", type, pathOverride);
            switch (type)
            {
                case CreateType.ProcessOnly:
                    if (!string.IsNullOrEmpty(pathOverride))
                    {
                        this.CreateCmdProcess(pathOverride);
                    }
                    else
                    {
                        this.CreateCmdProcess(binPath);
                    }
                    break;
                case CreateType.ShortcutFile:
                    if (!string.IsNullOrEmpty(pathOverride))
                    {
                        this.CreateCmdProcess(binPath, pathOverride);
                    }
                    else
                    {
                        this.CreateCmdProcess(binPath, linkPath);
                    }
                    break;
                default:
					_logger.LogWarning("Throwing NotImplementedException from CreateCmdProcess(type={Type},pathOverride={PathOverride})", type, pathOverride);
			_logger.LogDebug("<<< Exiting CreateCmdProcess(type={Type},pathOverride={PathOverride})", type, pathOverride);
                    throw new NotImplementedException(AutoHelpers.FormatInvariant("CreateType '{0}' not implemented.", type.ToString()));
            }
        }

        private void ExitCmdProcess()
        {
			_logger.LogDebug(">>> Entering ExitCmdProcess()");
            // Release attachment to the child process console.
            WinCon.FreeConsole();

            this.UIRoot = null;
			_logger.LogDebug("ExitCmdProcess(): this.UIRoot → {Arg0}", this.UIRoot);

            if (this.job != IntPtr.Zero)
            {
                WinBase.TerminateJobObject(this.job, 0);
                this.job = IntPtr.Zero;
				_logger.LogDebug("ExitCmdProcess(): job → {Job}", job);
				_logger.LogDebug("ExitCmdProcess(): this.job → {Arg0}", this.job);
            }
			_logger.LogDebug("<<< Exiting ExitCmdProcess()");
        }

        public WinEventSystem AttachWinEventSystem(IWinEventCallbacks callbacks)
        {
			_logger.LogDebug(">>> Entering AttachWinEventSystem(callbacks={Callbacks})", callbacks);
			_logger.LogDebug("<<< Exiting AttachWinEventSystem(callbacks={Callbacks})", callbacks);
            return new WinEventSystem(callbacks, (uint)this.pid);
        }
    }
}
