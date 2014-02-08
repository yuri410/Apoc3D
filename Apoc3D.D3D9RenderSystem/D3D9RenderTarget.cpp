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

			void D3D9RenderTarget::ReleaseVolatileResource()
			{
				if (m_isDefault)
					return;

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
				if (m_isDefault)
					return;

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

				if (!getMultiSampleCount())
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
					D3DMULTISAMPLE_TYPE mms = D3D9Utils::ConvertMultisample(getMultiSampleCount());

					DWORD quality;
					GraphicsDeviceManager* dmgr = m_device->getGraphicsDeviceManager();
					const DeviceSettings* sets = dmgr->getCurrentSetting();
					HRESULT hr = dmgr->getDirect3D()->CheckDeviceMultiSampleType(
						sets->D3D9.AdapterOrdinal, sets->D3D9.DeviceType, D3D9Utils::ConvertPixelFormat(getColorFormat()), sets->D3D9.PresentParameters.Windowed, mms, &quality);
					if (hr != S_OK)
					{
						throw AP_EXCEPTION(EX_NotSupported, L"");
					}
					DWORD quality2;
					hr = dmgr->getDirect3D()->CheckDeviceMultiSampleType(
						sets->D3D9.AdapterOrdinal, sets->D3D9.DeviceType, D3D9Utils::ConvertDepthFormat(getDepthFormat()), sets->D3D9.PresentParameters.Windowed, mms, &quality2);

					if (hr != S_OK)
					{
						throw AP_EXCEPTION(EX_NotSupported, L"");
					}

					quality = min(quality,quality2);
					if (m_hasColor)
					{
						hr = dev->CreateRenderTarget(getWidth(), getHeight(), 
							D3D9Utils::ConvertPixelFormat(getColorFormat()), mms, quality - 1, FALSE, &m_colorSurface, NULL);
						assert(SUCCEEDED(hr));

						hr = dev->CreateTexture(getWidth(), getHeight(), 1, D3DUSAGE_RENDERTARGET, 
							D3D9Utils::ConvertPixelFormat(getColorFormat()), D3DPOOL_DEFAULT, &m_color, NULL);
						assert(SUCCEEDED(hr));

						m_d3dTexture->setInternal2D(m_color);
					}
					

					if (m_hasDepth)
					{
						hr = dev->CreateDepthStencilSurface(getWidth(), getHeight(), 
							D3D9Utils::ConvertDepthFormat(getDepthFormat()), mms, quality - 1, TRUE, &m_depthSurface, NULL);
						assert(SUCCEEDED(hr));

						m_depthBuffer->setD3DBuffer(m_depthSurface);
					}
					
				}
			}

			void logRTFailure(PixelFormat colorFormat, DepthFormat depFormat, uint32 multisample)
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
				if (multisample != 0)
				{
					if (formatString.size())
						formatString.append(L", ");
					formatString.append(L"M=");
					formatString.append(StringUtils::ToString(multisample));
				}

				ApocLog(LOG_Graphics, L"[D3D9] RenderTarget format not supported: " + formatString, LOGLVL_Error);
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
				D3D9Utils::GetD3DTextureFormat(rt), 0), VolatileResource(device),
				m_device(device), m_color(rt), m_d3dTexture(0), m_depthSurface(0), m_depthBuffer(0),
				m_isDefault(false), m_hasDepth(false), m_hasColor(true),
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
				GetDepthSurfaceFormat(depth), 0), VolatileResource(device),
				m_device(device), m_color(rt), m_d3dTexture(0), m_depthSurface(depth),
				m_isDefault(false), m_hasDepth(true), m_hasColor(true),
				m_rtDirty(false)
			{
				m_color->GetSurfaceLevel(0, &m_colorSurface);
				m_depthBuffer = new D3D9DepthBuffer(device, depth);
				m_d3dTexture = new D3D9Texture(m_device, m_color);
			}

			D3D9RenderTarget::D3D9RenderTarget(D3D9RenderDevice* device, int32 width, int32 height, PixelFormat format)
				: RenderTarget(device, width, height, format, 0), VolatileResource(device),
				m_device(device), m_depthSurface(0), m_depthBuffer(0), m_d3dTexture(0),
				m_isDefault(false), m_hasDepth(false), m_hasColor(true),
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
				: RenderTarget(device, width, height, format, depthFormat, 0), VolatileResource(device),
				m_device(device),
				m_isDefault(false), m_hasDepth(true), m_hasColor(true),
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
				uint32 sampleCount, PixelFormat format, DepthFormat depthFormat)
				: RenderTarget(device, width, height, format, depthFormat, sampleCount), VolatileResource(device),
				m_device(device),
				m_isDefault(false), m_hasDepth(true), m_hasColor(true),
				m_rtDirty(false)
			{
				D3DDevice* dev = device->getDevice();

				if (!sampleCount)
				{
					HRESULT hr = dev->CreateTexture(width, height, 1, D3DUSAGE_RENDERTARGET, 
						D3D9Utils::ConvertPixelFormat(format), D3DPOOL_DEFAULT, &m_color, NULL);
					assert(SUCCEEDED(hr));
					if (FAILED(hr))
						logRTFailure(format, DEPFMT_Count, sampleCount);
					m_color->GetSurfaceLevel(0, &m_colorSurface);

					hr = dev->CreateDepthStencilSurface(width, height, 
						D3D9Utils::ConvertDepthFormat(depthFormat), D3DMULTISAMPLE_NONE, 0, TRUE, &m_depthSurface, NULL);
					assert(SUCCEEDED(hr));
					if (FAILED(hr))
						logRTFailure(FMT_Count, depthFormat, sampleCount);

					m_depthBuffer = new D3D9DepthBuffer(device, m_depthSurface);
				}
				else
				{
					D3DMULTISAMPLE_TYPE mms = D3D9Utils::ConvertMultisample(sampleCount);

					DWORD quality;
					GraphicsDeviceManager* dmgr = m_device->getGraphicsDeviceManager();
					const DeviceSettings* sets = dmgr->getCurrentSetting();
					HRESULT hr = dmgr->getDirect3D()->CheckDeviceMultiSampleType(
						sets->D3D9.AdapterOrdinal, sets->D3D9.DeviceType, D3D9Utils::ConvertPixelFormat(format), sets->D3D9.PresentParameters.Windowed, mms, &quality);
					if (hr != S_OK)
					{
						throw AP_EXCEPTION(EX_NotSupported, L"");
					}
					DWORD quality2;
					hr = dmgr->getDirect3D()->CheckDeviceMultiSampleType(
						sets->D3D9.AdapterOrdinal, sets->D3D9.DeviceType, D3D9Utils::ConvertDepthFormat(depthFormat), sets->D3D9.PresentParameters.Windowed, mms, &quality2);

					if (hr != S_OK)
					{
						throw AP_EXCEPTION(EX_NotSupported, L"");
					}

					hr = dev->CreateRenderTarget(width, height, 
						D3D9Utils::ConvertPixelFormat(format), mms, quality - 1, FALSE, &m_colorSurface, NULL);
					assert(SUCCEEDED(hr));

					hr = dev->CreateTexture(width, height, 1, D3DUSAGE_RENDERTARGET, 
						D3D9Utils::ConvertPixelFormat(format), D3DPOOL_DEFAULT, &m_color, NULL);
					assert(SUCCEEDED(hr));


					hr = dev->CreateDepthStencilSurface(width, height, 
						D3D9Utils::ConvertDepthFormat(depthFormat), mms, quality2 - 1, TRUE, &m_depthSurface, NULL);
					assert(SUCCEEDED(hr));

					m_depthBuffer = new D3D9DepthBuffer(device, m_depthSurface);
				}
				m_d3dTexture = new D3D9Texture(m_device, m_color);
			}
			D3D9RenderTarget::D3D9RenderTarget(D3D9RenderDevice* device, int32 width, int32 height, uint32 sampleCount, PixelFormat format)
				: RenderTarget(device, width, height, format, sampleCount), VolatileResource(device),
				m_device(device), m_depthSurface(0), m_depthBuffer(0), m_d3dTexture(0),
				m_isDefault(false), m_hasDepth(false), m_hasColor(true),
				m_rtDirty(false)
			{
				D3DDevice* dev = device->getDevice();

				if (!sampleCount)
				{
					HRESULT hr = dev->CreateTexture(width, height, 1, D3DUSAGE_RENDERTARGET, 
						D3D9Utils::ConvertPixelFormat(format), D3DPOOL_DEFAULT, &m_color, NULL);
					assert(SUCCEEDED(hr));
					if (FAILED(hr))
						logRTFailure(format, DEPFMT_Count, sampleCount);

					m_color->GetSurfaceLevel(0, &m_colorSurface);
				}
				else
				{
					D3DMULTISAMPLE_TYPE mms = D3D9Utils::ConvertMultisample(sampleCount);

					DWORD quality;
					GraphicsDeviceManager* dmgr = m_device->getGraphicsDeviceManager();
					const DeviceSettings* sets = dmgr->getCurrentSetting();
					HRESULT hr = dmgr->getDirect3D()->CheckDeviceMultiSampleType(
						sets->D3D9.AdapterOrdinal, sets->D3D9.DeviceType, D3D9Utils::ConvertPixelFormat(format), sets->D3D9.PresentParameters.Windowed, mms, &quality);
					if (hr != S_OK)
					{
						throw AP_EXCEPTION(EX_NotSupported, L"");
					}

					hr = dev->CreateRenderTarget(width, height, 
						D3D9Utils::ConvertPixelFormat(format), mms, quality - 1, FALSE, &m_colorSurface, NULL);
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
				if (getMultiSampleCount())
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