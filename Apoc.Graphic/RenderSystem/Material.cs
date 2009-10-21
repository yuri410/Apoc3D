using System;
using System.Collections.Generic;
using System.Text;
using Apoc.MathLib;
using Apoc.Vfs;

namespace Apoc.RenderSystem
{
    public class MaterialBase
    {
        public const int MaxTexLayers = 4;

        static readonly string IsTransparentTag = "IsTransparent";
        static readonly string CullModeTag = "CullMode";
        static readonly string MaterialColorTag = "MaterialColor";

        public CullMode Cull
        {
            get;
            set;
        }
        public bool IsTransparent
        {
            get;
            set;
        }


        public Color4F Ambient
        {
            get;
            set;
        }

        public Color4F Diffuse
        {
            get;
            set;
        }

        public Color4F Specular
        {
            get;
            set;
        }

        public Color4F Emissive
        {
            get;
            set;
        }

        public float Power
        {
            get;
            set;
        }

        protected virtual void ReadData(BinaryDataReader data)
        {
            Cull = (CullMode)data.GetDataInt32(CullModeTag, 0);
            IsTransparent = data.GetDataBool(IsTransparentTag, false);

            ContentBinaryReader br = data.GetData(MaterialColorTag);

            Ambient = new Color4F(br.ReadSingle(), br.ReadSingle(), br.ReadSingle(), br.ReadSingle());
            Diffuse = new Color4F(br.ReadSingle(), br.ReadSingle(), br.ReadSingle(), br.ReadSingle());
            Specular = new Color4F(br.ReadSingle(), br.ReadSingle(), br.ReadSingle(), br.ReadSingle());
            Emissive = new Color4F(br.ReadSingle(), br.ReadSingle(), br.ReadSingle(), br.ReadSingle());

            br.Close();
        }
        protected virtual void WriteData(BinaryDataWriter data)
        {
            data.AddEntry(CullModeTag, (int)Cull);
            data.AddEntry(IsTransparentTag, IsTransparent);

            ContentBinaryWriter bw = data.AddEntry(MaterialColorTag);

            Color4F clr = Ambient;
            bw.Write(clr.Alpha);
            bw.Write(clr.Red);
            bw.Write(clr.Green);
            bw.Write(clr.Blue);

            clr = Diffuse;
            bw.Write(clr.Alpha);
            bw.Write(clr.Red);
            bw.Write(clr.Green);
            bw.Write(clr.Blue);

            clr = Specular;
            bw.Write(clr.Alpha);
            bw.Write(clr.Red);
            bw.Write(clr.Green);
            bw.Write(clr.Blue);

            clr = Emissive;
            bw.Write(clr.Alpha);
            bw.Write(clr.Red);
            bw.Write(clr.Green);
            bw.Write(clr.Blue);

            bw.Close();
        }
    }


    public abstract class Material<TexType> : MaterialBase, IDisposable
        where TexType : class
    {
        static readonly string IsTexEmbededTag = "IsEmbeded";
        static readonly string HasTextureTag = "HasTexture";
        static readonly string TextureTag = "Texture";
        static readonly string EffectTag = "Effect";

        //public Material mat;
        protected string[] textureFiles = new string[MaxTexLayers];

        protected bool[] texEmbeded = new bool[MaxTexLayers];

        protected TexType[] textures = new TexType[MaxTexLayers];

        string effectName;

        public Effect Effect
        {
            get;
            protected set;
        }
        //public int EffectIndex
        //{
        //    get;
        //    protected set;
        //}

        //public string EffectBatchName
        //{
        //    get 
        //    {
        //        if (Effect != null)
        //        {
        //            return effectName + EffectIndex.ToString();
        //        }
        //        return SceneManagerBase.DefaultEffectBatch;
        //    }
        //}

        //public string GetTextureFile(int idx)
        //{
        //    return textureFiles[idx];
        //}
        //public bool GetTextureEmbedded(int idx)
        //{
        //    return texEmbeded[idx];
        //}
        public TexType GetTexture(int idx)
        {
            return textures[idx];
        }
        public void SetTexture(int idx, TexType tex)
        {
            textures[idx] = tex;
        }
        public void SetEffect(Effect eff)
        {
            Effect = eff;
        }


        protected abstract TexType LoadTexture(ContentBinaryReader br, bool isEmbeded, int index);
        protected abstract void DestroyTexture(TexType tex);

        protected abstract void SaveTexture(ContentBinaryWriter bw, TexType tex, bool isEmbeded, int index);
        protected abstract Effect LoadEffect(string name);

        protected override void ReadData(BinaryDataReader data)
        {
            base.ReadData(data);

            ContentBinaryReader br;

            //br = data.GetData(MaterialColorTag);
            //br.ReadMaterial(out mat);
            //br.Close();


            br = data.GetData(EffectTag);
            effectName = br.ReadStringUnicode();
            //if (effectName.Length == 0)
            //    effectName = StandardEffectFactory.Name;
            Effect = LoadEffect(effectName);

            br.Close();

            br = data.GetData(IsTexEmbededTag);
            for (int i = 0; i < MaxTexLayers; i++)
            {
                texEmbeded[i] = br.ReadBoolean();
            }
            br.Close();

            bool[] hasTexture = new bool[4];
            br = data.GetData(HasTextureTag);
            for (int i = 0; i < MaxTexLayers; i++)
            {
                hasTexture[i] = br.ReadBoolean();
            }
            br.Close();


            for (int i = 0; i < MaxTexLayers; i++)
            {
                if (hasTexture[i])
                {
                    br = data.GetData(TextureTag + i.ToString());
                    textures[i] = LoadTexture(br, texEmbeded[i], i);
                    br.Close();
                }
            }


        }
        protected override void WriteData(BinaryDataWriter data)
        {
            base.WriteData(data);


            //data.AddEntry(MaterialFlagTag, (int)Flags);

            ContentBinaryWriter bw;

            bw = data.AddEntry(EffectTag);
            //if (Effect == null)
            //{
            bw.WriteStringUnicode(effectName);
            //}
            //else
            //{
            //    bw.WriteStringUnicode(Effect.Name);
            //}
            bw.Close();

            //bw = data.AddEntry(MaterialColorTag);
            //bw.Write(ref mat);
            //bw.Close();


            bw = data.AddEntry(IsTexEmbededTag);
            for (int i = 0; i < MaxTexLayers; i++)
            {
                bw.Write(texEmbeded[i]);
            }
            bw.Close();

            bw = data.AddEntry(HasTextureTag);
            for (int i = 0; i < MaxTexLayers; i++)
            {
                bw.Write(textures[i] != null || !string.IsNullOrEmpty(textureFiles[i]));
            }
            bw.Close();

            for (int i = 0; i < MaxTexLayers; i++)
            {
                if (textures[i] != null || !string.IsNullOrEmpty(textureFiles[i]))
                {
                    bw = data.AddEntry(TextureTag + i.ToString());
                    SaveTexture(bw, textures[i], texEmbeded[i], i);
                    bw.Close();
                }
            }
        }


        #region IDisposable 成员

        public bool Disposed
        {
            get;
            private set;
        }

        public void DisposeRef()
        {
            if (!Disposed)
            {
                Dispose(false);

                Disposed = true;
            }
            else
                throw new ObjectDisposedException(ToString());
        }
        protected virtual void Dispose(bool disposing)
        {
            if (disposing)
            {
                if (Effect != null)
                {
                    Effect.Dispose();
                }
                if (textures != null)
                {
                    for (int i = 0; i < 4; i++)
                    {
                        if (textures[i] != null)
                        {
                            DestroyTexture(textures[i]);
                            //TextureManager.Instance.DestoryInstance(textures[i]);
                            textures[i] = null;
                        }
                    }
                }
            }
            Effect = null;
            textures = null;
        }
        public void Dispose()
        {
            if (!Disposed)
            {
                Dispose(true);
               
                Disposed = true;
            }
            else
                throw new ObjectDisposedException(ToString());
        }



        ~Material()
        {
            if (!Disposed) 
            {
                Dispose(false);
            }
        }

        #endregion
    }

