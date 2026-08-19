// <copyright file="TerminalContainer.cs" company="Microsoft Corporation">
// Copyright (c) Microsoft Corporation.
// Licensed under the MIT license.
// </copyright>

namespace Microsoft.Terminal.Wpf
{
    using System;
    using System.Runtime.InteropServices;
    using System.Windows;
    using System.Windows.Automation.Peers;
    using System.Windows.Interop;
    using System.Windows.Media;
	using Microsoft.Extensions.Logging;

    /// <summary>
    /// The container class that hosts the native hwnd terminal.
    /// </summary>
    /// <remarks>
    /// This class is only left public since xaml cannot work with internal classes.
    /// </remarks>
    public class TerminalContainer : HwndHost
    {
		private static readonly Microsoft.Extensions.Logging.ILogger _logger = Microsoft.Extensions.Logging.Abstractions.NullLogger.Instance;
        private ITerminalConnection connection;
        private IntPtr hwnd;
        private IntPtr terminal;
        private NativeMethods.ScrollCallback scrollCallback;
        private NativeMethods.WriteCallback writeCallback;

        /// <summary>
        /// Initializes a new instance of the <see cref="TerminalContainer"/> class.
        /// </summary>
        public TerminalContainer()
        {
			_logger.LogDebug(">>> Entering TerminalContainer()");
            // WPF & TSF can't deal with us setting TF_TMAE_CONSOLE on the UI thread.
            // It simply crashes on Windows 10 if you use the Emoji picker.
            // (On later versions of Windows it just doesn't work.)
            NativeMethods.AvoidBuggyTSFConsoleFlags();

            this.MessageHook += this.TerminalContainer_MessageHook;
            this.GotFocus += this.TerminalContainer_GotFocus;
            this.Focusable = true;
			_logger.LogDebug("TerminalContainer(): this.Focusable → {Arg0}", this.Focusable);
			_logger.LogDebug("<<< Exiting TerminalContainer()");
        }

        /// <summary>
        /// Event that is fired when the terminal buffer scrolls from text output.
        /// </summary>
        internal event EventHandler<(int viewTop, int viewHeight, int bufferSize)> TerminalScrolled;

        /// <summary>
        /// Event that is fired when the user engages in a mouse scroll over the terminal hwnd.
        /// </summary>
        internal event EventHandler<int> UserScrolled;

        /// <summary>
        /// Gets or sets a value indicating whether if the renderer should automatically resize to fill the control
        /// on user action.
        /// </summary>
        internal bool AutoResize { get; set; } = true;

        /// <summary>
        /// Gets or sets the size of the parent user control that hosts the terminal hwnd.
        /// </summary>
        /// <remarks>Control size is in device independent units, but for simplicity all sizes should be scaled.</remarks>
        internal Size TerminalControlSize { get; set; }

        /// <summary>
        /// Gets or sets the size of the terminal renderer.
        /// </summary>
        internal Size TerminalRendererSize { get; set; }

        /// <summary>
        /// Gets the current character rows available to the terminal.
        /// </summary>
        internal int Rows { get; private set; }

        /// <summary>
        /// Gets the current character columns available to the terminal.
        /// </summary>
        internal int Columns { get; private set; }

        /// <summary>
        /// Gets the window handle of the terminal.
        /// </summary>
        internal IntPtr Hwnd => this.hwnd;

        /// <summary>
        /// Sets the connection to the terminal backend.
        /// </summary>
        internal ITerminalConnection Connection
        {
            private get
            {
                return this.connection;
            }

            set
            {
                if (this.connection != null)
                {
                    this.connection.TerminalOutput -= this.Connection_TerminalOutput;
                }

                this.Connection_TerminalOutput(this, new TerminalOutputEventArgs("\x001bc\x1b]104\x1b\\")); // reset console/clear screen - https://github.com/microsoft/terminal/pull/15062#issuecomment-1505654110
                var wasNull = this.connection == null;
                this.connection = value;
                if (this.connection != null)
                {
                    if (wasNull)
                    {
                        this.Connection_TerminalOutput(this, new TerminalOutputEventArgs("\x1b[?25h")); // show cursor
                    }

                    this.connection.TerminalOutput += this.Connection_TerminalOutput;
                    this.connection.Start();
                }
                else
                {
                    this.Connection_TerminalOutput(this, new TerminalOutputEventArgs("\x1b[?25l")); // hide cursor
                }
            }
        }

