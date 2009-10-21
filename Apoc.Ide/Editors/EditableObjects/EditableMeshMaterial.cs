using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Drawing.Design;
using System.IO;
using System.Text;
using SlimDX;
using SlimDX.Direct3D9;
using VirtualBicycle.Ide.Designers;
using VirtualBicycle;
using VirtualBicycle.Graphics;
using VirtualBicycle.Graphics.Effects;
using VirtualBicycle.IO;
using System.Windows.Forms;

namespace VirtualBicycle.Ide.Editors.EditableObjects
{
    public delegate void EditObjChangedHandler();
    [TypeConverter(typeof(MeshMaterialConverter))]
    public class EditableMeshMaterial : MeshMaterialBase<Texture>, IComparable<MeshMaterial>
    {
        #region Fields

        string effectName;

        ImageFileFormat[] textureFormat = new ImageFileFormat[MaxTexLayers];

        Device device;

        EditObjChangedHandler changed;

        #endregion

        #region Constructors
        public EditableMeshMaterial()
        {
            this.device = GraphicsDevice.Instance.Device;
            for (int i = 0; i < MaxTexLayers; i++)
            {
                this.textureFormat[i] = ImageFileFormat.Png;
            }
        }
        #endregion

        #region Methods

        void Changed()
        {
            if (changed != null)
                changed();
        }
        //protected override EffectBase LoadEffect(string name)
        //{
        //    return null;
        //}

        //protected override ImageBase LoadTexture(ArchiveBinaryReader br, bool isEmbeded)
        //{
        //    throw new NotImplementedException();
        //}

        protected override ModelEffect LoadEffect(string name)
        {
            return null;
        }

        protected unsafe override void SaveTexture(ContentBinaryWriter bw, Texture tex, bool isEmbeded, int index)
        {
            if (isEmbeded)
            {
                ImageFileFormat fmt = textureFormat[index];

                DataStream stm = Texture.ToStream(tex, fmt);

                byte* data = (byte*)stm.DataPointer.ToPointer();

                byte[] buffer = new byte[(int)stm.Length];
                fixed (void* dst = &buffer[0])
                {
                    Memory.Copy(data, dst, buffer.Length);
                }

                stm.Close();
                stm.Dispose();
                
                bw.Write(buffer);

            }
            else
            {
                bw.WriteStringUnicode(textureFiles[index]);
            }
        }
        protected override Texture LoadTexture(ContentBinaryReader br, bool isEmbeded, int index)
        {
            if (isEmbeded)
            {
                ImageInformation imgInfo;
                Texture res = Texture.FromStream(device, br.BaseStream, 0, -1, -1, -1, Usage.None, Format.Unknown, Pool.Managed, Filter.Default, Filter.Default, 0, out imgInfo);

                textureFormat[index] = imgInfo.ImageFileFormat;
               
                return res;
            }
            else
            {
                string file = br.ReadStringUnicode().Trim();

                textureFiles[index] = file;

                if (file.Length > 0)
                {
                    //file = Path.Combine(defPath, file);
                    if (File.Exists(file))
                    {
                        return Texture.FromFile(device, file, Usage.None, Pool.Managed);
                    }
                    else
                    {
                        MessageBox.Show(string.Format(DevStringTable.Instance["MSG:MissingTexture"], file), 
                            DevStringTable.Instance["GUI:Error"], MessageBoxButtons.OK, MessageBoxIcon.Error);
                    }
                }
                return null;
            }
        }

        protected override void DestroyTexture(Texture tex)
        {
            tex.Dispose();
        }


        public static EditableMeshMaterial FromBinary(BinaryDataReader data)
        {
            EditableMeshMaterial res = new EditableMeshMaterial();

            res.ReadData(data);

            return res;
        }
        //public static EditableMeshMaterial FromBinary(Device dev, BinaryDataReader sounds, string path)
        //{
        //    EditableMeshMaterial res = new EditableMeshMaterial();

        //    res.defPath = path;
        //    res.ReadData(dev, sounds);

        //    return res;
        //}
        public static BinaryDataWriter ToBinary(EditableMeshMaterial mat)
        {
            BinaryDataWriter data = new BinaryDataWriter();
            mat.WriteData(data);

            return data;
        }

        #endregion

        #region Properties

        [Browsable(false)]
        public EditObjChangedHandler StateChanged
        {
            get { return changed; }
            set { changed = value; }
        }

