using System;
using System.Configuration;
using System.Net;
using System.Net.Sockets;
using System.Threading.Tasks;

namespace ServerLibrary
{
    public class ChatServer
    {
        private TcpListener listener;
        private IDatabaseManager DBManager;
        private ISystemLogger eventLog;
        int port;
        /// <summary>
        /// Uruchamia serwer, inicjalizuje bazę danych i rozpoczyna asynchroniczne akceptowanie połączeń klientów.
        /// </summary>
        /// <remarks>
        /// Ta metoda uruchamia nasłuchiwacz serwera i asynchronicznie inicjalizuje bazę danych. 
        /// Od momentu uruchomienia serwera, serwer ciągle akceptuje przychodzące połączenia klientów i obsługuje je w oddzielnych zadaniach.</remarks>
        public async Task StartAsync()
        {
            try
            {
                eventLog = new EventLogSystemLogger();
                port = int.Parse(ConfigurationManager.AppSettings["serverPort"]);
                listener = new TcpListener(IPAddress.Any, port);
                DBManager = new DatabaseManager();
                eventLog.Info("Uruchamianie serweru.");
                await DBManager.InitializeAsync();
                eventLog.Info("Baza danych została zainicjalizowana");
                listener.Start();
                while (true)
                {
                    TcpClient client = await listener.AcceptTcpClientAsync();
                    eventLog.Info("Klient sie polaczyl.");
                    ClientHandler handler = new ClientHandler(DBManager, eventLog);
                    _ = Task.Run(() => handler.HandleClientAsync(client));
                }
            }
            catch (ConfigurationErrorsException ex)
            {
                eventLog.Info("Wystąpił błąd w pliku konifguracji:" + ex.Message);
                throw new Exception(ex.Message);
            }
            catch (FormatException ex)
            {
                eventLog.Info("Błąd formatu w ustawieniach: " + ex.Message);
                throw new Exception(ex.Message);
            }
            catch (ArgumentNullException ex)
            {
                eventLog.Info("Brak wartości w ustawieniach: " + ex.Message);
                throw new Exception(ex.Message);
            }
            catch (Exception ex)
            {
                eventLog.Info("Uruchomienie serwera niepowiodło się: " + ex.Message);
                throw new Exception(ex.Message);
            }
            finally
            {
                eventLog.Info("Zatrzymywanie serwera.");
                listener?.Stop();
            }
        }
    }
}
