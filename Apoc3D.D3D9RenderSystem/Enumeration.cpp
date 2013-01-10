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

#include "Apoc3DException.h"

using namespace Apoc3D::Utility;
using namespace Apoc3D;

namespace Apoc3D
{
	namespace Graphics
	{
		namespace D3D9RenderSystem
		{

			bool CompareDisplayMode(const D3DDISPLAYMODE& x, const D3DDISPLAYMODE& y)
			{
				if (x.Width > y.Width)
					return true;//1;
				if (x.Width < y.Width)
					return false;//-1;
				if (x.Height > y.Height)
					return true;//1;
				if (x.Height < y.Height)
					return false;//-1;
				if (x.Format > y.Format)
					return true;//1;
				if (x.Format < y.Format)
					return false;//-1;
				if (x.RefreshRate > y.RefreshRate)
					return true;//1;
				if (x.RefreshRate < y.RefreshRate)
					return false;//-1;

				return false;
			}

			bool Enumeration::m_hasMinimumSettings = false;
			DeviceSettings Enumeration::m_minimumSettings;
			vector<AdapterInfo*> Enumeration::m_adapters;
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

					AdapterInfo* info = new AdapterInfo();
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
								if ((int)mode.Width < m_minimumSettings.BackBufferWidth ||
									(int)mode.Height < m_minimumSettings.BackBufferHeight ||
									(int)mode.RefreshRate < m_minimumSettings.RefreshRate)
								{
									continue;
								}
							}

							info->DisplayModes.push_back(mode);

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
					sort(info->DisplayModes.begin(), info->DisplayModes.end(), CompareDisplayMode);

					EnumerateDevices(d3d9, info, adapterFormats);

					if (info->Devices.size())
					{
						m_adapters.push_back(info);
					}
				}
				
