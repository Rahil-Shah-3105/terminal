//----------------------------------------------------------------------------------------------------------------------
// <copyright file="ExperimentalTabTests.cs" company="Microsoft">
// Copyright (c) Microsoft Corporation.
// Licensed under the MIT license.
// </copyright>
// <summary>UI Automation tests for the Experimental Tab control in the Console Properties dialog window.</summary>
//----------------------------------------------------------------------------------------------------------------------
namespace Conhost.UIA.Tests
{
    using System;
    using System.Collections.Generic;
    using System.Diagnostics;
    using System.IO;
    using System.Linq;
    using System.Runtime.InteropServices;
    using System.Threading;

    using Microsoft.Win32;

    using WEX.Common.Managed;
    using WEX.Logging.Interop;
    using WEX.TestExecution;
    using WEX.TestExecution.Markup;

    using Conhost.UIA.Tests.Common;
    using Conhost.UIA.Tests.Common.NativeMethods;
    using Conhost.UIA.Tests.Elements;
    using OpenQA.Selenium.Appium;
	using Microsoft.Extensions.Logging;

    [TestClass]
    public class ExperimentalTabTests
    {
		private static readonly Microsoft.Extensions.Logging.ILogger _logger = Microsoft.Extensions.Logging.Abstractions.NullLogger.Instance;
        public TestContext TestContext { get; set; }

        public const int timeout = Globals.Timeout;

        [TestMethod]
        [TestProperty("Ignore", "True")] // GH#7282 - investigate and reenable
        public void CheckExperimentalDisableState()
        {
			_logger.LogDebug(">>> Entering CheckExperimentalDisableState()");
            using (RegistryHelper reg = new RegistryHelper())
            {
                reg.BackupRegistry(); // manipulating the global v1/v2 state can affect the registry so back it up.

                using (CmdApp app = new CmdApp(CreateType.ProcessOnly, TestContext))
                {
                    using (PropertiesDialog properties = new PropertiesDialog(app))
                    {
                        properties.Open(OpenTarget.Defaults);

                        using (Tabs tabs = new Tabs(properties))
                        {
                            // check everything stays enabled when global is on.
                            AutoHelpers.LogInvariant("Check that items are all enabled when global is enabled.");
                            tabs.SetGlobalState(Tabs.GlobalState.ConsoleV2);

                            // iterate through each tab
                            AutoHelpers.LogInvariant("Checking elements on all tabs.");
                            foreach (TabBase tab in tabs.AllTabs)
                            {
                                tab.NavigateToTab();

                                IEnumerable<AppiumWebElement> itemsUnaffected = tab.GetObjectsUnaffectedByV1V2Switch();
                                IEnumerable<AppiumWebElement> itemsThatDisable = tab.GetObjectsDisabledForV1Console();

                                foreach (AppiumWebElement obj in itemsThatDisable.Concat(itemsUnaffected))
                                {
                                    Verify.IsTrue(obj.Enabled, AutoHelpers.FormatInvariant("Option: {0}", obj.Text));
                                }
                            }

                            // check that relevant boxes are disabled when global is off.
                            AutoHelpers.LogInvariant("Check that necessary items are disabled when global is disabled.");
                            tabs.SetGlobalState(Tabs.GlobalState.ConsoleV1);

                            foreach (TabBase tab in tabs.AllTabs)
                            {
                                tab.NavigateToTab();

                                IEnumerable<AppiumWebElement> itemsUnaffected = tab.GetObjectsUnaffectedByV1V2Switch();
                                IEnumerable<AppiumWebElement> itemsThatDisable = tab.GetObjectsDisabledForV1Console();

                                foreach (AppiumWebElement obj in itemsThatDisable)
                                {
                                    Verify.IsFalse(obj.Enabled, AutoHelpers.FormatInvariant("Option: {0}", obj.Text));
                                }
                                foreach (AppiumWebElement obj in itemsUnaffected)
                                {
                                    Verify.IsTrue(obj.Enabled, AutoHelpers.FormatInvariant("Option: {0}", obj.Text));
                                }
                            }
                        }
                    }
                }
            }
			_logger.LogDebug("<<< Exiting CheckExperimentalDisableState()");
        }

