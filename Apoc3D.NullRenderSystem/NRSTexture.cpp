
/* -----------------------------------------------------------------------
* This source file is part of Apoc3D Framework
*
* Copyright (c) 2009+ Tao Xin
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

#include "NRSTexture.h"
#include "NRSRenderDevice.h"

#include "apoc3d/Math/Rectangle.h"
#include "apoc3d/Math/Box.h"
#include "apoc3d/Utility/StringUtils.h"
#include "apoc3d/Exception.h"
#include "apoc3d/Core/Logging.h"
#include "apoc3d/IOLib/Streams.h"
#include "apoc3d/Vfs/PathUtils.h"
#include "apoc3d/Vfs/ResourceLocation.h"


using namespace Apoc3D::VFS;
using namespace Apoc3D::IO;
using namespace Apoc3D::Core;
using namespace Apoc3D::Utility;

namespace Apoc3D
{
	namespace Graphics
	{
		namespace NullRenderSystem
		{
			NRSTexture::NRSTexture(NRSRenderDevice* device, const ResourceLocation& rl, TextureUsage usage, bool managed)
				: Texture(device, rl, usage, managed),
				m_renderDevice(device)
			{
				if (!managed)
				{
					LoadTexture(&rl);
				}
			}

			void CreateEmptyData(TextureData& data, TextureType type, int32 width, int32 height, int32 depth, int32 level, PixelFormat format)
			{
				data.LevelCount = level;
				data.Type = type;
				data.Format = format;
				data.ContentSize = PixelFormatUtils::GetMemorySize(width, height, depth, format);
				data.Flags = TextureData::TDF_None;

				data.Levels.ReserveDiscard(level);
				for (int32 i = 0; i < level; i++)
				{
					TextureLevelData& lvl = data.Levels[i];
					lvl.Width = width;
					lvl.Height = height;
					lvl.Depth = depth;
					lvl.LevelSize = PixelFormatUtils::GetMemorySize(lvl.Width, lvl.Height, lvl.Depth, format);

					if (type == TextureType::CubeTexture)
						lvl.LevelSize *= 6;

					if (lvl.ContentData)
						delete[] lvl.ContentData;

					lvl.ContentData = new char[lvl.LevelSize]();

					width = (width + 1) / 2;
					height = (height + 1) / 2;
					depth = (depth + 1) / 2;
				}
			}

			NRSTexture::NRSTexture(NRSRenderDevice* device, int32 width, int32 height, int32 depth, int32 level, PixelFormat format, TextureUsage usage)
				: Texture(device, width, height, depth, level, format, usage), m_renderDevice(device)
			{
				CreateEmptyData(m_data, getType(), width, height, depth, level, format);
			}

			NRSTexture::NRSTexture(NRSRenderDevice* device, int32 length, int32 level, PixelFormat format, TextureUsage usage)
				: Texture(device, length, level, usage, format), m_renderDevice(device)
			{
				CreateEmptyData(m_data, getType(), length, length, 1, level, format);
			}

			NRSTexture::~NRSTexture()
			{
				
			}

			DataRectangle NRSTexture::lock(int32 surface, LockMode mode, const Apoc3D::Math::Rectangle& rect)
			{
				int32 pitch = PixelFormatUtils::GetBPP(getFormat()) * getWidth();
				void* data = m_data.Levels[surface].ContentData + pitch * rect.Y;

				if (mode == LOCK_Discard)
				{
					// discard lock must be full region
					assert(rect.X == 0 && rect.Y == 0 && rect.Width == getWidth() && rect.Height == getHeight());

					DataRectangle result(pitch, data, rect.Width, rect.Height, getFormat());

					assert(result.getWidth() == getWidth() && result.getHeight() == getHeight());

					return result;
				}

				DataRectangle result(pitch, data, rect.Width, rect.Height, getFormat());
				return result;
			}
			DataBox NRSTexture::lock(int32 surface, LockMode mode, const Box& box)
			{
				int32 rowPitch = PixelFormatUtils::GetBPP(getFormat()) * getWidth();
				int32 slicePitch = PixelFormatUtils::GetBPP(getFormat()) * getWidth() * getHeight();
				void* data = m_data.Levels[surface].ContentData + rowPitch * box.Top + slicePitch * box.Front;

				DataBox result(box.getWidth(), box.getHeight(), box.getDepth(), rowPitch, slicePitch, data, getFormat());
				return result;
			}
			DataRectangle NRSTexture::lock(int32 surface, CubeMapFace cubemapFace, LockMode mode, const Apoc3D::Math::Rectangle& rect)
			{
				int32 rowPitch = PixelFormatUtils::GetBPP(getFormat()) * getWidth();
				void* data = m_data.Levels[surface].ContentData + rowPitch * rect.Y + PixelFormatUtils::GetMemorySize(getWidth(), getHeight(), 1, getFormat()) * cubemapFace;

				DataRectangle result(rowPitch, data, rect.Width, rect.Height, getFormat());
				return result;
			}
			void NRSTexture::unlock(int32 surface)
			{
				
			}

			void NRSTexture::unlock(CubeMapFace cubemapFace, int32 surface)
			{
				
			}


			void NRSTexture::LoadTexture(const ResourceLocation* rl)
			{
				// load texture data into memory from the ResourceLocation
				TextureData data;
				data.Load(*rl);

				NRSCapabilities* caps = static_cast<NRSCapabilities*>(m_renderDevice->getCapabilities());

				bool impossible = false;

				// Check if the 
				// device supports this kind of texture
				PixelFormat newFmt;
				{
					PixelFormat fmt = data.Format;
					int32 width = data.Levels[0].Width;
					int32 height = data.Levels[0].Height;
					int32 depth = data.Levels[0].Depth;
					int32 levels = data.LevelCount;


					newFmt = fmt;
					int32 newWidth = width;
					int32 newHeight = height;
					int32 newDepth = depth;

					// Checking whether the texture format/size is possible to be supported.
					// Convert as needed.
					impossible |= !caps->FindCompatibleTextureFormat(newFmt);

					if (!impossible)
					{
						switch (data.Type)
						{
							case TextureType::Texture1D:
							case TextureType::Texture2D:
								impossible |= !caps->FindCompatibleTextureDimension(newWidth, newHeight, levels);
								break;
							case TextureType::CubeTexture:
								impossible |= !caps->FindCompatibleCubeTextureDimension(newWidth, levels);
								break;
							case TextureType::Texture3D:
								impossible |= !caps->FindCompatibleVolumeTextureDimension(newWidth, newHeight, newDepth, levels);
								break;
						}
					}


					// Currently not handling volume texture resizing
					if (!impossible && data.Type == TextureType::Texture3D &&
						(newHeight != height || newWidth != width || newDepth != depth))
					{
						String name = getResourceLocationName(rl);
						ApocLog(LOG_Graphics, L"[NRSTexture] Texture " + name + L" can not be resized.", LOGLVL_Warning);
						impossible = true;
					}

					if (impossible)
					{
						String name = getResourceLocationName(rl);
						ApocLog(LOG_Graphics, L"[NRSTexture] Texture " + name + L" impossible to support.", LOGLVL_Error);
					}
					else
					{
						// resize the texture to the preferred size
						// usually happens when the hardware does not support non power of 2 textures
						if ((newHeight != height || newWidth != width) && (newDepth == 1) && (depth == 1))
						{
							String name = getResourceLocationName(rl);

							ApocLog(LOG_Graphics,
								L"[NRSTexture]" + name + L" Dimension " +
								StringUtils::IntToString(width) + L"x" + StringUtils::IntToString(height)
								+ L" is not supported by hardware. Resizing to " +
								StringUtils::IntToString(newWidth) + L"x" + StringUtils::IntToString(newHeight)
								, LOGLVL_Warning);

							data.ResizeInPlace(newWidth, newHeight);
						}

						// automatically converts to the preferred format
						if (newFmt != fmt)
						{
							String name = getResourceLocationName(rl);

							ApocLog(LOG_Graphics,
								L"[NRSTexture]" + name + L" " + PixelFormatUtils::ToString(data.Format)
								+ L" Pixel format is not supported by hardware. Converting to " +
								PixelFormatUtils::ToString(newFmt), LOGLVL_Warning);

							data.ConvertInPlace(newFmt);
						}

					}
				}

				// update using the checked data
				UpdateInfo(data);

				if (!impossible)
				{
					m_data = std::move(data);
				}
			}
			
			void NRSTexture::load()
			{
				LoadTexture(getResourceLocation());
			}
			void NRSTexture::unload()
			{
				
			}

			void NRSTexture::Save(Stream& strm)
			{
				// First lock the resource to keep it safe.
				// This resource will not be unloaded by the background resource management
				// in any case until the unlock method is called
				Lock_Unloadable();

				// After securing it, ensure it is loaded.
				UseSync();
				
				// The resource is free now.
				Unlock_Unloadable();

				m_data.Save(strm);
			}

			String NRSTexture::getResourceLocationName(const ResourceLocation* rl)
			{
				if (rl)
				{
					const FileLocation* fl = up_cast<const FileLocation*>(rl);

					if (fl)
					{
						return PathUtils::GetFileNameNoExt(fl->getPath());
					}
					return rl->getName();
				}
				return getHashString();
			}
		}
	}
}