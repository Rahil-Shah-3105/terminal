//
// Copyright (C) Microsoft.  All rights reserved.
// Licensed under the terms described in the LICENSE file in the root of this project.
//

using System;
using System.Drawing;
using System.Linq;
using Microsoft.Extensions.Logging;

namespace ColorTool
{
    /// <summary>
    /// Represents a color scheme that can be applied to a console.
    /// </summary>
    public class ColorScheme
    {
		private static readonly Microsoft.Extensions.Logging.ILogger _logger = Microsoft.Extensions.Logging.Abstractions.NullLogger.Instance;
        public ColorScheme(string name, uint[] colorTable, ConsoleAttributes consoleAttributes)
        {
			_logger.LogDebug(">>> Entering ColorScheme(name={Name},colorTable={ColorTable},consoleAttributes={ConsoleAttributes})", name, colorTable, consoleAttributes);
            Name = name;
            ColorTable = colorTable;
			_logger.LogDebug("ColorScheme(name={Name},colorTable={ColorTable},consoleAttributes={ConsoleAttributes}): Name → {Name1}", name, colorTable, consoleAttributes, Name);
			_logger.LogDebug("ColorScheme(name={Name},colorTable={ColorTable},consoleAttributes={ConsoleAttributes}): ColorTable → {ColorTable1}", name, colorTable, consoleAttributes, ColorTable);
            ConsoleAttributes = consoleAttributes;
			_logger.LogDebug("ColorScheme(name={Name},colorTable={ColorTable},consoleAttributes={ConsoleAttributes}): ConsoleAttributes → {ConsoleAttributes1}", name, colorTable, consoleAttributes, ConsoleAttributes);
			_logger.LogDebug("<<< Exiting ColorScheme(name={Name},colorTable={ColorTable},consoleAttributes={ConsoleAttributes})", name, colorTable, consoleAttributes);
        }

        public string Name { get; }
        public uint[] ColorTable { get; }
        public ConsoleAttributes ConsoleAttributes { get; }

        public ushort? ScreenColorAttributes =>
            CalculateBackgroundForegroundAttributes(
                this.ConsoleAttributes.Background,
                this.ConsoleAttributes.Foreground
            );

        public ushort? PopupColorAttributes =>
            CalculateBackgroundForegroundAttributes(
                this.ConsoleAttributes.PopupBackground,
                this.ConsoleAttributes.PopupForeground
            );

        public Color this[int index] => UIntToColor(ColorTable[index]);

        public Color Foreground => UIntToColor(ConsoleAttributes.Foreground.Value);
        public Color Background => UIntToColor(ConsoleAttributes.Background.Value);
        public Color Cursor => UIntToColor(ConsoleAttributes.Cursor.Value);

        private static Color UIntToColor(uint color)
        {
			_logger.LogDebug(">>> Entering UIntToColor(color={Color})", color);
            byte r = (byte)(color >> 0);
            byte g = (byte)(color >> 8);
			_logger.LogDebug("UIntToColor(color={Color}): g → {G}", color, g);
            byte b = (byte)(color >> 16);
			_logger.LogDebug("UIntToColor(color={Color}): b → {B}", color, b);
			_logger.LogDebug("UIntToColor(color={Color}): r → {R}", color, r);
			_logger.LogDebug("<<< Exiting UIntToColor(color={Color})", color);
            return Color.FromArgb(r, g, b);
        }

        private ushort? CalculateBackgroundForegroundAttributes(uint? background, uint? foreground)
        {
			_logger.LogDebug(">>> Entering CalculateBackgroundForegroundAttributes(background={Background},foreground={Foreground})", background, foreground);
            if(!(background.HasValue && foreground.HasValue))
            {
                return null;
            }
            int fgidx = this.CalculateIndex(foreground.Value);
			_logger.LogDebug("CalculateBackgroundForegroundAttributes(background={Background},foreground={Foreground}): fgidx → {Fgidx}", background, foreground, fgidx);
            int bgidx = this.CalculateIndex(background.Value);
			_logger.LogDebug("CalculateBackgroundForegroundAttributes(background={Background},foreground={Foreground}): bgidx → {Bgidx}", background, foreground, bgidx);
            var attributes = (ushort)(fgidx | (bgidx << 4));
			_logger.LogDebug("CalculateBackgroundForegroundAttributes(background={Background},foreground={Foreground}): attributes → {Attributes}", background, foreground, attributes);
			_logger.LogDebug("<<< Exiting CalculateBackgroundForegroundAttributes(background={Background},foreground={Foreground})", background, foreground);
            return attributes;
        }

