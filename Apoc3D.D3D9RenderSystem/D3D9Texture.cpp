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

#include "D3D9Texture.h"
#include "D3D9RenderDevice.h"
#include "D3D9Utils.h"
#include "D3D9TextureUtils.h"

#include "apoc3d/Math/Rectangle.h"
#include "apoc3d/Math/Box.h"
#include "apoc3d/Utility/StringUtils.h"
#include "apoc3d/Exception.h"
#include "apoc3d/Core/Logging.h"
#include "apoc3d/IOLib/Streams.h"
#include "apoc3d/IOLib/TextureData.h"
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
		namespace D3D9RenderSystem
		{
			D3D9Texture::D3D9Texture(D3D9RenderDevice* device, D3DTexture2D* tex2D)
				: Texture(device,
				D3D9Utils::GetD3DTextureWidth(tex2D), 
				D3D9Utils::GetD3DTextureHeight(tex2D), 1, 
				static_cast<int32>(tex2D->GetLevelCount()), 
				D3D9Utils::GetD3DTextureFormat(tex2D), 
				D3D9Utils::GetD3DTextureUsage(tex2D)), 
				VolatileResource(device), 
				m_renderDevice(device), m_tex2D(tex2D)
			{

			}
			D3D9Texture::D3D9Texture(D3D9RenderDevice* device, D3DTexture3D* tex3D)
				: Texture(device,
				D3D9Utils::GetD3DTextureWidth(tex3D), 
				D3D9Utils::GetD3DTextureHeight(tex3D), 
				D3D9Utils::GetD3DTextureDepth(tex3D), 
				static_cast<int32>(tex3D->GetLevelCount()), 
				D3D9Utils::GetD3DTextureFormat(tex3D), 
				D3D9Utils::GetD3DTextureUsage(tex3D)), 
				VolatileResource(device), 
				m_renderDevice(device), m_tex3D(tex3D)
			{
				
			}
			D3D9Texture::D3D9Texture(D3D9RenderDevice* device, D3DTextureCube* texCube)
				: Texture(device,
				D3D9Utils::GetD3DTextureLength(texCube), 
				D3D9Utils::GetD3DTextureLength(texCube), 
				1, 
				static_cast<int32>(texCube->GetLevelCount()), 
				D3D9Utils::GetD3DTextureFormat(texCube), 
				D3D9Utils::GetD3DTextureUsage(texCube)),  
				VolatileResource(device), 
				m_renderDevice(device), m_cube(texCube)
			{

			}

			D3D9Texture::D3D9Texture(D3D9RenderDevice* device, const ResourceLocation& rl, TextureUsage usage, bool managed)
				: Texture(device, rl, usage, managed), VolatileResource(device), 
				m_renderDevice(device)
			{
				if (!managed)
				{
					LoadTexture(&rl);
				}
			}
			D3D9Texture::D3D9Texture(D3D9RenderDevice* device, int32 width, int32 height, int32 depth, int32 level, 
				PixelFormat format, TextureUsage usage)
				: Texture(device, width, height, depth, level, format, usage), VolatileResource(device), 
				m_renderDevice(device)
			{
				D3DDevice* dev = device->getDevice();

				if (getType() == TT_Texture2D || getType() == TT_Texture1D)
				{
					HRESULT hr = dev->CreateTexture(width, height, level, 
						D3D9Utils::ConvertTextureUsage(usage), D3D9Utils::ConvertPixelFormat(format), 
						(usage & TU_Dynamic) ? D3DPOOL_DEFAULT : D3DPOOL_MANAGED, &m_tex2D, NULL);;
					assert(SUCCEEDED(hr));
				}
				else if (getType() == TT_Texture3D)
				{
					HRESULT hr = dev->CreateVolumeTexture(width, height,depth, level, 
						D3D9Utils::ConvertTextureUsage(usage), D3D9Utils::ConvertPixelFormat(format), 
						(usage & TU_Dynamic) ? D3DPOOL_DEFAULT : D3DPOOL_MANAGED, &m_tex3D, NULL);;
					assert(SUCCEEDED(hr));
				}
			}

			D3D9Texture::D3D9Texture(D3D9RenderDevice* device, int32 length, int32 level, PixelFormat format, TextureUsage usage)
				: Texture(device, length, level, usage, format), VolatileResource(device), 
				m_renderDevice(device)
			{
				D3DDevice* dev = device->getDevice();

				HRESULT hr = dev->CreateCubeTexture(length, level, 
					D3D9Utils::ConvertTextureUsage(usage), D3D9Utils::ConvertPixelFormat(format), 
					(usage & TU_Dynamic) ? D3DPOOL_DEFAULT : D3DPOOL_MANAGED, &m_cube, NULL);
				assert(SUCCEEDED(hr));
			}

			D3D9Texture::~D3D9Texture()
			{
				if (m_tex2D)
				{
					m_tex2D->Release();
					m_tex2D = nullptr;
				}
				if (m_tex3D)
				{
					m_tex3D->Release();
					m_tex3D = nullptr;
				}
				if (m_cube)
				{
					m_cube->Release();
					m_cube = nullptr;
				}


				if (m_tempData)
				{
					delete[] m_tempData;
					m_tempData = NULL;
				}
			}

			DataRectangle D3D9Texture::lock(int32 surface, LockMode mode, const Apoc3D::Math::Rectangle& rect)
			{
				assert(m_tex2D);
				D3DLOCKED_RECT rrect;
				RECT rect0;
				rect0.left = rect.X;
				rect0.top = rect.Y;
				rect0.right = rect.getRight();
				rect0.bottom = rect.getBottom();

				if (mode == LOCK_Discard)
				{
					// discard lock must be full region
					assert(rect.X == 0 && rect.Y == 0 && rect.Width == getWidth() && rect.Height == getHeight());

					HRESULT hr = m_tex2D->LockRect(surface, &rrect, NULL, D3D9Utils::ConvertLockMode(mode));
					assert(SUCCEEDED(hr));

					DataRectangle result(rrect.Pitch, rrect.pBits, rect.Width, rect.Height, getFormat());

					assert(result.getWidth() == getWidth() && result.getHeight() == getHeight());

					return result;
				}

				HRESULT hr = m_tex2D->LockRect(surface, &rrect, &rect0, D3D9Utils::ConvertLockMode(mode));
				assert(SUCCEEDED(hr));
				DataRectangle result(rrect.Pitch, rrect.pBits, rect.Width, rect.Height, getFormat());
				return result;
			}
			DataBox D3D9Texture::lock(int32 surface, LockMode mode, const Box& box)
			{
				assert(m_tex3D);
				D3DLOCKED_BOX rbox;
				D3DBOX box0;

				HRESULT hr = m_tex3D->LockBox(surface, &rbox, &box0, D3D9Utils::ConvertLockMode(mode));
				assert(SUCCEEDED(hr));
				
				DataBox result(box.getWidth(), box.getHeight(), box.getDepth(), rbox.RowPitch, rbox.SlicePitch, rbox.pBits, getFormat());
				return result;
			}
			DataRectangle D3D9Texture::lock(int32 surface, CubeMapFace cubemapFace, LockMode mode, const Apoc3D::Math::Rectangle& rect)
			{
				assert(m_cube);			
				D3DLOCKED_RECT rrect;
				RECT rect0;
				rect0.left = rect.X;
				rect0.top = rect.Y;
				rect0.right = rect.getRight();
				rect0.bottom = rect.getBottom();

				m_lockedCubeFace = D3D9Utils::ConvertCubeMapFace(cubemapFace);
				HRESULT hr = m_cube->LockRect(m_lockedCubeFace, surface, &rrect, &rect0, D3D9Utils::ConvertLockMode(mode));
				assert(SUCCEEDED(hr));

				DataRectangle result(rrect.Pitch, rrect.pBits, rect.Width, rect.Height, getFormat());
				return result;
			}
			void D3D9Texture::unlock(int32 surface)
			{
				TextureType type = getType();
				if (type == TT_Texture3D)
				{
					m_tex3D->UnlockBox(surface);
				}
				else if (type != TT_CubeTexture)
				{
					m_tex2D->UnlockRect(surface);
				}
				else
				{
					m_cube->UnlockRect(m_lockedCubeFace, surface);
				}
			}
			void D3D9Texture::unlock(CubeMapFace cubemapFace, int32 surface)
			{
				m_cube->UnlockRect(D3D9Utils::ConvertCubeMapFace(cubemapFace), surface);
			}




			void D3D9Texture::LoadTexture(const ResourceLocation* rl)
			{
				// load texture data into memory from the ResourceLocation
				TextureData data;
				data.Load(*rl);

				D3DDevice* dev = m_renderDevice->getDevice();
				D3D9Capabilities* caps = static_cast<D3D9Capabilities*>(m_renderDevice->getCapabilities());

				bool impossible = false;

				// Before setting up the D3D9 texture objects, check if the 
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
							case TT_Texture1D:
							case TT_Texture2D:
								impossible |= !caps->FindCompatibleTextureDimension(newWidth, newHeight, levels);
								break;
							case TT_CubeTexture:
								impossible |= !caps->FindCompatibleCubeTextureDimension(newWidth, levels);
								break;
							case TT_Texture3D:
								impossible |= !caps->FindCompatibleVolumeTextureDimension(newWidth, newHeight, newDepth, levels);
								break;
						}
					}


					// Currently not handling volume texture resizing
					if (!impossible && data.Type == TT_Texture3D &&
						(newHeight != height || newWidth != width || newDepth != depth))
					{
						String name = getResourceLocationName(rl);
						ApocLog(LOG_Graphics, L"[D3D9Texture] Texture " + name + L" can not be resized.", LOGLVL_Warning);
						impossible = true;
					}

					if (impossible)
					{
						String name = getResourceLocationName(rl);
						ApocLog(LOG_Graphics, L"[D3D9Texture] Texture " + name + L" impossible to support.", LOGLVL_Error);
					}
					else
					{
						// resize the texture to the preferred size
						// usually happens when the hardware does not support non power of 2 textures
						if ((newHeight != height || newWidth != width) && (newDepth == 1) && (depth == 1))
						{
							String name = getResourceLocationName(rl);

							ApocLog(LOG_Graphics,
								L"[D3D9Texture]" + name + L" Dimension " +
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
								L"[D3D9Texture]" + name + L" " + PixelFormatUtils::ToString(data.Format)
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
					DWORD usage = D3D9Utils::ConvertTextureUsage(getUsage());
					HRESULT hr;

					switch (data.Type)
					{
						case TT_Texture1D:
						case TT_Texture2D:
							hr = dev->CreateTexture(getWidth(), getHeight(), getLevelCount(),
								usage, D3D9Utils::ConvertPixelFormat(newFmt),
								D3DPOOL_MANAGED, &m_tex2D, NULL);
							assert(SUCCEEDED(hr));

							if (FAILED(hr))
							{
								String name = getResourceLocationName(rl);
								String msg = L"[D3D9Texture] Failed creating " + name + L" " + PixelFormatUtils::ToString(newFmt);
								msg.append(L" ");
								msg += StringUtils::IntToString(getWidth());
								msg.append(L"x");
								msg += StringUtils::IntToString(getHeight());
								msg.append(L" L=");
								msg += StringUtils::IntToString(getLevelCount());

								ApocLog(LOG_Graphics, msg, LOGLVL_Warning);
							}
							else
							{
								setData(data, m_tex2D);
							}
							break;

						case TT_CubeTexture:
							hr = dev->CreateCubeTexture(getWidth(), getLevelCount(),
								usage, D3D9Utils::ConvertPixelFormat(newFmt),
								D3DPOOL_MANAGED, &m_cube, NULL);
							assert(SUCCEEDED(hr));

							if (FAILED(hr))
							{
								String name = getResourceLocationName(rl);
								String msg = L"[D3D9Texture] Failed creating " + name + L" " + PixelFormatUtils::ToString(newFmt);
								msg.append(L" ");
								msg += StringUtils::IntToString(getWidth());
								msg.append(L" L=");
								msg += StringUtils::IntToString(getLevelCount());

								ApocLog(LOG_Graphics, msg, LOGLVL_Warning);
							}
							else
							{
								setData(data, m_cube);
							}
							break;

						case TT_Texture3D:
							hr = dev->CreateVolumeTexture(getWidth(), getHeight(), getDepth(), getLevelCount(),
								usage, D3D9Utils::ConvertPixelFormat(newFmt),
								D3DPOOL_MANAGED, &m_tex3D, NULL);
							assert(SUCCEEDED(hr));

							if (FAILED(hr))
							{
								String name = getResourceLocationName(rl);
								String msg = L"[D3D9Texture] Failed creating " + name + L" " + PixelFormatUtils::ToString(newFmt);
								msg.append(L" ");
								msg += StringUtils::IntToString(getWidth());
								msg.append(L"x");
								msg += StringUtils::IntToString(getHeight());
								msg.append(L"x");
								msg += StringUtils::IntToString(getDepth());
								msg.append(L" L=");
								msg += StringUtils::IntToString(getLevelCount());

								ApocLog(LOG_Graphics, msg, LOGLVL_Warning);
							}
							else
							{
								setData(data, m_tex3D);
							}
							break;
					}
				}
				else
				{
					m_tex2D = nullptr;
					m_tex3D = nullptr;
					m_cube = nullptr;
				}

			}
			
			void D3D9Texture::load()
			{
				LoadTexture(getResourceLocation());
			}
			void D3D9Texture::unload()
			{
				if (m_tex2D)
				{
					m_tex2D->Release();
					m_tex2D = nullptr;
				}
				if (m_tex3D)
				{
					m_tex3D->Release();
					m_tex3D = nullptr;
				}
				if (m_cube)
				{
					m_cube->Release();
					m_cube = nullptr;
				}
			}


			void D3D9Texture::Save(Stream& strm)
			{
				// First lock the resource to keep it safe.
				// This resource will not be unloaded by the background resource management
				// in any case until the unlock method is called
				Lock_Unloadable();

				// After securing it, ensure it is loaded.
				UseSync();
				
				TextureData data;
				data.LevelCount = getLevelCount();
				data.Type = getType();
				data.Format = getFormat();
				data.ContentSize = 0;
				data.Flags = TextureData::TDF_None;
				switch (data.Type)
				{
					case TT_Texture1D:
					case TT_Texture2D:
						getData(data, m_tex2D);
						break;
					case TT_CubeTexture:
						getData(data, m_cube);
						break;
					case TT_Texture3D:
						getData(data, m_tex3D);
						break;
				}

				// The resource is free now.
				Unlock_Unloadable();

				data.Save(strm);
			}

			void D3D9Texture::ReleaseVolatileResource()
			{
				if (getUsage() & TU_Dynamic)
				{
					if (m_tempData)
					{
						delete[] m_tempData;
						m_tempData = NULL;
					}

					if (getType() == TT_Texture2D || getType() == TT_Texture1D)
					{
						DataRectangle dr = lock(0, LOCK_ReadOnly, Apoc3D::Math::Rectangle(0,0,getWidth(), getHeight()));
						m_tempData = new char[dr.getMemorySize()];

						int32 rowSize = PixelFormatUtils::GetMemorySize(getWidth(), 1, 1, dr.getFormat());
						for (int32 i=0;i<dr.getHeight();i++)
						{
							memcpy(m_tempData + rowSize * i, (char*)dr.getDataPointer() + dr.getPitch() * i, rowSize);
						}

						unlock(0);

						m_tex2D->Release();
						m_tex2D = NULL;
					}
					else if (getType() == TT_Texture3D)
					{
						m_tex3D->Release();
						m_tex3D = NULL;
					}
					else if (getType() == TT_CubeTexture)
					{
						m_cube->Release();
						m_cube = NULL;
					}
				}
			}
			void D3D9Texture::ReloadVolatileResource()
			{
				if (getUsage() & TU_Dynamic)
				{
					D3D9RenderDevice* device = m_renderDevice;
					D3DDevice* dev = device->getDevice();
					TextureUsage usage = getUsage();
					int32 level = getLevelCount();
					PixelFormat format = getFormat();

					if (getType() == TT_Texture2D || getType() == TT_Texture1D)
					{
						HRESULT hr = dev->CreateTexture(getWidth(), getHeight(), level, 
							D3D9Utils::ConvertTextureUsage(usage), D3D9Utils::ConvertPixelFormat(format), 
							(usage & TU_Dynamic) ? D3DPOOL_DEFAULT : D3DPOOL_MANAGED, &m_tex2D, NULL);;
						assert(SUCCEEDED(hr));
					}
					else if (getType() == TT_Texture3D)
					{
						HRESULT hr = dev->CreateVolumeTexture(getWidth(), getHeight(), getDepth(), level, 
							D3D9Utils::ConvertTextureUsage(usage), D3D9Utils::ConvertPixelFormat(format), 
							(usage & TU_Dynamic) ? D3DPOOL_DEFAULT : D3DPOOL_MANAGED, &m_tex3D, NULL);;
						assert(SUCCEEDED(hr));
					}
					else if (getType() == TT_CubeTexture)
					{
						HRESULT hr = dev->CreateCubeTexture(getWidth(), level, 
							D3D9Utils::ConvertTextureUsage(usage), D3D9Utils::ConvertPixelFormat(format), 
							(usage & TU_Dynamic) ? D3DPOOL_DEFAULT : D3DPOOL_MANAGED, &m_cube, NULL);
						assert(SUCCEEDED(hr));
					}

					if (m_tempData)
					{
						if (getType() == TT_Texture2D || getType() == TT_Texture1D)
						{
							DataRectangle dr = lock(0, LOCK_Discard, Apoc3D::Math::Rectangle(0,0,getWidth(), getHeight()));
							
							int32 rowSize = PixelFormatUtils::GetMemorySize(getWidth(), 1, 1, dr.getFormat());
							for (int32 i=0;i<dr.getHeight();i++)
							{
								memcpy((char*)dr.getDataPointer() + dr.getPitch() * i, m_tempData + rowSize * i, rowSize);
							}

							unlock(0);
						}

						delete[] m_tempData;
						m_tempData = NULL;
					}
				}
			}

			void D3D9Texture::SetInternal2D(D3DTexture2D* tex, int32 newWidth, int32 newHeight, int32 newLevelCount, PixelFormat newFormat)
			{
				m_tex2D = tex;

				UpdateProperties(TT_Texture2D, newWidth, newHeight, 1, 
					newLevelCount, newFormat, D3D9Utils::GetD3DTextureUsage(tex));
			}


			String D3D9Texture::getResourceLocationName(const ResourceLocation* rl)
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