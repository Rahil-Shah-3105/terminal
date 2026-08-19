//----------------------------------------------------------------------------------------------------------------------
// <copyright file="Globals.cs" company="Microsoft">
//     Copyright (c) Microsoft Corporation.  All rights reserved.
// </copyright>
// <summary>Console UI Automation global settings</summary>
//----------------------------------------------------------------------------------------------------------------------
namespace Conhost.UIA.Tests.Common
{
    using System;
	using Microsoft.Extensions.Logging;

    public static class Globals
    {
        public const int Timeout = 50; // in milliseconds
        public const int AppCreateTimeout = 3000; // in milliseconds

        // These were pulled via UISpy from system defined window classes.
        public const string PopupMenuClassId = "#32768";
        public const string DialogWindowClassId = "#32770";

        public static void WaitForTimeout()
        {
			_logger.LogDebug(">>> Entering WaitForTimeout()");
            System.Threading.Thread.Sleep(Globals.Timeout);
			_logger.LogDebug("<<< Exiting WaitForTimeout()");
        }
    }

    public enum OpenTarget
    {
		private static readonly Microsoft.Extensions.Logging.ILogger _logger = Microsoft.Extensions.Logging.Abstractions.NullLogger.Instance;
        Defaults,
        Specifics
    }

    public enum CreateType
    {
        ProcessOnly,
        ShortcutFile
    }
}
