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
			RenderTarget::RenderTarget(RenderDevice* renderDevice, int32 width, int32 height, PixelFormat colorFormat, DepthFormat depthFormat, const String& multiSampleMode)
				: m_device(renderDevice), m_width(width), m_height(height), 
				m_pixelFormat(colorFormat), m_depthFormat(depthFormat), m_multisampleMode(multiSampleMode)
			{
				m_isMultisampled = !IsMultisampleModeStringNone(m_multisampleMode);
			}

			RenderTarget::RenderTarget(RenderDevice* renderDevice, int32 width, int32 height, PixelFormat colorFormat, const String& multiSampleMode)
				: m_device(renderDevice), m_width(width), m_height(height), 
				m_pixelFormat(colorFormat), m_depthFormat(DEPFMT_Count), m_multisampleMode(multiSampleMode)
			{ 
				m_isMultisampled = !IsMultisampleModeStringNone(m_multisampleMode);
			}

			RenderTarget::~RenderTarget()
			{

			}

			bool RenderTarget::IsMultisampleModeStringNone(const String& aamode)
			{
				if (aamode.empty())
					return true;

				String c = aamode;
				StringUtils::ToLowerCase(c);
				return c == L"none";
			}

			void RenderTarget::SetPercentageLock(float wp, float hp)
			{
				Viewport vp = m_device->getViewport();

				int estWidth = static_cast<int>(vp.Width * wp + 0.5f);
				int estHeight = static_cast<int>(vp.Height * hp + 0.5f);
				m_hasPercentangeLock = true;
				m_widthPercentage = wp;
				m_heightPercentage = hp;

				if (estWidth != m_width || estHeight != m_height)
				{
					ApocLog(LOG_Graphics, L"[RT] Dimension percentage does not match current size.", LOGLVL_Warning);
				}
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

		}
	}
}