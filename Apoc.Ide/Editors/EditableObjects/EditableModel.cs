using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Drawing.Design;
using System.IO;
using System.Text;
using SlimDX;
using SlimDX.Direct3D9;
using VirtualBicycle.Ide.Converters;
using VirtualBicycle.Ide.Designers;
using VirtualBicycle.Design;
using VirtualBicycle.Graphics;
using VirtualBicycle.Graphics.Animation;
using VirtualBicycle.IO;

namespace VirtualBicycle.Ide.Editors.EditableObjects
{
    [TypeConverter(typeof(ExpandableObjectConverter))]
    [Editor(typeof(ModelEditor), typeof(UITypeEditor))]
    //[Editor(typeof(GameModelArrayEditor), typeof (UITypeEditor))]
    public class EditableModel : ModelBase<EditableMesh>, IDisposable
    {
        public EditableModel()
            : base(GraphicsDevice.Instance.Device)
        { }


        [Editor(typeof(MeshArrayEditor), typeof(UITypeEditor))]
        [TypeConverter(typeof(ArrayConverter<EmbededMeshEditor, EditableMesh>))]
        public new EditableMesh[] Entities
        {
            get { return entities; }
            set
            {
                entities = value;
            }
        }


        public static unsafe EditableModel FromFile(ResourceLocation rl)
        {
            ContentBinaryReader br = new ContentBinaryReader(rl);
            if (br.ReadInt32() == (int)Model.MdlId)
            {
                BinaryDataReader data = br.ReadBinaryData();
                EditableModel mdl = FromBinary(data);
                br.Close();
                return mdl;
            }
            br.Close();
            throw new InvalidDataException();
        }
        public static void ToFile(EditableModel mdl, ResourceLocation dest)
        {
            ContentBinaryWriter bw = new ContentBinaryWriter(dest);

            bw.Write(Model.MdlId);
            BinaryDataWriter mdlData = ToBinary(mdl);
            bw.Write(mdlData);
            mdlData.Dispose();

            bw.Close();
        }

        public static EditableModel FromBinary(BinaryDataReader data)
        {
            EditableModel mdl = new EditableModel();

            mdl.ReadData(data);

         
            return mdl;
        }
        public static unsafe BinaryDataWriter ToBinary(EditableModel mdl)
        {
            BinaryDataWriter data = new BinaryDataWriter();
            mdl.WriteData(data);
            return data;
        }



        public void Render()
        {
            if (entities != null && TransformAnim != null)
            {
                device.SetRenderState(RenderState.NormalizeNormals, true);
                device.VertexShader = null;
                device.PixelShader = null;

                if (TransformAnim != null)
                {
                    TransformAnim.Update(0.025f);
                }
                if (SkinAnim != null)
                {
                    SkinAnim.Update(0.025f);
                }

                for (int i = 0; i < entities.Length; i++)
                {
                    device.SetTransform(TransformState.World, TransformAnim.GetTransform(i));

                    entities[i].Render();
                }

            }
        }

        public void Render(Matrix trans)
        {
            if (entities != null && TransformAnim != null)
            {
                device.SetRenderState(RenderState.NormalizeNormals, true);
                device.VertexShader = null;
                device.PixelShader = null;

                if (TransformAnim != null)
                {
                    TransformAnim.Update(0.025f);
                }
                if (SkinAnim != null)
                {
                    SkinAnim.Update(0.025f);
                }
                for (int i = 0; i < entities.Length; i++)
                {
                    device.SetTransform(TransformState.World, TransformAnim.GetTransform(i) * trans);
 
                    entities[i].Render();
                }
            }
        }

        public void SetSkinAnimInst(SkinAnimationInstance inst)
        {
            base.SkinAnim = inst;
        }
        public void SetTransformAnimInst(TransformAnimationInstance inst)
        {
            base.TransformAnim = inst;
        }


        protected override EditableMesh LoadMesh(BinaryDataReader data)
        {
            EditableMesh mesh = new EditableMesh();
            mesh.Load(data);
            return mesh;
        }
        protected override BinaryDataWriter SaveMesh(EditableMesh mesh)
        {
            return mesh.Save();
        }

        public void ImportEntityFromXml(string file)
        {
            Xml2ModelConverter conv = new Xml2ModelConverter();
            System.IO.MemoryStream ms = new System.IO.MemoryStream(65536);
            conv.Convert(new FileLocation(file), new StreamedLocation(new VirtualStream(ms, 0)));

            ms.Position = 0;

            EditableModel data = EditableModel.FromFile(new StreamedLocation(ms));

            //if (animation is NoAnimation && data.animation is NoAnimation)
            //{
            EditableMesh[] newEnt = new EditableMesh[data.entities.Length + entities.Length];
            EditableMesh[] addEnt = data.entities;
            Array.Copy(entities, newEnt, entities.Length);
            Array.Copy(addEnt, 0, newEnt, entities.Length, addEnt.Length);



            //NoAnimation na = (NoAnimation)animation;

            //Matrix[] newTrans = new Matrix[newEnt.Length];
            //Matrix[] addTrans = ((NoAnimation)data.animation).Transforms;

            //Array.Copy(na.Transforms, newTrans, na.Transforms.Length);
            //Array.Copy(addTrans, 0, newTrans, na.Transforms.Length, addTrans.Length);

            entities = newEnt;
            //na.Transforms = newTrans;
            //}
        }


        //#region IDisposable 成员

        //public void Dispose()
        //{
        //    if (entities != null)
        //    {
        //        for (int i = 0; i < entities.Length; i++)
        //        {
        //            entities[i].Dispose();
        //        }
        //        entities = null;
        //    }
        //    //animation = null;
        //    //StateChanged = null;
        //}

        //#endregion

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
    }
}
