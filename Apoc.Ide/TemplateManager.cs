using System;
using System.Collections.Generic;
using System.Text;
using VirtualBicycle.Ide.Templates;
using VirtualBicycle.Ide.Designers;

namespace VirtualBicycle.Ide
{
    public class TemplateManager
    {
        static TemplateManager singleton;

        public static TemplateManager Instance
        {
            get
            {
                if (singleton == null)
                {
                    singleton = new TemplateManager();
                }
                return singleton; 
            }
        }

        List<FileTemplateBase> fileTmps;

        private TemplateManager()
        {
            fileTmps = new List<FileTemplateBase>(256);
        }

        public void RegisterTemplate(FileTemplateBase tem)
        {
            if (fileTmps.Contains(tem))
            {
                throw new InvalidOperationException();
            }
            fileTmps.Add(tem);
        }

        public void UnregisterTemplate(FileTemplateBase tem)
        {
            fileTmps.Remove(tem);
        }

        public FileTemplateBase[] GetFileTemplates()
        {
            return fileTmps.ToArray();
        }

        public FileTemplateBase[] GetFileTemplates(int platform)
        {
            List<FileTemplateBase> res = new List<FileTemplateBase>(fileTmps.Count);

            for (int i = 0; i < res.Count; i++)
            {
                if ((res[i].Platform & platform) != 0)
                {
                    res.Add(res[i]);
                }
            }
            return res.ToArray();
        }
    }
}
