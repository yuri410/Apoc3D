#pragma once

/* -----------------------------------------------------------------------
* This source file is part of Apoc3D Framework
*
* Copyright (c) 2009+ Tao Xin
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

#ifndef NRSRENDERTARGET_H
#define NRSRENDERTARGET_H

#include "NRSCommon.h"

#include "apoc3d/Graphics/RenderSystem/RenderTarget.h"
#include "apoc3d/Graphics/GraphicsCommon.h"
#include "NRSRenderDevice.h"

using namespace Apoc3D::Graphics;
using namespace Apoc3D::Graphics::RenderSystem;
using namespace Apoc3D::VFS;
using namespace Apoc3D::IO;
using namespace Apoc3D::Math;

namespace Apoc3D
{
	namespace Graphics
	{
		namespace NullRenderSystem
		{
			class NRSRenderTarget final : public RenderTarget
			{
				RTTI_DERIVED(NRSRenderTarget, RenderTarget);

			public:
				NRSRenderTarget(NRSRenderDevice* device, int32 width, int32 height, PixelFormat format, const String& multisampleMode);
				NRSRenderTarget(NRSRenderDevice* device, int32 width, int32 height, PixelFormat format, const NRSCapabilities::AAProfile* explicitAAMode);

				~NRSRenderTarget();

				virtual Texture* GetColorTexture() override;

				virtual void PrecacheLockedData() override;

				NRSTexture* getTextureRef() const { return m_colorWrapperTex; }
			private:

				virtual DataRectangle lock(LockMode mode, const Apoc3D::Math::Rectangle& rect) override;
				virtual void unlock() override;


				NRSRenderDevice* m_device;
				NRSTexture* m_colorWrapperTex = nullptr;


				const NRSCapabilities::AAProfile* m_explicitAAMode = nullptr;

			};

			class NRSCubemapRenderTarget final : public CubemapRenderTarget
			{
				RTTI_DERIVED(NRSCubemapRenderTarget, CubemapRenderTarget);
			public:
				class RefRenderTarget : public RenderTarget
				{
					RTTI_DERIVED(RefRenderTarget, RenderTarget);
				public:
					RefRenderTarget(NRSRenderDevice* device, int32 width, int32 height, PixelFormat fmt, NRSCubemapRenderTarget* parent);
					~RefRenderTarget();

					virtual Texture* GetColorTexture() override { assert(0); return nullptr; }

					virtual void PrecacheLockedData() override { assert(0); }

					NRSCubemapRenderTarget* getParent() const { return m_parent; }
				private:

					virtual DataRectangle lock(LockMode mode, const Apoc3D::Math::Rectangle& rect) override { assert(0); return DataRectangle(0, 0, 0, 0, FMT_Unknown); }
					virtual void unlock() override { assert(0); }

					NRSCubemapRenderTarget* m_parent;
				};

				NRSCubemapRenderTarget(NRSRenderDevice* device, int32 length, PixelFormat format);
				~NRSCubemapRenderTarget();

				virtual Texture* GetColorTexture() override;

			private:
				
				NRSRenderDevice* m_device;

				NRSTexture* m_cubeMapWrapper = nullptr;
			};
		}
	}
}
#endif