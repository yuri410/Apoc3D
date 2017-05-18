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

		void Keyboard::Serialize(Apoc3D::IO::BinaryWriter& bw)
		{
			bw.WriteUInt32(m_keyState.getData());
			bw.WriteUInt32(m_lastKeyState.getData());
		}
		void Keyboard::Deserialize(Apoc3D::IO::BinaryReader& br)
		{
			br.ReadUInt32(m_keyState.getData());
			br.ReadUInt32(m_lastKeyState.getData());
		}

	
	}
}