using System;
using System.Collections.Generic;
using System.Diagnostics;
using System.IO;
using System.Net.Sockets;
using System.Reflection;
using System.Threading.Tasks;

namespace ServerLibrary
{
    public class MessageItem
    {
        public string Sender { get; set; }
        public string Text { get; set; }
        public string Timestamp { get; set; }
    }
    public class ClientHandler
    {
        private TcpClient client;
        private StreamReader reader;
        private StreamWriter writer;
        private IDatabaseManager DBManager;
        private ISystemLogger eventLog;
        public ClientHandler(IDatabaseManager db, ISystemLogger logger)
        {
            this.DBManager = db;
            eventLog = logger;
        }
        /// <summary>
        /// Asynchronicznie obsługuje komunikację z podłączonym klientem.
        /// </summary>
        /// <remarks> Ta metoda przetwarza przychodzące polecenia od klienta, wykonuje żądane operacje i wysyła odpowiednie odpowiedzi z powrotem do klienta.
        /// Obsługiwane polecenia obejmują logowanie, rejestrację, pobieranie listy użytkowników, pobieranie wiadomości i wysyłanie wiadomości. 
        /// W przypadku otrzymania nieznanego polecenia wysyłana jest odpowiedź o błędzie. 
        /// Metoda odczytuje dane wejściowe od klienta linia po linii, analizuje polecenie i jego parametry oraz współdziała z bazą danych w celu realizacji żądanych operacji. 
        /// Zapewnia również odpowiednie czyszczenie zasobów po rozłączeniu klienta lub wystąpieniu wyjątku.</remarks>
        public async Task HandleClientAsync(TcpClient client)
        {
            this.client = client;
            NetworkStream stream = client.GetStream();
            reader = new StreamReader(stream);
            writer = new StreamWriter(stream) { AutoFlush = true };
            try
            {
                string line;
                while ((line = await reader.ReadLineAsync()) != null)
                {
                    string[] parts = line.Split('|');
                    string command = parts[0];

                    switch (command)
                    {
                        case "LOGIN":
                            {
                                string username = parts[1];
                                string password = parts[2];
                                await writer.WriteLineAsync(await LoginUser(username, password));

                                break;
                            }
                        case "REGISTER":
                            {
                                string username = parts[1];
                                string password = parts[2];
                                await writer.WriteLineAsync(await RegisterUser(username, password));
                                break;
                            }
                        case "GET_USERS":
                            {
                                string username = parts[1];
                                await writer.WriteLineAsync(await GetUsers(username));
                                break;
                            }
                        case "GET_MESSAGES":
                            {
                                string username = parts[1];
                                string contact = parts[2];
                                string lastMessageTime = parts[3];
                                await writer.WriteLineAsync(await GetMessages(username, contact, lastMessageTime));
                                break;
                            }
                        case "SEND_MESSAGE":
                            {
                                string sender = parts[1];
                                string message = parts[2];
                                string contact = parts[3];
                                await writer.WriteLineAsync(await SendMessage(sender, message, contact));
                                break;
                            }
                        default:
                            await writer.WriteLineAsync("ERROR|Unknown command.");
                            break;
                    }
                }
            }
            catch (Exception ex)
            {
                eventLog.Info("Uzytkownik został rozłączony.");
            }
            finally
            {
                client.Close();
            }
        }
        /// <summary>
        /// Obsługuje logowanie użytkownika asynchronicznie.
        /// </summary>
        /// <param name="username">Login użytkownika</param>
        /// <param name="password">Hasło użytkownika</param>
        /// <returns>Ciąg znaków będących opisem stanu powoedzenia operacji</returns>
        public async Task<string> LoginUser(string username, string password)
        {
            try
            {
                bool success = await DBManager.LoginUserAsync(username, password);
                if (success)
                {
                    eventLog.Info($"Uzytkownik {username} zalogował się w aplikacji.");
                    return "LOGIN_SUCCESS";
                }
                else
                {
                    eventLog.Info($"Uzytkownik {username} nie zalogował się w aplikacji.");
                    return "LOGIN_FAILED|Invalid username or password.";
                }
            }
            catch (Exception ex) 
                {
                    eventLog.Info("Polecenie bazy danych zakończyło się błędem");
                    return "LOGIN_FAILED|Couldn't procces database reguest.";
                }
            }
        /// <summary>
        /// Obsługuje rejestrację użytkownika asynchronicznie.
        /// </summary>
        /// <param name="username">Login użytkownika</param>
        /// <param name="password">Hasło użytkownika</param>
        /// <returns>Ciąg znaków będących opisem stanu powoedzenia operacji</returns>
        /// <exception cref="ArgumentNullException"> Wyrzuca ten wyjątek wtedy gdy login lub hasło jest puste</exception>
        public async Task<string> RegisterUser(string username, string password)
        {
            if (string.IsNullOrWhiteSpace(username))
            {
                throw new ArgumentNullException("Username is empty");
            }
            if (string.IsNullOrWhiteSpace(password))
            {
                throw new ArgumentNullException("Password is empty");
            }
            try
            {
                bool success = await DBManager.RegisterUserAsync(username, password);
                eventLog.Info($"Uzytkownik {username} zarejestrował się w aplikacji.");
                return (success ? "REGISTER_SUCCESS" : "ERROR|Registration failed. User may already exist.");
            }
            catch (Exception ex)
            {
                eventLog.Info("Polecenie bazy danych zakończyło się błędem");
                return "ERROR|Couldn't procces database reguest.";
            }
        }

