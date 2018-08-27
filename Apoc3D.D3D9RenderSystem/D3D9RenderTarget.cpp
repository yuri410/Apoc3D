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

#include "D3D9RenderTarget.h"

#include "D3D9Texture.h"
#include "D3D9RenderDevice.h"
#include "D3D9Utils.h"
#include "GraphicsDeviceManager.h"

#include "apoc3d/Exception.h"

#include "apoc3d/Core/Logging.h"
#include "apoc3d/Math/Rectangle.h"
#include "apoc3d/Math/MathCommon.h"
#include "apoc3d/Utility/StringUtils.h"

using namespace Apoc3D::Utility;

namespace Apoc3D
{
	namespace Graphics
	{
		namespace D3D9RenderSystem
		{
			void D3D9RenderTarget::ReleaseVolatileResource()
			{
				if (m_color)
				{
					m_colorSurface->Release();
					m_color->Release();
					m_colorWrapperTex->ClearInternalTextureRef();
				}
			}

			void D3D9RenderTarget::ReloadVolatileResource()
			{
				// recreating from a device lost needs some work
				D3DDevice* dev = m_device->getDevice();

				bool hasSizeChanged = CheckAndUpdateDimensionBasedOnLock();

				if (!isMultiSampled())
				{
					HRESULT hr = dev->CreateTexture(getWidth(), getHeight(), 1, D3DUSAGE_RENDERTARGET,
						D3D9Utils::ConvertPixelFormat(getColorFormat()), D3DPOOL_DEFAULT, &m_color, NULL);
					assert(SUCCEEDED(hr));

					m_color->GetSurfaceLevel(0, &m_colorSurface);

					if (m_colorWrapperTex)
						m_colorWrapperTex->SetInternal2D(m_color, getWidth(), getHeight(), 1, getColorFormat());
					else
						m_colorWrapperTex = new D3D9Texture(m_device, m_color);
				}
				else
				{
					GraphicsDeviceManager* dmgr = m_device->getGraphicsDeviceManager();
					const RawSettings* sets = dmgr->getCurrentSetting();

					D3D9Capabilities* caps = m_device->getD3D9Capabilities();
					const D3D9Capabilities::AAProfile* aamode = m_explicitAAMode ? m_explicitAAMode : caps->LookupAAProfile(getMultisampleMode(), getColorFormat(), DEPFMT_Count);
					if (aamode == nullptr)
					{
						D3D9Utils::logRTFailure(getColorFormat(), DEPFMT_Count, getMultisampleMode());
						throw AP_EXCEPTION(ExceptID::NotSupported, L"");
					}

					DWORD qualityCount;
					HRESULT hr = dmgr->getDirect3D()->CheckDeviceMultiSampleType(
						sets->AdapterOrdinal, sets->DeviceType, D3D9Utils::ConvertPixelFormat(getColorFormat()), sets->PresentParameters.Windowed, aamode->SampleType, &qualityCount);
					if (hr != S_OK || qualityCount == 0)
						throw AP_EXCEPTION(ExceptID::NotSupported, L"");

					DWORD selectedQuality = Math::Min((uint32)aamode->SampleQuality, qualityCount - 1);

					hr = dev->CreateRenderTarget(getWidth(), getHeight(),
						D3D9Utils::ConvertPixelFormat(getColorFormat()), aamode->SampleType, selectedQuality, FALSE, &m_colorSurface, NULL);
					assert(SUCCEEDED(hr));

					hr = dev->CreateTexture(getWidth(), getHeight(), 1, D3DUSAGE_RENDERTARGET,
						D3D9Utils::ConvertPixelFormat(getColorFormat()), D3DPOOL_DEFAULT, &m_color, NULL);
					assert(SUCCEEDED(hr));

					if (m_colorWrapperTex)
						m_colorWrapperTex->SetInternal2D(m_color, getWidth(), getHeight(), 1, getColorFormat());
					else
						m_colorWrapperTex = new D3D9Texture(m_device, m_color);
				}

				if (hasSizeChanged && m_offscreenPlainSurface)
				{
					// m_offscreenPlainSurface is the lock surface that always needs to match current size
					if (m_offscreenPlainSurface)
						m_offscreenPlainSurface->Release();

					CreateLockingSurface();
				}

				// no event is bound if this function is called in the ctor
				eventReseted.Invoke(this);
			}

			D3D9RenderTarget::D3D9RenderTarget(D3D9RenderDevice* device, int32 width, int32 height, PixelFormat format, const String& multisampleMode)
				: RenderTarget(device, width, height, format, multisampleMode), VolatileResource(device), m_device(device)
			{
				ReloadVolatileResource();
			}
			
			D3D9RenderTarget::D3D9RenderTarget(D3D9RenderDevice* device, int32 width, int32 height, PixelFormat format, const D3D9Capabilities::AAProfile* explicitAAMode)
				: RenderTarget(device, width, height, format, explicitAAMode->Name), VolatileResource(device), m_explicitAAMode(explicitAAMode)
			{
				ReloadVolatileResource();
			}

			D3D9RenderTarget::~D3D9RenderTarget()
			{
				ReleaseVolatileResource();

				if (m_offscreenPlainSurface)
				{
					m_offscreenPlainSurface->Release();
					m_offscreenPlainSurface = nullptr;
				}

				DELETE_AND_NULL(m_colorWrapperTex);
			}

