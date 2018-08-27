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

#ifndef APOC3D_D3D9DEVICESETTINGS_H
#define APOC3D_D3D9DEVICESETTINGS_H

#include "D3D9Common.h"

namespace Apoc3D
{
	namespace Graphics
	{
		namespace D3D9RenderSystem
		{
			struct RawSettings
			{
				uint32 AdapterOrdinal = 0;
				D3DFORMAT AdapterFormat = D3DFMT_UNKNOWN;
				D3DDEVTYPE DeviceType = D3DDEVTYPE_HAL;

				uint32 CreationFlags = D3DCREATE_HARDWARE_VERTEXPROCESSING | D3DCREATE_FPU_PRESERVE;
				D3DPRESENT_PARAMETERS PresentParameters;

				RawSettings()
				{
					memset(&PresentParameters, 0, sizeof(PresentParameters));

					PresentParameters.BackBufferFormat = D3DFMT_UNKNOWN;
					PresentParameters.BackBufferCount = 1;
					PresentParameters.MultiSampleQuality = D3DMULTISAMPLE_NONE;
					PresentParameters.SwapEffect = D3DSWAPEFFECT_DISCARD;
					PresentParameters.EnableAutoDepthStencil = TRUE;
					PresentParameters.AutoDepthStencilFormat = D3DFMT_UNKNOWN;
					PresentParameters.Flags = D3DPRESENTFLAG_DISCARD_DEPTHSTENCIL;
					PresentParameters.PresentationInterval = D3DPRESENT_INTERVAL_DEFAULT;
					PresentParameters.Windowed = TRUE;
				}
			};


			/*struct DeviceSettings
			{
				uint32 AdapterOrdinal = 0;
				D3DFORMAT AdapterFormat = D3DFMT_UNKNOWN;

				D3DDEVTYPE DeviceType = D3DDEVTYPE_HAL;

				int32 BackBufferWidth = 0;
				int32 BackBufferHeight = 0;
				int32 BackBufferCount = 0;
				D3DFORMAT BackBufferFormat = D3DFMT_UNKNOWN;

				D3DFORMAT DepthStencilFormat = D3DFMT_UNKNOWN;

				bool Windowed = true;
				bool EnableVSync = true;
				bool Multithreaded = false;

				D3DMULTISAMPLE_TYPE MultiSampleType = D3DMULTISAMPLE_NONE;
				int MultiSampleQuality = 0;

				int32 RefreshRate = 0;


				DeviceSettings() { }
			};*/
		}
	};
};
#endif