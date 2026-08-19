//
// Copyright (C) Microsoft.  All rights reserved.
// Licensed under the terms described in the LICENSE file in the root of this project.
//

using System;
using System.Collections.Generic;
using System.IO;
using System.Linq;
using System.Runtime.Serialization.Json;
using System.Xml;
using static ColorTool.ConsoleAPI;
using Microsoft.Extensions.Logging;

namespace ColorTool.SchemeParsers
{
    class JsonParser : SchemeParserBase
    {
		private static readonly Microsoft.Extensions.Logging.ILogger _logger = Microsoft.Extensions.Logging.Abstractions.NullLogger.Instance;
        public override string FileExtension { get; } = ".json";
        private static readonly IReadOnlyList<string> ConcfgColorNames = new[]
        {
            "black",        // Dark Black
            "dark_blue",    // Dark Blue
            "dark_green",   // Dark Green
            "dark_cyan",    // Dark Cyan
            "dark_red",     // Dark Red
            "dark_magenta", // Dark Magenta
            "dark_yellow",  // Dark Yellow
            "gray",         // Dark White
            "dark_gray",    // Bright Black
            "blue",         // Bright Blue
            "green",        // Bright Green
            "cyan",         // Bright Cyan
            "red",          // Bright Red
            "magenta",      // Bright Magenta
            "yellow",       // Bright Yellow
            "white"         // Bright White
        };

        public override string Name { get; } = "concfg Parser";