        public ImageFileFormat Texture1Format
        {
            get { return textureFormat[0]; }
            set { textureFormat[0] = value; }
        }
        public ImageFileFormat Texture2Format
        {
            get { return textureFormat[1]; }
            set { textureFormat[1] = value; }
        }
        public ImageFileFormat Texture3Format
        {
            get { return textureFormat[2]; }
            set { textureFormat[2] = value; }
        }
        public ImageFileFormat Texture4Format
        {
            get { return textureFormat[3]; }
            set { textureFormat[3] = value; }
        }

        public string TextureFile1
        {
            get { return textureFiles[0]; }
            set { textureFiles[0] = value; }
        }
        public string TextureFile2
        {
            get { return textureFiles[1]; }
            set { textureFiles[1] = value; }
        }
        public string TextureFile3
        {
            get { return textureFiles[2]; }
            set { textureFiles[2] = value; }
        }
        public string TextureFile4
        {
            get { return textureFiles[3]; }
            set { textureFiles[3] = value; }
        }

        public bool Texture1Embeded
        {
            get { return texEmbeded[0]; }
            set { texEmbeded[0] = value; }
        }
        public bool Texture2Embeded
        {
            get { return texEmbeded[1]; }
            set { texEmbeded[1] = value; }
        }
        public bool Texture3Embeded
        {
            get { return texEmbeded[2]; }
            set { texEmbeded[2] = value; }
        }
        public bool Texture4Embeded
        {
            get { return texEmbeded[3]; }
            set { texEmbeded[3] = value; }
        }

        [Editor(typeof(TextureEditor), typeof(UITypeEditor))]
        [LocalizedDescription("PROP:MaterialTexture1")]
        public Texture Texture1
        {
            get { return textures[0]; }
            set
            {
                textures[0] = value;
                Changed();
            }
        }
        [Editor(typeof(TextureEditor), typeof(UITypeEditor))]
        [LocalizedDescription("PROP:MaterialTexture2")]
        public Texture Texture2
        {
            get { return textures[1]; }
            set
            {
                textures[1] = value;
                Changed();
            }
        }
        [Editor(typeof(TextureEditor), typeof(UITypeEditor))]
        [LocalizedDescription("PROP:MaterialTexture3")]
        public Texture Texture3
        {
            get { return textures[2]; }
            set
            {
                textures[2] = value;
                Changed();
            }
        }
        [Editor(typeof(TextureEditor), typeof(UITypeEditor))]
        [LocalizedDescription("PROP:MaterialTexture4")]
        public Texture Texture4
        {
            get { return textures[3]; }
            set
            {
                textures[3] = value;
                Changed();
            }
        }


        [Editor(typeof(Color4Editor), typeof(UITypeEditor))]
        [LocalizedDescription("PROP:MaterialAmbient")]
        public new Color4 Ambient
        {
            get { return mat.Ambient; }
            set
            {
                mat.Ambient = value;
                Changed();
            }
        }

        [Editor(typeof(Color4Editor), typeof(UITypeEditor))]
        [LocalizedDescription("PROP:MaterialDiffuse")]
        public new Color4 Diffuse
        {
            get { return mat.Diffuse; }
            set
            {
                mat.Diffuse = value;
                Changed();
            }
        }

        [Editor(typeof(Color4Editor), typeof(UITypeEditor))]
        [LocalizedDescription("PROP:MaterialSpecular")]
        public new Color4 Specular
        {
            get { return mat.Specular; }
            set
            {
                mat.Specular = value;
                Changed();
            }
        }

        [Editor(typeof(Color4Editor), typeof(UITypeEditor))]
        [LocalizedDescription("PROP:MaterialEmissive")]
        public new Color4 Emissive
        {
            get { return mat.Emissive; }
            set
            {
                mat.Emissive = value;
                Changed();
            }
        }
        [LocalizedDescription("PROP:MaterialPower")]
        public new float Power
        {
            get { return mat.Power; }
            set
            {
                mat.Power = value;
                Changed();
            }
        }
        public string EffectName
        {
            get { return effectName; }
            set { effectName = value; }
        }
        #endregion

        #region IComparable<MeshMaterial> 成员

        public int CompareTo(MeshMaterial other)
        {
            return this.GetHashCode().CompareTo(other.GetHashCode());
        }

        #endregion
    }
}
