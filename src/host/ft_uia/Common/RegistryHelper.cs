//----------------------------------------------------------------------------------------------------------------------
// <copyright file="RegistryHelper.cs" company="Microsoft">
//     Copyright (c) Microsoft Corporation.  All rights reserved.
// </copyright>
// <summary>Console UI Automation registry manipulation</summary>
//----------------------------------------------------------------------------------------------------------------------
namespace Conhost.UIA.Tests.Common
{
    using System;
    using System.Diagnostics;
    using System.IO;

    using Microsoft.Win32;

    using WEX.Common.Managed;
    using WEX.TestExecution;
	using Microsoft.Extensions.Logging;

    public class RegistryHelper : IDisposable
    {
		private static readonly Microsoft.Extensions.Logging.ILogger _logger = Microsoft.Extensions.Logging.Abstractions.NullLogger.Instance;
        private static readonly string regExePath = Path.Combine(Environment.GetFolderPath(Environment.SpecialFolder.System), "reg.exe");
        private static readonly string regSaveCommand = @"export {0} {1}"; // 0 = path in reg, 1 = file name
        private static readonly string regDelCommand = @"delete {0} /f"; // 0 = path in reg
        private static readonly string regLoadCommand = @"import {0}"; // 0 = file

        private static readonly string regNode = @"HKCU\Console";
        private static readonly string regNodeVerbose = @"HKEY_CURRENT_USER\Console";

        private string backupFile;

        public RegistryHelper()
        {
		_logger.LogDebug(">>> Entering RegistryHelper()");

		_logger.LogDebug("<<< Exiting RegistryHelper()");
        }

        ~RegistryHelper()
        {
            this.Dispose(false);
        }

        public void Dispose()
        {
			_logger.LogDebug(">>> Entering Dispose()");
            this.Dispose(true);
            GC.SuppressFinalize(this);
			_logger.LogDebug("<<< Exiting Dispose()");
        }

        protected virtual void Dispose(bool disposing)
        {
			_logger.LogDebug(">>> Entering Dispose(disposing={Disposing})", disposing);
            if (disposing)
            {
                if (!string.IsNullOrEmpty(this.backupFile))
                {
                    this.RestoreRegistry();
                }
            }
			_logger.LogDebug("<<< Exiting Dispose(disposing={Disposing})", disposing);
        }

        public void SetDefaultValue(string valueName, object valueValue)
        {
			_logger.LogDebug(">>> Entering SetDefaultValue(valueName={ValueName},valueValue={ValueValue})", valueName, valueValue);
            AutoHelpers.LogInvariant("Setting registry key {0}'s value name {1} to value {2}", regNode, valueName, valueValue);
            Registry.SetValue(regNodeVerbose, valueName, valueValue);
			_logger.LogDebug("<<< Exiting SetDefaultValue(valueName={ValueName},valueValue={ValueValue})", valueName, valueValue);
        }

        public void BackupRegistry()
        {
			_logger.LogDebug(">>> Entering BackupRegistry()");
            AutoHelpers.LogInvariant("Save existing registry key status.");
            this.backupFile = Path.Combine(Path.GetTempPath(), Path.GetRandomFileName());

            AutoHelpers.LogInvariant("Backing up registry to file: {0}", backupFile);

            Verify.IsFalse(File.Exists(backupFile));

            string backupCmd = AutoHelpers.FormatInvariant(regSaveCommand, regNode, backupFile);

            AutoHelpers.LogInvariant("Calling command: {0} {1}", regExePath, backupCmd);

            Process regProc = Process.Start(regExePath, backupCmd);
			_logger.LogDebug("BackupRegistry(): regProc → {RegProc}", regProc);
            regProc.WaitForExit();

            Verify.IsTrue(File.Exists(backupFile));
			_logger.LogDebug("<<< Exiting BackupRegistry()");
        }

        private void DeleteRegistry()
        {
			_logger.LogDebug(">>> Entering DeleteRegistry()");
            AutoHelpers.LogInvariant("Deleting registry node: {0}", regNode);
            string deleteCmd = AutoHelpers.FormatInvariant(regDelCommand, regNode);

            Process regProc = Process.Start(regExePath, deleteCmd);
			_logger.LogDebug("DeleteRegistry(): regProc → {RegProc}", regProc);
            regProc.WaitForExit();
			_logger.LogDebug("<<< Exiting DeleteRegistry()");
        }

        public void RestoreRegistry()
        {
			_logger.LogDebug(">>> Entering RestoreRegistry()");
            AutoHelpers.LogInvariant("Restore settings to pre-test status.");
            this.DeleteRegistry();

            AutoHelpers.LogInvariant("Restoring registry from file: {0}", backupFile);

            Verify.IsTrue(File.Exists(backupFile));

            string restoreCmd = AutoHelpers.FormatInvariant(regLoadCommand, backupFile);

            Process regProc = Process.Start(regExePath, restoreCmd);
			_logger.LogDebug("RestoreRegistry(): regProc → {RegProc}", regProc);
            regProc.WaitForExit();

            File.Delete(backupFile);

            this.backupFile = null;
			_logger.LogDebug("RestoreRegistry(): this.backupFile → {Arg0}", this.backupFile);
			_logger.LogDebug("<<< Exiting RestoreRegistry()");
        }

        public RegistryKey GetMatchingKey(OpenTarget target)
        {
			_logger.LogDebug(">>> Entering GetMatchingKey(target={Target})", target);
            switch (target)
            {
                case OpenTarget.Defaults:
                    return Registry.CurrentUser.OpenSubKey(@"Console");
                case OpenTarget.Specifics:
                    return Registry.CurrentUser.OpenSubKey(@"Console").OpenSubKey("%SystemRoot%_system32_cmd.exe");
                default:
					_logger.LogWarning("Throwing NotImplementedException from GetMatchingKey(target={Target})", target);
			_logger.LogDebug("<<< Exiting GetMatchingKey(target={Target})", target);
                    throw new NotImplementedException(AutoHelpers.FormatInvariant("This type of registry key isn't implemented: {0}", target.ToString()));
            }
        }
    }
}
