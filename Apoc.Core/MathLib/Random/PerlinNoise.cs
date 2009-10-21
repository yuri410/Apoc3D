#region Using directives

using System;

#endregion

namespace Apoc.MathLib.Random
{
    /// <summary>
    /// A class to compute the perlin noise
    /// </summary>
	public sealed class PerlinNoise
	{
		#region Constants

    	/// <summary>
    	/// TODO
    	/// </summary>
		private const int _b = 0x1000;

		/// <summary>
		/// TODO
		/// </summary>
		private const int _bm = 0xff;

		/// <summary>
		/// TODO
		/// </summary>
		private const int _n = 0x1000;

		/// <summary>
		/// TODO
		/// </summary>
		private const int _np = 12;   /* 2^N */

		/// <summary>
		/// TODO
		/// </summary>
		private const int _nm = 0xfff;

		#endregion

		#region Fields

		private static int[] _p;
		private static float[,] _g3;
		private static float[,] _g2;
		private static float[] _g1;
		private static Random _r;
		private static float _frequency = 0.05f;
		private static float _persistency = 0.65f;
		private static int _numInterations = 8;
		private static float _amplitude = 1;
		private static bool _tileable = false;
		private static float _width = 200;
		private static float _height = 200;

    	#endregion

		#region Constructors

        /// <summary>
        /// Default constructor
        /// </summary>
		private PerlinNoise()
		{

			_p = new int[_b + _b + 2];
			_g3 = new float[_b + _b + 2, 3];
			_g2 = new float[_b + _b + 2, 2];
			_g1 = new float[_b + _b + 2];

			_r = new Random();

			ComputeRandomTable();
		}

		#endregion

		#region Properties

		/// <summary>
		/// TODO
		/// </summary>
		public static float Frequency
		{
			get { return PerlinNoise._frequency; }
			set { PerlinNoise._frequency = value; }
		}

		/// <summary>
		/// TODO
		/// </summary>
		public static float Persistency
		{
			get { return PerlinNoise._persistency; }
			set { PerlinNoise._persistency = value; }
		}

		/// <summary>
		/// TODO
		/// </summary>
		public static int NumInterations
		{
			get { return PerlinNoise._numInterations; }
			set { PerlinNoise._numInterations = value; }
		}

		/// <summary>
		/// TODO
		/// </summary>
		public static float Amplitude
		{
			get { return PerlinNoise._amplitude; }
			set { PerlinNoise._amplitude = value; }
		}

		/// <summary>
		/// TODO
		/// </summary>
		public static bool Tileable
		{
			get { return PerlinNoise._tileable; }
			set { PerlinNoise._tileable = value; }
		}

		/// <summary>
		/// TODO
		/// </summary>
		public static float Width
		{
			get { return PerlinNoise._width; }
			set { PerlinNoise._width = value; }
		}

		/// <summary>
		/// TODO
		/// </summary>
		public static float Height
		{
			get { return PerlinNoise._height; }
			set { PerlinNoise._height = value; }
		}

		#endregion


		#region Methods

		#region Private Implementation

		private static float sCurve(float t)
		{
			return (t * t * (3 - 2 * t));
		}
		
		private static float lerp(float t, float a, float b)
		{
			return (a + t * (b - a));
		}

    	private static void setup(float i, out int b0, out int b1, out float r0, out float r1)
		{
			float t = i + _n;
			b0 = ((int) t) & _bm;
			b1 = (b0 + 1) & _bm;
			r0 = t - (int) t;
			r1 = r0 - 1;
		}

    	private static float noise1(float x)
		{
			int bx0, bx1;
			float rx0, rx1, sx, u, v;

			setup(x, out bx0,out bx1, out rx0,out rx1);

			sx = sCurve(rx0);

			u = rx0 * _g1[_p[bx0]];
			v = rx1 * _g1[_p[bx1]];

			return lerp(sx, u, v);
		}

    	private static float at2(float rx, float ry, int i)
		{
			return (rx * _g2[i,0] + ry * _g2[i,1]);
		}

