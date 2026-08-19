//----------------------------------------------------------------------------------------------------------------------
// <copyright file="VirtualTerminalTests.cs" company="Microsoft">
// Copyright (c) Microsoft Corporation.
// Licensed under the MIT license.
// </copyright>
// <summary>UI Automation tests for the Virtual Terminal feature.</summary>
//----------------------------------------------------------------------------------------------------------------------
namespace Conhost.UIA.Tests
{
    using System;
    using System.Collections.Generic;
    using System.Diagnostics;
    using System.Drawing;
    using System.IO;
    using System.Linq;
    using System.Runtime.InteropServices;
    using System.Threading;

    using Microsoft.Win32;

    using WEX.Common.Managed;
    using WEX.Logging.Interop;
    using WEX.TestExecution;
    using WEX.TestExecution.Markup;

    using Conhost.UIA.Tests.Common;
    using Conhost.UIA.Tests.Common.NativeMethods;
    using Conhost.UIA.Tests.Elements;
    using OpenQA.Selenium;
	using Microsoft.Extensions.Logging;

    [TestClass]
    public class VirtualTerminalTests
    {
		private static readonly Microsoft.Extensions.Logging.ILogger _logger = Microsoft.Extensions.Logging.Abstractions.NullLogger.Instance;
        private static string VIRTUAL_TERMINAL_KEY_NAME = "VirtualTerminalLevel";
        private static int VIRTUAL_TERMINAL_ON_VALUE = 100;

        private static string vtAppLocation;

        public const int timeout = Globals.Timeout;

        public TestContext TestContext { get; set; }

        [ClassInitialize]
        public static void ClassSetup(TestContext context)
        {
			_logger.LogDebug(">>> Entering ClassSetup(context={Context})", context);
            Log.Comment("Searching for VtApp.exe in the same directory where this test was launched from...");
            vtAppLocation = Path.Combine(context.TestDeploymentDir, "VtApp.exe");
            Verify.IsFalse(string.IsNullOrEmpty(vtAppLocation));
			_logger.LogDebug("ClassSetup(context={Context}): vtAppLocation → {VtAppLocation}", context, vtAppLocation);
            Verify.IsTrue(File.Exists(vtAppLocation));
			_logger.LogDebug("<<< Exiting ClassSetup(context={Context})", context);
        }

        [TestMethod]
        public void RunVtAppTester()
        {
			_logger.LogDebug(">>> Entering RunVtAppTester()");
            using (RegistryHelper reg = new RegistryHelper())
            {
                reg.BackupRegistry(); // we're going to modify the virtual terminal state for this, so back it up first.
                VersionSelector.SetConsoleVersion(reg, ConsoleVersion.V2);
                reg.SetDefaultValue(VIRTUAL_TERMINAL_KEY_NAME, VIRTUAL_TERMINAL_ON_VALUE);

                bool haveVtAppPath = !string.IsNullOrEmpty(vtAppLocation);

                Verify.IsTrue(haveVtAppPath, "Ensure that we passed in the location to VtApp.exe");

                if (haveVtAppPath)
                {
                    using (CmdApp app = new CmdApp(CreateType.ProcessOnly, TestContext, vtAppLocation))
                    {
                        using (ViewportArea area = new ViewportArea(app))
                        {
                            // Get console handle.
                            IntPtr hConsole = app.GetStdOutHandle();
                            Verify.IsNotNull(hConsole, "Ensure the STDOUT handle is valid.");

                            Log.Comment("Check that the VT test app loaded up properly with its output line and the cursor down one line.");
                            Rectangle selectRect = new Rectangle(0, 0, 9, 1);
                            IEnumerable<string> scrapedText = area.GetLinesInRectangle(hConsole, selectRect);

                            Verify.AreEqual(scrapedText.Count(), 1, "We should have retrieved one line.");
                            string testerWelcome = scrapedText.Single();

                            Verify.AreEqual(testerWelcome, "VT Tester");

                            WinCon.COORD cursorPos = app.GetCursorPosition(hConsole);
                            WinCon.COORD cursorExpected = new WinCon.COORD();
                            cursorExpected.X = 0;
                            cursorExpected.Y = 1;
                            Verify.AreEqual(cursorExpected, cursorPos, "Check cursor has moved to expected starting position.");

                            TestCursorPositioningCommands(app, hConsole, cursorExpected);

                            TestCursorVisibilityCommands(app, hConsole);

                            TestAreaEraseCommands(app, area, hConsole);

                            TestGraphicsCommands(app, area, hConsole);

                            TestQueryResponses(app, hConsole);

                            TestVtToggle(app, hConsole);

                            TestInsertDelete(app, area, hConsole);
                        }
                    }
                }
            }
			_logger.LogDebug("<<< Exiting RunVtAppTester()");
        }

