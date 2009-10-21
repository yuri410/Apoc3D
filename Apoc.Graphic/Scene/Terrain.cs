using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Diagnostics;
using System.Drawing;
using System.IO;
using System.Text;
using Apoc.Collections;
using Apoc.CollisionModel;
using Apoc.CollisionModel.Shapes;
using Apoc.Config;
using Apoc.Graphics;
using Apoc.Graphics.Effects;
using Apoc.Vfs;
using Apoc.MathLib;
using Apoc.Physics;
using Apoc.Physics.Dynamics;

namespace Apoc.Scene
{
    /// <summary>
    ///  表示地形。实现地形渲染。
    /// </summary>
    /// <remarks>
    /// DisplacementMap和4级LOD
    /// </remarks>
    public unsafe class Terrain : SceneObject
    {
        protected class BlockMesh
        {
            List<TreeFace> faces;

            BBTreeNode bbTree;

            public BlockMesh(int x, int y, float* data, float cellUnit, float heightScale)
            {
                faces = new List<TreeFace>(BlockEdgeLen * BlockEdgeLen * 2);

                int triIndex = 0;
                for (int i = 0; i < BlockEdgeLen; i++)
                {
                    for (int j = 0; j < BlockEdgeLen; j++)
                    {
                        int cx = x + j;
                        int cy = y + i;

                        int index1 = cy * TerrainSize + cx;
                        int index2 = (cy + 1) * TerrainSize + cx;
                        int index3 = (cy + 1) * TerrainSize + cx + 1;
                        int index4 = cy * TerrainSize + cx + 1;


                        TreeFace face1 = new TreeFace(triIndex++,
                            new Vector3(cx * cellUnit, data[index1] * heightScale, cy * cellUnit),
                            new Vector3(cx * cellUnit, data[index2] * heightScale, (cy + 1) * cellUnit),
                            new Vector3((cx + 1) * cellUnit, data[index3] * heightScale, (cy + 1) * cellUnit));

                        TreeFace face2 = new TreeFace(triIndex++,
                            new Vector3(cx * cellUnit, data[index1] * heightScale, cy * cellUnit),
                            new Vector3((cx + 1) * cellUnit, data[index3] * heightScale, (cy + 1) * cellUnit),
                            new Vector3((cx + 1) * cellUnit, data[index4] * heightScale, cy * cellUnit));

                        faces.Add(face1);
                        faces.Add(face2);
                    }
                }


                bbTree = new BBTreeNode(faces);
            }

            public bool Intersects(Vector3 start, Vector3 end, out Vector3 result)
            {
                LineSegment line = new LineSegment(start, end);

                List<DirectDetectData> results = new List<DirectDetectData>();
                bbTree.IntersectDF(ref line, results);

                if (results.Count > 0)
                {
                    result = results[0].Position;
                    return true;
                }
                result = new Vector3();
                return false;
            }

            public void Update(int x, int y, int* data, float cellUnit, float heightScale)
            {
                Update(x, y, new Rectangle(0, 0, BlockEdgeLen, BlockEdgeLen), data, cellUnit, heightScale);
            }

            public void Update(int x, int y, Rectangle area, int* data, float cellUnit, float heightScale)
            {
                int triIndex = 0;

                for (int i = area.Top; i < area.Bottom; i++)
                {
                    for (int j = area.Left; j < area.Right; j++)
                    {
                        int cx = x + j;
                        int cy = y + i;

                        int index1 = cy * TerrainSize + cx;
                        int index2 = (cy + 1) * TerrainSize + cx;
                        int index3 = (cy + 1) * TerrainSize + cx + 1;
                        int index4 = cy * TerrainSize + cx + 1;

                        faces[triIndex++].Update(
                           new Vector3(cx * cellUnit, data[index1] * heightScale, cy * cellUnit),
                           new Vector3(cx * cellUnit, data[index2] * heightScale, (cy + 1) * cellUnit),
                           new Vector3((cx + 1) * cellUnit, data[index3] * heightScale, (cy + 1) * cellUnit));

                        faces[triIndex++].Update(
                            new Vector3(cx * cellUnit, data[index1] * heightScale, cy * cellUnit),
                            new Vector3((cx + 1) * cellUnit, data[index3] * heightScale, (cy + 1) * cellUnit),
                            new Vector3((cx + 1) * cellUnit, data[index4] * heightScale, cy * cellUnit));

                    }
                }

                bbTree.Update();
            }
        }

        struct TerrainVertex
        {
            public Vector3 position;

            public static int Size
            {
                get { return Vector3.SizeInBytes; }
            }

            public static VertexFormat Format
            {
                get { return VertexFormat.Position; }
            }
        }

        #region 常量

        public const int DetailLayerCount = 4;

        public const int BlockSize = 33;
        public const int BlockEdgeLen = BlockSize - 1;

        public const int TerrainSize = 513;
        public const int TerrainLength = Cluster.ClusterLength;


