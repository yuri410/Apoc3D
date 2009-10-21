using System;
using System.Collections.Generic;
using System.Text;

namespace VirtualBicycle.Ide.Projects
{
    public abstract class ProjectBase
    {
        protected string projectPath;
        protected string name;

        protected ProjectBase(string name, string path)
        {
            projectPath = path;
            this.name = name == null ? string.Empty : name;
        }

        public string ProjectPath
        {
            get { return projectPath; }
        }
        public string Name
        {
            get { return name; }
            set { name = value; }
        }

        public abstract void Save();
        public abstract void Load();
    }
}