    	private static float noise2(float x, float y)
		{
			int bx0, bx1, by0, by1, b00, b10, b01, b11;
			float rx0, rx1, ry0, ry1, sx, sy, a, b, u, v;
			int i, j;

			setup(x, out bx0,out bx1, out rx0,out rx1);
			setup(y, out by0,out by1, out ry0,out ry1);

			i = _p[bx0];
			j = _p[bx1];

			b00 = _p[i + by0];
			b10 = _p[j + by0];
			b01 = _p[i + by1];
			b11 = _p[j + by1];

			sx = sCurve(rx0);
			sy = sCurve(ry0);


			u = at2(rx0,ry0,b00);
			v = at2(rx1,ry0,b10);
			a = lerp(sx, u, v);

			u = at2(rx0,ry1,b01);
			v = at2(rx1,ry1,b11);
			b = lerp(sx, u, v);

			return lerp(sy, a, b);
		}

    	private static float at3(float rx, float ry, float rz, int i)
		{
			return (rx * _g3[i,0] + ry * _g3[i,1] + rz * _g3[i,2]);
		}

    	private static float noise3(float x, float y, float z)
		{
			int bx0, bx1, by0, by1, bz0, bz1, b00, b10, b01, b11;
			float rx0, rx1, ry0, ry1, rz0, rz1, sy, sz, a, b, c, d, t, u, v;
			int i, j;

			setup(x, out bx0,out bx1, out rx0,out rx1);
			setup(y, out by0,out by1, out ry0,out ry1);
			setup(z, out bz0,out bz1, out rz0,out rz1);

			i = _p[bx0];
			j = _p[bx1];

			b00 = _p[i + by0];
			b10 = _p[j + by0];
			b01 = _p[i + by1];
			b11 = _p[j + by1];

			t  = sCurve(rx0);
			sy = sCurve(ry0);
			sz = sCurve(rz0);

			u = at3(rx0, ry0, rz0, b00 + bz0);
			v = at3(rx1, ry0, rz0, b10 + bz0);
			a = lerp(t, u, v);

			u = at3(rx0, ry1, rz0, b01 + bz0);
			v = at3(rx1, ry1, rz0, b11 + bz0);
			b = lerp(t, u, v);

			c = lerp(sy, a, b);

			u = at3(rx0, ry0, rz1, b00 + bz1);
			v = at3(rx1, ry0, rz1, b10 + bz1);
			a = lerp(t, u, v);

			u = at3(rx0, ry1, rz1, b01 + bz1);
			v = at3(rx1, ry1, rz1, b11 + bz1);
			b = lerp(t, u, v);

			d = lerp(sy, a, b);

			return lerp(sz, c, d);
		}

    	private static void normalize2(int i)
		{
			float s = 1.0f / (float)Math.Sqrt(_g2[i, 0] * _g2[i, 0] + _g2[i, 1] * _g2[i, 1]);
			_g2[i, 0] *= s;
			_g2[i, 1] *= s;
		}

    	private static void normalize3(int i)
		{
			float s = 1.0f / (float)Math.Sqrt(_g3[i, 0] * _g3[i, 0] + _g3[i, 1] * _g3[i, 1] + _g3[i, 2] * _g3[i, 2]);
			_g3[i, 0] *= s;
			_g3[i, 1] *= s;
			_g3[i, 2] *= s;
		}

		private static float turbulence2(float x, float y, float freq)
		{
			float t = 0.0f;

			do {
				t += noise2(freq * x, freq * y) / freq;
				freq *= 0.5f;
			} while (freq >= 1.0f);
	        
			return t;
		}

		private static float turbulence3(float x, float y, float z, float freq)
		{
			float t = 0.0f;

			do {
				t += noise3(freq * x, freq * y, freq * z) / freq;
				freq *= 0.5f;
			} while (freq >= 1.0f);
	        
			return t;
		}

		private static float tileableNoise1(float x, float w)
		{
			return (noise1(x)     * (w - x) +
					noise1(x - w) *      x) / w;
		}

