using System.Windows;
using WpfGauge;
using System.Windows.Threading;
using System;

namespace WpfGauge.Test
{
    /// <summary>
    /// Interaction logic for MainWindow.xaml
    /// </summary>
    public partial class MainWindow : Window
    {

        public readonly DependencyProperty RunsPerHourProperty =
            DependencyProperty.Register("RunsPerHour", typeof(double), typeof(MainWindow));

        public double RunsPerHour
        {
            get { return (double)base.GetValue(RunsPerHourProperty); }
            set { base.SetValue(RunsPerHourProperty, value); }
        }

        public MainWindow()
        {
            InitializeComponent();

            DispatcherTimer timer = new DispatcherTimer();
            timer.Interval = TimeSpan.FromSeconds(1);
            timer.Tick += (args, e) =>
                {
                    RunsPerHour = (double)DateTime.Now.Second;
                };
            timer.Start();
        }
    }
}
