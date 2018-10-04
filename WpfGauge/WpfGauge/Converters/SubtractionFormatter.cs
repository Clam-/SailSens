using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Windows.Data;
using System.Globalization;

namespace WpfGauge.Converters
{
    internal class SubtractionFormatter : IValueConverter
    {
        public object Convert(object value, Type targetType, object parameter, CultureInfo culture)
        {
            if (value is double)
            {
                var d = (double)value;
                if (parameter is double)
                {
                    var p = (double)parameter;
                    return d - p;
                }
                if (parameter is int)
                {
                    var p = (int)parameter;
                    return d - p;
                }
            }
            return value;
        }

        public object ConvertBack(object value, Type targetType, object parameter, CultureInfo culture)
        {
            return null;
        }
    }
}
