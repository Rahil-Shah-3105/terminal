//----------------------------------------------------------------------------------------------------------------------
// <copyright file="Globals.cs" company="Microsoft">
//     Copyright (c) Microsoft Corporation.  All rights reserved.
// </copyright>
// <summary>Windows Terminal UI Automation global settings</summary>
//----------------------------------------------------------------------------------------------------------------------
namespace WindowsTerminal.UIA.Tests.Common
{
    using System;
    using WEX.TestExecution.Markup;
	using Microsoft.Extensions.Logging;

    public static class Globals
    {
		private static readonly Microsoft.Extensions.Logging.ILogger _logger = Microsoft.Extensions.Logging.Abstractions.NullLogger.Instance;
        public const int Timeout = 50; // in milliseconds
        public const int LongTimeout = 5000; // in milliseconds
        public const int AppCreateTimeout = 3000; // in milliseconds

        public static void WaitForTimeout()
        {
			_logger.LogDebug(">>> Entering WaitForTimeout()");
            System.Threading.Thread.Sleep(Globals.Timeout);
			_logger.LogDebug("<<< Exiting WaitForTimeout()");
        }

        public static void WaitForLongTimeout()
        {
			_logger.LogDebug(">>> Entering WaitForLongTimeout()");
            System.Threading.Thread.Sleep(Globals.LongTimeout);
			_logger.LogDebug("<<< Exiting WaitForLongTimeout()");
        }


        static string[] modules =
        {
            "WindowsTerminal.exe",
            "OpenConsole.exe",
            "Microsoft.Terminal.Control.dll",
            "Microsoft.Terminal.Settings.Editor.dll",
            "Microsoft.Terminal.Settings.Model.dll",
            "TerminalApp.dll",
            "TerminalConnection.dll"
        };

        public static void SweepAllModules(TestContext context)
        {
			_logger.LogDebug(">>> Entering SweepAllModules(context={Context})", context);
            foreach (var mod in modules)
            {
                PgoManager.PgoSweepIfInstrumented(context, mod);
            }
			_logger.LogDebug("<<< Exiting SweepAllModules(context={Context})", context);
        }
    }
}
