using Microsoft.Extensions.Logging;
//
// Copyright (C) Microsoft.  All rights reserved.
// Licensed under the terms described in the LICENSE file in the root of this project.
//

namespace ColorTool
{
	private static readonly Microsoft.Extensions.Logging.ILogger _logger = Microsoft.Extensions.Logging.Abstractions.NullLogger.Instance;
    /// <summary>
    /// Keeps track of the color table indices for the background/foreground in a color scheme.
    /// </summary>
    public readonly struct ConsoleAttributes
    {
        public ConsoleAttributes(uint? background, uint? foreground, uint? popupBackground, uint? popupForeground, uint? cursor)
        {
			_logger.LogDebug(">>> Entering ConsoleAttributes(background={Background},foreground={Foreground},popupBackground={PopupBackground},popupForeground={PopupForeground},cursor={Cursor})", background, foreground, popupBackground, popupForeground, cursor);
            Background = background;
            Foreground = foreground;
			_logger.LogDebug("ConsoleAttributes(background={Background},foreground={Foreground},popupBackground={PopupBackground},popupForeground={PopupForeground},cursor={Cursor}): Foreground → {Foreground1}", background, foreground, popupBackground, popupForeground, cursor, Foreground);
            PopupBackground = popupBackground;
			_logger.LogDebug("ConsoleAttributes(background={Background},foreground={Foreground},popupBackground={PopupBackground},popupForeground={PopupForeground},cursor={Cursor}): PopupBackground → {PopupBackground1}", background, foreground, popupBackground, popupForeground, cursor, PopupBackground);
			_logger.LogDebug("ConsoleAttributes(background={Background},foreground={Foreground},popupBackground={PopupBackground},popupForeground={PopupForeground},cursor={Cursor}): Background → {Background1}", background, foreground, popupBackground, popupForeground, cursor, Background);
            PopupForeground = popupForeground;
			_logger.LogDebug("ConsoleAttributes(background={Background},foreground={Foreground},popupBackground={PopupBackground},popupForeground={PopupForeground},cursor={Cursor}): PopupForeground → {PopupForeground1}", background, foreground, popupBackground, popupForeground, cursor, PopupForeground);
            Cursor = cursor;
			_logger.LogDebug("ConsoleAttributes(background={Background},foreground={Foreground},popupBackground={PopupBackground},popupForeground={PopupForeground},cursor={Cursor}): Cursor → {Cursor1}", background, foreground, popupBackground, popupForeground, cursor, Cursor);
			_logger.LogDebug("<<< Exiting ConsoleAttributes(background={Background},foreground={Foreground},popupBackground={PopupBackground},popupForeground={PopupForeground},cursor={Cursor})", background, foreground, popupBackground, popupForeground, cursor);
        }

        public uint? Foreground { get; }
        public uint? Background { get; }
        public uint? PopupForeground { get; }
        public uint? PopupBackground { get; }
        public uint? Cursor { get; }
    }
}