        protected static readonly string DisplacementMapTag = "DisplacementMap";
        protected static readonly string ColorMapTag = "ColorMap";
        protected static readonly string NormalMapTag = "NormalMap";
        protected static readonly string IndexMapTag = "IndexMap";
        protected static readonly string DetailMapTag = "DetailMap";

        protected static readonly string OffsetTag = "Offset";

        #endregion

        #region 静态字段和属性

        public static IndexBuffer[] SharedIndexBuffers
        {
            get;
            private set;
        }

        public static VertexBuffer SharedVertexBuffer
        {
            get;
            private set;
        }

        static int[] levelLengths;

        /// <summary>
        ///  在不同lod级别下一个单元的跨度
        /// </summary>
        static int[] cellSpan;

        /// <summary>
        ///  lod 权值
        /// </summary>
        static float[] lodLevelThreshold;

        static int[] levelPrimConut;

        static int[] levelVertexCount;

        #endregion

        #region 字段

        TerrainSettings terrainSettings;

        Device device;

        VertexBuffer vertexBuffer;
        VertexDeclaration vtxDecl;

        IndexBuffer[] levelIb;

        protected MeshMaterial material;

        TerrainTexture displacementMap;
        TerrainTexture colorMap;
        TerrainTexture normalMap;
        TerrainTexture indexMap;

        protected GameTexture[] detailMaps = new GameTexture[DetailLayerCount];
        protected GameTexture[] detailNrmMaps = new GameTexture[DetailLayerCount];
        protected string[] detailMapName = new string[DetailLayerCount]
        {
            "TerrainDefault",
            "TerrainDefault", 
            "TerrainDefault", 
            "TerrainDefault"
        };

        Matrix worldTrans;

        int blockCount;
        int blockEdgeCount;

        Queue<TerrainTreeNode> bfsQueue;

        TerrainTreeNode rootNode;

        FastList<RenderOperation> opBuffer;

        protected BlockMesh[][] collBlocks;

        float[] dispMapData;

        #endregion

        #region 属性

        [Browsable(false)]
        public int BlockCount
        {
            get { return blockCount; }
        }

        [Browsable(false)]
        public float HeightScale
        {
            get;
            private set;
        }
        [Browsable(false)]
        public float CellUnit
        {
            get;
            private set;
        }

        [Browsable(false)]
        public Device Device
        {
            get { return device; }
        }

        [Browsable(false)]
        protected TerrainTreeNode RootNode
        {
            get { return rootNode; }
        }

        protected TerrainSettings TerrainSettings
        {
            get { return terrainSettings; }
        }

        [Browsable(false)]
        public bool IsCollisionModelLoaded
        {
            get { return collBlocks != null; }
        }

        public TerrainTexture DisplacementMap
        {
            get { return displacementMap; }
            protected set { displacementMap = value; }
        }
        public TerrainTexture ColorMap
        {
            get { return colorMap; }
            protected set { colorMap = value; }
        }
        public TerrainTexture NormalMap
        {
            get { return normalMap; }
            protected set { normalMap = value; }
        }
        public TerrainTexture IndexMap
        {
            get { return indexMap; }
            protected set { indexMap = value; }
        }

        #endregion

        #region 构造函数