		private static float tileableNoise2(float x, float y, float w, float h)
		{
			return (noise2(x,     y)     * (w - x) * (h - y) +
					noise2(x - w, y)     *      x  * (h - y) +
					noise2(x,     y - h) * (w - x) *      y  +
					noise2(x - w, y - h) *      x  *      y) / (w * h);
		}

		private static float tileableNoise3(float x, float y, float z, float w, float h, float d)
		{
			return (noise3(x,     y,     z)     * (w - x) * (h - y) * (d - z) +
					noise3(x - w, y,     z)     *      x  * (h - y) * (d - z) +
					noise3(x,     y - h, z)     * (w - x) *      y  * (d - z) +
					noise3(x - w, y - h, z)     *      x  *      y  * (d - z) + 
					noise3(x,     y,     z - d) * (w - x) * (h - y) *      z  +
					noise3(x - w, y,     z - d) *      x  * (h - y) *      z  +
					noise3(x,     y - h, z - d) * (w - x) *      y  *      z  +
					noise3(x - w, y - h, z - d) *      x  *      y  *      z) / (w * h * d);
		}

		private static float tileableTurbulence2(float x, float y, float w, float h, float freq)
		{
			float t = 0.0f;

			do {
				t += tileableNoise2(freq * x, freq * y, w * freq, h * freq) / freq;
				freq *= 0.5f;
			} while (freq >= 1.0f);
	        
			return t;
		}

		private static float tileableTurbulence3(float x, float y, float z, float w, float h, float d, float freq)
		{
			float t = 0.0f;

			do {
				t += tileableNoise3(freq * x, freq * y, freq * z, w * freq, h * freq, d * freq) / freq;
				freq *= 0.5f;
			} while (freq >= 1.0f);
	        
			return t;
		}

		private static int rand()
		{
			return _r.Next();
		}

		#endregion

		/// <summary>
		/// Returns a value between 0 and 1 computing the noise for this position
		/// </summary>
		/// <param name="x">x</param>
		/// <param name="y">y</param>
		/// <returns>A [0,1] value</returns>
		public static float GetNoise(int x, int y)
		{
			float total = 0;
			float curAmplitude = Amplitude;
			float freq_factor = Frequency;

			float i = (float) x, j = (float) y;

			for (int k = 0; k < NumInterations; k++)
			{
				float v;

				if (Tileable)
					v = tileableNoise2(i * freq_factor, j * freq_factor, Width * freq_factor, Height * freq_factor);

				else
					v = noise2(i * freq_factor, j * freq_factor);

				total += v * curAmplitude;
				curAmplitude *= Persistency;
				freq_factor *= 2;
			}

			total = total * 0.5f + 0.5f;

			if (total < 0) total = 0.0f;
			if (total > 1) total = 1.0f;

			return total;
		}


		/// <summary>
		/// Computes the random table
		/// </summary>
		public static void ComputeRandomTable()
		{
			int i, j, k;

			for (i = 0; i < _b; i++)
			{
				_p[i] = i;

				_g1[i] = (float)((rand() % (_b + _b)) - _b) / _b;

				for (j = 0; j < 2; j++)
					_g2[i, j] = (float)((rand() % (_b + _b)) - _b) / _b;

				normalize2(i);

				for (j = 0; j < 3; j++)
					_g3[i, j] = (float)((rand() % (_b + _b)) - _b) / _b;

				normalize3(i);
			}

			while (--i>=0)
			{
				k = _p[i];
				_p[i] = _p[j = rand() % _b];
				_p[j] = k;
			}

			for (i = 0; i < _b + 2; i++)
			{
				_p[_b + i] = _p[i];
				_g1[_b + i] = _g1[i];
				for (j = 0; j < 2; j++)
					_g2[_b + i,j] = _g2[i,j];
				for (j = 0; j < 3; j++)
					_g3[_b + i,j] = _g3[i,j];
			}
		}
    	
		#endregion
	}
}
