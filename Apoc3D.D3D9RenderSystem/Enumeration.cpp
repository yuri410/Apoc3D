/*
-----------------------------------------------------------------------------
This source file is part of Apoc3D Engine

Copyright (c) 2009+ Tao Xin

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
#include "D3D9Utils.h"

#include "apoc3d/Utility/StringUtils.h"
#include "apoc3d/Math/Math.h"

#include "apoc3d/Exception.h"

using namespace Apoc3D;
using namespace Apoc3D::Utility;

namespace Apoc3D
{
	namespace Graphics
	{
		namespace D3D9RenderSystem
		{
			static int GetDepthBits(D3DFORMAT format);
			static int GetStencilBits(D3DFORMAT format);
			static int GetColorBits(D3DFORMAT format);
			static int32 CompareDisplayMode(const D3DDISPLAYMODE& x, const D3DDISPLAYMODE& y);

			//////////////////////////////////////////////////////////////////////////

			void EnumerationMinimumSettings::Set(const RenderParameters& params)
			{
				IsSet = true;
				Width = params.BackBufferWidth;
				Height = params.BackBufferHeight;

				ColorFormat = D3D9Utils::ConvertPixelFormat(params.ColorBufferFormat);
				DepthFormat = D3D9Utils::ConvertDepthFormat(params.DepthBufferFormat);
				MultisampleType = D3D9Utils::ConvertMultisample(params.FSAASampleCount);
				RefreshRate = params.RefreshRate;;
			}

			EnumerationMinimumSettings Enumeration::MinimumSettings;
			List<AdapterInfo*> Enumeration::m_adapters;
			bool Enumeration::m_hasEnumerated = false;


			void Enumeration::Enumerate(IDirect3D9* d3d9)
			{
				if (m_hasEnumerated)
				{
					m_adapters.DeleteAndClear();
				}

				UINT count = d3d9->GetAdapterCount();

				for (size_t i = 0; i < count; i++)
				{
					AdapterInfo* adapter = new AdapterInfo(d3d9, i, MinimumSettings);

					if (adapter->Devices.getCount() > 0)
					{
						m_adapters.Add(adapter);
					}
					else
					{
						delete adapter;
					}
				}

				// add subscripts to adapter description with the same name
				bool unique = true;
				if (m_adapters.getCount() > 0)
				{
					for (int32 i = 0; i < m_adapters.getCount() && !unique; i++)
					{
						for (int32 j = i + 1; j < m_adapters.getCount(); j++)
						{
							if (m_adapters[i]->Description == m_adapters[j]->Description)
							{
								unique = false;
								break;
							}
						}
					}
				}

				if (!unique)
				{
					HashMap<String, int32> descCounter;
					for (AdapterInfo* ai : m_adapters)
					{
						if (!unique)
						{
							int32 count = 0;

							if (!descCounter.TryGetValue(ai->Description, count))
							{
								descCounter.Add(ai->Description, count);
							}
							else
							{
								count++;
								descCounter[ai->Description] = count;
							}

							String temp = L" [" + StringUtils::IntToString(count + 1) + L"]";
							ai->Description.append(temp);
						}
					}
				}
			}

			//////////////////////////////////////////////////////////////////////////

			AdapterInfo::AdapterInfo(IDirect3D9* d3d9, int32 index, const EnumerationMinimumSettings& minSettings)
				: AdapterIndex(index)
			{
				const D3DFORMAT allowedAdapterFormat[] =
				{
					D3DFMT_X8R8G8B8, D3DFMT_X1R5G5B5,
					D3DFMT_R5G6B5, D3DFMT_A2R10G10B10
				};
				const D3DDEVTYPE allowedDevTypes[] = { D3DDEVTYPE_HAL, D3DDEVTYPE_REF };

				D3DADAPTER_IDENTIFIER9 rawAdapter;
				HRESULT hr = d3d9->GetAdapterIdentifier(index, 0, &rawAdapter);
				assert(SUCCEEDED(hr));

				Description = StringUtils::toPlatformWideString(rawAdapter.Description);


				D3DFormatHashSet adapterFormats;
				for (D3DFORMAT fmt : allowedAdapterFormat)
				{
					uint32 dispCount = d3d9->GetAdapterModeCount(index, fmt);

					for (size_t k = 0; k < dispCount; k++)
					{
						D3DDISPLAYMODE mode;
						hr = d3d9->EnumAdapterModes(index, fmt, k, &mode);
						assert(SUCCEEDED(hr));

						// check minimum
						if (minSettings.IsSet)
						{
							if ((int)mode.Width < minSettings.Width ||
								(int)mode.Height < minSettings.Height ||
								(int)mode.RefreshRate < minSettings.RefreshRate)
							{
								continue;
							}
						}

						DisplayModes.Add(mode);

						if (!adapterFormats.Contains(mode.Format))
							adapterFormats.Add(mode.Format);
					}
				}

				D3DDISPLAYMODE currentMode;
				hr = d3d9->GetAdapterDisplayMode(index, &currentMode);
				assert(SUCCEEDED(hr));

				if (!adapterFormats.Contains(currentMode.Format))
					adapterFormats.Add(currentMode.Format);

				DisplayModes.Sort(CompareDisplayMode);

				for (D3DDEVTYPE devType : allowedDevTypes)
				{
					if (minSettings.IsSet && minSettings.DeviceType != devType)
					{
						continue;
					}

					DeviceInfo* devInfo = new DeviceInfo(d3d9, devType, this, adapterFormats, minSettings);

					if (devInfo->SettingCombos.getCount() > 0)
					{
						Devices.Add(devInfo);
					}
					else
					{
						delete devInfo;
					}
				}
			}
			
			AdapterInfo::~AdapterInfo()
			{
				Devices.DeleteAndClear();
			}

			//////////////////////////////////////////////////////////////////////////

			DeviceInfo::DeviceInfo(IDirect3D9* d3d9, D3DDEVTYPE devType, AdapterInfo* parentAdp,
				const D3DFormatHashSet& adapterFormats, const EnumerationMinimumSettings& minSettings)
				: DeviceType(devType)
			{
				HRESULT hr = d3d9->GetDeviceCaps(parentAdp->AdapterIndex, devType, &Capabilities);
				assert(SUCCEEDED(hr));

				const D3DFORMAT allowedBackBufferFormats[] =
				{
					D3DFMT_A8R8G8B8, D3DFMT_X8R8G8B8, D3DFMT_A2R10G10B10,
					D3DFMT_R5G6B5, D3DFMT_A1R5G5B5, D3DFMT_X1R5G5B5
				};

				for (D3DFORMAT adpFmt : adapterFormats)
				{
					for (D3DFORMAT bbfmt : allowedBackBufferFormats)
					{
						for (int windowed = 0; windowed < 2; windowed++)
						{
							if (windowed == 0 && parentAdp->DisplayModes.getCount() == 0)
								continue;

							HRESULT hr = d3d9->CheckDeviceType(parentAdp->AdapterIndex, DeviceType,
								adpFmt, bbfmt, (windowed == 1) ? TRUE : FALSE);

							if (FAILED(hr))
								continue;

							hr = d3d9->CheckDeviceFormat(parentAdp->AdapterIndex, DeviceType,
								adpFmt, D3DUSAGE_QUERY_POSTPIXELSHADER_BLENDING, D3DRTYPE_TEXTURE, bbfmt);

							if (FAILED(hr))
								continue;


							SettingsCombo combo(d3d9,
								parentAdp->AdapterIndex, DeviceType, adpFmt, bbfmt, windowed == 1,
								parentAdp, this);

							if (combo.MultisampleTypes.getCount() == 0)
								continue;

							if (minSettings.IsSet)
							{
								if (minSettings.ColorFormat != D3DFMT_UNKNOWN &&
									minSettings.ColorFormat != bbfmt)
								{
									continue;
								}

								// check depth stencil format against D3DFMT_UNKNOWN
								if (minSettings.DepthFormat != D3DFMT_UNKNOWN &&
									combo.DepthStencilFormats.IndexOf(minSettings.DepthFormat) == -1)
								{
									continue;
								}

								if (combo.MultisampleTypes.IndexOf(minSettings.MultisampleType) == -1)
								{
									continue;
								}
							}

							SettingCombos.Add(new SettingsCombo(combo));
						}
					}
				}
			}

			DeviceInfo::~DeviceInfo()
			{
				SettingCombos.DeleteAndClear();
			}

			//////////////////////////////////////////////////////////////////////////

			SettingsCombo::SettingsCombo(IDirect3D9* d3d9,
				int32 adpIdx, D3DDEVTYPE devType, D3DFORMAT adpFmt, D3DFORMAT bbFmt, bool wnd,
				AdapterInfo* parentAdp, DeviceInfo* parentDev)
				: AdapterIndex(adpIdx), DeviceType(devType), AdapterFormat(adpFmt), BackBufferFormat(bbFmt), Windowed(wnd),
				ParentAdapterInfo(parentAdp), ParentDeviceInfo(parentDev)
			{
				{ // depth stencil formats
					// check every possible depth stencil format see if passes. Add valid one into the list
					const D3DFORMAT possibleDepthStencilFormats[] = { D3DFMT_D16, D3DFMT_D15S1, D3DFMT_D24X8,
						D3DFMT_D24S8, D3DFMT_D24X4S4, D3DFMT_D32 };

					for (D3DFORMAT format : possibleDepthStencilFormats)
					{
						HRESULT hr = d3d9->CheckDeviceFormat(
							AdapterIndex, DeviceType, AdapterFormat,
							D3DUSAGE_DEPTHSTENCIL, D3DRTYPE_SURFACE, format);

						if (FAILED(hr))
							continue;

						hr = d3d9->CheckDepthStencilMatch(
							AdapterIndex, DeviceType, AdapterFormat, BackBufferFormat, format);

						if (FAILED(hr))
							continue;

						DepthStencilFormats.Add(format);
					}
				}

				{ // multisample types
					const D3DMULTISAMPLE_TYPE possibleMultisampleTypes[] = {
						D3DMULTISAMPLE_NONE, D3DMULTISAMPLE_NONMASKABLE,
						D3DMULTISAMPLE_2_SAMPLES, D3DMULTISAMPLE_3_SAMPLES,
						D3DMULTISAMPLE_4_SAMPLES, D3DMULTISAMPLE_5_SAMPLES,
						D3DMULTISAMPLE_6_SAMPLES, D3DMULTISAMPLE_7_SAMPLES,
						D3DMULTISAMPLE_8_SAMPLES, D3DMULTISAMPLE_9_SAMPLES,
						D3DMULTISAMPLE_10_SAMPLES, D3DMULTISAMPLE_11_SAMPLES,
						D3DMULTISAMPLE_12_SAMPLES, D3DMULTISAMPLE_13_SAMPLES,
						D3DMULTISAMPLE_14_SAMPLES, D3DMULTISAMPLE_15_SAMPLES,
						D3DMULTISAMPLE_16_SAMPLES
					};

					DWORD quality;
					for (const D3DMULTISAMPLE_TYPE type : possibleMultisampleTypes)
					{
						HRESULT hr = d3d9->CheckDeviceMultiSampleType(AdapterIndex, DeviceType,
							AdapterFormat, Windowed, type, &quality);

						if (SUCCEEDED(hr))
						{
							MultisampleTypes.Add(type);
							MultisampleQualities.Add(quality);
						}
					}
				}

				{ // present intervals
					const DWORD possiblePresentIntervals[] = {
						D3DPRESENT_INTERVAL_IMMEDIATE, D3DPRESENT_INTERVAL_DEFAULT,
						D3DPRESENT_INTERVAL_ONE, D3DPRESENT_INTERVAL_TWO,
						D3DPRESENT_INTERVAL_THREE, D3DPRESENT_INTERVAL_FOUR
					};

					for (DWORD interval : possiblePresentIntervals)
					{
						if (Windowed &&
							(interval == D3DPRESENT_INTERVAL_TWO ||
							interval == D3DPRESENT_INTERVAL_THREE || interval == D3DPRESENT_INTERVAL_FOUR))
						{
							continue;
						}

						if (interval == D3DPRESENT_INTERVAL_DEFAULT || (ParentDeviceInfo->Capabilities.PresentationIntervals & interval) != 0)
						{
							PresentIntervals.Add(interval);
						}
					}
				}
			}


			float SettingsCombo::RankSettingsCombo(const RawSettings& targetOptimal, const D3DDISPLAYMODE& desktopMode) const
			{
				float ranking = 0.0f;

				if (AdapterIndex == (int32)targetOptimal.AdapterOrdinal)
					ranking += 1000.0f;

				if (DeviceType == targetOptimal.DeviceType)
					ranking += 100.0f;

				if (DeviceType == D3DDEVTYPE_HAL)
					ranking += 0.1f;

				if ((Windowed && targetOptimal.PresentParameters.Windowed) ||
					(!Windowed && !targetOptimal.PresentParameters.Windowed))
					ranking += 10.0f;

				if (AdapterFormat == targetOptimal.AdapterFormat)
					ranking += 1.0f;
				else
				{
					int bitDepthDelta = abs(GetColorBits(AdapterFormat) -
						GetColorBits(targetOptimal.AdapterFormat));
					float scale = max(0.9f - bitDepthDelta * 0.2f, 0.0f);
					ranking += scale;
				}

				if (!Windowed)
				{
					bool match;
					if (GetColorBits(desktopMode.Format) >= 8)
						match = (AdapterFormat == desktopMode.Format);
					else
						match = (AdapterFormat == D3DFMT_X8R8G8B8);

					if (match)
						ranking += 0.1f;
				}

				if ((targetOptimal.CreationFlags & D3DCREATE_HARDWARE_VERTEXPROCESSING) != 0 &&
					(targetOptimal.CreationFlags & D3DCREATE_MIXED_VERTEXPROCESSING) != 0)
				{
					if ((ParentDeviceInfo->Capabilities.DevCaps & D3DDEVCAPS_HWTRANSFORMANDLIGHT) != 0)
						ranking += 1.0f;
				}

				if ((ParentDeviceInfo->Capabilities.DevCaps & D3DDEVCAPS_HWTRANSFORMANDLIGHT) != 0)
					ranking += 0.1f;

				const List<D3DDISPLAYMODE>& modes = ParentAdapterInfo->DisplayModes;

				for (const D3DDISPLAYMODE& displayMode : modes)
				{
					if (displayMode.Format == AdapterFormat &&
						displayMode.Width == targetOptimal.PresentParameters.BackBufferWidth &&
						displayMode.Height == targetOptimal.PresentParameters.BackBufferHeight)
					{
						ranking += 1.0f;
						break;
					}
				}

				if (BackBufferFormat == targetOptimal.PresentParameters.BackBufferFormat)
				{
					ranking += 1.0f;
				}
				else
				{
					int bitDepthDelta = abs(GetColorBits(BackBufferFormat) -
						GetColorBits(targetOptimal.PresentParameters.BackBufferFormat));
					float scale = max(0.9f - bitDepthDelta * 0.2f, 0.0f);
					ranking += scale;
				}

				if (BackBufferFormat == AdapterFormat)
					ranking += 0.1f;

				for (int32 i = 0; i < MultisampleTypes.getCount(); i++)
				{
					D3DMULTISAMPLE_TYPE type = MultisampleTypes[i];
					int quality = MultisampleQualities[i];

					if (type == targetOptimal.PresentParameters.MultiSampleType)
					{
						ranking += 1.0f + quality * 0.5f;
						break;
					}
				}

				if (DepthStencilFormats.IndexOf(targetOptimal.PresentParameters.AutoDepthStencilFormat) != -1)
					ranking += 1.0f;

				for (const D3DDISPLAYMODE& displayMode : modes)
				{
					if (displayMode.Format == AdapterFormat &&
						displayMode.RefreshRate == targetOptimal.PresentParameters.FullScreen_RefreshRateInHz)
					{
						ranking += 1.0f;
						break;
					}
				}

				if (PresentIntervals.IndexOf(targetOptimal.PresentParameters.PresentationInterval) != -1)
					ranking += 1.0f;

				return ranking;
			}

			Point SettingsCombo::FindBestResolution(int32 optimalWidth, int32 optimalHeight) const
			{
				if (ParentAdapterInfo->DisplayModes.getCount() == 0)
				{
					throw AP_EXCEPTION(ExceptID::NotSupported, L"No device modes available");
				}

				Point result;

				if (Windowed)
				{
					return Point(optimalWidth, optimalHeight);
				}

				int bestRanking = 100000;
				int ranking;

				for (const D3DDISPLAYMODE& mode : ParentAdapterInfo->DisplayModes)
				{
					if (mode.Format != AdapterFormat)
						continue;

					ranking = abs((int)mode.Width - optimalWidth) +
						abs((int)mode.Height - optimalHeight);

					if (ranking < bestRanking)
					{
						result.X = mode.Width;
						result.Y = mode.Height;

						bestRanking = ranking;

						if (bestRanking == 0)
							break;
					}
				}

				if (result.X == 0)
				{
					result.X = optimalWidth;
					result.Y = optimalHeight;
				}

				return result;
			}



			//////////////////////////////////////////////////////////////////////////

			void Enumeration::FindValidSettings(IDirect3D9* d3d9, const RenderParameters& settings, RawSettings& result)
			{
				if (!hasEnumerated())
					Enumerate(d3d9);

				RawSettings raw;
				BuildRawSettings(d3d9, settings, raw);

				FindValidSettings(d3d9, raw, result);
			}
			void Enumeration::FindValidSettings(IDirect3D9* d3d9, const RawSettings& raw, RawSettings& result)
			{
				if (!hasEnumerated())
					Enumerate(d3d9);

				float bestRanking = -1.0f;
				const SettingsCombo* bestCombo = nullptr;

				for (const AdapterInfo* adapterInfo : m_adapters)
				{
					D3DDISPLAYMODE desktopMode;
					HRESULT hr = d3d9->GetAdapterDisplayMode(adapterInfo->AdapterIndex, &desktopMode);
					assert(SUCCEEDED(hr));

					for (const DeviceInfo* deviceInfo : adapterInfo->Devices)
					{
						for (const SettingsCombo* combo : deviceInfo->SettingCombos)
						{
							if (combo->Windowed && combo->AdapterFormat == desktopMode.Format)
								continue;

							float ranking = combo->RankSettingsCombo(raw, desktopMode);
							if (ranking > bestRanking)
							{
								bestCombo = combo;
								bestRanking = ranking;
							}
						}
					}
				}

				if (bestCombo == nullptr)
				{
					throw AP_EXCEPTION(ExceptID::NotSupported, L"Can not create Direct3D9 Device. No compatible Direct3D9 devices found.");
				}
				BuildValidSettings(bestCombo, raw, result);
			}

			void Enumeration::BuildRawSettings(IDirect3D9* d3d9, const RenderParameters& input, RawSettings& result)
			{
				D3DDISPLAYMODE desktopMode;
				HRESULT hr = d3d9->GetAdapterDisplayMode(0, &desktopMode);
				assert(SUCCEEDED(hr));

				

				result.AdapterOrdinal = input.AdapterIndex;
				result.DeviceType = D3DDEVTYPE_HAL; //settings.DeviceType;
				result.PresentParameters.Windowed = input.IsWindowed;
				result.PresentParameters.BackBufferCount = input.TripleBuffering ? 2 : 1;// settings.BackBufferCount;
				result.PresentParameters.MultiSampleType = D3D9Utils::ConvertMultisample(input.FSAASampleCount);
				result.PresentParameters.MultiSampleQuality = 0;
				result.PresentParameters.FullScreen_RefreshRateInHz = input.RefreshRate;

				if (input.IsMultithreaded)
					result.CreationFlags |= D3DCREATE_MULTITHREADED;

				if (result.PresentParameters.Windowed || GetColorBits(desktopMode.Format) >= 8)
					result.AdapterFormat = desktopMode.Format;
				else
					result.AdapterFormat = D3DFMT_X8R8G8B8;

				if (input.BackBufferWidth == 0 || input.BackBufferHeight == 0)
				{
					if (result.PresentParameters.Windowed)
					{
						result.PresentParameters.BackBufferWidth = 640;
						result.PresentParameters.BackBufferHeight = 480;
					}
					else
					{
						result.PresentParameters.BackBufferWidth = desktopMode.Width;
						result.PresentParameters.BackBufferHeight = desktopMode.Height;
					}
				}
				else
				{
					result.PresentParameters.BackBufferWidth = input.BackBufferWidth;
					result.PresentParameters.BackBufferHeight = input.BackBufferHeight;
				}

				if (input.ColorBufferFormat == FMT_Unknown)
					result.PresentParameters.BackBufferFormat = result.AdapterFormat;
				else
					result.PresentParameters.BackBufferFormat = D3D9Utils::ConvertPixelFormat(input.ColorBufferFormat);

				if (input.DepthBufferFormat == FMT_Unknown)
				{
					if (GetColorBits(result.PresentParameters.BackBufferFormat) >= 8)
						result.PresentParameters.AutoDepthStencilFormat = D3DFMT_D32;
					else
						result.PresentParameters.AutoDepthStencilFormat = D3DFMT_D16;
				}
				else
					result.PresentParameters.AutoDepthStencilFormat = D3D9Utils::ConvertDepthFormat(input.DepthBufferFormat);

				if (!input.EnableVSync)
					result.PresentParameters.PresentationInterval = D3DPRESENT_INTERVAL_IMMEDIATE;
			}

			void Enumeration::BuildValidSettings(const SettingsCombo* combo, const RawSettings& input, RawSettings& result)
			{
				result.AdapterOrdinal = combo->AdapterIndex;
				result.DeviceType = combo->DeviceType;
				result.PresentParameters.Windowed = combo->Windowed;
				result.AdapterFormat = combo->AdapterFormat;
				result.PresentParameters.BackBufferFormat = combo->BackBufferFormat;
				result.PresentParameters.SwapEffect = input.PresentParameters.SwapEffect;
				result.PresentParameters.Flags = input.PresentParameters.Flags | D3DPRESENTFLAG_DISCARD_DEPTHSTENCIL;

				result.CreationFlags = input.CreationFlags;
				if ((combo->ParentDeviceInfo->Capabilities.DevCaps & D3DDEVCAPS_HWTRANSFORMANDLIGHT) == 0 &&
					((result.CreationFlags & D3DCREATE_HARDWARE_VERTEXPROCESSING) != 0 ||
					(result.CreationFlags & D3DCREATE_MIXED_VERTEXPROCESSING) != 0))
				{
					result.CreationFlags &= ~D3DCREATE_HARDWARE_VERTEXPROCESSING;
					result.CreationFlags &= ~D3DCREATE_MIXED_VERTEXPROCESSING;
					result.CreationFlags |= D3DCREATE_SOFTWARE_VERTEXPROCESSING;
				}

				if ((result.CreationFlags & D3DCREATE_HARDWARE_VERTEXPROCESSING) == 0 &&
					(result.CreationFlags & D3DCREATE_MIXED_VERTEXPROCESSING) == 0 &&
					(result.CreationFlags & D3DCREATE_SOFTWARE_VERTEXPROCESSING) == 0)
				{
					if ((combo->ParentDeviceInfo->Capabilities.DevCaps & D3DDEVCAPS_HWTRANSFORMANDLIGHT) != 0)
						result.CreationFlags |= D3DCREATE_HARDWARE_VERTEXPROCESSING;
					else
						result.CreationFlags |= D3DCREATE_SOFTWARE_VERTEXPROCESSING;
				}

				Point bestResol = combo->FindBestResolution(input.PresentParameters.BackBufferWidth, input.PresentParameters.BackBufferHeight);
				result.PresentParameters.BackBufferWidth = bestResol.X;
				result.PresentParameters.BackBufferHeight = bestResol.Y;

				result.PresentParameters.BackBufferCount = input.PresentParameters.BackBufferCount;
				if (result.PresentParameters.BackBufferCount > D3DPRESENT_BACK_BUFFERS_MAX)
					result.PresentParameters.BackBufferCount = D3DPRESENT_BACK_BUFFERS_MAX;
				if (result.PresentParameters.BackBufferCount < 1)
					result.PresentParameters.BackBufferCount = 1;

				if (input.PresentParameters.SwapEffect != D3DSWAPEFFECT_DISCARD)
				{
					result.PresentParameters.MultiSampleType = D3DMULTISAMPLE_NONE;
					result.PresentParameters.MultiSampleQuality = 0;
				}
				else
				{
					D3DMULTISAMPLE_TYPE bestType = D3DMULTISAMPLE_NONE;
					int bestQuality = 0;

					for (int32 i = 0; i < combo->MultisampleTypes.getCount(); i++)
					{
						D3DMULTISAMPLE_TYPE type = combo->MultisampleTypes[i];
						int quality = combo->MultisampleQualities[i];

						if (abs((int32)type - (int32)input.PresentParameters.MultiSampleType) < abs((int32)bestType -
							(int32)input.PresentParameters.MultiSampleType))
						{
							bestType = type;
							bestQuality = Math::Min(quality - 1, (int)input.PresentParameters.MultiSampleQuality);
						}
					}

					result.PresentParameters.MultiSampleType = bestType;
					result.PresentParameters.MultiSampleQuality = bestQuality;
				}

				List<int> dsRankings;
				int inputDepthBitDepth = GetDepthBits(input.PresentParameters.AutoDepthStencilFormat);
				int inputStencilBitDepth = GetStencilBits(input.PresentParameters.AutoDepthStencilFormat);

				for (D3DFORMAT format : combo->DepthStencilFormats)
				{
					int currentBitDepth = GetDepthBits(format);
					int currentStencilDepth = GetStencilBits(format);

					int ranking = abs(currentBitDepth - inputDepthBitDepth);
					ranking += abs(currentStencilDepth - inputStencilBitDepth);
					dsRankings.Add(ranking);
				}

				int bestIndex = -1;
				int bestRanking = MAXINT32;
				for (int32 i = 0; i < dsRankings.getCount(); i++)
				{
					if (dsRankings[i] < bestRanking)
					{
						bestRanking = dsRankings[i];
						bestIndex = i;
					}
				}

				if (bestIndex >= 0)
				{
					result.PresentParameters.AutoDepthStencilFormat = combo->DepthStencilFormats[bestIndex];
					result.PresentParameters.EnableAutoDepthStencil = true;
				}
				else
				{
					result.PresentParameters.AutoDepthStencilFormat = D3DFMT_UNKNOWN;
					result.PresentParameters.EnableAutoDepthStencil = false;
				}

				if (combo->Windowed)
					result.PresentParameters.FullScreen_RefreshRateInHz = 0;
				else
				{
					int32 targetRefreshRate = input.PresentParameters.FullScreen_RefreshRateInHz;
					int32 resultRefreshRate = 0;
					if (targetRefreshRate != 0)
					{
						bestRanking = 100000;

						for (const D3DDISPLAYMODE& mode : combo->ParentAdapterInfo->DisplayModes)
						{
							if (mode.Format != combo->AdapterFormat ||
								mode.Width != bestResol.X ||
								mode.Height != bestResol.Y)
								continue;

							int ranking = abs(static_cast<int>(mode.RefreshRate) - targetRefreshRate);

							if (ranking < bestRanking)
							{
								resultRefreshRate = mode.RefreshRate;
								bestRanking = ranking;

								if (bestRanking == 0)
									break;
							}
						}
					}

					result.PresentParameters.FullScreen_RefreshRateInHz = resultRefreshRate;
				}

				const List<uint32>& prIntrvls = combo->PresentIntervals;
				if (prIntrvls.IndexOf(input.PresentParameters.PresentationInterval) != -1)
					result.PresentParameters.PresentationInterval = input.PresentParameters.PresentationInterval;
				else
					result.PresentParameters.PresentationInterval = D3DPRESENT_INTERVAL_DEFAULT;
			}

			//////////////////////////////////////////////////////////////////////////

			static int GetDepthBits(D3DFORMAT format)
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
			static int GetStencilBits(D3DFORMAT format)
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
			static int GetColorBits(D3DFORMAT format)
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

			static int32 CompareDisplayMode(const D3DDISPLAYMODE& x, const D3DDISPLAYMODE& y)
			{
				if (x.Width > y.Width) return 1;
				if (x.Width < y.Width) return -1;
				if (x.Height > y.Height) return 1;
				if (x.Height < y.Height) return -1;
				if (x.Format > y.Format) return 1;
				if (x.Format < y.Format) return -1;
				if (x.RefreshRate > y.RefreshRate) return 1;
				if (x.RefreshRate < y.RefreshRate) return -1;

				return 0;
			}

		}

	}
}