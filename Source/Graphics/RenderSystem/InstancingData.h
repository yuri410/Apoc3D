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
#ifndef APOC3D_INSTANCINGDATA_H
#define APOC3D_INSTANCINGDATA_H

#include "Common.h"
#include "Collections/FastMap.h"
#include "Graphics/GraphicsCommon.h"
#include "Graphics/PixelFormat.h"

using namespace Apoc3D::Collections;
using namespace Apoc3D::Graphics;

using namespace std;

namespace Apoc3D
{
	namespace Graphics
	{
		namespace RenderSystem
		{
			/** An interface for instancing
			*/
			class InstancingData
			{
			public:
				static const int MaxOneTimeInstances = 50;

				InstancingData(RenderDevice* device);

				/** Prepares the instancing data for a set of render operation began at beginIndex in op.
					This is called each time of drawing up to MaxOneTimeInstances instances.

					@return: the actual prepared number of render operations in this call.
				*/
				virtual int Setup(const RenderOperation* op, int count, int beginIndex) = 0;


				///** Retrieve the corresponding vertex declaration expanded with instancing vertex elements
				//*/
				//virtual VertexDeclaration* MapVertexDecl(VertexDeclaration* decl) = 0;
			private:
				RenderDevice* m_device;
				//VertexBuffer* m_instanceData;
			};
		}
	}
}

#endif