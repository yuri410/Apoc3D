#pragma once
#ifndef APOC3D_RENDERWINDOWHANDLER_H
#define APOC3D_RENDERWINDOWHANDLER_H

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

#include "apoc3d/ApocCommon.h"

using namespace Apoc3D::Core;

namespace Apoc3D
{
	namespace Graphics
	{
		namespace RenderSystem
		{
			/**
			 *  An interface for client application to handle RenderWindow's events.
			 *  The engine will call back the implemented methods upon the raise of the event.
			 */
			class APAPI RenderWindowHandler
			{
			public:
				virtual ~RenderWindowHandler() { }

				virtual void Initialize() = 0;
				virtual void Finalize() = 0;

				virtual void Load() = 0;
				virtual void Unload() = 0;
				virtual void Update(const AppTime* time) = 0;
				virtual void UpdateConstrainedVarTimeStep(const AppTime* time) { }
				virtual void Draw(const AppTime* time) = 0;

				virtual void OnFrameStart() = 0;
				virtual void OnFrameEnd() = 0;

				
			};
		}
	}
}


#endif