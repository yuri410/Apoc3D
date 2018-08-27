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

#ifndef APOC3D_D3D9_VOLATILERESOURCE_H
#define APOC3D_D3D9_VOLATILERESOURCE_H

#include "D3D9Common.h"

namespace Apoc3D
{
	namespace Graphics
	{
		namespace D3D9RenderSystem
		{
			/** Provides an interface to release and restore those resource needed to handle when the device is lost. 
			*/
			class VolatileResource
			{
			private:
				D3D9RenderDevice* m_rawDevice;
			public:
				VolatileResource(D3D9RenderDevice* device);
				virtual ~VolatileResource();

				/** called when OnDeviceLost
				*/
				virtual void ReleaseVolatileResource() = 0;
				/** called when OnDeviceReset
				*/
				virtual void ReloadVolatileResource() = 0;
			};
		}
	}
}

#endif