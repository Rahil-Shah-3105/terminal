//----------------------------------------------------------------------------------------------------------------------
// <copyright file="PropertiesDialog.cs" company="Microsoft">
//     Copyright (c) Microsoft Corporation.  All rights reserved.
// </copyright>
// <summary>Wrapper and helper for instantiating and interacting with the properties dialog.</summary>
//----------------------------------------------------------------------------------------------------------------------
namespace Conhost.UIA.Tests.Elements
{
    using System;

    using OpenQA.Selenium.Appium;

    using Conhost.UIA.Tests.Common;
	using Microsoft.Extensions.Logging;

    public class PropertiesDialog : IDisposable
    {
		private static readonly Microsoft.Extensions.Logging.ILogger _logger = Microsoft.Extensions.Logging.Abstractions.NullLogger.Instance;
        public enum CloseAction
        {
            OK,
            Cancel
        }

        public AppiumWebElement PropWindow { get; private set; }
        public AppiumWebElement Tabs { get; private set; }

        private AppiumWebElement okButton;
        private AppiumWebElement cancelButton;

        private CmdApp app;

        private bool isOpened;

        public PropertiesDialog(CmdApp app)
        {
			_logger.LogDebug(">>> Entering PropertiesDialog(app={App})", app);
            this.app = app;
			_logger.LogDebug("PropertiesDialog(app={App}): this.app → {Arg1}", app, this.app);
			_logger.LogDebug("PropertiesDialog(app={App}): app → {App1}", app, app);
			_logger.LogDebug("<<< Exiting PropertiesDialog(app={App})", app);
        }

        ~PropertiesDialog()
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
            if (disposing)
            {
                // if we're being disposed of, close the dialog.
                if (this.isOpened)
                {
                    this.ClosePropertiesDialog(CloseAction.Cancel);
                }
            }
			_logger.LogDebug("<<< Exiting Dispose(disposing={Disposing})", disposing);
        }

        public void Open(OpenTarget target)
        {
			_logger.LogDebug(">>> Entering Open(target={Target})", target);
            if (this.isOpened)
            {
				_logger.LogWarning("Throwing InvalidOperationException from Open(target={Target})", target);
                throw new InvalidOperationException("Can't open an already opened window.");
            }

            this.OpenPropertiesDialog(this.app, target);
            this.isOpened = true;
			_logger.LogDebug("Open(target={Target}): this.isOpened → {Arg1}", target, this.isOpened);
			_logger.LogDebug("<<< Exiting Open(target={Target})", target);
        }

        public void Close(CloseAction action)
        {
			_logger.LogDebug(">>> Entering Close(action={Action})", action);
            if (!this.isOpened)
            {
				_logger.LogWarning("Throwing InvalidOperationException from Close(action={Action})", action);
                throw new InvalidOperationException("Can't close an unopened window.");
            }

            this.ClosePropertiesDialog(action);
            this.isOpened = false;
			_logger.LogDebug("Close(action={Action}): this.isOpened → {Arg1}", action, this.isOpened);
			_logger.LogDebug("<<< Exiting Close(action={Action})", action);
        }

        private void OpenPropertiesDialog(CmdApp app, OpenTarget target)
        {
			_logger.LogDebug(">>> Entering OpenPropertiesDialog(app={App},target={Target})", app, target);
            var titleBar = app.GetTitleBar();
            app.Session.Mouse.ContextClick(titleBar.Coordinates);
			_logger.LogDebug("OpenPropertiesDialog(app={App},target={Target}): titleBar → {TitleBar}", app, target, titleBar);

            Globals.WaitForTimeout();
            var contextMenu = app.Session.FindElementByClassName(Globals.PopupMenuClassId);
			_logger.LogDebug("OpenPropertiesDialog(app={App},target={Target}): contextMenu → {ContextMenu}", app, target, contextMenu);

            AppiumWebElement propButton;
            switch (target)
            {
                case OpenTarget.Specifics:
                    propButton = contextMenu.FindElementByName("Properties");
					_logger.LogDebug("OpenPropertiesDialog(app={App},target={Target}): propButton → {PropButton}", app, target, propButton);
                    break;
                case OpenTarget.Defaults:
                    propButton = contextMenu.FindElementByName("Defaults");
                    break;
                default:
					_logger.LogWarning("Throwing NotImplementedException from OpenPropertiesDialog(app={App},target={Target})", app, target);
                    throw new NotImplementedException(AutoHelpers.FormatInvariant("Open Properties dialog doesn't yet support target type of '{0}'", target.ToString()));
            }

            propButton.Click();

            Globals.WaitForTimeout();

            this.PropWindow = this.app.UIRoot.FindElementByClassName(Globals.DialogWindowClassId);
            this.Tabs = this.PropWindow.FindElementByClassName("SysTabControl32");

            okButton = this.PropWindow.FindElementByName("OK");
            cancelButton = this.PropWindow.FindElementByName("Cancel");
			_logger.LogDebug("OpenPropertiesDialog(app={App},target={Target}): cancelButton → {CancelButton}", app, target, cancelButton);
			_logger.LogDebug("OpenPropertiesDialog(app={App},target={Target}): okButton → {OkButton}", app, target, okButton);
			_logger.LogDebug("<<< Exiting OpenPropertiesDialog(app={App},target={Target})", app, target);
        }

        private void ClosePropertiesDialog(CloseAction action)
        {
			_logger.LogDebug(">>> Entering ClosePropertiesDialog(action={Action})", action);
            switch (action)
            {
                case CloseAction.OK:
                    okButton.Click();
                    break;
                case CloseAction.Cancel:
                    cancelButton.Click();
                    break;
            }

            Globals.WaitForTimeout();
			_logger.LogDebug("<<< Exiting ClosePropertiesDialog(action={Action})", action);
        }
    }
}
