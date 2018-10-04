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
using System.Windows.Threading;
using System.ComponentModel;
using System.Collections.ObjectModel;

namespace WpfGauge
{
    /// <summary>
    /// Interaction logic for Gauge.xaml
    /// </summary>
    public partial class Gauge : UserControl
    {
        #region Dependency Properties

        public static DependencyProperty TickLabelStyleProperty =
            DependencyProperty.Register("TickLabelStyle", typeof(Style), typeof(Gauge));

        public static DependencyProperty AutoScaleProperty =
            DependencyProperty.Register("AutoScale", typeof(bool), typeof(Gauge), new PropertyMetadata(true));

        public static DependencyProperty LineMarginProperty =
            DependencyProperty.Register("LineMargin", typeof(Thickness), typeof(Gauge),
            new PropertyMetadata(new Thickness(40,0,0,0)));

        public static DependencyProperty LabelMarginProperty =
            DependencyProperty.Register("LabelMargin", typeof(Thickness), typeof(Gauge),
            new PropertyMetadata(new Thickness(0, 20, 0, 0)));

        public static DependencyProperty MajorTickMarkColorProperty =
            DependencyProperty.Register("MajorTickMarkColor", typeof(Brush), typeof(Gauge),
            new PropertyMetadata(Brushes.White, OnMajorTickMarkColorChanged));

        public static DependencyProperty MinorTickMarkColorProperty =
            DependencyProperty.Register("MinorTickMarkColor", typeof(Brush), typeof(Gauge),
            new PropertyMetadata(Brushes.White, OnMinorTickMarkColorChanged));

        public static DependencyProperty InnerEllipseStrokeProperty =
            DependencyProperty.Register("InnerEllipseStroke", typeof(Brush), typeof(Gauge),
            new PropertyMetadata(OnInnerEllipseStrokeChanged));

        public static void OnInnerEllipseStrokeChanged(DependencyObject d, DependencyPropertyChangedEventArgs e)
        {
            var gauge = d as Gauge;
            if (e.NewValue == null)
                gauge.InnerEllipse.Stroke = Brushes.DimGray;
            else
                gauge.InnerEllipse.Stroke = (Brush)e.NewValue;
        }

        public static DependencyProperty OuterEllipseStrokeProperty =
            DependencyProperty.Register("OuterEllipseStroke", typeof(Brush), typeof(Gauge),
            new PropertyMetadata(OnOuterEllipseStrokeChanged));

        public static void OnOuterEllipseStrokeChanged(DependencyObject d, DependencyPropertyChangedEventArgs e)
        {
            var gauge = d as Gauge;
            if (e.NewValue == null)
                gauge.OuterEllipse.Stroke = Brushes.DimGray;
            else
                gauge.OuterEllipse.Stroke = (Brush)e.NewValue;
        }

        public static DependencyProperty InnerEllipseFillProperty =
            DependencyProperty.Register("InnerEllipseFill", typeof(Brush), typeof(Gauge),
            new PropertyMetadata(OnInnerEllipseFillChanged));

        public static void OnInnerEllipseFillChanged(DependencyObject d, DependencyPropertyChangedEventArgs e)
        {
            var gauge = d as Gauge;
            if (e.NewValue == null)
                gauge.InnerEllipse.Fill = Brushes.Black;
            else
                gauge.InnerEllipse.Fill = (Brush)e.NewValue;
        }

        public static DependencyProperty OuterEllipseFillProperty =
            DependencyProperty.Register("OuterEllipseFill", typeof(Brush), typeof(Gauge),
            new PropertyMetadata(OnOuterEllipseFillChanged));

        public static void OnOuterEllipseFillChanged(DependencyObject d, DependencyPropertyChangedEventArgs e)
        {
            var gauge = d as Gauge;
            if (e.NewValue == null)
                gauge.OuterEllipse.Fill = Defaults.GradientBrushes.GaugeOuterEllipseBackground;
            else
                gauge.OuterEllipse.Fill = (Brush)e.NewValue;
        }

        public static DependencyProperty ValueProperty =
            DependencyProperty.Register("Value", typeof(double), typeof(Gauge),
            new PropertyMetadata(double.NegativeInfinity, OnGaugeChanged));

        public static DependencyProperty ValueTextStyleProperty =
            DependencyProperty.Register("ValueTextStyle", typeof(Style), typeof(Gauge), 
            new PropertyMetadata(OnValueTextStyleChanged));

