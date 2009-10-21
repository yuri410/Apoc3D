using System;
using System.Collections.Generic;
using System.Text;
using System.Windows.Forms;

namespace VirtualBicycle.Ide.Designers
{
    public abstract class DocumentConfigBase
    {
        static List<DocumentConfigBase> existing;
        public static void SaveAll()
        {
            Environment.CurrentDirectory = Application.StartupPath;
            if (existing != null)
            {
                for (int i = 0; i < existing.Count; i++)
                    existing[i].Save();
            }
        }

        //protected bool isSaved;

        //public void Save()
        //{
        //    //if (!isSaved)
        //    //{
        //        //SaveImpl();
        //        //isSaved = true;
        //    //}
        //}

        protected abstract void Save();

        protected DocumentConfigBase()
        {
            if (existing == null)
                existing = new List<DocumentConfigBase>();
            existing.Add(this);
        }
    }
}

