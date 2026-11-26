using Microsoft.Data.Sqlite;
using System;
using System.Collections.Generic;
using System.Configuration;
using System.IO;
using System.Threading.Tasks;

namespace ServerLibrary
{
    public interface IDatabaseManager
    {
        Task InitializeAsync();
        Task<bool> RegisterUserAsync(string username, string password);
        Task<bool> LoginUserAsync(string username, string password);
        Task<List<string>> GetUsersAsync(string username);
        Task<List<MessageItem>> GetMessagesAsync(string username, string contact, string lastTime);
        Task SaveMessageAsync(string sender, string message, string timestamp, string contact);
    }
    public class DatabaseManager : IDatabaseManager
    {
        private string connectionString;

        /// <summary>
        /// Inicjalizuje połączenie z bazą SQLite.
        /// </summary>
        public async Task InitializeAsync()
        {
            try
            {
                string databaseFolder = ConfigurationManager.AppSettings["databaseFolder"];
                string databaseFile = ConfigurationManager.AppSettings["databaseFile"];

                string folder = Path.Combine(
                    Environment.GetFolderPath(Environment.SpecialFolder.CommonApplicationData),
                    databaseFolder);

                Directory.CreateDirectory(folder);

                string dbPath = Path.Combine(folder, databaseFile);
                connectionString = $"Data Source={dbPath}";

                using (SqliteConnection conn = new SqliteConnection(connectionString))
                {
                    conn.Open();
                    SqliteCommand cmd = conn.CreateCommand();
                    cmd.CommandText = @"
                CREATE TABLE IF NOT EXISTS users (
                    id INTEGER PRIMARY KEY AUTOINCREMENT,
                    username TEXT UNIQUE,
                    password TEXT
                );";
                    await cmd.ExecuteNonQueryAsync();

                    cmd.CommandText = @"
                CREATE TABLE IF NOT EXISTS messages (
                    id INTEGER PRIMARY KEY AUTOINCREMENT,
                    sender TEXT,
                    message TEXT,
                    timestamp TEXT,
                    contact TEXT
                );";
                    await cmd.ExecuteNonQueryAsync();
                }
            }
            catch (Exception ex)
            {
                throw new Exception("Inicjalizacja bazy danych nie powiodła się: " + ex.Message);
            }
        }
        /// <summary>
        /// Obsługuje wysłanie polecenia rejestracji nowego użytkownika do bazy danych.
        /// </summary>
        /// <param name="username">Login użytkoniwka</param>
        /// <param name="password">Hasło użytkownika</param>
        /// <returns>Zwraca true jeśli udało się zarejestrować użytkownika do bazy danych, w przeciwnym wypadku zwraca false</returns>
        public async Task<bool> RegisterUserAsync(string username, string password)
        {
            int result = 0;
            using (SqliteConnection conn = new SqliteConnection(connectionString))
            {
                await conn.OpenAsync();

                SqliteCommand checkCmd = conn.CreateCommand();
                checkCmd.CommandText = "SELECT COUNT(*) FROM users WHERE username = @username";
                checkCmd.Parameters.AddWithValue("@username", username);

                long exists = (long)await checkCmd.ExecuteScalarAsync();
                if (exists > 0)
                {
                    return false;
                }

                SqliteCommand cmd = conn.CreateCommand();
                cmd.CommandText = "INSERT INTO users (username, password) VALUES (@username, @password)";
                cmd.Parameters.AddWithValue("@username", username);
                cmd.Parameters.AddWithValue("@password", password);

                result = await cmd.ExecuteNonQueryAsync();
            }
            return result > 0;
        }
        /// <summary>
        /// Obsługuje wysłanie polecenia logowania nowego użytkownika do bazy danych.
        /// </summary>
        /// <param name="username">Login użytkoniwka</param>
        /// <param name="password">Hasło użytkownika</param>
        /// <returns>Zwraca true jeśli udało się zalogować użytkownika do bazy danych, w przeciwnym wypadku zwraca false</returns>
        public async Task<bool> LoginUserAsync(string username, string password)
        {
            long count = 0;
            using (SqliteConnection conn = new SqliteConnection(connectionString))
            {
                await conn.OpenAsync();

                SqliteCommand cmd = conn.CreateCommand();
                cmd.CommandText = "SELECT COUNT(*) FROM users WHERE username = @username AND password = @password";
                cmd.Parameters.AddWithValue("@username", username);
                cmd.Parameters.AddWithValue("@password", password);

                count = (long)await cmd.ExecuteScalarAsync();
            }
            return count > 0;
        }
        /// <summary>
        /// Obsługuje wysłanie polecenia pobrania listy użytkowników z bazy danych.
        /// </summary>
        /// <param name="username">Login użytkownika</param>
        /// <returns>Listę wszystkich użytkowników, poza tym który wysłał polecenie</returns>
        public async Task<List<string>> GetUsersAsync(string username)
        {
            List<string> users = new List<string>();
            using (SqliteConnection conn = new SqliteConnection(connectionString))
            {
                await conn.OpenAsync();

                SqliteCommand cmd = conn.CreateCommand();
                cmd.CommandText = "SELECT username FROM users WHERE username != @user";
                cmd.Parameters.AddWithValue("@user", username);

                using (SqliteDataReader reader = await cmd.ExecuteReaderAsync())
                {
                    while (await reader.ReadAsync())
                    {
                        users.Add(reader.GetString(0));
                    }
                }
            }
            return users;
        }
        /// <summary>
        /// Obsługuje wysłanie polecenia pobrania wiadomości z bazy danych danego użytkownika.
        /// </summary>
        /// <param name="username">Login użytkownika</param>
        /// <param name="contact">Login użytkownika z którym prowadzimy rozmowę</param>
        /// <param name="lastTime">Data ostatniej pobranej wiadomości</param>
        /// <returns>Obiekt typu MessageItem</returns>
        public async Task<List<MessageItem>> GetMessagesAsync(string username, string contact, string lastTime)
        {
            List<MessageItem> messages = new List<MessageItem>();
            using (SqliteConnection conn = new SqliteConnection(connectionString))
            {
                await conn.OpenAsync();

                SqliteCommand cmd = conn.CreateCommand();

                if (lastTime == "null")
                {
                    cmd.CommandText = @"
                    SELECT sender, message, timestamp FROM messages
                    WHERE ((contact = @contact AND sender = @username)
                       OR (sender = @contact AND contact = @username))
                    ORDER BY id";
                }
                else
                {
                    cmd.CommandText = @"
                    SELECT sender, message, timestamp FROM messages
                    WHERE ((contact = @contact AND sender = @username)
                       OR (sender = @contact AND contact = @username))
                       AND timestamp > @time
                    ORDER BY id";

                    cmd.Parameters.AddWithValue("@time", lastTime);
                }

                cmd.Parameters.AddWithValue("@contact", contact);
                cmd.Parameters.AddWithValue("@username", username);

                using (SqliteDataReader reader = await cmd.ExecuteReaderAsync())
                {
                    while (await reader.ReadAsync())
                    {
                        messages.Add(new MessageItem
                        {
                            Sender = reader.GetString(0),
                            Text = reader.GetString(1),
                            Timestamp = reader.GetString(2)
                        });
                    }
                }
            }
            return messages;
        }
        /// <summary>
        /// Obsługuje wysłanie polecenia zapisania wiadomości do bazy danych.
        /// </summary>
        /// <param name="sender">Nadawca</param>
        /// <param name="message">Treść</param>
        /// <param name="timestamp">Data wysłania</param>
        /// <param name="contact">Odbiorca</param>
        public async Task SaveMessageAsync(string sender, string message, string timestamp, string contact)
        {
            using (SqliteConnection conn = new SqliteConnection(connectionString))
            {
                await conn.OpenAsync();

                SqliteCommand cmd = conn.CreateCommand();
                cmd.CommandText = @"
                INSERT INTO messages (sender, message, timestamp, contact)
                VALUES (@sender, @message, @timestamp, @contact)";

                cmd.Parameters.AddWithValue("@sender", sender);
                cmd.Parameters.AddWithValue("@message", message);
                cmd.Parameters.AddWithValue("@timestamp", timestamp);
                cmd.Parameters.AddWithValue("@contact", contact);

                await cmd.ExecuteNonQueryAsync();
            }
        }
    }
}
