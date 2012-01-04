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
#ifndef GAMECAMERA_H
#define GAMECAMERA_H

#include "TerrainCommon.h"

#include "Graphics/Camera.h"

using namespace Apoc3D::Graphics;
using namespace Apoc3D::Math;

namespace SampleTerrain
{
	

	class GameCamera : public FpsCamera
	{
	public:
		GameCamera(float aspectRatio);
		~GameCamera();


		virtual void Update(const GameTime* const time);

		void Sprint();
		void Jump();

		bool isSprinting() const { return m_isSprinting; }
	private:
		//float m_height;
		//float m_lastHeight;
		//float m_fallSpeed;
		bool m_isSprinting;
		bool m_isOnGround;
	};
	
}

#endif