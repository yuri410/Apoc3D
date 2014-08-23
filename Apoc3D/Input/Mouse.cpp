/*
-----------------------------------------------------------------------------
This source file is part of Apoc3D Engine

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

#include "Mouse.h"

#include "apoc3d/IOLib/BinaryReader.h"
#include "apoc3d/IOLib/BinaryWriter.h"

using namespace Apoc3D::IO;

namespace Apoc3D
{
	namespace Input
	{
		Mouse::Mouse()
			: m_z(0), m_lastZ(0), m_currentPos(0,0), m_lastPosition(0,0)
		{
			memset(m_lastBtnState, 0, sizeof(m_lastBtnState));
			memset(m_btnState, 0, sizeof(m_btnState));
		}
		Mouse::~Mouse()
		{

		}

		void Mouse::SetPosition(const Point& loc)
		{
			m_currentPos = loc;
		}

		void Mouse::Serialize(Apoc3D::IO::BinaryWriter* bw)
		{
			const int32 lastCount = sizeof(m_lastBtnState) / sizeof(*m_lastBtnState);
			const int32 count = sizeof(m_btnState) / sizeof(*m_btnState);

			bool packedBools[lastCount + count];
			memcpy(packedBools, m_lastBtnState, sizeof(bool)*lastCount);
			memcpy(packedBools+lastCount, m_btnState, sizeof(bool)*count);

			bw->WriteBooleanBits(packedBools, lastCount+count);
			
			bw->WriteInt16(m_lastPosition.X);
			bw->WriteInt16(m_lastPosition.Y);
			bw->WriteInt16(m_currentPos.X);
			bw->WriteInt16(m_currentPos.Y);

			bw->WriteInt32(m_z);
			bw->WriteInt32(m_lastZ);
		}
		void Mouse::Deserialize(Apoc3D::IO::BinaryReader* br)
		{
			const int32 lastCount = sizeof(m_lastBtnState) / sizeof(*m_lastBtnState);
			const int32 count = sizeof(m_btnState) / sizeof(*m_btnState);

			bool packedBools[lastCount + count];

			br->ReadBooleanBits(packedBools, lastCount+count);

			memcpy(m_lastBtnState, packedBools, sizeof(bool)*lastCount);
			memcpy(m_btnState, packedBools+lastCount, sizeof(bool)*count);

			m_lastPosition.X = br->ReadInt16();
			m_lastPosition.Y = br->ReadInt16();
			m_currentPos.X = br->ReadInt16();
			m_currentPos.Y = br->ReadInt16();
			
			m_z = br->ReadInt32();
			m_lastZ = br->ReadInt32();
		}
	}
}