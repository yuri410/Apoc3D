using System;
using System.Collections.Generic;
using System.Text;
using VirtualBicycle.IO;

namespace VirtualBicycle.Ide.Designers.WorldBuilder
{
    public class WorldDesignerFactory : DesignerAbstractFactory
    {

        public override DocumentBase CreateInstance(ResourceLocation res)
        {
            return new WorldDesigner(this, res);
        }

        public override Type CreationType
        {
            get { return typeof(WorldDesigner); }
        }

        public override string Description
        {
            get { return DevStringTable.Instance["DOCS:VMPDESC"]; }
        }

        public override string[] Filters
        {
            get { return new string[1] { WorldDesigner.Extension }; }
        }
    }
}
