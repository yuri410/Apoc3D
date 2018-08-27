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

#ifndef D3D9INDEXBUFFER_H
#define D3D9INDEXBUFFER_H

#include "../D3D9Common.h"

#include "apoc3d/Graphics/RenderSystem/HardwareBuffer.h"

#include "../VolatileResource.h"

using namespace Apoc3D::Graphics;
using namespace Apoc3D::Graphics::RenderSystem;

namespace Apoc3D
{
	namespace Graphics
	{
		namespace D3D9RenderSystem
		{
			class D3D9IndexBuffer : public IndexBuffer, public VolatileResource
			{
			public:
				D3D9IndexBuffer(D3D9RenderDevice* device, IndexBufferFormat type, int32 size, BufferUsageFlags usage);
				~D3D9IndexBuffer();

				void ReleaseVolatileResource() override;
				void ReloadVolatileResource() override;

				D3DIndexBuffer* getD3DBuffer() const { return m_indexBuffer; }

			protected:
				virtual void* lock(int offset, int size, LockMode mode) override;
				virtual void unlock() override;

			private:

				D3D9RenderDevice* m_device;
				D3DIndexBuffer* m_indexBuffer;

				char* m_tempData = nullptr;
			};
		}
	}
}

#endif