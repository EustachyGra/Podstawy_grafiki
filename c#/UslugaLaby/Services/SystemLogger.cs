using System.Diagnostics;

namespace ServerLibrary
{
    public interface ISystemLogger
    {
        void Info(string message);
    }

    public class EventLogSystemLogger : ISystemLogger
    {
        private const string Source = "MojeZrodlo";
        private const string LogName = "MojLog";

        public EventLogSystemLogger()
        {
            if (!EventLog.SourceExists(Source))
            {
                EventLog.CreateEventSource(Source, LogName);
            }
        }
        /// <summary>
        /// Zapisuje informacyjną wiadomość do dziennika zdarzeń systemu Windows.
        /// </summary>
        /// <param name="message"></param>
        public void Info(string message)
        {
            EventLog.WriteEntry(Source, message, EventLogEntryType.Information);
        }
    }

}