        public static DependencyProperty MaxValueProperty =
            DependencyProperty.Register("MaxValue", typeof(double), typeof(Gauge), new PropertyMetadata(double.NegativeInfinity));

        public static DependencyProperty MinValueProperty =
            DependencyProperty.Register("MinValue", typeof(double), typeof(Gauge), new PropertyMetadata(0.0d));

        public static DependencyProperty NeedleTooltipProperty =
            DependencyProperty.Register("NeedleTooltip", typeof(object), typeof(Gauge), 
            new PropertyMetadata(null, OnNeedleTooltipChanged));

        public static DependencyProperty IsNeedleTooltipEnabledProperty =
            DependencyProperty.Register("IsNeedleTooltipEnabled", typeof(bool), typeof(Gauge), new PropertyMetadata(false));

        public static DependencyProperty LightVisibilityProperty =
            DependencyProperty.Register("LightVisibility", typeof(Visibility), typeof(Gauge));

        public static DependencyProperty ControlGridProperty =
            DependencyProperty.Register("ControlGrid", typeof(Grid), typeof(Gauge), new PropertyMetadata(OnControlsGridChanged));

        public static DependencyProperty ValueFormatProperty =
            DependencyProperty.Register("ValueFormat", typeof(string), typeof(Gauge), new PropertyMetadata(OnValueFormatChanged));
        
        internal static DependencyProperty ValueTextMarginProperty =
            DependencyProperty.Register("ValueTextMargin", typeof(Thickness), typeof(Gauge), 
            new PropertyMetadata(new Thickness(0,100,0,0)));

        public static DependencyProperty MajorTickMarksProperty =
            DependencyProperty.Register("MajorTickMarks", typeof(ObservableCollection<TickMarker>), typeof(Gauge));

        public static DependencyProperty MinorTickMarksProperty =
            DependencyProperty.Register("MinorTickMarks", typeof(ObservableCollection<TickMarker>), typeof(Gauge));

        public static DependencyProperty StartAngleProperty =
            DependencyProperty.Register("StartAngle", typeof(double), typeof(Gauge), new PropertyMetadata(-45.0, OnGaugeChanged));

        public static DependencyProperty EndAngleProperty =
            DependencyProperty.Register("EndAngle", typeof(double), typeof(Gauge), new PropertyMetadata(225.0, OnGaugeChanged));

        public static DependencyProperty MajorTickCountProperty =
            DependencyProperty.Register("MajorTickCount", typeof(int), typeof(Gauge), new PropertyMetadata(10, OnGaugeChanged));

        public static DependencyProperty MinorTickCountProperty =
            DependencyProperty.Register("MinorTickCount", typeof(int), typeof(Gauge), new PropertyMetadata(4, OnGaugeChanged));

        public static DependencyProperty GoalMarkersProperty =
            DependencyProperty.Register("GoalMarkers", typeof(ObservableCollection<GoalMarker>), typeof(Gauge), 
            new PropertyMetadata(new ObservableCollection<GoalMarker>()));

        public static void OnGaugeChanged(DependencyObject d, DependencyPropertyChangedEventArgs e)
        {
            var gauge = d as Gauge;
            gauge.UpdateAngle();
            if (!string.IsNullOrWhiteSpace(gauge.ValueFormat))
                gauge.ValueText.Text = string.Format(gauge.ValueFormat.Replace("{}",""), gauge.Value);
            else
                gauge.ValueText.Text = gauge.Value.ToString();
        }

        public static void OnNeedleTooltipChanged(DependencyObject d, DependencyPropertyChangedEventArgs e)
        {
            var gauge = d as Gauge;
            if (e.NewValue == null)
                gauge.IsNeedleTooltipEnabled = false;
            else
                gauge.IsNeedleTooltipEnabled = true;
        }

        public static void OnValueFormatChanged(DependencyObject d, DependencyPropertyChangedEventArgs e)
        {
            var gauge = d as Gauge;
            gauge.UpdateAngle();
        }

        public static void OnValueTextStyleChanged(DependencyObject d, DependencyPropertyChangedEventArgs e)
        {
            var gauge = d as Gauge;
            if (e.NewValue == null)
                gauge.SetValueTextDefaultStyle();
            else
                gauge.ValueText.Style = gauge.ValueTextStyle;
        }

