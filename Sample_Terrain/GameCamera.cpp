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

		float groundHeight = Terrain::GetHeightAt(_V3X(m_position), _V3Z(m_position));

		float target = groundHeight * Terrain::HeightScale + 5;

		if ((_V3Y(m_position)-target)/time->getElapsedTime()>gravity)
		{
			m_isOnGround = false;
		}

		if ((_V3Y(m_position)>= target && !m_isOnGround) || _V3Y(m_velocity)>0)
		{
			_V3Y(m_velocity) -= time->getElapsedTime() * gravity;
			//m_fallSpeed += time->getElapsedTime() * gravity;
			//m_height -= m_fallSpeed * time->getElapsedTime();
		}
		else
		{
			_V3Y(m_velocity) = 0;
			_V3Y(m_position) = target;
			//m_fallSpeed = 0;
			m_isOnGround = true;
		}
		//_V3Y(m_position) = m_height;
		//m_lastHeight = m_height;

		
		

		{
			if (m_isOnGround)
			{
				m_velocity = Vector3Utils::Add(m_velChange, m_velocity);
			}

			Vector2 hozV = Vector2Utils::LDVector(_V3X(m_velocity), _V3Z(m_velocity));
			Vector3 v = m_velocity;

			if (Vector2Utils::LengthSquared(hozV)>1)
			{
				hozV = Vector2Utils::Normalize(hozV);
			}

			//if (m_isOnGround)
			{
				float vLen = Vector2Utils::Length(hozV);

				if (vLen > 0.05f)
				{
					if (m_isOnGround)
						vLen -= time->getElapsedTime() * 1.5f;

					hozV = Vector2Utils::Normalize(hozV);
					hozV = Vector2Utils::Multiply(hozV, vLen);
					_V3X(v) = Vector2Utils::GetX(hozV)* m_maxVelocity;
					_V3Z(v) = Vector2Utils::GetY(hozV)* m_maxVelocity;

					_V3X(m_velocity) = Vector2Utils::GetX(hozV);
					_V3Z(m_velocity) = Vector2Utils::GetY(hozV);
				}
				else
				{
					_V3X(v) = 0; _V3Z(v) = 0;

					_V3X(m_velocity) = 0;
					_V3Z(m_velocity) = 0;
				}
			}

			

			Vector3 dp = Vector3Utils::Multiply(v, time->getElapsedTime());
			m_position = Vector3Utils::Add(m_position, dp);
			m_velChange = Vector3Utils::Zero;
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
		if (m_isOnGround)
		{
			_V3Y(m_velocity) = 25.0f;
			m_isOnGround = false;
		}
	}
}