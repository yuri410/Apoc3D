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

#include "LockData.h"


namespace Apoc3D
{
	namespace Graphics
	{
		namespace RenderSystem
		{
			const DataRectangle DataRectangle::Empty = DataRectangle(0, 0, 0, 0, FMT_Unknown);
			const DataBox DataBox::Empty = DataBox(0, 0, 0, 0, 0, 0, FMT_Unknown);


			/************************************************************************/
			/*   DataRectangle                                                      */
			/************************************************************************/

			DataRectangle::DataRectangle(int pitch, void* pointer, int width, int height, PixelFormat fmt)
				: m_pitch(pitch), m_pointer(pointer), m_width(width), m_height(height), m_format(fmt)
			{ }

			int32 DataRectangle::getMemorySize() const
			{
				return PixelFormatUtils::GetMemorySize(m_width, m_height, 1, m_format);
			}
			bool DataRectangle::isCompressed() const
			{
				return PixelFormatUtils::IsCompressed(m_format);
			}

			bool DataRectangle::isConsecutive() const
			{
				return m_pitch == m_width;
			}

			/************************************************************************/
			/*   DataBox                                                            */
			/************************************************************************/


			DataBox::DataBox(int width, int height, int depth, int rowPitch, int slicePitch, void* pointer, PixelFormat fmt)
				: m_rowPitch(rowPitch), m_slicePitch(slicePitch), m_pointer(pointer), m_format(fmt),
				m_width(width), m_height(height), m_depth(depth) 
			{ }

			bool DataBox::isConsecutive() const
			{
				return m_rowPitch == m_width && m_slicePitch == (m_width*m_height);
			}

			int32 DataBox::getMemorySize() const
			{
				return PixelFormatUtils::GetMemorySize(m_width, m_height, m_depth, m_format);
			}

			bool DataBox::isCompressed() const
			{
				return PixelFormatUtils::IsCompressed(m_format);
			}

		}
	}
}