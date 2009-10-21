using System;
using System.Collections.Generic;
using System.Drawing;
using System.Text;
using System.Windows.Forms;
using SlimDX;
using SlimDX.Direct3D9;
using VirtualBicycle.Ide.Editors.EditableObjects;
using VirtualBicycle.MathLib;
using VirtualBicycle.Scene;

namespace VirtualBicycle.Ide.Designers.WorldBuilder
{
    enum ModifyShapeMode
    {
        Higher,
        Lower,
        Smoothen
    }

    class TerrainShapeTool : WBTool
    {
        Point lastPos;

        bool isModifying;

        public TerrainShapeTool(WorldDesigner wb, EditableGameScene scene)
            : base(wb, scene)
        {
            BrushSize = 1;
        }

        public int BrushSize
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
                //if (isModifying)
                //{
                //    EditableTerrain terrain = WorldBuilder.CurrentTerrain;
                //    if (terrain != null)
                //    {
                //        terrain.UpdateCollisionModel();
                //    }
                //}

                isModifying = false;
            }
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

                        TerrainTexture disp = terrain.DisplacementMap;

                        Texture dtex = disp.GetTexture;

                        DataRectangle dr = dtex.LockRectangle(0, new Rectangle(x - halfSize, y - halfSize, BrushSize, BrushSize), LockFlags.None);
                        int pitch = dr.Pitch;
                        float* data = (float*)(dr.Data.DataPointer);

                        float[][] gaussFilter = PaintHelper.GetWeight(BrushSize);

                        switch (Mode)
                        {
                            case ModifyShapeMode.Higher:
                                for (int i = 0; i < BrushSize; i++)
                                {
                                    for (int j = 0; j < BrushSize; j++)
                                    {
                                        data[i * (pitch / sizeof(float)) + j] += gaussFilter[j][i] * invHeightScale * delta * 0.8f;
                                    }
                                }
                                break;
                            case ModifyShapeMode.Lower:
                                for (int i = 0; i < BrushSize; i++)
                                {
                                    for (int j = 0; j < BrushSize; j++)
                                    {
                                        data[i * (pitch / sizeof(float)) + j] -= gaussFilter[j][i] * invHeightScale * delta * 0.8f;
                                    }
                                }
                                break;
                            case ModifyShapeMode.Smoothen:
                                float averageHeight = 0;
                                for (int i = 0; i < BrushSize; i++)
                                {
                                    for (int j = 0; j < BrushSize; j++)
                                    {
                                        averageHeight += data[i * (pitch / sizeof(float)) + j];
                                    }
                                }

                                averageHeight /= (float)(BrushSize * BrushSize);

                                for (int i = 0; i < BrushSize; i++)
                                {
                                    for (int j = 0; j < BrushSize; j++)
                                    {
                                        int ofs = i * (pitch / sizeof(float)) + j;

                                        float diff = averageHeight - data[ofs];

                                        data[ofs] += gaussFilter[j][i] * diff;
                                    }
                                }

                                break;
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


        public ModifyShapeMode Mode
        {
            get;
            set;
        }
    }
}
