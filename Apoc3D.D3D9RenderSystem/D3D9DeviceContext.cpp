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

#include "D3D9DeviceContext.h"

#include "D3D9RenderDevice.h"
#include "D3D9RenderWindow.h"
#include "D3D9Utils.h"
#include "GraphicsDeviceManager.h"
#include "Enumeration.h"
#include "apoc3d/Collections/HashMap.h"
#include "apoc3d/Utility/StringUtils.h"
#include "apoc3d/Utility/Hash.h"

using namespace Apoc3D::Utility;

namespace Apoc3D
{
	namespace Graphics
	{
		namespace D3D9RenderSystem
		{
			D3D9DeviceContext::D3D9DeviceContext()
				: DeviceContext(true), m_window(0), m_viewSet(NULL)
			{
				m_d3d9 = Direct3DCreate9(D3D_SDK_VERSION);

				D3DADAPTER_IDENTIFIER9 did;
				m_d3d9->GetAdapterIdentifier(0, NULL, &did);
				m_hardwareName = StringUtils::toPlatformWideString(did.Description);
			}
			D3D9DeviceContext::~D3D9DeviceContext()
			{
				m_d3d9->Release();
				m_d3d9 = 0;
			}


			void D3D9DeviceContext::NotifyWindowClosed(D3D9RenderWindow* wnd)
			{
				if (m_window != wnd)
					m_window = NULL;
			}

			RenderView* D3D9DeviceContext::create(const RenderParameters &pm)
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
						m_window = new D3D9RenderWindow(0, this, pm);

						return m_window;
					}
				}
				// keep the compiler happy
				return NULL;
			}

			RenderDevice* D3D9DeviceContext::getRenderDevice()
			{
				if (m_window)
					return m_window->getRenderDevice();
				return nullptr;
			}

			String D3D9DeviceContext::GetHardwareName()
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

			List<RenderDisplayMode> D3D9DeviceContext::GetSupportedDisplayModes()
			{
				if (!Enumeration::hasEnumerated())
					Enumeration::Enumerate(m_d3d9);

				const List<AdapterInfo*>& adpInfos = Enumeration::getAdapters();

				HashSet<RenderDisplayMode, RenderDisplayModeEqualityComparer> modeTabls(adpInfos.getCount() * 2);
				for (const AdapterInfo* ai : adpInfos)
				{
					RenderDisplayMode mode;
					mode.AdapterIndex = ai->AdapterIndex;

					mode.GraphicsCardName = ai->GraphicsCardName;
					mode.DisplayName = ai->DisplayName;
					mode.MonitorNames = ai->MonitorNames;
					
					for (D3DDISPLAYMODE d3dmode : ai->DisplayModes)
					{
						mode.Height = d3dmode.Height;
						mode.Width = d3dmode.Width;
						mode.RefreshRate = d3dmode.RefreshRate;

						for (const DeviceInfo* di : ai->Devices)
						{
							if (di->DeviceType == D3DDEVTYPE_HAL)
							{
								for (SettingsCombo* sc : di->SettingCombos)
								{
									mode.FullScreen = !sc->Windowed;

									for (int n = 0; n < sc->MultisampleTypes.getCount(); n++)
									{
										mode.FSAASampleCount = D3D9Utils::ConvertBackMultiSample(sc->MultisampleTypes[n]);
										if (!modeTabls.Contains(mode))
											modeTabls.Add(mode);
									}
								}
							}
						}
					}
				}

				List<RenderDisplayMode> result;
				for (const RenderDisplayMode& rdm : modeTabls)
					result.Add(rdm);
				return result;
			}

		}
	}
}