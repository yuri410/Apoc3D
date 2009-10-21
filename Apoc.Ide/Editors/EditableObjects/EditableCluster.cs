using System;
using System.Collections.Generic;
using System.Text;
using VirtualBicycle.Ide.Designers;
using VirtualBicycle.Logic.Traffic;
using VirtualBicycle.MathLib;
using VirtualBicycle.Scene;

namespace VirtualBicycle.Ide.Editors.EditableObjects
{
    public class EditableCluster : Cluster
    {
        public EditableCluster(EditableGameScene scene, int x, int y, float cellUnit)
            : base(null, GraphicsDevice.Instance.Device, x, y, cellUnit)
        {
            //tracks = new List<Track>();
        }

        EditableTerrain terrain;

        void FindTerrain()
        {
            List<SceneObject> objs = sceneMgr.SceneObjects;

            for (int i = 0; i < objs.Count; i++)
            {
                if (objs[i] is EditableTerrain)
                {
                    terrain = (EditableTerrain)objs[i];
                }
            }
        }

        public EditableTerrain Terrain
        {
            get
            {
                if (terrain == null)
                {
                    FindTerrain();
                }
                return terrain;
            }
            set { terrain = value; }
        }

        //public void AddPath(Track track)
        //{
        //    track.AddToScene(sceneMgr);
        //    tracks.Add(track);
        //}

        //public Track GetPath(int index)
        //{
        //    return tracks[index];
        //}

        //public void RemovePath(Track track)
        //{
        //    track.RemoveFromScene(sceneMgr);
        //    tracks.Remove(track);
        //}

        //public void RemovePath(int index)
        //{
        //    tracks[index].RemoveFromScene(sceneMgr);
        //    tracks.RemoveAt(index);
        //}
    }
}
