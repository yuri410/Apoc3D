/*
-----------------------------------------------------------------------------
This source file is part of Apoc3D

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

#include "GameCamera.h"

#include "Core/GameTime.h"
#include "Terrain.h"
//#include "Math/PerlinNoise.h"

namespace SampleTerrain
{
	GameCamera::GameCamera(float aspectRatio)
		: FpsCamera(aspectRatio), m_height(100)
	{
		

	}
	GameCamera::~GameCamera()
	{
	}

	void GameCamera::Update(const GameTime* const time)
	{

		float groundHeight = Terrain::GetHeightAt(_V3X(m_position), _V3Z(m_position));

		m_height = groundHeight * Terrain::HeightScale + 5;
		_V3Y(m_position) = m_height;

		FpsCamera::Update(time);

	}
	
}