        private static void TestInsertDelete(CmdApp app, ViewportArea area, IntPtr hConsole)
        {
			_logger.LogDebug(">>> Entering TestInsertDelete(app={App},area={Area},hConsole={HConsole})", app, area, hConsole);
            Log.Comment("--Insert/Delete Commands");
            ScreenFillHelper(app, area, hConsole);

            Log.Comment("Move cursor to the middle-ish");
            Point cursorExpected = new Point();
			_logger.LogDebug("TestInsertDelete(app={App},area={Area},hConsole={HConsole}): cursorExpected → {CursorExpected}", app, area, hConsole, cursorExpected);
            // H is at 5, 1. VT coords are 1-based and buffer is 0-based so adjust.
            cursorExpected.Y = 5 - 1;
			_logger.LogDebug("TestInsertDelete(app={App},area={Area},hConsole={HConsole}): cursorExpected.Y → {Arg3}", app, area, hConsole, cursorExpected.Y);
			_logger.LogDebug("TestInsertDelete(app={App},area={Area},hConsole={HConsole}): cursorExpected.X → {Arg3}", app, area, hConsole, cursorExpected.X);
            cursorExpected.X = 1 - 1;
            app.UIRoot.SendKeys("H");

            // Move to middle-ish from here. 10 Bs and 10 Cs should about do it.
            for (int i = 0; i < 10; i++)
            {
                app.UIRoot.SendKeys("BC");
                cursorExpected.Y++;
                cursorExpected.X++;
            }

            WinCon.SMALL_RECT viewport = app.GetViewport(hConsole);
			_logger.LogDebug("TestInsertDelete(app={App},area={Area},hConsole={HConsole}): viewport → {Viewport}", app, area, hConsole, viewport);

            // The entire buffer should be Zs except for what we're about to insert and delete.
            app.UIRoot.SendKeys("O"); // insert
            WinCon.CHAR_INFO ciCursor = area.GetCharInfoAt(hConsole, cursorExpected);
			_logger.LogDebug("TestInsertDelete(app={App},area={Area},hConsole={HConsole}): ciCursor → {CiCursor}", app, area, hConsole, ciCursor);
            Verify.AreEqual(' ', ciCursor.UnicodeChar);

            Point endOfCursorLine = new Point(viewport.Right, cursorExpected.Y);
			_logger.LogDebug("TestInsertDelete(app={App},area={Area},hConsole={HConsole}): endOfCursorLine → {EndOfCursorLine}", app, area, hConsole, endOfCursorLine);

            app.UIRoot.SendKeys("P"); // delete
            WinCon.CHAR_INFO ciEndOfLine = area.GetCharInfoAt(hConsole, endOfCursorLine);
			_logger.LogDebug("TestInsertDelete(app={App},area={Area},hConsole={HConsole}): ciEndOfLine → {CiEndOfLine}", app, area, hConsole, ciEndOfLine);
            Verify.AreEqual(' ', ciEndOfLine.UnicodeChar);
            ciCursor = area.GetCharInfoAt(hConsole, cursorExpected);
            Verify.AreEqual('Z', ciCursor.UnicodeChar);

            // Move to end of line and check both insert and delete operations
            while (cursorExpected.X < viewport.Right)
            {
                app.UIRoot.SendKeys("C");
                cursorExpected.X++;
            }

            // move up a line to get some fresh Z
            app.UIRoot.SendKeys("A");
            cursorExpected.Y--;

            app.UIRoot.SendKeys("O"); // insert at end of line
            ciCursor = area.GetCharInfoAt(hConsole, cursorExpected);
            Verify.AreEqual(' ', ciCursor.UnicodeChar);

            // move up a line to get some fresh Z
            app.UIRoot.SendKeys("A");
            cursorExpected.Y--;

            app.UIRoot.SendKeys("P"); // delete at end of line
            ciCursor = area.GetCharInfoAt(hConsole, cursorExpected);
            Verify.AreEqual(' ', ciCursor.UnicodeChar);
			_logger.LogDebug("<<< Exiting TestInsertDelete(app={App},area={Area},hConsole={HConsole})", app, area, hConsole);
        }

        private static void TestVtToggle(CmdApp app, IntPtr hConsole)
        {
			_logger.LogDebug(">>> Entering TestVtToggle(app={App},hConsole={HConsole})", app, hConsole);
            WinCon.COORD cursorPos;
            Log.Comment("--Test VT Toggle--");

            Verify.IsTrue(app.IsVirtualTerminalEnabled(hConsole), "Verify we're starting with VT on.");

            app.UIRoot.SendKeys("H-"); // move cursor to top left area H location and then turn off VT.

            cursorPos = app.GetCursorPosition(hConsole);

			_logger.LogDebug("TestVtToggle(app={App},hConsole={HConsole}): cursorPos → {CursorPos}", app, hConsole, cursorPos);
            Verify.IsFalse(app.IsVirtualTerminalEnabled(hConsole), "Verify VT was turned off.");

            app.UIRoot.SendKeys("-");
            Verify.IsTrue(app.IsVirtualTerminalEnabled(hConsole), "Verify VT was turned back on .");
			_logger.LogDebug("<<< Exiting TestVtToggle(app={App},hConsole={HConsole})", app, hConsole);
        }

