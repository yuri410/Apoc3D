/*
-----------------------------------------------------------------------------
This source file is part of Apoc3D Engine

Copyright (c) 2009+ Tao Xin

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