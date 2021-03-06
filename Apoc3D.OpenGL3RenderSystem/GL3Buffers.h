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

#ifndef GL3VERTEXBUFFER_H
#define GL3VERTEXBUFFER_H

#include "GL3Common.h"

#include "apoc3d/Graphics/RenderSystem/HardwareBuffer.h"
#include "apoc3d/Graphics/RenderSystem/RenderTarget.h"
#include "apoc3d/Graphics/GraphicsCommon.h"

using namespace Apoc3D::Graphics;
using namespace Apoc3D::Graphics::RenderSystem;

namespace Apoc3D
{
	namespace Graphics
	{
		namespace GL3RenderSystem
		{
			// Aka. vertex attribute buffer
			class GL3VertexBuffer : public VertexBuffer
			{
			public:
				GL3VertexBuffer(GL3RenderDevice* device, int32 vertexCount, int32 vertexSize, BufferUsageFlags usage);
				~GL3VertexBuffer();

				void Bind();

				GLuint getGLBufferID() const { return m_bufferID; }

			protected:
				virtual void* lock(int offset, int size, LockMode mode) override;
				virtual void unlock() override;

			private:
				GL3RenderDevice* m_device;
				
				GLuint m_bufferID = 0;
				bool m_isDynamic = false;

			};

			// Aka. vertex element array
			class GL3IndexBuffer : public IndexBuffer
			{
			public:
				GL3IndexBuffer(GL3RenderDevice* device, IndexBufferFormat type, int32 size, BufferUsageFlags usage);
				~GL3IndexBuffer();

				GLuint getGLBufferID() const { return m_bufferID; }

			protected:
				virtual void* lock(int offset, int size, LockMode mode) override;
				virtual void unlock() override;

			private:
				GL3RenderDevice* m_device;
				
				GLuint m_bufferID = 0;
				bool m_isDynamic = false;
			};


			class GL3DepthStencilBuffer : public DepthStencilBuffer
			{
			public:
				GL3DepthStencilBuffer(GL3RenderDevice* device, int32 width, int32 height, DepthFormat fmt);
				~GL3DepthStencilBuffer();

				GLuint getGLBufferID() const { return m_buf; }
			private:
				GLuint m_buf;
			};
		}
	}
}

#endif