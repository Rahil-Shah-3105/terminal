//----------------------------------------------------------------------------------------------------------------------
// <copyright file="ViewportArea.cs" company="Microsoft">
//     Copyright (c) Microsoft Corporation.  All rights reserved.
// </copyright>
// <summary>Wrapper and helper for instantiating and interacting with the main text region (viewport area) of the console.</summary>
//----------------------------------------------------------------------------------------------------------------------
namespace Conhost.UIA.Tests.Elements
{
    using System;
    using System.Collections.Generic;
    using System.Drawing;
    using System.Linq;
    using System.Runtime.InteropServices;
    using System.Text;

    using OpenQA.Selenium.Appium;

    using WEX.Logging.Interop;
    using WEX.TestExecution;

    using Conhost.UIA.Tests.Common;
    using Conhost.UIA.Tests.Common.NativeMethods;
    using OpenQA.Selenium;
	using Microsoft.Extensions.Logging;

    public class ViewportArea : IDisposable
    {
		private static readonly Microsoft.Extensions.Logging.ILogger _logger = Microsoft.Extensions.Logging.Abstractions.NullLogger.Instance;
        private CmdApp app;
        private Point clientTopLeft;
        private Size sizeFont;

        private ViewportStates state;

        public enum ViewportStates
        {
            Normal,
            Mark, // keyboard selection
            Select, // mouse selection
            Scroll
        }

        public ViewportArea(CmdApp app)
        {
			_logger.LogDebug(">>> Entering ViewportArea(app={App})", app);
            this.app = app;

			_logger.LogDebug("ViewportArea(app={App}): this.app → {Arg1}", app, this.app);
			_logger.LogDebug("ViewportArea(app={App}): app → {App1}", app, app);
            this.state = ViewportStates.Normal;
			_logger.LogDebug("ViewportArea(app={App}): state → {State}", app, state);
			_logger.LogDebug("ViewportArea(app={App}): this.state → {Arg1}", app, this.state);

            this.InitializeFont();
            this.InitializeWindow();
			_logger.LogDebug("<<< Exiting ViewportArea(app={App})", app);
        }

        ~ViewportArea()
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

        protected virtual void Dispose(bool disposing)
        {
			_logger.LogDebug(">>> Entering Dispose(disposing={Disposing})", disposing);
            // Don't need to dispose of anything now, but this helps maintain the pattern used by other controls.
			_logger.LogDebug("<<< Exiting Dispose(disposing={Disposing})", disposing);
        }

        private void InitializeFont()
        {
			_logger.LogDebug(">>> Entering InitializeFont()");
            AutoHelpers.LogInvariant("Initializing font data for viewport area...");
            this.sizeFont = new Size();
			_logger.LogDebug("InitializeFont(): sizeFont → {SizeFont}", sizeFont);

            IntPtr hCon = app.GetStdOutHandle();
			_logger.LogDebug("InitializeFont(): hCon → {HCon}", hCon);
            Verify.IsNotNull(hCon, "Check that we obtained the console output handle.");

            WinCon.CONSOLE_FONT_INFO cfi = new WinCon.CONSOLE_FONT_INFO();
			_logger.LogDebug("InitializeFont(): cfi → {Cfi}", cfi);
            NativeMethods.Win32BoolHelper(WinCon.GetCurrentConsoleFont(hCon, false, out cfi), "Attempt to get current console font.");

            this.sizeFont.Width = cfi.dwFontSize.X;
            this.sizeFont.Height = cfi.dwFontSize.Y;
            AutoHelpers.LogInvariant("Font size is X:{0} Y:{1}", this.sizeFont.Width, this.sizeFont.Height);
			_logger.LogDebug("<<< Exiting InitializeFont()");
        }

        private void InitializeWindow()
        {
			_logger.LogDebug(">>> Entering InitializeWindow()");
            AutoHelpers.LogInvariant("Initializing window data for viewport area...");

            IntPtr hWnd = app.GetWindowHandle();
			_logger.LogDebug("InitializeWindow(): hWnd → {HWnd}", hWnd);

            User32.RECT lpRect;
            User32.GetClientRect(hWnd, out lpRect);

            int style = User32.GetWindowLong(hWnd, User32.GWL_STYLE);
			_logger.LogDebug("InitializeWindow(): style → {Style}", style);
            int exStyle = User32.GetWindowLong(hWnd, User32.GWL_EXSTYLE);
			_logger.LogDebug("InitializeWindow(): exStyle → {ExStyle}", exStyle);

            Verify.IsTrue(User32.AdjustWindowRectEx(ref lpRect, style, false, exStyle));

            this.clientTopLeft = new Point();
			_logger.LogDebug("InitializeWindow(): clientTopLeft → {ClientTopLeft}", clientTopLeft);
            this.clientTopLeft.X = Math.Abs(lpRect.left);
            this.clientTopLeft.Y = Math.Abs(lpRect.top);
            AutoHelpers.LogInvariant("Top left corner of client area is at X:{0} Y:{1}", this.clientTopLeft.X, this.clientTopLeft.Y);
			_logger.LogDebug("<<< Exiting InitializeWindow()");
        }

