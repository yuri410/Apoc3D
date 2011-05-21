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
#ifndef RENDERDEVICE_H
#define RENDERDEVICE_H

#include "Common.h"
#include "RenderDeviceCaps.h"

namespace Apoc3D
{
	namespace Graphics
	{
		namespace RenderSystem
		{
			/* Applications use RenderDevice to perform DrawPrimitive level rendering
			*/
			class APOC3D_API RenderDevice
			{
			private:
				String m_rdName;


			protected:
				Capabilities m_caps;
				int m_batchCount;
				int m_primitiveCount;
				int m_vertexCount;

				RenderDevice(const String &renderSysName)
					: m_rdName(renderSysName)
				{

				}
			public:
				int getBatchCount() const { return m_batchCount; }
				int getPrimitiveCount() const { return m_primitiveCount; }
				int getVertexCount() const { return m_vertexCount; }

				const Capabilities* getCapabilities() const { return &m_caps; }
				const String &getRenderDeviceName() const { return m_rdName; }

				virtual void Initialize() = 0;

			};
		}
	}
}
#endif