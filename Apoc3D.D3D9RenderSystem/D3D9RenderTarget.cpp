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

#include "D3D9RenderTarget.h"

#include "D3D9Texture.h"
#include "D3D9RenderDevice.h"
#include "D3D9Utils.h"
#include "Buffer/D3D9DepthBuffer.h"
#include "GraphicsDeviceManager.h"

#include "apoc3d/ApocException.h"

#include "apoc3d/Core/Logging.h"
#include "apoc3d/Utility/StringUtils.h"

using namespace Apoc3D::Utility;

namespace Apoc3D
{
	namespace Graphics
	{
		namespace D3D9RenderSystem
		{
			DepthFormat GetDepthSurfaceFormat(IDirect3DSurface9* depth)
			{
				D3DSURFACE_DESC desc;
				depth->GetDesc(&desc);
				return D3D9Utils::ConvertBackDepthFormat(desc.Format);
			}
			int32 getSurfaceWidth(IDirect3DSurface9* color)
			{
				D3DSURFACE_DESC desc;
				color->GetDesc(&desc);
				return desc.Height;
			}
			int32 getSurfaceHeight(IDirect3DSurface9* color)
			{
				D3DSURFACE_DESC desc;
				color->GetDesc(&desc);
				return desc.Width;
			}
			PixelFormat GetColorSurfaceFormat(IDirect3DSurface9* color)
			{
				D3DSURFACE_DESC desc;
				color->GetDesc(&desc);
				return D3D9Utils::ConvertBackPixelFormat(desc.Format);
			}
			uint32 GetMultiSampleCount(IDirect3DSurface9* color)
			{
				D3DSURFACE_DESC desc;
				color->GetDesc(&desc);
				return D3D9Utils::ConvertBackMultiSample(desc.MultiSampleType);
			}

			void logRTFailure(PixelFormat colorFormat, DepthFormat depFormat, const String& multisampleType)
			{
				String formatString;
				if (colorFormat != FMT_Count)
				{
					formatString.append(L"C-");
					formatString.append(PixelFormatUtils::ToString(colorFormat));
				}
				if (depFormat != DEPFMT_Count)
				{
					if (formatString.size())
						formatString.append(L", ");
					formatString.append(L"D-");
					formatString.append(PixelFormatUtils::ToString(depFormat));
				}
				if (!RenderTarget::CheckMultisampleModeStringNone(multisampleType))
				{
					if (formatString.size())
						formatString.append(L", ");
					formatString.append(L"MSAA=");
					formatString.append(multisampleType);
				}

				ApocLog(LOG_Graphics, L"[D3D9] RenderTarget format not supported: " + formatString, LOGLVL_Error);
			}


