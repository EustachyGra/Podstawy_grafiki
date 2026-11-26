using System;
using System.Collections.Generic;
using System.Diagnostics;
using System.IO;
using System.Net;
using System.Net.Sockets;
using System.Threading;
using System.Threading.Tasks;

namespace Client
{
    public interface ISystemLogger
    {
        void Info(string message);
    }

    public class EventLogSystemLogger : ISystemLogger
    {
        private const string Source = "ClientZrodlo";
        private const string LogName = "MojLog";
        private EventLog eventLog;
        public EventLogSystemLogger()
        {
            if (!EventLog.SourceExists(Source))
                EventLog.CreateEventSource(Source, LogName);
            eventLog = new EventLog();
            eventLog.Source = Source;
            eventLog.Log = LogName;
        }

        public void Info(string message)
        {
            eventLog.WriteEntry(message, EventLogEntryType.Information);
        }
    }
}