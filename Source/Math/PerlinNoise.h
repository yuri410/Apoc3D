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
		public:
			// Constructor
			PerlinNoise();
			PerlinNoise(double _persistence, double _frequency, double _amplitude, int _octaves, int _randomseed);

			// Get Height
			double GetHeight(double x, double y) const;

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

		private:

			double Total(double i, double j) const;
			double GetValue(double x, double y) const;
			double Interpolate(double x, double y, double a) const;
			double Noise(int x, int y) const;

			double persistence, frequency, amplitude;
			int octaves, randomseed;
		};
	}
}

#endif