        [TestMethod]
        [TestProperty("Ignore", "True")] // GH#7282 - investigate and reenable
        public void CheckRegistryWritebacks()
        {
			_logger.LogDebug(">>> Entering CheckRegistryWritebacks()");
            using (RegistryHelper reg = new RegistryHelper())
            {
                reg.BackupRegistry();

                using (CmdApp app = new CmdApp(CreateType.ProcessOnly, TestContext))
                {
                    this.CheckRegistryWritebacks(reg, app, OpenTarget.Defaults);
                    this.CheckRegistryWritebacks(reg, app, OpenTarget.Specifics);
                }
            }
			_logger.LogDebug("<<< Exiting CheckRegistryWritebacks()");
        }

        [TestMethod]
        [TestProperty("Ignore", "True")] // GH#7282 - investigate and reenable
        public void CheckShortcutWritebacks()
        {
			_logger.LogDebug(">>> Entering CheckShortcutWritebacks()");
            using (RegistryHelper reg = new RegistryHelper())
            {
                // The global state changes can still impact the registry, so back up the registry anyway despite this being the shortcut test.
                reg.BackupRegistry();

                using (ShortcutHelper shortcut = new ShortcutHelper())
                {
                    shortcut.CreateTempCmdShortcut();

                    using (CmdApp app = new CmdApp(CreateType.ShortcutFile, TestContext, shortcut.ShortcutPath))
                    {
                        this.CheckShortcutWritebacks(shortcut, app, OpenTarget.Specifics);
                    }
                }
            }
			_logger.LogDebug("<<< Exiting CheckShortcutWritebacks()");
        }

        private void CheckRegistryWritebacks(RegistryHelper reg, CmdApp app, OpenTarget target)
        {
			_logger.LogDebug(">>> Entering CheckRegistryWritebacks(reg={Reg},app={App},target={Target})", reg, app, target);
            this.CheckWritebacks(reg, null, app, target);
			_logger.LogDebug("<<< Exiting CheckRegistryWritebacks(reg={Reg},app={App},target={Target})", reg, app, target);
        }

        private void CheckShortcutWritebacks(ShortcutHelper shortcut, CmdApp app, OpenTarget target)
        {
			_logger.LogDebug(">>> Entering CheckShortcutWritebacks(shortcut={Shortcut},app={App},target={Target})", shortcut, app, target);
            this.CheckWritebacks(null, shortcut, app, target);
			_logger.LogDebug("<<< Exiting CheckShortcutWritebacks(shortcut={Shortcut},app={App},target={Target})", shortcut, app, target);
        }

