using System;
using System.Collections.Generic;
using System.Text;

namespace Apoc.MathLib
{
    public static class PackUtils
    {
        // Methods
        private static double ClampAndRound(float value, float min, float max)
        {
            if (float.IsNaN(value))
            {
                return 0.0;
            }
            if (float.IsInfinity(value))
            {
                return (float.IsNegativeInfinity(value) ? ((double)min) : ((double)max));
            }
            if (value < min)
            {
                return (double)min;
            }
            if (value > max)
            {
                return (double)max;
            }
            return Math.Round((double)value);
        }
        //[CLSCompliant(false)]
        public static uint PackSigned(uint bitmask, float value)
        {
            float max = bitmask >> 1;
            float min = -max - 1f;
            return (((uint)((int)ClampAndRound(value, min, max))) & bitmask);
        }
        //[CLSCompliant(false)]
        public static uint PackSNorm(uint bitmask, float value)
        {
            float max = bitmask >> 1;
            value *= max;
            return (((uint)((int)ClampAndRound(value, -max, max))) & bitmask);
        }
        //[CLSCompliant(false)]
        public static uint PackUNorm(float bitmask, float value)
        {
            value *= bitmask;
            return (uint)ClampAndRound(value, 0f, bitmask);
        }
        //[CLSCompliant(false)]
        public static uint PackUnsigned(float bitmask, float value)
        {
            return (uint)ClampAndRound(value, 0f, bitmask);
        }
        //[CLSCompliant(false)]
        public static float UnpackSNorm(uint bitmask, uint value)
        {
            uint num = (uint)((bitmask + 1) >> 1);
            if ((value & num) != 0)
            {
                if ((value & bitmask) == num)
                {
                    return -1f;
                }
                value |= ~bitmask;
            }
            else
            {
                value &= bitmask;
            }
            float num2 = bitmask >> 1;
            return (((float)value) / num2);
        }
        //[CLSCompliant(false)]
        public static float UnpackUNorm(uint bitmask, uint value)
        {
            value &= bitmask;
            return (((float)value) / ((float)bitmask));
        }
    }

 

}
