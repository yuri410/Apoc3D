#pragma once
#ifndef APOC3D_RENDERTARGET_H
#define APOC3D_RENDERTARGET_H

/**
 * -----------------------------------------------------------------------------
 * This source file is part of Apoc3D Engine
 * 
 * Copyright (c) 2009+ Tao Xin
 * 
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 * 
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 * 
 * You should have received a copy of the GNU General Public License
 * along with this program.  if not, write to the Free Software Foundation, 
 * Inc., 59 Temple Place - Suite 330, Boston, MA 02111-1307, USA, or go to
 * http://www.gnu.org/copyleft/gpl.txt.
 * 
 * -----------------------------------------------------------------------------
 */

#include "apoc3d/Graphics/LockData.h"

using namespace Apoc3D::VFS;
using namespace Apoc3D::Core;
using namespace Apoc3D::IO;
using namespace Apoc3D::Math;

namespace Apoc3D
{
	namespace Graphics
	{
		namespace RenderSystem
		{
			/**
			 *  A render target.
			 *
			 *  Render target must have a color buffer. An additional depth buffer can be created as well.
			 *  Additional depth buffer is useful when multisample is preferred.
			 *
			 *  RTs with depth buffer should always be the primary one( at index 0).
			 */
			class APAPI RenderTarget
			{
			private:
				int32 m_width;
				int32 m_height;
				DepthFormat m_depthFormat;
				PixelFormat m_pixelFormat;
				uint32 m_sampleCount;

			protected:
				RenderTarget(RenderDevice* renderDevice, int32 width, int32 height, PixelFormat colorFormat, DepthFormat depthFormat, uint32 multiSampleCount);
				RenderTarget(RenderDevice* renderDevice, int32 width, int32 height, PixelFormat colorFormat, uint32 multiSampleCount);
			public:
				virtual ~RenderTarget();
				
				uint32 getMultiSampleCount() const { return m_sampleCount; }
				int32 getWidth() const { return m_width; }
				int32 getHeight() const { return m_height; }

				DepthFormat getDepthFormat() const { return m_depthFormat; }
				PixelFormat getColorFormat() const { return m_pixelFormat; }

				virtual Texture* GetColorTexture() = 0;
				virtual DepthBuffer* GetDepthBuffer() = 0;

			};
		}

	}
}
#endif