        private void CheckWritebacks(RegistryHelper reg, ShortcutHelper shortcut, CmdApp app, OpenTarget target)
        {
			_logger.LogDebug(">>> Entering CheckWritebacks(reg={Reg},shortcut={Shortcut},app={App},target={Target})", reg, shortcut, app, target);
            // either registry or shortcut are null
            if ((reg == null && shortcut == null) || (reg != null && shortcut != null))
            {
				_logger.LogWarning("Throwing NotSupportedException from CheckWritebacks(reg={Reg},shortcut={Shortcut},app={App},target={Target})", reg, shortcut, app, target);
                throw new NotSupportedException("Must leave either registry or shortcut null. And must supply one of the two.");
            }

            bool isRegMode = reg != null; // true is reg mode, false is shortcut mode

            string modeName = isRegMode ? "registry" : "shortcut";

            AutoHelpers.LogInvariant("Beginning {0} writeback tests for {1}", modeName, target.ToString());

            using (PropertiesDialog props = new PropertiesDialog(app))
            {
                // STEP 1: VERIFY EVERYTHING SAVES IN AN ON/MAX STATE
                AutoHelpers.LogInvariant("Open dialog and check boxes.");
                props.Open(target);

                using (Tabs tabs = new Tabs(props))
                {
                    // Set V2 on.
                    tabs.SetGlobalState(Tabs.GlobalState.ConsoleV2);

                    AutoHelpers.LogInvariant("Toggling elements on all tabs.");
                    foreach (TabBase tab in tabs.AllTabs)
                    {
                        tab.NavigateToTab();

                        foreach (CheckBoxMeta obj in tab.GetCheckboxesForVerification())
                        {
                            obj.Check();
                        }

                        foreach (SliderMeta obj in tab.GetSlidersForVerification())
                        {
                            // adjust slider to the maximum
                            obj.SetToMaximum();
                        }
                    }

                    AutoHelpers.LogInvariant("Hit OK to save.");
                    props.Close(PropertiesDialog.CloseAction.OK);

                    AutoHelpers.LogInvariant("Verify values changed as appropriate.");
                    CheckWritebacksVerifyValues(isRegMode, reg, shortcut, target, tabs, SliderMeta.ExpectedPosition.Maximum, false, Tabs.GlobalState.ConsoleV2);
                }

                // STEP 2: VERIFY EVERYTHING SAVES IN AN OFF/MIN STATE
                AutoHelpers.LogInvariant("Open dialog and uncheck boxes.");
                props.Open(target);

                using (Tabs tabs = new Tabs(props))
                {
                    AutoHelpers.LogInvariant("Toggling elements on all tabs.");
                    foreach (TabBase tab in tabs.AllTabs)
                    {
                        tab.NavigateToTab();

                        foreach (SliderMeta slider in tab.GetSlidersForVerification())
                        {
                            // adjust slider to the minimum
                            slider.SetToMinimum();
                        }

                        foreach (CheckBoxMeta obj in tab.GetCheckboxesForVerification())
                        {
                            obj.Uncheck();
                        }
                    }

                    tabs.SetGlobalState(Tabs.GlobalState.ConsoleV1);

                    AutoHelpers.LogInvariant("Hit OK to save.");
                    props.Close(PropertiesDialog.CloseAction.OK);

                    AutoHelpers.LogInvariant("Verify values changed as appropriate.");
                    CheckWritebacksVerifyValues(isRegMode, reg, shortcut, target, tabs, SliderMeta.ExpectedPosition.Minimum, true, Tabs.GlobalState.ConsoleV1);
                }

                // STEP 3: VERIFY CANCEL DOES NOT SAVE
                AutoHelpers.LogInvariant("Open dialog and check boxes.");
                props.Open(target);

                using (Tabs tabs = new Tabs(props))
                {
                    tabs.SetGlobalState(Tabs.GlobalState.ConsoleV2);

                    AutoHelpers.LogInvariant("Toggling elements on all tabs.");
                    foreach (TabBase tab in tabs.AllTabs)
                    {
                        tab.NavigateToTab();

                        foreach (CheckBoxMeta obj in tab.GetCheckboxesForVerification())
                        {
                            obj.Check();
                        }

                        foreach (SliderMeta obj in tab.GetSlidersForVerification())
                        {
                            // adjust slider to the maximum
                            obj.SetToMaximum();
                        }
                    }

                    AutoHelpers.LogInvariant("Hit cancel to not save.");
                    props.Close(PropertiesDialog.CloseAction.Cancel);

                    AutoHelpers.LogInvariant("Verify values did not change.");
                    CheckWritebacksVerifyValues(isRegMode, reg, shortcut, target, tabs, SliderMeta.ExpectedPosition.Minimum, true, Tabs.GlobalState.ConsoleV1);
                }
            }
			_logger.LogDebug("<<< Exiting CheckWritebacks(reg={Reg},shortcut={Shortcut},app={App},target={Target})", reg, shortcut, app, target);
        }

        private void CheckWritebacksVerifyValues(bool isRegMode, RegistryHelper reg, ShortcutHelper shortcut, OpenTarget target, Tabs tabs, SliderMeta.ExpectedPosition sliderExpected, bool checkboxValue, Tabs.GlobalState consoleVersion)
        {
			_logger.LogDebug(">>> Entering CheckWritebacksVerifyValues(isRegMode={IsRegMode},reg={Reg},shortcut={Shortcut},target={Target},tabs={Tabs},sliderExpected={SliderExpected},checkboxValue={CheckboxValue},consoleVersion={ConsoleVersion})", isRegMode, reg, shortcut, target, tabs, sliderExpected, checkboxValue, consoleVersion);
            foreach (TabBase tab in tabs.AllTabs)
            {
                CheckWritebacksVerifyValues(isRegMode, reg, shortcut, target, tab, sliderExpected, checkboxValue, consoleVersion);
            }
			_logger.LogDebug("<<< Exiting CheckWritebacksVerifyValues(isRegMode={IsRegMode},reg={Reg},shortcut={Shortcut},target={Target},tabs={Tabs},sliderExpected={SliderExpected},checkboxValue={CheckboxValue},consoleVersion={ConsoleVersion})", isRegMode, reg, shortcut, target, tabs, sliderExpected, checkboxValue, consoleVersion);
        }

