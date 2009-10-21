using System;
using System.Collections.Generic;
using System.Text;
using VirtualBicycle.MathLib;

namespace VirtualBicycle.Ide.Designers.WorldBuilder
{
    static class PaintHelper
    {
        struct Weight
        {
            public float[][] weights;

            public int brushSize;


            public Weight(int brushSize)
            {
                this.brushSize = brushSize;

                this.weights = MathEx.ComputeGuassFilter2D((float)Math.Sqrt(0.4 * brushSize), brushSize);

                Normalize();
            }

            void Normalize()
            {
                float maxValue = 0;
                for (int i = 0; i < brushSize; i++)
                {
                    for (int j = 0; j < brushSize; j++)
                    {
                        if (maxValue < weights[i][j])
                        {
                            maxValue = weights[i][j];
                        }
                    }
                }

                if (maxValue != 0)
                {
                    float scale = 1.0f / maxValue;

                    for (int i = 0; i < brushSize; i++)
                    {
                        for (int j = 0; j < brushSize; j++)
                        {
                            maxValue *= scale;
                        }
                    }
                }
            }
        }

        static Dictionary<int, Weight> weightTable = new Dictionary<int, Weight>();

        public static float[][] GetWeight(int brushSize)
        {
            Weight result;

            if (weightTable.TryGetValue(brushSize, out result))
            {
                return result.weights;
            }
            result = new Weight(brushSize);
            weightTable.Add(brushSize, result);
            return result.weights;
        }
    }
}