        public void ExitModes()
        {
			_logger.LogDebug(">>> Entering ExitModes()");
            app.UIRoot.SendKeys(Keys.Escape);
            this.state = ViewportStates.Normal;
			_logger.LogDebug("ExitModes(): this.state → {Arg0}", this.state);
			_logger.LogDebug("ExitModes(): state → {State}", state);
			_logger.LogDebug("<<< Exiting ExitModes()");
        }

        public void EnterMode(ViewportStates state)
        {
			_logger.LogDebug(">>> Entering EnterMode(state={State})", state);
            if (state == ViewportStates.Normal)
            {
                ExitModes();
                return;
            }

            var titleBar = app.UIRoot.FindElementByAccessibilityId("TitleBar");
			_logger.LogDebug("EnterMode(state={State}): titleBar → {TitleBar}", state, titleBar);
            app.Session.Mouse.ContextClick(titleBar.Coordinates);

            Globals.WaitForTimeout();
            var contextMenu = app.Session.FindElementByClassName(Globals.PopupMenuClassId);
			_logger.LogDebug("EnterMode(state={State}): contextMenu → {ContextMenu}", state, contextMenu);

            var editButton = contextMenu.FindElementByName("Edit");
			_logger.LogDebug("EnterMode(state={State}): editButton → {EditButton}", state, editButton);

            editButton.Click();
            Globals.WaitForTimeout();

            Globals.WaitForTimeout();

            AppiumWebElement subMenuButton;
            switch (state)
            {
                case ViewportStates.Mark:
                    subMenuButton = app.Session.FindElementByName("Mark");
					_logger.LogDebug("EnterMode(state={State}): subMenuButton → {SubMenuButton}", state, subMenuButton);
                    break;
                default:
					_logger.LogWarning("Throwing NotImplementedException from EnterMode(state={State})", state);
                    throw new NotImplementedException(AutoHelpers.FormatInvariant("Set Mode doesn't yet support type of '{0}'", state.ToString()));
            }

            subMenuButton.Click();
            Globals.WaitForTimeout();

            this.state = state;
			_logger.LogDebug("EnterMode(state={State}): state → {State1}", state, state);
			_logger.LogDebug("EnterMode(state={State}): this.state → {Arg1}", state, this.state);
			_logger.LogDebug("<<< Exiting EnterMode(state={State})", state);
        }


        // Accepts Point in characters. Will convert to pixels and move to the right location relative to this viewport.
        public void MouseMove(Point pt)
        {
			_logger.LogDebug(">>> Entering MouseMove(pt={Pt})", pt);
            Log.Comment($"Character position {pt.X}, {pt.Y}");

            Point modPoint = pt;
            ConvertCharacterOffsetToPixelPosition(ref modPoint);

            Log.Comment($"Pixel position {modPoint.X}, {modPoint.Y}");

            app.Session.Mouse.MouseMove(app.UIRoot.Coordinates, modPoint.X, modPoint.Y);
			_logger.LogDebug("<<< Exiting MouseMove(pt={Pt})", pt);
        }

        public void MouseDown()
        {
			_logger.LogDebug(">>> Entering MouseDown()");
            app.Session.Mouse.MouseDown(null);
			_logger.LogDebug("<<< Exiting MouseDown()");
        }

        public void MouseUp()
        {
			_logger.LogDebug(">>> Entering MouseUp()");
            app.Session.Mouse.MouseUp(null);
			_logger.LogDebug("<<< Exiting MouseUp()");
        }

        private void ConvertCharacterOffsetToPixelPosition(ref Point pt)
        {
			_logger.LogDebug(">>> Entering ConvertCharacterOffsetToPixelPosition(pt={Pt})", pt);
            // Scale by pixel count per character
            pt.X *= this.sizeFont.Width;
            pt.Y *= this.sizeFont.Height;

            // Move it to center of character
            pt.X += this.sizeFont.Width / 2;
            pt.Y += this.sizeFont.Height / 2;

            // Adjust to the top left corner of the client rectangle.
            pt.X += this.clientTopLeft.X;
            pt.Y += this.clientTopLeft.Y;
			_logger.LogDebug("<<< Exiting ConvertCharacterOffsetToPixelPosition(pt={Pt})", pt);
        }