        public static void OnMajorTickMarkColorChanged(DependencyObject d, DependencyPropertyChangedEventArgs e)
        {
            var gauge = d as Gauge;
            if (gauge.MajorTickMarks != null)
            {
                // Set all Major Tick Fill Properties
                foreach (var tick in gauge.MajorTickMarks)
                    tick.TickMarkColor = gauge.MajorTickMarkColor;
            }
        }

        public static void OnMinorTickMarkColorChanged(DependencyObject d, DependencyPropertyChangedEventArgs e)
        {
            var gauge = d as Gauge;
            if (gauge.MinorTickMarks != null)
            {
                // Set all Major Tick Fill Properties
                foreach (var tick in gauge.MinorTickMarks)
                    tick.TickMarkColor = gauge.MajorTickMarkColor;
            }
        }

        public static void OnControlsGridChanged(DependencyObject d, DependencyPropertyChangedEventArgs e)
        {
            var gauge = d as Gauge;
            gauge.LayoutRoot.Children.Add((Grid)e.NewValue);
        }

        #endregion Dependency Properties

        #region Public Vars

        public bool AutoScale
        {
            get { return (bool)base.GetValue(AutoScaleProperty); }
            set { base.SetValue(AutoScaleProperty, value); }
        }

        public Style TickLabelStyle
        {
            get { return (Style)base.GetValue(TickLabelStyleProperty); }
            set { base.SetValue(TickLabelStyleProperty, value); }
        }

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

        public Brush InnerEllipseStroke
        {
            get { return (Brush)base.GetValue(InnerEllipseStrokeProperty); }
            set { base.SetValue(InnerEllipseStrokeProperty, value); }
        }

        public Brush MajorTickMarkColor
        {
            get { return (Brush)base.GetValue(MajorTickMarkColorProperty); }
            set { base.SetValue(MajorTickMarkColorProperty, value); }
        }

        public Brush MinorTickMarkColor
        {
            get { return (Brush)base.GetValue(MinorTickMarkColorProperty); }
            set { base.SetValue(MinorTickMarkColorProperty, value); }
        }

        public Brush OuterEllipseStroke
        {
            get { return (Brush)base.GetValue(OuterEllipseStrokeProperty); }
            set { base.SetValue(OuterEllipseStrokeProperty, value); }
        }

        public Brush InnerEllipseFill
        {
            get { return (Brush)base.GetValue(InnerEllipseFillProperty); }
            set { base.SetValue(InnerEllipseFillProperty, value); }
        }

        public Brush OuterEllipseFill
        {
            get { return (Brush)base.GetValue(OuterEllipseFillProperty); }
            set { base.SetValue(OuterEllipseFillProperty, value); }
        }

        public double Value
        {
            get { return (double)base.GetValue(ValueProperty); }
            set { base.SetValue(ValueProperty, value); }
        }

        public Style ValueTextStyle
        {
            get { return (Style)base.GetValue(ValueTextStyleProperty); }
            set { base.SetValue(ValueTextStyleProperty, value); }
        }

        public double MaxValue
        {
            get { return (double)base.GetValue(MaxValueProperty); }
            set { base.SetValue(MaxValueProperty, value); }
        }

        public double MinValue
        {
            get { return (double)base.GetValue(MinValueProperty); }
            set { base.SetValue(MinValueProperty, value); }
        }

        public Thickness ValueTextMargin
        {
            get { return (Thickness)base.GetValue(ValueTextMarginProperty); }
            set { base.SetValue(ValueTextMarginProperty, value); }
        }

        public object NeedleTooltip
        {
            get { return (object)base.GetValue(NeedleTooltipProperty); }
            set { base.SetValue(NeedleTooltipProperty, value); }
        }

        public bool IsNeedleTooltipEnabled
        {
            get { return (bool)base.GetValue(IsNeedleTooltipEnabledProperty); }
            set { base.SetValue(IsNeedleTooltipEnabledProperty, value); }
        }

        public Visibility LightVisibility
        {
            get { return (Visibility)base.GetValue(LightVisibilityProperty); }
            set { base.SetValue(LightVisibilityProperty, value); }
        }

        public Grid ControlGrid
        {
            get { return (Grid)base.GetValue(ControlGridProperty); }
            set { base.SetValue(ControlGridProperty, value); }
        }

        public string ValueFormat
        {
            get { return (string)base.GetValue(ValueFormatProperty); }
            set { base.SetValue(ValueFormatProperty, value); }
        }

