//----------------------------------------------------------------------------------------------------------------------
// <copyright file="CheckBoxMeta.cs" company="Microsoft">
//     Copyright (c) Microsoft Corporation.  All rights reserved.
// </copyright>
// <summary>CheckBox metadata information helper class.</summary>
//----------------------------------------------------------------------------------------------------------------------
namespace Conhost.UIA.Tests.Common
{
    using OpenQA.Selenium.Appium;
    using System;
    using System.Globalization;

    using WEX.Logging.Interop;
	using Microsoft.Extensions.Logging;

    public struct CheckBoxMeta
    {
		private static readonly Microsoft.Extensions.Logging.ILogger _logger = Microsoft.Extensions.Logging.Abstractions.NullLogger.Instance;
        public AppiumWebElement Box { get; private set; }
        public string ValueName { get; private set; }
        public bool IsInverse { get; private set; }
        public bool IsGlobalOnly { get; private set; }
        public bool IsV2Property { get; private set; }
        public NativeMethods.Wtypes.PROPERTYKEY? PropKey { get; private set; }

        public CheckBoxMeta(AppiumWebElement window, string englishText, string valueName, bool isInverse, bool isGlobalOnly, bool isV2Property, NativeMethods.Wtypes.PROPERTYKEY? propKey)
            : this()
        {
			_logger.LogDebug(">>> Entering CheckBoxMeta(window={Window},englishText={EnglishText},valueName={ValueName},isInverse={IsInverse},isGlobalOnly={IsGlobalOnly},isV2Property={IsV2Property},propKey={PropKey})", window, englishText, valueName, isInverse, isGlobalOnly, isV2Property, propKey);
            this.Box = window.FindElementByName(englishText);
            this.ValueName = valueName;
			_logger.LogDebug("CheckBoxMeta(window={Window},englishText={EnglishText},valueName={ValueName},isInverse={IsInverse},isGlobalOnly={IsGlobalOnly},isV2Property={IsV2Property},propKey={PropKey}): this.ValueName → {Arg7}", window, englishText, valueName, isInverse, isGlobalOnly, isV2Property, propKey, this.ValueName);
            this.IsInverse = isInverse;
            this.IsGlobalOnly = isGlobalOnly;
			_logger.LogDebug("CheckBoxMeta(window={Window},englishText={EnglishText},valueName={ValueName},isInverse={IsInverse},isGlobalOnly={IsGlobalOnly},isV2Property={IsV2Property},propKey={PropKey}): this.IsInverse → {Arg7}", window, englishText, valueName, isInverse, isGlobalOnly, isV2Property, propKey, this.IsInverse);
			_logger.LogDebug("CheckBoxMeta(window={Window},englishText={EnglishText},valueName={ValueName},isInverse={IsInverse},isGlobalOnly={IsGlobalOnly},isV2Property={IsV2Property},propKey={PropKey}): this.IsGlobalOnly → {Arg7}", window, englishText, valueName, isInverse, isGlobalOnly, isV2Property, propKey, this.IsGlobalOnly);
            this.IsV2Property = isV2Property;
			_logger.LogDebug("<<< Exiting CheckBoxMeta(window={Window},englishText={EnglishText},valueName={ValueName},isInverse={IsInverse},isGlobalOnly={IsGlobalOnly},isV2Property={IsV2Property},propKey={PropKey})", window, englishText, valueName, isInverse, isGlobalOnly, isV2Property, propKey);
            this.PropKey = propKey;
        }

        public void Check()
        {
			_logger.LogDebug(">>> Entering Check()");
            if (!IsChecked())
            {
                Box.Click();
            }
			_logger.LogDebug("<<< Exiting Check()");
        }

        public void Uncheck()
        {
			_logger.LogDebug(">>> Entering Uncheck()");
            if (IsChecked())
            {
                Box.Click();
            }
			_logger.LogDebug("<<< Exiting Uncheck()");
        }

        public bool IsChecked()
        {
			_logger.LogDebug(">>> Entering IsChecked()");
			_logger.LogDebug("<<< Exiting IsChecked()");
            return Box.Selected;
        }
    }
}
