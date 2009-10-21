using System;
using System.Collections.Generic;
using System.Text;
using SlimDX;
using SlimDX.Direct3D9;
using VirtualBicycle.CollisionModel;
using VirtualBicycle.Graphics;
using VirtualBicycle.Graphics.Effects;
using VirtualBicycle.MathLib;
using VirtualBicycle.Scene;

namespace VirtualBicycle.Ide.Editors.EditableObjects
{
    public unsafe class EditableTerrain : Terrain
    {
        TerrainRenderingEditEffect renderEffect;

        public EditableTerrain(Device device, float cellUnit, TerrainSettings terrData)
            : base(device, cellUnit, terrData)
        {
        }

        public void LoadEmptyTerrain()
        {
            Texture dtex = new Texture(base.Device, Terrain.TerrainSize, Terrain.TerrainSize, 1, Usage.None, Format.R32F, Pool.Managed);
            TerrainTexture tex = TerrainTextureManager.Instance.CreateInstance(dtex, true);
            SetDisplacementMap(tex);

            dtex = new Texture(base.Device, Cluster.ClusterLength, Cluster.ClusterLength, 1, Usage.None, Format.A8R8G8B8, Pool.Managed);
            tex = TerrainTextureManager.Instance.CreateInstance(dtex, false);
            SetColorMap(tex);

            dtex = new Texture(base.Device, Cluster.ClusterLength, Cluster.ClusterLength, 1, Usage.None, Format.A8R8G8B8, Pool.Managed);
            tex = TerrainTextureManager.Instance.CreateInstance(dtex, false);
            SetIndexMap(tex);

            dtex = new Texture(base.Device, Cluster.ClusterLength, Cluster.ClusterLength, 1, Usage.None, Format.A8R8G8B8, Pool.Managed);
            tex = TerrainTextureManager.Instance.CreateInstance(dtex, false);
            SetNormalMap(tex);
        }


        public void UpdateCollisionModel()
        {
            const int blockEdgeCount = TerrainLength / BlockEdgeLen;

            Texture dm = DisplacementMap.GetTexture;

            int* data = (int*)dm.LockRectangle(0, LockFlags.ReadOnly).Data.DataPointer.ToPointer();

            for (int i = 0; i < blockEdgeCount; i++)
            {
                for (int j = 0; j < blockEdgeCount; j++)
                {
                    if (collBlocks[i][j] != null)
                    {
                        collBlocks[i][j].Update(i, j, data, CellUnit, HeightScale);
                    }
                }
            }

            dm.UnlockRectangle(0);

            DisplacementMap.NotifyChanged();
        }

        public void SetDisplacementMap(TerrainTexture tex)
        {
            if (tex != DisplacementMap)
            {
                if (DisplacementMap != null)
                {
                    TerrainTextureManager.Instance.DestroyInstance(DisplacementMap);
                }
                DisplacementMap = tex;
                material.SetTexture(0, DisplacementMap);

                if (!IsCollisionModelLoaded)
                {
                    LoadCollisionModel();
                }
                UpdateCollisionModel();
            }
        }
        public void SetColorMap(TerrainTexture tex)
        {
            if (tex != ColorMap)
            {
                if (ColorMap != null)
                {
                    TerrainTextureManager.Instance.DestroyInstance(ColorMap);
                }
                ColorMap = tex;
                material.SetTexture(1, ColorMap);
            }
        }
        public void SetIndexMap(TerrainTexture tex)
        {
            if (tex != IndexMap)
            {
                if (IndexMap != null)
                {
                    TerrainTextureManager.Instance.DestroyInstance(IndexMap);
                }
                IndexMap = tex;
                material.SetTexture(3, IndexMap);
            }
        }
        public void SetNormalMap(TerrainTexture tex)
        {
            if (tex != NormalMap)
            {
                if (NormalMap != null)
                {
                    TerrainTextureManager.Instance.DestroyInstance(NormalMap);
                }
                NormalMap = tex;
                material.SetTexture(2, NormalMap);
            }
        }
        //public void UpdateMaps()
        //{
        //ClusterDescription desc = ParentCluster.Description;

