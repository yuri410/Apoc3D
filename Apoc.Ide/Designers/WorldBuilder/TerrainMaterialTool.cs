using System;
using System.Collections.Generic;
using System.Drawing;
using System.Text;
using System.Windows.Forms;
using SlimDX;
using VirtualBicycle.Ide.Editors.EditableObjects;
using VirtualBicycle.MathLib;
using VirtualBicycle.Scene;
using SlimDX.Direct3D9;

namespace VirtualBicycle.Ide.Designers.WorldBuilder
{
    class TerrainMaterialTool : WBTool
    {
        Point lastPos;

        bool isModifying;

        public TerrainMaterialTool(WorldDesigner wb, EditableGameScene scene)
            : base(wb, scene)
        {
            BrushSize = 1;
        }

        public int BrushSize
        {
            get;
            set;
        }
        public int SelectedMaterialIndex
        {
            get;
            set;
        }

        public override void Deactivate()
        {
            base.Deactivate();

             EditableTerrain terrain = WorldBuilder.CurrentTerrain;
             if (terrain != null)
             {
                 terrain.DrawBrush = false;
             }
        }


        public override void NotifyMouseDown(MouseEventArgs e)
        {
            if (e.Button == MouseButtons.Left)
            {
                lastPos = e.Location;
                isModifying = true;
            }
        }

        public override void NotifyMouseUp(MouseEventArgs e)
        {
            if ((e.Button & MouseButtons.Left) == MouseButtons.Left)
            {
                isModifying = false;
            }
        }


        byte CByte(float value)
        {
            int v = (int)value;
            if (v > byte.MaxValue)
                return byte.MaxValue;
            return (byte)v;
        }

        public unsafe override void NotifyMouseMove(MouseEventArgs e)
        {
            Point loc = e.Location;
            int halfSize = BrushSize / 2;

            float delta = Vector2.Distance(new Vector2(loc.X, loc.Y), new Vector2(lastPos.X, lastPos.Y));
              
            LineSegment ray = WorldBuilder.GetPickRay(e.X, e.Y);
            EditableTerrain terrain = WorldBuilder.CurrentTerrain;
            if (terrain != null)
            {
                Vector3 pos;
                if (terrain.Intersects(ray, out pos))
                {
                    terrain.DrawBrush = true;
                    terrain.BrushSize = BrushSize;
                    terrain.BrushPosition = new Vector2(pos.X, pos.Z);

                    if (isModifying)
                    {
                        float invCellUnit = 1.0f / terrain.CellUnit;
                        float invHeightScale = 1.0f / terrain.HeightScale;

                        int x = (int)(pos.X * invCellUnit);
                        int y = (int)(pos.Z * invCellUnit);

                        TerrainTexture disp = terrain.IndexMap;

                        Texture dtex = disp.GetTexture;

                        DataRectangle dr = dtex.LockRectangle(0, new Rectangle(x - halfSize, y - halfSize, BrushSize, BrushSize), LockFlags.None);
                        int pitch = dr.Pitch;
                        byte* data = (byte*)(dr.Data.DataPointer);

                        float[][] gaussFilter = PaintHelper.GetWeight(BrushSize);

                        for (int i = 0; i < BrushSize; i++)
                        {
                            for (int j = 0; j < BrushSize; j++)
                            {
                                int offset = i * pitch + j * sizeof(int);
                                int offsetSM = offset + SelectedMaterialIndex;

                                data[offsetSM] = CByte(data[offsetSM] + gaussFilter[j][i] * Math.Abs(delta) * 20);

                                int total = data[offset] + data[offset + 1] + data[offset + 2] + data[offset + 3];

                                if (total != 0)
                                {
                                    float invTotal = 255.0f / (float)total;
                                    data[offset] = CByte(data[offset] * invTotal);
                                    data[offset + 1] = CByte(data[offset + 1] * invTotal);
                                    data[offset + 2] = CByte(data[offset + 2] * invTotal);
                                    data[offset + 3] = CByte(data[offset + 3] * invTotal);
                                }
                            }
                        }
                        
                        dtex.UnlockRectangle(0);
                        disp.NotifyChanged();
                    }

                    RequiresRedraw = true;
                }
                else
                {
                    terrain.DrawBrush = false;
                    terrain.BrushSize = 1;
                    terrain.BrushPosition = Vector2.Zero;
                }
            }

            lastPos = loc;
        }

        public override void NotifyMouseClick(MouseEventArgs e)
        {

        }

        public override void NotifyMouseDoubleClick(MouseEventArgs e)
        {

        }

        public override void NotifyMouseWheel(MouseEventArgs e)
        {

        }

        public override void Render()
        {
 
        }
    }
}