			void D3D9RenderTarget::ReleaseVolatileResource()
			{
				if (m_color)
				{
					m_colorSurface->Release();
					m_color->Release();
				}
				if (m_depthSurface)
				{
					m_depthSurface->Release();
				}
			}
			void D3D9RenderTarget::ReloadVolatileResource()
			{
				// recreating from a device lost needs some work
				D3DDevice* dev = m_device->getDevice();

				if (m_hasPercentangeLock)
				{
					Viewport vp = m_device->getViewport();

					int estWidth = static_cast<uint>(vp.Width * m_widthPercentage + 0.5f);
					int estHeight = static_cast<uint>(vp.Height * m_heightPercentage + 0.5f);
					m_width = estWidth;
					m_height = estHeight;
				}

				if (!isMultiSampled())
				{
					if (m_hasColor)
					{
						HRESULT hr = dev->CreateTexture(getWidth(), getHeight(), 1, D3DUSAGE_RENDERTARGET, 
							D3D9Utils::ConvertPixelFormat(getColorFormat()), D3DPOOL_DEFAULT, &m_color, NULL);
						assert(SUCCEEDED(hr));
						
						m_color->GetSurfaceLevel(0, &m_colorSurface);

						m_d3dTexture->setInternal2D(m_color);
					}
					
					if (m_hasDepth)
					{
						HRESULT hr = dev->CreateDepthStencilSurface(getWidth(), getHeight(), 
							D3D9Utils::ConvertDepthFormat(getDepthFormat()), D3DMULTISAMPLE_NONE, 0, TRUE, &m_depthSurface, NULL);
						assert(SUCCEEDED(hr));

						m_depthBuffer->setD3DBuffer(m_depthSurface);
					}
				}
				else
				{
					GraphicsDeviceManager* dmgr = m_device->getGraphicsDeviceManager();
					const DeviceSettings* sets = dmgr->getCurrentSetting();

					D3D9Capabilities* caps = m_device->getD3D9Capabilities();
					const D3D9Capabilities::AAProfile* aamode = caps->LookupAAProfile(getMultisampleMode(), getColorFormat(), getDepthFormat());
					if (aamode == nullptr)
					{
						logRTFailure(getColorFormat(), getDepthFormat(), getMultisampleMode());
						throw AP_EXCEPTION(EX_NotSupported, L"");
					}

					DWORD quality;
					DWORD quality2;

					if (m_hasColor)
					{
						HRESULT hr = dmgr->getDirect3D()->CheckDeviceMultiSampleType(
							sets->D3D9.AdapterOrdinal, sets->D3D9.DeviceType, D3D9Utils::ConvertPixelFormat(getColorFormat()), sets->D3D9.PresentParameters.Windowed, aamode->SampleType, &quality);
						if (hr != S_OK || quality == 0)
							throw AP_EXCEPTION(EX_NotSupported, L"");
					}
					if (m_hasDepth)
					{
						HRESULT hr = dmgr->getDirect3D()->CheckDeviceMultiSampleType(
							sets->D3D9.AdapterOrdinal, sets->D3D9.DeviceType, D3D9Utils::ConvertDepthFormat(getDepthFormat()), sets->D3D9.PresentParameters.Windowed, aamode->SampleType, &quality2);
						if (hr != S_OK || quality2 == 0)
							throw AP_EXCEPTION(EX_NotSupported, L"");
					}

					if (m_hasColor && m_hasDepth)
					{
						DWORD overallQ = min(quality, quality2)-1;
						overallQ = min(overallQ, aamode->SampleQuality);

						quality2 = quality = overallQ;
					}

					if (m_hasColor)
					{
						HRESULT hr = dev->CreateRenderTarget(getWidth(), getHeight(), 
							D3D9Utils::ConvertPixelFormat(getColorFormat()), aamode->SampleType, quality, FALSE, &m_colorSurface, NULL);
						assert(SUCCEEDED(hr));

						hr = dev->CreateTexture(getWidth(), getHeight(), 1, D3DUSAGE_RENDERTARGET, 
							D3D9Utils::ConvertPixelFormat(getColorFormat()), D3DPOOL_DEFAULT, &m_color, NULL);
						assert(SUCCEEDED(hr));

						m_d3dTexture->setInternal2D(m_color);
					}
					if (m_hasDepth)
					{
						HRESULT hr = dev->CreateDepthStencilSurface(getWidth(), getHeight(), 
							D3D9Utils::ConvertDepthFormat(getDepthFormat()), aamode->SampleType, quality2, TRUE, &m_depthSurface, NULL);
						assert(SUCCEEDED(hr));

						m_depthBuffer->setD3DBuffer(m_depthSurface);
					}
					
				}

				eventReseted.Invoke(this);
			}


			/*D3D9RenderTarget::D3D9RenderTarget(D3D9RenderDevice* device, IDirect3DSurface9* color, IDirect3DSurface9* depth)
				: RenderTarget(device, 
				getSurfaceWidth(color),
				getSurfaceHeight(color), 
				GetColorSurfaceFormat(color), 
				GetDepthSurfaceFormat(depth), 
				GetMultiSampleCount(color)), VolatileResource(device),
				m_device(device), m_colorSurface(color), m_color(0), m_d3dTexture(0), m_depthSurface(depth),
				m_isDefault(true), m_hasDepth(true), m_hasColor(true)
			{
				m_depthBuffer = new D3D9DepthBuffer(device, depth);
			}*/

