/*
-----------------------------------------------------------------------------
This source file is part of Apoc3D Engine

Copyright (c) 2009+ Tao Games

This program is free software: you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation, either version 3 of the License, or
(at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program.  if not, write to the Free Software Foundation, 
Inc., 59 Temple Place - Suite 330, Boston, MA 02111-1307, USA, or go to
http://www.gnu.org/copyleft/gpl.txt.

-----------------------------------------------------------------------------
*/
#include "Enumeration.h"
#include "Utility/StringUtils.h"

using namespace Apoc3D::Utility;

namespace Apoc3D
{
	namespace Graphics
	{
		namespace D3D9RenderSystem
		{
			int CompareDisplayMode(const D3DDISPLAYMODE& x, const D3DDISPLAYMODE& y)
			{
				if (x.Width > y.Width)
					return 1;
				if (x.Width < y.Width)
					return -1;
				if (x.Height > y.Height)
					return 1;
				if (x.Height < y.Height)
					return -1;
				if (x.Format > y.Format)
					return 1;
				if (x.Format < y.Format)
					return -1;
				if (x.RefreshRate > y.RefreshRate)
					return 1;
				if (x.RefreshRate < y.RefreshRate)
					return -1;

				return 0;
			}

			bool Enumeration::m_hasMinimumSettings = false;
			D3D9DeviceSettings Enumeration::m_minimumSettings;
			vector<AdapterInfo> Enumeration::m_adapters;
			bool Enumeration::m_hasEnumerated = false;

			void Enumeration::Enumerate(IDirect3D9* d3d9)
			{
				m_hasEnumerated = true;
				D3DFORMAT allowedAdapterFormat[] =  { D3DFMT_X8R8G8B8, D3DFMT_X1R5G5B5, D3DFMT_R5G6B5, 
					D3DFMT_A2R10G10B10 };

				vector<D3DFORMAT> adapterFormats;

				UINT count = d3d9->GetAdapterCount();

				for (size_t i=0;i<count;i++)
				{
					D3DADAPTER_IDENTIFIER9 adapter;
					HRESULT hr = d3d9->GetAdapterIdentifier(i, 0, &adapter);
					assert(SUCCEEDED(hr));

					AdapterInfo info = new AdapterInfo();
					info->AdapterOrdinal = i;
					info->Details = adapter;					

					for (size_t j=0;j<(sizeof(allowedAdapterFormat)/sizeof(D3DFORMAT));j++)
					{
						uint32 dispCount = d3d9->GetAdapterModeCount(i, allowedAdapterFormat[j]);

						for (size_t k=0;k<dispCount;k++)
						{
							D3DDISPLAYMODE mode;
							hr = d3d9->EnumAdapterModes(i, allowedAdapterFormat[j], k, &mode);
							assert(SUCCEEDED(hr));

							// check minimum
							if (m_hasMinimumSettings)
							{
								if (mode.Width < m_minimumSettings.getBackBufferWidth() ||
									mode.Height < m_minimumSettings.getBackBufferHeight())
								{
									continue;
								}
							}

							info.DisplayModes.push_back(mode);

							if (find(adapterFormats.begin(), adapterFormats.end(), mode.Format) == adapterFormats.end())
							{
								adapterFormats.push_back(mode.Format);
							}
						}
					}

					D3DDISPLAYMODE currentMode;
					hr = d3d9->GetAdapterDisplayMode(i, &currentMode);
					assert(SUCCEEDED(hr));

					if (find(adapterFormats.begin(), adapterFormats.end(), currentMode.Format) == adapterFormats.end())
					{
						adapterFormats.push_back(currentMode.Format);
					}

					// sort info.DisplayModes
					sort(info.DisplayModes.begin(), info.DisplayModes.end(), CompareDisplayMode);

					EnumerateDevices(d3d9, info, adapterFormats);

					if (info.Devices.size())
					{
						m_adapters.push_back(info);
					}
				}
				
				bool unique = false;
				for (size_t i=0;i<m_adapters.size() && !unique; i++)
				{
					for (size_t j=i+1;j<m_adapters.size();j++)
					{
						if (!strcmp(m_adapters[i].Details.Description, m_adapters[j].Details.Description))
						{
							unique = false;
							break;
						}
					}
				}
				for (size_t i=0;i<m_adapters.size(); i++)
				{
					wchar_t buffer[512];
					mbstowcs(buffer, m_adapters[i].Details.Description, 512);
					m_adapters[i].Description = buffer;
					
					if (!unique)
					{
						String temp = StringUtils::ToString(m_adapters[i].AdapterOrdinal);
						m_adapters[i].Description.append(temp);
					}
				}
			}