        /// <summary>
        /// Manually invoke a scroll of the terminal buffer.
        /// </summary>
        /// <param name="viewTop">The top line to show in the terminal.</param>
        internal void UserScroll(int viewTop)
        {
			_logger.LogDebug(">>> Entering UserScroll(viewTop={ViewTop})", viewTop);
            NativeMethods.TerminalUserScroll(this.terminal, viewTop);
			_logger.LogDebug("<<< Exiting UserScroll(viewTop={ViewTop})", viewTop);
        }

        /// <summary>
        /// Sets the theme for the terminal. This includes font family, size, color, as well as background and foreground colors.
        /// </summary>
        /// <param name="theme">The color theme for the terminal to use.</param>
        /// <param name="fontFamily">The font family to use in the terminal.</param>
        /// <param name="fontSize">The font size to use in the terminal.</param>
        internal void SetTheme(TerminalTheme theme, string fontFamily, short fontSize)
        {
			_logger.LogDebug(">>> Entering SetTheme(theme={Theme},fontFamily={FontFamily},fontSize={FontSize})", theme, fontFamily, fontSize);
            var dpiScale = VisualTreeHelper.GetDpi(this);

			_logger.LogDebug("SetTheme(theme={Theme},fontFamily={FontFamily},fontSize={FontSize}): dpiScale → {DpiScale}", theme, fontFamily, fontSize, dpiScale);
            NativeMethods.TerminalSetTheme(this.terminal, theme, fontFamily, fontSize, (int)dpiScale.PixelsPerInchX);

            // Validate before resizing that we have a non-zero size.
            if (!this.RenderSize.IsEmpty && !this.TerminalControlSize.IsEmpty
                && this.TerminalControlSize.Width != 0 && this.TerminalControlSize.Height != 0)
            {
                this.Resize(this.TerminalControlSize);
            }
			_logger.LogDebug("<<< Exiting SetTheme(theme={Theme},fontFamily={FontFamily},fontSize={FontSize})", theme, fontFamily, fontSize);
        }

        /// <summary>
        /// Gets the selected text from the terminal renderer and clears the selection.
        /// </summary>
        /// <returns>The selected text, empty if no text is selected.</returns>
        internal string GetSelectedText()
        {
			_logger.LogDebug(">>> Entering GetSelectedText()");
            if (NativeMethods.TerminalIsSelectionActive(this.terminal))
            {
			_logger.LogDebug("<<< Exiting GetSelectedText()");
                return NativeMethods.TerminalGetSelection(this.terminal);
            }

            return string.Empty;
        }

        /// <summary>
        /// Triggers a resize of the terminal with the given size, redrawing the rendered text.
        /// </summary>
        /// <param name="renderSize">Size of the rendering window.</param>
        internal void Resize(Size renderSize)
        {
			_logger.LogDebug(">>> Entering Resize(renderSize={RenderSize})", renderSize);
            if (renderSize.Width == 0 || renderSize.Height == 0)
            {
				_logger.LogWarning("Throwing ArgumentException from Resize(renderSize={RenderSize})", renderSize);
                throw new ArgumentException("Terminal column or row count cannot be 0.", nameof(renderSize));
            }

            NativeMethods.TerminalTriggerResize(
                this.terminal,
                (int)renderSize.Width,
                (int)renderSize.Height,
                out NativeMethods.TilSize dimensions);

            this.Rows = dimensions.Y;
			_logger.LogDebug("Resize(renderSize={RenderSize}): this.Rows → {Arg1}", renderSize, this.Rows);
            this.Columns = dimensions.X;
            this.TerminalRendererSize = renderSize;
			_logger.LogDebug("Resize(renderSize={RenderSize}): this.Columns → {Arg1}", renderSize, this.Columns);
			_logger.LogDebug("Resize(renderSize={RenderSize}): this.TerminalRendererSize → {Arg1}", renderSize, this.TerminalRendererSize);

            this.Connection?.Resize((uint)dimensions.Y, (uint)dimensions.X);
			_logger.LogDebug("<<< Exiting Resize(renderSize={RenderSize})", renderSize);
        }

