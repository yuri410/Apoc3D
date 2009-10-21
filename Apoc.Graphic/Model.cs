using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.IO;
using System.Text;
using SlimDX.Direct3D9;
using Apoc.Graphics.Animation;
using Apoc.IO;

namespace Apoc.Graphics
{
    /// <summary>
    ///  定义3D模型提供基础结构
    /// </summary>
    /// <typeparam name="MeshType"></typeparam>
    public abstract class ModelBase<MeshType> : Apoc.Core.Resource
        where MeshType : class
    {
        public const int MdlId = 0;

        protected readonly string EntityCountTag = "EntityCount";

        protected readonly string EntityPrefix = "Ent";
        protected readonly string AnimationTag = "Animation";
        protected readonly string AnimationFlagTag = "AnimationFlag";

        protected MeshType[] entities;
        //protected Animation animation;
        protected Device device;

        TransformAnimationInstance transAnim;
        SkinAnimationInstance skinAnim;

        public Dictionary<string, TapeHelper> TapeHelpers
        {
            get;
            set;
        }
        
        public TransformAnimationInstance TransformAnim
        {
            get { return transAnim; }
            protected set { transAnim = value; }
        }
        public SkinAnimationInstance SkinAnim
        {
            get { return skinAnim; }
            protected set { skinAnim = value; }
        }

        public ResourceLocation DataSource
        {
            get;
            private set;
        }

        protected ModelBase(Device dev, ResourceLocation rl)
            : base(ModelManager.Instance, rl.Name)
        {
            device = dev;
            DataSource = rl;
        }

        protected ModelBase(Device rs)
        {
            device = rs;
        }
        protected ModelBase(Device rs, ModelBase<MeshType> refMdl)
            : base(ModelManager.Instance, refMdl)
        {
        }
        protected ModelBase(Device rs, string name)
            : base(ModelManager.Instance, name)
        {
            device = rs;
        }

        protected ModelBase(Device rs, string name, bool allowdl)
            : base(ModelManager.Instance, name, allowdl)
        {
            device = rs;
        }

        [Browsable(false)]
        public Device Device
        {
            get { return device; }
        }

        //public Animation ModelAnimation
        //{
        //    get { return animation; }
        //    set { animation = value; }
        //}

        //[Browsable(false)]
        public MeshType[] Entities
        {
            get
            {
                Use();
                return entities;
            }
            set { entities = value; }
        }

        protected abstract MeshType LoadMesh(BinaryDataReader data);
        protected abstract BinaryDataWriter SaveMesh(MeshType mesh);

        protected void ReadData(BinaryDataReader data)
        {
            int entCount = data.GetDataInt32(EntityCountTag);
            entities = new MeshType[entCount];

            ContentBinaryReader br;
            for (int i = 0; i < entCount; i++)
            {
                br = data.GetData(EntityPrefix + i.ToString());
                BinaryDataReader meshData = br.ReadBinaryData();
                entities[i] = LoadMesh(meshData);
                meshData.Close();
                br.Close();
            }

            ModelAnimationFlags flags = (ModelAnimationFlags)data.GetDataInt32(AnimationFlagTag);
            BinaryDataReader animData;

            if ((flags & ModelAnimationFlags.EntityTransform) == ModelAnimationFlags.EntityTransform)
            {
                br = data.GetData(AnimationTag + ModelAnimationFlags.EntityTransform.ToString());
                TransformAnimation transAnimDat = new TransformAnimation(entities.Length);

                animData = br.ReadBinaryData();
                transAnimDat.Load(animData);
                animData.Close();

                br.Close();

                transAnim = new TransformAnimationInstance(transAnimDat);
            }
            if ((flags & ModelAnimationFlags.Skin) == ModelAnimationFlags.Skin)
            {

            }

            //br = data.TryGetData(LodMeshTag);
            //if (br != null)
            //{
            //    BinaryDataReader meshData = br.ReadBinaryData();
            //    lodMesh = LoadMesh(meshData);

            //    meshData.Close();
            //    br.Close();
            //}

        }
        protected void WriteData(BinaryDataWriter data)
        {
            Use();
            data.AddEntry(EntityCountTag, entities.Length);

            ContentBinaryWriter bw;
            for (int i = 0; i < entities.Length; i++)
            {
                bw = data.AddEntry(EntityPrefix + i.ToString());

                BinaryDataWriter meshData = SaveMesh(entities[i]);
                bw.Write(meshData);
                meshData.Dispose();
                bw.Close();
            }

            ModelAnimationFlags flags = ModelAnimationFlags.EntityTransform;

            if (skinAnim != null)
            {
                flags |= ModelAnimationFlags.Skin;
            }


            data.AddEntry(AnimationFlagTag, (int)flags);

            BinaryDataWriter animData;

            if ((flags & ModelAnimationFlags.EntityTransform) == ModelAnimationFlags.EntityTransform)
            {
                bw = data.AddEntry(AnimationTag + ModelAnimationFlags.EntityTransform.ToString());
                animData = transAnim.Data.Save();
                bw.Write(animData);
                animData.Dispose();
            }

            if ((flags & ModelAnimationFlags.Skin) == ModelAnimationFlags.Skin)
            {
                bw = data.AddEntry(AnimationTag + ModelAnimationFlags.Skin.ToString());
                animData = skinAnim.Data.Save();
                bw.Write(animData);
                animData.Dispose();
            }
        }


        public override int GetSize()
        {
            if (DataSource != null)
            {
                return DataSource.Size;
            }
            return 0;
        }
        protected override void load()
        {
            if (DataSource != null)
            {
                ContentBinaryReader br = new ContentBinaryReader(DataSource);
                if (br.ReadInt32() == MdlId)
                {
                    BinaryDataReader data = br.ReadBinaryData();

                    ReadData(data);

                    data.Close();
                }

                br.Close();
            }
        }

        public override void ReadCacheData(Stream stream)
        {
            ReadData(new BinaryDataReader(new VirtualStream(stream)));
        }
        public override void WriteCacheData(Stream stream)
        {
            BinaryDataWriter data = new BinaryDataWriter();
            WriteData(data);
            data.Save(new VirtualStream(stream));
        }
    }

