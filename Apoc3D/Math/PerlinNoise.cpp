
#include "PerlinNoise.h"

#include "RandomUtils.h"

namespace Apoc3D
{
	namespace Math
	{
		FORCE_INLINE void ModulateLerpAmount(double& a)
		{
			// map to cubic S-curve
			a = a * a * (3.0 - 2.0 * a);
		}
		FORCE_INLINE double LinearInterpolate(double x, double y, double a)
		{
			// linear
			return x * (1 - a) + y * a;
		}

		//////////////////////////////////////////////////////////////////////////

		PerlinNoise::PerlinNoise()
		{ }

		PerlinNoise::PerlinNoise(double persistence, double frequency, int octaves)
			: PerlinNoise(persistence, frequency, octaves, Randomizer::Next())
		{ }

		PerlinNoise::PerlinNoise(double persistence, double frequency, int octaves, int randomseed)
			: m_persistence(persistence), m_frequency(frequency), m_octaves(octaves), 
			m_randomseed(2 + randomseed * randomseed)
		{ }

		void PerlinNoise::Set(double persistence, double frequency, int octaves, int randomseed)
		{
			m_persistence = persistence;
			m_frequency = frequency;
			m_octaves = octaves;
			m_randomseed = 2 + randomseed * randomseed;
		}


		double PerlinNoise::GetValue1D(double x) const
		{
			double t = 0;
			double amplitude = 1;
			double freq = m_frequency;
			double persistence = m_persistence;
			int seed = m_randomseed;

			for (int i = 0; i < m_octaves; i++)
			{
				t += InterpolatedNoise1D(x * freq + seed) * amplitude;
				amplitude *= persistence;
				freq *= 2;
			}

			return t;
		}
		double PerlinNoise::GetValue2D(double x, double y) const
		{
			double t = 0;
			double amplitude = 1;
			double freq = m_frequency;
			double persistence = m_persistence;
			int seed = m_randomseed;

			for (int k = 0; k < m_octaves; k++) 
			{
				t += InterpolatedNoise2D(x * freq + seed, y * freq + seed) * amplitude;
				amplitude *= persistence;
				freq *= 2;
			}

			return t;
		}
		double PerlinNoise::GetValue3D(double x, double y, double z) const
		{
			double t = 0;
			double amplitude = 1;
			double freq = m_frequency;
			double persistence = m_persistence;
			int seed = m_randomseed;

			for (int k = 0; k < m_octaves; k++)
			{
				t += InterpolatedNoise3D(x * freq + seed, y * freq + seed, z * freq + seed) * amplitude;
				amplitude *= persistence;
				freq *= 2;
			}

			return t;
		}

		double PerlinNoise::InterpolatedNoise1D(double x)
		{
			int xint = x >= 0 ? (int)x : (int)x - 1;
			double Xfrac = x - xint;
			ModulateLerpAmount(Xfrac);

			// sampling original: 
			//  v1 = Sample(x)
			//  v2 = Sample(x+1) 
			//  
			//  where Sample() is:
			//     return [x]/2 + [x-1]/4 + [x+1]/4


			double n0 = Noise1D(xint - 1);
			double n1 = Noise1D(xint);
			double n2 = Noise1D(xint + 1);
			double n3 = Noise1D(xint + 2);

			double v1 = 0.5*n1 + 0.25*n0 + 0.25*n2;  // x
			double v2 = 0.5*n2 + 0.25*n1 + 0.25*n3;  // x+1
			return LinearInterpolate(v1, v2, Xfrac);
		}

		double PerlinNoise::InterpolatedNoise2D(double x, double y)
		{
			int Xint = x >= 0 ? (int)x : (int)x - 1;
			int Yint = y >= 0 ? (int)y : (int)y - 1;

			double Xfrac = x - Xint;
			double Yfrac = y - Yint;

			// pre-modulation to prevent duplicated calculation
			ModulateLerpAmount(Xfrac);
			ModulateLerpAmount(Yfrac);

			// sampling original: 
			//  x0y0 = Sample(x, y)
			//  x1y0 = Sample(x+1, y) 
			//  x0y1 = Sample(x, y+1)
			//  x1y1 = Sample(x+1, y+1)
			//  
			//  where Sample() is:
			//     corners = ([x-1, y-1] + [x+1, y-1] + [x-1, y+1] + [x+1, y+1]) / 16.0;
			//     sides =   ([x-1, y] + [x+1, y] + [x, y-1] + [x, y+1]) / 8.0;
			//     center =   [x, y] / 4.0;
			//     return corners + sides + center;

			// expanded to reduce duplicated sampling
			// generates less instructions
			double n01 = Noise2D(Xint-1, Yint-1);
			double n02 = Noise2D(Xint+1, Yint-1);
			double n03 = Noise2D(Xint-1, Yint+1);
			double n04 = Noise2D(Xint+1, Yint+1);
			double n05 = Noise2D(Xint-1, Yint);
			double n06 = Noise2D(Xint+1, Yint);
			double n07 = Noise2D(Xint, Yint-1);
			double n08 = Noise2D(Xint, Yint+1);
			double n09 = Noise2D(Xint, Yint);

			double n12 = Noise2D(Xint+2, Yint-1);
			double n14 = Noise2D(Xint+2, Yint+1);
			double n16 = Noise2D(Xint+2, Yint);

			double n23 = Noise2D(Xint-1, Yint+2);
			double n24 = Noise2D(Xint+1, Yint+2);
			double n28 = Noise2D(Xint, Yint+2);

			double n34 = Noise2D(Xint+2, Yint+2);

			double x0y0 = 0.0625*(n01+n02+n03+n04) + 0.125*(n05+n06+n07+n08) + 0.25*(n09);
			double x1y0 = 0.0625*(n07+n12+n08+n14) + 0.125*(n09+n16+n02+n04) + 0.25*(n06);
			double x0y1 = 0.0625*(n05+n06+n23+n24) + 0.125*(n03+n04+n09+n28) + 0.25*(n08);
			double x1y1 = 0.0625*(n09+n16+n28+n34) + 0.125*(n08+n14+n06+n24) + 0.25*(n04);
			
			// bilinear
			double v1 = LinearInterpolate(x0y0, x1y0, Xfrac);
			double v2 = LinearInterpolate(x0y1, x1y1, Xfrac);
			return LinearInterpolate(v1, v2, Yfrac);
		}

