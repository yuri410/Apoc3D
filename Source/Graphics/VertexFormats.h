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

#ifndef VERTEXFORMATS_H
#define VERTEXFORMATS_H

#include "Common.h"
#include "Collections/FastList.h"
#include "RenderSystem/VertexElement.h"
#include "Core/IHashed.h"
#include "Math/Vector.h"

using namespace Apoc3D::Core;
using namespace Apoc3D::Collections;
using namespace Apoc3D::Graphics;

namespace Apoc3D
{
	namespace Graphics
	{
		struct APAPI VertexPN : public IHashed
		{
		private:
			FastList<VertexElement> GetElements();
		public:
			static const FastList<VertexElement> Elements;

			Vector3 Position;
			Vector3 Normal;

			virtual HashHandle GetHashCode() const
			{
				
			}
			virtual String GetHashString() const;

		};
	}
}

#endif