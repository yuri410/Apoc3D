#pragma once
#ifndef APOC3D_RENDERTARGET_H
#define APOC3D_RENDERTARGET_H

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
			class APAPI RenderTarget2DBase
			{
			public:
				RenderTarget2DBase(RenderDevice* device, int32 width, int32 height, const String& multiSampleMode);

				void SetPercentageLock(float wp, float hp);

				bool isMultiSampled() const { return m_isMultisampled; }
				const String& getMultisampleMode() const { return m_multisampleMode; }

				int32 getWidth() const { return m_width; }
				int32 getHeight() const { return m_height; }

				static bool IsMultisampleModeStringNone(const String& aamode);

				void* UserPointer = nullptr;
				int32 UserInt32 = 0;
			protected:
				~RenderTarget2DBase(){}

				Point EvaluatePerferredSize() const;
				bool CheckAndUpdateDimensionBasedOnLock();

				RenderDevice* m_device;

				float m_widthPercentage = 0;
				float m_heightPercentage = 0;
				bool m_hasPercentangeLock = false;

				int32 m_width;
				int32 m_height;


				bool m_isMultisampled;
				String m_multisampleMode;
			};

			/**
			 *  A render target.
			 *
			 *  Render target must have a color buffer. An additional depth buffer can be created as well.
			 *  Additional depth buffer is useful when multisample is preferred.
			 *
			 *  RTs with depth buffer should always be the primary one( at index 0).
			 */
			class APAPI RenderTarget : public RenderTarget2DBase
			{
				RTTI_BASE;
			protected:
				RenderTarget(RenderDevice* renderDevice, int32 width, int32 height, PixelFormat colorFormat, const String& multiSampleMode);

			public:
				virtual ~RenderTarget();

				PixelFormat getColorFormat() const { return m_pixelFormat; }

				virtual Texture* GetColorTexture() = 0;

				virtual void PrecacheLockedData() = 0;

				DataRectangle Lock(LockMode mode);
				DataRectangle Lock(LockMode mode, const Apoc3D::Math::Rectangle& rect);
				void Unlock();

				EventDelegate<RenderTarget*> eventReseted;

			protected:
				virtual DataRectangle lock(LockMode mode, const Apoc3D::Math::Rectangle& rect) = 0;
				virtual void unlock() = 0;

			private:
				PixelFormat m_pixelFormat;
				
				bool m_isLocked = false;
			};

			class APAPI DepthStencilBuffer : public RenderTarget2DBase
			{
				RTTI_BASE;
			public:
				virtual ~DepthStencilBuffer() { }

				DepthFormat getFormat() const { return m_depthFormat; }
				
				EventDelegate<DepthStencilBuffer*> eventReset;

			protected:
				DepthStencilBuffer(RenderDevice* renderDevice, int32 width, int32 height, DepthFormat format);
				DepthStencilBuffer(RenderDevice* renderDevice, int32 width, int32 height, DepthFormat format, const String& multiSampleMode);

				DepthFormat m_depthFormat;

			};

			class APAPI CubemapRenderTarget
			{
				RTTI_BASE;
			protected:
				CubemapRenderTarget(int32 length, PixelFormat fmt);

			public:
				virtual ~CubemapRenderTarget();

				virtual Texture* GetColorTexture() = 0;

				int32 getLength() const { return m_length; }
				PixelFormat getColorFormat() const { return m_pixelFormat; }

				RenderTarget* getSubRenderTarget(int32 i) const { return m_faces[i]; }

				EventDelegate<CubemapRenderTarget*> eventReset;

			protected:
				FixedList<RenderTarget*, CUBE_Count> m_faces;

				int32 m_length;
			private:
				PixelFormat m_pixelFormat;
			};
		}

	}
}
#endif