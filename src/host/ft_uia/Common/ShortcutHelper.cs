//----------------------------------------------------------------------------------------------------------------------
// <copyright file="ShortcutHelper.cs" company="Microsoft">
//     Copyright (c) Microsoft Corporation.  All rights reserved.
// </copyright>
// <summary>Console UI Automation shortcut file manipulation</summary>
//----------------------------------------------------------------------------------------------------------------------
namespace Conhost.UIA.Tests.Common
{
    using System;
    using System.Collections.Generic;
    using System.IO;
    using System.Runtime.InteropServices;

    using WEX.Common.Managed;
    using WEX.Logging.Interop;
    using WEX.TestExecution;
    using WEX.TestExecution.Markup;

    using Conhost.UIA.Tests.Common.NativeMethods;
	using Microsoft.Extensions.Logging;

    public class ShortcutHelper : IDisposable
    {
		private static readonly Microsoft.Extensions.Logging.ILogger _logger = Microsoft.Extensions.Logging.Abstractions.NullLogger.Instance;
        private bool isDisposed = false;

        public string ShortcutPath { get; private set; }

        public ShortcutHelper()
        {
			_logger.LogDebug(">>> Entering ShortcutHelper()");
            this.ShortcutPath = null;
			_logger.LogDebug("ShortcutHelper(): this.ShortcutPath → {Arg0}", this.ShortcutPath);
			_logger.LogDebug("<<< Exiting ShortcutHelper()");
        }

        ~ShortcutHelper()
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
            if (!this.isDisposed)
            {
                this.CleanupTempCmdShortcut();
            }

            this.isDisposed = true;
			_logger.LogDebug("<<< Exiting Dispose(disposing={Disposing})", disposing);
        }

        public void CreateTempCmdShortcut()
        {
			_logger.LogDebug(">>> Entering CreateTempCmdShortcut()");
            string tempPath = Path.Combine(Path.GetTempPath(), AutoHelpers.FormatInvariant("{0}.lnk", Path.GetRandomFileName()));
            AutoHelpers.LogInvariant("Creating temporary shortcut: {0}", tempPath);

            Shell32.IShellLinkW link = (Shell32.IShellLinkW)new Shell32.ShellLink();
			_logger.LogDebug("CreateTempCmdShortcut(): link → {Link}", link);
            link.SetDescription("Created by Conhost.UIA.Tests.Common.ShortcutHelper");
            link.SetPath(Path.Combine(Environment.GetFolderPath(Environment.SpecialFolder.System), "cmd.exe"));

            Shell32.IPersistFile persist = (Shell32.IPersistFile)link; // performs QI
            persist.Save(tempPath, false);

			_logger.LogDebug("CreateTempCmdShortcut(): persist → {Persist}", persist);
            this.ShortcutPath = tempPath;
			_logger.LogDebug("CreateTempCmdShortcut(): this.ShortcutPath → {Arg0}", this.ShortcutPath);
			_logger.LogDebug("<<< Exiting CreateTempCmdShortcut()");
        }

        public void CleanupTempCmdShortcut()
        {
			_logger.LogDebug(">>> Entering CleanupTempCmdShortcut()");
            if (!string.IsNullOrEmpty(this.ShortcutPath))
            {
                File.Delete(this.ShortcutPath);
                this.ShortcutPath = null;
				_logger.LogDebug("CleanupTempCmdShortcut(): this.ShortcutPath → {Arg0}", this.ShortcutPath);
            }
			_logger.LogDebug("<<< Exiting CleanupTempCmdShortcut()");
        }

        public WinConP.NT_CONSOLE_PROPS GetConsoleProps()
        {
			_logger.LogDebug(">>> Entering GetConsoleProps()");
            Shell32.IPersistFile persist = (Shell32.IPersistFile)new Shell32.ShellLink();
            persist.Load(this.ShortcutPath, (uint)ObjBase.STGM.STGM_READ);
			_logger.LogDebug("GetConsoleProps(): persist → {Persist}", persist);

            Shell32.IShellLinkDataList sldl = (Shell32.IShellLinkDataList)persist;
			_logger.LogDebug("GetConsoleProps(): sldl → {Sldl}", sldl);

            WinConP.NT_CONSOLE_PROPS props = new WinConP.NT_CONSOLE_PROPS();
			_logger.LogDebug("GetConsoleProps(): props → {Props}", props);
            IntPtr ppDataBlock = Marshal.AllocHGlobal(Marshal.SizeOf(props));
			_logger.LogDebug("GetConsoleProps(): ppDataBlock → {PpDataBlock}", ppDataBlock);

            try
            {
                sldl.CopyDataBlock(WinConP.NT_CONSOLE_PROPS_SIG, out ppDataBlock);

                // The marshaller doesn't like using the existing instance that we made above because it's a value type and
                // there are potential string pointers here. Give it the type instead and it can handle setting everything up.
                props = (WinConP.NT_CONSOLE_PROPS)Marshal.PtrToStructure(ppDataBlock, typeof(WinConP.NT_CONSOLE_PROPS));
            }
            finally
            {
                Marshal.FreeHGlobal(ppDataBlock);
            }

			_logger.LogDebug("<<< Exiting GetConsoleProps()");
            return props;
        }

