using System;
using System.Collections.Generic;
using System.Collections.ObjectModel;
using System.Windows;
using WpfGauge;

namespace SensorDisplay
{
    /// <summary>
    /// Interaction logic for MainWindow.xaml
    /// </summary>
    public partial class MainWindow : Window
    {
        List<Gauge> gauges;
        List<ObservableCollection<GoalMarker>> markers;

        public MainWindow()
        {
            InitializeComponent();
            gauges = new List<Gauge>{ Heel, Sheet, Rudder, Ram1, Ram2};
            markers = new List<ObservableCollection<GoalMarker>> {
                new ObservableCollection<GoalMarker>(), new ObservableCollection<GoalMarker>(), new ObservableCollection<GoalMarker>(),
                new ObservableCollection<GoalMarker>(), new ObservableCollection<GoalMarker>(),
            };
            for(int i=0; i<gauges.Count; i++)
            {
                //gauges[i].GoalMarkers = markers[i];
            }
        }

        private void ResetMarkers_Click(object sender, RoutedEventArgs e)
        {
            while (markers[0].Count > 0)
            {
                markers[0].RemoveAt(0);
            }
            Sheet.Value = new Random().Next(0, 1024);

        }

        private void AddMarker_Click(object sender, RoutedEventArgs e)
        {
            //var marker = ;
            //marker.Value = Heel.Value;
            //marker.MarkerColor
            Heel.GoalMarkers[0].Value = new Random().Next(0, 1024);
            Heel.GoalMarkers.Add(new GoalMarker());
        }
    }
}
