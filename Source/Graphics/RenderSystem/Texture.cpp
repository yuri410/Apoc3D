/*
-----------------------------------------------------------------------------
This source file is part of Apoc3D Engine

Copyright (c) 2009+ Tao Games

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
#include "Texture.h"
#include "VertexElement.h"

#include "RenderDevice.h"
#include "IO\TextureData.h"
#include "Apoc3DException.h"

namespace Apoc3D
{
	namespace Graphics
	{
		namespace RenderSystem
		{
			Texture::Texture(RenderDevice* device, const ResourceLocation* rl, TextureUsage usage, bool managed)
				: m_renderDevice(device), m_resourceLocation(rl), m_usage(usage),
				m_levelCount(0), m_width(0), m_height(0), m_depth(0), 
				m_format(FMT_Unknown), m_type(TT_Texture2D), m_contentSize(0),
				m_isLocked(false)
			{

			}
			Texture::Texture(RenderDevice* device, int32 width, int32 height, int32 depth, 
				int32 levelCount, PixelFormat format, TextureUsage usage)
				: m_renderDevice(device), m_resourceLocation(0), m_usage(usage),
				m_levelCount(levelCount), m_width(width), m_height(height),
				m_depth(depth),  m_format(format), 
				m_isLocked(false)
			{
				if (depth == 1)
				{
					if (width == 1 || height == 1)
					{
						m_type = TT_Texture1D;
					}
					else
					{
						m_type = TT_Texture2D;
					}
				}
				else
				{
					m_type = TT_Texture3D;
				}
				m_contentSize = PixelFormatUtils::GetMemorySize(width, height, 1, format);
			}
			Texture::Texture(RenderDevice* device, int length, int levelCount, TextureUsage usage, PixelFormat format)
				: m_renderDevice(device), m_resourceLocation(0), m_usage(usage), 
				m_levelCount(levelCount), m_width(length), m_height(length), 
				m_depth(1), m_format(format),
				m_type(TT_CubeTexture),
				m_isLocked(false)
			{
				m_contentSize = 6 * PixelFormatUtils::GetMemorySize(length, length, 1, format);
			}

			void Texture::UpdateInfo(const TextureData& data)
			{
				m_contentSize = data.ContentSize;
				m_depth = data.Levels[0].Depth;
				m_format = data.Format;
				m_height = data.Levels[0].Height;
				m_levelCount = data.LevelCount;
				m_type = data.Type;
				m_width = data.Levels[0].Width;
			}

			DataBox Texture::Lock(int surface, LockMode mode, const Box& box)
			{
				if (!m_isLocked)
				{
					if (m_type == TT_Texture3D)
					{

					}
				}
				throw Apoc3DException::createException(EX_InvalidOperation, L"Texture is already locked.");
			}
			DataRectangle Texture::Lock(int surface, LockMode mode)
			{
				if (!m_isLocked)
				{
					if (m_type == TT_Texture1D || m_type == TT_Texture2D)
					{

					}
				}
				throw Apoc3DException::createException(EX_InvalidOperation, L"Texture is already locked.");
			}
			DataRectangle Texture::Lock(int surface, LockMode mode, const Rectangle& rect)
			{
				if (!m_isLocked)
				{
					if (m_type == TT_Texture1D || m_type == TT_Texture2D)
					{

					}
				}
				throw Apoc3DException::createException(EX_InvalidOperation, L"Texture is already locked.");
			}
			DataRectangle Texture::Lock(int surface, LockMode mode, CubeMapFace cubemapFace)
			{
				if (!m_isLocked)
				{
					if (m_type == TT_CubeTexture)
					{

					}
				}
				throw Apoc3DException::createException(EX_InvalidOperation, L"Texture is already locked.");
			}
			DataRectangle Texture::Lock(int surface, LockMode mode, CubeMapFace cubemapFace, const Rectangle& rect)
			{
				if (!m_isLocked)
				{
					if (m_type == TT_CubeTexture)
					{

					}
				}
				throw Apoc3DException::createException(EX_InvalidOperation, L"Texture is already locked.");
			}

			void Texture::Unlock(int surface)
			{
				if (m_isLocked)
				{
					unlock(surface);
					m_isLocked = false;
				}
				throw Apoc3DException::createException(EX_InvalidOperation, L"Texture is not locked.");
			}
			void Texture::Unlock(CubeMapFace cubemapFace, int surface)
			{
				if (m_isLocked)
				{
					unlock(cubemapFace, surface);
					m_isLocked = false;
				}
				throw Apoc3DException::createException(EX_InvalidOperation, L"Texture is not locked.");
			}
			
		}
	}
}