		double PerlinNoise::InterpolatedNoise3D(double x, double y, double z)
		{
			int Xint = x >= 0 ? (int)x : (int)x - 1;
			int Yint = y >= 0 ? (int)y : (int)y - 1;
			int Zint = z >= 0 ? (int)z : (int)z - 1;

			double Xfrac = x - Xint;
			double Yfrac = y - Yint;
			double Zfrac = z - Zint;

			// pre-modulation to prevent duplicated calculation
			ModulateLerpAmount(Xfrac);
			ModulateLerpAmount(Yfrac);
			ModulateLerpAmount(Zfrac);

			double x0y0z0 = SampleNoise3D(Xint, Yint, Zint);
			double x1y0z0 = SampleNoise3D(Xint+1, Yint, Zint);

			double x0y1z0 = SampleNoise3D(Xint, Yint+1, Zint);
			double x1y1z0 = SampleNoise3D(Xint+1, Yint+1, Zint);

			double x0y0z1 = SampleNoise3D(Xint, Yint, Zint+1);
			double x1y0z1 = SampleNoise3D(Xint+1, Yint, Zint+1);

			double x0y1z1 = SampleNoise3D(Xint, Yint+1, Zint+1);
			double x1y1z1 = SampleNoise3D(Xint+1, Yint+1, Zint+1);

			// trilinear
			double y0z0 = LinearInterpolate(x0y0z0, x1y0z0, Xfrac);
			double y1z0 = LinearInterpolate(x0y1z0, x1y1z0, Xfrac);
			double y0z1 = LinearInterpolate(x0y0z1, x1y0z1, Xfrac);
			double y1z1 = LinearInterpolate(x0y1z1, x1y1z1, Xfrac);

			double z0 = LinearInterpolate(y0z0, y1z0, Yfrac);
			double z1 = LinearInterpolate(y0z1, y1z1, Yfrac);

			return LinearInterpolate(z0, z1, Zfrac);
		}

		double PerlinNoise::SampleNoise2D(int x, int y)
		{
			double corners = Noise2D(x-1, y-1) + Noise2D(x+1, y-1) + Noise2D(x-1, y+1) + Noise2D(x+1, y+1);
			double sides = Noise2D(x-1, y) + Noise2D(x+1, y) + Noise2D(x, y-1) + Noise2D(x, y+1);
			double center = Noise2D(x, y);
			return corners * (0.25 / 4.0) + sides * (0.5 / 4.0) + center * 0.25;
		}

		double PerlinNoise::SampleNoise3D(int x, int y, int z)
		{
			// 0.125
			double corners =
				Noise3D(x-1, y-1,z-1) + Noise3D(x+1, y-1, z-1) + Noise3D(x-1, y+1, z-1) + Noise3D(x+1, y+1, z-1) + 
				Noise3D(x-1, y-1,z+1) + Noise3D(x+1, y-1, z+1) + Noise3D(x-1, y+1, z+1) + Noise3D(x+1, y+1, z+1);

			// 0.4
			double faces =
				Noise3D(x-1, y, z) + Noise3D(x+1, y, z) + Noise3D(x, y-1, z) + 
				Noise3D(x, y+1, z) + Noise3D(x, y, z-1) + Noise3D(x, y, z+1);

			// 0.225
			double edges =
				Noise3D(x-1, y-1, z) + Noise3D(x-1, y+1, z) + Noise3D(x+1, y-1, z) + Noise3D(x+1, y+1, z) + 
				Noise3D(x, y-1, z-1) + Noise3D(x, y+1, z-1) + Noise3D(x, y-1, z+1) + Noise3D(x, y+1, z+1) +
				Noise3D(x-1, y, z-1) + Noise3D(x-1, y, z+1) + Noise3D(x+1, y, z-1) + Noise3D(x+1, y, z+1);

			// 0.25
			double center = Noise3D(x, y, z);

			return corners * (0.125 / 8.0) + faces * (0.4 / 6.0) + edges * (0.225 / 12.0) + center * 0.25;
		}


		double PerlinNoise::Noise1D(int x)
		{
			x = (x << 13) ^ x;
			return (1.0 - ((x * (x * x * 15731 + 789221) + 1376312589) & 0x7fffffff) / 1073741824.0);
		}

		double PerlinNoise::Noise2D(int x, int y)
		{
			int n = x + y * 3251;
			n = (n << 13) ^ n;
			int t = (n * (n * n * 15731 + 789221) + 1376312589) & 0x7fffffff;
			//return ( 1.0 - ( (n * (n * n * 15731 + 789221) + 1376312589) & 0x7fffffff) / 1073741824.0f);    
			
			return 1.0 - double(t) * 0.931322574615478515625e-9;/// 1073741824.0);
		}

		double PerlinNoise::Noise3D(int x, int y, int z)
		{
			int n = x + y * 3251 + z * 31147;
			n = (n << 13) ^ n;
			int t = (n * (n * n * 15731 + 789221) + 1376312589) & 0x7fffffff;

			return 1.0 - double(t) * 0.931322574615478515625e-9;
		}

	}
}