//----------------------------------------------------------------------------------------------------------------------
// <copyright file="ColorsTab.cs" company="Microsoft">
//     Copyright (c) Microsoft Corporation.  All rights reserved.
// </copyright>
// <summary>Wrapper and helper for instantiating and interacting with the Colors tab of the properties dialog.</summary>
//----------------------------------------------------------------------------------------------------------------------
namespace Conhost.UIA.Tests.Elements
{
    using System;
    using System.Collections.Generic;

    using Conhost.UIA.Tests.Common;
    using NativeMethods = Conhost.UIA.Tests.Common.NativeMethods;
    using OpenQA.Selenium.Appium;
	using Microsoft.Extensions.Logging;

    public class ColorsTab : TabBase
    {
		private static readonly Microsoft.Extensions.Logging.ILogger _logger = Microsoft.Extensions.Logging.Abstractions.NullLogger.Instance;
        public SliderMeta OpacitySlider { get; private set; }

        public ColorsTab(PropertiesDialog propDialog) : base(propDialog, " Colors ")
        {
		_logger.LogDebug(">>> Entering ColorsTab(propDialog={PropDialog})", propDialog);
		_logger.LogDebug("<<< Exiting ColorsTab(propDialog={PropDialog})", propDialog);
        }

        protected override void PopulateItemsOnNavigate(AppiumWebElement propWindow)
        {
			_logger.LogDebug(">>> Entering PopulateItemsOnNavigate(propWindow={PropWindow})", propWindow);
            var slider = propWindow.FindElementByClassName("msctls_trackbar32");

			_logger.LogDebug("PopulateItemsOnNavigate(propWindow={PropWindow}): slider → {Slider}", propWindow, slider);
            this.OpacitySlider = new SliderMeta(slider, "WindowAlpha", true, NativeMethods.WinConP.PKEY_Console_WindowTransparency);
			_logger.LogDebug("<<< Exiting PopulateItemsOnNavigate(propWindow={PropWindow})", propWindow);
        }

        public override IEnumerable<AppiumWebElement> GetObjectsDisabledForV1Console()
        {
			_logger.LogDebug(">>> Entering GetObjectsDisabledForV1Console()");
			_logger.LogDebug("<<< Exiting GetObjectsDisabledForV1Console()");
            return new AppiumWebElement[] { this.OpacitySlider.Slider };
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
            return new SliderMeta[] { this.OpacitySlider };
        }
    }
}