        private static void TestQueryResponses(CmdApp app, IntPtr hConsole)
        {
			_logger.LogDebug(">>> Entering TestQueryResponses(app={App},hConsole={HConsole})", app, hConsole);
            WinCon.COORD cursorPos;
            Log.Comment("---Status Request Commands---");
            Globals.WaitForTimeout();
            app.UIRoot.SendKeys("c");
            string expectedTitle = string.Format("Response Received: {0}", "\x1b[?61;4;6;7;14;21;22;23;24;28;32;42;52c");
			_logger.LogDebug("TestQueryResponses(app={App},hConsole={HConsole}): expectedTitle → {ExpectedTitle}", app, hConsole, expectedTitle);

            Globals.WaitForTimeout();
            string title = app.GetWindowTitle();
			_logger.LogDebug("TestQueryResponses(app={App},hConsole={HConsole}): title → {Title}", app, hConsole, title);
            Verify.AreEqual(expectedTitle, title, "Verify that we received the proper response to the Device Attributes request.");

            app.UIRoot.SendKeys("R");
            cursorPos = app.GetCursorPosition(hConsole);
            expectedTitle = string.Format("Response Received: {0}", string.Format("\x1b[{0};{1}R", cursorPos.Y + 1, cursorPos.X + 1));

			_logger.LogDebug("TestQueryResponses(app={App},hConsole={HConsole}): cursorPos → {CursorPos}", app, hConsole, cursorPos);
            Globals.WaitForTimeout();
            title = app.GetWindowTitle();
            Verify.AreEqual(expectedTitle, title, "Verify that we received the proper response to the Cursor Position request.");
			_logger.LogDebug("<<< Exiting TestQueryResponses(app={App},hConsole={HConsole})", app, hConsole);
        }