        public Terrain(Device device, float cellUnit, TerrainSettings terrData)
            : base(true)
        {
            this.bfsQueue = new Queue<TerrainTreeNode>();
            this.opBuffer = new FastList<RenderOperation>();

            this.device = device;

            this.terrainSettings = terrData;

            this.HeightScale = terrData.HeightScale;
            this.CellUnit = cellUnit;

            this.blockEdgeCount = TerrainLength / BlockEdgeLen;
            this.blockCount = MathEx.Sqr(blockEdgeCount);

            this.vtxDecl = GetVertexDeclaration();


            if (SharedVertexBuffer == null)
            {
                SharedVertexBuffer = new VertexBuffer(device, TerrainVertex.Size * TerrainSize * TerrainSize,
                    Usage.None, TerrainVertex.Format, Pool.Managed);

                TerrainVertex* pointer = (TerrainVertex*)SharedVertexBuffer.Lock(0, 0, LockFlags.None).DataPointer.ToPointer();

                for (int i = 0; i < TerrainSize; i++)
                {
                    for (int j = 0; j < TerrainSize; j++)
                    {
                        pointer[j * TerrainSize + i].position = new Vector3(i * CellUnit, 0, j * CellUnit);
                    }
                }

                SharedVertexBuffer.Unlock();

            }
            vertexBuffer = SharedVertexBuffer;


            if (SharedIndexBuffers == null)
            {
                SharedIndexBuffers = new IndexBuffer[4];
                levelLengths = new int[4];
                cellSpan = new int[4];
                lodLevelThreshold = new float[4];

                levelPrimConut = new int[4];
                levelVertexCount = new int[4];

                for (int k = 0, levelLength = BlockEdgeLen; k < 4; k++, levelLength /= 2)
                {
                    int cellLength = BlockEdgeLen / levelLength;

                    int halfCellLen = cellLength / 2;

                    lodLevelThreshold[k] = (TerrainSize * MathEx.Root2 * 0.25f) / (float)(k + 1);
                    lodLevelThreshold[k] = MathEx.Sqr(lodLevelThreshold[k]);

                    cellSpan[k] = cellLength;
                    levelLengths[k] = levelLength;


                    if (halfCellLen == 0)
                    {
                        int indexCount = MathEx.Sqr(levelLength) * 2 * 3;

                        levelPrimConut[k] = MathEx.Sqr(levelLength) * 2;
                        levelVertexCount[k] = MathEx.Sqr(levelLength + 1);

                        SharedIndexBuffers[k] = new IndexBuffer(device, sizeof(int) * indexCount, Usage.None, Pool.Managed, false);

                        int* iptr = (int*)SharedIndexBuffers[k].Lock(0, 0, LockFlags.None).DataPointer.ToPointer();

                        for (int i = 0; i < levelLength; i++)
                        {
                            for (int j = 0; j < levelLength; j++)
                            {
                                int x = i * cellLength;
                                int y = j * cellLength;

                                (*iptr) = y * TerrainSize + x;
                                iptr++;
                                (*iptr) = y * TerrainSize + (x + cellLength);
                                iptr++;
                                (*iptr) = (y + cellLength) * TerrainSize + (x + cellLength);
                                iptr++;

                                (*iptr) = y * TerrainSize + x;
                                iptr++;
                                (*iptr) = (y + cellLength) * TerrainSize + (x + cellLength);
                                iptr++;
                                (*iptr) = (y + cellLength) * TerrainSize + x;
                                iptr++;
                            }
                        }

                        SharedIndexBuffers[k].Unlock();
                    }
                    else
                    {
                        int indexCount = MathEx.Sqr(levelLength - 2) * 2 * 3 +
                            4 * (levelLength - 2) * 3 * 3 + 4 * 3 * 4;

                        levelPrimConut[k] = indexCount / 3;
                        levelVertexCount[k] = MathEx.Sqr(levelLength + 1) + 4 * (levelLength - 1);

                        SharedIndexBuffers[k] = new IndexBuffer(device, sizeof(int) * indexCount, Usage.None, Pool.Managed, false);

                        int* iptr = (int*)SharedIndexBuffers[k].Lock(0, 0, LockFlags.None).DataPointer.ToPointer();

                        #region 四个角

                        // tl
                        (*iptr) = 0;
                        iptr++;
                        (*iptr) = halfCellLen;
                        iptr++;
                        (*iptr) = halfCellLen * TerrainSize;
                        iptr++;

                        (*iptr) = cellLength * TerrainSize + cellLength;
                        iptr++;
                        (*iptr) = cellLength * TerrainSize;
                        iptr++;
                        (*iptr) = halfCellLen * TerrainSize;
                        iptr++;

                        (*iptr) = halfCellLen * TerrainSize;
                        iptr++;
                        (*iptr) = halfCellLen;
                        iptr++;
                        (*iptr) = cellLength * TerrainSize + cellLength;
                        iptr++;

                        (*iptr) = cellLength;
                        iptr++;
                        (*iptr) = cellLength * TerrainSize + cellLength;
                        iptr++;
                        (*iptr) = halfCellLen;
                        iptr++;


                        int levelLengthAbs = levelLength * cellLength;
                        // ==============================
                        //bl
                        (*iptr) = levelLengthAbs * TerrainSize;
                        iptr++;
                        (*iptr) = (levelLengthAbs - halfCellLen) * TerrainSize;
                        iptr++;
                        (*iptr) = levelLengthAbs * TerrainSize + halfCellLen;
                        iptr++;

                        (*iptr) = (levelLengthAbs - halfCellLen) * TerrainSize;
                        iptr++;
                        (*iptr) = (levelLengthAbs - cellLength) * TerrainSize;
                        iptr++;
                        (*iptr) = (levelLengthAbs - cellLength) * TerrainSize + cellLength;
                        iptr++;

                        (*iptr) = (levelLengthAbs - halfCellLen) * TerrainSize;
                        iptr++;
                        (*iptr) = (levelLengthAbs - cellLength) * TerrainSize + cellLength;
                        iptr++;
                        (*iptr) = levelLengthAbs * TerrainSize + halfCellLen;
                        iptr++;

                        (*iptr) = levelLengthAbs * TerrainSize + halfCellLen;
                        iptr++;
                        (*iptr) = (levelLengthAbs - cellLength) * TerrainSize + cellLength;
                        iptr++;
                        (*iptr) = levelLengthAbs * TerrainSize + cellLength;
                        iptr++;

                        // ==============================
                        // br


                        (*iptr) = levelLengthAbs * TerrainSize - halfCellLen + levelLengthAbs;
                        iptr++;
                        (*iptr) = (levelLengthAbs - halfCellLen) * TerrainSize + levelLengthAbs;
                        iptr++;
                        (*iptr) = levelLengthAbs * TerrainSize + levelLengthAbs;
                        iptr++;

                        (*iptr) = levelLengthAbs * TerrainSize - halfCellLen + levelLengthAbs;
                        iptr++;
                        (*iptr) = levelLengthAbs * TerrainSize - cellLength + levelLengthAbs;
                        iptr++;
                        (*iptr) = (levelLengthAbs - cellLength) * TerrainSize - cellLength + levelLengthAbs;
                        iptr++;

                        (*iptr) = (levelLengthAbs - halfCellLen) * TerrainSize + levelLengthAbs;
                        iptr++;
                        (*iptr) = levelLengthAbs * TerrainSize - halfCellLen + levelLengthAbs;
                        iptr++;
                        (*iptr) = (levelLengthAbs - cellLength) * TerrainSize - cellLength + levelLengthAbs;
                        iptr++;

                        (*iptr) = (levelLengthAbs - cellLength) * TerrainSize + levelLengthAbs;
                        iptr++;
                        (*iptr) = (levelLengthAbs - halfCellLen) * TerrainSize + levelLengthAbs;
                        iptr++;
                        (*iptr) = (levelLengthAbs - cellLength) * TerrainSize - cellLength + levelLengthAbs;
                        iptr++;

                        // ==============================
                        // tr
                        (*iptr) = levelLengthAbs;
                        iptr++;
                        (*iptr) = halfCellLen * TerrainSize + levelLengthAbs;
                        iptr++;
                        (*iptr) = levelLengthAbs - halfCellLen;
                        iptr++;

                        (*iptr) = cellLength * TerrainSize + levelLengthAbs - cellLength;
                        iptr++;
                        (*iptr) = levelLengthAbs - cellLength;
                        iptr++;
                        (*iptr) = levelLengthAbs - halfCellLen;
                        iptr++;

                        (*iptr) = cellLength * TerrainSize + levelLengthAbs - cellLength;
                        iptr++;
                        (*iptr) = levelLengthAbs - halfCellLen;
                        iptr++;
                        (*iptr) = halfCellLen * TerrainSize + levelLengthAbs;
                        iptr++;

                        (*iptr) = cellLength * TerrainSize + levelLengthAbs - cellLength;
                        iptr++;
                        (*iptr) = halfCellLen * TerrainSize + levelLengthAbs;
                        iptr++;
                        (*iptr) = cellLength * TerrainSize + levelLengthAbs;
                        iptr++;

                        #endregion

                        #region 四条边
                        for (int i = 1; i < levelLength - 1; i++)
                        {
                            // left
                            int x = 0;
                            int y = i * cellLength;

                            (*iptr) = y * TerrainSize + x + cellLength;
                            iptr++;
                            (*iptr) = (y + halfCellLen) * TerrainSize + x;
                            iptr++;
                            (*iptr) = y * TerrainSize + x;
                            iptr++;

                            (*iptr) = (y + halfCellLen) * TerrainSize + x;
                            iptr++;
                            (*iptr) = y * TerrainSize + x + cellLength;
                            iptr++;
                            (*iptr) = (y + cellLength) * TerrainSize + (x + cellLength);
                            iptr++;

                            (*iptr) = (y + cellLength) * TerrainSize + (x + cellLength);
                            iptr++;
                            (*iptr) = (y + cellLength) * TerrainSize + x;
                            iptr++;
                            (*iptr) = (y + halfCellLen) * TerrainSize + x;
                            iptr++;

                            // top
                            x = y;
                            y = 0;

                            (*iptr) = y * TerrainSize + x;
                            iptr++;
                            (*iptr) = y * TerrainSize + (x + halfCellLen);
                            iptr++;
                            (*iptr) = (y + cellLength) * TerrainSize + x;
                            iptr++;

                            (*iptr) = (y + cellLength) * TerrainSize + (x + cellLength);
                            iptr++;
                            (*iptr) = (y + cellLength) * TerrainSize + x;
                            iptr++;
                            (*iptr) = y * TerrainSize + (x + halfCellLen);
                            iptr++;

                            (*iptr) = y * TerrainSize + (x + cellLength);
                            iptr++;
                            (*iptr) = (y + cellLength) * TerrainSize + (x + cellLength);
                            iptr++;
                            (*iptr) = y * TerrainSize + (x + halfCellLen);
                            iptr++;

                            // right
                            x = (levelLength - 1) * cellLength;
                            y = i * cellLength;

                            (*iptr) = (y + halfCellLen) * TerrainSize + (x + cellLength);
                            iptr++;
                            (*iptr) = y * TerrainSize + x;
                            iptr++;
                            (*iptr) = y * TerrainSize + (x + cellLength);
                            iptr++;

                            (*iptr) = y * TerrainSize + x;
                            iptr++;
                            (*iptr) = (y + halfCellLen) * TerrainSize + (x + cellLength);
                            iptr++;
                            (*iptr) = (y + cellLength) * TerrainSize + x;
                            iptr++;

                            (*iptr) = (y + cellLength) * TerrainSize + (x + cellLength);
                            iptr++;
                            (*iptr) = (y + cellLength) * TerrainSize + x;
                            iptr++;
                            (*iptr) = (y + halfCellLen) * TerrainSize + (x + cellLength);
                            iptr++;

                            // bottom
                            x = y;
                            y = (levelLength - 1) * cellLength;

                            (*iptr) = (y + cellLength) * TerrainSize + (x + halfCellLen);
                            iptr++;
                            (*iptr) = (y + cellLength) * TerrainSize + x;
                            iptr++;
                            (*iptr) = y * TerrainSize + x;
                            iptr++;

                            (*iptr) = y * TerrainSize + (x + cellLength);
                            iptr++;
                            (*iptr) = (y + cellLength) * TerrainSize + (x + halfCellLen);
                            iptr++;
                            (*iptr) = y * TerrainSize + x;
                            iptr++;

                            (*iptr) = (y + cellLength) * TerrainSize + (x + cellLength);
                            iptr++;
                            (*iptr) = (y + cellLength) * TerrainSize + (x + halfCellLen);
                            iptr++;
                            (*iptr) = y * TerrainSize + (x + cellLength);
                            iptr++;

                        }

                        #endregion


                        for (int i = 1; i < levelLength - 1; i++)
                        {
                            for (int j = 1; j < levelLength - 1; j++)
                            {
                                int x = i * cellLength;
                                int y = j * cellLength;

                                (*iptr) = y * TerrainSize + x;
                                iptr++;
                                (*iptr) = y * TerrainSize + (x + cellLength);
                                iptr++;
                                (*iptr) = (y + cellLength) * TerrainSize + (x + cellLength);
                                iptr++;

                                (*iptr) = y * TerrainSize + x;
                                iptr++;
                                (*iptr) = (y + cellLength) * TerrainSize + (x + cellLength);
                                iptr++;
                                (*iptr) = (y + cellLength) * TerrainSize + x;
                                iptr++;
                            }
                        }

                        SharedIndexBuffers[k].Unlock();
                    }
                }
            }

            levelIb = SharedIndexBuffers;

            worldTrans = Matrix.Identity;

            material = new MeshMaterial(device);
            material.CullMode = Cull.Counterclockwise;

            material.mat.Ambient = terrData.MaterialAmbient;
            material.mat.Diffuse = terrData.MaterialDiffuse;
            material.mat.Emissive = terrData.MaterialEmissive;
            material.mat.Specular = terrData.MaterialSpecular;
            material.mat.Power = terrData.MaterialPower;
            material.SetEffect(GetTerrainEffect());
        }