    /// <summary>
    ///  表示3D模型
    /// </summary>
    public class Model : ModelBase<GameMesh>, IRenderable, IDisposable, IUpdatable
    {
        /// <summary>
        ///  已缓存的RenderOperation
        /// </summary>
        RenderOperation[] opBuffer;
        /// <summary>
        ///  renderOpEntId[i] 表示索引为i的renderOperation的Entity索引
        /// </summary>
        int[] renderOpEntId;

        public Model(Device device, ResourceLocation rl)
            : base(device, rl)
        {
            
        }

        //public Model(RenderSystem rs, Animation anim, Mesh[] meshes)
        //    : base(rs)
        //{
        //    this.animation = animation;
        //    this.entities = meshes;
        //}
        public Model(Device device, string name)
            : base(device, name, false)
        {
        }

        public Model(Device device, GameMesh[] entities)
            : base(device)
        {
            this.entities = entities;

            TransformAnimation animData = new TransformAnimation(entities.Length);
            this.TransformAnim = new TransformAnimationInstance(animData);
        }
        public Model(Device device, int entityCount)
            : base(device)
        {
            this.entities = new GameMesh[entityCount];

            TransformAnimation animData = new TransformAnimation(entityCount);
            this.TransformAnim = new TransformAnimationInstance(animData);
        }

        protected override void unload()
        {
            if (entities != null)
            {
                for (int i = 0; i < entities.Length; i++)
                {
                    if (!entities[i].Disposed)
                    {
                        entities[i].Dispose();
                    }
                    entities[i] = null;
                }
            }
        }

        /// <summary>
        ///  引用mdl的数据，但重新创建动画实例
        /// </summary>
        /// <param name="rs"></param>
        /// <param name="mdl"></param>
        public Model(Device dev, Model mdl)
            : base(dev, mdl)
        {
            if (mdl.entities != null)
            {
                this.entities = new GameMesh[mdl.entities.Length];
                for (int i = 0; i < entities.Length; i++)
                {
                    this.entities[i] = new GameMesh(dev, mdl.entities[i]);
                }
            }

            base.TransformAnim = new TransformAnimationInstance(mdl.TransformAnim.Data);

            if (mdl.SkinAnim != null)
            {
                base.SkinAnim = new SkinAnimationInstance(mdl.SkinAnim.Data);
            }
            //isResourceEntity = false;
        }

