using System;
using System.Collections.Generic;
using System.IO;
using System.Text;
using System.Windows.Forms;
using VirtualBicycle.Ide.Designers;
using VirtualBicycle.Ide.Designers.WorldBuilder;
using VirtualBicycle.Ide.Editors.EditableObjects;
using VirtualBicycle.Scene;

namespace VirtualBicycle.Ide.Templates
{
    class WorldTemplate : FileTemplateBase
    {
        public override DocumentBase CreateInstance(string fileName)
        {
            NewWorldForm dlg = new NewWorldForm();

            dlg.ShowDialog();

            if (dlg.DialogResult == DialogResult.OK)
            {
                if (string.IsNullOrEmpty(fileName))
                {
                    fileName = Path.Combine(Application.StartupPath, DefaultFileName);
                }

                EditableSceneData.CreateNewScene(NewWorldForm.SceneWidth, NewWorldForm.SceneHeight, fileName);

                DocumentBase doc = DesignerManager.Instance.CreateDocument(new DevFileLocation(fileName), WorldDesigner.Extension);
                return doc;
            }
            return null;
        }

        public override string DefaultFileName
        {
            get
            {
                return "Scene.vmp";
            }
        }

        public override string Filter
        {
            get { return DesignerManager.Instance.FindFactory(WorldDesigner.Extension).Filter; }
        }

        public override string Description
        {
            get { return DevStringTable.Instance["MSG:VMPDESC"]; }
        }

        public override int Platform
        {
            get { return PresetedPlatform.VirtualBike | PresetedPlatform.YurisRevenge | PresetedPlatform.Ra2Reload; }
        }

        public override string Name
        {
            get { return DevStringTable.Instance["DOCS:VMPDESC"]; }
        }
    }
}
