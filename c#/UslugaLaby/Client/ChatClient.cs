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
    public class ChatClient
    {
        private IChatTransport transport;
        private bool _disposed;
        private readonly SemaphoreSlim semaphore = new SemaphoreSlim(1, 1);
        private ISystemLogger systemLogger;
        public ChatClient(IChatTransport trans, ISystemLogger logger)
        {
            transport = trans;
            systemLogger = logger;
        }

        /// <summary>
        /// Wysyła polecenie do serwera czatu i oczekuje na odpowiedź.
        /// </summary>
        /// <param name="command">Treść polcenia</param>
        /// <param name="terminator">Ostatnia wartość odesłana dla tej odpowiedzi, kończąca czytanie</param>
        /// <returns>Listę ciągów znaków, będących odpowiedzią od serwera</returns>
        /// <exception cref="ArgumentNullException">Wyrzuca ten wyjątek gdy próbujemy wysłać puste polecenie</exception>
        public async Task<string[]> SendCommandAsync(string command, string terminator = null)
        {
            if (string.IsNullOrWhiteSpace(command))
                throw new ArgumentNullException(nameof(command));

            await semaphore.WaitAsync();
            try
            {
                if (!transport.IsConnected)
                    await transport.ConnectAsync();

                await transport.WriteLineAsync(command);

                var result = new List<string>();
                string line;

                if (terminator == null)
                {
                    line = await transport.ReadLineAsync();
                    if (line != null)
                        result.Add(line);
                    systemLogger.Info($"Sent: {command}, received {result.Count} lines.");
                }
                else
                {
                    while ((line = await transport.ReadLineAsync()) != null)
                    {
                        if (line == terminator)
                            break;

                        result.Add(line);
                    }
                }

                return result.ToArray();
            }
            catch (Exception ex)
            {
                systemLogger.Info($"Błąd w wysyłaniu polecenie do serwera: {ex.Message}");
                throw;
            }
            finally
            {
                semaphore.Release();
            }
        }
    }
}