        private void CheckWritebacksVerifyValues(bool isRegMode, RegistryHelper reg, ShortcutHelper shortcut, OpenTarget target, TabBase tab, SliderMeta.ExpectedPosition sliderExpected, bool checkboxValue, Tabs.GlobalState consoleVersion)
        {
			_logger.LogDebug(">>> Entering CheckWritebacksVerifyValues(isRegMode={IsRegMode},reg={Reg},shortcut={Shortcut},target={Target},tab={Tab},sliderExpected={SliderExpected},checkboxValue={CheckboxValue},consoleVersion={ConsoleVersion})", isRegMode, reg, shortcut, target, tab, sliderExpected, checkboxValue, consoleVersion);
            if (isRegMode)
            {
                VerifyBoxes(tab, reg, checkboxValue, target, consoleVersion);
                VerifySliders(tab, reg, sliderExpected, target, consoleVersion);
            }
            else
            {
                // Have to wait for shortcut to get written. 
                // There isn't really an event to know when this occurs, so just wait.
                Globals.WaitForTimeout();

                VerifyBoxes(tab, shortcut, checkboxValue, consoleVersion);
                VerifySliders(tab, shortcut, sliderExpected, consoleVersion);
            }
			_logger.LogDebug("<<< Exiting CheckWritebacksVerifyValues(isRegMode={IsRegMode},reg={Reg},shortcut={Shortcut},target={Target},tab={Tab},sliderExpected={SliderExpected},checkboxValue={CheckboxValue},consoleVersion={ConsoleVersion})", isRegMode, reg, shortcut, target, tab, sliderExpected, checkboxValue, consoleVersion);
        }

        private void VerifyBoxes(TabBase tab, RegistryHelper reg, bool inverse, OpenTarget target, Tabs.GlobalState consoleVersion)
        {
			_logger.LogDebug(">>> Entering VerifyBoxes(tab={Tab},reg={Reg},inverse={Inverse},target={Target},consoleVersion={ConsoleVersion})", tab, reg, inverse, target, consoleVersion);
            // get the key for the current target
            RegistryKey consoleKey = reg.GetMatchingKey(target);
			_logger.LogDebug("VerifyBoxes(tab={Tab},reg={Reg},inverse={Inverse},target={Target},consoleVersion={ConsoleVersion}): consoleKey → {ConsoleKey}", tab, reg, inverse, target, consoleVersion, consoleKey);

            // hold the parent console key in case we need to look things up for specifics.
            RegistryKey parentConsoleKey = reg.GetMatchingKey(OpenTarget.Defaults);
			_logger.LogDebug("VerifyBoxes(tab={Tab},reg={Reg},inverse={Inverse},target={Target},consoleVersion={ConsoleVersion}): parentConsoleKey → {ParentConsoleKey}", tab, reg, inverse, target, consoleVersion, parentConsoleKey);

            // include the global checkbox in the set for verification purposes
            IEnumerable<CheckBoxMeta> boxes = tab.GetCheckboxesForVerification();
			_logger.LogDebug("VerifyBoxes(tab={Tab},reg={Reg},inverse={Inverse},target={Target},consoleVersion={ConsoleVersion}): boxes → {Boxes}", tab, reg, inverse, target, consoleVersion, boxes);

            AutoHelpers.LogInvariant("Testing target: {0} in inverse {1} mode", target.ToString(), inverse.ToString());

            // If we're opened as specifics, remove all global only boxes from the test set
            if (target == OpenTarget.Specifics)
            {
                AutoHelpers.LogInvariant("Reducing");
                boxes = boxes.Where(box => !box.IsGlobalOnly);
            }

            foreach (CheckBoxMeta meta in boxes)
            {
                int? storedValue = consoleKey.GetValue(meta.ValueName) as int?;

                string boxName = AutoHelpers.FormatInvariant("Box: {0}", meta.ValueName);

                // if we're in specifics mode, we might have a null and if so, we check the parent value
                if (target == OpenTarget.Specifics)
                {
                    if (storedValue == null)
                    {
                        AutoHelpers.LogInvariant("Specific setting missing. Checking defaults.");
                        storedValue = parentConsoleKey.GetValue(meta.ValueName) as int?;
                    }
					_logger.LogDebug("VerifyBoxes(tab={Tab},reg={Reg},inverse={Inverse},target={Target},consoleVersion={ConsoleVersion}): storedValue → {StoredValue}", tab, reg, inverse, target, consoleVersion, storedValue);
                }
                else
                {
                    Verify.IsNotNull(storedValue, boxName);
                }

                if (consoleVersion == Tabs.GlobalState.ConsoleV1 && meta.IsV2Property)
                {
                    AutoHelpers.LogInvariant("Skipping validation of v2 property {0} after switching to v1 console.", meta.ValueName);
                }
                else
                {
                    // A box can be inverse if checking it means false in the registry.
                    // This method can be inverse if we're turning off the boxes and expecting it to be on.
                    // Therefore, a box will be false if it's checked and supposed to be off. Or if it's unchecked and supposed to be on.
                    if ((meta.IsInverse && !inverse) || (!meta.IsInverse && inverse))
                    {
                        Verify.IsFalse(storedValue.Value.DwordToBool(), boxName);
                    }
                    else
                    {
                        Verify.IsTrue(storedValue.Value.DwordToBool(), boxName);
                    }
                }
            }
			_logger.LogDebug("<<< Exiting VerifyBoxes(tab={Tab},reg={Reg},inverse={Inverse},target={Target},consoleVersion={ConsoleVersion})", tab, reg, inverse, target, consoleVersion);
        }