        private Model(Device dev)
            : base(dev)
        {
            //isResourceEntity = true;
        }

        protected override GameMesh LoadMesh(BinaryDataReader data)
        {
            MeshData md = new MeshData(device);
            md.Load(data);
            return new GameMesh(device, md);
        }
        protected override BinaryDataWriter SaveMesh(GameMesh mesh)
        {
            MeshData md = new MeshData(mesh);
            return md.Save();
        }

        public static BinaryDataWriter ToBinary(Model mdl)
        {
            BinaryDataWriter data = new BinaryDataWriter();
            mdl.WriteData(data);
            return data;
        }
        public static void ToFile(Model mdl, string file)
        {
            FileStream fs = new FileStream(file, FileMode.OpenOrCreate, FileAccess.Write);
            fs.SetLength(0);
            ContentBinaryWriter bw = new ContentBinaryWriter(fs);

            bw.Write(Model.MdlId);
            BinaryDataWriter mdlData = ToBinary(mdl);
            bw.Write(mdlData);
            mdlData.Dispose();

            bw.Close();
        }
        public static void ToStream(Model mdl, Stream stm)
        {
            ContentBinaryWriter bw = new ContentBinaryWriter(stm, Encoding.Default);

            bw.Write(MdlId);

            BinaryDataWriter mdlData = ToBinary(mdl);
            bw.Write(mdlData);
            mdlData.Dispose();

            bw.Close();
        }

        #region IRenderable 成员

        public RenderOperation[] GetRenderOperation()
        {
            Use();
            if (opBuffer == null)
            {
                RenderOperation[][] entOps = new RenderOperation[entities.Length][];

                int opCount = 0;
                for (int i = 0; i < entities.Length; i++)
                {
                    entOps[i] = entities[i].GetRenderOperation();
                    //for (int j = 0; j < entOps[i].Length; j++)
                    //{
                    //    animation.GetTransform(i, out entOps[i][j].Transformation);
                    //}
                    opCount += entOps[i].Length;
                }

                int dstIdx = 0;
                //gmBuffer = new GeomentryData[opCount];
                opBuffer = new RenderOperation[opCount];
                renderOpEntId = new int[opCount];

                for (int i = 0; i < entities.Length; i++)
                {
                    Array.Copy(entOps[i], 0, opBuffer, dstIdx, entOps[i].Length);

                    for (int j = 0; j < entOps[i].Length; j++)
                    {
                        renderOpEntId[dstIdx + j] = i;
                        //opBuffer[dstIdx + j].Geomentry = entOps[i][j];
                        //animation.GetTransform(i, out  opBuffer[dstIdx + j].Transformation);
                        opBuffer[i].Transformation = TransformAnim.GetTransform(renderOpEntId[i]);
                    }

                    dstIdx += entOps[i].Length;
                }
            }
            else
            {
                for (int i = 0; i < opBuffer.Length; i++)
                {
                    opBuffer[i].Transformation = TransformAnim.GetTransform(renderOpEntId[i]);
                    //animation.GetTransform(renderOpEntId[i], out opBuffer[i].Transformation);
                }
                //animation.Animate();
            }
            return opBuffer;
        }


        #endregion

        #region IDisposable 成员

        protected override void Dispose(bool disposing)
        {
            base.Dispose(disposing);

            if (disposing)
            {
                for (int i = 0; i < entities.Length; i++)
                {
                    if (!entities[i].Disposed)
                    {
                        entities[i].Dispose();
                    }
                    entities[i] = null;
                }
            }
            entities = null;
        }

        #endregion

        #region IUpdatable 成员

        public void Update(float dt)
        {
            if (TransformAnim != null)
            {
                TransformAnim.Update(dt);
            }
            if (SkinAnim != null)
            {
                SkinAnim.Update(dt);
            }
            if (entities != null)
            {
                for (int i = 0; i < entities.Length; i++)
                {
                    entities[i].Update(dt);
                }
            }
        }

        #endregion
    }
}