        public ObservableCollection<TickMarker> MajorTickMarks
        {
            get { return (ObservableCollection<TickMarker>)base.GetValue(MajorTickMarksProperty); }
            set { base.SetValue(MajorTickMarksProperty, value); }
        }

        public ObservableCollection<TickMarker> MinorTickMarks
        {
            get { return (ObservableCollection<TickMarker>)base.GetValue(MinorTickMarksProperty); }
            set { base.SetValue(MinorTickMarksProperty, value); }
        }

        public int MajorTickCount
        {
            get { return (int)base.GetValue(MajorTickCountProperty); }
            set { base.SetValue(MajorTickCountProperty, value); }
        }

        public int MinorTickCount
        {
            get { return (int)base.GetValue(MinorTickCountProperty); }
            set { base.SetValue(MinorTickCountProperty, value); }
        }

        public ObservableCollection<GoalMarker> GoalMarkers
        {
            get { return (ObservableCollection<GoalMarker>)base.GetValue(GoalMarkersProperty); }
            set { base.SetValue(GoalMarkersProperty, value); }
        }

        public double StartAngle
        {
            get { return (double)base.GetValue(StartAngleProperty); }
            set { base.SetValue(StartAngleProperty, value); }
        }

        public double EndAngle
        {
            get { return (double)base.GetValue(EndAngleProperty); }
            set { base.SetValue(EndAngleProperty, value); }
        }

        #endregion Public Vars

        public Gauge()
        {
            // Setup events
            this.SizeChanged += (s, e) => { GenerateNeedlePoints(); };

            InitializeComponent();

            // Add events
            this.Loaded += new RoutedEventHandler(Gauge_Loaded);
            if (this.GoalMarkers != null)
                this.GoalMarkers.CollectionChanged += new System.Collections.Specialized.NotifyCollectionChangedEventHandler(GoalMarkers_CollectionChanged);
        }

        private void Gauge_Loaded(object sender, RoutedEventArgs e)
        {
            UpdateAngle();

            if (this.ValueTextStyle == null)
                this.SetValueTextDefaultStyle();
        }

        private void GenerateNeedlePoints()
        {
            double h = this.Height;
            double w = this.Width;
            double a = (3.5d / 300d) * h;
            double b = (90d / 300d) * w;
            double c = (1.75d / 300d) * h;
            double d = (95d / 300d) * w;

            var points = new PointCollection();
            points.Add(new Point(a, a));
            points.Add(new Point(a, -a));
            points.Add(new Point(-b, -c));
            points.Add(new Point(-d, 0));
            points.Add(new Point(-b, c));
            Needle.Points = points;
        }

        private void GoalMarkers_CollectionChanged(object sender, System.Collections.Specialized.NotifyCollectionChangedEventArgs e)
        {
            if(e.Action == System.Collections.Specialized.NotifyCollectionChangedAction.Add)
                foreach (GoalMarker gm in e.NewItems)
                {
                    gm.ParentGauge = this;
                    gm.GaugeMax = this.MaxValue;
                    gm.GaugeStartAngle = this.StartAngle;
                    gm.GaugeEndAngle = this.EndAngle;
                    if (gm.Parent is Grid)
                        ((Grid)gm.Parent).Children.Remove(gm);
                    GoalMarkerLayout.Children.Add(gm);
                }
        }