        public override ColorScheme ParseScheme(string schemeName, bool reportErrors = false)
        {
			_logger.LogDebug(">>> Entering ParseScheme(schemeName={SchemeName},reportErrors={ReportErrors})", schemeName, reportErrors);
            XmlDocument xmlDoc = LoadJsonFile(schemeName);
            if (xmlDoc == null) return null;
			_logger.LogDebug("ParseScheme(schemeName={SchemeName},reportErrors={ReportErrors}): xmlDoc → {XmlDoc}", schemeName, reportErrors, xmlDoc);

            try
            {
                XmlNode root = xmlDoc.DocumentElement;
                XmlNodeList children = root.ChildNodes;
                uint[] colorTable = new uint[ColorTableSize]; ;
                for (int i = 0; i < ColorTableSize; i++)
                {
                    string name = ConcfgColorNames[i];
                    var node = children.OfType<XmlNode>().Where(n => n.Name == name).Single();
					_logger.LogDebug("ParseScheme(schemeName={SchemeName},reportErrors={ReportErrors}): node → {Node}", schemeName, reportErrors, node);
                    colorTable[i] = ParseColor(node.InnerText);
                }


                uint? popupForeground = null;
                uint? popupBackground = null;

                var popupNode = children.OfType<XmlNode>().Where(n => n.Name == "popup_colors").SingleOrDefault();
				_logger.LogDebug("ParseScheme(schemeName={SchemeName},reportErrors={ReportErrors}): popupNode → {PopupNode}", schemeName, reportErrors, popupNode);
                if (popupNode != null)
                {
                    var parts = popupNode.InnerText.Split(',');
					_logger.LogDebug("ParseScheme(schemeName={SchemeName},reportErrors={ReportErrors}): parts → {Parts}", schemeName, reportErrors, parts);
                    if (parts.Length == 2)
                    {
                        var foregroundIndex = (ConcfgColorNames as IList<string>).IndexOf(parts[0]);
						_logger.LogDebug("ParseScheme(schemeName={SchemeName},reportErrors={ReportErrors}): foregroundIndex → {ForegroundIndex}", schemeName, reportErrors, foregroundIndex);
                        var backgroundIndex = (ConcfgColorNames as IList<string>).IndexOf(parts[1]);
						_logger.LogDebug("ParseScheme(schemeName={SchemeName},reportErrors={ReportErrors}): backgroundIndex → {BackgroundIndex}", schemeName, reportErrors, backgroundIndex);
                        if (foregroundIndex != -1 && backgroundIndex != -1)
                        {
                            popupForeground = colorTable[foregroundIndex];
							_logger.LogDebug("ParseScheme(schemeName={SchemeName},reportErrors={ReportErrors}): popupForeground → {PopupForeground}", schemeName, reportErrors, popupForeground);
                            popupBackground = colorTable[backgroundIndex];
							_logger.LogDebug("ParseScheme(schemeName={SchemeName},reportErrors={ReportErrors}): popupBackground → {PopupBackground}", schemeName, reportErrors, popupBackground);
                        }
                    }
                }

                uint? screenForeground = null;
                uint? screenBackground = null;

                var screenNode = children.OfType<XmlNode>().Where(n => n.Name == "screen_colors").SingleOrDefault();
				_logger.LogDebug("ParseScheme(schemeName={SchemeName},reportErrors={ReportErrors}): screenNode → {ScreenNode}", schemeName, reportErrors, screenNode);
                if (screenNode != null)
                {
                    var parts = screenNode.InnerText.Split(',');
                    if (parts.Length == 2)
                    {
                        var foregroundIndex = (ConcfgColorNames as IList<string>).IndexOf(parts[0]);
                        var backgroundIndex = (ConcfgColorNames as IList<string>).IndexOf(parts[1]);
                        if (foregroundIndex != -1 && backgroundIndex != -1)
                        {
                            screenForeground = colorTable[foregroundIndex];
							_logger.LogDebug("ParseScheme(schemeName={SchemeName},reportErrors={ReportErrors}): screenForeground → {ScreenForeground}", schemeName, reportErrors, screenForeground);
                            screenBackground = colorTable[backgroundIndex];
							_logger.LogDebug("ParseScheme(schemeName={SchemeName},reportErrors={ReportErrors}): screenBackground → {ScreenBackground}", schemeName, reportErrors, screenBackground);
                        }
                    }
                }

                var consoleAttributes = new ConsoleAttributes(screenBackground, screenForeground, popupBackground, popupForeground, null);
				_logger.LogDebug("ParseScheme(schemeName={SchemeName},reportErrors={ReportErrors}): consoleAttributes → {ConsoleAttributes}", schemeName, reportErrors, consoleAttributes);
                return new ColorScheme(ExtractSchemeName(schemeName), colorTable, consoleAttributes);
            }
            catch (Exception /*e*/)
            {
                if (reportErrors)
                {
                    Console.WriteLine("failed to load json scheme");
                }

				_logger.LogDebug("<<< Exiting ParseScheme(schemeName={SchemeName},reportErrors={ReportErrors})", schemeName, reportErrors);
                return null;
            }
        }

        private static uint ParseColor(string arg)
        {
			_logger.LogDebug(">>> Entering ParseColor(arg={Arg})", arg);
            System.Drawing.Color col = System.Drawing.ColorTranslator.FromHtml(arg);
			_logger.LogDebug("ParseColor(arg={Arg}): col → {Col}", arg, col);
			_logger.LogDebug("<<< Exiting ParseColor(arg={Arg})", arg);
            return RGB(col.R, col.G, col.B);
        }

        private XmlDocument LoadJsonFile(string schemeName) =>
            SchemeManager
            .GetSearchPaths(schemeName, FileExtension)
            .Select(path =>
            {
                try
                {
                    var data = File.ReadAllBytes(path);
                    var reader = JsonReaderWriterFactory.CreateJsonReader(data, XmlDictionaryReaderQuotas.Max);
                    var xmlDoc = new XmlDocument();
                    xmlDoc.Load(reader);
                    return xmlDoc;
                }
                catch (XmlException) { }
                catch (IOException) { }
                catch (UnauthorizedAccessException) { }
                catch (Exception e)
                {
                    Console.WriteLine($"Unexpected Exception: {e}.\nBailing...");
                    throw;
                }
                return null;
            })
            .FirstOrDefault(x => x != null);
			_logger.LogDebug("<<< Exiting LoadJsonFile(schemeName={SchemeName})", schemeName);
    }
}
