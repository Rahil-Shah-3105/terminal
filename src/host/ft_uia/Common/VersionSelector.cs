//----------------------------------------------------------------------------------------------------------------------
// <copyright file="VersionSelector.cs" company="Microsoft">
//     Copyright (c) Microsoft Corporation.  All rights reserved.
// </copyright>
// <summary>Console UI Automation Version selection</summary>
//----------------------------------------------------------------------------------------------------------------------
namespace Conhost.UIA.Tests.Common
{
    using System;

    using WEX.Logging.Interop;
	using Microsoft.Extensions.Logging;

    public static class VersionSelector
    {
        private static string v1v2ValueName = "ForceV2";

        public static void SetConsoleVersion(RegistryHelper reg, ConsoleVersion version)
        {
			_logger.LogDebug(">>> Entering SetConsoleVersion(reg={Reg},version={Version})", reg, version);
            switch (version)
            {
                case ConsoleVersion.V1:
                    Log.Comment("Setting console to v1 mode.");
                    reg.SetDefaultValue(v1v2ValueName, 0);
                    break;
                case ConsoleVersion.V2:
                    Log.Comment("Setting console to v2 mode.");
                    reg.SetDefaultValue(v1v2ValueName, 1);
                    break;
                default:
					_logger.LogWarning("Throwing NotImplementedException from SetConsoleVersion(reg={Reg},version={Version})", reg, version);
			_logger.LogDebug("<<< Exiting SetConsoleVersion(reg={Reg},version={Version})", reg, version);
                    throw new NotImplementedException();
            }
        }
    }

    public enum ConsoleVersion
    {
		private static readonly Microsoft.Extensions.Logging.ILogger _logger = Microsoft.Extensions.Logging.Abstractions.NullLogger.Instance;
        V1, // legacy 1990s console
        V2 // revision from 2014/2015.
    }
}