			D3D9RenderTarget::D3D9RenderTarget(D3D9RenderDevice* device, D3DTexture2D* rt)
				: RenderTarget(device, 
				D3D9Utils::GetD3DTextureWidth(rt),
				D3D9Utils::GetD3DTextureHeight(rt), 
				D3D9Utils::GetD3DTextureFormat(rt), L""), VolatileResource(device),
				m_device(device), m_color(rt), m_d3dTexture(0), m_depthSurface(0), m_depthBuffer(0),
				m_hasDepth(false), m_hasColor(true),
				m_rtDirty(false)
			{
				m_color->GetSurfaceLevel(0, &m_colorSurface);
				m_d3dTexture = new D3D9Texture(m_device, m_color);
			}
			D3D9RenderTarget::D3D9RenderTarget(D3D9RenderDevice* device, D3DTexture2D* rt, IDirect3DSurface9* depth)
				: RenderTarget(device, 
				D3D9Utils::GetD3DTextureWidth(rt),
				D3D9Utils::GetD3DTextureHeight(rt), 
				D3D9Utils::GetD3DTextureFormat(rt), 
				GetDepthSurfaceFormat(depth), L""), VolatileResource(device),
				m_device(device), m_color(rt), m_d3dTexture(0), m_depthSurface(depth),
				m_hasDepth(true), m_hasColor(true),
				m_rtDirty(false)
			{
				m_color->GetSurfaceLevel(0, &m_colorSurface);
				m_depthBuffer = new D3D9DepthBuffer(device, depth);
				m_d3dTexture = new D3D9Texture(m_device, m_color);
			}