				bool unique = false;
				for (size_t i=0;i<m_adapters.size() && !unique; i++)
				{
					for (size_t j=i+1;j<m_adapters.size();j++)
					{
						if (!strcmp(m_adapters[i]->Details.Description, m_adapters[j]->Details.Description))
						{
							unique = false;
							break;
						}
					}
				}
				for (size_t i=0;i<m_adapters.size(); i++)
				{
					wchar_t buffer[512];
					mbstowcs(buffer, m_adapters[i]->Details.Description, 512);
					m_adapters[i]->Description = buffer;
					
					if (!unique)
					{
						String temp = StringUtils::ToString(m_adapters[i]->AdapterOrdinal);
						m_adapters[i]->Description.append(temp);
					}
				}
			}

			void Enumeration::EnumerateDevices(IDirect3D9* d3d9, AdapterInfo* info, vector<D3DFORMAT>& adapterFormats)
			{
				D3DDEVTYPE devTypes[] = { D3DDEVTYPE_HAL, D3DDEVTYPE_REF };

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
					for (size_t j=0;j<sizeof(backBufferFormats)/sizeof(D3DFORMAT);j++)
					{
						for (int windowed = 0; windowed < 2; windowed++)
						{
							if (windowed == 0 && adapterInfo->DisplayModes.empty())
								continue;

							HRESULT hr = d3d9->CheckDeviceType(adapterInfo->AdapterOrdinal, deviceInfo->DeviceType,
								adapterFormats[i], backBufferFormats[j], (windowed == 1) ? TRUE : FALSE);

							if (FAILED(hr))
								continue;
							

							hr = d3d9->CheckDeviceFormat(adapterInfo->AdapterOrdinal, deviceInfo->DeviceType,
								adapterFormats[i], D3DUSAGE_QUERY_POSTPIXELSHADER_BLENDING, D3DRTYPE_TEXTURE, backBufferFormats[j]);
							
							if (FAILED(hr))
								continue;


							SettingsCombo* combo = new SettingsCombo();
							combo->AdapterOrdinal = adapterInfo->AdapterOrdinal;
							combo->DeviceType = deviceInfo->DeviceType;
							combo->AdapterFormat = adapterFormats[i];
							combo->BackBufferFormat = backBufferFormats[j];
							combo->Windowed = (windowed == 1);
							combo->AdapterInfo = adapterInfo;
							combo->DeviceInfo = deviceInfo;

							BuildDepthStencilFormatList(d3d9, combo);
							BuildMultisampleTypeList(d3d9, combo);

							if (combo->MultisampleTypes.empty())
								continue;

							if (m_hasMinimumSettings)
							{
								if (m_minimumSettings.BackBufferFormat != D3DFMT_UNKNOWN &&
									m_minimumSettings.BackBufferFormat != backBufferFormats[j])
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
									m_minimumSettings.MultiSampleType)
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

			void Enumeration::BuildPresentIntervalList(IDirect3D9* d3d9, SettingsCombo* combo)
			{
				DWORD possiblePresentIntervals[] = {
					D3DPRESENT_INTERVAL_IMMEDIATE,  D3DPRESENT_INTERVAL_DEFAULT,
					D3DPRESENT_INTERVAL_ONE,        D3DPRESENT_INTERVAL_TWO,
					D3DPRESENT_INTERVAL_THREE,      D3DPRESENT_INTERVAL_FOUR
				};

				for (size_t i=0;i<sizeof(possiblePresentIntervals)/sizeof(DWORD);i++)
				{
					const DWORD& interval = possiblePresentIntervals[i];

					if (combo->Windowed && (interval == D3DPRESENT_INTERVAL_TWO || 
						interval == D3DPRESENT_INTERVAL_THREE || interval == D3DPRESENT_INTERVAL_FOUR))
					{
						continue;
					}

					if (interval == D3DPRESENT_INTERVAL_DEFAULT || (combo->DeviceInfo->Capabilities.PresentationIntervals & interval) != 0)
					{
						combo->PresentIntervals.push_back(interval);
					}
				}

			}

			
			void Enumeration::FindValidSettings(IDirect3D9* d3d9, const DeviceSettings& settings, DeviceSettings& result)
			{
				if (!m_hasEnumerated)
				{
					Enumerate(d3d9);
				}
				DeviceSettings newSettings = DeviceSettings(settings);

				Direct3D9Settings optimal;
				BuildOptimalSettings(d3d9, settings, optimal);

				float bestRanking = -1.0f;
				const SettingsCombo* bestCombo = 0;
				for (size_t i=0; i<m_adapters.size(); i++)
				{
					const AdapterInfo* adapterInfo = m_adapters[i];

					D3DDISPLAYMODE desktopMode;
					HRESULT hr = d3d9->GetAdapterDisplayMode(adapterInfo->AdapterOrdinal, &desktopMode);
					assert(SUCCEEDED(hr));

					for (size_t j=0; j<adapterInfo->Devices.size(); j++)
					{
						const DeviceInfo* deviceInfo = adapterInfo->Devices[j];
						for (size_t k=0; k<deviceInfo->DeviceSettings.size(); k++)
						{
							const SettingsCombo* combo = deviceInfo->DeviceSettings[k];

							if (combo->Windowed && combo->AdapterFormat == desktopMode.Format)
								continue;

							float ranking = RankSettingsCombo(combo, optimal, desktopMode);
							if (ranking>bestRanking)
							{
								bestCombo = combo;
								bestRanking = ranking;
							}
						}
					}
				}

				if (!bestCombo)
				{
					throw Apoc3DException::createException(EX_NotSupported, L"Can not create Direct3D9 Device. No compatible Direct3D9 devices found.");
				}
				Direct3D9Settings inteResult;
				BuildValidSettings(bestCombo, optimal, inteResult);

				result = settings;
				result.D3D9 = inteResult;
			}

			int GetDepthBits(D3DFORMAT format)
			{
				switch (format)
				{
				case D3DFMT_D32F_LOCKABLE:
				case D3DFMT_D32:
					return 32;

				case D3DFMT_D24X8:
				case D3DFMT_D24S8:
				case D3DFMT_D24X4S4:
				case D3DFMT_D24FS8:
					return 24;
					
				case D3DFMT_D16_LOCKABLE:
				case D3DFMT_D16:
					return 16;

				case D3DFMT_D15S1:
					return 15;

				default:
					return 0;
				}
			}
			int GetStencilBits(D3DFORMAT format)
			{
				switch (format)
				{
				case D3DFMT_D15S1:
					return 1;

				case D3DFMT_D24X4S4:
					return 4;

				case D3DFMT_D24S8:
				case D3DFMT_D24FS8:
					return 8;

				default:
					return 0;
				}
			}
			int GetColorBits(D3DFORMAT format)
			{
				switch (format)
				{
				case D3DFMT_R8G8B8:
				case D3DFMT_A8R8G8B8:
				case D3DFMT_A8B8G8R8:
				case D3DFMT_X8R8G8B8:
					return 8;

				case D3DFMT_R5G6B5:
				case D3DFMT_X1R5G5B5:
				case D3DFMT_A1R5G5B5:
					return 5;

				case D3DFMT_A4R4G4B4:
				case D3DFMT_X4R4G4B4:
					return 4;

				case D3DFMT_R3G3B2:
				case D3DFMT_A8R3G3B2:
					return 2;

				case D3DFMT_A2R10G10B10:
				case D3DFMT_A2B10G10R10:
					return 10;

				case D3DFMT_A16B16G16R16:
					return 16;

				default:
					return 0;
				}
			}

			void Enumeration::BuildOptimalSettings(IDirect3D9* d3d9, 
				const DeviceSettings& settings, Direct3D9Settings& optimal)
			{
				D3DDISPLAYMODE desktopMode;
				HRESULT hr = d3d9->GetAdapterDisplayMode(0, &desktopMode);
				assert(SUCCEEDED(hr));


				optimal.AdapterOrdinal = settings.AdapterOrdinal;
				optimal.DeviceType = settings.DeviceType;
				optimal.PresentParameters.Windowed = settings.Windowed;
				optimal.PresentParameters.BackBufferCount = settings.BackBufferCount;
				optimal.PresentParameters.MultiSampleType = settings.MultiSampleType;
				optimal.PresentParameters.MultiSampleQuality = settings.MultiSampleQuality;
				optimal.PresentParameters.FullScreen_RefreshRateInHz = settings.RefreshRate;

				if (settings.Multithreaded)
					optimal.CreationFlags |= D3DCREATE_MULTITHREADED;

				if (optimal.PresentParameters.Windowed || GetColorBits(desktopMode.Format) >= 8)
					optimal.AdapterFormat = desktopMode.Format;
				else
					optimal.AdapterFormat = D3DFMT_X8R8G8B8;

				if (settings.BackBufferWidth == 0 || settings.BackBufferHeight == 0)
				{
					if (optimal.PresentParameters.Windowed)
					{
						optimal.PresentParameters.BackBufferWidth = 640;
						optimal.PresentParameters.BackBufferHeight = 480;
					}
					else
					{
						optimal.PresentParameters.BackBufferWidth = desktopMode.Width;
						optimal.PresentParameters.BackBufferHeight = desktopMode.Height;
					}
				}
				else
				{
					optimal.PresentParameters.BackBufferWidth = settings.BackBufferWidth;
					optimal.PresentParameters.BackBufferHeight = settings.BackBufferHeight;
				}

				if (settings.BackBufferFormat == D3DFMT_UNKNOWN)
					optimal.PresentParameters.BackBufferFormat = optimal.AdapterFormat;
				else
					optimal.PresentParameters.BackBufferFormat = settings.BackBufferFormat;

				if (settings.DepthStencilFormat == D3DFMT_UNKNOWN)
				{
					if (GetColorBits(optimal.PresentParameters.BackBufferFormat) >= 8)
						optimal.PresentParameters.AutoDepthStencilFormat = D3DFMT_D32;
					else
						optimal.PresentParameters.AutoDepthStencilFormat = D3DFMT_D16;
				}
				else
					optimal.PresentParameters.AutoDepthStencilFormat = settings.DepthStencilFormat;

				if (!settings.EnableVSync)
					optimal.PresentParameters.PresentationInterval = D3DPRESENT_INTERVAL_IMMEDIATE;
			}

			float Enumeration::RankSettingsCombo(const SettingsCombo* combo, 
				const Direct3D9Settings& optimal, const D3DDISPLAYMODE& desktopMode)
			{
				float ranking = 0.0f;

				if(combo->AdapterOrdinal == (int32)optimal.AdapterOrdinal)
					ranking += 1000.0f;

				if(combo->DeviceType == optimal.DeviceType)
					ranking += 100.0f;

				if(combo->DeviceType == D3DDEVTYPE_HAL)
					ranking += 0.1f;

				if ((combo->Windowed && optimal.PresentParameters.Windowed) || 
					(!combo->Windowed && !optimal.PresentParameters.Windowed))
					ranking += 10.0f;

				if(combo->AdapterFormat == optimal.AdapterFormat)
					ranking += 1.0f;
				else
				{
					int bitDepthDelta = abs(GetColorBits(combo->AdapterFormat) -
						GetColorBits(optimal.AdapterFormat));
					float scale = max(0.9f - bitDepthDelta * 0.2f, 0.0f);
					ranking += scale;
				}
				
				if(!(combo->Windowed))
				{
					bool match;
					if(GetColorBits(desktopMode.Format) >= 8)
						match = (combo->AdapterFormat == desktopMode.Format);
					else
						match = (combo->AdapterFormat == D3DFMT_X8R8G8B8);

					if(match)
						ranking += 0.1f;
				}

				if((optimal.CreationFlags & D3DCREATE_HARDWARE_VERTEXPROCESSING) != 0 &&
					(optimal.CreationFlags & D3DCREATE_MIXED_VERTEXPROCESSING) != 0)
				{
					if((combo->DeviceInfo->Capabilities.DevCaps & D3DDEVCAPS_HWTRANSFORMANDLIGHT) != 0)
						ranking += 1.0f;
				}

				if((combo->DeviceInfo->Capabilities.DevCaps & D3DDEVCAPS_HWTRANSFORMANDLIGHT) != 0)
					ranking += 0.1f;

				const vector<D3DDISPLAYMODE>& modes =  combo->AdapterInfo->DisplayModes;

				for (size_t i=0;i<modes.size();i++)
				{
					const D3DDISPLAYMODE& displayMode = modes[i];
					if(displayMode.Format == combo->AdapterFormat &&
						displayMode.Width == optimal.PresentParameters.BackBufferWidth &&
						displayMode.Height == optimal.PresentParameters.BackBufferHeight)
					{
						ranking += 1.0f;
						break;
					}
				}

				if(combo->BackBufferFormat == optimal.PresentParameters.BackBufferFormat)
				{
					ranking += 1.0f;
				}
				else
				{
					int bitDepthDelta = abs(GetColorBits(combo->BackBufferFormat) -
						GetColorBits(optimal.PresentParameters.BackBufferFormat));
					float scale = max(0.9f - bitDepthDelta * 0.2f, 0.0f);
					ranking += scale;
				}

				if(combo->BackBufferFormat == combo->AdapterFormat)
					ranking += 0.1f;

				for(size_t i = 0; i < combo->MultisampleTypes.size(); i++)
				{
					D3DMULTISAMPLE_TYPE type = combo->MultisampleTypes[i];
					int quality = combo->MultisampleQualities[i];

					if(type == optimal.PresentParameters.MultiSampleType)// &&  quality == optimal.PresentParameters.MultiSampleQuality
					{
						ranking += 1.0f + quality * 0.5f;
						break;
					}
				}

				const vector<D3DFORMAT>& dsfmts = combo->DepthStencilFormats;
				if(find(dsfmts.begin(), dsfmts.end(), optimal.PresentParameters.AutoDepthStencilFormat) != dsfmts.end())
					ranking += 1.0f;

				//const vector<D3DDISPLAYMODE>& modes2 =  combo->AdapterInfo->DisplayModes;
				for (size_t i=0;i<modes.size();i++)
				{
					const D3DDISPLAYMODE& displayMode = modes[i];
					if(displayMode.Format == combo->AdapterFormat &&
						displayMode.RefreshRate == optimal.PresentParameters.FullScreen_RefreshRateInHz)
					{
						ranking += 1.0f;
						break;
					}
				}

				const vector<uint32>& presentIntervals = combo->PresentIntervals;
				if(find(presentIntervals.begin(), presentIntervals.end(), 
					optimal.PresentParameters.PresentationInterval) != presentIntervals.end())
					ranking += 1.0f;

				return ranking;
			}

			void Enumeration::BuildValidSettings(const SettingsCombo* combo, const Direct3D9Settings& input, Direct3D9Settings& settings)
			{
				settings.AdapterOrdinal = combo->AdapterOrdinal;
				settings.DeviceType = combo->DeviceType;
				settings.PresentParameters.Windowed = combo->Windowed;
				settings.AdapterFormat = combo->AdapterFormat;
				settings.PresentParameters.BackBufferFormat = combo->BackBufferFormat;
				settings.PresentParameters.SwapEffect = input.PresentParameters.SwapEffect;
				settings.PresentParameters.Flags = input.PresentParameters.Flags | D3DPRESENTFLAG_DISCARD_DEPTHSTENCIL;

				settings.CreationFlags = input.CreationFlags;
				if((combo->DeviceInfo->Capabilities.DevCaps & D3DDEVCAPS_HWTRANSFORMANDLIGHT) == 0 &&
					((settings.CreationFlags & D3DCREATE_HARDWARE_VERTEXPROCESSING) != 0 ||
					(settings.CreationFlags & D3DCREATE_MIXED_VERTEXPROCESSING) != 0))
				{
					settings.CreationFlags &= ~D3DCREATE_HARDWARE_VERTEXPROCESSING;
					settings.CreationFlags &= ~D3DCREATE_MIXED_VERTEXPROCESSING;
					settings.CreationFlags |= D3DCREATE_SOFTWARE_VERTEXPROCESSING;
				}

				if((settings.CreationFlags & D3DCREATE_HARDWARE_VERTEXPROCESSING) == 0 &&
					(settings.CreationFlags & D3DCREATE_MIXED_VERTEXPROCESSING) == 0 &&
					(settings.CreationFlags & D3DCREATE_SOFTWARE_VERTEXPROCESSING) == 0)
				{
					if((combo->DeviceInfo->Capabilities.DevCaps & D3DDEVCAPS_HWTRANSFORMANDLIGHT) != 0)
						settings.CreationFlags |= D3DCREATE_HARDWARE_VERTEXPROCESSING;
					else
						settings.CreationFlags |= D3DCREATE_SOFTWARE_VERTEXPROCESSING;
				}

				D3DDISPLAYMODE bestDisplayMode = FindValidResolution(combo, input);
				settings.PresentParameters.BackBufferWidth = bestDisplayMode.Width;
				settings.PresentParameters.BackBufferHeight = bestDisplayMode.Height;

				settings.PresentParameters.BackBufferCount = input.PresentParameters.BackBufferCount;
				if(settings.PresentParameters.BackBufferCount > 3)
					settings.PresentParameters.BackBufferCount = 3;
				if(settings.PresentParameters.BackBufferCount < 1)
					settings.PresentParameters.BackBufferCount = 1;

				if(input.PresentParameters.SwapEffect != D3DSWAPEFFECT_DISCARD)
				{
					settings.PresentParameters.MultiSampleType = D3DMULTISAMPLE_NONE;
					settings.PresentParameters.MultiSampleQuality = 0;
				}
				else
				{
					D3DMULTISAMPLE_TYPE bestType = D3DMULTISAMPLE_NONE;
					int bestQuality = 0;

					for(size_t i = 0; i < combo->MultisampleTypes.size(); i++)
					{
						D3DMULTISAMPLE_TYPE type = combo->MultisampleTypes[i];
						int quality = combo->MultisampleQualities[i];

						if(abs((int32)type - (int32)input.PresentParameters.MultiSampleType) < abs((int32)bestType -
							(int32)input.PresentParameters.MultiSampleType))
						{
							bestType = type;
							bestQuality = min(quality - 1, (int)input.PresentParameters.MultiSampleQuality);
						}
					}

					settings.PresentParameters.MultiSampleType = bestType;
					settings.PresentParameters.MultiSampleQuality = bestQuality;
				}

				vector<int> rankings;
				int inputDepthBitDepth = GetDepthBits(input.PresentParameters.AutoDepthStencilFormat);
				int inputStencilBitDepth = GetStencilBits(input.PresentParameters.AutoDepthStencilFormat);

				for (size_t i=0;i<combo->DepthStencilFormats.size();i++)
				{
					D3DFORMAT format = combo->DepthStencilFormats[i];
					int currentBitDepth = GetDepthBits(format);
					int currentStencilDepth = GetStencilBits(format);

					int ranking = abs(currentBitDepth - inputDepthBitDepth);
					ranking += abs(currentStencilDepth - inputStencilBitDepth);
					rankings.push_back(ranking);
				}

				int bestIndex = -1;
				int bestRanking = MAXINT32;
				for (size_t i=0;i<rankings.size();i++)
				{
					if(rankings[i] < bestRanking)
					{
						bestRanking = rankings[i];
						bestIndex = i;
					}
				}

				if(bestIndex >= 0)
				{
					settings.PresentParameters.AutoDepthStencilFormat = combo->DepthStencilFormats[bestIndex];
					settings.PresentParameters.EnableAutoDepthStencil = true;
				}
				else
				{
					settings.PresentParameters.AutoDepthStencilFormat = D3DFMT_UNKNOWN;
					settings.PresentParameters.EnableAutoDepthStencil = false;
				}

				if(combo->Windowed)
					settings.PresentParameters.FullScreen_RefreshRateInHz = 0;
				else
				{
					int match = input.PresentParameters.FullScreen_RefreshRateInHz;
					bestDisplayMode.RefreshRate = 0;
					if(match != 0)
					{
						bestRanking = 100000;

						const vector<D3DDISPLAYMODE>& displayModes = combo->AdapterInfo->DisplayModes;
						for (size_t i=0;i<displayModes.size();i++)
						{
							const D3DDISPLAYMODE& mode = displayModes[i];
							if(mode.Format != combo->AdapterFormat ||
								mode.Width != bestDisplayMode.Width ||
								mode.Height != bestDisplayMode.Height)
								continue;

							int ranking = abs(static_cast<int>(mode.RefreshRate) - match);

							if(ranking < bestRanking)
							{
								bestDisplayMode.RefreshRate = mode.RefreshRate;
								bestRanking = ranking;

								if(bestRanking == 0)
									break;
							}
						}
					}

					settings.PresentParameters.FullScreen_RefreshRateInHz = bestDisplayMode.RefreshRate;
				}

				const vector<uint32> prIntrvls = combo->PresentIntervals;
				if(find(prIntrvls.begin(), prIntrvls.end(), input.PresentParameters.PresentationInterval) != prIntrvls.end())
					settings.PresentParameters.PresentationInterval = input.PresentParameters.PresentationInterval;
				else
					settings.PresentParameters.PresentationInterval = D3DPRESENT_INTERVAL_DEFAULT;
			}

			D3DDISPLAYMODE Enumeration::FindValidResolution(const SettingsCombo* combo, const Direct3D9Settings& input)
			{
				if (combo->AdapterInfo->DisplayModes.empty())
				{
					throw Apoc3DException::createException(EX_NotSupported, L"No device modes available");
				}

				D3DDISPLAYMODE bestMode;
				bestMode.Width = 0;

				if(combo->Windowed)
				{
					bestMode.Width = input.PresentParameters.BackBufferWidth;
					bestMode.Height = input.PresentParameters.BackBufferHeight;
					return bestMode;
				}

				int bestRanking = 100000;
				int ranking;

				for (size_t i=0; i<combo->AdapterInfo->DisplayModes.size();i++)
				{
					const D3DDISPLAYMODE& mode = combo->AdapterInfo->DisplayModes[i];

					if(mode.Format != combo->AdapterFormat)
						continue;

					ranking = abs((int)mode.Width - (int)input.PresentParameters.BackBufferWidth) +
						abs((int)mode.Height - (int)input.PresentParameters.BackBufferHeight);

					if(ranking < bestRanking)
					{
						bestMode = mode;
						bestRanking = ranking;

						if(bestRanking == 0)
							break;
					}
				}


				if(bestMode.Width == 0)
				{
					bestMode.Width = input.PresentParameters.BackBufferWidth;
					bestMode.Height = input.PresentParameters.BackBufferHeight;
				}

				return bestMode;
			}
		}
	}
}