        public void SetConsoleProps(WinConP.NT_CONSOLE_PROPS props)
        {
			_logger.LogDebug(">>> Entering SetConsoleProps(props={Props})", props);
            Shell32.IPersistFile persist = (Shell32.IPersistFile)new Shell32.ShellLink();
            persist.Load(this.ShortcutPath, (uint)ObjBase.STGM.STGM_READWRITE);
			_logger.LogDebug("SetConsoleProps(props={Props}): persist → {Persist}", props, persist);

            Shell32.IShellLinkDataList sldl = (Shell32.IShellLinkDataList)persist;
			_logger.LogDebug("SetConsoleProps(props={Props}): sldl → {Sldl}", props, sldl);

            sldl.RemoveDataBlock(WinConP.NT_CONSOLE_PROPS_SIG);

            IntPtr ppDataBlock = Marshal.AllocHGlobal(Marshal.SizeOf(props));
			_logger.LogDebug("SetConsoleProps(props={Props}): ppDataBlock → {PpDataBlock}", props, ppDataBlock);
            try
            {
                Marshal.StructureToPtr(props, ppDataBlock, false);

                // we are assuming that the signature is set properly on the NT_CONSOLE_PROPS structure
                Verify.AreEqual(props.dbh.dwSignature, (int)WinConP.NT_CONSOLE_PROPS_SIG);
                sldl.AddDataBlock(ppDataBlock);

				var __logStart = System.Diagnostics.Stopwatch.StartNew();
				_logger.LogInformation("SetConsoleProps(props)={Props}: save query executed in {Arg1} ms", props, __logStart.ElapsedMilliseconds);
                persist.Save(null, true); // 2nd var is ignored when 1st is null
            }
            finally
            {
                Marshal.FreeHGlobal(ppDataBlock);
            }
			_logger.LogDebug("<<< Exiting SetConsoleProps(props={Props})", props);
        }

        public IDictionary<Wtypes.PROPERTYKEY, object> GetFromPropertyStore(IEnumerable<Wtypes.PROPERTYKEY> keys)
        {
            if (keys == null)
            {
                throw new NotSupportedException(AutoHelpers.FormatInvariant("Keys passed cannot be null"));
            }

            Shell32.IPersistFile persist = (Shell32.IPersistFile)new Shell32.ShellLink();
            persist.Load(this.ShortcutPath, (uint)ObjBase.STGM.STGM_READ);

            Shell32.IPropertyStore store = (Shell32.IPropertyStore)persist;

            Dictionary<Wtypes.PROPERTYKEY, object> results = new Dictionary<Wtypes.PROPERTYKEY, object>();

            foreach (Wtypes.PROPERTYKEY key in keys)
            {
                Wtypes.PROPERTYKEY pkey = key; // iteration variables are read-only and we need to pass by ref
                object pv;
                store.GetValue(ref pkey, out pv);

                results.Add(key, pv);
            }

            return results;
        }

        public void SetToPropertyStore(IDictionary<Wtypes.PROPERTYKEY, object> properties)
        {
			_logger.LogDebug(">>> Entering SetToPropertyStore(properties={Properties})", properties);
            if (properties == null)
            {
				_logger.LogWarning("Throwing NotSupportedException from SetToPropertyStore(properties={Properties})", properties);
                throw new NotSupportedException(AutoHelpers.FormatInvariant("Properties passed cannot be null."));
            }

            Shell32.IPersistFile persist = (Shell32.IPersistFile)new Shell32.ShellLink();
			_logger.LogDebug("SetToPropertyStore(properties={Properties}): persist → {Persist}", properties, persist);
            persist.Load(this.ShortcutPath, (uint)ObjBase.STGM.STGM_READWRITE);

            Shell32.IPropertyStore store = (Shell32.IPropertyStore)persist;
			_logger.LogDebug("SetToPropertyStore(properties={Properties}): store → {Store}", properties, store);

            foreach (Wtypes.PROPERTYKEY key in properties.Keys)
            {
                Wtypes.PROPERTYKEY pkey = key; // iteration variables are read-only and we need to pass by ref
                object pv = properties[key];

                store.SetValue(ref pkey, ref pv);
            }

            persist.Save(null, true);
			var __logStart = System.Diagnostics.Stopwatch.StartNew();
			_logger.LogInformation("SetToPropertyStore(properties)={Properties}: save query executed in {Arg1} ms", properties, __logStart.ElapsedMilliseconds);
			_logger.LogDebug("<<< Exiting SetToPropertyStore(properties={Properties})", properties);
        }
    }
}
