using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Text;
using VirtualBicycle.IO;
using System.IO;

namespace VirtualBicycle.Ide.Converters
{
    class DemParameters
    {
        public DemParameters()
        {
            CellSize = 90f;
            HeightScale = 5000f;

            GenereateNoise = true;
        }

        [DefaultValue(90f)]
        public float CellSize
        {
            get;
            set;
        }

        [DefaultValue(5000f)]
        public float HeightScale
        {
            get;
            set;
        }

        [DefaultValue(true)]
        public bool GenereateNoise
        {
            get;
            set;
        }
    }

    public class DemConverter : ConverterBase
    {


        public override void ShowDialog(object sender, EventArgs e)
        {
            DemConvDlg dlg = new DemConvDlg(this);
            dlg.ShowDialog();
        }

        public override void Convert(ResourceLocation source, ResourceLocation dest)
        {
            StreamReader sr = new StreamReader(source.GetStream, Encoding.Default);

            int width = -1;
            int height = -1;

            int nodVal = -9999;

            float cellSize = 0.00083333333333333f;

            float x = 0;
            float y = 0;

            int[][] heightMap = null;

            int row = 0;
            while (!sr.EndOfStream)
            {
                string line = sr.ReadLine();
                string[] v = line.Split((char[])null, StringSplitOptions.RemoveEmptyEntries);

                switch (v[0])
                {
                    case "ncols":
                        width = int.Parse(v[1]);
                        break;
                    case "nrows":
                        height = int.Parse(v[1]);

                        heightMap = new int[height][];
                        for (int i = 0; i < height; i++)
                        {
                            heightMap[i] = new int[width];
                        }
                        break;
                    case "xllcorner":
                        x = float.Parse(v[1]);
                        break;
                    case "yllcorner":
                        y = float.Parse(v[1]);
                        break;
                    case "cellsize":
                        cellSize = float.Parse(v[1]);
                        break;
                    case "NODATA_value":
                        nodVal = int.Parse(v[1]);
                        break;
                    default:
                        for (int i = 0; i < v.Length; i++)
                        {
                            heightMap[row][i] = int.Parse(v[i]);
                        }
                        row++;

                        break;
                }
            }

            sr.Close();






        }





        public override string Name
        {
            get { return DevStringTable.Instance["GUI:DemConverter"]; }
        }

        public override string[] SourceExt
        {
            get { return new string[] { ".asc", ".tif" }; }
        }

        public override string[] DestExt
        {
            get { return new string[] { ".dmp" }; }
        }

        public override string SourceDesc
        {
            get { return "Elevation Data"; }
        }

        public override string DestDesc
        {
            get { return "Displacement Map"; }
        }
    }
}
