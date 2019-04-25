#pragma once

/* -----------------------------------------------------------------------
 * This source file is part of Apoc3D Framework
 *
 * Copyright (c) 2011-2019 Tao Xin
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

#ifndef GL3GRAPHICSAPIFACTORY_H
#define GL3GRAPHICSAPIFACTORY_H

#include "GL3Common.h"
#include "apoc3d/Graphics/RenderSystem/GraphicsAPI.h"

using namespace Apoc3D::Graphics::RenderSystem;

namespace Apoc3D
{
	namespace Graphics
	{
		namespace GL3RenderSystem
		{
			class GL3GraphicsAPIFactory : public GraphicsAPIFactory
			{
			private:
				static APIDescription GetDescription();

			public:
				GL3GraphicsAPIFactory()
					: GraphicsAPIFactory(GetDescription())
				{ }

				virtual DeviceContext* CreateDeviceContext();
			};
		}
	}
}

#endif