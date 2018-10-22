using System;
using System.Collections.Generic;
using System.Collections.ObjectModel;
using System.Windows;
using System.Windows.Controls;
using System.Windows.Input;
using WpfGauge;
using System.Windows.Media;
using System.ComponentModel;
using System.IO.Ports;

namespace SensorDisplay
{
    /// <summary>
    /// Interaction logic for MainWindow.xaml
    /// </summary>
    public partial class MainWindow : Window
    {
        int SKIP_MESSAGES = 120;
        List<Gauge> gauges;
        List<ObservableCollection<GoalMarker>> markers;
        // Red Green Blue Pink
        List<string> Colours = new List<string> { "Red", "Green", "Blue", "Pink" };
        List<int> MarkerIndex = new List<int> { 0, 0, 0, 0, 0};
        BackgroundWorker backgroundWorker;

        public MainWindow()
        {
            InitializeComponent();
            gauges = new List<Gauge>{ Sheet, Rudder, Heel, Ram1, Ram2};

             backgroundWorker = new BackgroundWorker
            {
                WorkerReportsProgress = true,
                WorkerSupportsCancellation = true
            };
            backgroundWorker.DoWork += PollSerial;
            backgroundWorker.ProgressChanged += GotData;
        }

        private void Window_Loaded(object sender, RoutedEventArgs e)
        {
            backgroundWorker.RunWorkerAsync();
        }

        private void PollSerial(object sender, DoWorkEventArgs e)
        {
            BackgroundWorker worker = (BackgroundWorker)sender;
            SerialPort port = null;
            bool connected = false;
            System.Threading.Thread.Sleep(2500);
            int messagecount = 0;
            var line = "";
            while (!worker.CancellationPending)
            {
                if (!connected)
                {
                    //connect 
                    foreach (string s in SerialPort.GetPortNames())
                    {
                        worker.ReportProgress(0, "MTrying Port:" + s);
                        port = new SerialPort(s, 38400);
                        port.ReadTimeout = 500;
                        try
                        {
                            port.Open();
                            port.ReadLine();
                            connected = true;
                            worker.ReportProgress(0, "MConnected");
                        }
                        catch (Exception ex) {
                            worker.ReportProgress(0, "MException: " + ex.Message);
                            if (port.IsOpen) { port.Close(); }
                            connected = false;
                        }
                    }     
                } else
                {
                    try { line = port.ReadLine(); }// get data
                    catch (Exception ex)
                    {
                        line = "";
                        worker.ReportProgress(0, "MException: " + ex.Message);
                        if (port.IsOpen) { port.Close(); }
                        connected = false;
                    }
                    if (messagecount >= SKIP_MESSAGES && !string.IsNullOrWhiteSpace(line))
                    {
                        // send data
                        worker.ReportProgress(0, "V" + line.Trim());
                        messagecount = 0;
                    } else
                    {
                        // rate limit
                        messagecount++;
                    }
                }
                System.Threading.Thread.Sleep(1);
            }
            if (port != null && port.IsOpen) { port.Close(); }
        }

        private void GotData(object sender, ProgressChangedEventArgs e)
        {
            var msg = (string)e.UserState;
            if (msg[0] == 'M' || true) { Log.Text = LogBuffer.Log(msg.Substring(1)); Log.ScrollToEnd(); }
            if (msg[0] == 'V')
            {
                // process data
                var values = msg.Substring(1).Split(' ');
                for (int x=0; x<values.Length;x++)
                {
                    gauges[x].Value = int.Parse(values[x]);
                }
            }
        }

        private void ResetMarkers_Click(object sender, RoutedEventArgs e)
        {
            foreach (var m in Heel.GoalMarkers)
            {
                m.Value = 1025;
            }
            for (int i = 0; i < MarkerIndex.Count; i++) { MarkerIndex[i] = 0; }
            foreach( var g in gauges)
            {
                SetLabelColour((Label)FindName(g.Name + "Marker"), 0);
            }
        }

        private void RMouseGauge(object sender, MouseButtonEventArgs e)
        {
            Gauge g = (Gauge)sender;
            Label l = (Label)FindName(g.Name + "Marker");
            int tag = int.Parse((string)g.Tag);
            int index = MarkerIndex[tag];
            index = index - 1;
            if (index < 0)
                index = Colours.Count - 1;
            g.GoalMarkers[index+(tag*4)].Value = 1025;
            MarkerIndex[int.Parse((string)g.Tag)] = index;
            SetLabelColour(l, index);
        }

        private void SetLabelColour(Label l, int i)
        {
            l.Content = Colours[i];
            System.Drawing.Color c = System.Drawing.Color.FromName(Colours[i]);
            l.Foreground = new SolidColorBrush(Color.FromArgb(c.A, c.R, c.G, c.B));
        }
        private void LMouseGauge(object sender, MouseButtonEventArgs e)
        {
            Gauge g = (Gauge)sender;
            Label l = (Label)FindName(g.Name + "Marker");
            int tag = int.Parse((string)g.Tag);
            int index = MarkerIndex[tag];
            g.GoalMarkers[index + (tag * 4)].Value = g.Value;
            index = index + 1;
            if (index >= Colours.Count)
                index = 0;
            MarkerIndex[int.Parse((string)g.Tag)] = index;
            SetLabelColour(l, index);
        }
    }
}