			void Enumeration::EnumerateDevices(IDirect3D9* d3d9, AdapterInfo* info, vector<D3DFORMAT>& adapterFormats)
			{
				D3DDEVTYPE devTypes[] = { D3DDEVTYPE_HAL, D3DDEVTYPE_SW };

				for (size_t i=0;i<sizeof(devTypes)/sizeof(D3DDEVTYPE);i++)
				{
					if (m_hasMinimumSettings && m_minimumSettings.DeviceType != devTypes[i])
					{
						continue;
					}

					DeviceInfo* devInfo = new DeviceInfo();
					devInfo->DeviceType = devTypes[i];

					HRESULT hr = d3d9->GetDeviceCaps(info->AdapterOrdinal, devTypes[i], &devInfo->Capabilities);
					assert(SUCCEEDED(hr));
					
					EnumerateSettingsCombos(d3d9, info, devInfo, adapterFormats);

					if (devInfo->DeviceSettings.size())
					{
						info->Devices.push_back(devInfo);
					}
				}
			}

			void Enumeration::EnumerateSettingsCombos(IDirect3D9* d3d9, AdapterInfo* adapterInfo, 
				DeviceInfo* deviceInfo, vector<D3DFORMAT>& adapterFormats)
			{
				D3DFORMAT backBufferFormats[] = { D3DFMT_A8R8G8B8, D3DFMT_X8R8G8B8, D3DFMT_A2R10G10B10,
					D3DFMT_R5G6B5, D3DFMT_A1R5G5B5, D3DFMT_X1R5G5B5 };

				for (size_t i=0;i<adapterFormats.size();i++)
				{
					for (size_t j=0;j<sizeof(backBufferFormats)/sizeof(D3DFORMAT);i++)
					{
						for (int windowed = 0; windowed < 2; windowed++)
						{
							if (windowed == 0 && adapterInfo->DisplayModes.empty())
								continue;

							HRESULT hr = d3d9->CheckDeviceType(adapterInfo->AdapterOrdinal, deviceInfo->DeviceType,
								adapterFormats[i], backBufferFormats[j], (windowed == 1) ? TRUE : FALSE);

							if (!FAILED(hr))
								continue;
							

							hr = d3d9->CheckDeviceFormat(adapterInfo->AdapterOrdinal, deviceInfo->DeviceType,
								adapterFormats[i], D3DUSAGE_QUERY_POSTPIXELSHADER_BLENDING, D3DRTYPE_TEXTURE, backBufferFormats[j]);
							
							if (!FAILED(hr))
								continue;


							SettingsCombo* combo = new SettingsCombo();
							combo->AdapterOrdinal = adapterInfo->AdapterOrdinal;
							combo->DeviceType = deviceInfo->DeviceType;
							combo->AdapterFormat = adapterFormats[i];
							combo->BackBufferFormat = backBufferFormats[j];
							combo->Windowed = (windowed == 1);
							combo->AdapterInfo = adapterInfo;
							combo->DeviceInfo = deviceInfo;

							BuildDepthStencilFormatList(combo);
							BuildMultisampleTypeList(combo);

							if (combo->MultisampleTypes.empty())
								continue;

							if (m_hasMinimumSettings)
							{
								if (m_minimumSettings.PresentParams.BackBufferFormat != D3DFMT_UNKNOWN &&
									m_minimumSettings.PresentParams.BackBufferFormat != backBufferFormats[j])
								{
									continue;
								}

								// check depth stencil format against D3DFMT_UNKNOWN
								if (m_minimumSettings.DepthStencilFormat != D3DFMT_UNKNOWN &&
									find(combo->DepthStencilFormats.begin(), 
									combo->DepthStencilFormats.end(), 
									m_minimumSettings.DepthStencilFormat) == combo->DepthStencilFormats.end())
								{
									continue;
								}

								if (find(combo->MultisampleTypes.begin(), 
									combo->MultisampleTypes.end(), 
									m_minimumSettings.PresentParams.MultiSampleType)
									== combo->MultisampleTypes.end())
								{
									continue;
								}
							}

							deviceInfo->DeviceSettings.push_back(combo);
						}
					}
				}
			}

