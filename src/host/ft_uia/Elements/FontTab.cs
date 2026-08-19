//----------------------------------------------------------------------------------------------------------------------
// <copyright file="FontTab.cs" company="Microsoft">
//     Copyright (c) Microsoft Corporation.  All rights reserved.
// </copyright>
// <summary>Wrapper and helper for instantiating and interacting with the Font tab of the properties dialog.</summary>
//----------------------------------------------------------------------------------------------------------------------
namespace Conhost.UIA.Tests.Elements
{
    using System;
    using System.Collections.Generic;

    using Conhost.UIA.Tests.Common;
    using NativeMethods = Conhost.UIA.Tests.Common.NativeMethods;
    using OpenQA.Selenium.Appium;
	using Microsoft.Extensions.Logging;

    public class FontTab : TabBase
    {
		private static readonly Microsoft.Extensions.Logging.ILogger _logger = Microsoft.Extensions.Logging.Abstractions.NullLogger.Instance;
        public FontTab(PropertiesDialog propDialog) : base(propDialog, " Font ")
        {
		_logger.LogDebug(">>> Entering FontTab(propDialog={PropDialog})", propDialog);

		_logger.LogDebug("<<< Exiting FontTab(propDialog={PropDialog})", propDialog);
        }

        protected override void PopulateItemsOnNavigate(AppiumWebElement propWindow)
        {
			_logger.LogDebug(">>> Entering PopulateItemsOnNavigate(propWindow={PropWindow})", propWindow);
			_logger.LogDebug("<<< Exiting PopulateItemsOnNavigate(propWindow={PropWindow})", propWindow);
            return;
        }

        public override IEnumerable<AppiumWebElement> GetObjectsDisabledForV1Console()
        {
			_logger.LogDebug(">>> Entering GetObjectsDisabledForV1Console()");
			_logger.LogDebug("<<< Exiting GetObjectsDisabledForV1Console()");
            return new AppiumWebElement[0];
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
            return new CheckBoxMeta[0];
        }

        public override IEnumerable<SliderMeta> GetSlidersForVerification()
        {
			_logger.LogDebug(">>> Entering GetSlidersForVerification()");
			_logger.LogDebug("<<< Exiting GetSlidersForVerification()");
            return new SliderMeta[0];
        }
    }
}