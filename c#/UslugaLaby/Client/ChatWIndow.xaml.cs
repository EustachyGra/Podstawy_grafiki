using System;
using System.Threading.Tasks;
using System.Windows;
using System.Windows.Controls;
using System.Windows.Input;
using System.Windows.Threading;

namespace Client
{
    public partial class ChatWindow : Window
    {
        private IChatTransport transport;
        private ISystemLogger systemLogger;
        private bool isConnecting = false;
        private string username, contact;
        bool isUpdating = false;
        bool isSending = false;
        private bool isChatEmpty;
        private ChatClient client;
        private DispatcherTimer timer;
        private string lastMessageTime = "";

        public ChatWindow(string username, ChatClient client, IChatTransport trans, ISystemLogger logger)
        {
            InitializeComponent();
            this.username = username;
            this.client = client;
            Title = $"Chat - {username}";
            this.transport = trans;
            this.systemLogger = logger;

            timer = new DispatcherTimer();
            timer.Interval = TimeSpan.FromSeconds(1);
            timer.Tick += async (s, e) => await UpdateChatAsync(); // Co sekundę wywołuje funckcję UpdateChatAsync, aby zaktualizować interfejs użytkownika czatu.
            timer.Tick += async (s, e) =>
            { // Co sekundę sprawdza czy klient jest połączony, jeśli nie zmienia kolor i treść etykiety w zależności od stanu połączenia.
                if (!transport.IsConnected)
                {
                    Connection.Content = "Connection: Offline";
                    Connection.Foreground = System.Windows.Media.Brushes.Red;
                    if (!isConnecting)
                    {
                        isConnecting = true;
                        await transport.ConnectAsync();
                        client = new ChatClient(transport, systemLogger);
                        isConnecting = false;
                    }
                }
                else
                {
                    Connection.Content = "Connection: Online";
                    Connection.Foreground = System.Windows.Media.Brushes.Green;
                }
            };
            timer.Start();

            UpdateChatAsync();
        }
        /// <summary>
        /// Aktualizuje interfejs czatu, pobierając listę użytkowników i wiadomości z serwera.
        /// </summary>
        /// <remarks>Ta metoda asynchronicznie pobiera listę użytkowników i aktualizuje interfejs użytkownika, żeby je wyświetlić. 
        /// Jeśli wybrano kontakt, pobiera i wyświetla wiadomości czatu wymieniane z wybranym użytkownikiem.</remarks>
        private async Task UpdateChatAsync()
        {

            if (!transport.IsConnected)
            {
                return;
            }
            if (isUpdating)
            {
                return;
            }

            isUpdating = true;
            string[] users = await client.SendCommandAsync($"GET_USERS|{username}", "END_OF_USERS");
            lstUsers.Children.Clear();
            foreach (string user in users)
            {
                Button tmp = new Button
                {
                    Height = 25,
                    Content = user
                };
                tmp.Click += ChatSelected;
                lstUsers.Children.Add(tmp);
            }

            if (!string.IsNullOrEmpty(contact))
            {

                string[] messages = await client.SendCommandAsync($"GET_MESSAGES|{username}|{contact}|{lastMessageTime}", "END_OF_MESSAGES");
                if (messages.Length != 0 && isChatEmpty)
                {
                    lstMessages.Items.Clear();
                    isChatEmpty = false;
                }
                foreach (string line in messages)
                {
                    string[] parts = line.Split('|');
                    if (parts.Length >= 3)
                    {
                        string time = parts[0];
                        string senderName = parts[1];
                        string text = parts[2];
                        string n_line = $"[{time}] {senderName}: \n{text}";
                        lstMessages.Items.Add(n_line);
                        lastMessageTime = time;
                    }
                }
            }
            
                isUpdating = false;

            }

        /// <summary>
        /// Obsługuje zdarzenie kliknięcia przycisku wysyłania wiadomości.
        /// </summary>
        /// <remarks> Ta metoda pobiera tekst wiadomości z pola wejściowego, sprawdza jego poprawność i wysyła
        /// wiadomość do serwera, który zapisze ją w bazie danych za pomocą asynchronicznego polecenia. 
        /// Po wysłaniu wiadomości pole wejściowe jest czyszczone, a czat jest aktualizowany.</remarks>
        private async void SendMessage_Click(object sender, RoutedEventArgs e)
        {
            if (!transport.IsConnected)
            {
                await transport.ConnectAsync();
            }
            isSending = true;
            string message = txtMessage.Text.Trim();
            if (message == "" || string.IsNullOrEmpty(contact))
            {
                return;
            }


            await client.SendCommandAsync($"SEND_MESSAGE|{username}|{message}|{contact}");
            txtMessage.Clear();
            isSending = false;
            await UpdateChatAsync();

        }
        /// <summary>
        /// Obługuje zdarzenie zamknięcia okna.
        /// </summary>
        /// <remarks>Ta metoda zatrzymuje powiązany timer, rozłącza klienta i zamyka aplikację.
        /// Wywoływana jest automatycznie, gdy okno zostaje zamknięte.</remarks>
        protected override void OnClosed(EventArgs e)
        {
            base.OnClosed(e);
            timer.Stop();
            Application.Current.Shutdown();
        }
        /// <summary>
        /// Obsługuje zdarzenie naciśnięcia klawisza Enter, aby wywołać akcję wysyłania wiadomości.
        /// </summary>
        /// <remarks>Ta metoda sprawdza, czy naciśnięto klawisz Enter, jeśli tak, wywołuje metodę <c>SendMessage_Click</c>.</remarks>

        private void SendMessage_Enter(object sender, System.Windows.Input.KeyEventArgs e)
        {
            if (e.Key == Key.Enter)
            {
                SendMessage_Click(sender, e);
            }
        }
        /// <summary>
        /// Obsługuje wybór czatu i aktualizuje interfejs użytkownika, aby wyświetlić rozmowę pomiędzy bieżącym użytkownikiem a wybranym kontaktem.
        /// </summary>
        /// <remarks>Ta metoda pobiera wiadomości czatu pomiędzy bieżącym użytkownikiem a wybranym kontaktem z serwera i wyświetla je na liście wiadomości.
        /// Tytuł okna czatu jest aktualizowany, aby odzwierciedlić wybraną rozmowę.</remarks>
        private async void ChatSelected(object sender, RoutedEventArgs e)
        {
            if (sender is Button btn)
            {
                contact = btn.Content.ToString();
                Title = $"Chat - {username} with {contact}";
                lastMessageTime = "null";
                lstMessages.Items.Clear();
                if (!transport.IsConnected)
                {
                    lstMessages.Items.Add("Disconected!");
                    await transport.ConnectAsync();
                    return;
                }

                string[] messages = await client.SendCommandAsync($"GET_MESSAGES|{username}|{contact}|{lastMessageTime}", "END_OF_MESSAGES");
                if (messages.Length == 0)
                {
                    lstMessages.Items.Add("No messages found.");
                    lastMessageTime = DateTime.Now.ToString("yyyy-MM-dd HH:mm:ss");
                    isChatEmpty = true;
                    return;
                }
                isChatEmpty = false;
                foreach (string line in messages)
                {
                    string[] parts = line.Split('|');
                    if (parts.Length >= 3)
                    {
                        string time = parts[0];
                        string senderName = parts[1];
                        string text = parts[2];
                        string n_line = $"[{time}] {senderName}: \n{text}";
                        lstMessages.Items.Add(n_line);
                        lastMessageTime = time;
                    }
                }

            }
        }
    }
}