    public class Material : Material<Texture>
    {
        public static Material DefaultMaterial
        {
            get;
            private set;
        }

        static Material()
        {
            Color4F clr;
            clr.Alpha = 1;
            clr.Blue = 1;
            clr.Green = 1;
            clr.Red = 1;

            DefaultMaterial = new Material();
            DefaultMaterial.Ambient = clr;
            DefaultMaterial.Diffuse = clr;
            DefaultMaterial.Power = 0;
            clr.Alpha = 0;
            clr.Red = 0;
            clr.Green = 0;
            clr.Blue = 0;
            DefaultMaterial.Emissive = clr;
            DefaultMaterial.Specular = clr;
        }


        Texture LoadTexture(string fileName)
        {
            fileName = fileName.Trim();
            if (!string.IsNullOrEmpty(fileName))
            {
                FileLocation fl = FileSystem.Instance.TryLocate(fileName, DefaultLocateRules.Instance.TextureLocateRule);
                if (fl != null)
                {
                    ImageLoader image = ImageManager.Instance.CreateInstance(fl);

                    return TextureManager.Instance.CreateInstance(image);// Texture.FromStream(device, fl.GetStream, Usage.None, Pool.Managed);
                }
                else
                {
                    R3DConsole.Instance.Write("Texture: " + fileName + "is not found.", ConsoleMessageType.Warning);
                    return null;
                }
            }
            return null;
        }
        protected override Texture LoadTexture(ContentBinaryReader br, bool isEmbeded, int index)
        {
            if (isEmbeded)
            {
                string ext = br.ReadStringUnicode();

                VirtualStream vs = new VirtualStream(br.BaseStream, br.BaseStream.Position, br.BaseStream.Length - br.BaseStream.Position);

                StreamedLocation sl = new StreamedLocation(vs);

                ImageLoader imgLdr = ImageManager.Instance.CreateInstance(sl, ext);
                Image image = imgLdr.Load();
                return TextureManager.Instance.CreateInstance(image);

                //return Texture.FromStream(device, br.BaseStream, Usage.None, Pool.Managed);
            }
            else
            {
                return LoadTexture(br.ReadStringUnicode());
            }
        }

        protected override void DestroyTexture(Texture tex)
        {
            tex.Dispose();
        }

        protected override void SaveTexture(ContentBinaryWriter bw, Texture tex, bool isEmbeded, int index)
        {
            throw new NotImplementedException();
        }

        protected override Effect LoadEffect(string name)
        {
            throw new NotImplementedException();

            //return EffectManager.Instance.GetModelEffect(name);
        }

        public static Material FromBinary(BinaryDataReader data)
        {
            Material res = new Material();

            res.ReadData(data);

            return res;
        }
        public static BinaryDataWriter ToBinary(Material mat)
        {
            BinaryDataWriter data = new BinaryDataWriter();
            mat.WriteData(data);

            return data;
        }
    }
}