        /// <summary>
        /// Resizes the terminal using row and column count as the new size.
        /// </summary>
        /// <param name="rows">Number of rows to show.</param>
        /// <param name="columns">Number of columns to show.</param>
        internal void Resize(uint rows, uint columns)
        {
			_logger.LogDebug(">>> Entering Resize(rows={Rows},columns={Columns})", rows, columns);
            if (rows == 0)
            {
				_logger.LogWarning("Throwing ArgumentException from Resize(rows={Rows},columns={Columns})", rows, columns);
                throw new ArgumentException("Terminal row count cannot be 0.", nameof(rows));
            }

            if (columns == 0)
            {
                throw new ArgumentException("Terminal column count cannot be 0.", nameof(columns));
            }

            NativeMethods.TilSize dimensions = new NativeMethods.TilSize
            {
                X = (int)columns,
                Y = (int)rows,
            };

            NativeMethods.TerminalTriggerResizeWithDimension(this.terminal, dimensions, out var dimensionsInPixels);

            this.Columns = dimensions.X;
			_logger.LogDebug("Resize(rows={Rows},columns={Columns}): this.Columns → {Arg2}", rows, columns, this.Columns);
            this.Rows = dimensions.Y;

			_logger.LogDebug("Resize(rows={Rows},columns={Columns}): this.Rows → {Arg2}", rows, columns, this.Rows);
            this.TerminalRendererSize = new Size
            {
                Width = dimensionsInPixels.X,
                Height = dimensionsInPixels.Y,
            };
			_logger.LogDebug("Resize(rows={Rows},columns={Columns}): this.TerminalRendererSize → {Arg2}", rows, columns, this.TerminalRendererSize);

            this.Connection?.Resize((uint)dimensions.Y, (uint)dimensions.X);
			_logger.LogDebug("<<< Exiting Resize(rows={Rows},columns={Columns})", rows, columns);
        }

        /// <summary>
        /// Calculates the rows and columns that would fit in the given size.
        /// </summary>
        /// <param name="size">DPI scaled size.</param>
        /// <returns>Amount of rows and columns that would fit the given size.</returns>
        internal (int columns, int rows) CalculateRowsAndColumns(Size size)
        {
			_logger.LogDebug(">>> Entering CalculateRowsAndColumns(columns={Columns},rows={Rows})", columns, rows);
            NativeMethods.TerminalCalculateResize(this.terminal, (int)size.Width, (int)size.Height, out NativeMethods.TilSize dimensions);

			_logger.LogDebug("<<< Exiting CalculateRowsAndColumns(columns={Columns},rows={Rows})", columns, rows);
            return (dimensions.X, dimensions.Y);
        }

        /// <summary>
        /// Triggers the terminal resize event if more space is available in the terminal control.
        /// </summary>
        internal void RaiseResizedIfDrawSpaceIncreased()
        {
			_logger.LogDebug(">>> Entering RaiseResizedIfDrawSpaceIncreased()");
            var (columns, rows) = this.CalculateRowsAndColumns(this.TerminalControlSize);

            if (this.Columns < columns || this.Rows < rows)
            {
                this.connection?.Resize((uint)rows, (uint)columns);
            }
			_logger.LogDebug("<<< Exiting RaiseResizedIfDrawSpaceIncreased()");
        }

        /// <summary>
        /// WPF's HwndHost likes to mark the WM_GETOBJECT message as handled to
        /// force the usage of the WPF automation peer. We explicitly mark it as
        /// not handled and don't return an automation peer in "OnCreateAutomationPeer" below.
        /// This forces the message to go down to the HwndTerminal where we return terminal's UiaProvider.
        /// </summary>
        /// <inheritdoc/>
        protected override IntPtr WndProc(IntPtr hwnd, int msg, IntPtr wParam, IntPtr lParam, ref bool handled)
        {
			_logger.LogDebug(">>> Entering WndProc(hwnd={Hwnd},msg={Msg},wParam={WParam},lParam={LParam},handled={Handled})", hwnd, msg, wParam, lParam, handled);
            if (msg == (int)NativeMethods.WindowMessage.WM_GETOBJECT)
            {
                handled = false;
			_logger.LogDebug("<<< Exiting WndProc(hwnd={Hwnd},msg={Msg},wParam={WParam},lParam={LParam},handled={Handled})", hwnd, msg, wParam, lParam, handled);
                return IntPtr.Zero;
            }

            return base.WndProc(hwnd, msg, wParam, lParam, ref handled);
        }