			D3D9RenderTarget::D3D9RenderTarget(D3D9RenderDevice* device, int32 width, int32 height, PixelFormat format)
				: RenderTarget(device, width, height, format, L""), VolatileResource(device),
				m_device(device), m_depthSurface(0), m_depthBuffer(0), m_d3dTexture(0),
				m_hasDepth(false), m_hasColor(true),
				m_rtDirty(false)
			{
				D3DDevice* dev = device->getDevice();
				HRESULT hr = dev->CreateTexture(width, height, 1, D3DUSAGE_RENDERTARGET, 
					D3D9Utils::ConvertPixelFormat(format), D3DPOOL_DEFAULT, &m_color, NULL);
				assert(SUCCEEDED(hr));
				if (FAILED(hr))
					logRTFailure(format, DEPFMT_Count, 0);

				m_color->GetSurfaceLevel(0, &m_colorSurface);
				m_d3dTexture = new D3D9Texture(m_device, m_color);
			}
			D3D9RenderTarget::D3D9RenderTarget(D3D9RenderDevice* device, int32 width, int32 height, PixelFormat format, DepthFormat depthFormat)
				: RenderTarget(device, width, height, format, depthFormat, L""), VolatileResource(device),
				m_device(device),
				m_hasDepth(true), m_hasColor(true),
				m_rtDirty(false)
			{
				D3DDevice* dev = device->getDevice();
				HRESULT hr = dev->CreateTexture(width, height, 1, D3DUSAGE_RENDERTARGET, 
					D3D9Utils::ConvertPixelFormat(format), D3DPOOL_DEFAULT, &m_color, NULL);
				assert(SUCCEEDED(hr));
				if (FAILED(hr))
					logRTFailure(format, DEPFMT_Count, 0);
				m_color->GetSurfaceLevel(0, &m_colorSurface);

				hr = dev->CreateDepthStencilSurface(width, height, 
					D3D9Utils::ConvertDepthFormat(depthFormat), D3DMULTISAMPLE_NONE, 0, TRUE, &m_depthSurface, NULL);
				assert(SUCCEEDED(hr));
				if (FAILED(hr))
					logRTFailure(FMT_Count, depthFormat, 0);

				m_depthBuffer = new D3D9DepthBuffer(device, m_depthSurface);
				m_d3dTexture = new D3D9Texture(m_device, m_color);
			}
			D3D9RenderTarget::D3D9RenderTarget(D3D9RenderDevice* device, int32 width, int32 height, 
				const String& multisampleMode, PixelFormat format, DepthFormat depthFormat)
				: RenderTarget(device, width, height, format, depthFormat, multisampleMode), VolatileResource(device),
				m_device(device),
				m_hasDepth(true), m_hasColor(true),
				m_rtDirty(false)
			{
				D3DDevice* dev = device->getDevice();

				if (!isMultiSampled())
				{
					HRESULT hr = dev->CreateTexture(width, height, 1, D3DUSAGE_RENDERTARGET, 
						D3D9Utils::ConvertPixelFormat(format), D3DPOOL_DEFAULT, &m_color, NULL);
					assert(SUCCEEDED(hr));
					if (FAILED(hr))
						logRTFailure(format, DEPFMT_Count, 0);
					m_color->GetSurfaceLevel(0, &m_colorSurface);

					hr = dev->CreateDepthStencilSurface(width, height, 
						D3D9Utils::ConvertDepthFormat(depthFormat), D3DMULTISAMPLE_NONE, 0, TRUE, &m_depthSurface, NULL);
					assert(SUCCEEDED(hr));
					if (FAILED(hr))
						logRTFailure(FMT_Count, depthFormat, 0);

					m_depthBuffer = new D3D9DepthBuffer(device, m_depthSurface);
				}
				else
				{
					D3D9Capabilities* caps = m_device->getD3D9Capabilities();
					const D3D9Capabilities::AAProfile* aamode = caps->LookupAAProfile(multisampleMode, format, depthFormat);
					if (aamode == nullptr)
					{
						logRTFailure(format, depthFormat, multisampleMode);
						throw AP_EXCEPTION(EX_NotSupported, L"");
					}

					GraphicsDeviceManager* dmgr = m_device->getGraphicsDeviceManager();
					const DeviceSettings* sets = dmgr->getCurrentSetting();
					DWORD quality;
					DWORD quality2;
					HRESULT hr = dmgr->getDirect3D()->CheckDeviceMultiSampleType(
						sets->D3D9.AdapterOrdinal, sets->D3D9.DeviceType, D3D9Utils::ConvertPixelFormat(format), sets->D3D9.PresentParameters.Windowed, aamode->SampleType, &quality);
					if (hr != S_OK || quality == 0)
					{
						logRTFailure(format, depthFormat, multisampleMode);
						throw AP_EXCEPTION(EX_NotSupported, L"");
					}
					hr = dmgr->getDirect3D()->CheckDeviceMultiSampleType(
						sets->D3D9.AdapterOrdinal, sets->D3D9.DeviceType, D3D9Utils::ConvertDepthFormat(depthFormat), sets->D3D9.PresentParameters.Windowed, aamode->SampleType, &quality2);

					if (hr != S_OK || quality2 == 0)
					{
						logRTFailure(format, depthFormat, multisampleMode);
						throw AP_EXCEPTION(EX_NotSupported, L"");
					}

					DWORD overallQ = min(quality, quality2)-1;
					overallQ = min(overallQ, aamode->SampleQuality);
					

					hr = dev->CreateRenderTarget(width, height, 
						D3D9Utils::ConvertPixelFormat(format), aamode->SampleType, overallQ, FALSE, &m_colorSurface, NULL);
					assert(SUCCEEDED(hr));

					hr = dev->CreateTexture(width, height, 1, D3DUSAGE_RENDERTARGET, 
						D3D9Utils::ConvertPixelFormat(format), D3DPOOL_DEFAULT, &m_color, NULL);
					assert(SUCCEEDED(hr));


					hr = dev->CreateDepthStencilSurface(width, height, 
						D3D9Utils::ConvertDepthFormat(depthFormat), aamode->SampleType, overallQ, TRUE, &m_depthSurface, NULL);
					assert(SUCCEEDED(hr));

					m_depthBuffer = new D3D9DepthBuffer(device, m_depthSurface);
				}
				m_d3dTexture = new D3D9Texture(m_device, m_color);
			}
			D3D9RenderTarget::D3D9RenderTarget(D3D9RenderDevice* device, int32 width, int32 height, const String& multisampleMode, PixelFormat format)
				: RenderTarget(device, width, height, format, multisampleMode), VolatileResource(device),
				m_device(device), m_depthSurface(0), m_depthBuffer(0), m_d3dTexture(0),
				m_hasDepth(false), m_hasColor(true),
				m_rtDirty(false)
			{
				D3DDevice* dev = device->getDevice();

				if (!isMultiSampled())
				{
					HRESULT hr = dev->CreateTexture(width, height, 1, D3DUSAGE_RENDERTARGET, 
						D3D9Utils::ConvertPixelFormat(format), D3DPOOL_DEFAULT, &m_color, NULL);
					assert(SUCCEEDED(hr));
					if (FAILED(hr))
						logRTFailure(format, DEPFMT_Count, multisampleMode);

					m_color->GetSurfaceLevel(0, &m_colorSurface);
				}
				else
				{
					D3D9Capabilities* caps = m_device->getD3D9Capabilities();
					const D3D9Capabilities::AAProfile* aamode = caps->LookupAAProfile(multisampleMode, format, DEPFMT_Count);
					if (aamode == nullptr)
					{
						logRTFailure(format, DEPFMT_Count, multisampleMode);
						throw AP_EXCEPTION(EX_NotSupported, L"");
					}

					DWORD quality;
					GraphicsDeviceManager* dmgr = m_device->getGraphicsDeviceManager();
					const DeviceSettings* sets = dmgr->getCurrentSetting();
					HRESULT hr = dmgr->getDirect3D()->CheckDeviceMultiSampleType(
						sets->D3D9.AdapterOrdinal, sets->D3D9.DeviceType, D3D9Utils::ConvertPixelFormat(format), sets->D3D9.PresentParameters.Windowed, aamode->SampleType, &quality);
					if (hr != S_OK || quality == 0)
					{
						logRTFailure(format, DEPFMT_Count, multisampleMode);
						throw AP_EXCEPTION(EX_NotSupported, L"");
					}

					quality--;
					quality = min(quality, aamode->SampleQuality);


					hr = dev->CreateRenderTarget(width, height, 
						D3D9Utils::ConvertPixelFormat(format), aamode->SampleType, quality, FALSE, &m_colorSurface, NULL);
					assert(SUCCEEDED(hr));

					hr = dev->CreateTexture(width, height, 1, D3DUSAGE_RENDERTARGET, 
						D3D9Utils::ConvertPixelFormat(format), D3DPOOL_DEFAULT, &m_color, NULL);
					assert(SUCCEEDED(hr));
				}
				m_d3dTexture = new D3D9Texture(m_device, m_color);
			}
			D3D9RenderTarget::~D3D9RenderTarget()
			{
				if (m_colorSurface)
				{
					m_colorSurface->Release();
					m_colorSurface = 0;
				}

				if (m_d3dTexture)
				{
					delete m_d3dTexture;
				}
				if (m_depthBuffer)
				{
					delete m_depthBuffer;
				}
			}

			Texture* D3D9RenderTarget::GetColorTexture()
			{
				if (m_rtDirty)
				{
					Resolve();
					m_rtDirty = false;
				}
				/*if (!m_d3dTexture)
				{
					throw AP_EXCEPTION(EX_InvalidOperation, L"Cannot get texture from default render target");
				}*/
				return m_d3dTexture;
			}
			DepthBuffer* D3D9RenderTarget::GetDepthBuffer()
			{
				return m_depthBuffer;
			}

			void D3D9RenderTarget::Resolve()
			{
				/** Because D3D9 RT texture is used and this RS is for Windows PC, 
				 *  only multisample ones need to resolve.
				 */
				if (isMultiSampled())
				{
					D3DDevice* dev = m_device->getDevice();
					IDirect3DSurface9* colorSuf;
					m_color->GetSurfaceLevel(0, &colorSuf);
					HRESULT hr = dev->StretchRect(m_colorSurface, NULL, colorSuf, NULL, D3DTEXF_NONE);
					assert(SUCCEEDED(hr));
					colorSuf->Release();

				}
			}
		}
	}
}