using ServerLibrary;
using System;
using System.ServiceProcess;
using System.Threading.Tasks;

namespace UslugaLaby
{
    public partial class MyNewService : ServiceBase
    {
        private ISystemLogger eventLog;
        private ChatServer server;
        private DatabaseManager dbManager;

        public MyNewService()
        {
            dbManager = new DatabaseManager();
            InitializeComponent();
            server = new ChatServer();
            eventLog = new EventLogSystemLogger();
        }
        /// <summary>
        /// Uruchamia usługę, inicjalizując serwer czatu i logując zdarzenia do dziennika systemowego.
        /// </summary>
        protected override void OnStart(string[] args)
        {
            eventLog.Info("Uruchomianie usługi");
            _ = Task.Run(async () =>
            {
                try
                {
                    await server.StartAsync();
                }
                catch (Exception ex)
                {
                    eventLog.Info($"Błąd podczas uruchamiania serwera: {ex}");
                    Stop();
                }
            });
        }
        /// <summary>
        /// Obługuje zatrzymanie usługi, logując odpowiednie zdarzenie do dziennika systemowego.
        /// </summary>
        protected override void OnStop()
        {
            eventLog.Info("Zatrzymano usługę");
        }
    }
}