        /// <inheritdoc/>
        protected override AutomationPeer OnCreateAutomationPeer()
        {
			_logger.LogDebug(">>> Entering OnCreateAutomationPeer()");
			_logger.LogDebug("<<< Exiting OnCreateAutomationPeer()");
            return null;
        }

        /// <inheritdoc/>
        protected override void OnDpiChanged(DpiScale oldDpi, DpiScale newDpi)
        {
			_logger.LogDebug(">>> Entering OnDpiChanged(oldDpi={OldDpi},newDpi={NewDpi})", oldDpi, newDpi);
            if (this.terminal != IntPtr.Zero)
            {
                NativeMethods.TerminalDpiChanged(this.terminal, (int)(NativeMethods.USER_DEFAULT_SCREEN_DPI * newDpi.DpiScaleX));
            }
			_logger.LogDebug("<<< Exiting OnDpiChanged(oldDpi={OldDpi},newDpi={NewDpi})", oldDpi, newDpi);
        }

        /// <inheritdoc/>
        protected override HandleRef BuildWindowCore(HandleRef hwndParent)
        {
			_logger.LogDebug(">>> Entering BuildWindowCore(hwndParent={HwndParent})", hwndParent);
            var dpiScale = VisualTreeHelper.GetDpi(this);
            NativeMethods.CreateTerminal(hwndParent.Handle, out this.hwnd, out this.terminal);
			_logger.LogDebug("BuildWindowCore(hwndParent={HwndParent}): dpiScale → {DpiScale}", hwndParent, dpiScale);

            this.scrollCallback = this.OnScroll;
			_logger.LogDebug("BuildWindowCore(hwndParent={HwndParent}): scrollCallback → {ScrollCallback}", hwndParent, scrollCallback);
			_logger.LogDebug("BuildWindowCore(hwndParent={HwndParent}): this.scrollCallback → {Arg1}", hwndParent, this.scrollCallback);
            this.writeCallback = this.OnWrite;

			_logger.LogDebug("BuildWindowCore(hwndParent={HwndParent}): writeCallback → {WriteCallback}", hwndParent, writeCallback);
			_logger.LogDebug("BuildWindowCore(hwndParent={HwndParent}): this.writeCallback → {Arg1}", hwndParent, this.writeCallback);
            NativeMethods.TerminalRegisterScrollCallback(this.terminal, this.scrollCallback);
            NativeMethods.TerminalRegisterWriteCallback(this.terminal, this.writeCallback);

            // If the saved DPI scale isn't the default scale, we push it to the terminal.
            if (dpiScale.PixelsPerInchX != NativeMethods.USER_DEFAULT_SCREEN_DPI)
            {
                NativeMethods.TerminalDpiChanged(this.terminal, (int)dpiScale.PixelsPerInchX);
            }

			_logger.LogDebug("<<< Exiting BuildWindowCore(hwndParent={HwndParent})", hwndParent);
            return new HandleRef(this, this.hwnd);
        }

        /// <inheritdoc/>
        protected override void DestroyWindowCore(HandleRef hwnd)
        {
			_logger.LogDebug(">>> Entering DestroyWindowCore(hwnd={Hwnd})", hwnd);
            NativeMethods.DestroyTerminal(this.terminal);
            this.terminal = IntPtr.Zero;
			_logger.LogDebug("DestroyWindowCore(hwnd={Hwnd}): terminal → {Terminal}", hwnd, terminal);
			_logger.LogDebug("DestroyWindowCore(hwnd={Hwnd}): this.terminal → {Arg1}", hwnd, this.terminal);
			_logger.LogDebug("<<< Exiting DestroyWindowCore(hwnd={Hwnd})", hwnd);
        }

        private static void UnpackKeyMessage(IntPtr wParam, IntPtr lParam, out ushort vkey, out ushort scanCode, out ushort flags)
        {
			_logger.LogDebug(">>> Entering UnpackKeyMessage(wParam={WParam},lParam={LParam})", wParam, lParam);
            ulong scanCodeAndFlags = ((ulong)lParam >> 16) & 0xFFFF;
            scanCode = (ushort)(scanCodeAndFlags & 0x00FFu);
            flags = (ushort)(scanCodeAndFlags & 0xFF00u);
			_logger.LogDebug("UnpackKeyMessage(wParam={WParam},lParam={LParam}): flags → {Flags}", wParam, lParam, flags);
			_logger.LogDebug("UnpackKeyMessage(wParam={WParam},lParam={LParam}): scanCode → {ScanCode}", wParam, lParam, scanCode);
            vkey = (ushort)wParam;
			_logger.LogDebug("UnpackKeyMessage(wParam={WParam},lParam={LParam}): vkey → {Vkey}", wParam, lParam, vkey);
			_logger.LogDebug("<<< Exiting UnpackKeyMessage(wParam={WParam},lParam={LParam})", wParam, lParam);
        }

