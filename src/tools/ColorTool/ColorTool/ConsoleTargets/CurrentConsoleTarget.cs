//
// Copyright (C) Microsoft.  All rights reserved.
// Licensed under the terms described in the LICENSE file in the root of this project.
//

using System;
using static ColorTool.ConsoleAPI;
using Microsoft.Extensions.Logging;

namespace ColorTool.ConsoleTargets
{
    /// <summary>
    /// A console target that writes to the currently open console.
    /// </summary>
    class CurrentConsoleTarget : IConsoleTarget
    {
		private static readonly Microsoft.Extensions.Logging.ILogger _logger = Microsoft.Extensions.Logging.Abstractions.NullLogger.Instance;
        public void ApplyColorScheme(ColorScheme colorScheme, bool quietMode, bool compactColortable)
        {
			_logger.LogDebug(">>> Entering ApplyColorScheme(colorScheme={ColorScheme},quietMode={QuietMode},compactColortable={CompactColortable})", colorScheme, quietMode, compactColortable);
            CONSOLE_SCREEN_BUFFER_INFO_EX csbiex = CONSOLE_SCREEN_BUFFER_INFO_EX.Create();
            IntPtr hOut = GetStdOutputHandle();
			_logger.LogDebug("ApplyColorScheme(colorScheme={ColorScheme},quietMode={QuietMode},compactColortable={CompactColortable}): hOut → {HOut}", colorScheme, quietMode, compactColortable, hOut);
            bool success = GetConsoleScreenBufferInfoEx(hOut, ref csbiex);
			_logger.LogDebug("ApplyColorScheme(colorScheme={ColorScheme},quietMode={QuietMode},compactColortable={CompactColortable}): csbiex → {Csbiex}", colorScheme, quietMode, compactColortable, csbiex);
            if (!success)
            {
				_logger.LogWarning("Throwing InvalidOperationException from ApplyColorScheme(colorScheme={ColorScheme},quietMode={QuietMode},compactColortable={CompactColortable})", colorScheme, quietMode, compactColortable);
                throw new InvalidOperationException("Could not obtain console screen buffer");
            }

            csbiex.srWindow.Bottom++;
            for (int i = 0; i < 16; i++)
            {
                csbiex.ColorTable[i] = colorScheme.ColorTable[i];
            }
            if(colorScheme.ScreenColorAttributes is ushort wAttrs)
            {
                csbiex.wAttributes = wAttrs;
				_logger.LogDebug("ApplyColorScheme(colorScheme={ColorScheme},quietMode={QuietMode},compactColortable={CompactColortable}): csbiex.wAttributes → {Arg3}", colorScheme, quietMode, compactColortable, csbiex.wAttributes);
            }
            if(colorScheme.PopupColorAttributes is ushort wPopupAttrs)
            {
                csbiex.wPopupAttributes = wPopupAttrs;
				_logger.LogDebug("ApplyColorScheme(colorScheme={ColorScheme},quietMode={QuietMode},compactColortable={CompactColortable}): csbiex.wPopupAttributes → {Arg3}", colorScheme, quietMode, compactColortable, csbiex.wPopupAttributes);
            }
            SetConsoleScreenBufferInfoEx(hOut, ref csbiex);

            if (!quietMode)
            {
                ColorTable.PrintTable(compactColortable);
            }
			_logger.LogDebug("<<< Exiting ApplyColorScheme(colorScheme={ColorScheme},quietMode={QuietMode},compactColortable={CompactColortable})", colorScheme, quietMode, compactColortable);
        }
    }
}
