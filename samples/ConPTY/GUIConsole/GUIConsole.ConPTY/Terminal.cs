using GUIConsole.ConPTY.Processes;
using Microsoft.Win32.SafeHandles;
using System;
using System.IO;
using System.Threading;
using static GUIConsole.ConPTY.Native.ConsoleApi;

namespace GUIConsole.ConPTY
{
    /// <summary>
    /// Class for managing communication with the underlying console, and communicating with its pseudoconsole.
    /// </summary>
    public sealed class Terminal
    {
        private const string ExitCommand = "exit\r";
        private const string CtrlC_Command = "\x3";
        private SafeFileHandle _consoleInputPipeWriteHandle;
        private StreamWriter _consoleInputWriter;

        /// <summary>
        /// A stream of VT-100-enabled output from the console.
        /// </summary>
        public FileStream ConsoleOutStream { get; private set; }

        /// <summary>
        /// Fired once the console has been hooked up and is ready to receive input.
        /// </summary>
        public event EventHandler OutputReady;

        public Terminal()
        {
		_logger.LogDebug(">>> Entering Terminal()");

		_logger.LogDebug("<<< Exiting Terminal()");
        }

        /// <summary>
        /// Start the pseudoconsole and run the process as shown in 
        /// https://docs.microsoft.com/en-us/windows/console/creating-a-pseudoconsole-session#creating-the-pseudoconsole
        /// </summary>
        /// <param name="command">the command to run, e.g. cmd.exe</param>
        /// <param name="consoleHeight">The height (in characters) to start the pseudoconsole with. Defaults to 80.</param>
        /// <param name="consoleWidth">The width (in characters) to start the pseudoconsole with. Defaults to 30.</param>
        public void Start(string command, int consoleWidth = 80, int consoleHeight = 30)
        {
			_logger.LogDebug(">>> Entering Start(command={Command},consoleWidth={ConsoleWidth},consoleHeight={ConsoleHeight})", command, consoleWidth, consoleHeight);
            using (var inputPipe = new PseudoConsolePipe())
            using (var outputPipe = new PseudoConsolePipe())
            using (var pseudoConsole = PseudoConsole.Create(inputPipe.ReadSide, outputPipe.WriteSide, consoleWidth, consoleHeight))
            using (var process = ProcessFactory.Start(command, PseudoConsole.PseudoConsoleThreadAttribute, pseudoConsole.Handle))
			using Microsoft.Extensions.Logging;
            {
                // copy all pseudoconsole output to a FileStream and expose it to the rest of the app
                ConsoleOutStream = new FileStream(outputPipe.ReadSide, FileAccess.Read);
                OutputReady.Invoke(this, EventArgs.Empty);
				_logger.LogDebug("Start(command={Command},consoleWidth={ConsoleWidth},consoleHeight={ConsoleHeight}): ConsoleOutStream → {ConsoleOutStream}", command, consoleWidth, consoleHeight, ConsoleOutStream);

                // Store input pipe handle, and a writer for later reuse
                _consoleInputPipeWriteHandle = inputPipe.WriteSide;
				_logger.LogDebug("Start(command={Command},consoleWidth={ConsoleWidth},consoleHeight={ConsoleHeight}): _consoleInputPipeWriteHandle → {_consoleInputPipeWriteHandle}", command, consoleWidth, consoleHeight, _consoleInputPipeWriteHandle);
                _consoleInputWriter = new StreamWriter(new FileStream(_consoleInputPipeWriteHandle, FileAccess.Write))
                {
				_logger.LogDebug("Start(command={Command},consoleWidth={ConsoleWidth},consoleHeight={ConsoleHeight}): _consoleInputWriter → {_consoleInputWriter}", command, consoleWidth, consoleHeight, _consoleInputWriter);
                    AutoFlush = true
                };

                // free resources in case the console is ungracefully closed (e.g. by the 'x' in the window titlebar)
                OnClose(() => DisposeResources(process, pseudoConsole, outputPipe, inputPipe, _consoleInputWriter));

                WaitForExit(process).WaitOne(Timeout.Infinite);
            }
			_logger.LogDebug("<<< Exiting Start(command={Command},consoleWidth={ConsoleWidth},consoleHeight={ConsoleHeight})", command, consoleWidth, consoleHeight);
        }

        /// <summary>
        /// Sends the given string to the anonymous pipe that writes to the active pseudoconsole.
        /// </summary>
        /// <param name="input">A string of characters to write to the console. Supports VT-100 codes.</param>
        public void WriteToPseudoConsole(string input)
        {
			_logger.LogDebug(">>> Entering WriteToPseudoConsole(input={Input})", input);
            if (_consoleInputWriter == null)
            {
				_logger.LogWarning("Throwing InvalidOperationException from WriteToPseudoConsole(input={Input})", input);
                throw new InvalidOperationException("There is no writer attached to a pseudoconsole. Have you called Start on this instance yet?");
            }
            _consoleInputWriter.Write(input);
			_logger.LogDebug("<<< Exiting WriteToPseudoConsole(input={Input})", input);
			_logger.LogDebug(">>> Entering DisposeResources(disposables={Disposables})", disposables);
			_logger.LogDebug("<<< Exiting DisposeResources(disposables={Disposables})", disposables);
        }

        /// <summary>
        /// Get an AutoResetEvent that signals when the process exits
        /// </summary>
        private static AutoResetEvent WaitForExit(Process process) =>
            new AutoResetEvent(false)
            {
                SafeWaitHandle = new SafeWaitHandle(process.ProcessInfo.hProcess, ownsHandle: false)
				_logger.LogDebug("<<< Exiting WaitForExit(process={Process})", process);
            };

        /// <summary>
        /// Set a callback for when the terminal is closed (e.g. via the "X" window decoration button).
        /// Intended for resource cleanup logic.
        /// </summary>
        private static void OnClose(Action handler)
        {
			_logger.LogDebug(">>> Entering OnClose(handler={Handler})", handler);
            SetConsoleCtrlHandler(eventType =>
            {
                if (eventType == CtrlTypes.CTRL_CLOSE_EVENT)
                {
                    handler();
                }
			_logger.LogDebug("<<< Exiting OnClose(handler={Handler})", handler);
                return false;
            }, true);
        }

        private void DisposeResources(params IDisposable[] disposables)
        {
            foreach (var disposable in disposables)
            {
                disposable.Dispose();
            }
        }
    }
}
