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

#include "GameCamera.h"

#include "Terrain.h"
//#include "Math/PerlinNoise.h"

namespace SampleTerrain
{
	float GameCamera::JumpVelocity = 25;
	bool GameCamera::Flying = false;
	float GameCamera::FlyingAlt = 25;

	GameCamera::GameCamera(float aspectRatio)
		: FreeCamera(aspectRatio), m_isOnGround(false), m_isSprinting(false)//m_height(100), m_fallSpeed(0)
	{
		
		//m_lastHeight = m_height;

	}
	GameCamera::~GameCamera()
	{
	}

	void GameCamera::Update(const GameTime* time)
	{
		float gravity = 50;

		float groundHeight = Terrain::GetHeightAt(m_position.X, m_position.Z);

		if (Flying)
		{
			groundHeight = FlyingAlt;
		}

		float target = groundHeight * Terrain::HeightScale + 5;

		// check by comparing acceleration
		if ((m_position.Y-target)/time->getElapsedTime()>gravity)
		{
			m_isOnGround = false;
		}

		// apply gravity when not on ground or jumping
		if ((m_position.Y>= target && !m_isOnGround) || m_velocity.Y>0)
		{
			m_velocity.Y -= time->getElapsedTime() * gravity;
			//m_fallSpeed += time->getElapsedTime() * gravity;
			//m_height -= m_fallSpeed * time->getElapsedTime();
		}
		else
		{
			m_velocity.Y = 0;
			m_position.Y = target;
			//m_fallSpeed = 0;
			m_isOnGround = true;
		}
		//_V3Y(m_position) = m_height;
		//m_lastHeight = m_height;

		
		

		{
			// camera can change speed when on ground
			if (m_isOnGround)
			{
				m_velocity += m_velChange;
			}

			// calculate the normalized horizontal velocity, storing into hozV
			Vector2 hozV(m_velocity.X, m_velocity.Z);
			Vector3 v = m_velocity;

			if (Vector2::LengthSquared(hozV)>1)
			{
				hozV.NormalizeInPlace();
			}

			 
			//if (m_isOnGround)
			{
				float vLen = Vector2::Length(hozV);

				// if the vel is big enough, add some drag
				if (vLen > 0.05f)
				{
					if (m_isOnGround)
						vLen -= time->getElapsedTime() * 1.5f;

					hozV.NormalizeInPlace();
					hozV = Vector2::Multiply(hozV, vLen);
					v.X = hozV.X* MaxVelocity;
					v.Z = hozV.Y* MaxVelocity;

					m_velocity.X = hozV.X;
					m_velocity.Z = hozV.Y;
				}
				else
				{
					v.X = v.Z = 0;

					m_velocity.X = m_velocity.Z = 0;
				}
			}

			

			Vector3 dp = v * time->getElapsedTime();
			m_position += dp;
			m_velChange = Vector3::Zero;
		}


		UpdateTransform();
		CalculateMatrices();

		MaxVelocity = 20;
		m_isSprinting = false;
	}
	void GameCamera::Sprint()
	{
		MaxVelocity = 60;
		m_isSprinting = true;
	}
	void GameCamera::Jump()
	{
		if (Flying)
			return;

		if (m_isOnGround)
		{
			m_velocity.Y = JumpVelocity;
			m_isOnGround = false;
		}
	}
}