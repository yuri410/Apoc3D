using System;
using System.Collections.Generic;
using System.Text;
using System.Drawing;
using VirtualBicycle.Ide.Designers;
using VirtualBicycle.Ide.Projects;

namespace VirtualBicycle.Ide.Templates
{
    /// <summary>
    /// 模板控制着文档和项目的创建。Factory method
    /// </summary>
    public abstract class TemplateBase
    {
        protected Icon icon;

        public Icon GetIcon
        {
            get
            {
                if (icon == null)
                {
                    return Program.DefaultIcon;
                }
                return icon;
            }
        }

        public virtual string CategoryPath
        {
            get { return DevStringTable.Instance["GUI:DEFCATE"]; }
        }

        public abstract string Description
        {
            get; 
        }

        public abstract int Platform
        {
            get;
        }

        public abstract string Name
        {
            get;
        }
    }

    /// <summary>
    /// 文件模板控制着文档的创建。性质类似抽象工厂。
    /// </summary>
    public abstract class FileTemplateBase : TemplateBase
    {
        public abstract DocumentBase CreateInstance(string fileName);

        public abstract string Filter
        {
            get;
        }

        public virtual string DefaultFileName
        {
            get { return DevStringTable.Instance["GUI:DefFileName"]; }
        }
    }
    public abstract class ProjectTemplateBase : TemplateBase     
    {
        public abstract ProjectBase CreateInstance();
    }

}
