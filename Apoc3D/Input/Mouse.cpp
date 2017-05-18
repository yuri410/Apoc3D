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

#include "Mouse.h"

#include "apoc3d/IOLib/BinaryReader.h"
#include "apoc3d/IOLib/BinaryWriter.h"
#include "InputAPI.h"

using namespace Apoc3D::IO;

namespace Apoc3D
{
	namespace Input
	{
		Mouse::Mouse()
		{
			ZeroArray(m_lastBtnState);
			ZeroArray(m_btnState);
		}
		Mouse::~Mouse()
		{

		}

		void Mouse::SwapButton(bool swapped)
		{
			m_buttonSwapped = swapped;
		}

		void Mouse::SetPosition(const Point& loc)
		{
			m_currentPos = loc;
		}

		void Mouse::Serialize(Apoc3D::IO::BinaryWriter& bw)
		{
			const int32 lastCount = sizeof(m_lastBtnState) / sizeof(*m_lastBtnState);
			const int32 count = sizeof(m_btnState) / sizeof(*m_btnState);

			bool packedBools[lastCount + count + 1];
			memcpy(packedBools, m_lastBtnState, sizeof(bool)*lastCount);
			memcpy(packedBools+lastCount, m_btnState, sizeof(bool)*count);
			packedBools[lastCount + count] = m_buttonSwapped;

			bw.WriteBooleanBits(packedBools, lastCount+count);
			
			bw.WriteInt16(m_lastPosition.X);
			bw.WriteInt16(m_lastPosition.Y);
			bw.WriteInt16(m_currentPos.X);
			bw.WriteInt16(m_currentPos.Y);

			bw.WriteInt32(m_z);
			bw.WriteInt32(m_lastZ);
		}
		void Mouse::Deserialize(Apoc3D::IO::BinaryReader& br)
		{
			const int32 lastCount = sizeof(m_lastBtnState) / sizeof(*m_lastBtnState);
			const int32 count = sizeof(m_btnState) / sizeof(*m_btnState);

			bool packedBools[lastCount + count + 1];

			br.ReadBooleanBits(packedBools, countof(packedBools));

			memcpy(m_lastBtnState, packedBools, sizeof(bool)*lastCount);
			memcpy(m_btnState, packedBools + lastCount, sizeof(bool)*count);
			m_buttonSwapped = packedBools[lastCount + count];

			m_lastPosition.X = br.ReadInt16();
			m_lastPosition.Y = br.ReadInt16();
			m_currentPos.X = br.ReadInt16();
			m_currentPos.Y = br.ReadInt16();

			m_z = br.ReadInt32();
			m_lastZ = br.ReadInt32();
		}

		/************************************************************************/
		/*  MouseMoveDistanceTracker                                            */
		/************************************************************************/

		MouseMoveDistanceTracker::MouseMoveDistanceTracker() { Reset(); }
		MouseMoveDistanceTracker::~MouseMoveDistanceTracker() { }

		void MouseMoveDistanceTracker::Reset(Mouse* mouse)
		{
			m_distance = 0;

			if (mouse == nullptr)
			{
				mouse = InputAPIManager::getSingleton().getMouse();
			}

			m_startingPos = mouse->GetPosition();
		}
		void MouseMoveDistanceTracker::Track(Mouse* mouse)
		{
			if (mouse == nullptr)
			{
				mouse = InputAPIManager::getSingleton().getMouse();
			}

			float di = Point::Distance(m_startingPos, mouse->GetPosition());
			m_distance += di;
			m_startingPos = mouse->GetPosition();
		}
	}
}