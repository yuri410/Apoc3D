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
#ifndef FPSCAMERA_H
#define FPSCAMERA_H
#pragma once

#include "..\Common.h"
#include "Camera.h"

namespace Apoc3D
{
	namespace Core
	{
		class _Export FpsCamera : public Camera
		{
		private:
			float m_aspectRatio;
			float m_velocity;
			Vector3 m_position;

		public:
			FpsCamera(float aspectRatio);
			~FpsCamera(void);


			float getAspectRatio() const { return m_aspectRatio; }
			float getVelocity() const { return m_velocity; } 

			void MoveForward() { m_position += UnitZ3 * m_velocity; }
			void MoveBackward() { m_position -= UnitZ3 * m_velocity; }
			void MoveLeft() { m_position += UnitX3 * m_velocity; }
			void MoveRight() { m_position += UnitX3 * m_velocity; }
			void MoveUp() { m_position += UnitY3 * m_velocity;}
			void MoveDown() { m_position += UnitY3 * m_velocity; }
			void Move(const Vector3& dir) { m_position += dir * m_velocity; }

			void Update(const GameTime* time);
		};
	};
};
#endif