        public int CalculateIndex(uint value) =>
            ColorTable.Select((color, idx) => Tuple.Create(color, idx))
                      .OrderBy(Difference(value))
                      .First().Item2;

        private static Func<Tuple<uint, int>, double> Difference(uint c1) =>
            // heuristic 1: nearest neighbor in RGB space
            // tup => Distance(RGB(c1), RGB(tup.Item1));
            // heuristic 2: nearest neighbor in RGB space
            // tup => Distance(HSV(c1), HSV(tup.Item1));
            // heuristic 3: weighted RGB L2 distance
            tup => WeightedRGBSimilarity(c1, tup.Item1);

        private static double WeightedRGBSimilarity(uint c1, uint c2)
        {
			_logger.LogDebug(">>> Entering WeightedRGBSimilarity(c1={C1},c2={C2})", c1, c2);
            var rgb1 = RGB(c1);
            var rgb2 = RGB(c2);
			_logger.LogDebug("WeightedRGBSimilarity(c1={C1},c2={C2}): rgb2 → {Rgb2}", c1, c2, rgb2);
            var dist = rgb1.Zip(rgb2, (a, b) => Math.Pow((int)a - (int)b, 2)).ToArray();
			_logger.LogDebug("WeightedRGBSimilarity(c1={C1},c2={C2}): dist → {Dist}", c1, c2, dist);
            var rbar = (rgb1[0] + rgb1[0]) / 2.0;
			_logger.LogDebug("WeightedRGBSimilarity(c1={C1},c2={C2}): rbar → {Rbar}", c1, c2, rbar);
			_logger.LogDebug("WeightedRGBSimilarity(c1={C1},c2={C2}): rgb1 → {Rgb1}", c1, c2, rgb1);
			_logger.LogDebug("<<< Exiting WeightedRGBSimilarity(c1={C1},c2={C2})", c1, c2);
            return Math.Sqrt(dist[0] * (2 + rbar / 256.0) + dist[1] * 4 + dist[2] * (2 + (255 - rbar) / 256.0));
        }

        internal static uint[] RGB(uint c) => new[] { c & 0xFF, (c >> 8) & 0xFF, (c >> 16) & 0xFF };

        internal static uint[] HSV(uint c)
        {
			_logger.LogDebug(">>> Entering HSV(c={C})", c);
            var rgb = RGB(c).Select(_ => (int)_).ToArray();
            int max = rgb.Max();
			_logger.LogDebug("HSV(c={C}): max → {Max}", c, max);
            int min = rgb.Min();
			_logger.LogDebug("HSV(c={C}): min → {Min}", c, min);
			_logger.LogDebug("HSV(c={C}): rgb → {Rgb}", c, rgb);

            int d = max - min;
            int h = 0;
			_logger.LogDebug("HSV(c={C}): h → {H}", c, h);
            int s = (int)(255 * ((max == 0) ? 0 : d / (double)max));
            int v = max;
			_logger.LogDebug("HSV(c={C}): s → {S}", c, s);

            if (d != 0)
            {
                double dh;
                if (rgb[0] == max) dh = ((rgb[1] - rgb[2]) / (double)d);
                else if (rgb[1] == max) dh = 2.0 + ((rgb[2] - rgb[0]) / (double)d);
                else /* if (rgb[2] == max) */ dh = 4.0 + ((rgb[0] - rgb[1]) / (double)d);
                dh *= 60;
                if (dh < 0) dh += 360.0;
                h = (int)(dh * 255.0 / 360.0);
            }

			_logger.LogDebug("<<< Exiting HSV(c={C})", c);
            return new[] { (uint)h, (uint)s, (uint)v };
        }

        internal void Dump()
        {
			_logger.LogDebug(">>> Entering Dump()");
            Action<string, uint> _dump = (str, c) =>
            {
                var rgb = RGB(c);
				_logger.LogDebug("Dump(): rgb → {Rgb}", rgb);
                var hsv = HSV(c);
				_logger.LogDebug("Dump(): hsv → {Hsv}", hsv);
                Console.WriteLine($"{str} =\tRGB({rgb[0]}, {rgb[1]}, {rgb[2]}),\tHSV({hsv[0]}, {hsv[1]}, {hsv[2]})");
            };

            for (int i = 0; i < 16; ++i)
            {
                _dump($"Color[{i}]", ColorTable[i]);
            }

            if (ConsoleAttributes.Foreground != null)
            {
                _dump("FG       ", ConsoleAttributes.Foreground.Value);
            }

            if (ConsoleAttributes.Background != null)
            {
                _dump("BG       ", ConsoleAttributes.Background.Value);
            }
			_logger.LogDebug("<<< Exiting Dump()");
        }
    }
}
