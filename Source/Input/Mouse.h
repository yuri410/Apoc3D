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

#ifndef MOUSE_H
#define MOUSE_H

#include "Common.h"
#include "Math/Point.h"

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
			~Mouse();
		public:
			const Point& GetCurrentPosition() const { return m_currentPos; }
			bool IsLeftPressed() const { return m_btnState[0] & !m_lastBtnState[0]; }
			bool IsLeftUp() const { return !m_btnState[0] & m_lastBtnState[0]; }
			bool IsRightPressed() const { return m_btnState[2] & !m_lastBtnState[2]; }
			bool IsRightUp() const { return !m_btnState[2] & m_lastBtnState[2]; }

			bool IsLeftReleasedState() const { return !m_btnState[0]; }
			bool IsLeftPressedState() const { m_btnState[0]; }
			virtual void Update(const GameTime* const time) = 0;
		};
	}
}

#endif