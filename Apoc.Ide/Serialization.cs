using System;
using System.Collections.Generic;
using System.Text;
using System.IO;
using System.Xml.Serialization;

namespace VirtualBicycle.Ide
{
    public static class Serialization
    {
        public static T XmlDeserialize<T>(string file) where T : class
        {
            StreamReader sr = new StreamReader(file, Encoding.Unicode);

            T obj = (T)(new XmlSerializer(typeof(T)).Deserialize(sr));

            sr.Close();
            return obj;
        }

        public static void XmlSerialize<T>(T obj, string file) where T : class
        {
            FileStream fs = new FileStream(file, FileMode.OpenOrCreate, FileAccess.Write, FileShare.Write);
            fs.SetLength(0);
            StreamWriter sw = new StreamWriter(fs, Encoding.Unicode);

            new XmlSerializer(typeof(T)).Serialize(sw, obj);

            sw.Close();
        }
    }
}
