//----------------------------------------------------------------------------------------------------------------------
// <copyright file="ExperimentalTabTests.cs" company="Microsoft">
// Copyright (c) Microsoft Corporation.
// Licensed under the MIT license.
// </copyright>
//----------------------------------------------------------------------------------------------------------------------

using System;
using System.Collections.Generic;
using System.Diagnostics;
using System.IO;
using System.Linq;
using System.Security.Principal;
using System.Text;
using System.Threading.Tasks;
using WEX.Logging.Interop;
using WEX.TestExecution;
using WEX.TestExecution.Markup;
using Microsoft.Extensions.Logging;

namespace Host.Tests.UIA
{
    [TestClass]
    class Init
    {
		private static readonly Microsoft.Extensions.Logging.ILogger _logger = Microsoft.Extensions.Logging.Abstractions.NullLogger.Instance;
        static Process appDriver;

        [AssemblyInitialize]
        public static void SetupAll(TestContext context)
        {
			_logger.LogDebug(">>> Entering SetupAll(context={Context})", context);
            Log.Comment("Searching for WinAppDriver in the same directory where this test was launched from...");
            string winAppDriver = Path.Combine(context.TestDeploymentDir, "WinAppDriver", "WinAppDriver.exe");
			_logger.LogDebug("SetupAll(context={Context}): winAppDriver → {WinAppDriver}", context, winAppDriver);

            if (!File.Exists(winAppDriver))
            {
                winAppDriver = Path.Combine(context.TestDeploymentDir, "WinAppDriver.exe");
            }

            Log.Comment($"Attempting to launch WinAppDriver at: {winAppDriver}");
            Log.Comment($"Working directory: {Environment.CurrentDirectory}");

            appDriver = Process.Start(winAppDriver);
			_logger.LogDebug("SetupAll(context={Context}): appDriver → {AppDriver}", context, appDriver);
			_logger.LogDebug("<<< Exiting SetupAll(context={Context})", context);
        }

        [AssemblyCleanup]
        public static void CleanupAll()
        {
			_logger.LogDebug(">>> Entering CleanupAll()");
            try
            {
                appDriver.Kill();
            }
            catch
            {
			_logger.LogError("Exception in CleanupAll(): {Arg0}", ex.Message);

            }
			_logger.LogDebug("<<< Exiting CleanupAll()");
        }
    }
}
