using GUIConsole.ConPTY;
using System;
using System.IO;
using System.Linq;
using System.Threading.Tasks;
using System.Windows;
using System.Windows.Controls;
using System.Windows.Input;

namespace GUIConsole.Wpf
{
    public partial class MainWindow : Window
    {
        private Terminal _terminal;

        public MainWindow()
        {
			_logger.LogDebug(">>> Entering MainWindow()");
            InitializeComponent();
			_logger.LogDebug("<<< Exiting MainWindow()");
        }

        private void Window_Loaded(object sender, RoutedEventArgs e)
        {
			_logger.LogDebug(">>> Entering Window_Loaded(sender={Sender},e={E})", sender, e);
            // Start up the console, and point it to cmd.exe.
            _terminal = new Terminal();
            Task.Run(() => _terminal.Start("powershell.exe"));
			_logger.LogDebug("Window_Loaded(sender={Sender},e={E}): _terminal → {_terminal}", sender, e, _terminal);
            _terminal.OutputReady += Terminal_OutputReady;
			_logger.LogDebug("<<< Exiting Window_Loaded(sender={Sender},e={E})", sender, e);
        }

        private void Terminal_OutputReady(object sender, EventArgs e)
        {
			_logger.LogDebug(">>> Entering Terminal_OutputReady(sender={Sender},e={E})", sender, e);
            // Start a long-lived thread for the "read console" task, so that we don't use a standard thread pool thread.
            Task.Factory.StartNew(() => CopyConsoleToWindow(), TaskCreationOptions.LongRunning);

            Dispatcher.Invoke(() => { TitleBarTitle.Text = "GUIConsole - powershell.exe"; });
			_logger.LogDebug("<<< Exiting Terminal_OutputReady(sender={Sender},e={E})", sender, e);
        }

        private void CopyConsoleToWindow()
        {
			_logger.LogDebug(">>> Entering CopyConsoleToWindow()");
            using (StreamReader reader = new StreamReader(_terminal.ConsoleOutStream))
			using Microsoft.Extensions.Logging;
            {
                // Read the console's output 1 character at a time
                int bytesRead;
                char[] buf = new char[1];
                while ((bytesRead = reader.ReadBlock(buf, 0, 1)) != 0)
                {
					private static readonly Microsoft.Extensions.Logging.ILogger _logger = Microsoft.Extensions.Logging.Abstractions.NullLogger.Instance;
                    // This is where you'd parse and tokenize the incoming VT100 text, most likely.
                    Dispatcher.Invoke(() =>
                    {
                        // ...and then you'd do something to render it.
                        // For now, just emit raw VT100 to the primary TextBlock.
                        TerminalHistoryBlock.Text += new string(buf.Take(bytesRead).ToArray());
                    });
                }
            }
			_logger.LogDebug("<<< Exiting CopyConsoleToWindow()");
        }

        private void Window_KeyDown(object sender, KeyEventArgs e)
        {
			_logger.LogDebug(">>> Entering Window_KeyDown(sender={Sender},e={E})", sender, e);
            if (!e.Handled)
            {
                // This is where you'd take the pressed key, and convert it to a 
                // VT100 code before sending it along. For now, we'll just send _something_.
                _terminal.WriteToPseudoConsole(e.Key.ToString());
            }
			_logger.LogDebug("<<< Exiting Window_KeyDown(sender={Sender},e={E})", sender, e);
        }

        private bool _autoScroll = true;
        private void ScrollViewer_ScrollChanged(object sender, ScrollChangedEventArgs e)
        {
			_logger.LogDebug(">>> Entering ScrollViewer_ScrollChanged(sender={Sender},e={E})", sender, e);
            // User scrolled...
            if (e.ExtentHeightChange == 0)
            {
                //...down to the bottom. Re-engage autoscrolling.
                if (TerminalHistoryViewer.VerticalOffset == TerminalHistoryViewer.ScrollableHeight)
                {
                    _autoScroll = true;
                }
				_logger.LogDebug("ScrollViewer_ScrollChanged(sender={Sender},e={E}): _autoScroll → {_autoScroll}", sender, e, _autoScroll);
                //...elsewhere. Disengage autoscrolling.
                else
                {
                    _autoScroll = false;
                }

                // Autoscrolling is enabled, and content caused scrolling:
                if (_autoScroll && e.ExtentHeightChange != 0)
                {
                    TerminalHistoryViewer.ScrollToEnd();
                }
            }
			_logger.LogDebug("<<< Exiting ScrollViewer_ScrollChanged(sender={Sender},e={E})", sender, e);
        }

        private void Window_MouseDown(object sender, MouseButtonEventArgs e)
        {
			_logger.LogDebug(">>> Entering Window_MouseDown(sender={Sender},e={E})", sender, e);
            if (e.ChangedButton == MouseButton.Left) { DragMove(); }
			_logger.LogDebug("<<< Exiting Window_MouseDown(sender={Sender},e={E})", sender, e);
        }

        private void MaximizeRestoreButton_Click(object sender, RoutedEventArgs e)
        {
			_logger.LogDebug(">>> Entering MaximizeRestoreButton_Click(sender={Sender},e={E})", sender, e);
            if (WindowState == WindowState.Normal)
            {
                WindowState = WindowState.Maximized;
                MaximizeRestoreButton.Content = "\uE923";
				_logger.LogDebug("MaximizeRestoreButton_Click(sender={Sender},e={E}): WindowState → {WindowState}", sender, e, WindowState);
				_logger.LogDebug("MaximizeRestoreButton_Click(sender={Sender},e={E}): MaximizeRestoreButton.Content → {Arg2}", sender, e, MaximizeRestoreButton.Content);
            }
            else if (WindowState == WindowState.Maximized)
            {
                WindowState = WindowState.Normal;
                MaximizeRestoreButton.Content = "\uE922";
            }
			_logger.LogDebug("<<< Exiting MaximizeRestoreButton_Click(sender={Sender},e={E})", sender, e);
        }

        private void MinimizeButton_Click(object sender, RoutedEventArgs e)
        {
			_logger.LogDebug(">>> Entering MinimizeButton_Click(sender={Sender},e={E})", sender, e);
            WindowState = WindowState.Minimized;
			_logger.LogDebug("MinimizeButton_Click(sender={Sender},e={E}): WindowState → {WindowState}", sender, e, WindowState);
			_logger.LogDebug("<<< Exiting MinimizeButton_Click(sender={Sender},e={E})", sender, e);
        }

        private void CloseButton_Click(object sender, RoutedEventArgs e)
        {
			_logger.LogDebug(">>> Entering CloseButton_Click(sender={Sender},e={E})", sender, e);
            this.Close();
			_logger.LogDebug("<<< Exiting CloseButton_Click(sender={Sender},e={E})", sender, e);
        }
    }
}
