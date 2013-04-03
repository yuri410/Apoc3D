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
#include "RenderTarget.h"

namespace Apoc3D
{
	namespace Graphics
	{
		namespace RenderSystem
		{
			RenderTarget::RenderTarget(RenderDevice* renderDevice, int32 width, int32 height, PixelFormat colorFormat, DepthFormat depthFormat, uint32 multiSampleCount)
				: m_width(width), m_height(height), m_pixelFormat(colorFormat), m_depthFormat(depthFormat), m_sampleCount(multiSampleCount)
			{

			}

			RenderTarget::RenderTarget(RenderDevice* renderDevice, int32 width, int32 height, PixelFormat colorFormat, uint32 multiSampleCount)
				: m_width(width), m_height(height), m_pixelFormat(colorFormat), m_depthFormat(DEPFMT_Count), m_sampleCount(multiSampleCount)
			{ 
			}

			RenderTarget::~RenderTarget()
			{

			}
		}
	}
}