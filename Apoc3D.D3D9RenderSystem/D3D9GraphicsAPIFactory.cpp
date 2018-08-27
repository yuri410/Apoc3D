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

#include "D3D9GraphicsAPIFactory.h"
#include "D3D9DeviceContext.h"

namespace Apoc3D
{
	namespace Graphics
	{
		namespace D3D9RenderSystem
		{
			APIDescription D3D9GraphicsAPIFactory::GetDescription()
			{
				PlatformAPISupport platform = { 100, L"WINDOWS" };

				APIDescription desc;
				desc.Name = L"Direct3D9";
				desc.SupportedPlatforms.Add(platform);
				return desc;
			}

			DeviceContext* D3D9GraphicsAPIFactory::CreateDeviceContext()
			{
				return new D3D9DeviceContext();
			}
		}
	}
}