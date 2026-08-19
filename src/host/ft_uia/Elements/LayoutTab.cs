//----------------------------------------------------------------------------------------------------------------------
// <copyright file="LayoutTab.cs" company="Microsoft">
//     Copyright (c) Microsoft Corporation.  All rights reserved.
// </copyright>
// <summary>Wrapper and helper for instantiating and interacting with the Layout tab of the properties dialog.</summary>
//----------------------------------------------------------------------------------------------------------------------
namespace Conhost.UIA.Tests.Elements
{
    using System;
    using System.Collections.Generic;
    using System.Linq;

    using Conhost.UIA.Tests.Common;
    using NativeMethods = Conhost.UIA.Tests.Common.NativeMethods;
    using OpenQA.Selenium.Appium;
	using Microsoft.Extensions.Logging;

    public class LayoutTab : TabBase
    {
		private static readonly Microsoft.Extensions.Logging.ILogger _logger = Microsoft.Extensions.Logging.Abstractions.NullLogger.Instance;
        public List<CheckBoxMeta> Checkboxes { get; private set; }
        public CheckBoxMeta WrapTextCheckBox { get; private set; }

        public LayoutTab(PropertiesDialog propDialog) : base(propDialog, " Layout ")
        {
		_logger.LogDebug(">>> Entering LayoutTab(propDialog={PropDialog})", propDialog);
		_logger.LogDebug("<<< Exiting LayoutTab(propDialog={PropDialog})", propDialog);
        }

        protected override void PopulateItemsOnNavigate(AppiumWebElement propWindow)
        {
			_logger.LogDebug(">>> Entering PopulateItemsOnNavigate(propWindow={PropWindow})", propWindow);
            this.Checkboxes = new List<CheckBoxMeta>();
            this.WrapTextCheckBox = new CheckBoxMeta(propWindow, "Wrap text output on resize", "LineWrap", false, false, true, NativeMethods.WinConP.PKEY_Console_WrapText);
            this.Checkboxes.Add(this.WrapTextCheckBox);
			_logger.LogDebug("<<< Exiting PopulateItemsOnNavigate(propWindow={PropWindow})", propWindow);
        }

        public override IEnumerable<AppiumWebElement> GetObjectsDisabledForV1Console()
        {
			_logger.LogDebug(">>> Entering GetObjectsDisabledForV1Console()");
			_logger.LogDebug("<<< Exiting GetObjectsDisabledForV1Console()");
            return this.Checkboxes.Select(meta => meta.Box);
        }

        public override IEnumerable<AppiumWebElement> GetObjectsUnaffectedByV1V2Switch()
        {
			_logger.LogDebug(">>> Entering GetObjectsUnaffectedByV1V2Switch()");
			_logger.LogDebug("<<< Exiting GetObjectsUnaffectedByV1V2Switch()");
            return new AppiumWebElement[0];
        }

        public override IEnumerable<CheckBoxMeta> GetCheckboxesForVerification()
        {
			_logger.LogDebug(">>> Entering GetCheckboxesForVerification()");
			_logger.LogDebug("<<< Exiting GetCheckboxesForVerification()");
            return this.Checkboxes;
        }

        public override IEnumerable<SliderMeta> GetSlidersForVerification()
        {
			_logger.LogDebug(">>> Entering GetSlidersForVerification()");
			_logger.LogDebug("<<< Exiting GetSlidersForVerification()");
            return new SliderMeta[0];
        }
    }
}