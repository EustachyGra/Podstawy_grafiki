using System;
using System.Configuration;
using System.IO;
using System.Net.Sockets;
using System.Threading.Tasks;

namespace Client
{
    public interface IChatTransport : IDisposable
    {
        bool IsConnected { get; }
        Task ConnectAsync();
        Task WriteLineAsync(string line);
        Task<string> ReadLineAsync();
    }
    public class TcpChatTransport : IChatTransport
    {
        private TcpClient _client;
        private ISystemLogger _logger = new EventLogSystemLogger();
        private StreamReader _reader;
        private StreamWriter _writer;
        private readonly int DefaultPort;
        private readonly string DefaultHost;
        public TcpChatTransport()
        {

            DefaultHost = ConfigurationManager.AppSettings["serverAddress"];
            if (string.IsNullOrEmpty(DefaultHost))
            {
                _logger.Info("serverAddress jest pusty.");
                throw new ArgumentNullException("serverAddress jest pusty");
            }

            DefaultPort = int.Parse(ConfigurationManager.AppSettings["serverPort"]);
            if (DefaultPort <= 0 || DefaultPort > 65535)
            {
                _logger.Info("serverPort jest pusty.");
                throw new FormatException("Nieprawidłowy numer portu");
            }


        }
        /// <summary>
        /// Sprawdza, czy połączenie TCP jest aktywne.
        /// </summary>
        /// <returns>True jeśli uda się nawiązać połączenie z sererem, w przeciwnym wypadku zwraca false </returns>
        public bool IsConnected
        {
            get => _client != null &&
          _client.Connected &&
          !(_client.Client.Poll(0, SelectMode.SelectRead) && _client.Client.Available == 0);
        }
        /// <summary>
        /// Probóje nawiązać połączenie TCP z serwerem czatu.
        /// </summary>
        public async Task ConnectAsync()
        {
            _client = new TcpClient();
            try
            {
                await _client.ConnectAsync(DefaultHost, DefaultPort);
            }
            catch (Exception)
            {
                _client = null;
                return;
            }
            NetworkStream stream = _client.GetStream();
            _reader = new StreamReader(stream);
            _writer = new StreamWriter(stream) { AutoFlush = true };
        }
        /// <summary>
        /// Wysyła linię tekstu do serwera czatu.
        /// </summary>
        /// <param name="line"> Linia z poleceniem dla serwera</param>
        public Task WriteLineAsync(string line)
        {
            return _writer.WriteLineAsync(line);
        }
        /// <summary>
        /// Odczytuje linię tekstu z serwera czatu.
        /// </summary>
        /// <returns>Ciąg znaków otrzymany od serwera</returns>
        public Task<string> ReadLineAsync()
        {
            return _reader.ReadLineAsync();
        }
        /// <summary>
        /// Odłącza i zwalnia zasoby związane z połączeniem TCP.
        /// </summary>
        public void Dispose()
        {
            _reader?.Dispose();
            _writer?.Dispose();
            _client?.Close();
        }
    }

}