        //DisplacementMap = edTexCache.GetDisplacementMap(desc.X, desc.Y);
        //ColorMap = edTexCache.GetColorMap(desc.X, desc.Y);
        //NormalMap = edTexCache.GetNormalMap(desc.X, desc.Y);
        //IndexMap = edTexCache.GetIndexMap(desc.X, desc.Y);

        //    material.SetTexture(0, DisplacementMap);
        //    material.SetTexture(1, ColorMap);
        //    material.SetTexture(2, NormalMap);
        //    material.SetTexture(3, IndexMap);
        //}

        public void SetDetailMap(int index, string name)
        {
            detailMapName[index] = name;

            detailMaps[index] = TextureLibrary.Instance.GetColorMap(name);
            detailNrmMaps[index] = TextureLibrary.Instance.GetNormalMap(name);


            material.SetTexture(4, detailMaps[0]);
            material.SetTexture(5, detailNrmMaps[0]);
            material.SetTexture(6, detailMaps[1]);
            material.SetTexture(7, detailNrmMaps[1]);
            material.SetTexture(8, detailMaps[2]);
            material.SetTexture(9, detailNrmMaps[2]);
            material.SetTexture(10, detailMaps[3]);
            material.SetTexture(11, detailNrmMaps[3]);
        }

        public void SetDetailMaps(string[] name)
        {
            for (int i = 0; i < 4; i++)
            {
                detailMapName[i] = name[i];

                detailMaps[i] = TextureLibrary.Instance.GetColorMap(name[i]);
                detailNrmMaps[i] = TextureLibrary.Instance.GetNormalMap(name[i]);
            }


            material.SetTexture(4, detailMaps[0]);
            material.SetTexture(5, detailNrmMaps[0]);
            material.SetTexture(6, detailMaps[1]);
            material.SetTexture(7, detailNrmMaps[1]);
            material.SetTexture(8, detailMaps[2]);
            material.SetTexture(9, detailNrmMaps[2]);
            material.SetTexture(10, detailMaps[3]);
            material.SetTexture(11, detailNrmMaps[3]);
        }

        public void Select()
        {
            if (!IsCollisionModelLoaded)
            {
                this.LoadCollisionModel();
            }
        }

        public void Unselect()
        {
            if (IsCollisionModelLoaded)
            {
                this.ReleaseCollisionModel();
            }
        }

        protected override ModelEffect GetTerrainEffect()
        {
            renderEffect = (TerrainRenderingEditEffect)EffectManager.Instance.GetModelEffect("TerrainRenderingEdit");
            return renderEffect;
        }

        //public override void SetTerrainTextureCache(TerrainTextureCache cache)
        //{
        //    base.SetTerrainTextureCache(cache);

        //    edTexCache = (EditableTerrainTextureCache)cache;
        //}

        public float BrushSize
        {
            get { return renderEffect.BrushSize; }
            set { renderEffect.BrushSize = value; }
        }

        public Vector2 BrushPosition
        {
            get { return renderEffect.BrushPosition; }
            set { renderEffect.BrushPosition = value; }
        }

        public bool DrawBrush
        {
            get { return renderEffect.DrawBrush; }
            set { renderEffect.DrawBrush = value; }
        }

        public override RenderOperation[] GetRenderOperation()
        {
            material.Ambient = TerrainSettings.MaterialAmbient;
            material.Diffuse = TerrainSettings.MaterialDiffuse;
            material.Emissive = TerrainSettings.MaterialEmissive;
            material.Specular = TerrainSettings.MaterialSpecular;
            material.Power = TerrainSettings.MaterialPower;
            return base.GetRenderOperation();
        }

    }
}