        #endregion

        #region 方法

        public float ComputeTerrainHeight(float inp)
        {
            return inp * HeightScale;
        }

        VertexDeclaration GetVertexDeclaration()
        {
            VertexElement[] element = new VertexElement[2]
            {
                new VertexElement(0, 0, DeclarationType.Float3, DeclarationMethod.Default, DeclarationUsage.Position, 0),
                VertexElement.VertexDeclarationEnd
            };

            return new VertexDeclaration(device, element);
        }

        protected virtual ModelEffect GetTerrainEffect()
        {
            return EffectManager.Instance.GetModelEffect("TerrainRendering");
        }

        public GameTexture GetDetailMap(int index)
        {
            return detailMaps[index];
        }

        public GameTexture GetDetailNrmMap(int index)
        {
            return detailNrmMaps[index];
        }

        public string GetDetailMapName(int index)
        {
            return detailMapName[index];
        }

        public override void PrepareVisibleObjects(ICamera cam)
        {
            opBuffer.Clear();

            Frustum frus = cam.Frustum;
            Vector3 camPos = cam.Position;

            float ofsX = OffsetX;
            float ofsY = OffsetY;
            float ofsZ = OffsetZ;


            Vector3 c = rootNode.BoundingVolume.Center;
            c.X += ofsX;
            c.Y += ofsY;
            c.Z += ofsZ;

            if (frus.IntersectsSphere(ref c, rootNode.BoundingVolume.Radius))
            {
                bfsQueue.Enqueue(rootNode);

                while (bfsQueue.Count > 0)
                {
                    TerrainTreeNode node = bfsQueue.Dequeue();
                    TerrainTreeNode[] nodes = node.Children;

                    if (nodes != null)
                    {
                        // 遍历子节点
                        for (int i = 0; i < node.Children.Length; i++)
                        {
                            c = node.Children[i].BoundingVolume.Center;
                            c.X += ofsX;
                            c.Y += ofsY;
                            c.Z += ofsZ;

                            if (frus.IntersectsSphere(ref c, node.Children[i].BoundingVolume.Radius))
                            {
                                bfsQueue.Enqueue(node.Children[i]);
                            }
                        }
                    }
                    else
                    {
                        if (node.Block != null)
                        {
                            c = node.BoundingVolume.Center;
                            c.X += ofsX;
                            c.Y += ofsY;
                            c.Z += ofsZ;

                            if (frus.IntersectsSphere(ref c, node.BoundingVolume.Radius))
                            {
                                float dist = MathEx.DistanceSquared(ref c, ref camPos);

                                RenderOperation op;

                                op.Material = material;
                                op.Geomentry = node.Block.GeoData;

                                int lodLevel = 3;

                                for (int lod = 0; lod < 4; lod++)
                                {
                                    if (dist <= lodLevelThreshold[3 - lod])
                                    {
                                        lodLevel = lod;
                                        break;
                                    }
                                }

                                op.Geomentry.IndexBuffer = node.Block.IndexBuffers[lodLevel];
                                op.Geomentry.PrimCount = levelPrimConut[lodLevel];// levelLengths[lodLevel] * levelLengths[lodLevel] * 2;
                                op.Geomentry.VertexCount = levelVertexCount[lodLevel];// MathEx.Sqr(levelLengths[lodLevel] + 1);

                                op.Transformation = worldTrans;

                                opBuffer.Add(op);
                            }
                        }
                    }

                }
            }
        }

