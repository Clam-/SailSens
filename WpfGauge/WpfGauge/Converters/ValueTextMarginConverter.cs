using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Windows.Data;
using System.Globalization;
using System.Windows;

namespace WpfGauge.Converters
{
    internal class ValueTextMarginConverter : IValueConverter
    {
        public object Convert(object value, Type targetType, object parameter, CultureInfo culture)
        {
            if (value is double)
            {
                var h = (double)value;

                if (double.IsNaN(h) || double.IsInfinity(h) || h == 0)
                    return new Thickness(0, 100, 0, 0);
                else
                    return new Thickness(0, h / 3, 0, 0);
            }
            return value;
        }

        public object ConvertBack(object value, Type targetType, object parameter, CultureInfo culture)
        {
            return null;
        }

    }
}
