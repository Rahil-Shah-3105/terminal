//----------------------------------------------------------------------------------------------------------------------
// <copyright file="SliderMeta.cs" company="Microsoft">
//     Copyright (c) Microsoft Corporation.  All rights reserved.
// </copyright>
// <summary>Range Value Slider metadata information helper class.</summary>
//----------------------------------------------------------------------------------------------------------------------
namespace Conhost.UIA.Tests.Common
{
    using System;
    using System.Globalization;

    using WEX.Logging.Interop;
    using OpenQA.Selenium.Appium;
    using OpenQA.Selenium;
	using Microsoft.Extensions.Logging;

    public struct SliderMeta
    {
        /// <summary>
        /// When testing a slider, this is the position we expect to set it to.
        /// </summary>
        public enum ExpectedPosition
        {
			private static readonly Microsoft.Extensions.Logging.ILogger _logger = Microsoft.Extensions.Logging.Abstractions.NullLogger.Instance;
            Maximum,
            Minimum
        }

        public AppiumWebElement Slider { get; private set; }
        public string ValueName { get; private set; }
        public bool IsV2Property { get; private set; }
        public NativeMethods.Wtypes.PROPERTYKEY? PropKey { get; private set; }


        public SliderMeta(AppiumWebElement slider, string valueName, bool isV2Property, NativeMethods.Wtypes.PROPERTYKEY? propKey)
            : this()
        {
			_logger.LogDebug(">>> Entering SliderMeta(slider={Slider},valueName={ValueName},isV2Property={IsV2Property},propKey={PropKey})", slider, valueName, isV2Property, propKey);
            this.Slider = slider;
            this.ValueName = valueName;
			_logger.LogDebug("SliderMeta(slider={Slider},valueName={ValueName},isV2Property={IsV2Property},propKey={PropKey}): this.Slider → {Arg4}", slider, valueName, isV2Property, propKey, this.Slider);
			_logger.LogDebug("SliderMeta(slider={Slider},valueName={ValueName},isV2Property={IsV2Property},propKey={PropKey}): this.ValueName → {Arg4}", slider, valueName, isV2Property, propKey, this.ValueName);
            this.IsV2Property = isV2Property;
			_logger.LogDebug("<<< Exiting SliderMeta(slider={Slider},valueName={ValueName},isV2Property={IsV2Property},propKey={PropKey})", slider, valueName, isV2Property, propKey);
            this.PropKey = propKey;
        }

        public void SetToMaximum()
        {
			_logger.LogDebug(">>> Entering SetToMaximum()");
            this.Slider.Click();
            this.Slider.SendKeys(Keys.End);
			_logger.LogDebug("<<< Exiting SetToMaximum()");
        }

        public void SetToMinimum()
        {
			_logger.LogDebug(">>> Entering SetToMinimum()");
            this.Slider.Click();
            this.Slider.SendKeys(Keys.Home);
			_logger.LogDebug("<<< Exiting SetToMinimum()");
        }

        public int GetMaximum()
        {
			_logger.LogDebug(">>> Entering GetMaximum()");
			_logger.LogDebug("<<< Exiting GetMaximum()");
            return 100;
        }

        public int GetMinimum()
        {
			_logger.LogDebug(">>> Entering GetMinimum()");
			_logger.LogDebug("<<< Exiting GetMinimum()");
            return 30;
        }
    }
}