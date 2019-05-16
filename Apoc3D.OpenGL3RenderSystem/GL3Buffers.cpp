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

#include "GL3Buffers.h"
#include "GL3Utils.h"

namespace Apoc3D
{
	namespace Graphics
	{
		namespace GL3RenderSystem
		{

			GL3VertexBuffer::GL3VertexBuffer(GL3RenderDevice* device, int32 vertexCount, int32 vertexSize, BufferUsageFlags usage)
				: VertexBuffer(vertexCount, vertexSize, usage)
				, m_device(device)
				, m_isDynamic((usage& BU_Dynamic) != 0)
			{
				glGenBuffers(1, &m_bufferID);
				glBindBuffer(GL_ARRAY_BUFFER, m_bufferID);
				glBufferData(GL_ARRAY_BUFFER, vertexCount * vertexSize, nullptr, m_isDynamic ? GL_DYNAMIC_DRAW : GL_STREAM_DRAW);
			}
			GL3VertexBuffer::~GL3VertexBuffer()
			{
				if (m_bufferID)
				{
					glDeleteBuffers(1, m_bufferID);
					m_bufferID = 0;
				}
			}
			void* GL3VertexBuffer::lock(int offset, int size, LockMode mode)
			{
				GLbitfield access;
				todo;
				glBindBuffer(GL_ARRAY_BUFFER, m_bufferID);
				return glMapBufferRange(GL_ARRAY_BUFFER, offset, size, access);
			}
			void GL3VertexBuffer::unlock()
			{
				glBindBuffer(GL_ARRAY_BUFFER, m_bufferID);
				glUnmapBuffer(GL_ARRAY_BUFFER);
			}




			GL3IndexBuffer::GL3IndexBuffer(GL3RenderDevice* device, IndexBufferFormat type, int32 size, BufferUsageFlags usage)
				: IndexBuffer(type, size, usage)
				, m_device(device)
				, m_isDynamic((usage& BU_Dynamic) != 0)
			{
				glGenBuffers(1, &m_bufferID);
				glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_bufferID);
				glBufferData(GL_ELEMENT_ARRAY_BUFFER, size * getIndexElementSize(), nullptr, m_isDynamic ? GL_DYNAMIC_DRAW : GL_STREAM_DRAW);
			}

			GL3IndexBuffer::~GL3IndexBuffer()
			{
				if (m_bufferID)
				{
					glDeleteBuffers(1, &m_bufferID);
					m_bufferID = 0;
				}
			}

			void* GL3IndexBuffer::lock(int offset, int size, LockMode mode)
			{
				GLbitfield access;
				todo;
				glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_bufferID);
				return glMapBufferRange(GL_ELEMENT_ARRAY_BUFFER, offset, size, access);
			}
			void GL3IndexBuffer::unlock()
			{
				glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_bufferID);
				glUnmapBuffer(GL_ELEMENT_ARRAY_BUFFER);
			}

		}
	}
}