        private static void TestGraphicsCommands(CmdApp app, ViewportArea area, IntPtr hConsole)
        {
			_logger.LogDebug(">>> Entering TestGraphicsCommands(app={App},area={Area},hConsole={HConsole})", app, area, hConsole);
            Log.Comment("---Graphics Commands---");
            ScreenFillHelper(app, area, hConsole);

            WinCon.CHAR_INFO ciExpected = new WinCon.CHAR_INFO();
			_logger.LogDebug("TestGraphicsCommands(app={App},area={Area},hConsole={HConsole}): ciExpected → {CiExpected}", app, area, hConsole, ciExpected);
			_logger.LogDebug("TestGraphicsCommands(app={App},area={Area},hConsole={HConsole}): ciExpected.UnicodeChar → {Arg3}", app, area, hConsole, ciExpected.UnicodeChar);
            ciExpected.UnicodeChar = 'z';
            ciExpected.Attributes = app.GetCurrentAttributes(hConsole);

            WinCon.CHAR_INFO ciOriginal = ciExpected;

            WinCon.CHAR_INFO ciActual;
            Point pt = new Point();
			_logger.LogDebug("TestGraphicsCommands(app={App},area={Area},hConsole={HConsole}): pt → {Pt}", app, area, hConsole, pt);

            Log.Comment("Set foreground brightness (SGR.1)");
            app.FillCursorPosition(hConsole, ref pt);
            app.UIRoot.SendKeys("1`");

            ciExpected.Attributes |= WinCon.CONSOLE_ATTRIBUTES.FOREGROUND_INTENSITY;

            ciActual = area.GetCharInfoAt(hConsole, pt);
            Verify.AreEqual(ciExpected, ciActual, "Verify that foreground brightness got set.");

            Log.Comment("Set foreground green (SGR.32)");
            app.FillCursorPosition(hConsole, ref pt);
            app.UIRoot.SendKeys("2`");
			_logger.LogDebug("TestGraphicsCommands(app={App},area={Area},hConsole={HConsole}): ciActual → {CiActual}", app, area, hConsole, ciActual);

            ciExpected.Attributes &= ~WinCon.CONSOLE_ATTRIBUTES.FOREGROUND_ALL;
            ciExpected.Attributes |= WinCon.CONSOLE_ATTRIBUTES.FOREGROUND_GREEN;
            ciExpected.Attributes |= WinCon.CONSOLE_ATTRIBUTES.FOREGROUND_INTENSITY;

            ciActual = area.GetCharInfoAt(hConsole, pt);
            Verify.AreEqual(ciExpected, ciActual, "Verify that foreground green got set.");

            Log.Comment("Set foreground yellow (SGR.33)");
            app.FillCursorPosition(hConsole, ref pt);
            app.UIRoot.SendKeys("3`");

            ciExpected.Attributes &= ~WinCon.CONSOLE_ATTRIBUTES.FOREGROUND_ALL;
            ciExpected.Attributes |= WinCon.CONSOLE_ATTRIBUTES.FOREGROUND_YELLOW;
            ciExpected.Attributes |= WinCon.CONSOLE_ATTRIBUTES.FOREGROUND_INTENSITY;

            ciActual = area.GetCharInfoAt(hConsole, pt);
            Verify.AreEqual(ciExpected, ciActual, "Verify that foreground yellow got set.");

            Log.Comment("Set foreground blue (SGR.34)");
            app.FillCursorPosition(hConsole, ref pt);
            app.UIRoot.SendKeys("4`");

            ciExpected.Attributes &= ~WinCon.CONSOLE_ATTRIBUTES.FOREGROUND_ALL;
            ciExpected.Attributes |= WinCon.CONSOLE_ATTRIBUTES.FOREGROUND_BLUE;
            ciExpected.Attributes |= WinCon.CONSOLE_ATTRIBUTES.FOREGROUND_INTENSITY;

            ciActual = area.GetCharInfoAt(hConsole, pt);
            Verify.AreEqual(ciExpected, ciActual, "Verify that foreground blue got set.");

            Log.Comment("Set foreground magenta (SGR.35)");
            app.FillCursorPosition(hConsole, ref pt);
            app.UIRoot.SendKeys("5`");

            ciExpected.Attributes &= ~WinCon.CONSOLE_ATTRIBUTES.FOREGROUND_ALL;
            ciExpected.Attributes |= WinCon.CONSOLE_ATTRIBUTES.FOREGROUND_MAGENTA;
            ciExpected.Attributes |= WinCon.CONSOLE_ATTRIBUTES.FOREGROUND_INTENSITY;

            ciActual = area.GetCharInfoAt(hConsole, pt);
            Verify.AreEqual(ciExpected, ciActual, "Verify that foreground magenta got set.");

            Log.Comment("Set foreground cyan (SGR.36)");
            app.FillCursorPosition(hConsole, ref pt);
            app.UIRoot.SendKeys("6`");

            ciExpected.Attributes &= ~WinCon.CONSOLE_ATTRIBUTES.FOREGROUND_ALL;
            ciExpected.Attributes |= WinCon.CONSOLE_ATTRIBUTES.FOREGROUND_CYAN;
            ciExpected.Attributes |= WinCon.CONSOLE_ATTRIBUTES.FOREGROUND_INTENSITY;

            ciActual = area.GetCharInfoAt(hConsole, pt);
            Verify.AreEqual(ciExpected, ciActual, "Verify that foreground cyan got set.");

            Log.Comment("Set background white (SGR.47)");
            app.FillCursorPosition(hConsole, ref pt);
            app.UIRoot.SendKeys("W`");

            ciExpected.Attributes &= ~WinCon.CONSOLE_ATTRIBUTES.BACKGROUND_ALL;
            ciExpected.Attributes |= WinCon.CONSOLE_ATTRIBUTES.BACKGROUND_COLORS;
            ciExpected.Attributes |= WinCon.CONSOLE_ATTRIBUTES.FOREGROUND_INTENSITY;

            ciActual = area.GetCharInfoAt(hConsole, pt);
            Verify.AreEqual(ciExpected, ciActual, "Verify that background white got set.");

            Log.Comment("Set background black (SGR.40)");
            app.FillCursorPosition(hConsole, ref pt);
            app.UIRoot.SendKeys("Q`");

            ciExpected.Attributes &= ~WinCon.CONSOLE_ATTRIBUTES.BACKGROUND_ALL;

            ciActual = area.GetCharInfoAt(hConsole, pt);
            Verify.AreEqual(ciExpected, ciActual, "Verify that background black got set.");

            Log.Comment("Set background red (SGR.41)");
            app.FillCursorPosition(hConsole, ref pt);
            app.UIRoot.SendKeys("q`");

            ciExpected.Attributes &= ~WinCon.CONSOLE_ATTRIBUTES.BACKGROUND_ALL;
            ciExpected.Attributes |= WinCon.CONSOLE_ATTRIBUTES.BACKGROUND_RED;

            ciActual = area.GetCharInfoAt(hConsole, pt);
            Verify.AreEqual(ciExpected, ciActual, "Verify that background red got set.");

            Log.Comment("Set background yellow (SGR.43)");
            app.FillCursorPosition(hConsole, ref pt);
            app.UIRoot.SendKeys("w`");

            ciExpected.Attributes &= ~WinCon.CONSOLE_ATTRIBUTES.BACKGROUND_ALL;
            ciExpected.Attributes |= WinCon.CONSOLE_ATTRIBUTES.BACKGROUND_YELLOW;

            ciActual = area.GetCharInfoAt(hConsole, pt);
            Verify.AreEqual(ciExpected, ciActual, "Verify that background yellow got set.");

            Log.Comment("Set foreground bright red (SGR.91)");
            app.FillCursorPosition(hConsole, ref pt);
            app.UIRoot.SendKeys("!`");

            ciExpected.Attributes &= ~WinCon.CONSOLE_ATTRIBUTES.FOREGROUND_ALL;
            ciExpected.Attributes |= WinCon.CONSOLE_ATTRIBUTES.FOREGROUND_RED | WinCon.CONSOLE_ATTRIBUTES.FOREGROUND_INTENSITY;

            ciActual = area.GetCharInfoAt(hConsole, pt);
            Verify.AreEqual(ciExpected, ciActual, "Verify that foreground bright red got set.");

            Log.Comment("Set foreground bright blue (SGR.94)");
            app.FillCursorPosition(hConsole, ref pt);
            app.UIRoot.SendKeys("@`");

            ciExpected.Attributes &= ~WinCon.CONSOLE_ATTRIBUTES.FOREGROUND_ALL;
            ciExpected.Attributes |= WinCon.CONSOLE_ATTRIBUTES.FOREGROUND_BLUE | WinCon.CONSOLE_ATTRIBUTES.FOREGROUND_INTENSITY;

            ciActual = area.GetCharInfoAt(hConsole, pt);
            Verify.AreEqual(ciExpected, ciActual, "Verify that foreground bright blue got set.");

            Log.Comment("Set foreground bright cyan (SGR.96)");
            app.FillCursorPosition(hConsole, ref pt);
            app.UIRoot.SendKeys("#`");

            ciExpected.Attributes &= ~WinCon.CONSOLE_ATTRIBUTES.FOREGROUND_ALL;
            ciExpected.Attributes |= WinCon.CONSOLE_ATTRIBUTES.FOREGROUND_CYAN | WinCon.CONSOLE_ATTRIBUTES.FOREGROUND_INTENSITY;

            ciActual = area.GetCharInfoAt(hConsole, pt);
            Verify.AreEqual(ciExpected, ciActual, "Verify that foreground bright cyan got set.");

            Log.Comment("Set background bright red (SGR.101)");
            app.FillCursorPosition(hConsole, ref pt);
            app.UIRoot.SendKeys("$`");

            ciExpected.Attributes &= ~WinCon.CONSOLE_ATTRIBUTES.BACKGROUND_ALL;
            ciExpected.Attributes |= WinCon.CONSOLE_ATTRIBUTES.BACKGROUND_RED | WinCon.CONSOLE_ATTRIBUTES.BACKGROUND_INTENSITY;

            ciActual = area.GetCharInfoAt(hConsole, pt);
            Verify.AreEqual(ciExpected, ciActual, "Verify that background bright red got set.");

            Log.Comment("Set background bright blue (SGR.104)");
            app.FillCursorPosition(hConsole, ref pt);
            app.UIRoot.SendKeys(Keys.Shift + "5" + Keys.Shift + "`");

            ciExpected.Attributes &= ~WinCon.CONSOLE_ATTRIBUTES.BACKGROUND_ALL;
            ciExpected.Attributes |= WinCon.CONSOLE_ATTRIBUTES.BACKGROUND_BLUE | WinCon.CONSOLE_ATTRIBUTES.BACKGROUND_INTENSITY;

            ciActual = area.GetCharInfoAt(hConsole, pt);
            Verify.AreEqual(ciExpected, ciActual, "Verify that background bright blue got set.");

            Log.Comment("Set background bright cyan (SGR.106)");
            app.FillCursorPosition(hConsole, ref pt);
            app.UIRoot.SendKeys(Keys.Shift + "6" + Keys.Shift + "`");

            ciExpected.Attributes &= ~WinCon.CONSOLE_ATTRIBUTES.BACKGROUND_ALL;
            ciExpected.Attributes |= WinCon.CONSOLE_ATTRIBUTES.BACKGROUND_CYAN | WinCon.CONSOLE_ATTRIBUTES.BACKGROUND_INTENSITY;

            ciActual = area.GetCharInfoAt(hConsole, pt);
            Verify.AreEqual(ciExpected, ciActual, "Verify that background bright cyan  got set.");

            Log.Comment("Set overline (SGR.53)");
            app.FillCursorPosition(hConsole, ref pt);
            app.UIRoot.SendKeys("e`");

            ciExpected.Attributes |= WinCon.CONSOLE_ATTRIBUTES.COMMON_LVB_GRID_HORIZONTAL;

            ciActual = area.GetCharInfoAt(hConsole, pt);
            Verify.AreEqual(ciExpected, ciActual, "Verify that overline got set.");

            Log.Comment("Clear overline (SGR.55)");
            app.FillCursorPosition(hConsole, ref pt);
            app.UIRoot.SendKeys("d`");

            ciExpected.Attributes &= ~WinCon.CONSOLE_ATTRIBUTES.COMMON_LVB_GRID_HORIZONTAL;

            ciActual = area.GetCharInfoAt(hConsole, pt);
            Verify.AreEqual(ciExpected, ciActual, "Verify that overline got cleared.");

            Log.Comment("Set negative image video (SGR.7)");
            app.FillCursorPosition(hConsole, ref pt);
            app.UIRoot.SendKeys("r`");

            ciExpected.Attributes |= WinCon.CONSOLE_ATTRIBUTES.COMMON_LVB_REVERSE_VIDEO;

            ciActual = area.GetCharInfoAt(hConsole, pt);
            Verify.AreEqual(ciExpected, ciActual, "Verify that negative video got set.");

            Log.Comment("Set positive image video (SGR.27)");
            app.FillCursorPosition(hConsole, ref pt);
            app.UIRoot.SendKeys("f`");

            ciExpected.Attributes &= ~WinCon.CONSOLE_ATTRIBUTES.COMMON_LVB_REVERSE_VIDEO;

            ciActual = area.GetCharInfoAt(hConsole, pt);
            Verify.AreEqual(ciExpected, ciActual, "Verify that positive video got set.");

            Log.Comment("Set back to default (SGR.0)");
            app.FillCursorPosition(hConsole, ref pt);
            app.UIRoot.SendKeys("0`");

            ciExpected = ciOriginal;

            ciActual = area.GetCharInfoAt(hConsole, pt);
            Verify.AreEqual(ciExpected, ciActual, "Verify that we got set back to the original state.");

            Log.Comment("Set multiple properties in the same message (SGR.1,37,43,53)");
            app.FillCursorPosition(hConsole, ref pt);
            app.UIRoot.SendKeys("9`");

            ciExpected.Attributes |= WinCon.CONSOLE_ATTRIBUTES.FOREGROUND_COLORS;
            ciExpected.Attributes |= WinCon.CONSOLE_ATTRIBUTES.FOREGROUND_INTENSITY;
            ciExpected.Attributes |= WinCon.CONSOLE_ATTRIBUTES.BACKGROUND_YELLOW;
            ciExpected.Attributes |= WinCon.CONSOLE_ATTRIBUTES.COMMON_LVB_GRID_HORIZONTAL;

            ciActual = area.GetCharInfoAt(hConsole, pt);
            Verify.AreEqual(ciExpected, ciActual, "Verify that we set foreground bright white, background yellow, and underscore in the same SGR command.");

            Log.Comment("Set foreground back to original only (SGR.39)");
            app.FillCursorPosition(hConsole, ref pt);
            app.UIRoot.SendKeys(Keys.Shift + "9" + Keys.Shift + "`");

            ciExpected.Attributes &= ~WinCon.CONSOLE_ATTRIBUTES.FOREGROUND_ALL; // turn off all foreground flags
            ciExpected.Attributes |= (ciOriginal.Attributes & WinCon.CONSOLE_ATTRIBUTES.FOREGROUND_ALL); // turn on only the foreground part of the original attributes
            ciExpected.Attributes |= WinCon.CONSOLE_ATTRIBUTES.FOREGROUND_INTENSITY;

            ciActual = area.GetCharInfoAt(hConsole, pt);
            Verify.AreEqual(ciExpected, ciActual, "Verify that we set the foreground only back to the default.");

            Log.Comment("Set background back to original only (SGR.49)");
            app.FillCursorPosition(hConsole, ref pt);
            app.UIRoot.SendKeys(Keys.Shift + "0" + Keys.Shift + "`");

            ciExpected.Attributes &= ~WinCon.CONSOLE_ATTRIBUTES.BACKGROUND_ALL; // turn off all foreground flags
            ciExpected.Attributes |= (ciOriginal.Attributes & WinCon.CONSOLE_ATTRIBUTES.BACKGROUND_ALL); // turn on only the foreground part of the original attributes

            ciActual = area.GetCharInfoAt(hConsole, pt);
            Verify.AreEqual(ciExpected, ciActual, "Verify that we set the background only back to the default.");
			_logger.LogDebug("<<< Exiting TestGraphicsCommands(app={App},area={Area},hConsole={HConsole})", app, area, hConsole);
        }

