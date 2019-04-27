
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

#include "NRSRenderTarget.h"

#include "NRSTexture.h"
#include "NRSRenderDevice.h"

#include "apoc3d/Core/Logging.h"
#include "apoc3d/Math/Rectangle.h"
#include "apoc3d/Math/MathCommon.h"
#include "apoc3d/Utility/StringUtils.h"

using namespace Apoc3D::Utility;

namespace Apoc3D
{
	namespace Graphics
	{
		namespace NullRenderSystem
		{
			NRSRenderTarget::NRSRenderTarget(NRSRenderDevice* device, int32 width, int32 height, PixelFormat format, const String& multisampleMode)
				: RenderTarget(device, width, height, format, multisampleMode), m_device(device)
			{
				m_colorWrapperTex = new NRSTexture(m_device, width, height, 1, 1, format, TU_Default);
			}
			
			NRSRenderTarget::NRSRenderTarget(NRSRenderDevice* device, int32 width, int32 height, PixelFormat format, const NRSCapabilities::AAProfile* explicitAAMode)
				: RenderTarget(device, width, height, format, explicitAAMode->Name), m_explicitAAMode(explicitAAMode)
			{
				m_colorWrapperTex = new NRSTexture(m_device, width, height, 1, 1, format, TU_Default);
			}

			NRSRenderTarget::~NRSRenderTarget()
			{
				DELETE_AND_NULL(m_colorWrapperTex);
			}

			Texture* NRSRenderTarget::GetColorTexture()
			{
				return m_colorWrapperTex;
			}

			void NRSRenderTarget::PrecacheLockedData()
			{
				
			}

			DataRectangle NRSRenderTarget::lock(LockMode mode, const Apoc3D::Math::Rectangle& rect)
			{
				PrecacheLockedData();
				
				return m_colorWrapperTex->Lock(0, mode, rect);
			}

			void NRSRenderTarget::unlock()
			{
				m_colorWrapperTex->Unlock(0);
			}

			//////////////////////////////////////////////////////////////////////////

			NRSCubemapRenderTarget::RefRenderTarget::RefRenderTarget(NRSRenderDevice* device, int32 width, int32 height, PixelFormat fmt, NRSCubemapRenderTarget* parent)
				: RenderTarget(device, width, height, fmt, L""), m_parent(parent)
			{ }

			NRSCubemapRenderTarget::RefRenderTarget::~RefRenderTarget()
			{ }

			//////////////////////////////////////////////////////////////////////////

			NRSCubemapRenderTarget::NRSCubemapRenderTarget(NRSRenderDevice* device, int32 length, PixelFormat format)
				: CubemapRenderTarget(length, format), m_device(device)
			{
				PixelFormat fmt = getColorFormat();

				for (int32 i = 0; i < CUBE_Count; i++)
				{
					m_faces.Add(new RefRenderTarget(m_device, m_length, m_length, getColorFormat(), this));
				}

				m_cubeMapWrapper = new NRSTexture(m_device, m_length, 1, format, TU_Default);
			}

			NRSCubemapRenderTarget::~NRSCubemapRenderTarget()
			{
				m_faces.DeleteAndClear();

				DELETE_AND_NULL(m_cubeMapWrapper);
			}

			Texture* NRSCubemapRenderTarget::GetColorTexture()
			{
				return m_cubeMapWrapper;
			}

		}
	}
}