        private void VerifyBoxes(TabBase tab, ShortcutHelper shortcut, bool inverse, Tabs.GlobalState consoleVersion)
        {
			_logger.LogDebug(">>> Entering VerifyBoxes(tab={Tab},shortcut={Shortcut},inverse={Inverse},consoleVersion={ConsoleVersion})", tab, shortcut, inverse, consoleVersion);
            IEnumerable<CheckBoxMeta> boxes = tab.GetCheckboxesForVerification();

			_logger.LogDebug("VerifyBoxes(tab={Tab},shortcut={Shortcut},inverse={Inverse},consoleVersion={ConsoleVersion}): boxes → {Boxes}", tab, shortcut, inverse, consoleVersion, boxes);
            // collect up properties that we need to retrieve keys for
            IEnumerable<CheckBoxMeta> propBoxes = boxes.Where(box => box.PropKey != null);
			_logger.LogDebug("VerifyBoxes(tab={Tab},shortcut={Shortcut},inverse={Inverse},consoleVersion={ConsoleVersion}): propBoxes → {PropBoxes}", tab, shortcut, inverse, consoleVersion, propBoxes);
            IEnumerable<Wtypes.PROPERTYKEY> keys = propBoxes.Select(box => box.PropKey).Cast<Wtypes.PROPERTYKEY>();
			_logger.LogDebug("VerifyBoxes(tab={Tab},shortcut={Shortcut},inverse={Inverse},consoleVersion={ConsoleVersion}): keys → {Keys}", tab, shortcut, inverse, consoleVersion, keys);

            // fetch data for keys
            IDictionary<Wtypes.PROPERTYKEY, object> propertyData = shortcut.GetFromPropertyStore(keys);

            // enumerate each box and validate the data
            foreach (CheckBoxMeta meta in propBoxes)
            {
                string boxName = AutoHelpers.FormatInvariant("Box: {0}", meta.ValueName);

                Wtypes.PROPERTYKEY key = (Wtypes.PROPERTYKEY)meta.PropKey;
				_logger.LogWarning("VerifyBoxes(tab={Tab},shortcut={Shortcut},inverse={Inverse},consoleVersion={ConsoleVersion}): key → {Key}", tab, shortcut, inverse, consoleVersion, key);

                bool? value = (bool?)propertyData[key];

                Verify.IsNotNull(value, boxName);

                if (consoleVersion == Tabs.GlobalState.ConsoleV1 && meta.IsV2Property)
                {
                    AutoHelpers.LogInvariant("Skipping validation of v2 property {0} after switching to v1 console.", meta.ValueName);
                }
                else
                {
                    // A box can be inverse if checking it means false in the registry.
                    // This method can be inverse if we're turning off the boxes and expecting it to be on.
                    // Therefore, a box will be false if it's checked and supposed to be off. Or if it's unchecked and supposed to be on.
                    if ((meta.IsInverse && !inverse) || (!meta.IsInverse && inverse))
                    {
                        Verify.IsFalse(value.Value, boxName);
                    }
                    else
                    {
                        Verify.IsTrue(value.Value, boxName);
                    }
                }
            }
			_logger.LogDebug("<<< Exiting VerifyBoxes(tab={Tab},shortcut={Shortcut},inverse={Inverse},consoleVersion={ConsoleVersion})", tab, shortcut, inverse, consoleVersion);
        }