        private static void TestCursorPositioningCommands(CmdApp app, IntPtr hConsole, WinCon.COORD cursorExpected)
        {
			_logger.LogDebug(">>> Entering TestCursorPositioningCommands(app={App},hConsole={HConsole},cursorExpected={CursorExpected})", app, hConsole, cursorExpected);
            WinCon.COORD cursorPos;
            Log.Comment("---Cursor Positioning Commands---");
            // Try cursor commands
            Log.Comment("Press B key (cursor down)");
            app.UIRoot.SendKeys("B");
            cursorExpected.Y++;
            cursorPos = app.GetCursorPosition(hConsole);
            Verify.AreEqual(cursorExpected, cursorPos, "Check cursor has moved down by 1.");
			_logger.LogDebug("TestCursorPositioningCommands(app={App},hConsole={HConsole},cursorExpected={CursorExpected}): cursorPos → {CursorPos}", app, hConsole, cursorExpected, cursorPos);

            Log.Comment("Press A key (cursor up)");
            app.UIRoot.SendKeys("A");
            cursorExpected.Y--;
            cursorPos = app.GetCursorPosition(hConsole);
            Verify.AreEqual(cursorExpected, cursorPos, "Check cursor has moved up by 1.");

            Log.Comment("Press C key (cursor right)");
            app.UIRoot.SendKeys("C");
            cursorExpected.X++;
            cursorPos = app.GetCursorPosition(hConsole);
            Verify.AreEqual(cursorExpected, cursorPos, "Check cursor has moved right by 1.");

            Log.Comment("Press D key (cursor left)");
            app.UIRoot.SendKeys("D");
            cursorExpected.X--;
            cursorPos = app.GetCursorPosition(hConsole);
            Verify.AreEqual(cursorExpected, cursorPos, "Check cursor has moved left by 1.");

            Log.Comment("Move to the right (C) then move down a line (E)");
            app.UIRoot.SendKeys("CCC");
            cursorExpected.X += 3;
            cursorPos = app.GetCursorPosition(hConsole);
            Verify.AreEqual(cursorExpected, cursorPos, "Check cursor has right by 3.");

            app.UIRoot.SendKeys("E");
            cursorExpected.Y++;
            cursorExpected.X = 0;
			_logger.LogDebug("TestCursorPositioningCommands(app={App},hConsole={HConsole},cursorExpected={CursorExpected}): cursorExpected.X → {Arg3}", app, hConsole, cursorExpected, cursorExpected.X);
            cursorPos = app.GetCursorPosition(hConsole);
            Verify.AreEqual(cursorExpected, cursorPos, "Check cursor has moved down by 1 line and reset X position to 0.");

            Log.Comment("Move to the right (C) then move up a line (F)");
            app.UIRoot.SendKeys("CCC");
            cursorExpected.X += 3;
            cursorPos = app.GetCursorPosition(hConsole);
            Verify.AreEqual(cursorExpected, cursorPos, "Check curs has right by 3.");

            app.UIRoot.SendKeys("F");
            cursorExpected.Y--;
            cursorExpected.X = 0;
            cursorPos = app.GetCursorPosition(hConsole);
            Verify.AreEqual(cursorExpected, cursorPos, "Check cursor has moved up by 1 line.");

            Log.Comment("Check move directly to position 14 horizontally (G)");
            app.UIRoot.SendKeys("G");
            cursorExpected.X = 14 - 1; // 14 is the VT position which starts at array offset 1. 13 is the buffer position starting at array offset 0.
            cursorPos = app.GetCursorPosition(hConsole);
            Verify.AreEqual(cursorExpected, cursorPos, "Check cursor has moved to horizontal position 14.");

            Log.Comment("Check move directly to position 14 vertically (v key mapped to d)");
            app.UIRoot.SendKeys("v");
            cursorExpected.Y = 14 - 1; // 14 is the VT position which starts at array offset 1. 13 is the buffer position starting at array offset 0.
			_logger.LogDebug("TestCursorPositioningCommands(app={App},hConsole={HConsole},cursorExpected={CursorExpected}): cursorExpected.Y → {Arg3}", app, hConsole, cursorExpected, cursorExpected.Y);
            cursorPos = app.GetCursorPosition(hConsole);
            Verify.AreEqual(cursorExpected, cursorPos, "Check cursor has moved to vertical position 14.");

            Log.Comment("Check move directly to row 5, column 1 (H)");
            app.UIRoot.SendKeys("H");
            // Again -1s are to convert index base 1 VT to console base 0 arrays
            cursorExpected.Y = 5 - 1;
            cursorExpected.X = 1 - 1;
            cursorPos = app.GetCursorPosition(hConsole);
            Verify.AreEqual(cursorExpected, cursorPos, "Check cursor has moved to row 5, column 1.");
			_logger.LogDebug("<<< Exiting TestCursorPositioningCommands(app={App},hConsole={HConsole},cursorExpected={CursorExpected})", app, hConsole, cursorExpected);
        }

