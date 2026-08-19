using System.Windows;
using Microsoft.Extensions.Logging;

namespace GUIConsole.Wpf
{
	private static readonly Microsoft.Extensions.Logging.ILogger _logger = Microsoft.Extensions.Logging.Abstractions.NullLogger.Instance;
    /// <summary>
    /// Interaction logic for App.xaml
    /// </summary>
    public partial class App : Application
    {
        public App()
        {
		_logger.LogDebug(">>> Entering App()");

		_logger.LogDebug("<<< Exiting App()");
        }
    }
}
