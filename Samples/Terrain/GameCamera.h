#pragma once
/*
-----------------------------------------------------------------------------
This source file is part of Apoc3D

Copyright (c) 2009+ Tao Xin

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

namespace SampleTerrain
{
	/** Represent a FPS view into the game scene, with addition function like jump and sprinting.
	 *  The velocity used consists of a normalize directional speed vector and a max velocity.
	 *  
	 *  When sprinting, the max velocity will be modified to a bigger value for faster movement, good for
	 *  keeping other logic code remains unchanged.
	 */
	class GameCamera : public FpsCamera
	{
	public:
		GameCamera(float aspectRatio);
		~GameCamera();


		virtual void Update(const GameTime* const time);

		void Sprint();
		void Jump();

		bool isSprinting() const { return m_isSprinting; }


		static float JumpVelocity;
		static bool Flying;
		static float FlyingAlt;

	private:
		//float m_height;
		//float m_lastHeight;
		//float m_fallSpeed;
		bool m_isSprinting;
		bool m_isOnGround;

	};
	
}

#endif