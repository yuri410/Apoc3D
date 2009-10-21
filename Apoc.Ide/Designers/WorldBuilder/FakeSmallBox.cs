using System;
using System.Collections.Generic;
using System.IO;
using System.Text;
using SlimDX;
using SlimDX.Direct3D9;
using VirtualBicycle;
using VirtualBicycle.Graphics;
using VirtualBicycle.IO;
using VirtualBicycle.Scene;
using VirtualBicycle.MathLib;

namespace VirtualBicycle.Ide.Designers.WorldBuilder
{
    [TestOnly()]
    class FakeSmallBox : StaticObject
    {
        static readonly string typeTag = "SmallBox";
        static readonly string OrientationTag = "Orientation";
        static readonly string PositionTag = "Position";
        static readonly string OffsetTag = "Offset";

        public FakeSmallBox(Device device) 
        {
            FileLocation fl = FileSystem.Instance.Locate(Path.Combine(VirtualBicycle.IO.Paths.Models, "box1.mesh"), FileLocateRules.Default);
            ModelL0 = ModelManager.Instance.CreateInstance(device, fl);

            BoundingSphere.Radius = MathEx.Root3 * 0.5f; 
        }

        public override bool IsSerializable
        {
            get { return true; }
        }

        public override string TypeTag
        {
            get
            {
                return typeTag;
            }
        }
        public override void Serialize(BinaryDataWriter data)
        {
            ContentBinaryWriter bw = data.AddEntry(PositionTag);

            Vector3 pos = Position;
            bw.Write(pos.X);
            bw.Write(pos.Y);
            bw.Write(pos.Z);
            bw.Close();


            bw = data.AddEntry(OrientationTag);
            Quaternion quat = Orientation;
            bw.Write(quat.W);
            bw.Write(quat.X);
            bw.Write(quat.Y);
            bw.Write(quat.Z);
            bw.Close();


            bw = data.AddEntry(OffsetTag);
            bw.Write(OffsetX);
            bw.Write(OffsetY);
            bw.Write(OffsetZ);
            bw.Close();
        }
    }
}
