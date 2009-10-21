using System;
using System.Collections.Generic;
using System.Text;
using System.Xml;
using Apoc.Vfs;

namespace Apoc.Config
{
    /// <summary>
    ///  表示一个XML配置文件
    /// </summary>
    public class XmlConfiguration : Configuration
    {

        public static bool MoveToNextElement(XmlTextReader xmlIn)
        {
            if (!xmlIn.Read())
                return false;

            while (xmlIn.NodeType == XmlNodeType.EndElement)
            {
                if (!xmlIn.Read())
                    return false;
            }

            return true;
        }

        public XmlConfiguration(string file)
            : this(new FileLocation(file))
        { }

        public XmlConfiguration(ResourceLocation file) :
            base(file.Name, CaseInsensitiveStringComparer.Instance)
        {

            XmlTextReader xml = new XmlTextReader(file.GetStream);

            //XmlSection parentSection = null;
            int depth = xml.Depth;
            Stack<XmlSection> stack = new Stack<XmlSection>();
            stack.Push(null);

            while (MoveToNextElement(xml))
            {
                if (xml.Depth > depth)
                {
                    stack.Push(new XmlSection(this, xml.Name, stack.Peek()));
                    depth = xml.Depth;
                }
                else if (xml.Depth == depth)
                {
                    if (stack.Peek() != null)
                    {
                        stack.Peek().Add(xml.Name, xml.Value);
                    }
                }
                else
                {
                    stack.Pop();
                    depth = xml.Depth;
                }                
            }

            xml.Close();
        }

        public XmlConfiguration(string name, int cap)
            : base(name, cap, CaseInsensitiveStringComparer.Instance)
        { }

        public override Configuration Clone()
        {
            XmlConfiguration xml = new XmlConfiguration(Name, Count);

            foreach (KeyValuePair<string, ConfigurationSection> e1 in this)
            {
                //Dictionary<string, string> newSectData = new Dictionary<string, string>(e1.Value.Count);
                XmlSection source = (XmlSection)e1.Value;

                XmlSection newSect = new XmlSection(xml, e1.Key, source.Count, source.ParentSection);

                foreach (KeyValuePair<string, string> e2 in source)
                {
                    newSect.Add(e2.Key, e2.Value);
                }

                xml.Add(e1.Key, newSect);
            }
            return xml;
        }

        public override void Merge(Configuration config)
        {
            Configuration copy = config.Clone();

            foreach (KeyValuePair<string, ConfigurationSection> e1 in copy)
            {
                ConfigurationSection sect;
                if (!TryGetValue(e1.Key, out sect))
                {
                    Add(e1.Key, e1.Value);
                }
                else
                {
                    foreach (KeyValuePair<string, string> e2 in e1.Value)
                    {
                        if (sect.ContainsKey(e2.Key))
                        {
                            sect.Remove(e2.Key);
                        }

                        sect.Add(e2.Key, e2.Value);

                    }
                }
            }
        }
    }
}
