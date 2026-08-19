//----------------------------------------------------------------------------------------------------------------------
// <copyright file="TabBase.cs" company="Microsoft">
//     Copyright (c) Microsoft Corporation.  All rights reserved.
// </copyright>
// <summary>Wrapper and helper for instantiating various tabs of the properties dialog.</summary>
//----------------------------------------------------------------------------------------------------------------------
namespace Conhost.UIA.Tests.Elements
{
    using System;
    using System.Collections.Generic;

    using Conhost.UIA.Tests.Common;
    using OpenQA.Selenium.Appium;
	using Microsoft.Extensions.Logging;

    public abstract class TabBase : IDisposable
    {
		private static readonly Microsoft.Extensions.Logging.ILogger _logger = Microsoft.Extensions.Logging.Abstractions.NullLogger.Instance;
        protected string tabName;

        protected PropertiesDialog propDialog;

        private TabBase()
        {
		_logger.LogDebug(">>> Entering TabBase()");

		_logger.LogDebug("<<< Exiting TabBase()");
        }

        public TabBase(PropertiesDialog propDialog, string tabName)
        {
			_logger.LogDebug(">>> Entering TabBase(propDialog={PropDialog},tabName={TabName})", propDialog, tabName);
            this.propDialog = propDialog;
            this.tabName = tabName;
			_logger.LogDebug("TabBase(propDialog={PropDialog},tabName={TabName}): this.propDialog → {Arg2}", propDialog, tabName, this.propDialog);
			_logger.LogDebug("<<< Exiting TabBase(propDialog={PropDialog},tabName={TabName})", propDialog, tabName);
        }

        ~TabBase()
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

        public void NavigateToTab()
        {
			_logger.LogDebug(">>> Entering NavigateToTab()");
            AutoHelpers.LogInvariant("Navigating to '{0}'", this.tabName);
            var tab = this.propDialog.Tabs.FindElementByName(this.tabName);
			_logger.LogDebug("NavigateToTab(): tab → {Tab}", tab);

            tab.Click();
            Globals.WaitForTimeout();

			_logger.LogDebug("<<< Exiting NavigateToTab()");
            this.PopulateItemsOnNavigate(this.propDialog.PropWindow);

        }

        protected abstract void PopulateItemsOnNavigate(AppiumWebElement propWindow);

        public abstract IEnumerable<AppiumWebElement> GetObjectsDisabledForV1Console();
        public abstract IEnumerable<AppiumWebElement> GetObjectsUnaffectedByV1V2Switch();

        public abstract IEnumerable<CheckBoxMeta> GetCheckboxesForVerification();

        public abstract IEnumerable<SliderMeta> GetSlidersForVerification();
    }
}