using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Drawing;
using System.IO;
using System.Text;
using SlimDX;
using SlimDX.Direct3D9;
using VirtualBicycle.Ide.Designers;
using VirtualBicycle.Ide.Designers.WorldBuilder;
using VirtualBicycle;
using VirtualBicycle.Graphics;
using VirtualBicycle.IO;
using VirtualBicycle.Logic;
using VirtualBicycle.Scene;

namespace VirtualBicycle.Ide.Editors.EditableObjects
{
    public class EditableSceneData : SceneDataBase
    {
        public EditableSceneData(InGameObjectManager mgr)
            : base(GraphicsDevice.Instance.Device, mgr)
        {
        }

        public static void CreateNewScene(int width, int height, string file)
        {
            Device device = GraphicsDevice.Instance.Device;

            //int cw = (width - 1) / Cluster.ClusterLength;
            //int ch = (height - 1) / Cluster.ClusterLength;


            EditableSceneData result = new EditableSceneData(null);

            EditableTerrainSettings tdata = new EditableTerrainSettings();

            tdata.SetHeightScale(MaxTerrainHeight);
            tdata.SetMaterialAmbient(new Color4(0.2f, 0.2f, 0.2f));
            tdata.SetMaterialDiffuse(new Color4(0.9f, 0.9f, 0.9f));
            tdata.SetMaterialSpecular(new Color4(0.6f, 0.6f, 0.6f));
            tdata.SetMaterialEmissive(new Color4(0, 0, 0, 0));
            tdata.SetMaterialPower(16f);

            result.TerrainSettings = tdata;

            result.CellUnit = 1f;

            List<Terrain> terrains = new List<Terrain>();

            EditableTerrain terrain = new EditableTerrain(device, result.CellUnit, tdata);
            terrain.LoadEmptyTerrain();
            terrains.Add(terrain);

            result.sceneObjects = new SceneObject[1];
            result.sceneObjects[0] = terrain;

            result.SceneSize = width;


            //EditableCluster[] clusters = new EditableCluster[cw * ch];

            //for (int x = 0; x < cw; x++)
            //{
            //    for (int y = 0; y < ch; y++)
            //    {
            //        int wx = (x == 0) ? 0 : x * 512 + 1;
            //        int wy = (y == 0) ? 0 : y * 512 + 1;

            //        int index = y * cw + x;
            //        clusters[index] = new EditableCluster(wx, wy, result.CellUnit);
            //        clusters[index].LoadEmptyCluster();
            //    }
            //}

            //result.Clusters = clusters;
            //result.ClusterTable = new EditableClusterTable(clusters);

            AtmosphereInfo info = new AtmosphereInfo();
            info.AmbientColor = new Color4(1, 0.2f, 0.2f, 0.2f);
            info.DiffuseColor = new Color4(1, 0.8f, 0.8f, 0.8f);
            info.SpecularColor = new Color4(1, 0.7f, 0.7f, 0.7f);

            info.SkyName = "Default";
            info.Weather = WeatherType.None;

            result.AtmosphereData = info;

            result.AABB = new BoundingBox(new Vector3(0, 0, 0), new Vector3(width, 2048, height));

            ToFile(result, file);

            for (int i = 0; i < terrains.Count; i++)
            {
                terrains[i].Dispose();
            }
        }

        public static EditableSceneData FromFile(DevFileLocation fl, InGameObjectManager mgr)
        {
            Device device = GraphicsDevice.Instance.Device;

            ContentBinaryReader br = new ContentBinaryReader(fl);


            if (br.ReadInt32() == FileId)
            {
                EditableSceneData result = new EditableSceneData(mgr);

                BinaryDataReader data = br.ReadBinaryData();

                result.ReadData(data, null);

                data.Close();

                return result;
            }

            br.Close();
            throw new DataFormatException();
        }

        public static void ToStream(EditableSceneData data, Stream stm)
        {
            ContentBinaryWriter bw = new ContentBinaryWriter(stm);

            bw.Write(FileId);

            BinaryDataWriter sdata = data.WriteData();
            bw.Write(sdata);
            sdata.Dispose();

            bw.Close();
        }

        public static void ToFile(EditableSceneData data, string file)
        {
            FileStream fs = new FileStream(file, FileMode.OpenOrCreate, FileAccess.Write);
            ToStream(data, fs);
        }

        //protected override EditableCluster CreateCluster(int x, int y)
        //{
        //    return new EditableCluster(this, WorldDesigner.TerrainTextureSet, WorldDesigner.TrackTextureSet, x, y);
        //}

        //[Browsable(false)]
        //public EditableClusterTable ClusterTable
        //{
        //    get;
        //    private set;
        //}
        //public override void SetTerrainTextureCache(EditableTerrainTextureCache cache)
        //{            
        //    base.SetTerrainTextureCache(cache);
        //    EditableTTexCache = texCache;

        //    for (int i = 0; i < clusters.Length; i++)
        //    {
        //        clusters[i].SetTerrainTextureCache(cache);
        //        clusters[i].Terrain.UpdateMaps();
        //    }
        //}

        public void Save(string filePath)
        {
            Save(new FileStream(filePath, FileMode.OpenOrCreate, FileAccess.Write));
        }

        public void Save(Stream stream)
        {
            ContentBinaryWriter bw = new ContentBinaryWriter(stream);

            bw.Write(FileId);

            BinaryDataWriter data = WriteData();

            bw.Write(data);
            data.Dispose();


            bw.Close();
        }

        public void SetName(string n)
        {
            this.Name = n;
        }

        public void SetDescription(string desc)
        {
            this.Description = desc;
        }

        public unsafe void Rebuild(EditableGameScene scene)
        {
            EditableClusterTable clusterTbl = scene.ClusterTable;

            List<SceneObject> sceneObjs = new List<SceneObject>(clusterTbl.Count * 100);


            foreach (EditableCluster cluster in clusterTbl)
            {
                List<SceneObject> objs = cluster.SceneManager.SceneObjects;

                for (int i = 0; i < objs.Count; i++)
                {
                    if (objs[i].IsSerializable)
                    {
                        sceneObjs.Add(objs[i]);
                    }
                }
            }

            sceneObjects = sceneObjs.ToArray();
        }
    }
}
