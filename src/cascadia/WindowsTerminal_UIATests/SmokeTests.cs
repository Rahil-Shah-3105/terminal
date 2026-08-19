//----------------------------------------------------------------------------------------------------------------------
// <copyright file="SmokeTests.cs" company="Microsoft">
// Copyright (c) Microsoft Corporation.
// Licensed under the MIT license.
// </copyright>
// <summary>UI Automation tests for verifying the basic opening/closing of the Terminal.</summary>
//----------------------------------------------------------------------------------------------------------------------

namespace WindowsTerminal.UIA.Tests
{
    using OpenQA.Selenium;
    using WEX.TestExecution.Markup;

    using WindowsTerminal.UIA.Tests.Common;
    using WindowsTerminal.UIA.Tests.Elements;
	using Microsoft.Extensions.Logging;

    [TestClass]
    public class SmokeTests
    {
		private static readonly Microsoft.Extensions.Logging.ILogger _logger = Microsoft.Extensions.Logging.Abstractions.NullLogger.Instance;
        public TestContext TestContext { get; set; }

        [TestMethod]
        [TestProperty("IsPGO", "true")]
        public void StartTerminal()
        {
			_logger.LogDebug(">>> Entering StartTerminal()");
            using (TerminalApp app = new TerminalApp(TestContext))
            {
                var root = app.GetRoot();
                root.SendKeys("Hello smoke test!");
                Globals.WaitForLongTimeout();
            }
			_logger.LogDebug("<<< Exiting StartTerminal()");
        }

        [TestMethod]
        [TestProperty("IsPGO", "true")]
        public void RunBigTextPowershell()
        {
			_logger.LogDebug(">>> Entering RunBigTextPowershell()");
            using (TerminalApp app = new TerminalApp(TestContext))
            {
                var root = app.GetRoot();
                var contentPath = app.GetFullTestContentPath("big.txt");
                root.SendKeys($"cat \"{contentPath}\"");
                root.SendKeys(Keys.Enter);
                System.Threading.Thread.Sleep(25000);
            }
			_logger.LogDebug("<<< Exiting RunBigTextPowershell()");
        }

        [TestMethod]
        [TestProperty("IsPGO", "true")]
        public void RunBigTextPowershellBulk ()
        {
			_logger.LogDebug(">>> Entering RunBigTextPowershellBulk()");
            using (TerminalApp app = new TerminalApp(TestContext))
            {
                var root = app.GetRoot();
                var contentPath = app.GetFullTestContentPath("big.txt");
                root.SendKeys($"Get-Content -ReadCount 0 \"{contentPath}\" | Out-Default");
                root.SendKeys(Keys.Enter);
                System.Threading.Thread.Sleep(25000);
            }
			_logger.LogDebug("<<< Exiting RunBigTextPowershellBulk()");
        }

        [TestMethod]
        [TestProperty("IsPGO", "true")]
        public void RunBigTextCmd()
        {
			_logger.LogDebug(">>> Entering RunBigTextCmd()");
            using (TerminalApp app = new TerminalApp(TestContext, "cmd.exe"))
            {
                var root = app.GetRoot();
                var contentPath = app.GetFullTestContentPath("big.txt");
                root.SendKeys($"type \"{contentPath}\"");
                root.SendKeys(Keys.Enter);
                System.Threading.Thread.Sleep(25000);
            }
			_logger.LogDebug("<<< Exiting RunBigTextCmd()");
        }

        [TestMethod]
        [TestProperty("IsPGO", "true")]
        public void RunCmatrixCmd()
        {
			_logger.LogDebug(">>> Entering RunCmatrixCmd()");
            using (TerminalApp app = new TerminalApp(TestContext, "cmd.exe"))
            {
                var root = app.GetRoot();
                root.SendKeys("chcp 65001" + Keys.Enter); // This output needs UTF-8
                var contentPath = app.GetFullTestContentPath("cmatrix.txt");
                root.SendKeys($"type \"{contentPath}\"");
                root.SendKeys(Keys.Enter);
                System.Threading.Thread.Sleep(10000);
            }
			_logger.LogDebug("<<< Exiting RunCmatrixCmd()");
        }

