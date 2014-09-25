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

		}
		Keyboard::~Keyboard()
		{

		}

		void Keyboard::Serialize(Apoc3D::IO::BinaryWriter* bw)
		{
			bw->WriteUInt32(m_keyState.getData());
			bw->WriteUInt32(m_lastKeyState.getData());
		}
		void Keyboard::Deserialize(Apoc3D::IO::BinaryReader* br)
		{
			br->ReadUInt32(m_keyState.getData());
			br->ReadUInt32(m_lastKeyState.getData());
		}
	}
}