        private void VerifySliders(TabBase tab, RegistryHelper reg, SliderMeta.ExpectedPosition expected, OpenTarget target, Tabs.GlobalState consoleVersion)
        {
			_logger.LogDebug(">>> Entering VerifySliders(tab={Tab},reg={Reg},expected={Expected},target={Target},consoleVersion={ConsoleVersion})", tab, reg, expected, target, consoleVersion);
            // get the key for the current target
            RegistryKey consoleKey = reg.GetMatchingKey(target);
			_logger.LogDebug("VerifySliders(tab={Tab},reg={Reg},expected={Expected},target={Target},consoleVersion={ConsoleVersion}): consoleKey → {ConsoleKey}", tab, reg, expected, target, consoleVersion, consoleKey);

            // hold the parent console key in case we need to look things up for specifics.
            RegistryKey parentConsoleKey = reg.GetMatchingKey(OpenTarget.Defaults);
			_logger.LogDebug("VerifySliders(tab={Tab},reg={Reg},expected={Expected},target={Target},consoleVersion={ConsoleVersion}): parentConsoleKey → {ParentConsoleKey}", tab, reg, expected, target, consoleVersion, parentConsoleKey);

            IEnumerable<SliderMeta> sliders = tab.GetSlidersForVerification();
			_logger.LogDebug("VerifySliders(tab={Tab},reg={Reg},expected={Expected},target={Target},consoleVersion={ConsoleVersion}): sliders → {Sliders}", tab, reg, expected, target, consoleVersion, sliders);

            foreach (SliderMeta meta in sliders)
            {
                int? storedValue = consoleKey.GetValue(meta.ValueName) as int?;

                string sliderName = AutoHelpers.FormatInvariant("Slider: {0}", meta.ValueName);

                if (target == OpenTarget.Specifics)
                {
                    if (storedValue == null)
                    {
                        AutoHelpers.LogInvariant("Specific setting missing. Checking defaults.");
                        storedValue = parentConsoleKey.GetValue(meta.ValueName) as int?;
                    }
					_logger.LogDebug("VerifySliders(tab={Tab},reg={Reg},expected={Expected},target={Target},consoleVersion={ConsoleVersion}): storedValue → {StoredValue}", tab, reg, expected, target, consoleVersion, storedValue);
                }
                else
                {
                    Verify.IsNotNull(storedValue, sliderName);
                }

                int transparency = 0;
				_logger.LogDebug("VerifySliders(tab={Tab},reg={Reg},expected={Expected},target={Target},consoleVersion={ConsoleVersion}): transparency → {Transparency}", tab, reg, expected, target, consoleVersion, transparency);

                switch (expected)
                {
                    case SliderMeta.ExpectedPosition.Maximum:
                        transparency = meta.GetMaximum();
                        break;
                    case SliderMeta.ExpectedPosition.Minimum:
                        transparency = meta.GetMinimum();
                        break;
                    default:
						_logger.LogWarning("Throwing NotImplementedException from VerifySliders(tab={Tab},reg={Reg},expected={Expected},target={Target},consoleVersion={ConsoleVersion})", tab, reg, expected, target, consoleVersion);
                        throw new NotImplementedException();
                }

                if (consoleVersion == Tabs.GlobalState.ConsoleV1 && meta.IsV2Property)
                {
                    AutoHelpers.LogInvariant("Skipping validation of v2 property {0} after switching to v1 console.", meta.ValueName);
                }
                else
                {
                    Verify.AreEqual(storedValue.Value, RescaleSlider(transparency), sliderName);
                }
            }
			_logger.LogDebug("<<< Exiting VerifySliders(tab={Tab},reg={Reg},expected={Expected},target={Target},consoleVersion={ConsoleVersion})", tab, reg, expected, target, consoleVersion);
        }

