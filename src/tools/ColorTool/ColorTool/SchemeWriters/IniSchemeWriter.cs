//
// Copyright (C) Microsoft.  All rights reserved.
// Licensed under the terms described in the LICENSE file in the root of this project.
//

using ColorTool.SchemeParsers;
using System;
using static ColorTool.ConsoleAPI;
using Microsoft.Extensions.Logging;

namespace ColorTool.SchemeWriters
{
    class IniSchemeWriter
    {
		private static readonly Microsoft.Extensions.Logging.ILogger _logger = Microsoft.Extensions.Logging.Abstractions.NullLogger.Instance;
        public bool ExportCurrentAsIni(string outputPath)
        {
			_logger.LogDebug(">>> Entering ExportCurrentAsIni(outputPath={OutputPath})", outputPath);
            CONSOLE_SCREEN_BUFFER_INFO_EX csbiex = CONSOLE_SCREEN_BUFFER_INFO_EX.Create();
            IntPtr hOut = GetStdOutputHandle();
			_logger.LogDebug("ExportCurrentAsIni(outputPath={OutputPath}): hOut → {HOut}", outputPath, hOut);
            bool success = GetConsoleScreenBufferInfoEx(hOut, ref csbiex);
			_logger.LogDebug("ExportCurrentAsIni(outputPath={OutputPath}): csbiex → {Csbiex}", outputPath, csbiex);
            if (success)
            {
                try
                {
                    // StreamWriter can fail for a variety of file system reasons so catch exceptions and print message.
                    using (System.IO.StreamWriter file = new System.IO.StreamWriter(outputPath))
                    {
                        file.WriteLine("[table]");
                        for (int i = 0; i < 16; i++)
                        {
                            string line = IniSchemeParser.ColorNames[i];
                            line += " = ";
                            uint color = csbiex.ColorTable[i];
                            uint r = color & (0x000000ff);
                            uint g = (color & (0x0000ff00)) >> 8;
                            uint b = (color & (0x00ff0000)) >> 16;
                            line += r + "," + g + "," + b;
                            file.WriteLine(line);
                        }

                        file.WriteLine();
                        file.WriteLine("[screen]");
                        var foregroundIndex = csbiex.wAttributes & 0xF;
                        var backgroundIndex = csbiex.wAttributes >> 4;
                        file.WriteLine($"FOREGROUND = {IniSchemeParser.ColorNames[foregroundIndex]}");
                        file.WriteLine($"BACKGROUND = {IniSchemeParser.ColorNames[backgroundIndex]}");

                        file.WriteLine();
                        file.WriteLine("[popup]");
                        foregroundIndex = csbiex.wPopupAttributes & 0xF;
                        backgroundIndex = csbiex.wPopupAttributes >> 4;
                        file.WriteLine($"FOREGROUND = {IniSchemeParser.ColorNames[foregroundIndex]}");
                        file.WriteLine($"BACKGROUND = {IniSchemeParser.ColorNames[backgroundIndex]}");
                    }
                }
                catch (Exception ex)
                {
                    Console.WriteLine(ex.Message);
                }
            }
            else
            {
                Console.WriteLine("Failed to get console information.");
            }
			_logger.LogDebug("<<< Exiting ExportCurrentAsIni(outputPath={OutputPath})", outputPath);
            return success;
        }
    }
}