        private static void UnpackCharMessage(IntPtr wParam, IntPtr lParam, out char character, out ushort scanCode, out ushort flags)
        {
			_logger.LogDebug(">>> Entering UnpackCharMessage(wParam={WParam},lParam={LParam})", wParam, lParam);
            UnpackKeyMessage(wParam, lParam, out ushort vKey, out scanCode, out flags);
            character = (char)vKey;
			_logger.LogDebug("UnpackCharMessage(wParam={WParam},lParam={LParam}): character → {Character}", wParam, lParam, character);
			_logger.LogDebug("<<< Exiting UnpackCharMessage(wParam={WParam},lParam={LParam})", wParam, lParam);
        }

        private void TerminalContainer_GotFocus(object sender, RoutedEventArgs e)
        {
			_logger.LogDebug(">>> Entering TerminalContainer_GotFocus(sender={Sender},e={E})", sender, e);
            e.Handled = true;
            NativeMethods.SetFocus(this.hwnd);
			_logger.LogDebug("TerminalContainer_GotFocus(sender={Sender},e={E}): e.Handled → {Arg2}", sender, e, e.Handled);
			_logger.LogDebug("<<< Exiting TerminalContainer_GotFocus(sender={Sender},e={E})", sender, e);
        }

        private IntPtr TerminalContainer_MessageHook(IntPtr hwnd, int msg, IntPtr wParam, IntPtr lParam, ref bool handled)
        {
			_logger.LogDebug(">>> Entering TerminalContainer_MessageHook(hwnd={Hwnd},msg={Msg},wParam={WParam},lParam={LParam},handled={Handled})", hwnd, msg, wParam, lParam, handled);
            if (hwnd == this.hwnd)
            {
                switch ((NativeMethods.WindowMessage)msg)
                {
                    case NativeMethods.WindowMessage.WM_SETFOCUS:
                        NativeMethods.TerminalSetFocused(this.terminal, true);
                        break;
                    case NativeMethods.WindowMessage.WM_KILLFOCUS:
                        NativeMethods.TerminalSetFocused(this.terminal, false);
                        break;
                    case NativeMethods.WindowMessage.WM_MOUSEACTIVATE:
                        this.Focus();
                        NativeMethods.SetFocus(this.hwnd);
                        break;
                    case NativeMethods.WindowMessage.WM_SYSKEYDOWN: // fallthrough
                    case NativeMethods.WindowMessage.WM_KEYDOWN:
                        {
                            UnpackKeyMessage(wParam, lParam, out ushort vkey, out ushort scanCode, out ushort flags);
                            NativeMethods.TerminalSendKeyEvent(this.terminal, vkey, scanCode, flags, true);
                            break;
                        }

                    case NativeMethods.WindowMessage.WM_SYSKEYUP: // fallthrough
                    case NativeMethods.WindowMessage.WM_KEYUP:
                        {
                            // WM_KEYUP lParam layout documentation: https://docs.microsoft.com/en-us/windows/win32/inputdev/wm-keyup
                            UnpackKeyMessage(wParam, lParam, out ushort vkey, out ushort scanCode, out ushort flags);
                            NativeMethods.TerminalSendKeyEvent(this.terminal, (ushort)wParam, scanCode, flags, false);
                            break;
                        }

                    case NativeMethods.WindowMessage.WM_CHAR:
                        {
                            // WM_CHAR lParam layout documentation: https://docs.microsoft.com/en-us/windows/win32/inputdev/wm-char
                            UnpackCharMessage(wParam, lParam, out char character, out ushort scanCode, out ushort flags);
                            NativeMethods.TerminalSendCharEvent(this.terminal, character, scanCode, flags);
                            break;
                        }

                    case NativeMethods.WindowMessage.WM_WINDOWPOSCHANGED:
                        var windowpos = (NativeMethods.WINDOWPOS)Marshal.PtrToStructure(lParam, typeof(NativeMethods.WINDOWPOS));
						_logger.LogDebug("TerminalContainer_MessageHook(hwnd={Hwnd},msg={Msg},wParam={WParam},lParam={LParam},handled={Handled}): windowpos → {Windowpos}", hwnd, msg, wParam, lParam, handled, windowpos);
                        if (((NativeMethods.SetWindowPosFlags)windowpos.flags).HasFlag(NativeMethods.SetWindowPosFlags.SWP_NOSIZE)
                            || (windowpos.cx == 0 && windowpos.cy == 0))
                        {
                            break;
                        }

                        NativeMethods.TilSize dimensions;

                        if (this.AutoResize)
                        {
                            NativeMethods.TerminalTriggerResize(this.terminal, windowpos.cx, windowpos.cy, out dimensions);

                            this.Columns = dimensions.X;
							_logger.LogDebug("TerminalContainer_MessageHook(hwnd={Hwnd},msg={Msg},wParam={WParam},lParam={LParam},handled={Handled}): this.Columns → {Arg5}", hwnd, msg, wParam, lParam, handled, this.Columns);
                            this.Rows = dimensions.Y;

							_logger.LogDebug("TerminalContainer_MessageHook(hwnd={Hwnd},msg={Msg},wParam={WParam},lParam={LParam},handled={Handled}): this.Rows → {Arg5}", hwnd, msg, wParam, lParam, handled, this.Rows);
                            this.TerminalRendererSize = new Size
                            {
                                Width = windowpos.cx,
                                Height = windowpos.cy,
                            };
							_logger.LogDebug("TerminalContainer_MessageHook(hwnd={Hwnd},msg={Msg},wParam={WParam},lParam={LParam},handled={Handled}): this.TerminalRendererSize → {Arg5}", hwnd, msg, wParam, lParam, handled, this.TerminalRendererSize);
                        }
                        else
                        {
                            // Calculate the new columns and rows that fit the total control size and alert the control to redraw the margins.
                            NativeMethods.TerminalCalculateResize(this.terminal, (int)this.TerminalControlSize.Width, (int)this.TerminalControlSize.Height, out dimensions);
                        }

                        this.Connection?.Resize((uint)dimensions.Y, (uint)dimensions.X);
                        break;

                    case NativeMethods.WindowMessage.WM_MOUSEWHEEL:
                        var delta = (short)(((long)wParam) >> 16);
						_logger.LogDebug("TerminalContainer_MessageHook(hwnd={Hwnd},msg={Msg},wParam={WParam},lParam={LParam},handled={Handled}): delta → {Delta}", hwnd, msg, wParam, lParam, handled, delta);
                        this.UserScrolled?.Invoke(this, delta);
                        break;
                }
            }

			_logger.LogDebug("<<< Exiting TerminalContainer_MessageHook(hwnd={Hwnd},msg={Msg},wParam={WParam},lParam={LParam},handled={Handled})", hwnd, msg, wParam, lParam, handled);
            return IntPtr.Zero;
        }