        void BuildTerrainTree()
        {
            Texture d3ddm = displacementMap.GetTexture;

            float* dmData = (float*)d3ddm.LockRectangle(0, LockFlags.ReadOnly).Data.DataPointer.ToPointer();

            TerrainBlock[] blocks = new TerrainBlock[blockCount];

            int index = 0;
            for (int i = 0; i < blockEdgeCount; i++)
            {
                for (int j = 0; j < blockEdgeCount; j++)
                {
                    Vector3 center = new Vector3();

                    blocks[index] = new TerrainBlock(j * BlockEdgeLen, i * BlockEdgeLen);

                    // 检查该块中是否有特殊单元
                    if (!false)
                    {
                        blocks[index].IndexBuffers = levelIb;
                    }
                    else
                    {
                        // 为这个block创建特殊的IB
                    }

                    GeomentryData gd = new GeomentryData(this);
                    gd.VertexDeclaration = vtxDecl;
                    gd.Format = TerrainVertex.Format;
                    gd.VertexSize = TerrainVertex.Size;
                    gd.VertexBuffer = vertexBuffer;
                    gd.IndexBuffer = levelIb[0];
                    gd.PrimCount = levelPrimConut[0];// levelLengths[0] * levelLengths[0] * 2;
                    gd.VertexCount = levelVertexCount[0];// MathEx.Sqr(levelLengths[0] + 1);

                    gd.PrimitiveType = PrimitiveType.TriangleList;

                    int x = (j == 0) ? 0 : j * BlockEdgeLen;
                    int y = (i == 0) ? 0 : i * BlockEdgeLen;

                    gd.BaseVertex = y * TerrainSize + x;

                    blocks[index].GeoData = gd;

                    for (int ii = 0; ii < BlockSize; ii++)
                    {
                        for (int jj = 0; jj < BlockSize; jj++)
                        {
                            int dmY = i * BlockEdgeLen + ii;
                            int dmX = j * BlockEdgeLen + jj;

                            center.X += CellUnit * dmX;
                            center.Y += ComputeTerrainHeight(dmData[dmY * TerrainSize + dmX]);
                            center.Z += CellUnit * dmY;
                        }
                    }

                    float invVtxCount = 1f / (float)(BlockSize * BlockSize);
                    center.X *= invVtxCount;
                    center.Y *= invVtxCount;
                    center.Z *= invVtxCount;


                    float radius = 0;
                    for (int ii = 0; ii < BlockSize; ii++)
                    {
                        for (int jj = 0; jj < BlockSize; jj++)
                        {
                            int dmY = i * BlockEdgeLen + ii;
                            int dmX = j * BlockEdgeLen + jj;

                            Vector3 vtxPos = new Vector3(CellUnit * dmX, ComputeTerrainHeight(dmData[dmY * TerrainSize + dmX]), CellUnit * dmY);
                            float dist = Vector3.Distance(vtxPos, center);
                            if (dist > radius)
                            {
                                radius = dist;
                            }
                        }
                    }
                    blocks[index].Radius = radius;
                    blocks[index].Center = center;

                    index++;
                }
            }
            d3ddm.UnlockRectangle(0);

            rootNode = new TerrainTreeNode(new FastList<TerrainBlock>(blocks), TerrainLength / 2, TerrainLength / 2, 1);

            this.BoundingSphere = rootNode.BoundingVolume;
        }

