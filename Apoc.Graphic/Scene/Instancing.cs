using System;
using System.Collections.Generic;
using System.Text;
using Apoc.Collections;
using Apoc.Graphics;
using Apoc.MathLib;

namespace Apoc.Scene
{ 
    /// <summary>
    ///  表示几何体实例化渲染所用的顶点缓冲中的顶点格式
    ///  这个格式包含网格的世界变换矩阵
    /// </summary>
    public struct InstanceData 
    {
        /// <summary>
        ///  矩阵的第一行
        /// </summary>
        public Vector3 Row1;

        /// <summary>
        ///  矩阵的第二行
        /// </summary>
        public Vector3 Row2;

        /// <summary>
        ///  矩阵的第三行
        /// </summary>
        public Vector3 Row3;

        /// <summary>
        ///  矩阵的第四行
        /// </summary>
        public Vector3 Row4;

        /// <summary>
        ///  获取这个结构体的大小
        /// </summary>
        public static int Size 
        {
            get { return Vector3.SizeInBytes * 4; }
        }

    }

    /// <summary>
    ///  几何体实例化渲染器
    /// </summary>
    public  class Instancing:UnmanagedResource
    {
        public const int MaxInstances = 25;

        Device device;

        VertexBuffer instanceData;

        Dictionary<VertexDeclaration, VertexDeclaration> declBuffer;
        VertexElement[] idElements;

        /// <summary>
        ///  创建新的几何体实例化渲染器
        /// </summary>
        /// <param name="device"></param>
        public Instancing(Device device)
        {
            this.device = device;

            this.declBuffer = new Dictionary<VertexDeclaration, VertexDeclaration>();

            LoadUnmanagedResources();

            this.idElements = new VertexElement[4];
            this.idElements[0] = new VertexElement(1, 0, DeclarationType.Float3, DeclarationMethod.Default, DeclarationUsage.TextureCoordinate, 8);
            this.idElements[1] = new VertexElement(1, (short)Vector3.SizeInBytes, DeclarationType.Float3, DeclarationMethod.Default, DeclarationUsage.TextureCoordinate, 9);
            this.idElements[2] = new VertexElement(1, (short)(Vector3.SizeInBytes * 2), DeclarationType.Float3, DeclarationMethod.Default, DeclarationUsage.TextureCoordinate, 10);
            this.idElements[3] = new VertexElement(1, (short)(Vector3.SizeInBytes * 3), DeclarationType.Float3, DeclarationMethod.Default, DeclarationUsage.TextureCoordinate, 11);
        }

        /// <summary>
        ///  为实例化RenderOperation做准备
        /// </summary>
        /// <param name="opList">用于实例化的RenderOperation组成的列表</param>
        /// <param name="index">opList中开始RenderOperation的索引</param>
        /// <returns>一个<see cref="System.Int32"/>，表示实际处理的RenderOperation</returns>
        /// <remarks>
        ///  这个方法将opList的RenderOperation中的变换矩阵作为实例化的数据存入一个顶点缓冲，
        ///  在实例化渲染的时候使用Stream混合将网格的顶点和变换矩阵混合
        /// </remarks>
        public unsafe int Setup(FastList<RenderOperation> opList, int index)
        {
            InstanceData* dst = (InstanceData*)instanceData.Lock(0, 0, LockFlags.None).DataPointer;

            int count = 0;
            for (int i = index; i < opList.Count && count < MaxInstances; i++)
            {
                dst->Row1 = new Vector3(opList[i].Transformation.M11, opList[i].Transformation.M12, opList[i].Transformation.M13);
                dst->Row2 = new Vector3(opList[i].Transformation.M21, opList[i].Transformation.M22, opList[i].Transformation.M23);
                dst->Row3 = new Vector3(opList[i].Transformation.M31, opList[i].Transformation.M32, opList[i].Transformation.M33);
                dst->Row4 = new Vector3(opList[i].Transformation.M41, opList[i].Transformation.M42, opList[i].Transformation.M43);

                dst++;
                count++;
            }

            instanceData.Unlock();

            device.SetStreamSource(1, instanceData, 0, InstanceData.Size);
            device.SetStreamSourceFrequency(1, 1, StreamSource.InstanceData);

            return count;
        }


        /// <summary>
        ///  获取将网格的顶点附上实例化数据的顶点格式的声明
        /// </summary>
        /// <param name="decl">原始顶点格式的声明</param>
        /// <returns>复合顶点格式的声明</returns>
        public VertexDeclaration GetInstancingDecl(VertexDeclaration decl)
        {
            VertexDeclaration idecl;
            if (!declBuffer.TryGetValue(decl, out idecl)) 
            {
                VertexElement[] elems1 = decl.Elements;
                VertexElement[] elems2 = new VertexElement[elems1.Length + idElements.Length];

                Array.Copy(elems1, elems2, elems1.Length - 1);
                Array.Copy(idElements, 0, elems2, elems1.Length - 1, idElements.Length);
                elems2[elems2.Length - 1] = VertexElement.VertexDeclarationEnd;
                
                idecl = new VertexDeclaration(device, elems2);

                declBuffer.Add(decl, idecl);
            }
            return idecl;
        }

        protected override void loadUnmanagedResources()
        {
            this.instanceData = new VertexBuffer(device, MaxInstances * InstanceData.Size, Usage.Dynamic, VertexFormat.None, Pool.Default);
        }

        protected override void unloadUnmanagedResources()
        {
            this.instanceData.Dispose();
            this.instanceData = null;
        }
    }
}
