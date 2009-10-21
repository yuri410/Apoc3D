using System;
using System.Collections.Generic;
using System.Text;
using VirtualBicycle.Ide.Tools;

namespace VirtualBicycle.Ide
{
    public class ToolManager
    {
        static ToolManager singleton;

        public static ToolManager Instance
        {
            get
            {
                if (singleton == null)
                    singleton = new ToolManager();
                return singleton;
            }
        }

        Dictionary<Type, IToolAbstractFactory> factories;
        Dictionary<string, IToolAbstractFactory> factories2;

        

        private ToolManager()
        {
            factories = new Dictionary<Type, IToolAbstractFactory>();
            factories2 = new Dictionary<string, IToolAbstractFactory>();
        }

        public ITool CreateTool(string typeName)
        {
            IToolAbstractFactory fac;
            if (factories2.TryGetValue(typeName, out fac))
            {
                return fac.CreateInstance();
            }
            else
                throw new NotSupportedException();
        }
        public ITool CreateTool(Type type)
        {
            IToolAbstractFactory fac;
            if (factories.TryGetValue(type, out fac))
            {
                return fac.CreateInstance();
            }
            else
                throw new NotSupportedException();
        }

        public void RegisterToolType(IToolAbstractFactory fac)
        {
            factories.Add(fac.CreationType, fac);
            factories2.Add(fac.CreationType.ToString(), fac);
        }
        public void UnregisterToolType(IToolAbstractFactory fac)
        {
            factories.Remove(fac.CreationType);
            factories2.Remove(fac.CreationType.ToString());

        }
    }
}
