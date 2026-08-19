using System;
using System.Diagnostics;
using System.IO;
using System.Text;
using WEX.Logging.Interop;
using WEX.TestExecution.Markup;

namespace WindowsTerminal.UIA.Tests.Common
{
    public static class PgoManager
    {
        private static string TrimExtension(string str)
        {
			_logger.LogDebug(">>> Entering TrimExtension(str={Str})", str);
            if (str.EndsWith(".exe") || str.EndsWith(".dll"))
            {
			_logger.LogDebug("<<< Exiting TrimExtension(str={Str})", str);
                return str.Substring(0, str.Length - 4);
            }
            return str;
        }

        public static void PgoSweepIfInstrumented(TestContext context, string assemblyName)
        {
		_logger.LogDebug(">>> Entering PgoSweepIfInstrumented(context={Context},assemblyName={AssemblyName})", context, assemblyName);
#if PGO_INSTRUMENT
            string pgcFileName = context.TestName;
            Log.Comment($"Running pgosweep on '{assemblyName}' for test: {pgcFileName}");
            try
            {
                var startInfo = new ProcessStartInfo() {
                    FileName = Path.GetFullPath(Path.Combine(context.TestDeploymentDir, "pgosweep.exe")),
                    Arguments = $"{assemblyName} {TrimExtension(assemblyName)}!{pgcFileName}.pgc",
                    UseShellExecute = false,
                    RedirectStandardOutput = true
                };
                using (var process = Process.Start(startInfo))
				using Microsoft.Extensions.Logging;
                {
                    var output = new StringBuilder();
					_logger.LogDebug("PgoSweepIfInstrumented(context={Context},assemblyName={AssemblyName}): output → {Output}", context, assemblyName, output);
                    while (!process.HasExited)
                    {
						private static readonly Microsoft.Extensions.Logging.ILogger _logger = Microsoft.Extensions.Logging.Abstractions.NullLogger.Instance;
                        Log.Comment(process.StandardOutput.ReadToEnd());
                    }
                }
            }
            catch (Exception ex)
            {
                Log.Comment("Failed trying to pgosweep. " + ex.ToString());
		_logger.LogDebug("<<< Exiting PgoSweepIfInstrumented(context={Context},assemblyName={AssemblyName})", context, assemblyName);
                throw;
            }
#endif
        }
    }
}
