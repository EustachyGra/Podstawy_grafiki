using System;
using System.Configuration;
using System.Windows;
using System.Windows.Input;
using System.Windows.Threading;


namespace Client
{
    public partial class LoginWindow : Window
    {
        private IChatTransport transport;
        private ISystemLogger systemLogger = new EventLogSystemLogger();
        private ChatClient client;
        private bool isConnecting = false;
        private DispatcherTimer timer;
        public LoginWindow()
        {
            try
            {
                transport = new TcpChatTransport();
            }
            catch (ConfigurationErrorsException ex)
            {
                MessageBox.Show("Wystąpił błąd w pliku konifguracji:" + ex.Message);
                systemLogger.Info("Nie wczytano wartości z ustawień: " + ex.Message);
                Application.Current.Shutdown();
            }
            catch (ArgumentNullException ex)
            {
                MessageBox.Show("Brak wartości w pliku konifguracji: " + ex.Message);
                systemLogger.Info("Brak wartości w ustawieniach: " + ex.Message);
                Application.Current.Shutdown();
            }
            catch (FormatException ex)
            {
                MessageBox.Show("Błąd formatu w pliku konifguracji: " + ex.Message);
                systemLogger.Info("Błąd formatu w ustawieniach: " + ex.Message);
                Application.Current.Shutdown();
            }
            catch (Exception ex)
            {
                MessageBox.Show("Błąd podczas inicjalizacji połączenia TCP: " + ex.Message);
                systemLogger.Info("Błąd w konstuktorze TCP: " + ex.Message);
                Application.Current.Shutdown();
            }
            client = new ChatClient(transport, systemLogger);
            InitializeComponent();
            timer = new DispatcherTimer();
            timer.Interval = TimeSpan.FromSeconds(1);
            timer.Tick += async (s, e) =>
            { // Co sekunde sprawdza czy klient jest połączony. Zmienia kolor i treść etykiety w zależności od stanu połączenia.
                if (!transport.IsConnected)
                {
                    Connection.Content = "Offline";
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
                    Connection.Content = "Online";
                    Connection.Foreground = System.Windows.Media.Brushes.Green;
                }
            };
            timer.Start();
        }

        /// <summary>
        /// Obsługuje zdarzenie kliknięcia przycisku logowania, inicjując proces logowania użytkownika.
        /// </summary>
        /// <remarks>
        /// Ta metoda sprawdza poprawność pól wejściowych dla nazwy użytkownika i hasła, wysyła polecenie logowania do serwera
        /// oraz przetwarza odpowiedź serwera. Jeśli logowanie zakończy się sukcesem, otwiera okno czatu i zamyka bieżące okno.
        /// W przypadku niepowodzenia logowania wyświetlany jest komunikat o błędzie.
        /// </remarks>
        private async void Login_Click(object sender, RoutedEventArgs e)
        {
            if (!transport.IsConnected)
            {
                Error.Content = "Disconected.";
                Error.Foreground = System.Windows.Media.Brushes.Red;
                return;
            }
            string username = txtUsername.Text.Trim();
            string password = txtPassword.Password.Trim();
            if (username == "" || password == "")
            {
                Error.Content = "Enter username and password.";
                Error.Foreground = System.Windows.Media.Brushes.Red;
                return;
            }
            string[] response = await client.SendCommandAsync($"LOGIN|{username}|{password}");

            if (response.Length > 0 && response[0] == "LOGIN_SUCCESS")
            {
                ChatWindow chatWindow = new ChatWindow(username, client, transport, systemLogger);
                timer.Stop();
                chatWindow.Show();
                this.Close();
            }
            else
            {
                Error.Content = "Login failed. Check your username and password.";
                Error.Foreground = System.Windows.Media.Brushes.Red;
            }


        }

        /// <summary>
        /// Obsługuje zdarzenie naciśnięcia klawisza Enter, aby wywołać akcję logowania.
        /// </summary>
        /// <remarks>
        /// Ta metoda sprawdza, czy został naciśnięty klawisz Enter i w takim przypadku wywołuje metodę
        /// <c>Login_Click</c>, aby zainicjować proces logowania.
        /// </remarks>
        private void Login_Enter(object sender, System.Windows.Input.KeyEventArgs e)
        {
            if (e.Key == Key.Enter)
            {
                Login_Click(sender, e);
            }
        }
        /// <summary>
        /// Obsługuje zdarzenie kliknięcia przycisku Rejestruj, inicjując proces rejestracji użytkownika.
        /// </summary>
        /// <remarks>
        /// Ta metoda sprawdza poprawność pól wejściowych dla nazwy użytkownika i hasła, upewniając się, że nie są puste.
        /// Następnie wysyła polecenie rejestracji do serwera asynchronicznie, by sprawdzić czy login nie jest zajęty.
        /// Jeśli rejestracja zakończy się sukcesem, wyświetlany jest komunikat potwierdzający.
        /// W przeciwnym razie wyświetlany jest komunikat o błędzie.
        /// </remarks>

        private async void Register_Click(object sender, RoutedEventArgs e)
        {
            if (!transport.IsConnected)
            {
                Error.Content = "Disconected.";
                Error.Foreground = System.Windows.Media.Brushes.Red;
                return;
            }
            string username = txtUsername.Text.Trim();
            string password = txtPassword.Password.Trim();
            if (username == "" || password == "")
            {
                Error.Content = "Enter username and password.";
                Error.Foreground = System.Windows.Media.Brushes.Red;
                return;
            }

            string[] response = await client.SendCommandAsync($"REGISTER|{username}|{password}");
            if (response.Length > 0 && response[0] == "REGISTER_SUCCESS")
            {
                Error.Content = "Registration successful. You can now log in.";
                Error.Foreground = System.Windows.Media.Brushes.Green;
            }
            else
            {
                Error.Content = "Registration failed. Username might be already taken.";
                Error.Foreground = System.Windows.Media.Brushes.Red;
            }

        }
    }
}