        private void VerifySliders(TabBase tab, ShortcutHelper shortcut, SliderMeta.ExpectedPosition expected, Tabs.GlobalState consoleVersion)
        {
			_logger.LogDebug(">>> Entering VerifySliders(tab={Tab},shortcut={Shortcut},expected={Expected},consoleVersion={ConsoleVersion})", tab, shortcut, expected, consoleVersion);
            IEnumerable<SliderMeta> sliders = tab.GetSlidersForVerification();

			_logger.LogDebug("VerifySliders(tab={Tab},shortcut={Shortcut},expected={Expected},consoleVersion={ConsoleVersion}): sliders → {Sliders}", tab, shortcut, expected, consoleVersion, sliders);
            // collect up properties that we need to retrieve keys for
            IEnumerable<SliderMeta> propSliders = sliders.Where(slider => slider.PropKey != null);
			_logger.LogDebug("VerifySliders(tab={Tab},shortcut={Shortcut},expected={Expected},consoleVersion={ConsoleVersion}): propSliders → {PropSliders}", tab, shortcut, expected, consoleVersion, propSliders);
            IEnumerable<Wtypes.PROPERTYKEY> keys = propSliders.Select(slider => slider.PropKey).Cast<Wtypes.PROPERTYKEY>();
			_logger.LogDebug("VerifySliders(tab={Tab},shortcut={Shortcut},expected={Expected},consoleVersion={ConsoleVersion}): keys → {Keys}", tab, shortcut, expected, consoleVersion, keys);

            // fetch data for keys
            IDictionary<Wtypes.PROPERTYKEY, object> propertyData = shortcut.GetFromPropertyStore(keys);

            // enumerate each slider and validate data
            foreach (SliderMeta meta in sliders)
            {
                string sliderName = AutoHelpers.FormatInvariant("Slider: {0}", meta.ValueName);

                Wtypes.PROPERTYKEY key = (Wtypes.PROPERTYKEY)meta.PropKey;
				_logger.LogWarning("VerifySliders(tab={Tab},shortcut={Shortcut},expected={Expected},consoleVersion={ConsoleVersion}): key → {Key}", tab, shortcut, expected, consoleVersion, key);

                short value = (short)propertyData[key];
				_logger.LogDebug("VerifySliders(tab={Tab},shortcut={Shortcut},expected={Expected},consoleVersion={ConsoleVersion}): value → {Value}", tab, shortcut, expected, consoleVersion, value);

                int transparency = 0;
				_logger.LogDebug("VerifySliders(tab={Tab},shortcut={Shortcut},expected={Expected},consoleVersion={ConsoleVersion}): transparency → {Transparency}", tab, shortcut, expected, consoleVersion, transparency);

                switch (expected)
                {
                    case SliderMeta.ExpectedPosition.Maximum:
                        transparency = meta.GetMaximum();
                        break;
                    case SliderMeta.ExpectedPosition.Minimum:
                        transparency = meta.GetMinimum();
                        break;
                    default:
						_logger.LogWarning("Throwing NotImplementedException from VerifySliders(tab={Tab},shortcut={Shortcut},expected={Expected},consoleVersion={ConsoleVersion})", tab, shortcut, expected, consoleVersion);
                        throw new NotImplementedException();
                }

                if (consoleVersion == Tabs.GlobalState.ConsoleV1 && meta.IsV2Property)
                {
                    AutoHelpers.LogInvariant("Skipping validation of v2 property {0} after switching to v1 console.", meta.ValueName);
                }
                else
                {
                    Verify.AreEqual(value, RescaleSlider(transparency), sliderName);
                }
            }
			_logger.LogDebug("<<< Exiting VerifySliders(tab={Tab},shortcut={Shortcut},expected={Expected},consoleVersion={ConsoleVersion})", tab, shortcut, expected, consoleVersion);
        }

        private short RescaleSlider(int inputValue)
        {
			_logger.LogDebug(">>> Entering RescaleSlider(inputValue={InputValue})", inputValue);
            // we go on a scale from 0x4D to 0xFF.
            int minValue = 0x4D;
            int maxValue = 0xFF;

            int valueRange = maxValue - minValue;

            double scaleFactor = (double)inputValue / 100.0;
			_logger.LogDebug("RescaleSlider(inputValue={InputValue}): scaleFactor → {ScaleFactor}", inputValue, scaleFactor);

            short finalValue = (short)((valueRange * scaleFactor) + minValue);
			_logger.LogDebug("RescaleSlider(inputValue={InputValue}): finalValue → {FinalValue}", inputValue, finalValue);

			_logger.LogDebug("<<< Exiting RescaleSlider(inputValue={InputValue})", inputValue);
            return finalValue;
        }
    }
}
