
/* -----------------------------------------------------------------------
 * This source file is part of Apoc3D Framework
 * 
 * Copyright (c) 2011-2018 Tao Xin
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

#include "NRSDeviceContext.h"

#include "NRSRenderDevice.h"
#include "NRSRenderWindow.h"
#include "apoc3d/Collections/HashMap.h"
#include "apoc3d/Utility/StringUtils.h"
#include "apoc3d/Utility/Hash.h"

using namespace Apoc3D::Utility;

namespace Apoc3D
{
	namespace Graphics
	{
		namespace NullRenderSystem
		{
			NRSDeviceContext::NRSDeviceContext()
				: DeviceContext(true), m_window(0), m_hardwareName(L"Null Device")
			{

			}
			NRSDeviceContext::~NRSDeviceContext()
			{
				
			}


			void NRSDeviceContext::NotifyWindowClosed(NRSRenderWindow* wnd)
			{
				if (m_window != wnd)
					m_window = NULL;
			}

			RenderView* NRSDeviceContext::create(const RenderParameters &pm)
			{
				if (m_window)
				{
					AP_EXCEPTION(ErrorID::InvalidOperation, L"Cannot create more render view when a render window has been created.");
					return nullptr;
				}

				if (!pm.IsFullForm)
				{
					// TODO
				}
				else
				{
					if (!m_window)
					{
						m_window = new NRSRenderWindow(0, this, pm);

						return m_window;
					}
				}
				// keep the compiler happy
				return NULL;
			}

			RenderDevice* NRSDeviceContext::getRenderDevice()
			{
				if (m_window)
					return m_window->getRenderDevice();
				return nullptr;
			}

			String NRSDeviceContext::GetHardwareName()
			{
				if (m_window && !m_window->getHardwareName().empty())
				{
					return m_window->getHardwareName();
				}
				return m_hardwareName; 
			}


			struct RenderDisplayModeEqualityComparer
			{
				static bool Equals(const RenderDisplayMode& x, const RenderDisplayMode& y)
				{
					return x.AdapterIndex == y.AdapterIndex &&
						x.Width == y.Width && x.Height == y.Height &&
						x.FSAASampleCount == y.FSAASampleCount &&
						x.RefreshRate == y.RefreshRate &&
						x.FullScreen == y.FullScreen;
				}
				static int64 GetHashCode(const RenderDisplayMode& obj)
				{
					FNVHash32 h;
					h.Accumulate(&obj.AdapterIndex, sizeof(obj.AdapterIndex));
					h.Accumulate(&obj.Width, sizeof(obj.Width));
					h.Accumulate(&obj.Height, sizeof(obj.Height));

					h.Accumulate(&obj.FSAASampleCount, sizeof(obj.FSAASampleCount));
					h.Accumulate(&obj.RefreshRate, sizeof(obj.RefreshRate));

					h.Accumulate(&obj.FullScreen, sizeof(obj.FullScreen));
					return h.getResult();
				}
			};

			List<RenderDisplayMode> NRSDeviceContext::GetSupportedDisplayModes()
			{
				List<RenderDisplayMode> result;
				
				RenderDisplayMode m;
				m.AdapterIndex = 0;
				m.GraphicsCardName = L"Null";
				m.DisplayName = L"Null";
				m.MonitorNames.Add(L"Null");
				m.Width = NRSRenderDevice::ResolutionX;
				m.Height = NRSRenderDevice::ResolutionY;
				m.RefreshRate = 60;
				m.FullScreen = false;
				m.FSAASampleCount = 0;

				result.Add(m);

				return result;
			}

		}
	}
}