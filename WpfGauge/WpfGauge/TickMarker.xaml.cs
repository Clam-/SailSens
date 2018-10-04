using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Windows;
using System.Windows.Controls;
using System.Windows.Data;
using System.Windows.Documents;
using System.Windows.Input;
using System.Windows.Media;
using System.Windows.Media.Imaging;
using System.Windows.Navigation;
using System.Windows.Shapes;
using System.ComponentModel;

namespace WpfGauge
{
    /// <summary>
    /// Interaction logic for TickMarker.xaml
    /// </summary>
    public partial class TickMarker : UserControl, INotifyPropertyChanged
    {
        #region Dependency Properties

        public static DependencyProperty TickLabelStyleProperty =
            DependencyProperty.Register("TickLabelStyle", typeof(Style), typeof(TickMarker),
            new PropertyMetadata(Defaults.DefaultTickLabelStyle, OnTickLabelStyleChanged));

        public static void OnTickLabelStyleChanged(DependencyObject d, DependencyPropertyChangedEventArgs e)
        {
            var t = d as TickMarker;
            var o = t.TickLabelStyle;
        }

        public static DependencyProperty TickMarkColorProperty =
            DependencyProperty.Register("TickMarkColor", typeof(Brush), typeof(TickMarker),
            new PropertyMetadata(Brushes.White));

        public static DependencyProperty LineMarginProperty =
            DependencyProperty.Register("LineMargin", typeof(Thickness), typeof(TickMarker),
            new PropertyMetadata(new Thickness(0,0,0,0)));

        public static DependencyProperty LabelMarginProperty =
            DependencyProperty.Register("LabelMargin", typeof(Thickness), typeof(TickMarker),
            new PropertyMetadata(new Thickness(0, 20, 0, 0)));

        #endregion Dependency Properties

        #region Private Vars

        private double _labelAngle;
        private string _label;
        private double _angle;
        private double _strokeThickness;
        private double _lineHeight;

        #endregion Private Vars

        #region Public Vars

        public Thickness LineMargin
        {
            get { return (Thickness)base.GetValue(LineMarginProperty); }
            set { base.SetValue(LineMarginProperty, value); }
        }

        public Thickness LabelMargin
        {
            get { return (Thickness)base.GetValue(LabelMarginProperty); }
            set { base.SetValue(LabelMarginProperty, value); }
        }

        public Style TickLabelStyle
        {
            get { return (Style)base.GetValue(TickLabelStyleProperty); }
            set { base.SetValue(TickLabelStyleProperty, value); }
        }

        public Brush TickMarkColor
        {
            get { return (Brush)base.GetValue(TickMarkColorProperty); }
            set { base.SetValue(TickMarkColorProperty, value); }
        }

        public double LabelAngle
        {
            get { return _labelAngle; }
            set
            {
                _labelAngle = value;
                this.OnPropertyChanged("LabelAngle");
            }
        }

        public string Label
        {
            get { return _label; }
            set
            {
                _label = value;
                this.OnPropertyChanged("Label");
            }
        }

        public double Angle
        {
            get { return _angle; }
            set
            {
                _angle = value;
                this.OnPropertyChanged("Angle");
                this.LabelAngle = value - 90;
            }
        }

        public double StrokeThickness
        {
            get { return _strokeThickness; }
            set
            {
                _strokeThickness = value;
                this.OnPropertyChanged("StrokeThickness");
            }
        }

        public double LineHeight
        {
            get { return _lineHeight; }
            set
            {
                _lineHeight = value;
                this.OnPropertyChanged("LineHeight");
                this.LineMargin = new Thickness(55 - this.LineHeight, 0, 0, 0);
            }
        }

        #endregion Public Vars

        public TickMarker()
        {
            InitializeComponent();
        }

        #region INotifyPropertyChanged

        public event PropertyChangedEventHandler PropertyChanged;
        protected void OnPropertyChanged(string propertyName)
        {
            PropertyChangedEventHandler handler = PropertyChanged;

            if (handler != null)
            {
                handler(this, new PropertyChangedEventArgs(propertyName));
            }
        }

        #endregion INotifyPropertyChanged

    }
}