        private static void TestCursorVisibilityCommands(CmdApp app, IntPtr hConsole)
        {
			_logger.LogDebug(">>> Entering TestCursorVisibilityCommands(app={App},hConsole={HConsole})", app, hConsole);
            WinCon.COORD cursorExpected;
            Log.Comment("---Cursor Visibility Commands---");
            Log.Comment("Turn cursor display off. (l)");
            app.UIRoot.SendKeys("l");
            Verify.AreEqual(false, app.IsCursorVisible(hConsole), "Check that cursor is invisible.");

            Log.Comment("Turn cursor display on. (h)");
            app.UIRoot.SendKeys("h");
            Verify.AreEqual(true, app.IsCursorVisible(hConsole), "Check that cursor is visible.");

            Log.Comment("---Cursor Save/Restore Commands---");
            Log.Comment("Save current cursor position with DEC save.");
            app.UIRoot.SendKeys("7");
            cursorExpected = app.GetCursorPosition(hConsole);

			_logger.LogDebug("TestCursorVisibilityCommands(app={App},hConsole={HConsole}): cursorExpected → {CursorExpected}", app, hConsole, cursorExpected);
            Log.Comment("Move the cursor a bit away from the saved position.");
            app.UIRoot.SendKeys("BBBBCCCCC");

            Log.Comment("Restore existing position with DEC restore.");
            app.UIRoot.SendKeys("8");
            Verify.AreEqual(cursorExpected, app.GetCursorPosition(hConsole), "Check that cursor restored back to the saved position.");

            Log.Comment("Move the cursor a bit away from the saved position.");
            app.UIRoot.SendKeys("BBBBCCCCC");

            Log.Comment("Restore existing position with ANSISYS restore.");
            app.UIRoot.SendKeys("u");
            Verify.AreEqual(cursorExpected, app.GetCursorPosition(hConsole), "Check that cursor restored back to the saved position.");

            Log.Comment("Move the cursor a bit away from either position.");
            app.UIRoot.SendKeys("BBB");

            Log.Comment("Save current cursor position with ANSISYS save.");
            app.UIRoot.SendKeys("y");
            cursorExpected = app.GetCursorPosition(hConsole);

            Log.Comment("Move the cursor a bit away from the saved position.");
            app.UIRoot.SendKeys("CCCBB");

            Log.Comment("Restore existing position with DEC restore.");
            app.UIRoot.SendKeys("8");
            Verify.AreEqual(cursorExpected, app.GetCursorPosition(hConsole), "Check that cursor restored back to the saved position.");
			_logger.LogDebug("<<< Exiting TestCursorVisibilityCommands(app={App},hConsole={HConsole})", app, hConsole);
        }

