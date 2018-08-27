#pragma once
#ifndef APOC3D_HARDWAREBUFFER_H
#define APOC3D_HARDWAREBUFFER_H

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

#include "apoc3d/Graphics/GraphicsCommon.h"

#include "apoc3d/Graphics/PixelFormat.h"

namespace Apoc3D
{
	namespace Graphics
	{
		namespace RenderSystem
		{			
			class APAPI HardwareBuffer
			{
			public:
				
				void* Lock(LockMode mode);
				void* Lock(int offset, int size, LockMode mode);
				void Unlock();

				BufferUsageFlags getUsage() const { return m_usage; }
				int getSize() const { return m_size; }
				int getLockOffset() const { return m_lockOffset; }
				int getLockSize() const { return m_lockSize; }

				bool isLocked() const { return m_isLocked; }

			protected:
				HardwareBuffer(BufferUsageFlags usage, int sizeInBytes)
					: m_usage(usage), m_size(sizeInBytes)
				{ }
				virtual ~HardwareBuffer() { }

				virtual void* lock(int offset, int size, LockMode mode) = 0;
				virtual void unlock() = 0;

			private:
				BufferUsageFlags m_usage;
				int m_size;

				bool m_isLocked = false;
				int m_lockOffset;
				int m_lockSize;


			};

			class APAPI VertexBuffer : public HardwareBuffer
			{
			public:
				int32 getVertexCount() const { return m_vertexCount; }
				int32 getVertexSize() const { return m_vertexSize; }

			protected:
				VertexBuffer(int32 vertexCount, int32 vertexSize, BufferUsageFlags usage)
					: HardwareBuffer(usage, vertexSize*vertexCount), m_vertexCount(vertexCount), m_vertexSize(vertexSize)
				{ }

				
			private:
				int32 m_vertexCount;
				int32 m_vertexSize;
			};

			class APAPI IndexBuffer : public HardwareBuffer
			{
			public:
				IndexBufferFormat getIndexType() const { return m_type; }
				int getIndexElementSize() const { return m_type == IndexBufferFormat::Bit16 ? sizeof(ushort) : sizeof(uint); }
				int getIndexCount() const { return m_indexCount; }

			protected:
				IndexBuffer(IndexBufferFormat type, int size, BufferUsageFlags usage)
					: HardwareBuffer(usage, size)
				{
					m_type = type;
					m_indexCount = size / getIndexElementSize();
				}
			private:
				IndexBufferFormat m_type;

				int m_indexCount;

			};

		}		
	}
}

#endif