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

#include "D3D9DepthBuffer.h"

#include "D3D9RenderDevice.h"
#include "D3D9Utils.h"
#include "GraphicsDeviceManager.h"
#include "apoc3d/Math/MathCommon.h"
#include "apoc3d/Exception.h"

namespace Apoc3D
{
	namespace Graphics
	{
		namespace D3D9RenderSystem
		{
			D3D9DepthBuffer::D3D9DepthBuffer(D3D9RenderDevice* device, int32 width, int32 height, DepthFormat depthFormat, const String& multisampleMode)
				: DepthStencilBuffer(device, width, height, depthFormat, multisampleMode), VolatileResource(device), m_device(device)
			{
				ReloadVolatileResource();
			}
			D3D9DepthBuffer::D3D9DepthBuffer(D3D9RenderDevice* device, int32 width, int32 height, DepthFormat depthFormat, const D3D9Capabilities::AAProfile* aamode)
				: DepthStencilBuffer(device, width, height, depthFormat, aamode->Name), VolatileResource(device), m_device(device), m_explicitAAMode(aamode)
			{
				ReloadVolatileResource();
			}
			D3D9DepthBuffer::~D3D9DepthBuffer()
			{
				ReleaseVolatileResource();
			}


			void D3D9DepthBuffer::ReleaseVolatileResource() 
			{
				if (m_depthSurface)
				{
					m_depthSurface->Release();
					m_depthSurface = NULL;
				}
			}
			void D3D9DepthBuffer::ReloadVolatileResource() 
			{
				D3DDevice* dev = m_device->getDevice();

				CheckAndUpdateDimensionBasedOnLock();

				if (!isMultiSampled())
				{
					HRESULT hr = dev->CreateDepthStencilSurface(getWidth(), getHeight(),
						D3D9Utils::ConvertDepthFormat(getFormat()), D3DMULTISAMPLE_NONE, 0, TRUE, &m_depthSurface, NULL);
					assert(SUCCEEDED(hr));
				}
				else
				{
					GraphicsDeviceManager* dmgr = m_device->getGraphicsDeviceManager();
					const RawSettings* sets = dmgr->getCurrentSetting();

					D3D9Capabilities* caps = m_device->getD3D9Capabilities();
					const D3D9Capabilities::AAProfile* aamode = m_explicitAAMode ? m_explicitAAMode : caps->LookupAAProfile(getMultisampleMode(), FMT_Count, getFormat());
					if (aamode == nullptr)
					{
						D3D9Utils::logRTFailure(FMT_Count, getFormat(), getMultisampleMode());
						throw AP_EXCEPTION(ExceptID::NotSupported, L"");
					}

					DWORD qualityCount;
					HRESULT hr = dmgr->getDirect3D()->CheckDeviceMultiSampleType(
						sets->AdapterOrdinal, sets->DeviceType, D3D9Utils::ConvertDepthFormat(getFormat()), sets->PresentParameters.Windowed, aamode->SampleType, &qualityCount);
					if (hr != S_OK || qualityCount == 0)
						throw AP_EXCEPTION(ExceptID::NotSupported, L"");

					DWORD selectedQuality = Math::Min((uint32)aamode->SampleQuality, qualityCount - 1);

					hr = dev->CreateDepthStencilSurface(getWidth(), getHeight(),
						D3D9Utils::ConvertDepthFormat(getFormat()), aamode->SampleType, selectedQuality, TRUE, &m_depthSurface, NULL);
					assert(SUCCEEDED(hr));
				}

				// no event is bound if this function is called in the ctor
				eventReset.Invoke(this);
			}


		}
	}
}