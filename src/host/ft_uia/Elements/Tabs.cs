//----------------------------------------------------------------------------------------------------------------------
// <copyright file="Tabs.cs" company="Microsoft">
//     Copyright (c) Microsoft Corporation.  All rights reserved.
// </copyright>
// <summary>Wrapper and helper for instantiating all known tabs of the properties dialog.</summary>
//----------------------------------------------------------------------------------------------------------------------
namespace Conhost.UIA.Tests.Elements
{
    using System;
    using System.Collections.Generic;
    using System.Linq;

    using Conhost.UIA.Tests.Common;
	using Microsoft.Extensions.Logging;

    public class Tabs : IDisposable
    {
		private static readonly Microsoft.Extensions.Logging.ILogger _logger = Microsoft.Extensions.Logging.Abstractions.NullLogger.Instance;
        protected PropertiesDialog propDialog;

        protected List<TabBase> tabs;

        public Tabs(PropertiesDialog propDialog)
        {
			_logger.LogDebug(">>> Entering Tabs(propDialog={PropDialog})", propDialog);
            this.propDialog = propDialog;

			_logger.LogDebug("Tabs(propDialog={PropDialog}): this.propDialog → {Arg1}", propDialog, this.propDialog);
			_logger.LogDebug("Tabs(propDialog={PropDialog}): propDialog → {PropDialog1}", propDialog, propDialog);
            this.InitializeAllTabs();
			_logger.LogDebug("<<< Exiting Tabs(propDialog={PropDialog})", propDialog);
			_logger.LogDebug(">>> Entering Tabs()");
			_logger.LogDebug("<<< Exiting Tabs()");
        }

        private void InitializeAllTabs()
        {
			_logger.LogDebug(">>> Entering InitializeAllTabs()");
            this.tabs = new List<TabBase>();

			_logger.LogDebug("InitializeAllTabs(): tabs → {Tabs}", tabs);
            this.tabs.Add(new OptionsTab(this.propDialog));
            this.tabs.Add(new FontTab(this.propDialog));
            this.tabs.Add(new LayoutTab(this.propDialog));
			_logger.LogDebug("<<< Exiting InitializeAllTabs()");
            this.tabs.Add(new ColorsTab(this.propDialog));

        }

        private Tabs()
        {

        }

        ~Tabs()
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


        public IEnumerable<TabBase> AllTabs
        {
            get
            {
                return this.tabs;
            }
        }

        public enum GlobalState
        {
            ConsoleV1,
            ConsoleV2
        }

        public void SetGlobalState(GlobalState state)
        {
			_logger.LogDebug(">>> Entering SetGlobalState(state={State})", state);
            AutoHelpers.LogInvariant("Updating global state to '{0}'", state.ToString());
            OptionsTab tabWithGlobal = this.tabs.Single(tab => typeof(OptionsTab) == tab.GetType()) as OptionsTab;
			_logger.LogDebug("SetGlobalState(state={State}): tabWithGlobal → {TabWithGlobal}", state, tabWithGlobal);
            tabWithGlobal.NavigateToTab();

            switch (state)
            {
                case GlobalState.ConsoleV1:
                    tabWithGlobal.GlobalV1V2Box.Check();
                    break;
                case GlobalState.ConsoleV2:
                    tabWithGlobal.GlobalV1V2Box.Uncheck();
                    break;
                default:
					_logger.LogWarning("Throwing NotImplementedException from SetGlobalState(state={State})", state);
			_logger.LogDebug("<<< Exiting SetGlobalState(state={State})", state);
                    throw new NotImplementedException();
            }
        }

        public void SetWrapState(bool isWrapOn)
        {
			_logger.LogDebug(">>> Entering SetWrapState(isWrapOn={IsWrapOn})", isWrapOn);
            AutoHelpers.LogInvariant("Updating wrap state to '{0}'", isWrapOn.ToString());
            LayoutTab tabWithWrap = this.tabs.Single(tab => typeof(LayoutTab) == tab.GetType()) as LayoutTab;
			_logger.LogDebug("SetWrapState(isWrapOn={IsWrapOn}): tabWithWrap → {TabWithWrap}", isWrapOn, tabWithWrap);
            tabWithWrap.NavigateToTab();

            if (isWrapOn)
            {
                tabWithWrap.WrapTextCheckBox.Check();
            }
            else
            {
                tabWithWrap.WrapTextCheckBox.Uncheck();
            }
			_logger.LogDebug("<<< Exiting SetWrapState(isWrapOn={IsWrapOn})", isWrapOn);
        }
    }
}