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
#include "PerlinNoise.h"

#include "RandomUtils.h"

namespace Apoc3D
{
	namespace Math
	{
		float PerlinNoise::Frequency = 0.05f;
		float PerlinNoise::Persistency = 0.65f;
		int PerlinNoise::NumInterations = 8;
		float PerlinNoise::Amplitude = 1;
		bool PerlinNoise::Tileable = false;
		float PerlinNoise::Width = 200;
		float PerlinNoise::Height = 200;


		int PerlinNoise::rand()
		{
			return Randomizer::Next();
		}

	}
}