        public WinCon.CHAR_INFO GetCharInfoAt(IntPtr handle, Point pt)
        {
			_logger.LogDebug(">>> Entering GetCharInfoAt(handle={Handle},pt={Pt})", handle, pt);
            Size size = new Size(1, 1);
            Rectangle rect = new Rectangle(pt, size);
			_logger.LogDebug("GetCharInfoAt(handle={Handle},pt={Pt}): rect → {Rect}", handle, pt, rect);
			_logger.LogDebug("GetCharInfoAt(handle={Handle},pt={Pt}): size → {Size}", handle, pt, size);

            WinCon.CHAR_INFO[,] data = GetCharInfoInRectangle(handle, rect);

			_logger.LogDebug("<<< Exiting GetCharInfoAt(handle={Handle},pt={Pt})", handle, pt);
            return data[0, 0];
        }

        public WinCon.CHAR_INFO[,] GetCharInfoInRectangle(IntPtr handle, Rectangle rect)
        {
			_logger.LogDebug(">>> Entering GetCharInfoInRectangle(handle={Handle},rect={Rect})", handle, rect);
            WinCon.SMALL_RECT readRectangle = new WinCon.SMALL_RECT();
            readRectangle.Top = (short)rect.Top;
			_logger.LogDebug("GetCharInfoInRectangle(handle={Handle},rect={Rect}): readRectangle → {ReadRectangle}", handle, rect, readRectangle);
            readRectangle.Bottom = (short)(rect.Bottom - 1);
            readRectangle.Left = (short)rect.Left;
            readRectangle.Right = (short)(rect.Right - 1);

            WinCon.COORD dataBufferSize = new WinCon.COORD();
			_logger.LogDebug("GetCharInfoInRectangle(handle={Handle},rect={Rect}): dataBufferSize → {DataBufferSize}", handle, rect, dataBufferSize);
            dataBufferSize.X = (short)rect.Width;
            dataBufferSize.Y = (short)rect.Height;

            WinCon.COORD dataBufferPos = new WinCon.COORD();
			_logger.LogDebug("GetCharInfoInRectangle(handle={Handle},rect={Rect}): dataBufferPos → {DataBufferPos}", handle, rect, dataBufferPos);
			_logger.LogDebug("GetCharInfoInRectangle(handle={Handle},rect={Rect}): dataBufferPos.X → {Arg2}", handle, rect, dataBufferPos.X);
			_logger.LogDebug("GetCharInfoInRectangle(handle={Handle},rect={Rect}): dataBufferPos.Y → {Arg2}", handle, rect, dataBufferPos.Y);
            dataBufferPos.X = 0;
            dataBufferPos.Y = 0;

            WinCon.CHAR_INFO[,] data = new WinCon.CHAR_INFO[dataBufferSize.Y, dataBufferSize.X];

            NativeMethods.Win32BoolHelper(WinCon.ReadConsoleOutput(handle, data, dataBufferSize, dataBufferPos, ref readRectangle), string.Format("Attempting to read rectangle (L: {0}, T: {1}, R: {2}, B: {3}) from output buffer.", readRectangle.Left, readRectangle.Top, readRectangle.Right, readRectangle.Bottom));

			_logger.LogDebug("<<< Exiting GetCharInfoInRectangle(handle={Handle},rect={Rect})", handle, rect);
            return data;
        }

        public IEnumerable<string> GetLinesInRectangle(IntPtr handle, Rectangle rect)
        {
			_logger.LogDebug(">>> Entering GetLinesInRectangle(handle={Handle},rect={Rect})", handle, rect);
            WinCon.CHAR_INFO[,] data = GetCharInfoInRectangle(handle, rect);
            List<string> lines = new List<string>();
			_logger.LogDebug("GetLinesInRectangle(handle={Handle},rect={Rect}): lines → {Lines}", handle, rect, lines);

            for (int row = 0; row < data.GetLength(0); row++)
            {
                StringBuilder builder = new StringBuilder();
				_logger.LogDebug("GetLinesInRectangle(handle={Handle},rect={Rect}): builder → {Builder}", handle, rect, builder);

                for (int col = 0; col < data.GetLength(1); col++)
                {
                    char z = data[row, col].UnicodeChar;
                    builder.Append(z);
                }

                lines.Add(builder.ToString());
            }

			_logger.LogDebug("<<< Exiting GetLinesInRectangle(handle={Handle},rect={Rect})", handle, rect);
            return lines;
        }
    }
}