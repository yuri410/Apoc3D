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