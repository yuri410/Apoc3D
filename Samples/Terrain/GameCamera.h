#pragma once
/* -----------------------------------------------------------------------
 * This source file is part of Apoc3D Framework
 * 
 * Copyright (c) 2009-2018 Tao Xin
 * 
 * This content of this file is subject to the terms of the Mozilla Public 
 * License v2.0. If a copy of the MPL was not distributed with this file, 
 * you can obtain one at http://mozilla.org/MPL/2.0/.
 * 
 * This program is distributed in the hope that it will be useful, 
 * WITHOUT WARRANTY OF ANY KIND; either express or implied. See the 
 * Mozilla Public License for more details.
 * 
 * ------------------------------------------------------------------------
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
	class GameCamera : public FreeCamera
	{
	public:
		GameCamera(float aspectRatio);
		~GameCamera();


		virtual void Update(const GameTime* time);

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