        /// <summary>
        /// Obsługuje wysyłanie wiadomości asynchronicznie.
        /// </summary>
        /// <param name="sender">Nadawaca</param>
        /// <param name="message">Treść wiadomości</param>
        /// <param name="contact">Odbiorca</param>
        /// <returns>Ciąg znaków będących opisem stanu powoedzenia operacji</returns>
        /// <exception cref="ArgumentNullException"> Wyrzuca ten wyjątek gdy którykolwiek argument jest pusty</exception>
        /// <exception cref="ArgumentException">Wyrzuca ten wyjątek jeśli odbiorca i nadawca jest taki sam</exception>
        public async Task<string> SendMessage(string sender, string message, string contact)
        {
            if (string.IsNullOrWhiteSpace(sender))
            {
                throw new ArgumentNullException("Sender is empty");
            }
            if (string.IsNullOrWhiteSpace(message))
            {
                throw new ArgumentNullException("Message is empty");
            }
            if (string.IsNullOrWhiteSpace(contact))
            {
                throw new ArgumentNullException("Contact is empty");
            }
            if (sender == contact)
            {
                throw new ArgumentException("Sender and contact cannot be the same");
            }
            string timestamp = DateTime.Now.ToString("yyyy-MM-dd HH:mm:ss");
            try
            {
                await DBManager.SaveMessageAsync(sender, message, timestamp, contact);
                eventLog.Info($"Uzytkownik {sender} wyslal wiadomosc do {contact}");
                return "MESSAGE_SENT";
            }
            catch (Exception ex)
            {
                eventLog.Info("Polecenie bazy danych zakończyło się błędem");
                return "MESSAGE_NOT_SENT";
            }
        }
        /// <summary>
        /// Obsługuje pobieranie listy użytkowników asynchronicznie.
        /// </summary>
        /// <param name="username">Login użytkownika</param>
        /// <returns>Ciąg znaków, którym jest lista wszystkich użytkownikó, oprócz użytkownika, który wywołuje tą funckje</returns>
        /// <exception cref="ArgumentNullException">Wyrzuca ten wyjątek gdy nie podano użytkownika</exception>
        public async Task<string> GetUsers(string username)
        {
            string result = "";
            if (string.IsNullOrWhiteSpace(username))
            {
                throw new ArgumentNullException("Username is empty");
            }
            try
            {
                List<string> users = await DBManager.GetUsersAsync(username);
                foreach (string user in users)
                {
                    result += user + "\n";
                }
                return result + "END_OF_USERS";
            }
            catch (Exception ex)
            {
                eventLog.Info("Polecenie bazy danych zakończyło się błędem");
                return "END_OF_USERS";

            }
        }
        /// <summary>
        /// Obsługuje pobieranie wiadomości asynchronicznie.
        /// </summary>
        /// <param name="username">Login uzytkownika wysyłający zapytanie</param>
        /// <param name="contact">Login użytkownika z którym jest rozmowa</param>
        /// <param name="lastMessageTime">Data ostatniej pobranej wiadomości</param>
        /// <returns>Ciąg znaków będący wszystkimi wiadomościami wysłanymi między użytkownikiem a kontaktem</returns>
        /// <exception cref="ArgumentNullException">Wyrzuca ten wyjątek gdy którekolwiek arguemt jest pusty</exception>
        /// <exception cref="ArgumentException">Wyrzuca ten wyjątek gdy użykownik i kontakt są tą samą osobą</exception>
        public async Task<string> GetMessages(string username, string contact, string lastMessageTime)
        {
            if (string.IsNullOrWhiteSpace(username))
            {
                throw new ArgumentNullException("Username is empty");
            }
            if (string.IsNullOrWhiteSpace(contact))
            {
                throw new ArgumentNullException("Contact is empty");
            }
            if (string.IsNullOrWhiteSpace(lastMessageTime))
            {
                throw new ArgumentNullException("LastMessageTime is empty");
            }
            if (username == contact)
            {
                throw new ArgumentException("Username and contact cannot be the same");
            }
            string result = "";
            try
            {
                List<MessageItem> messages = await DBManager.GetMessagesAsync(username, contact, lastMessageTime);
                foreach (MessageItem msg in messages)
                {
                    result += msg.Timestamp + "|" + msg.Sender + "|" + msg.Text + "\n";
                }
                return result + "END_OF_MESSAGES";
            }
            catch (Exception ex)
            {
                eventLog.Info("Polecenie bazy danych zakończyło się błędem");
                return "END_OF_MESSAGES";

            }
        }

    }
}
