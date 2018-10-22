using System.Collections.Generic;

namespace SensorDisplay
{
    static class LogBuffer
    {
        static LinkedList<string> _buff = new LinkedList<string>();
        private static int MAXSIZE = 20;
        public static string Log(string s)
        {
            _buff.AddLast(s);
            if (_buff.Count > MAXSIZE)
                _buff.RemoveFirst();
            return string.Join("\n", _buff);
        }

    }
}
