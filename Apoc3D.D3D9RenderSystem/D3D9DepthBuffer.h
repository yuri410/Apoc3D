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

#ifndef D3D9DEPTHBUFFER_H
#define D3D9DEPTHBUFFER_H

#include "D3D9Common.h"
#include "VolatileResource.h"
#include "apoc3d/Graphics/RenderSystem/RenderTarget.h"
#include "D3D9RenderDevice.h"

using namespace Apoc3D::Graphics;
using namespace Apoc3D::Graphics::RenderSystem;

namespace Apoc3D
{
	namespace Graphics
	{
		namespace D3D9RenderSystem
		{
			class D3D9DepthBuffer final : public DepthStencilBuffer, public VolatileResource
			{
				RTTI_DERIVED(D3D9DepthBuffer, DepthStencilBuffer);

			public:
				D3D9DepthBuffer(D3D9RenderDevice* device, int32 width, int32 height, DepthFormat depthFormat, const String& multisampleMode);
				D3D9DepthBuffer(D3D9RenderDevice* device, int32 width, int32 height, DepthFormat depthFormat, const D3D9Capabilities::AAProfile* aamode);

				~D3D9DepthBuffer();

				IDirect3DSurface9* getD3DBuffer() const { return m_depthSurface; }

				virtual void ReleaseVolatileResource() override;
				virtual void ReloadVolatileResource() override;

			private:
				D3D9RenderDevice* m_device;
				IDirect3DSurface9* m_depthSurface = nullptr;

				const D3D9Capabilities::AAProfile* m_explicitAAMode = nullptr;
			};
		}
	}
}

#endif