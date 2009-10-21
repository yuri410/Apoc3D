using System;
using System.Collections.Generic;
using System.Text;
using System.IO;
using VirtualBicycle;
using VirtualBicycle.Ide.Designers;

namespace VirtualBicycle.Ide.Templates
{
    public class CsfTemplate : FileTemplateBase
    {

        public override DocumentBase CreateInstance(string fileName)
        {
            if (string.IsNullOrEmpty(fileName))
            {
                return DesignerManager.Instance.CreateDocument(null, CsfDesigner.Extension);
            }
            else
            {
                DevFileLocation fl = new DevFileLocation(fileName);

                BinaryWriter bw = new BinaryWriter(fl.GetStream, Encoding.Default);
                bw.Write((int)StringTableCsfFormat.CsfID);
                bw.Write((int)3);

                bw.Write((int)0);
                bw.Write((int)0);
                bw.Write((int)0);
                bw.Close();

                return DesignerManager.Instance.CreateDocument(fl, CsfDesigner.Extension);
            }
        }
        public override string DefaultFileName
        {
            get { return "CsfFile.csf"; }
        }
        public override string Description
        {
            get { return DevStringTable.Instance["MSG:CSFDESC"]; }
        }

        public override int Platform
        {
            get { return PresetedPlatform.VirtualBike | PresetedPlatform.YurisRevenge | PresetedPlatform.Ra2Reload; }
        }

        public override string Name
        {
            get { return DevStringTable.Instance["DOCS:CSFDESC"]; }
        }


        public override string Filter
        {
            get
            {
                return DesignerManager.Instance.FindFactory(CsfDesigner.Extension).Filter;
            }
        }
    }

    //public class Tile3DTemplate : FileTemplateBase
    //{

    //    public override DocumentBase CreateInstance(string fileName)
    //    {
    //        if (string.IsNullOrEmpty(fileName))
    //        {
    //            return DesignerManager.Instance.CreateDocument(null, Tile3DDocument.Extension);
    //        }
    //        else
    //        {
    //            DevFileLocation fl = new DevFileLocation(fileName);

    //            EditableTile3D data = new EditableTile3D();


    //            EditableBlock[] blocks = new EditableBlock[1];
    //            blocks[0].mat1 = new EditableBlockMaterial();
    //            blocks[0].mat1.Texture = null;
    //            blocks[0].bits = BlockBits.None;

    //            data.Blocks = blocks;

    //            EditableTile3D.ToFile(data, fileName);

    //            return DesignerManager.Instance.CreateDocument(fl, CsfDocument.Extension);
    //        }
    //    }

    //    public override string Filter
    //    {
    //        get { return DesignerManager.Instance.FindFactory(Tile3DDocument.Extension).Filter; }
    //    }

    //    public override string Description
    //    {
    //        get { return Program.StringTable["MSG:TILE3DDesc"]; }
    //    }

    //    public override int Platform
    //    {
    //        get { return PresetedPlatform.Ra2Reload; }
    //    }

    //    public override string Name
    //    {
    //        get { return Program.StringTable["DOCS:TILE3DDesc"]; }
    //    }
    //}
}