        private static void TestAreaEraseCommands(CmdApp app, ViewportArea area, IntPtr hConsole)
        {
			_logger.LogDebug(">>> Entering TestAreaEraseCommands(app={App},area={Area},hConsole={HConsole})", app, area, hConsole);
            WinCon.COORD cursorPos;
            Log.Comment("---Area Erase Commands---");
            ScreenFillHelper(app, area, hConsole);
            Log.Comment("Clear screen after");
            app.UIRoot.SendKeys("J");

            Globals.WaitForTimeout(); // give buffer time to clear.

            cursorPos = app.GetCursorPosition(hConsole);

			_logger.LogDebug("TestAreaEraseCommands(app={App},area={Area},hConsole={HConsole}): cursorPos → {CursorPos}", app, area, hConsole, cursorPos);
            GetExpectedChar expectedCharAlgorithm;
            expectedCharAlgorithm = (int rowId, int colId, int height, int width) =>
            {
			_logger.LogDebug("TestAreaEraseCommands(app={App},area={Area},hConsole={HConsole}): expectedCharAlgorithm → {ExpectedCharAlgorithm}", app, area, hConsole, expectedCharAlgorithm);
                if (rowId == (height - 1) && colId == (width - 1))
                {
                    return ' ';
                }
                else if (rowId < cursorPos.Y)
                {
                    return 'Z';
                }
                else if (rowId > cursorPos.Y)
                {
                    return ' ';
                }
                else
                {
                    if (colId < cursorPos.X)
                    {
                        return 'Z';
                    }
                    else
                    {
                        return ' ';
                    }
                }
            };

            BufferVerificationHelper(app, area, hConsole, expectedCharAlgorithm);

            ScreenFillHelper(app, area, hConsole);
            Log.Comment("Clear screen before");
            app.UIRoot.SendKeys("j");

            expectedCharAlgorithm = (int rowId, int colId, int height, int width) =>
            {
                if (rowId == (height - 1) && colId == (width - 1))
                {
                    return ' ';
                }
                else if (rowId < cursorPos.Y)
                {
                    return ' ';
                }
                else if (rowId > cursorPos.Y)
                {
                    return 'Z';
                }
                else
                {
                    if (colId <= cursorPos.X)
                    {
                        return ' ';
                    }
                    else
                    {
                        return 'Z';
                    }
                }
            };

            BufferVerificationHelper(app, area, hConsole, expectedCharAlgorithm);

            ScreenFillHelper(app, area, hConsole);
            Log.Comment("Clear line after");
            app.UIRoot.SendKeys("K");

            expectedCharAlgorithm = (int rowId, int colId, int height, int width) =>
            {
                if (rowId == (height - 1) && colId == (width - 1))
                {
                    return ' ';
                }
                else if (rowId != cursorPos.Y)
                {
                    return 'Z';
                }
                else
                {
                    if (colId < cursorPos.X)
                    {
                        return 'Z';
                    }
                    else
                    {
                        return ' ';
                    }
                }
            };

            BufferVerificationHelper(app, area, hConsole, expectedCharAlgorithm);

            ScreenFillHelper(app, area, hConsole);
            Log.Comment("Clear line before");
            app.UIRoot.SendKeys("k");

            expectedCharAlgorithm = (int rowId, int colId, int height, int width) =>
            {
                if (rowId == (height - 1) && colId == (width - 1))
                {
                    return ' ';
                }
                else if (rowId != cursorPos.Y)
                {
                    return 'Z';
                }
                else
                {
                    if (colId <= cursorPos.X)
                    {
                        return ' ';
                    }
                    else
                    {
                        return 'Z';
                    }
                }
            };

            BufferVerificationHelper(app, area, hConsole, expectedCharAlgorithm);
			_logger.LogDebug("<<< Exiting TestAreaEraseCommands(app={App},area={Area},hConsole={HConsole})", app, area, hConsole);
        }

