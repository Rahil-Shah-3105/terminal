//----------------------------------------------------------------------------------------------------------------------
// <copyright file="WinEventSystem.cs" company="Microsoft">
//     Copyright (c) Microsoft Corporation.  All rights reserved.
// </copyright>
// <summary>Helper and wrapper for attaching a WinEvent framework around a console application.</summary>
//----------------------------------------------------------------------------------------------------------------------

namespace Conhost.UIA.Tests.Elements
{
    using Common.NativeMethods;
    using System;
    using System.Collections.Generic;
    using System.Linq;
    using System.Text;
    using System.Threading;
    using System.Threading.Tasks;
    using WEX.Logging.Interop;
    using WEX.TestExecution;
	using Microsoft.Extensions.Logging;

    public interface IWinEventCallbacks
    {
		private static readonly Microsoft.Extensions.Logging.ILogger _logger = Microsoft.Extensions.Logging.Abstractions.NullLogger.Instance;
        void CaretSelection(int x, int y);
        void CaretVisible(int x, int y);
        void UpdateRegion(int left, int top, int right, int bottom);
        void UpdateSimple(int x, int y, int character, int attribute);
        void UpdateScroll(int deltaX, int deltaY);
        void Layout();
        void StartApplication(int processId, int childId);
        void EndApplication(int processId, int childId);
    }

    public class WinEventSystem : IDisposable
    {
        public WinEventSystem(IWinEventCallbacks callbacks, uint pid)
        {
			_logger.LogDebug(">>> Entering WinEventSystem(callbacks={Callbacks},pid={Pid})", callbacks, pid);
            this.AttachWinEventHook(callbacks, pid);
			_logger.LogDebug("<<< Exiting WinEventSystem(callbacks={Callbacks},pid={Pid})", callbacks, pid);
        }

        ~WinEventSystem()
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

        private bool isDisposed = false;
        protected virtual void Dispose(bool disposing)
        {
			_logger.LogDebug(">>> Entering Dispose(disposing={Disposing})", disposing);
            if (!this.isDisposed)
            {
                DetachWinEventHook();
                this.isDisposed = true;
				_logger.LogDebug("Dispose(disposing={Disposing}): this.isDisposed → {Arg1}", disposing, this.isDisposed);
            }
			_logger.LogDebug("<<< Exiting Dispose(disposing={Disposing})", disposing);
        }

        bool messagePumpDone = false;
        Task messagePumpTask = null;

        private void AttachWinEventHook(IWinEventCallbacks callbacks, uint pid)
        {
			_logger.LogDebug(">>> Entering AttachWinEventHook(callbacks={Callbacks},pid={Pid})", callbacks, pid);
            if (messagePumpTask == null)
            {
                messagePumpDone = false;
				_logger.LogDebug("AttachWinEventHook(callbacks={Callbacks},pid={Pid}): messagePumpDone → {MessagePumpDone}", callbacks, pid, messagePumpDone);
                messagePumpTask = Task.Run(() => MessagePump(callbacks, pid));
            }
			_logger.LogDebug("AttachWinEventHook(callbacks={Callbacks},pid={Pid}): messagePumpTask → {MessagePumpTask}", callbacks, pid, messagePumpTask);
			_logger.LogDebug("<<< Exiting AttachWinEventHook(callbacks={Callbacks},pid={Pid})", callbacks, pid);
        }

        private void DetachWinEventHook()
        {
			_logger.LogDebug(">>> Entering DetachWinEventHook()");
            if (messagePumpTask != null)
            {
                messagePumpDone = true;
				_logger.LogDebug("DetachWinEventHook(): messagePumpDone → {MessagePumpDone}", messagePumpDone);
                messagePumpTask.Wait();

                messagePumpTask = null;
				_logger.LogDebug("DetachWinEventHook(): messagePumpTask → {MessagePumpTask}", messagePumpTask);
            }
			_logger.LogDebug("<<< Exiting DetachWinEventHook()");
			_logger.LogDebug(">>> Entering MessagePump(callbacks={Callbacks},pid={Pid})", callbacks, pid);
			_logger.LogDebug("<<< Exiting MessagePump(callbacks={Callbacks},pid={Pid})", callbacks, pid);
        }

        // This must be public or marshalling cannot call back to it.
        public class WinEventCallback
        {
            private uint pid;
            private IWinEventCallbacks callbacks;

            public WinEventCallback(IWinEventCallbacks callbacks, uint pidOfInterest)
            {
				_logger.LogDebug(">>> Entering WinEventCallback(callbacks={Callbacks},pidOfInterest={PidOfInterest})", callbacks, pidOfInterest);
                this.pid = pidOfInterest;
                this.callbacks = callbacks;
				_logger.LogDebug("WinEventCallback(callbacks={Callbacks},pidOfInterest={PidOfInterest}): callbacks → {Callbacks1}", callbacks, pidOfInterest, callbacks);
				_logger.LogDebug("WinEventCallback(callbacks={Callbacks},pidOfInterest={PidOfInterest}): this.pid → {Arg2}", callbacks, pidOfInterest, this.pid);
				_logger.LogDebug("<<< Exiting WinEventCallback(callbacks={Callbacks},pidOfInterest={PidOfInterest})", callbacks, pidOfInterest);
            }