        [TestMethod]
        [TestProperty("IsPGO", "true")]
        public void RunCacafireCmd()
        {
			_logger.LogDebug(">>> Entering RunCacafireCmd()");
            using (TerminalApp app = new TerminalApp(TestContext, "cmd.exe"))
            {
                var root = app.GetRoot();
                root.SendKeys("chcp 65001" + Keys.Enter); // This output needs UTF-8
                var contentPath = app.GetFullTestContentPath("cacafire.txt");
                root.SendKeys($"type \"{contentPath}\"");
                root.SendKeys(Keys.Enter);
                System.Threading.Thread.Sleep(25000);
            }
			_logger.LogDebug("<<< Exiting RunCacafireCmd()");
        }

        [TestMethod]
        [TestProperty("IsPGO", "true")]
        public void RunChafaCmd()
        {
			_logger.LogDebug(">>> Entering RunChafaCmd()");
            using (TerminalApp app = new TerminalApp(TestContext, "cmd.exe"))
            {
                var root = app.GetRoot();
                root.SendKeys("chcp 65001" + Keys.Enter); // This output needs UTF-8
                var contentPath = app.GetFullTestContentPath("chafa.txt");
                root.SendKeys($"type \"{contentPath}\"");
                root.SendKeys(Keys.Enter);
                System.Threading.Thread.Sleep(10000);
            }
			_logger.LogDebug("<<< Exiting RunChafaCmd()");
        }

        [TestMethod]
        [TestProperty("IsPGO", "true")]
        public void RunMakeKillPanes()
        {
			_logger.LogDebug(">>> Entering RunMakeKillPanes()");
            using (TerminalApp app = new TerminalApp(TestContext))
            {
                var root = app.GetRoot();

                root.SendKeys(Keys.LeftAlt + Keys.LeftShift + "+");
                Globals.WaitForTimeout();
                root.SendKeys(Keys.LeftAlt + Keys.LeftShift + "+");
                Globals.WaitForTimeout();
                root.SendKeys(Keys.LeftAlt + Keys.LeftShift + "-");
                Globals.WaitForTimeout();
                root.SendKeys(Keys.LeftAlt + Keys.LeftShift + "-");
                Globals.WaitForTimeout();
                root.SendKeys(Keys.LeftControl + Keys.LeftShift + "W");
                Globals.WaitForTimeout();
                root.SendKeys(Keys.LeftControl + Keys.LeftShift + "W");
                Globals.WaitForTimeout();
                root.SendKeys(Keys.LeftControl + Keys.LeftShift + "W");
                Globals.WaitForTimeout();
                root.SendKeys(Keys.LeftControl + Keys.LeftShift + "W");
                Globals.WaitForTimeout();

                Globals.WaitForLongTimeout();
            }
			_logger.LogDebug("<<< Exiting RunMakeKillPanes()");
        }

        [TestMethod]
        [TestProperty("IsPGO", "true")]
        public void RunMakeKillTabs()
        {
			_logger.LogDebug(">>> Entering RunMakeKillTabs()");
            using (TerminalApp app = new TerminalApp(TestContext))
            {
                var root = app.GetRoot();

                root.SendKeys(Keys.LeftControl + Keys.LeftShift + "T");
                Globals.WaitForTimeout();
                root.SendKeys(Keys.LeftControl + Keys.LeftShift + "T");
                Globals.WaitForTimeout();
                root.SendKeys(Keys.LeftControl + Keys.LeftShift + "T");
                Globals.WaitForTimeout();
                root.SendKeys(Keys.LeftControl + Keys.LeftShift + "T");
                Globals.WaitForTimeout();
                root.SendKeys(Keys.LeftControl + Keys.LeftShift + "W");
                Globals.WaitForTimeout();
                root.SendKeys(Keys.LeftControl + Keys.LeftShift + "W");
                Globals.WaitForTimeout();
                root.SendKeys(Keys.LeftControl + Keys.LeftShift + "W");
                Globals.WaitForTimeout();
                root.SendKeys(Keys.LeftControl + Keys.LeftShift + "W");
                Globals.WaitForTimeout();

                Globals.WaitForLongTimeout();
            }
			_logger.LogDebug("<<< Exiting RunMakeKillTabs()");
        }

        [TestMethod]
        [TestProperty("IsPGO", "true")]
        public void RunOpenSettingsUI()
        {
			_logger.LogDebug(">>> Entering RunOpenSettingsUI()");
            using (TerminalApp app = new TerminalApp(TestContext))
            {
                var root = app.GetRoot();

                root.SendKeys(Keys.LeftControl + ",");
                Globals.WaitForTimeout();

                Globals.WaitForLongTimeout();
            }
			_logger.LogDebug("<<< Exiting RunOpenSettingsUI()");
        }
    }
}
