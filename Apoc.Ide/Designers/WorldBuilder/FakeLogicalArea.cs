using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Text;
using SlimDX;
using SlimDX.Direct3D9;
using VirtualBicycle.Graphics;
using VirtualBicycle.Graphics.Effects;
using VirtualBicycle.IO;
using VirtualBicycle.Scene;

namespace VirtualBicycle.Ide.Designers.WorldBuilder
{
    public class FakeLogicalArea : SceneObject, IPositionedObject
    {
        #region Fields
        static readonly string TypeNameTag = "TypeName";
        static readonly string PositionTag = "Position";
        static readonly string RadiusTag = "Radius";

        private string typeName;
       
        public string TypeName
        {
            get { return typeName; }
            set { typeName = value; }
        }

        private float radius;
        public float Radius
        {
            get { return radius; }
            set
            {
                radius = value;
                BoundingSphere.Radius = value;
            }
        }

        private Vector3 position;
        public Vector3 Position
        {
            get { return position; }
            set
            {
                position = value;
                Transformation = Matrix.Translation(value);
                BoundingSphere.Center = value;
            }
        }
        #endregion

        #region Constructor
        public FakeLogicalArea(float radius, Vector3 pos, string name)
            : base(false)
        {
            this.Radius = radius;
            this.Position = pos;
            this.typeName = name;
        }
        #endregion

        #region SceneObject 序列化

        public override void Serialize(BinaryDataWriter data)
        {
            ContentBinaryWriter bw = data.AddEntry(PositionTag);
            Vector3 pos = Position;
            bw.Write(pos.X);
            bw.Write(pos.Y);
            bw.Write(pos.Z);
            bw.Close();

            data.AddEntry(RadiusTag, Radius);
            bw.Close();

            bw = data.AddEntry(TypeNameTag);
            bw.WriteStringUnicode(TypeName);
            bw.Close();
        }

        [Browsable(false)]
        public override bool IsSerializable
        {
            get { return true; }
        }

        public override string TypeTag
        {
            get
            {
                return "LogicArea";
            }
        }
        #endregion

        RenderOperation[] opBuffer;
        public override RenderOperation[] GetRenderOperation()
        {
            if (opBuffer == null) 
            {
                GeomentryData gd = GraphicsDevice.Instance.BallGeoData;

                opBuffer = new RenderOperation[1];
                opBuffer[0].Geomentry = gd;
                opBuffer[0].Material = new MeshMaterial(GraphicsDevice.Instance.Device);

                Material mat = MeshMaterial.DefaultMatColor;

                Color4 color = mat.Ambient;
                color.Alpha = 0.5f;
                mat.Ambient = color;

                color = mat.Ambient;
                color.Alpha = 0.5f; 
                mat.Diffuse = color;

                color = mat.Ambient;
                color.Alpha = 0.5f;
                mat.Emissive = color;

                color = mat.Ambient;
                color.Alpha = 0.5f; 
                mat.Specular = color;

                opBuffer[0].Material.D3DMaterial = mat;
                
                opBuffer[0].Material.SetEffect(EffectManager.Instance.GetModelEffect(StandardEffectFactory.Name));
            }
            opBuffer[0].Transformation = Matrix.Scaling(radius, radius, radius);
            return opBuffer;
        }

        public override void Update(float dt)
        {

        }

        #region IPositionedObject 成员


        public void UpdateTransform()
        {

        }

        #endregion

        #region IPositionedObject 成员

        [Browsable(false)]
        public bool EditorMovable
        {
            get { return true; }
        }

        #endregion
    }
}