        public void LoadData(BinaryDataReader data)
        {
            Stream stm = data.GetDataStream(DisplacementMapTag);
            StreamedLocation loc = new StreamedLocation(stm);

            displacementMap = TerrainTextureManager.Instance.CreateInstance(device, loc, true);

            // ---------------------------------------

            stm = data.GetDataStream(ColorMapTag);
            loc = new StreamedLocation(stm);

            colorMap = TerrainTextureManager.Instance.CreateInstance(device, loc, false);

            // ---------------------------------------

            stm = data.GetDataStream(NormalMapTag);
            loc = new StreamedLocation(stm);

            normalMap = TerrainTextureManager.Instance.CreateInstance(device, loc, false);

            // ---------------------------------------

            stm = data.GetDataStream(IndexMapTag);
            loc = new StreamedLocation(stm);

            indexMap = TerrainTextureManager.Instance.CreateInstance(device, loc, false);

            // ---------------------------------------
            ContentBinaryReader br;

            for (int i = 0; i < DetailLayerCount; i++)
            {
                br = data.GetData(DetailMapTag + i.ToString());
                detailMapName[i] = br.ReadStringUnicode();
                br.Close();

                detailMaps[i] = TextureLibrary.Instance.GetColorMap(detailMapName[i]);
                detailNrmMaps[i] = TextureLibrary.Instance.GetNormalMap(detailMapName[i]);
            }

            br = data.GetData(OffsetTag);
            OffsetX = br.ReadSingle();
            OffsetY = br.ReadSingle();
            OffsetZ = br.ReadSingle();
            br.Close();

            material.SetTexture(0, displacementMap);
            material.SetTexture(1, colorMap);
            material.SetTexture(2, normalMap);
            material.SetTexture(3, indexMap);
            material.SetTexture(4, detailMaps[0]);
            material.SetTexture(5, detailNrmMaps[0]);
            material.SetTexture(6, detailMaps[1]);
            material.SetTexture(7, detailNrmMaps[1]);
            material.SetTexture(8, detailMaps[2]);
            material.SetTexture(9, detailNrmMaps[2]);
            material.SetTexture(10, detailMaps[3]);
            material.SetTexture(11, detailNrmMaps[3]);


            BuildTerrainTree();
        }