        private void Connection_TerminalOutput(object sender, TerminalOutputEventArgs e)
        {
			_logger.LogDebug(">>> Entering Connection_TerminalOutput(sender={Sender},e={E})", sender, e);
            if (this.terminal == IntPtr.Zero || string.IsNullOrEmpty(e.Data))
            {
                return;
            }

            NativeMethods.TerminalSendOutput(this.terminal, e.Data);
			_logger.LogDebug("<<< Exiting Connection_TerminalOutput(sender={Sender},e={E})", sender, e);
        }

        private void OnScroll(int viewTop, int viewHeight, int bufferSize)
        {
			_logger.LogDebug(">>> Entering OnScroll(viewTop={ViewTop},viewHeight={ViewHeight},bufferSize={BufferSize})", viewTop, viewHeight, bufferSize);
            this.TerminalScrolled?.Invoke(this, (viewTop, viewHeight, bufferSize));
			_logger.LogDebug("<<< Exiting OnScroll(viewTop={ViewTop},viewHeight={ViewHeight},bufferSize={BufferSize})", viewTop, viewHeight, bufferSize);
        }

        private void OnWrite(string data)
        {
			_logger.LogDebug(">>> Entering OnWrite(data={Data})", data);
            this.Connection?.WriteInput(data);
			_logger.LogDebug("<<< Exiting OnWrite(data={Data})", data);
        }
    }
}
