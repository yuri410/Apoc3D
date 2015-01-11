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
#include "Texture.h"

#include "VertexElement.h"
#include "RenderDevice.h"

#include "apoc3d/Graphics/TextureManager.h"
#include "apoc3d/IOLib/TextureData.h"
#include "apoc3d/Math/Rectangle.h"
#include "apoc3d/Math/Box.h"
#include "apoc3d/Exception.h"
#include "apoc3d/VFS/ResourceLocation.h"
#include "apoc3d/Library/squish.h"

namespace Apoc3D
{
	namespace Graphics
	{
		namespace RenderSystem
		{
			Texture::Texture(RenderDevice* device, const ResourceLocation& rl, TextureUsage usage, bool managed)
				: Resource(managed ? &TextureManager::getSingleton() : 0, rl.getName()),
				m_renderDevice(device), m_resourceLocation(managed ? rl.Clone() : nullptr), m_usage(usage),
				m_format(FMT_Unknown), m_type(TT_Texture2D)
			{

			}

			Texture::Texture(RenderDevice* device, int32 width, int32 height, int32 depth, 
				int32 levelCount, PixelFormat format, TextureUsage usage)
				: m_renderDevice(device), m_usage(usage),
				m_levelCount(levelCount), m_width(width), m_height(height),
				m_depth(depth),  m_format(format)
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
				RecalculateContentSize();
			}
			Texture::Texture(RenderDevice* device, int length, int levelCount, TextureUsage usage, PixelFormat format)
				: m_renderDevice(device), m_usage(usage), 
				m_levelCount(levelCount), m_width(length), m_height(length), 
				m_depth(1), m_format(format),
				m_type(TT_CubeTexture)
			{
				RecalculateContentSize();
			}
			Texture::~Texture()
			{
				DELETE_AND_NULL(m_resourceLocation);
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
			void Texture::UpdateProperties(TextureType type, int width, int height, int depth, int levelCount, PixelFormat format, TextureUsage usage)
			{
				m_type = type;
				m_width = width;
				m_height = height;
				m_depth = depth;
				m_levelCount = levelCount;
				m_format = format;
				m_usage = usage;
				RecalculateContentSize();
			}
			void Texture::RecalculateContentSize()
			{
				int width = m_width;
				int height = m_height;
				int depth = m_depth;

				m_contentSize = 0;

				// One assumption is made here:
				//  All sub levels' size in the texture is the same as the mipmap's.
				for (int i = 0; i < m_levelCount; i++)
				{
					m_contentSize += PixelFormatUtils::GetMemorySize(width, height, depth, m_format);

					if (width > 1) width = width / 2;//>>= 1;
					if (height > 1) height = height / 2;// >>= 1;
					if (depth > 1) depth = depth / 2;//>>= 1;
				}

				if (m_type == TT_CubeTexture)
					m_contentSize *= 6;
			}


			DataBox Texture::Lock(int surface, LockMode mode, const Box& box)
			{
				if (!m_isLocked)
				{
					if (m_type == TT_Texture3D)
					{
						if (isManaged() && !isLoaded())
						{
							return DataBox::Empty;
						}
						else
						{
							DataBox res = lock(surface, mode, box);
							m_isLocked = true;
							return res;
						}
					}
					throw AP_EXCEPTION(ExceptID::InvalidOperation, L"Texture is not volume.");
				}
				throw AP_EXCEPTION(ExceptID::InvalidOperation, L"Texture is already locked.");
			}

			DataRectangle Texture::Lock(int surface, LockMode mode)
			{
				return Lock(surface, mode, Apoc3D::Math::Rectangle(0, 0, m_width, m_height));
			}
			DataRectangle Texture::Lock(int surface, LockMode mode, const Apoc3D::Math::Rectangle& rect)
			{
				if (!m_isLocked)
				{
					if (m_type == TT_Texture1D || m_type == TT_Texture2D)
					{
						if (isManaged() && !isLoaded())
						{
							return DataRectangle::Empty;
						}
						else
						{
							DataRectangle res = lock(surface, mode, rect);
							m_isLocked = true;
							return res;
						}
					}
				}
				throw AP_EXCEPTION(ExceptID::InvalidOperation, L"Texture is already locked.");
			}

			DataRectangle Texture::Lock(int surface, LockMode mode, CubeMapFace cubemapFace)
			{
				return Lock(surface, mode, cubemapFace, Apoc3D::Math::Rectangle(0, 0, m_width, m_height));
			}
			DataRectangle Texture::Lock(int surface, LockMode mode, CubeMapFace cubemapFace, const Apoc3D::Math::Rectangle& rect)
			{
				if (!m_isLocked)
				{
					if (m_type == TT_CubeTexture)
					{
						if (isManaged() && !isLoaded())
						{
							return DataRectangle::Empty;
						}
						else
						{
							DataRectangle res = lock(surface, cubemapFace, mode, rect);
							m_isLocked = true;
							return res;
						}
					}
				}
				throw AP_EXCEPTION(ExceptID::InvalidOperation, L"Texture is already locked.");
			}

			void Texture::Unlock(int surface)
			{
				if (m_isLocked)
				{
					unlock(surface);
					m_isLocked = false;
				}
				else
				{
					throw AP_EXCEPTION(ExceptID::InvalidOperation, L"Texture is not locked.");
				}
				
			}
			void Texture::Unlock(CubeMapFace cubemapFace, int surface)
			{
				if (m_isLocked)
				{
					unlock(cubemapFace, surface);
					m_isLocked = false;
				}
				else
				{
					throw AP_EXCEPTION(ExceptID::InvalidOperation, L"Texture is not locked.");
				}
			}
			
			void Texture::FillColor(ColorValue cv)
			{
				if (m_type == TT_Texture2D || m_type == TT_Texture1D)
					FillColor(cv, Apoc3D::Math::Rectangle(0, 0, m_width, m_height));
				else if (m_type == TT_Texture3D)
					;
				else
					;
			}

			void Texture::FillColor(ColorValue cv, const Apoc3D::Math::Rectangle& rect)
			{
				DataRectangle dr = Lock(0, LOCK_None);
				char* dst = (char*)dr.getDataPointer();

				if (PixelFormatUtils::IsCompressed(m_format))
				{
					squish::u8 colorInBlock[16 * 4];

					for (int32 i = 0; i < 16; i++)
					{
						colorInBlock[i * 4 + 0] = CV_GetColorR(cv);
						colorInBlock[i * 4 + 1] = CV_GetColorG(cv);
						colorInBlock[i * 4 + 2] = CV_GetColorB(cv);
						colorInBlock[i * 4 + 3] = CV_GetColorA(cv);
					}

					squish::u8 block[16] = { 0 };

					int32 blockSize = 0;

					int squishFlags = 0;

					switch (m_format)
					{
						case Apoc3D::Graphics::FMT_DXT1:
							squishFlags |= squish::kDxt1;
							blockSize = 8;
							break;
						case Apoc3D::Graphics::FMT_DXT3:
							squishFlags |= squish::kDxt3;
							blockSize = 16;
							break;
						case Apoc3D::Graphics::FMT_DXT5:
							squishFlags |= squish::kDxt5;
							blockSize = 16;
							break;
						default:
							throw AP_EXCEPTION(ExceptID::NotSupported, L"Format not supported for filling.");
					}

					squish::Compress(colorInBlock, block, squishFlags);

					// iterate through 2d array of blocks
					for (int y = 0; y < dr.getHeight(); y += 4)
					{
						char* writeLine = dst + dr.getPitch() * y;

						for (int x = 0; x < dr.getWidth(); x += 4)
						{
							memcpy(writeLine, block, blockSize);
							writeLine += blockSize;
						}
					}
				}
				else
				{
					switch (m_format)
					{
						case Apoc3D::Graphics::FMT_Luminance8:
							break;
						case Apoc3D::Graphics::FMT_Luminance16:
							break;
						case Apoc3D::Graphics::FMT_Alpha8:
							break;
						case Apoc3D::Graphics::FMT_A4L4:
							break;
						case Apoc3D::Graphics::FMT_A8L8:
							break;
						case Apoc3D::Graphics::FMT_R5G6B5:
							break;
						case Apoc3D::Graphics::FMT_B5G6R5:
							break;
						case Apoc3D::Graphics::FMT_A4R4G4B4:
							break;
						case Apoc3D::Graphics::FMT_A1R5G5B5:
							break;
						case Apoc3D::Graphics::FMT_R8G8B8:
							break;
						case Apoc3D::Graphics::FMT_B8G8R8:
							break;
						case Apoc3D::Graphics::FMT_A8R8G8B8:
							for (int32 y = 0; y < dr.getHeight(); y++)
							{
								for (int32 x = 0; x < dr.getWidth(); x++)
									*(((uint32*)dst) + x) = cv;
								dst += dr.getPitch();
							}

							break;
						case Apoc3D::Graphics::FMT_A8B8G8R8:
							break;
						case Apoc3D::Graphics::FMT_B8G8R8A8:
							break;
						case Apoc3D::Graphics::FMT_A2R10G10B10:
							break;
						case Apoc3D::Graphics::FMT_A2B10G10R10:
							break;

						case Apoc3D::Graphics::FMT_A16B16G16R16F:
							break;
						case Apoc3D::Graphics::FMT_A32B32G32R32F:
							break;
						case Apoc3D::Graphics::FMT_X8R8G8B8:
							break;
						case Apoc3D::Graphics::FMT_X8B8G8R8:
							break;
						case Apoc3D::Graphics::FMT_X1R5G5B5:
							break;
						case Apoc3D::Graphics::FMT_R8G8B8A8:
							break;
						case Apoc3D::Graphics::FMT_A16B16G16R16:
							break;
						case Apoc3D::Graphics::FMT_R3G3B2:
							break;
						case Apoc3D::Graphics::FMT_R16F:
							break;
						case Apoc3D::Graphics::FMT_R32F:
							break;
						case Apoc3D::Graphics::FMT_G16R16:
							break;
						case Apoc3D::Graphics::FMT_G16R16F:
							break;
						case Apoc3D::Graphics::FMT_G32R32F:
							break;
						case Apoc3D::Graphics::FMT_R16G16B16:
							break;
						case Apoc3D::Graphics::FMT_B4G4R4A4:
							break;
						default:
							throw AP_EXCEPTION(ExceptID::NotSupported, L"Format not supported for filling.");
					}
				}
				

				Unlock(0);
			}
		}
	}
}