        public override bool IntersectsSelectionRay(ref Ray ray)
        {
            return false;
        }

        #endregion

        #region 相交检测

        public unsafe void LoadCollisionModel()
        {
            const int blockEdgeCount = TerrainLength / BlockEdgeLen;

            collBlocks = new BlockMesh[blockEdgeCount][];

            Texture dm = DisplacementMap.GetTexture;

            float* data = (float*)dm.LockRectangle(0, LockFlags.ReadOnly).Data.DataPointer.ToPointer();

            for (int i = 0; i < blockEdgeCount; i++)
            {
                collBlocks[i] = new BlockMesh[blockEdgeCount];
                //for (int j = 0; j < blockEdgeCount; j++)
                //{
                //    collBlocks[i][j] = new BlockMesh(i, j, data, CellUnit, HeightScale);
                //}
            }

            SurfaceDescription desc = dm.GetLevelDescription(0);

            dispMapData = new float[desc.Width * desc.Height];
            fixed (float* dst = &dispMapData[0])
            {
                Memory.Copy(data, dst, desc.Width * desc.Height * 4);
            }

            dm.UnlockRectangle(0);
        }

        public void ReleaseCollisionModel()
        {
            collBlocks = null;
            dispMapData = null;
        }

        public bool Intersects(LineSegment ray, out Vector3 result)
        {
            if (!IsCollisionModelLoaded)
            {
                throw new InvalidOperationException();
            }

            float ofsX = OffsetX;
            float ofsY = OffsetY;
            float ofsZ = OffsetZ;

            ray.Start.X -= ofsX;
            ray.Start.Y -= ofsY;
            ray.Start.Z -= ofsZ;

            ray.End.X -= ofsX;
            ray.End.Y -= ofsY;
            ray.End.Z -= ofsZ;


            bfsQueue.Enqueue(RootNode);
            while (bfsQueue.Count > 0)
            {
                TerrainTreeNode node = bfsQueue.Dequeue();
                TerrainTreeNode[] nodes = node.Children;

                if (nodes != null)
                {
                    // 遍历子节点
                    for (int i = 0; i < node.Children.Length; i++)
                    {
                        if (MathEx.BoundingSphereIntersects(ref node.Children[i].BoundingVolume, ref ray.Start, ref ray.End))
                        {
                            bfsQueue.Enqueue(node.Children[i]);
                        }
                    }
                }
                else
                {
                    if (node.Block != null)
                    {
                        int blkX = node.Block.X / BlockEdgeLen;
                        int blkY = node.Block.Y / BlockEdgeLen;

                        if (collBlocks[blkX][blkY] == null)
                        {
                            fixed (float* ptr = &dispMapData[0])
                            {
                                collBlocks[blkX][blkY] = new BlockMesh(node.Block.X, node.Block.Y, ptr, CellUnit, HeightScale);
                            }
                        }

                        if (collBlocks[blkX][blkY].Intersects(ray.Start, ray.End, out result))
                        {
                            bfsQueue.Clear();
                            return true;
                        }
                    }
                }
            }
            result = new Vector3();
            return false;
        }

