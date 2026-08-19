// <copyright file="MainWindow.xaml.cs" company="Microsoft Corporation">
// Copyright (c) Microsoft Corporation.
// Licensed under the MIT license.
// </copyright>

using Microsoft.Terminal.Wpf;
using System;
using System.Windows;
using Microsoft.Extensions.Logging;

namespace WpfTerminalTestNetCore
{
    public class EchoConnection : Microsoft.Terminal.Wpf.ITerminalConnection
    {
		private static readonly Microsoft.Extensions.Logging.ILogger _logger = Microsoft.Extensions.Logging.Abstractions.NullLogger.Instance;
        public event EventHandler<TerminalOutputEventArgs> TerminalOutput;

        public void Resize(uint rows, uint columns)
        {
			_logger.LogDebug(">>> Entering Resize(rows={Rows},columns={Columns})", rows, columns);
			_logger.LogDebug("<<< Exiting Resize(rows={Rows},columns={Columns})", rows, columns);
            return;
        }

        public void Start()
        {
			_logger.LogDebug(">>> Entering Start()");
            TerminalOutput.Invoke(this, new TerminalOutputEventArgs("ECHO CONNECTION\r\n^A: toggle printable ESC\r\n^B: toggle SGR mouse mode\r\n^C: toggle win32 input mode\r\n\r\n"));
			_logger.LogDebug("<<< Exiting Start()");
            return;
        }

        private bool _escapeMode;
        private bool _mouseMode;
        private bool _win32InputMode;

        public void WriteInput(string data)
        {
			_logger.LogDebug(">>> Entering WriteInput(data={Data})", data);
            if (data.Length == 0)
            {
                return;
            }

            if (data[0] == '\x01') // ^A
            {
                _escapeMode = !_escapeMode;
                TerminalOutput.Invoke(this, new TerminalOutputEventArgs($"Printable ESC mode: {_escapeMode}\r\n"));
				_logger.LogDebug("WriteInput(data={Data}): _escapeMode → {_escapeMode}", data, _escapeMode);
            }
            else if (data[0] == '\x02') // ^B
            {
                _mouseMode = !_mouseMode;
				_logger.LogDebug("WriteInput(data={Data}): _mouseMode → {_mouseMode}", data, _mouseMode);
                var decSet = _mouseMode ? "h" : "l";
				_logger.LogDebug("WriteInput(data={Data}): decSet → {DecSet}", data, decSet);
                TerminalOutput.Invoke(this, new TerminalOutputEventArgs($"\x1b[?1003{decSet}\x1b[?1006{decSet}"));
                TerminalOutput.Invoke(this, new TerminalOutputEventArgs($"SGR Mouse mode (1003, 1006): {_mouseMode}\r\n"));
            }
            else if ((data[0] == '\x03') ||
                     (data == "\x1b[67;46;3;1;8;1_")) // ^C
            {
                _win32InputMode = !_win32InputMode;
				_logger.LogDebug("WriteInput(data={Data}): _win32InputMode → {_win32InputMode}", data, _win32InputMode);
                var decSet = _win32InputMode ? "h" : "l";
                TerminalOutput.Invoke(this, new TerminalOutputEventArgs($"\x1b[?9001{decSet}"));
                TerminalOutput.Invoke(this, new TerminalOutputEventArgs($"Win32 input mode: {_win32InputMode}\r\n"));

                // If escape mode isn't currently enabled, turn it on now.
                if (_win32InputMode && !_escapeMode)
                {
                    _escapeMode = true;
                    TerminalOutput.Invoke(this, new TerminalOutputEventArgs($"Printable ESC mode: {_escapeMode}\r\n"));
                }
            }
            else
            {
                // Echo back to the terminal, but make backspace/newline work properly.
                var str = data.Replace("\r", "\r\n").Replace("\x7f", "\x08 \x08");
				_logger.LogDebug("WriteInput(data={Data}): str → {Str}", data, str);
                if (_escapeMode)
                {
                    str = str.Replace("\x1b", "\u241b");
                }
                TerminalOutput.Invoke(this, new TerminalOutputEventArgs(str));
            }
			_logger.LogDebug("<<< Exiting WriteInput(data={Data})", data);
        }

        public void Close()
        {
			_logger.LogDebug(">>> Entering Close()");
			_logger.LogDebug("<<< Exiting Close()");
            return;
        }
    }
    /// <summary>
    /// Interaction logic for MainWindow.xaml
    /// </summary>
    public partial class MainWindow : Window
    {
        public MainWindow()
        {
			_logger.LogDebug(">>> Entering MainWindow()");
            InitializeComponent();
            Terminal.Loaded += Terminal_Loaded;
			_logger.LogDebug("<<< Exiting MainWindow()");
        }

        private void Terminal_Loaded(object sender, RoutedEventArgs e)
        {
			_logger.LogDebug(">>> Entering Terminal_Loaded(sender={Sender},e={E})", sender, e);
            var theme = new TerminalTheme
            {
                DefaultBackground = 0x0c0c0c,
                DefaultForeground = 0xcccccc,
                DefaultSelectionBackground = 0xcccccc,
                CursorStyle = CursorStyle.BlinkingBar,
                // This is Campbell.
                ColorTable = new uint[] { 0x0C0C0C, 0x1F0FC5, 0x0EA113, 0x009CC1, 0xDA3700, 0x981788, 0xDD963A, 0xCCCCCC, 0x767676, 0x5648E7, 0x0CC616, 0xA5F1F9, 0xFF783B, 0x9E00B4, 0xD6D661, 0xF2F2F2 },
            };

            Terminal.Connection = new EchoConnection();
            Terminal.SetTheme(theme, "Cascadia Code", 12);
            Terminal.Focus();
			_logger.LogDebug("<<< Exiting Terminal_Loaded(sender={Sender},e={E})", sender, e);
        }
    }
}
