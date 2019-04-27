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

#include "RenderTarget.h"
#include "RenderDevice.h"
#include "apoc3d/Core/Logging.h"
#include "apoc3d/Utility/StringUtils.h"
#include "apoc3d/Math/Rectangle.h"

using namespace Apoc3D::Utility;

namespace Apoc3D
{
	namespace Graphics
	{
		namespace RenderSystem
		{
			RenderTarget2DBase::RenderTarget2DBase(RenderDevice* device, int32 width, int32 height, const String& multiSampleMode)
				: m_device(device), m_width(width), m_height(height),
				m_multisampleMode(multiSampleMode)
			{
				m_isMultisampled = !IsMultisampleModeStringNone(m_multisampleMode);
			}

			void RenderTarget2DBase::SetPercentageLock(float wp, float hp)
			{
				m_hasPercentangeLock = true;
				m_widthPercentage = wp;
				m_heightPercentage = hp;

				Point estSize = EvaluatePerferredSize();

				if (estSize.X != m_width || estSize.Y != m_height)
				{
					ApocLog(LOG_Graphics, L"[RT] Dimension percentage does not match current size.", LOGLVL_Warning);
				}
			}

			Point RenderTarget2DBase::EvaluatePerferredSize() const
			{
				assert(m_hasPercentangeLock);

				Viewport vp = m_device->getViewport();

				int32 estWidth = static_cast<int32>(vp.Width * m_widthPercentage + 0.5f);
				int32 estHeight = static_cast<int32>(vp.Height * m_heightPercentage + 0.5f);
				return Point(estWidth, estHeight);
			}
			bool RenderTarget2DBase::CheckAndUpdateDimensionBasedOnLock()
			{
				if (!m_hasPercentangeLock)
					return false;

				Point estSize = EvaluatePerferredSize();

				int32 estWidth = estSize.X;
				int32 estHeight = estSize.Y;

				if (estWidth != m_width || estHeight != m_height)
				{
					m_width = estWidth;
					m_height = estHeight;

					return true;
				}
				return false;
			}
			bool RenderTarget2DBase::IsMultisampleModeStringNone(const String& aamode)
			{
				if (aamode.empty())
					return true;

				String c = aamode;
				StringUtils::ToLowerCase(c);
				return c == L"none";
			}

			//////////////////////////////////////////////////////////////////////////

			RenderTarget::RenderTarget(RenderDevice* renderDevice, int32 width, int32 height, PixelFormat colorFormat, const String& multiSampleMode)
				: RenderTarget2DBase(renderDevice, width, height, multiSampleMode), m_pixelFormat(colorFormat)
			{ 
				m_isMultisampled = !IsMultisampleModeStringNone(m_multisampleMode);
			}

			RenderTarget::~RenderTarget()
			{

			}

			DataRectangle RenderTarget::Lock(LockMode mode, const Apoc3D::Math::Rectangle& rect)
			{
				if (!m_isLocked)
				{
					DataRectangle res = lock(mode, rect);
					m_isLocked = true;
					return res;
				}
				AP_EXCEPTION(ErrorID::InvalidOperation, L"RenderTarget is already locked.");
				return DataRectangle(0, 0, 0, 0, FMT_Unknown);
			}
			DataRectangle RenderTarget::Lock(LockMode mode)
			{
				return Lock(mode, Apoc3D::Math::Rectangle(0, 0, m_width, m_height));
			}

			void RenderTarget::Unlock()
			{
				if (m_isLocked)
				{
					unlock();
					m_isLocked = false;
				}
				else
				{
					AP_EXCEPTION(ErrorID::InvalidOperation, L"RenderTarget is not locked.");
				}
			}

			//////////////////////////////////////////////////////////////////////////

			DepthStencilBuffer::DepthStencilBuffer(RenderDevice* renderDevice, int32 width, int32 height, DepthFormat format)
				: RenderTarget2DBase(renderDevice, width, height, L""), m_depthFormat(format)
			{ }

			DepthStencilBuffer::DepthStencilBuffer(RenderDevice* renderDevice, int32 width, int32 height, DepthFormat format, const String& multiSampleMode)
				: RenderTarget2DBase(renderDevice, width, height, multiSampleMode), m_depthFormat(format)
			{
				m_isMultisampled = !RenderTarget::IsMultisampleModeStringNone(m_multisampleMode);
			}

			//////////////////////////////////////////////////////////////////////////

			CubemapRenderTarget::CubemapRenderTarget(int32 length, PixelFormat fmt)
				: m_length(length), m_pixelFormat(fmt) { }

			CubemapRenderTarget::~CubemapRenderTarget()
			{
			}

		}
	}
}