			void Enumeration::BuildDepthStencilFormatList(IDirect3D9* d3d9, SettingsCombo* combo)
			{
				// check every possible depth stencil format see if passes. Add valid one into the list

				D3DFORMAT possibleDepthStencilFormats[] = { D3DFMT_D16, D3DFMT_D15S1, D3DFMT_D24X8,
					D3DFMT_D24S8, D3DFMT_D24X4S4, D3DFMT_D32 };

				for (size_t i=0; i<sizeof(possibleDepthStencilFormats)/sizeof(D3DFORMAT); i++)
				{
					const D3DFORMAT& format = possibleDepthStencilFormats[i];
					HRESULT hr = d3d9->CheckDeviceFormat(
						combo->AdapterOrdinal, combo->DeviceType, combo->AdapterFormat, 
						D3DUSAGE_DEPTHSTENCIL, D3DRTYPE_SURFACE, format);

					if (FAILED(hr))
						continue;

					hr = d3d9->CheckDepthStencilMatch(
						combo->AdapterOrdinal, combo->DeviceType, combo->AdapterFormat, combo->BackBufferFormat, format);

					if (FAILED(hr))
					{
						continue;
					}
					
					combo->DepthStencilFormats.push_back(format);
				}
			}

			void Enumeration::BuildMultisampleTypeList(IDirect3D9* d3d9, SettingsCombo* combo)
			{
				D3DMULTISAMPLE_TYPE possibleMultisampleTypes[] = {
					D3DMULTISAMPLE_NONE,			D3DMULTISAMPLE_NONMASKABLE,
					D3DMULTISAMPLE_2_SAMPLES,		D3DMULTISAMPLE_3_SAMPLES,
					D3DMULTISAMPLE_4_SAMPLES,		D3DMULTISAMPLE_5_SAMPLES,
					D3DMULTISAMPLE_6_SAMPLES,		D3DMULTISAMPLE_7_SAMPLES,
					D3DMULTISAMPLE_8_SAMPLES,		D3DMULTISAMPLE_9_SAMPLES,
					D3DMULTISAMPLE_10_SAMPLES,		D3DMULTISAMPLE_11_SAMPLES,
					D3DMULTISAMPLE_12_SAMPLES,		D3DMULTISAMPLE_13_SAMPLES,
					D3DMULTISAMPLE_14_SAMPLES,		D3DMULTISAMPLE_15_SAMPLES,
					D3DMULTISAMPLE_16_SAMPLES
				};

				DWORD quality;
				for (size_t i=0;i<sizeof(possibleMultisampleTypes)/sizeof(D3DMULTISAMPLE_TYPE);i++)
				{
					const D3DMULTISAMPLE_TYPE& type = possibleMultisampleTypes[i];
					HRESULT hr = d3d9->CheckDeviceMultiSampleType(combo->AdapterOrdinal, combo->DeviceType, 
						combo->AdapterFormat, combo->Windowed, type, &quality);

					if (SUCCEEDED(hr))
					{
						combo->MultisampleTypes.push_back(type);
						combo->MultisampleQualities.push_back(quality);
					}
				}

			}

		}
	}
}