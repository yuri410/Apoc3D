#pragma once
#ifndef APOC3D_INSTANCINGDATA_H
#define APOC3D_INSTANCINGDATA_H

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

#include "apoc3d/Collections/HashMap.h"
#include "apoc3d/Graphics/GraphicsCommon.h"
#include "apoc3d/Graphics/PixelFormat.h"

using namespace Apoc3D::Collections;
using namespace Apoc3D::Graphics;

namespace Apoc3D
{
	namespace Graphics
	{
		namespace RenderSystem
		{
			/** An interface for instancing */
			class APAPI InstancingData
			{
			public:
				static const int32 MaxOneTimeInstances = 50;

				InstancingData(RenderDevice* device);
				virtual ~InstancingData();

				/** Prepares the instancing data for a set of render operation began at beginIndex in op.
				 *  This is called each time of drawing up to MaxOneTimeInstances instances.
				 *
				 *  @return: the actual prepared number of render operations in this call.
				 */
				virtual int32 Setup(const RenderOperation* op, int32 count, int32 beginIndex) = 0;

			private:
				RenderDevice* m_device;
				
			};
		}
	}
}

#endif