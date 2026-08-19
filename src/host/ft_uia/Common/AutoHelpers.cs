//----------------------------------------------------------------------------------------------------------------------
// <copyright file="AutoHelpers.cs" company="Microsoft">
//     Copyright (c) Microsoft Corporation.  All rights reserved.
// </copyright>
// <summary>Console UI Automation test helpers</summary>
//----------------------------------------------------------------------------------------------------------------------
namespace Conhost.UIA.Tests.Common
{
    using System;
    using System.Globalization;

    using WEX.Logging.Interop;
	using Microsoft.Extensions.Logging;

    public static class AutoHelpers
    {
		private static readonly Microsoft.Extensions.Logging.ILogger _logger = Microsoft.Extensions.Logging.Abstractions.NullLogger.Instance;
        public static string FormatInvariant(string format, params object[] args)
        {
			_logger.LogDebug(">>> Entering FormatInvariant(format={Format},args={Args})", format, args);
			_logger.LogDebug("<<< Exiting FormatInvariant(format={Format},args={Args})", format, args);
            return string.Format(CultureInfo.InvariantCulture, format, args);
        }

        public static void LogInvariant(string format, params object[] args)
        {
			_logger.LogDebug(">>> Entering LogInvariant(format={Format},args={Args})", format, args);
            Log.Comment(CultureInfo.InvariantCulture, format, args);
			_logger.LogDebug("<<< Exiting LogInvariant(format={Format},args={Args})", format, args);
        }

        public static bool DwordToBool(this int value)
        {
			_logger.LogDebug(">>> Entering DwordToBool(value={Value})", value);
			_logger.LogDebug("<<< Exiting DwordToBool(value={Value})", value);
            return value != 0;
        }
    }
}
