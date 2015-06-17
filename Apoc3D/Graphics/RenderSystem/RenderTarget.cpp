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
				throw AP_EXCEPTION(ExceptID::InvalidOperation, L"RenderTarget is already locked.");
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
					throw AP_EXCEPTION(ExceptID::InvalidOperation, L"RenderTarget is not locked.");
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