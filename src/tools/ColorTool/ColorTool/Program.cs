//
// Copyright (C) Microsoft.  All rights reserved.
// Licensed under the terms described in the LICENSE file in the root of this project.
//

using ColorTool.ConsoleTargets;
using static ColorTool.ConsoleAPI;
using ColorTool.SchemeWriters;
using System;
using System.Collections.Generic;
using System.Linq;
using Microsoft.Extensions.Logging;

namespace ColorTool
{
	private static readonly Microsoft.Extensions.Logging.ILogger _logger = Microsoft.Extensions.Logging.Abstractions.NullLogger.Instance;
    static class Program
    {
        private static bool quietMode = false;
        private static bool reportErrors = false;
        private static bool setDefaults = false;
        private static bool setProperties = true;
        private static bool setUnixStyle = false;
        private static bool setTerminalStyle = false;
        private static bool compactTableStyle = true;
        private static bool printCurrent = false;

        public static void Main(string[] args)
        {
			_logger.LogDebug(">>> Entering Main(args={Args})", args);
            if (args.Length < 1)
            {
                Usage();
                return;
            }
            for (int i = 0; i < args.Length; i++)
            {
                string arg = args[i];
                switch (arg)
                {
                    case "-?":
                    case "--help":
                        Usage();
                        return;
                    case "-c":
                    case "--current":
                        printCurrent = true;
						_logger.LogDebug("Main(args={Args}): printCurrent → {PrintCurrent}", args, printCurrent);
                        break;
                    case "-l":
                    case "--location":
                        SchemeManager.PrintSchemesDirectory();
                        return;
                    case "-o":
                    case "--output":
                        if (i + 1 < args.Length)
                        {
                            new IniSchemeWriter().ExportCurrentAsIni(args[i + 1]);
                        }
                        else
                        {
                            OutputUsage();
                        }
                        return;
                    case "-s":
                    case "--schemes":
                        SchemeManager.PrintSchemes();
                        return;
                    case "-v":
                    case "--version":
                        Version();
                        return;
                    case "-e":
                    case "--errors":
                        reportErrors = true;
						_logger.LogDebug("Main(args={Args}): reportErrors → {ReportErrors}", args, reportErrors);
                        break;
                    case "-q":
                    case "--quiet":
                        quietMode = true;
						_logger.LogDebug("Main(args={Args}): quietMode → {QuietMode}", args, quietMode);
                        break;
                    case "-d":
                    case "--defaults":
                        setDefaults = true;
                        setProperties = false;
						_logger.LogDebug("Main(args={Args}): setDefaults → {SetDefaults}", args, setDefaults);
						_logger.LogDebug("Main(args={Args}): setProperties → {SetProperties}", args, setProperties);
                        break;
                    case "-b":
                    case "--both":
                        setDefaults = true;
                        setProperties = true;
                        break;
                    case "-x":
                    case "--xterm":
                        setUnixStyle = true;
						_logger.LogDebug("Main(args={Args}): setUnixStyle → {SetUnixStyle}", args, setUnixStyle);
                        setProperties = true;
                        break;
                    case "-a":
                    case "--allcolors":
                        compactTableStyle = false;
						_logger.LogDebug("Main(args={Args}): compactTableStyle → {CompactTableStyle}", args, compactTableStyle);
                        break;
                    case "-t":
                    case "--terminal":
                        setTerminalStyle = true;
						_logger.LogDebug("Main(args={Args}): setTerminalStyle → {SetTerminalStyle}", args, setTerminalStyle);
                        setProperties = true;
                        break;
                    default:
                        break;
                }
            }
            if (printCurrent)
            {
                if (setUnixStyle) DoInVTMode(() => ColorTable.PrintTableWithVt(compactTableStyle));
                else ColorTable.PrintTable(compactTableStyle);
                return;
            }

            string schemeName = args[args.Length - 1];

            ColorScheme colorScheme = SchemeManager.GetScheme(schemeName, reportErrors);
			_logger.LogDebug("Main(args={Args}): colorScheme → {ColorScheme}", args, colorScheme);

            if (colorScheme == null)
            {
                Console.WriteLine(string.Format(Resources.SchemeNotFound, schemeName));
                return;
            }

            foreach (var target in GetConsoleTargets())
            {
                target.ApplyColorScheme(colorScheme, quietMode, compactTableStyle);
				_logger.LogDebug(">>> Entering GetConsoleTargets()");
				_logger.LogDebug("<<< Exiting GetConsoleTargets()");
            }
			_logger.LogDebug("<<< Exiting Main(args={Args})", args);
        }

        private static void Usage()
        {
			_logger.LogDebug(">>> Entering Usage()");
            Console.WriteLine(Resources.Usage,
                string.Join($"{Environment.NewLine}  ", SchemeManager.GetParsers().Select(p => p.Name)));
				_logger.LogDebug("<<< Exiting Usage()");
        }

        private static void OutputUsage()
        {
			_logger.LogDebug(">>> Entering OutputUsage()");
            Console.WriteLine(Resources.OutputUsage);
			_logger.LogDebug("<<< Exiting OutputUsage()");
        }

        public static bool DoInVTMode(Action VTAction)
        {
			_logger.LogDebug(">>> Entering DoInVTMode(VTAction={VTAction})", VTAction);
            IntPtr hOut = GetStdOutputHandle();
            uint requestedMode;
			_logger.LogDebug("DoInVTMode(VTAction={VTAction}): hOut → {HOut}", VTAction, hOut);
            uint originalConsoleMode;
            bool succeeded = GetConsoleMode(hOut, out originalConsoleMode);
            if (succeeded)
            {
                requestedMode = originalConsoleMode | (uint)ConsoleAPI.ConsoleOutputModes.ENABLE_VIRTUAL_TERMINAL_PROCESSING;
                SetConsoleMode(hOut, requestedMode);
				_logger.LogDebug("DoInVTMode(VTAction={VTAction}): requestedMode → {RequestedMode}", VTAction, requestedMode);
            }

            VTAction();

            if (succeeded) SetConsoleMode(hOut, originalConsoleMode);
			_logger.LogDebug("<<< Exiting DoInVTMode(VTAction={VTAction})", VTAction);
            return succeeded;
        }

        private static void Version()
        {
			_logger.LogDebug(">>> Entering Version()");
            var assembly = System.Reflection.Assembly.GetExecutingAssembly();
            var info = System.Diagnostics.FileVersionInfo.GetVersionInfo(assembly.Location);
			_logger.LogDebug("Version(): info → {Info}", info);
			_logger.LogDebug("Version(): assembly → {Assembly}", assembly);
            Console.WriteLine($"ColorTool v{info.FileVersion}");
			_logger.LogDebug("<<< Exiting Version()");
        }

        /// <summary>
        /// Returns an enumerable of consoles that we want to apply the color scheme to.
        /// The contents of this enumerable depends on the user's provided command line flags.
        /// </summary>
        private static IEnumerable<IConsoleTarget> GetConsoleTargets()
        {
			_logger.LogDebug(">>> Entering GetConsoleTargets()");
            if (setDefaults)
            {
                yield return new DefaultConsoleTarget();
            }
            if (setProperties)
            {
                if (setUnixStyle)
                {
                    yield return new VirtualTerminalConsoleTarget();
                }
                else if (setTerminalStyle)
                {
                    yield return new TerminalSchemeConsoleTarget();
                }
                else
                {
                    yield return new CurrentConsoleTarget();
                }
            }
			_logger.LogDebug("<<< Exiting GetConsoleTargets()");
        }
    }
}
