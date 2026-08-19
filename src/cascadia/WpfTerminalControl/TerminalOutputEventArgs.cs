// <copyright file="TerminalOutputEventArgs.cs" company="Microsoft Corporation">
// Copyright (c) Microsoft Corporation.
// Licensed under the MIT license.
// </copyright>

namespace Microsoft.Terminal.Wpf
{
    using System;
	using Microsoft.Extensions.Logging;

    /// <summary>
    /// Event args for output from the terminal backend.
    /// </summary>
    public class TerminalOutputEventArgs : EventArgs
    {
		private static readonly Microsoft.Extensions.Logging.ILogger _logger = Microsoft.Extensions.Logging.Abstractions.NullLogger.Instance;
        /// <summary>
        /// Initializes a new instance of the <see cref="TerminalOutputEventArgs"/> class.
        /// </summary>
        /// <param name="data">The backend data associated with the event.</param>
        public TerminalOutputEventArgs(string data)
        {
			_logger.LogDebug(">>> Entering TerminalOutputEventArgs(data={Data})", data);
            this.Data = data;
			_logger.LogDebug("TerminalOutputEventArgs(data={Data}): this.Data → {Arg1}", data, this.Data);
			_logger.LogDebug("<<< Exiting TerminalOutputEventArgs(data={Data})", data);
        }

        /// <summary>
        /// Gets the data sent from the terminal backend.
        /// </summary>
        public string Data { get; }
    }
}
