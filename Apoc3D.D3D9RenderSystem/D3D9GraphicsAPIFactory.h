#pragma once
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

#ifndef D3D9GRAPHICSAPIFACTORY_H
#define D3D9GRAPHICSAPIFACTORY_H

#include "D3D9Common.h"
#include "apoc3d/Graphics/RenderSystem/GraphicsAPI.h"

using namespace Apoc3D::Graphics::RenderSystem;

namespace Apoc3D
{
	namespace Graphics
	{
		namespace D3D9RenderSystem
		{
			class D3D9GraphicsAPIFactory final : public GraphicsAPIFactory
			{
			public:
				D3D9GraphicsAPIFactory()
					: GraphicsAPIFactory(GetDescription())
				{ }

				virtual DeviceContext* CreateDeviceContext();
			private:
				static APIDescription GetDescription();

			};
		}
	}
}

#endif