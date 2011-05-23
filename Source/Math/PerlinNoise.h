/*
-----------------------------------------------------------------------------
This source file is part of Apoc3D Engine

Copyright (c) 2009+ Tao Games

This program is free software: you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation, either version 3 of the License, or
(at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program.  if not, write to the Free Software Foundation,
Inc., 59 Temple Place - Suite 330, Boston, MA 02111-1307, USA, or go to
http://www.gnu.org/copyleft/gpl.txt.

-----------------------------------------------------------------------------
*/
#ifndef PERLINNOISE_H
#define PERLINNOISE_H

#include "Common.h"

namespace Apoc3D
{
	namespace Math
	{
		class APAPI PerlinNoise
		{
		private:
			static const int _b = 0x1000;
			static const int _bm = 0xff;
			static const int _n = 0x1000;
			static const int _np = 12;
			static const int _nm = 0xfff;

			static int _p[_b+_b+2];
			static float _g3[(_b+_b+2)*3];
			static float _g2[(_b+_b+2)*2];
			static float _g1[(_b+_b+2)*1];
		public:
			
			static float Frequency;
			static float Persistency;
			static int NumInterations;
			static float Amplitude;
			static bool Tileable;
			static float Width;
			static float Height;



		private:
			static float sCurve(float t)
			{
				return (t * t * (3 - 2 * t));
			}

			static float lerp(float t, float a, float b)
			{
				return (a + t * (b - a));
			}

			static void setup(float i, int& b0, int& b1, float& r0, float& r1)
			{
				float t = i + _n;
				b0 = ((int) t) & _bm;
				b1 = (b0 + 1) & _bm;
				r0 = t - (int) t;
				r1 = r0 - 1;
			}

			static float noise1(float x)
			{
				int bx0, bx1;
				float rx0, rx1, sx, u, v;

				setup(x, bx0, bx1, rx0,rx1);

				sx = sCurve(rx0);

				u = rx0 * _g1[_p[bx0]];
				v = rx1 * _g1[_p[bx1]];

				return lerp(sx, u, v);
			}

			static float at2(float rx, float ry, int i)
			{
				return (rx * _g2[i,0] + ry * _g2[i,1]);
			}

			static float noise2(float x, float y)
			{
				int bx0, bx1, by0, by1, b00, b10, b01, b11;
				float rx0, rx1, ry0, ry1, sx, sy, a, b, u, v;
				int i, j;

				setup(x, bx0, bx1, rx0,rx1);
				setup(y, by0, by1, ry0,ry1);

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

			static float at3(float rx, float ry, float rz, int i)
			{
				return (rx * _g3[i,0] + ry * _g3[i,1] + rz * _g3[i,2]);
			}

			static float noise3(float x, float y, float z)
			{
				int bx0, bx1, by0, by1, bz0, bz1, b00, b10, b01, b11;
				float rx0, rx1, ry0, ry1, rz0, rz1, sy, sz, a, b, c, d, t, u, v;
				int i, j;

				setup(x, bx0,bx1, rx0,rx1);
				setup(y, by0,by1, ry0,ry1);
				setup(z, bz0,bz1, rz0,rz1);

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

			static void normalize2(int i)
			{
				float s = 1.0f / sqrtf(_g2[i, 0] * _g2[i, 0] + _g2[i, 1] * _g2[i, 1]);
				_g2[i, 0] *= s;
				_g2[i, 1] *= s;
			}

			static void normalize3(int i)
			{
				float s = 1.0f / sqrtf(_g3[i, 0] * _g3[i, 0] + _g3[i, 1] * _g3[i, 1] + _g3[i, 2] * _g3[i, 2]);
				_g3[i, 0] *= s;
				_g3[i, 1] *= s;
				_g3[i, 2] *= s;
			}

			static float turbulence2(float x, float y, float freq)
			{
				float t = 0.0f;

				do {
					t += noise2(freq * x, freq * y) / freq;
					freq *= 0.5f;
				} while (freq >= 1.0f);

				return t;
			}

			static float turbulence3(float x, float y, float z, float freq)
			{
				float t = 0.0f;

				do {
					t += noise3(freq * x, freq * y, freq * z) / freq;
					freq *= 0.5f;
				} while (freq >= 1.0f);

				return t;
			}

			static float tileableNoise1(float x, float w)
			{
				return (noise1(x)     * (w - x) +
					noise1(x - w) *      x) / w;
			}

			static float tileableNoise2(float x, float y, float w, float h)
			{
				return (noise2(x,     y)     * (w - x) * (h - y) +
					noise2(x - w, y)     *      x  * (h - y) +
					noise2(x,     y - h) * (w - x) *      y  +
					noise2(x - w, y - h) *      x  *      y) / (w * h);
			}

			static float tileableNoise3(float x, float y, float z, float w, float h, float d)
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

			static float tileableTurbulence2(float x, float y, float w, float h, float freq)
			{
				float t = 0.0f;

				do {
					t += tileableNoise2(freq * x, freq * y, w * freq, h * freq) / freq;
					freq *= 0.5f;
				} while (freq >= 1.0f);

				return t;
			}

			static float tileableTurbulence3(float x, float y, float z, float w, float h, float d, float freq)
			{
				float t = 0.0f;

				do {
					t += tileableNoise3(freq * x, freq * y, freq * z, w * freq, h * freq, d * freq) / freq;
					freq *= 0.5f;
				} while (freq >= 1.0f);

				return t;
			}

			static inline int rand();


		public:
			/** Returns a value between 0 and 1 computing the noise for this position
			*/
			static float GetNoise(int x, int y)
			{
				float total = 0;
				float curAmplitude = Amplitude;
				float freq_factor = Frequency;

				float i = (float) x, j = (float) y;

				for (int k = 0; k < NumInterations; k++)
				{
					float v;

					if (Tileable)
					{
						v = tileableNoise2(i * freq_factor, j * freq_factor, Width * freq_factor, Height * freq_factor);
					}
					else
					{
						v = noise2(i * freq_factor, j * freq_factor);
					}

					total += v * curAmplitude;
					curAmplitude *= Persistency;
					freq_factor *= 2;
				}

				total = total * 0.5f + 0.5f;

				if (total < 0) total = 0.0f;
				if (total > 1) total = 1.0f;

				return total;
			}

			/** Computes the random table
			*/
			static void ComputeRandomTable()
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
		};
	}
}

#endif