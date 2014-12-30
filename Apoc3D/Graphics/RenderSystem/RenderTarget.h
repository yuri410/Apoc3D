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
#include "apoc3d/Meta/EventDelegate.h"

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
				RTTI_BASE;
			protected:
				RenderTarget(RenderDevice* renderDevice, int32 width, int32 height, PixelFormat colorFormat, DepthFormat depthFormat, const String& multiSampleMode);
				RenderTarget(RenderDevice* renderDevice, int32 width, int32 height, PixelFormat colorFormat, const String& multiSampleMode);

			public:
				virtual ~RenderTarget();
				
				void SetPercentageLock(float wp, float hp);

				bool isMultiSampled() const { return m_isMultisampled; }
				const String& getMultisampleMode() const { return m_multisampleMode; }

				int32 getWidth() const { return m_width; }
				int32 getHeight() const { return m_height; }

				DepthFormat getDepthFormat() const { return m_depthFormat; }
				PixelFormat getColorFormat() const { return m_pixelFormat; }

				virtual Texture* GetColorTexture() = 0;
				virtual DepthBuffer* GetDepthBuffer() = 0;

				virtual void PrecacheLockedData() = 0;

				DataRectangle Lock(LockMode mode);
				DataRectangle Lock(LockMode mode, const Apoc3D::Math::Rectangle& rect);
				void Unlock();

				static bool IsMultisampleModeStringNone(const String& aamode);

				EventDelegate<RenderTarget*> eventReseted;

			protected:
				virtual DataRectangle lock(LockMode mode, const Apoc3D::Math::Rectangle& rect) = 0;
				virtual void unlock() = 0;

				RenderDevice* m_device;

				float m_widthPercentage = 0;
				float m_heightPercentage = 0;
				bool m_hasPercentangeLock = false;

				int32 m_width;
				int32 m_height;

			private:
				DepthFormat m_depthFormat;
				PixelFormat m_pixelFormat;
				bool m_isMultisampled;
				String m_multisampleMode;

				bool m_isLocked = false;
			};
		}

	}
}
#endif