			Texture* D3D9RenderTarget::GetColorTexture()
			{
				EnsureResolvedTexture();
				return m_colorWrapperTex;
			}

			void D3D9RenderTarget::EnsureResolvedTexture()
			{
				if (m_isResolvedRTDirty)
				{
					m_isResolvedRTDirty = false;
					Resolve();
				}
			}

			void D3D9RenderTarget::Resolve()
			{
				// resolve multisample surface to a texture
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

			void D3D9RenderTarget::PrecacheLockedData()
			{
				if (m_offscreenPlainSurface == nullptr)
				{
					CreateLockingSurface();
				}

				if (m_isLockSurfaceDirty)
				{
					D3DDevice* dev = m_device->getDevice();

					if (isMultiSampled())
					{
						EnsureResolvedTexture();

						IDirect3DSurface9* colorSuf;
						m_color->GetSurfaceLevel(0, &colorSuf);
						HRESULT hr = dev->GetRenderTargetData(colorSuf, m_offscreenPlainSurface);
						assert(SUCCEEDED(hr));
						colorSuf->Release();
					}
					else
					{
						HRESULT hr = dev->GetRenderTargetData(m_colorSurface, m_offscreenPlainSurface);
						assert(SUCCEEDED(hr));
					}

					m_isLockSurfaceDirty = false;
				}
			}

			void D3D9RenderTarget::CreateLockingSurface()
			{
				D3DDevice* dev = m_device->getDevice();
				HRESULT hr = dev->CreateOffscreenPlainSurface(m_width, m_height, D3D9Utils::ConvertPixelFormat(getColorFormat()), 
					D3DPOOL_SYSTEMMEM, &m_offscreenPlainSurface, NULL);
				assert(SUCCEEDED(hr));
			}

			DataRectangle D3D9RenderTarget::lock(LockMode mode, const Apoc3D::Math::Rectangle& rect)
			{
				PrecacheLockedData();
				
				RECT rect0;
				rect0.left = rect.X;
				rect0.top = rect.Y;
				rect0.right = rect.getRight();
				rect0.bottom = rect.getBottom();

				D3DLOCKED_RECT rrect;
				HRESULT hr = m_offscreenPlainSurface->LockRect(&rrect, &rect0, D3D9Utils::ConvertLockMode(mode));
				assert(SUCCEEDED(hr));

				return DataRectangle(rrect.Pitch, rrect.pBits, rect.Width, rect.Height, getColorFormat());
			}

			void D3D9RenderTarget::unlock()
			{
				HRESULT hr = m_offscreenPlainSurface->UnlockRect();
				assert(SUCCEEDED(hr));
			}

			//////////////////////////////////////////////////////////////////////////
			D3D9CubemapRenderTarget::RefRenderTarget::RefRenderTarget(D3D9RenderDevice* device, int32 width, int32 height, PixelFormat fmt, IDirect3DSurface9* s, D3D9CubemapRenderTarget* parent)
				: RenderTarget(device, width, height, fmt, L""), m_colorSurface(s), m_parent(parent)
			{ }
			D3D9CubemapRenderTarget::RefRenderTarget::~RefRenderTarget()
			{
				if (m_colorSurface)
				{
					m_colorSurface->Release();
					m_colorSurface = NULL;
				}
			}

			//////////////////////////////////////////////////////////////////////////

			D3D9CubemapRenderTarget::D3D9CubemapRenderTarget(D3D9RenderDevice* device, int32 length, PixelFormat format)
				: CubemapRenderTarget(length, format), VolatileResource(device), m_device(device)
			{
				ReloadVolatileResource();
			}

			D3D9CubemapRenderTarget::~D3D9CubemapRenderTarget()
			{
				ReleaseVolatileResource();

				DELETE_AND_NULL(m_cubeMapWrapper);
			}

			void D3D9CubemapRenderTarget::ReleaseVolatileResource() 
			{
				m_faces.DeleteAndClear();

				m_cubemap->Release();
				m_cubeMapWrapper->ClearInternalTextureRef();
			}
			void D3D9CubemapRenderTarget::ReloadVolatileResource() 
			{
				D3DDevice* dev = m_device->getDevice();

				PixelFormat fmt = getColorFormat();

				HRESULT hr = dev->CreateCubeTexture(m_length, 1, D3DUSAGE_RENDERTARGET, D3D9Utils::ConvertPixelFormat(fmt), D3DPOOL_DEFAULT, &m_cubemap, NULL);
				assert(SUCCEEDED(hr));
				if (FAILED(hr))
					D3D9Utils::logRTFailure(fmt, DEPFMT_Count, L"");

				for (int32 i = 0; i < CUBE_Count; i++)
				{
					IDirect3DSurface9* pFace;
					m_cubemap->GetCubeMapSurface((D3DCUBEMAP_FACES)i, 0, &pFace);

					m_faces.Add(new RefRenderTarget(m_device, m_length, m_length, getColorFormat(), pFace, this));
				}

				if (m_cubeMapWrapper == nullptr)
					m_cubeMapWrapper = new D3D9Texture(m_device, m_cubemap);
				else
					m_cubeMapWrapper->SetInternalCube(m_cubemap, m_length, 1, getColorFormat());

				// no event is bound if this function is called in the ctor
				eventReset.Invoke(this);
			}
			Texture* D3D9CubemapRenderTarget::GetColorTexture() 
			{
				return m_cubeMapWrapper;
			}

		}
	}
}