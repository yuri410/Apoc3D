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

#include "Keyboard.h"

#include "apoc3d/IOLib/BinaryReader.h"
#include "apoc3d/IOLib/BinaryWriter.h"

using namespace Apoc3D::IO;

namespace Apoc3D
{
	namespace Input
	{
		Keyboard::Keyboard()
		{
			memset(m_keyState,0,sizeof(m_keyState));
		}
		Keyboard::~Keyboard()
		{

		}

		void WriteBoolArray(Apoc3D::IO::BinaryWriter* bw, const bool* arr, int32 count)
		{
			assert((count % 8) == 0);
			for (int32 i=0;i<count;i+=8)
			{
				byte bits = 0;

				for (int32 j=0;j<8 && i+j<count;j++)
					if (arr[i+j])
						bits |= (byte)(1 << j);

				bw->WriteByte((char)bits);
			}
		}

		void Keyboard::Serialize(Apoc3D::IO::BinaryWriter* bw)
		{
			int32 count = sizeof(m_keyState) / sizeof(*m_keyState);
			bw->WriteBooleanBits(m_keyState, count);

			count = sizeof(m_lastKeyState) / sizeof(*m_lastKeyState);
			bw->WriteBooleanBits(m_lastKeyState, count);
		}
		void Keyboard::Deserialize(Apoc3D::IO::BinaryReader* br)
		{
			int32 count = sizeof(m_keyState) / sizeof(*m_keyState);
			br->ReadBooleanBits(m_keyState, count);

			count = sizeof(m_lastKeyState) / sizeof(*m_lastKeyState);
			br->ReadBooleanBits(m_lastKeyState, count);
		}
	}
}