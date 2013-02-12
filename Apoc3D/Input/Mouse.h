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

#ifndef APOC3D_MOUSE_H
#define APOC3D_MOUSE_H

#include "apoc3d/Common.h"

#include "apoc3d/Math/Point.h"

using namespace Apoc3D::Core;
using namespace Apoc3D::Math;

namespace Apoc3D
{
	namespace Input
	{
		class APAPI Mouse
		{
		protected:
			bool m_lastBtnState[3];
			bool m_btnState[3];

			Point m_lastPosition;
			Point m_currentPos;

			int m_z;
			int m_lastZ;

			Mouse();
		public:

			virtual ~Mouse();

			const Point& GetCurrentPosition() const { return m_currentPos; }

			int getX() const { return m_currentPos.X; }
			int getY() const { return m_currentPos.Y; }

			/** 
			 *  Get the difference of X between the last Update and current
			 */
			int getDX() const { return m_currentPos.X - m_lastPosition.X; }
			int getDY() const { return m_currentPos.Y - m_lastPosition.Y; }
			int getDZ() const { return m_z - m_lastZ; }

			/**
			 *  Check if the left button is just pressed
			 */
			bool IsLeftPressed() const { return m_btnState[0] & !m_lastBtnState[0]; }
			/**
			 *  Check if the left button is just released
			 */
			bool IsLeftUp() const { return !m_btnState[0] & m_lastBtnState[0]; }
			/**
			 *  Check if the right button is just pressed
			 */
			bool IsRightPressed() const { return m_btnState[2] & !m_lastBtnState[2]; }
			/**
			 *  Check if the right button is just released
			 */
			bool IsRightUp() const { return !m_btnState[2] & m_lastBtnState[2]; }
			
			/**
			 *  Check if the left button is currently being pressing
			 */
			bool IsLeftPressedState() const { return m_btnState[0]; }
			/**
			 *  Check if the left button is not currently being pressing
			 */
			bool IsLeftReleasedState() const { return !m_btnState[0]; }
			/**
			 *  Check if the right button is currently being pressing
			 */
			bool IsRightPressedState() const { return m_btnState[2]; }
			/**
			 *  Check if the right button is not currently being pressing
			 */
			bool IsRightReleasedState() const { return !m_btnState[2]; }

			virtual void Update(const GameTime* const time) = 0;
		};
	}
}

#endif