#pragma once
#ifndef APOC3D_MOUSE_H
#define APOC3D_MOUSE_H

/* -----------------------------------------------------------------------
 * This source file is part of Apoc3D Framework
 * 
 * Copyright (c) 2011-2017 Tao Xin
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

#include "apoc3d/ApocCommon.h"

#include "apoc3d/Math/Point.h"

using namespace Apoc3D::Core;
using namespace Apoc3D::Math;

namespace Apoc3D
{
	namespace Input
	{
		class APAPI Mouse
		{
		public:
			virtual ~Mouse();

			const Point& GetPosition() const { return m_currentPos; }
			virtual void SetPosition(const Point& loc);

			void SwapButton(bool swapped);

			int getX() const { return m_currentPos.X; }
			int getY() const { return m_currentPos.Y; }

			/** Get the difference of X between the last Update and current */
			int getDX() const { return m_currentPos.X - m_lastPosition.X; }
			int getDY() const { return m_currentPos.Y - m_lastPosition.Y; }
			int getDZ() const { return m_z - m_lastZ; }

			bool IsLeftPressed() const { return m_btnState[0] & !m_lastBtnState[0]; }		/** Check if the left button is just pressed */
			bool IsLeftUp() const { return !m_btnState[0] & m_lastBtnState[0]; }			/** Check if the left button is just released */

			bool IsMiddlePressed() const { return m_btnState[1] & !m_lastBtnState[1]; }
			bool IsMiddleUp() const { return !m_btnState[1] & m_lastBtnState[1]; }

			bool IsRightPressed() const { return m_btnState[2] & !m_lastBtnState[2]; }		/** Check if the right button is just pressed */
			bool IsRightUp() const { return !m_btnState[2] & m_lastBtnState[2]; }			/** Check if the right button is just released */
			

			bool IsLeftPressedState() const { return m_btnState[0]; }			/** Check if the left button is currently being pressing */
			bool IsLeftReleasedState() const { return !m_btnState[0]; }			/** Check if the left button is not currently being pressing */
			
			bool IsMiddlePressedState() const { return m_btnState[1]; }
			bool IsMiddleReleasedState() const { return !m_btnState[1]; }

			bool IsRightPressedState() const { return m_btnState[2]; }			/** Check if the right button is currently being pressing */
			bool IsRightReleasedState() const { return !m_btnState[2]; }		/** Check if the right button is not currently being pressing */

			virtual void Update(const AppTime* time) = 0;

			void Serialize(Apoc3D::IO::BinaryWriter& bw);
			void Deserialize(Apoc3D::IO::BinaryReader& br);

		protected:
			Mouse();

			bool m_lastBtnState[3];
			bool m_btnState[3];
			bool m_buttonSwapped = false;

			Point m_lastPosition;
			Point m_currentPos;

			int m_z = 0;
			int m_lastZ = 0;

		};

		class APAPI MouseMoveDistanceTracker
		{
		public:
			MouseMoveDistanceTracker();
			~MouseMoveDistanceTracker();

			void Reset(Mouse* mouse = nullptr);
			void Track(Mouse* mouse = nullptr);

			float getDistance() const { return m_distance; }
		private:
			float m_distance;
			Point m_startingPos;
		};
	}
}

#endif