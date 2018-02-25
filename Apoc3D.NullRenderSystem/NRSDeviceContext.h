#pragma once

/* -----------------------------------------------------------------------
* This source file is part of Apoc3D Framework
*
* Copyright (c) 2009+ Tao Xin
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

#ifndef NRSDDEVICECONTEXT_H
#define NRSDDEVICECONTEXT_H

#include "NRSCommon.h"
#include "apoc3d/Graphics/RenderSystem/DeviceContext.h"

using namespace Apoc3D::Graphics::RenderSystem;

namespace Apoc3D
{
	namespace Graphics
	{
		namespace NullRenderSystem
		{
			class NRSDeviceContext : public DeviceContext
			{
			public:
				NRSDeviceContext();
				~NRSDeviceContext();

				virtual List<RenderDisplayMode> GetSupportedDisplayModes();
				virtual String GetHardwareName();
				
				virtual RenderDevice* getRenderDevice();
				
				void NotifyWindowClosed(NRSRenderWindow* wnd);

			protected:
				virtual RenderView* create(const RenderParameters &pm);
			private:
				String m_hardwareName;

				NRSRenderWindow* m_window;
			};
		}
	}
}

#endif