        internal void DrawElements()
        {
            // TODO: Improve this...
            // First, clear any children that may exist in the grid
            DynamicLayout.Children.Clear();

            // There are no specified Major Tick Markers so auto-create them
            // The default number of  Major Tick Markers is 11 (0 - 10)
            // The total degrees of the scale
            var totalDegrees = this.EndAngle - this.StartAngle;
            var degreeIncrement = totalDegrees / (this.MajorTickCount);
            var labelIncrement = (this.MaxValue - this.MinValue) / (this.MajorTickCount);
            var smallDegreeIncrement = degreeIncrement / (this.MinorTickCount + 1);

            this.MajorTickMarks = new ObservableCollection<TickMarker>();
            
            for (int i = 0; i <= this.MajorTickCount; i++)
            {
                TickMarker majorTick = new TickMarker();

                majorTick.StrokeThickness = 8;
                majorTick.LineHeight = 10;
                Panel.SetZIndex(majorTick, 1);
                if (this.AutoScale)
                {
                    majorTick.Angle = (i * degreeIncrement) + this.StartAngle;
                    majorTick.Label = (i * labelIncrement).ToString();
                }
                else
                {
                    majorTick.Angle = (i * degreeIncrement) + this.StartAngle;
                    majorTick.Label = ((i * labelIncrement) + this.MinValue).ToString();
                }
                this.MajorTickMarks.Add(majorTick);
                DynamicLayout.Children.Add(majorTick);

                majorTick.TickMarkColor = this.MajorTickMarkColor;
                majorTick.LineMargin = this.LineMargin;
                majorTick.LabelMargin = this.LabelMargin;

                if (this.TickLabelStyle != null)
                {
                    majorTick.TickLabelStyle = this.TickLabelStyle;
                }
            }

            this.MinorTickMarks = new ObservableCollection<TickMarker>();

            foreach (var majorTick in this.MajorTickMarks)
            {
                if (this.MajorTickMarks.IndexOf(majorTick) < this.MajorTickMarks.Count - 1)
                {
                    for (int i = 0; i <= this.MinorTickCount; i++)
                    {
                        TickMarker minorTick = new TickMarker();
                        minorTick.StrokeThickness = 4;
                        minorTick.LineHeight = 5;
                        Panel.SetZIndex(minorTick, 0);
                        minorTick.Angle = (i * smallDegreeIncrement) + smallDegreeIncrement + majorTick.Angle;
                        this.MinorTickMarks.Add(minorTick);
                        DynamicLayout.Children.Add(minorTick);

                        minorTick.TickMarkColor = this.MinorTickMarkColor;
                        minorTick.LineMargin = this.LineMargin;
                        minorTick.LabelMargin = this.LabelMargin;
                    }
                }
            }
        }

        internal void UpdateAngle()
        {
            if (this.Value == double.NegativeInfinity)
            {
                NeedleRotateTransform.Angle = StartAngle - 10;
                NeedleDropShadowEffect.Direction = StartAngle - 10;
                DrawElements();
                return;
            }
            Needle.Visibility = System.Windows.Visibility.Visible;

            if (this.AutoScale)
            {
                var v = GetGreatestValue();
                this.MaxValue = v.GetGaugeTop();
                this.MinValue = 0;
            }

            double valueInPercent = this.Value / this.MaxValue;
            if (this.AutoScale)
                valueInPercent = this.Value / this.MaxValue;
            else
                valueInPercent = (this.Value - this.MinValue) / (this.MaxValue - this.MinValue);

            var valueInDegrees = valueInPercent * (this.EndAngle - this.StartAngle) + this.StartAngle;

            NeedleRotateTransform.Angle = valueInDegrees;
            NeedleDropShadowEffect.Direction = valueInDegrees;

            DrawElements();

            // Set Binding on Value
            /*Binding value = new Binding("Value");
            value.ElementName = "GaugeControl";
            
            value.UpdateSourceTrigger = UpdateSourceTrigger.PropertyChanged;
            value.StringFormat = this.ValueFormat;
            ValueText.SetBinding(TextBlock.TextProperty, value);

            var b = ValueText.GetBindingExpression(TextBlock.TextProperty);
            if (b != null)
                b.UpdateTarget();*/
        }

        // Get greatest value
        // compare all gauge markers and the gauge value to determine the greatest number
        private double GetGreatestValue()
        {
            if (this.GoalMarkers != null && this.GoalMarkers.Count > 0)
            {
                double greatest = this.Value;
                foreach (var goal in this.GoalMarkers.ToList())
                {
                    if (goal.Value > greatest)
                        greatest = goal.Value;
                }
                return greatest;
            }
            else
                return this.Value;
        }

        internal void SetValueTextDefaultStyle()
        {
            this.ValueText.Style = Defaults.DefaultValueTextStyle;
            // Set up the default margin binding
            Binding b = new Binding("Height");
            b.UpdateSourceTrigger = UpdateSourceTrigger.PropertyChanged;
            var relSource = new RelativeSource();
            relSource.Mode = RelativeSourceMode.FindAncestor;
            relSource.AncestorType = typeof(Gauge);
            b.RelativeSource = relSource;
            b.Converter = new WpfGauge.Converters.ValueTextMarginConverter();
            this.ValueText.SetBinding(TextBlock.MarginProperty, b);
        }
    }
}