        delegate char GetExpectedChar(int rowId, int colId, int height, int width);

        private static void ScreenFillHelper(CmdApp app, ViewportArea area, IntPtr hConsole)
        {
			_logger.LogDebug(">>> Entering ScreenFillHelper(app={App},area={Area},hConsole={HConsole})", app, area, hConsole);
            Log.Comment("Fill screen with junk");
            app.UIRoot.SendKeys(Keys.Shift + "`" + Keys.Shift);

            Globals.WaitForTimeout(); // give the buffer time to fill.

            GetExpectedChar expectedCharAlgorithm = (int rowId, int colId, int height, int width) =>
            {
                // For the very last bottom right corner character, it should be space. Every other character is a Z when filled.
                if (rowId == (height - 1) && colId == (width - 1))
                {
                    return ' ';
                }
                else
                {
                    return 'Z';
                }
            };

            BufferVerificationHelper(app, area, hConsole, expectedCharAlgorithm);
        }

        private static void BufferVerificationHelper(CmdApp app, ViewportArea area, IntPtr hConsole, GetExpectedChar expectedCharAlgorithm)
        {
            WinCon.SMALL_RECT viewport = app.GetViewport(hConsole);
            Rectangle selectRect = new Rectangle(viewport.Left, viewport.Top, viewport.Width, viewport.Height);
            IEnumerable<string> scrapedText = area.GetLinesInRectangle(hConsole, selectRect);

            Verify.AreEqual(viewport.Height, scrapedText.Count(), "Verify the rows scraped is equal to the entire viewport height.");

            bool isValidState = true;
            string[] rows = scrapedText.ToArray();
            for (int i = 0; i < rows.Length; i++)
            {
                for (int j = 0; j < viewport.Width; j++)
                {
                    char actual = rows[i][j];
                    char expected = expectedCharAlgorithm(i, j, rows.Length, viewport.Width);

                    isValidState = actual == expected;

                    if (!isValidState)
                    {
                        Verify.Fail(string.Format("Text buffer verification failed at Row: {0} Col: {1} Expected: '{2}' Actual: '{3}'", i, j, expected, actual));
                        break;
                    }
                }

                if (!isValidState)
                {
                    break;
                }
            }
        }
    }
}
