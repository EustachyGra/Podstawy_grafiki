using System;
using System.Diagnostics;
using System.ServiceProcess;
using System.Text;
using System.Windows;
using System.Windows.Threading;

namespace ServiceManager
{
    public partial class MainWindow : Window
    {
        private readonly ServiceController _service;
        private readonly EventLog _eventLog;
        private DispatcherTimer timer;
        public MainWindow()
        {
            InitializeComponent();

            _service = new ServiceController("ChatServerDisplay");

            _eventLog = new EventLog();
            _eventLog.Source = "MojeZrodlo";
            _eventLog.Log = "MojLog";
            DispatcherTimer timer = new DispatcherTimer();
            timer.Interval = TimeSpan.FromSeconds(1);
            timer.Tick += async (s, e) => RefreshStatus();
            RefreshStatus();
        }
        private void RefreshStatus()
        {
            _service.Refresh();

            StatusText.Text = _service.Status.ToString();

            StartButton.IsEnabled = _service.Status == ServiceControllerStatus.Stopped;
            StopButton.IsEnabled = _service.Status == ServiceControllerStatus.Running;
        }

        private async void StartButton_Click(object sender, RoutedEventArgs e)
        {
            try
            {
                _service.Start();
                await System.Threading.Tasks.Task.Run(() =>
                {
                    _service.WaitForStatus(ServiceControllerStatus.Running, TimeSpan.FromSeconds(10));
                });
            }
            catch (Exception ex)
            {
                MessageBox.Show("Start error: " + ex.Message);
            }

            RefreshStatus();
        }

        private async void StopButton_Click(object sender, RoutedEventArgs e)
        {
            try
            {
                _service.Stop();
                await System.Threading.Tasks.Task.Run(() =>
                {
                    _service.WaitForStatus(ServiceControllerStatus.Stopped, TimeSpan.FromSeconds(10));
                });
            }
            catch (Exception ex)
            {
                MessageBox.Show("Stop error: " + ex.Message);
            }

            RefreshStatus();
        }

        private void RefreshLogButton_Click(object sender, RoutedEventArgs e)
        {
            LogTextBox.Clear();

            StringBuilder sb = new StringBuilder();

            if (!EventLog.SourceExists(_eventLog.Source))
            {
                return;
            }
            foreach (EventLogEntry entry in _eventLog.Entries)
            {
                if(entry.Source == _eventLog.Source)
                    sb.AppendLine($"{entry.TimeGenerated} - {entry.Message}");
            }

            LogTextBox.Text = sb.ToString();
        }

        private void ClearLogButton_Click(object sender, RoutedEventArgs e)
        {
            LogTextBox.Clear();
        }
    }
}
