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

#include "GL3GraphicsAPIFactory.h"
#include "GL3DeviceContext.h"

namespace Apoc3D
{
	namespace Graphics
	{
		namespace GL3RenderSystem
		{
			APIDescription GL3GraphicsAPIFactory::GetDescription()
			{
				PlatformAPISupport platform = { 60, L"WINDOWS" };

				APIDescription desc;
				desc.Name = L"OpenGL 3.1";
				desc.SupportedPlatforms.Add(platform);
				return desc;
			}

			DeviceContext* GL3GraphicsAPIFactory::CreateDeviceContext()
			{
				return new GL3DeviceContext();
			}
		}
	}
}