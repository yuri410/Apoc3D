/* -----------------------------------------------------------------------
 * This source file is part of Apoc3D Framework
 * 
 * Copyright (c) 2009-2018 Tao Xin
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

#ifndef D3D9VERTEXBUFFER_H
#define D3D9VERTEXBUFFER_H

#include "../D3D9Common.h"
#include "../VolatileResource.h"

#include "apoc3d/Graphics/RenderSystem/HardwareBuffer.h"
#include "apoc3d/Graphics/GraphicsCommon.h"

using namespace Apoc3D::Graphics;
using namespace Apoc3D::Graphics::RenderSystem;

namespace Apoc3D
{
	namespace Graphics
	{
		namespace D3D9RenderSystem
		{
			class D3D9VertexBuffer : public VertexBuffer, public VolatileResource
			{
			public:
				D3D9VertexBuffer(D3D9RenderDevice* device, int32 vertexCount, int32 vertexSize, BufferUsageFlags usage);
				~D3D9VertexBuffer();

				void ReleaseVolatileResource() override;
				void ReloadVolatileResource() override;

				D3DVertexBuffer* getD3DBuffer() const { return m_vertexBuffer; }

			protected:
				virtual void* lock(int offset, int size, LockMode mode) override;
				virtual void unlock() override;
			private:
				D3D9RenderDevice* m_device;
				D3DVertexBuffer* m_vertexBuffer;

				char* m_tempData;
			};
		}
	}
}

#endif