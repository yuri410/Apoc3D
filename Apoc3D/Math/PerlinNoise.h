#pragma once
#ifndef APOC3D_PERLINNOISE_H
#define APOC3D_PERLINNOISE_H

#include "apoc3d/Common.h"

namespace Apoc3D
{
	namespace Math
	{
		class APAPI PerlinNoise
		{
		public:
			// Constructor
			PerlinNoise();
			PerlinNoise(double _persistence, double _frequency, double _amplitude, int _octaves, int _randomseed);

			// Get Height
			double GetHeight(double x, double y) const { return amplitude * Total(x, y); }
			double GetUnifiedValue(double x, double y) const { return amplitude * Total(x, y) * 0.5 + 0.5; }

			// Get
			double Persistence() const { return persistence; }
			double Frequency()   const { return frequency;   }
			double Amplitude()   const { return amplitude;   }
			int    Octaves()     const { return octaves;     }
			int    RandomSeed()  const { return randomseed;  }

			// Set
			void Set(double _persistence, double _frequency, double _amplitude, int _octaves, int _randomseed);

			void SetPersistence(double _persistence) { persistence = _persistence; }
			void SetFrequency(  double _frequency)   { frequency = _frequency;     }
			void SetAmplitude(  double _amplitude)   { amplitude = _amplitude;     }
			void SetOctaves(    int    _octaves)     { octaves = _octaves;         }
			void SetRandomSeed( int    _randomseed)  { randomseed = _randomseed;   }

			double Noise(int x, int y) const;

		private:

			double Total(double i, double j) const;
			double GetValue(double x, double y) const;
			double Interpolate(double x, double y, double a) const;
			
			double persistence, frequency, amplitude;
			int octaves, randomseed;
		};

		class APAPI PerlinNoise1D
		{
		public:
			PerlinNoise1D()
				: m_persistence(0), m_frequency(0), m_octaves(0), m_randomSeed(0)
			{ }
			PerlinNoise1D(double _persistence, double _frequency, int _octaves, int _randomseed)
				: m_persistence(_persistence), m_frequency(_frequency), m_octaves(_octaves), m_randomSeed(_randomseed)
			{ }

			// Get
			double getPersistence() const { return m_persistence; }
			double getFrequency()   const { return m_frequency;   }
			int    getOctaves()     const { return m_octaves;     }
			int    getRandomSeed()  const { return m_randomSeed;  }

			// Set
			void SetParams(double _persistence, double _frequency, int _octaves, int _randomseed)
			{
				m_persistence = _persistence; m_frequency = _frequency; m_octaves = _octaves; m_randomSeed = _randomseed;
			}


			double GetNoise(double x) const
			{
				double total = 0;
				double freq = m_frequency;
				double _amplitude = 1;

				for (int i=0;i<m_octaves;i++)
				{
					total += InterpolatedNoise(x * freq) * _amplitude;

					_amplitude *= m_persistence;
					freq *= 2;
				}
				return total;
			}
		private:
			double Interpolate(double x, double y, double a) const;
			double InterpolatedNoise(double x) const
			{
				int xint = x>=0 ? (int)x : (int)x - 1;
				double Xfrac = x - xint;

				double v1 = SmoothedNoise(xint);
				double v2 = SmoothedNoise(xint+1);
				return Interpolate(v1, v2, Xfrac);
			}
			double SmoothedNoise(int x) const
			{
				return Noise(x) / 2.0 + Noise(x-1) / 4.0 + Noise(x+1) / 4.0;
			}

			double Noise(int x) const
			{
				x = (x<<13) ^ x;
				return ( 1.0 - ( (x * (x * x * 15731 + 789221) + 1376312589) & 0x7fffffff) / 1073741824.0);
			}

			double m_persistence;
			double m_frequency;
			int m_octaves;
			int m_randomSeed;
		};
	}
}

#endif