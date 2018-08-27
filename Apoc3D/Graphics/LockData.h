#pragma once
#ifndef APOC3D_LOCKDATA_H
#define APOC3D_LOCKDATA_H

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

using namespace Apoc3D::VFS;
using namespace Apoc3D::Core;

namespace Apoc3D
{
	namespace Graphics
	{
		namespace RenderSystem
		{
			class APAPI DataRectangle
			{
			public:
				static const DataRectangle Empty;

				DataRectangle(int pitch, void* pointer, int width, int height, PixelFormat fmt);

				PixelFormat getFormat() const { return m_format; }
				int32 getPitch() const { return m_pitch; }
				int32 getWidth() const { return m_width; }
				int32 getHeight() const { return m_height; }
				int32 getMemorySize() const;
				bool isCompressed() const;
				void* getDataPointer() const { return m_pointer; }
				
				/**
				 *  Return whether this buffer is laid out consecutive in memory (ie the pitches
				 *  are equal to the dimensions)
				 */
				bool isConsecutive() const;

			private:
				int32 m_pitch;
				void* m_pointer;
				int32 m_width;
				int32 m_height;
				PixelFormat m_format;

			};

			class APAPI DataBox
			{
			public:
				static const DataBox Empty;

				DataBox(int width, int height, int depth, int rowPitch, int slicePitch, void* pointer, PixelFormat fmt);

				PixelFormat getFormat() const { return m_format; }
				int32 getWidth() const { return m_width; }
				int32 getHeight() const { return m_height; }
				int32 getDepth() const { return m_depth; }

				bool isCompressed() const;

				/** 
				 *  Return whether this buffer is laid out consecutive in memory (ie the pitches
				 *  are equal to the dimensions)
				 */
				bool isConsecutive() const;

				int32 getRowPitch() const { return m_rowPitch; }
				int32 getSlicePitch() const { return m_slicePitch; }
				void* getDataPointer() const { return m_pointer; }
				int32 getMemorySize() const;

			private:
				int32 m_rowPitch;
				int32 m_slicePitch;
				void* m_pointer;
				int32 m_width;
				int32 m_height;
				int32 m_depth;

				PixelFormat m_format;


			};
		}
	}
}

#endif