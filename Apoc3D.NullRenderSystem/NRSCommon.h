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

#ifndef NRSCOMMON_H
#define NRSCOMMON_H

#include "apoc3d/ApocCommon.h"

#include "apoc3d/Meta/EventDelegate.h"

#pragma comment(lib, "Apoc3D.lib")


namespace Apoc3D
{
	namespace Graphics
	{
		namespace NullRenderSystem
		{
			class NRSRenderWindow;
			class NRSRenderView;
			class NRSDeviceContext;

			class NRSTexture;
			class NRSRenderTarget;

			class NRSRenderDevice;
			class NRSRenderStateManager;
			class NativeStateManager;

			class ConstantTable;

			class NRSVertexShader;
			class NRSPixelShader;

			class NRSDepthBuffer;
			class NRSInstancingData;
			class NRSVertexDeclaration;

			class NRSVertexBuffer;
			class NRSIndexBuffer;

		}
	}
}
#endif