        public bool Intersects(LineSegment ray)
        {
            //if (!IsCollisionModelLoaded)
            //{
            //    throw new InvalidOperationException();
            //}


            float ofsX = OffsetX;
            float ofsY = OffsetY;
            float ofsZ = OffsetZ;

            ray.Start.X -= ofsX;
            ray.Start.Y -= ofsY;
            ray.Start.Z -= ofsZ;

            ray.End.X -= ofsX;
            ray.End.Y -= ofsY;
            ray.End.Z -= ofsY;


            bfsQueue.Enqueue(RootNode);
            while (bfsQueue.Count > 0)
            {
                TerrainTreeNode node = bfsQueue.Dequeue();
                TerrainTreeNode[] nodes = node.Children;

                if (nodes != null)
                {
                    // 遍历子节点
                    for (int i = 0; i < node.Children.Length; i++)
                    {
                        if (MathEx.BoundingSphereIntersects(ref node.Children[i].BoundingVolume, ref ray.Start, ref ray.End))
                        {
                            bfsQueue.Enqueue(node.Children[i]);
                        }
                    }
                }
                else
                {
                    if (node.Block != null)
                    {
                        if (!IsCollisionModelLoaded)
                        {
                            bfsQueue.Clear();
                            return true;
                        }
                        else
                        {
                            int blkX = node.Block.X / BlockEdgeLen;
                            int blkY = node.Block.Y / BlockEdgeLen;


                            Vector3 result;

                            if (collBlocks[blkX][blkY] == null)
                            {
                                fixed (float* ptr = &dispMapData[0])
                                {
                                    collBlocks[blkX][blkY] = new BlockMesh(node.Block.X, node.Block.Y, ptr, CellUnit, HeightScale);
                                }
                            }


                            if (collBlocks[blkX][blkY].Intersects(ray.Start, ray.End, out result))
                            {
                                bfsQueue.Clear();
                                return true;
                            }
                        }
                    }
                }
            }
            return false;
        }

        #endregion

        #region IRenderable 成员

        public override RenderOperation[] GetRenderOperation()
        {
            return opBuffer.Elements;
        }

        #endregion

        #region IUpdatable 成员

        public override void Update(float dt)
        {

        }

        #endregion

        #region IDisposable 成员

        protected override void Dispose(bool disposing)
        {
            base.Dispose(disposing);

            if (disposing)
            {
                TerrainTextureManager.Instance.DestroyInstance(DisplacementMap);
                DisplacementMap = null;

                TerrainTextureManager.Instance.DestroyInstance(ColorMap);
                ColorMap = null;

                TerrainTextureManager.Instance.DestroyInstance(IndexMap);
                IndexMap = null;

                TerrainTextureManager.Instance.DestroyInstance(NormalMap);
                NormalMap = null;
            }
        }

        #endregion

        #region 物理相关

        DefaultMotionState motionState;

        [Browsable(false)]
        public override bool HasPhysicsModel
        {
            get { return true; }
        }
        ClusterTerrainShape shape;

        bool isPhyBuilt;

        public override void BuildPhysicsModel(DynamicsWorld world)
        {
            if (!isPhyBuilt)
            {
                TerrainTexture terrTex = DisplacementMap;
                if (terrTex.ResourceEntity != null) 
                {
                    terrTex = (TerrainTexture)terrTex.ResourceEntity;
                }

                shape = new ClusterTerrainShape(CellUnit, HeightScale, terrTex);
                motionState = new DefaultMotionState(Matrix.Translation(Terrain.TerrainSize * 0.5f, 0, Terrain.TerrainSize * 0.5f));

                RigidBody = new RigidBody(0, motionState, shape);

                if (world != null)
                {
                    world.AddRigidBody(RigidBody);
                }
                isPhyBuilt = true;
            }
        }

        #endregion

        #region SceneObject 序列化

        [Browsable(false)]
        public override string TypeTag
        {
            get { return TerrainFactory.TypeId; ;}
        }

        public override void Serialize(BinaryDataWriter data)
        {
            Stream stream = data.AddEntryStream(DisplacementMapTag);
            displacementMap.Save(stream);
            stream.Close();

            // ---------------------------------------

            stream = data.AddEntryStream(ColorMapTag);
            colorMap.Save(stream);
            stream.Close();

            // ---------------------------------------

            stream = data.AddEntryStream(NormalMapTag);
            normalMap.Save(stream);
            stream.Close();

            // ---------------------------------------

            stream = data.AddEntryStream(IndexMapTag);
            indexMap.Save(stream);
            stream.Close();

            // ---------------------------------------

            ContentBinaryWriter bw;
            for (int i = 0; i < DetailLayerCount; i++)
            {
                bw = data.AddEntry(DetailMapTag + i.ToString());
                bw.WriteStringUnicode(detailMapName[i]);
                bw.Close();
            }

            bw = data.AddEntry(OffsetTag);
            bw.Write(OffsetX);
            bw.Write(OffsetY);
            bw.Write(OffsetZ);
            bw.Close();
        }

        [Browsable(false)]
        public override bool IsSerializable
        {
            get { return true; }
        }
        #endregion
    }
}
