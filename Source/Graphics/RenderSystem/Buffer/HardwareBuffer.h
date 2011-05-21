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


#ifndef HARDWAREBUFFER_H
#define HARDWAREBUFFER_H

#pragma once

#include "Common.h"
#include "Graphics\GraphicsCommon.h"
#include "Graphics\PixelFormat.h"

namespace Apoc3D
{
	namespace Graphics
	{
		namespace RenderSystem
		{			
			class APAPI HardwareBuffer
			{
			private:
				BufferUsage m_usage;
				int m_size;
				
				bool m_isLocked;
				int m_lockOffset;
				int m_lockSize;
				bool m_useSystemMemory;

			protected:
				HardwareBuffer(BufferUsage usage, int sizeInBytes, bool useSysMem)
				{
					m_usage = usage;
					m_size = sizeInBytes;
					m_useSystemMemory = useSysMem;
				}
				virtual ~HardwareBuffer() { }

				virtual void* lock(int offset, int size, LockMode mode) = 0;
				virtual void unlock() = 0;

			public:
				BufferUsage getUsage() { return m_usage; }
				int getSize() { return m_size; } 
				int getLockOffset() { return m_lockOffset; } 
				int getLockSize() { return m_lockSize; } 
				bool UseSystemMemory() { return m_useSystemMemory; }
				bool IsLocked() { return m_isLocked; } 

				void* Lock(LockMode mode);
				void* Lock(int offset, int size, LockMode mode);
				void Unlock();
			};

			class APAPI VertexBuffer : public HardwareBuffer
			{
			protected:
				VertexBuffer(int size, BufferUsage usage, bool useSysMem)
					: HardwareBuffer(usage, size, useSysMem)
				{
				}

			public:
				
				virtual void SetData(void* data, int size) = 0;

			};

			class APAPI IndexBuffer : public HardwareBuffer
			{
			private:
				IndexBufferType m_type;

				int m_indexCount;

			protected:
				IndexBuffer(IndexBufferType type, int size, BufferUsage usage, bool useSysMem)
					: HardwareBuffer(usage, size, useSysMem)
				{
					m_type = type;
					m_indexCount = size / getIndexSize();
				}

			public:
				
				virtual void SetData(void* data, int size) = 0;

				int getIndexSize() { return m_type == IBT_Bit16 ? sizeof(ushort) : sizeof(uint); }
			};

			class APAPI DepthBuffer : public HardwareBuffer
			{
			private:
				int m_width;
				int m_height;
				DepthFormat m_depthFormat;

			public:
				int getWidth() { return m_width; }
				int getHeight() { return m_height; }
				DepthFormat getFormat() { return m_depthFormat; }

			protected:
				DepthBuffer(int width, int height, BufferUsage usage, DepthFormat format)
					: HardwareBuffer(usage,PixelFormat::GetMemorySize(width, height, format), false)
				{
					m_width = width;
					m_height = height;
					m_depthFormat = format;
				}
			};
		}		
	}
}

#endif