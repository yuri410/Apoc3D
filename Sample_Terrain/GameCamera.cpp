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

#include "GameCamera.h"

#include "apoc3d/Core/GameTime.h"
#include "Terrain.h"
//#include "Math/PerlinNoise.h"

namespace SampleTerrain
{
	float GameCamera::JumpVelocity = 25;
	bool GameCamera::Flying = false;
	float GameCamera::FlyingAlt = 25;

	GameCamera::GameCamera(float aspectRatio)
		: FpsCamera(aspectRatio), m_isOnGround(false), m_isSprinting(false)//m_height(100), m_fallSpeed(0)
	{
		
		//m_lastHeight = m_height;

	}
	GameCamera::~GameCamera()
	{
	}

	void GameCamera::Update(const GameTime* const time)
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
					v.X = hozV.X* m_maxVelocity;
					v.Z = hozV.Y* m_maxVelocity;

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
		Camera::Update(time);

		m_maxVelocity = 20;
		m_isSprinting = false;
	}
	void GameCamera::Sprint()
	{
		m_maxVelocity = 60;
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