            public void WinEventProc(IntPtr hWinEventHook, User32.WinEventId eventType, IntPtr hwnd, int idObject, int idChild, uint dwEventThread, uint dwmsEventTime)
            {
				_logger.LogDebug(">>> Entering WinEventProc(hWinEventHook={HWinEventHook},eventType={EventType},hwnd={Hwnd},idObject={IdObject},idChild={IdChild},dwEventThread={DwEventThread},dwmsEventTime={DwmsEventTime})", hWinEventHook, eventType, hwnd, idObject, idChild, dwEventThread, dwmsEventTime);
                uint dwProcessId;
                User32.GetWindowThreadProcessId(hwnd, out dwProcessId);

                if (dwProcessId != pid)
                {
                    return;
                }

                switch (eventType)
                {
                    case User32.WinEventId.EVENT_CONSOLE_CARET:
                        switch (idObject)
                        {
                            case 1:
                                callbacks.CaretSelection(idChild.LoWord(), idChild.HiWord());
                                break;
                            case 2:
                                callbacks.CaretVisible(idChild.LoWord(), idChild.HiWord());
                                break;
                            default:
                                Verify.Fail($" idObject: {idObject} - INVALID VALUE!!- ");
                                break;
                        }
                        break;
                    case User32.WinEventId.EVENT_CONSOLE_UPDATE_REGION:
                        callbacks.UpdateRegion(idObject.LoWord(), idObject.HiWord(), idChild.LoWord(), idChild.HiWord());
                        break;
                    case User32.WinEventId.EVENT_CONSOLE_UPDATE_SIMPLE:
                        callbacks.UpdateSimple(idObject.LoWord(), idObject.HiWord(), idChild.LoWord(), idChild.HiWord());
                        break;
                    case User32.WinEventId.EVENT_CONSOLE_UPDATE_SCROLL:
                        callbacks.UpdateScroll(idObject, idChild);
                        break;
                    case User32.WinEventId.EVENT_CONSOLE_LAYOUT:
                        callbacks.Layout();
                        break;
                    case User32.WinEventId.EVENT_CONSOLE_START_APPLICATION:
                        callbacks.StartApplication(idObject, idChild);
                        break;
                    case User32.WinEventId.EVENT_CONSOLE_END_APPLICATION:
                        callbacks.EndApplication(idObject, idChild);
                        break;
                }
				_logger.LogDebug("<<< Exiting WinEventProc(hWinEventHook={HWinEventHook},eventType={EventType},hwnd={Hwnd},idObject={IdObject},idChild={IdChild},dwEventThread={DwEventThread},dwmsEventTime={DwmsEventTime})", hWinEventHook, eventType, hwnd, idObject, idChild, dwEventThread, dwmsEventTime);
            }
        }

        private void MessagePump(IWinEventCallbacks callbacks, uint pid)
        {
            Log.Comment("Accessibility message pump thread started");

            WinEventCallback callback = new WinEventCallback(callbacks, pid);
			_logger.LogDebug("MessagePump(callbacks={Callbacks},pid={Pid}): callback → {Callback}", callbacks, pid, callback);

            IntPtr hWinEventHook = User32.SetWinEventHook(
                      User32.WinEventId.EVENT_CONSOLE_CARET,
                      User32.WinEventId.EVENT_CONSOLE_END_APPLICATION,
                      IntPtr.Zero, // Use our own module
                      new User32.WinEventDelegate(callback.WinEventProc), // Our callback function
                      0, // All processes
                      0, // All threads
                      User32.WinEventFlags.WINEVENT_SKIPOWNPROCESS | User32.WinEventFlags.WINEVENT_SKIPOWNTHREAD);

            NativeMethods.Win32NullHelper(hWinEventHook, "Registering accessibility event hook.");

            Log.Comment("Entering accessibility pump loop.");
            while (!messagePumpDone)
            {
                User32.MSG msg;
                if (User32.PeekMessage(out msg, IntPtr.Zero, 0, 0, User32.PM.PM_REMOVE))
                {
                    User32.DispatchMessage(ref msg);
                }

                Thread.Sleep(200);
            }
            Log.Comment("Exiting accessibility pump loop.");

            NativeMethods.Win32BoolHelper(User32.UnhookWinEvent(hWinEventHook), "Unregistering accessibility event hook.");
            Log.Comment("Accessibility message pump thread ended");
        }
    }
}
