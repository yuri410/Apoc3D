using System;
using System.Collections.Generic;
using System.Drawing;
using System.Text;
using VirtualBicycle.Ide.Properties;
using VirtualBicycle.Ide.Tools;

namespace VirtualBicycle.Ide.Designers.WorldBuilder
{
    public class RoadCategory : ToolBoxCategory
    {

        public override Image Icon
        {
            get { return Resources.PointerHS; }
        }

        public override string Name
        {
            get { return DevStringTable.Instance["Tool:CategoryRoad"]; }
        }
    }
    public class BuildingCategory : ToolBoxCategory
    {
        public override Image Icon
        {
            get { return Resources.PointerHS; }
        }

        public override string Name
        {
            get { return DevStringTable.Instance["Tool:CategoryBuilding"]; }
        }
    }
    public class TreeCategory : ToolBoxCategory
    {
        public override Image Icon
        {
            get { return Resources.PointerHS; }
        }

        public override string Name
        {
            get { return DevStringTable.Instance["Tool:CategoryTree"]; }
        }
    }
    public class TOCategory : ToolBoxCategory
    {

        public override Image Icon
        {
            get { return Resources.PointerHS; }
        }

        public override string Name
        {
            get { return DevStringTable.Instance["Tool:CategoryTO"]; }
        }
    }

    public class LogicCategory : ToolBoxCategory
    {

        public override Image Icon
        {
            get { return Resources.PointerHS; }
        }

        public override string Name
        {
            get { return DevStringTable.Instance["Tool:CategoryLogic"]; }
        }
    }

    public class ToolCategories
    {
        static ToolCategories singleton;

        public static ToolCategories Instance
        {
            get
            {
                if (singleton == null)
                {
                    singleton = new ToolCategories();
                }
                return singleton;
            }
        }

        private ToolCategories()
        {
            Road = new RoadCategory();
            Building = new BuildingCategory();
            Tree = new TreeCategory();
            TO = new TOCategory();
            Logic = new LogicCategory();
        }

        public LogicCategory Logic
        {
            get;
            private set;
        }
        public RoadCategory Road
        {
            get;
            private set;
        }

        public BuildingCategory Building
        {
            get;
            private set;
        }

        public TreeCategory Tree
        {
            get;
            private set;
        }

        public TOCategory TO
        {
            get;
            private set;
        }

    }
}
