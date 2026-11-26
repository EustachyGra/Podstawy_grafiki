using System;
using System.Configuration;
using System.Diagnostics;
using System.IO;
using System.ServiceProcess;

namespace FileMonitorService
{
    public enum LogLevel
    {
        Off = 0,
        Error = 1,
        Warning = 2,
        Info = 3,
    }

    public partial class FileMonitorService : ServiceBase
    {
        private FileSystemWatcher watcher;
        private LogLevel logLevel;
        private string eventLogName;
        private string eventSource;
        TraceSwitch logSwitch;

        public FileMonitorService()
        {
            InitializeComponent();
        }

        protected override void OnStart(string[] args)
        {
            string path = ConfigurationManager.AppSettings["Sciezka"];
            eventLogName = ConfigurationManager.AppSettings["Dziennik"];
            eventSource = ConfigurationManager.AppSettings["Zrodlo"];
            logSwitch = new TraceSwitch("Logowanie", "Poziom logowania usługi");

            string logowanieValue = ConfigurationManager.GetSection("system.diagnostics/switches") is System.Collections.Hashtable switches &&
                                    switches.ContainsKey("Logowanie") ? switches["Logowanie"].ToString() : "Off";

            Enum.TryParse(logowanieValue, out logLevel);

            if (!EventLog.SourceExists(eventSource))
            {
                var sourceData = new EventSourceCreationData(eventSource, eventLogName);
                EventLog.CreateEventSource(sourceData);
            }
            else
            {
                string currentLog = EventLog.LogNameFromSourceName(eventSource, ".");
                if (currentLog != eventLogName)
                {
                    throw new ArgumentException(
                        $"Źródło '{eventSource}' istnieje, ale jest przypisane do logu '{currentLog}', a nie do '{eventLogName}'. Usuń źródło w rejestrze."
                    );
                }
            }

            EventLog.WriteEntry(eventSource, "Usługa została uruchomiona.", EventLogEntryType.Information);

            watcher = new FileSystemWatcher(path)
            {
                IncludeSubdirectories = true,
                EnableRaisingEvents = true,
                NotifyFilter = NotifyFilters.FileName | NotifyFilters.DirectoryName |
                               NotifyFilters.LastWrite | NotifyFilters.CreationTime |
                               NotifyFilters.Size | NotifyFilters.Attributes
            };

            watcher.Created += OnCreated;
            watcher.Deleted += OnDeleted;
            watcher.Changed += OnChanged;
            watcher.Renamed += OnRenamed;
        }

        protected override void OnStop()
        {
            EventLog.WriteEntry(eventSource, "Usługa została zatrzymana.", EventLogEntryType.Information);

            if (watcher != null)
            {
                watcher.EnableRaisingEvents = false;
                watcher.Created -= OnCreated;
                watcher.Changed -= OnChanged;
                watcher.Deleted -= OnDeleted;
                watcher.Renamed -= OnRenamed;
                watcher.Dispose();
                watcher = null;
            }
        }

        private void Log(string message, EventLogEntryType type, TraceLevel requiredLevel)
        {
            if (logSwitch.Level < requiredLevel)
                return;

            EventLog.WriteEntry(eventSource, message, type);
        }

        private void OnCreated(object sender, FileSystemEventArgs e)
        {
            Log($"Utworzono: {e.FullPath}", EventLogEntryType.Information, TraceLevel.Error);
        }

        private void OnDeleted(object sender, FileSystemEventArgs e)
        {
            Log($"Usunięto: {e.FullPath}", EventLogEntryType.Information, TraceLevel.Error);
        }

        private void OnChanged(object sender, FileSystemEventArgs e)
        {
            Log($"Zmieniono zawartość: {e.FullPath}", EventLogEntryType.Information, TraceLevel.Info);
        }

        private void OnRenamed(object sender, RenamedEventArgs e)
        {
            Log($"Zmieniono nazwę: {e.OldFullPath} → {e.FullPath}", EventLogEntryType.Information, TraceLevel.Warning);
        }
    }
}
