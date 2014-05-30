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
#include "apoc3d/Math/Rectangle.h"
#include "apoc3d/Math/Box.h"
#include "apoc3d/Utility/StringUtils.h"
#include "apoc3d/ApocException.h"
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
				m_renderDevice(device), m_tex2D(tex2D), m_tex3D(NULL), m_cube(NULL),
				m_tempData(NULL)
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
				m_renderDevice(device), m_tex2D(NULL), m_tex3D(tex3D), m_cube(NULL),
				m_tempData(NULL)
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
				m_renderDevice(device), m_tex2D(NULL), m_tex3D(NULL), m_cube(texCube),
				m_tempData(NULL)
			{

			}

			D3D9Texture::D3D9Texture(D3D9RenderDevice* device, ResourceLocation* rl, TextureUsage usage, bool managed)
				: Texture(device, rl, usage, managed),  
				VolatileResource(device), 
				m_renderDevice(device), m_tex2D(NULL), m_tex3D(NULL), m_cube(NULL),
				m_tempData(NULL)
			{
				if (!managed)
				{
					load();
				}
			}
			D3D9Texture::D3D9Texture(D3D9RenderDevice* device, int32 width, int32 height, int32 depth, int32 level, 
				PixelFormat format, TextureUsage usage)
				: Texture(device, width, height, depth, level, format, usage), 
				VolatileResource(device), 
				m_renderDevice(device), m_tex2D(NULL), m_tex3D(NULL), m_cube(NULL),
				m_tempData(NULL)
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
				: Texture(device, length, level, usage, format), 
				VolatileResource(device), 
				m_renderDevice(device), m_tex2D(NULL), m_tex3D(NULL),
				m_tempData(NULL)
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
					m_tex2D = 0;
				}
				if (m_tex3D)
				{
					m_tex3D->Release();
					m_tex3D = 0;
				}
				if (m_cube)
				{
					m_cube->Release();
					m_cube = 0;
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

			// These are 2 way(getting and setting) utility functions
			// for accessing the content of D3D9 textures.
			void copyData(void* tex, 
				int pitch, void* texData, PixelFormat surfaceFormat,
				DWORD dwLockWidth, DWORD dwLockHeight, bool isSetting)
			{
				byte* texPtr = reinterpret_cast<byte*>(tex);
				byte* texDataPtr = reinterpret_cast<byte*>(texData);

				byte bytesPerPixel = (byte)PixelFormatUtils::GetBPP(surfaceFormat);// GetExpectedByteSizeFromFormat(surfaceFormat);


				bool isDxt = false;
				if (surfaceFormat == FMT_DXT1 || 
					surfaceFormat == FMT_DXT2 || 
					surfaceFormat == FMT_DXT3 || 
					surfaceFormat == FMT_DXT4 ||
					surfaceFormat == FMT_DXT5)
				{
					isDxt = true;
					
					dwLockWidth = (dwLockWidth + 3) >> 2;
					dwLockHeight = (dwLockHeight + 3) >> 2;

					bytesPerPixel = surfaceFormat == FMT_DXT1 ? 8 : 16;
				}
				if (dwLockHeight)
				{
					DWORD j = dwLockHeight;
					DWORD lineSize = dwLockWidth * bytesPerPixel;

					do 
					{
						if (isSetting)
						{
							memcpy(texPtr, texDataPtr, lineSize);
						}
						else
						{
							memcpy(texDataPtr, texPtr, lineSize);
						}
						texPtr += pitch;
						texDataPtr += lineSize;

						j--;
					} while (j>0);

				}
			}
			void copyData(void* tex,
				int rowPitch, int slicePitch, void* texData,
				PixelFormat surfaceFormat,
				DWORD dwLockWidth, DWORD dwLockHeight, DWORD dwLockDepth,
				bool isSetting)
			{
				byte* texPtr = reinterpret_cast<byte*>(tex);
				byte* texDataPtr = reinterpret_cast<byte*>(texData);

				byte bytesPerPixel = (byte)PixelFormatUtils::GetBPP(surfaceFormat);//(surfaceFormat);

				bool isDxt = false;
				if (surfaceFormat == FMT_DXT1 || 
					surfaceFormat == FMT_DXT2 || 
					surfaceFormat == FMT_DXT3 || 
					surfaceFormat == FMT_DXT4 ||
					surfaceFormat == FMT_DXT5)
				{
					isDxt = true;

					dwLockWidth = (dwLockWidth + 3) >> 2;
					dwLockHeight = (dwLockHeight + 3) >> 2;

					bytesPerPixel = surfaceFormat == FMT_DXT1 ? 8 : 16;
				}

				if (dwLockDepth)
				{
					DWORD k = dwLockDepth;
					do 
					{
						byte* ptr = texPtr;
						//byte* ptr2 = texDataPtr;

						if (dwLockHeight)
						{
							DWORD j = dwLockHeight;
							DWORD lineSize = dwLockWidth * bytesPerPixel;

							do 
							{
								if (isSetting)
								{
									memcpy(ptr, texDataPtr, lineSize);
								}
								else
								{
									memcpy(texDataPtr, ptr, lineSize);
								}
								ptr += rowPitch;
								texDataPtr += lineSize;
								j--;
							} while (j>0);
						}
						texPtr += slicePitch;
						k--;
					} while (k>0);
				}
			}

			void getData(TextureData& data, D3DTexture2D* tex)
			{
				for (int i=0;i<data.LevelCount;i++)
				{
					TextureLevelData lvlData;

					D3DSURFACE_DESC desc;
					tex->GetLevelDesc(i, &desc);

					D3DLOCKED_RECT rect;
					HRESULT hr = tex->LockRect(i, &rect, NULL, D3DLOCK_READONLY);
					assert(SUCCEEDED(hr));

					lvlData.Width = (int32)desc.Width;
					lvlData.Height = (int32)desc.Height;
					lvlData.Depth = 1;
					lvlData.LevelSize = PixelFormatUtils::GetMemorySize(lvlData.Width, lvlData.Height, 1, data.Format);
					lvlData.ContentData  = new char[lvlData.LevelSize];
					copyData(rect.pBits, rect.Pitch, lvlData.ContentData, data.Format, desc.Width, desc.Height, false);

					hr = tex->UnlockRect(i);
					assert(SUCCEEDED(hr));

					data.Levels.Add(lvlData);
					data.ContentSize += lvlData.LevelSize;
				}
			}
			void getData(TextureData& data, D3DTextureCube* tex)
			{
				for (int i=0;i<data.LevelCount;i++)
				{
					int startPos = 0;
					
					D3DSURFACE_DESC desc;
					tex->GetLevelDesc(i, &desc);
					TextureLevelData lvlData;
					lvlData.Width = (int32)desc.Width;
					lvlData.Height = (int32)desc.Width;
					lvlData.Depth = 1;
					lvlData.LevelSize = PixelFormatUtils::GetMemorySize(lvlData.Width, lvlData.Height, 1, data.Format) * 6;
					lvlData.ContentData  = new char[lvlData.LevelSize];
					int faceSize = data.Levels[i].LevelSize / 6;




					D3DLOCKED_RECT rect;
					HRESULT hr = tex->LockRect(D3DCUBEMAP_FACE_POSITIVE_X, i, &rect, NULL, D3DLOCK_READONLY);
					assert(SUCCEEDED(hr));

					copyData(rect.pBits, rect.Pitch, data.Levels[i].ContentData+startPos,
						data.Format, desc.Width, desc.Height, false);
					startPos += faceSize;

					hr = tex->UnlockRect(D3DCUBEMAP_FACE_POSITIVE_X, i);
					assert(SUCCEEDED(hr));

					// ======================================================================

					hr = tex->LockRect(D3DCUBEMAP_FACE_NEGATIVE_X, i, &rect, NULL, D3DLOCK_READONLY);
					assert(SUCCEEDED(hr));

					copyData(rect.pBits, rect.Pitch, data.Levels[i].ContentData+startPos,
						data.Format, desc.Width, desc.Height, false);
					startPos += faceSize;

					hr = tex->UnlockRect(D3DCUBEMAP_FACE_NEGATIVE_X, i);
					assert(SUCCEEDED(hr));

					// ======================================================================

					hr = tex->LockRect(D3DCUBEMAP_FACE_POSITIVE_Y, i, &rect, NULL, D3DLOCK_READONLY);
					assert(SUCCEEDED(hr));

					copyData(rect.pBits, rect.Pitch, data.Levels[i].ContentData+startPos,
						data.Format, desc.Width, desc.Height, false);
					startPos += faceSize;

					hr = tex->UnlockRect(D3DCUBEMAP_FACE_POSITIVE_Y, i);
					assert(SUCCEEDED(hr));

					// ======================================================================
					hr = tex->LockRect(D3DCUBEMAP_FACE_NEGATIVE_Y, i, &rect, NULL, D3DLOCK_READONLY);
					assert(SUCCEEDED(hr));

					copyData(rect.pBits, rect.Pitch, data.Levels[i].ContentData+startPos,
						data.Format, desc.Width, desc.Height, false);
					startPos += faceSize;

					hr = tex->UnlockRect(D3DCUBEMAP_FACE_NEGATIVE_Y, i);
					assert(SUCCEEDED(hr));

					// ======================================================================

					hr = tex->LockRect(D3DCUBEMAP_FACE_POSITIVE_Z, i, &rect, NULL, D3DLOCK_READONLY);
					assert(SUCCEEDED(hr));

					copyData(rect.pBits, rect.Pitch, data.Levels[i].ContentData+startPos,
						data.Format, desc.Width, desc.Height, false);
					startPos += faceSize;

					hr = tex->UnlockRect(D3DCUBEMAP_FACE_POSITIVE_Z, i);
					assert(SUCCEEDED(hr));
					// ======================================================================
					hr = tex->LockRect(D3DCUBEMAP_FACE_NEGATIVE_Z, i, &rect, NULL, D3DLOCK_READONLY);
					assert(SUCCEEDED(hr));

					copyData(rect.pBits, rect.Pitch, data.Levels[i].ContentData+startPos,
						data.Format, desc.Width, desc.Height, false);
					startPos += faceSize;

					hr = tex->UnlockRect(D3DCUBEMAP_FACE_NEGATIVE_Z, i);
					assert(SUCCEEDED(hr));

					// ======================================================================
					data.Levels.Add(lvlData);
					data.ContentSize += lvlData.LevelSize;

				}
			}
			void getData(TextureData& data, D3DTexture3D* tex)
			{
				for (int i=0;i<data.LevelCount;i++)
				{
					TextureLevelData lvlData;

					D3DVOLUME_DESC desc;
					tex->GetLevelDesc(i, &desc);
					
					lvlData.Width = (int32)desc.Width;
					lvlData.Height = (int32)desc.Height;
					lvlData.Depth = (int32)desc.Depth;
					lvlData.LevelSize = PixelFormatUtils::GetMemorySize(lvlData.Width, lvlData.Height, desc.Depth, data.Format);
					lvlData.ContentData  = new char[lvlData.LevelSize];

					D3DLOCKED_BOX box;
					HRESULT hr = tex->LockBox(i, &box, NULL, 0);
					assert(SUCCEEDED(hr));

					copyData(box.pBits, box.RowPitch, box.SlicePitch,
						data.Levels[i].ContentData, data.Format,
						desc.Width, desc.Height, desc.Depth, false);

					hr = tex->UnlockBox(i);
					assert(SUCCEEDED(hr));
				}
			}

			void setData(const TextureData& data, D3DTextureCube* tex)
			{
				for (int i=0;i<data.LevelCount;i++)
				{
					int startPos = 0;
					int faceSize = data.Levels[i].LevelSize / 6;
					D3DSURFACE_DESC desc;
					tex->GetLevelDesc(i, &desc);

					D3DLOCKED_RECT rect;
					HRESULT hr = tex->LockRect(D3DCUBEMAP_FACE_POSITIVE_X, i, &rect, NULL, 0);
					assert(SUCCEEDED(hr));

					copyData(rect.pBits, rect.Pitch, data.Levels[i].ContentData+startPos,
						data.Format, desc.Width, desc.Height, true);
					startPos += faceSize;

					hr = tex->UnlockRect(D3DCUBEMAP_FACE_POSITIVE_X, i);
					assert(SUCCEEDED(hr));

					// ======================================================================

					hr = tex->LockRect(D3DCUBEMAP_FACE_NEGATIVE_X, i, &rect, NULL, 0);
					assert(SUCCEEDED(hr));

					copyData(rect.pBits, rect.Pitch, data.Levels[i].ContentData+startPos,
						data.Format, desc.Width, desc.Height, true);
					startPos += faceSize;

					hr = tex->UnlockRect(D3DCUBEMAP_FACE_NEGATIVE_X, i);
					assert(SUCCEEDED(hr));

					// ======================================================================

					hr = tex->LockRect(D3DCUBEMAP_FACE_POSITIVE_Y, i, &rect, NULL, 0);
					assert(SUCCEEDED(hr));

					copyData(rect.pBits, rect.Pitch, data.Levels[i].ContentData+startPos,
						data.Format, desc.Width, desc.Height, true);
					startPos += faceSize;

					hr = tex->UnlockRect(D3DCUBEMAP_FACE_POSITIVE_Y, i);
					assert(SUCCEEDED(hr));

					// ======================================================================
					hr = tex->LockRect(D3DCUBEMAP_FACE_NEGATIVE_Y, i, &rect, NULL, 0);
					assert(SUCCEEDED(hr));

					copyData(rect.pBits, rect.Pitch, data.Levels[i].ContentData+startPos,
						data.Format, desc.Width, desc.Height, true);
					startPos += faceSize;

					hr = tex->UnlockRect(D3DCUBEMAP_FACE_NEGATIVE_Y, i);
					assert(SUCCEEDED(hr));

					// ======================================================================

					hr = tex->LockRect(D3DCUBEMAP_FACE_POSITIVE_Z, i, &rect, NULL, 0);
					assert(SUCCEEDED(hr));

					copyData(rect.pBits, rect.Pitch, data.Levels[i].ContentData+startPos,
						data.Format, desc.Width, desc.Height, true);
					startPos += faceSize;

					hr = tex->UnlockRect(D3DCUBEMAP_FACE_POSITIVE_Z, i);
					assert(SUCCEEDED(hr));
					// ======================================================================
					hr = tex->LockRect(D3DCUBEMAP_FACE_NEGATIVE_Z, i, &rect, NULL, 0);
					assert(SUCCEEDED(hr));

					copyData(rect.pBits, rect.Pitch, data.Levels[i].ContentData+startPos,
						data.Format, desc.Width, desc.Height, true);
					startPos += faceSize;

					hr = tex->UnlockRect(D3DCUBEMAP_FACE_NEGATIVE_Z, i);
					assert(SUCCEEDED(hr));

					// ======================================================================


				}
			}
			void setData(const TextureData& data, D3DTexture2D* tex)
			{
				for (int i=0;i<data.LevelCount;i++)
				{
					D3DSURFACE_DESC desc;
					tex->GetLevelDesc(i, &desc);

					D3DLOCKED_RECT rect;
					HRESULT hr = tex->LockRect(i, &rect, NULL, 0);
					assert(SUCCEEDED(hr));

					copyData(rect.pBits, rect.Pitch, data.Levels[i].ContentData, data.Format, desc.Width, desc.Height, true);

					hr = tex->UnlockRect(i);
					assert(SUCCEEDED(hr));
				}
			}
			void setData(const TextureData& data, D3DTexture3D* tex)
			{
				for (int i=0;i<data.LevelCount;i++)
				{
					D3DVOLUME_DESC desc;
					tex->GetLevelDesc(i, &desc);

					D3DLOCKED_BOX box;
					HRESULT hr = tex->LockBox(i, &box, NULL, 0);
					assert(SUCCEEDED(hr));

					copyData(box.pBits, box.RowPitch, box.SlicePitch,
						data.Levels[i].ContentData, data.Format,
						desc.Width, desc.Height, desc.Depth, true);

					hr = tex->UnlockBox(i);
					assert(SUCCEEDED(hr));
				}
			}

			void D3D9Texture::load()
			{
				// These 2 lines will load the data into memory from the ResourceLocation
				TextureData data;
				data.Load(getResourceLocation());
				
				// Before setting up the D3D9 texture objects, check if the 
				// device supports this kind of texture
				D3DDevice* dev = m_renderDevice->getDevice();

				D3DFORMAT newFmt;
				{
					//DWORD usage = D3D9Utils::ConvertTextureUsage(getUsage());
					D3DFORMAT fmt = D3D9Utils::ConvertPixelFormat(data.Format);
					UINT width = data.Levels[0].Width;
					UINT height = data.Levels[0].Height;
					UINT depth = data.Levels[0].Depth;
					UINT levels = data.LevelCount;



					newFmt = fmt;
					UINT newWidth = width;
					UINT newHeight = height;
					UINT newDepth = depth;

					// Checking whether the texture parameter is supported
					// to get rid of the occasion that a rare texture format
					// is not supported by an old hardware, causing the application to crash
					switch (data.Type)
					{
					case (int)TT_Texture1D:
					case (int)TT_Texture2D:
						{
							HRESULT hr = D3DXCheckTextureRequirements(dev, &newWidth, &newHeight, &levels,
								0, &newFmt, 
								D3DPOOL_MANAGED);
							assert(SUCCEEDED(hr));
							break;
						}
					case (int)TT_CubeTexture:
						{
							HRESULT hr = D3DXCheckCubeTextureRequirements(dev, &newWidth, &levels,
								0, &newFmt, 
								D3DPOOL_MANAGED);
							assert(SUCCEEDED(hr));
							break;
						}
					case (int)TT_Texture3D:
						{
							HRESULT hr = D3DXCheckVolumeTextureRequirements(dev,
								&newWidth, &newHeight, &newDepth,
								&levels, 0, &newFmt, D3DPOOL_MANAGED);
							assert(SUCCEEDED(hr));
							break;
						}
					}

					// resize the texture to the preferred size
					// usually happens when the hardware does not support non power of 2 textures
					if (newHeight != height || newWidth != width || newDepth != depth)
					{
						String name;
						const FileLocation* fl = dynamic_cast<const FileLocation*>(getResourceLocation());

						if (fl)
						{
							name = PathUtils::GetFileNameNoExt( fl->getPath());
						}
						else
						{
							name = getResourceLocation()->getName();
						}

						// TODO: resize here
						// 
						//PixelFormatUtils::Resize();

						LogManager::getSingleton().Write(LOG_Graphics, 
							L"[D3D9Texture]" + name +  L" Dimension " +
							StringUtils::UIntToString(width) + L"x" + StringUtils::UIntToString(height)
							+ L" is not supported by hardware. Resizing to " +
							StringUtils::UIntToString(newWidth) + L"x" + StringUtils::UIntToString(newHeight)
							, LOGLVL_Warning);
					}

					// automatically converts to the preferred format
					if (newFmt != fmt)
					{
						TextureData newdata;
						newdata.Format = D3D9Utils::ConvertBackPixelFormat(newFmt);
						newdata.ContentSize = 0;
						newdata.LevelCount = data.LevelCount;
						newdata.Type = data.Type;
						newdata.Levels.ResizeDiscard(data.LevelCount);
						newdata.Flags = data.Flags;

						String name;
						const FileLocation* fl = dynamic_cast<const FileLocation*>(getResourceLocation());

						if (fl)
						{
							name = PathUtils::GetFileNameNoExt( fl->getPath());
						}
						else
						{
							name = getResourceLocation()->getName();
						}
						LogManager::getSingleton().Write(LOG_Graphics, 
							L"[D3D9Texture]" + name + L" " + PixelFormatUtils::ToString(data.Format) 
							+ L" Pixel format is not supported by hardware. Converting to " +
							PixelFormatUtils::ToString(D3D9Utils::ConvertBackPixelFormat(newFmt)), LOGLVL_Warning);

						// do it for all levels
						for (int i=0;i<newdata.LevelCount;i++)
						{
							TextureLevelData& srcLvl = data.Levels[i];
							
							TextureLevelData dstLvl;
							dstLvl.Depth = srcLvl.Depth;
							dstLvl.Width = srcLvl.Width;
							dstLvl.Height = srcLvl.Height;

							int lvlSize = PixelFormatUtils::GetMemorySize(
								dstLvl.Width, dstLvl.Height, dstLvl.Depth, newdata.Format);
							dstLvl.LevelSize = lvlSize;

							dstLvl.ContentData = new char[lvlSize];
							newdata.ContentSize += lvlSize;

							DataBox src = DataBox(
								srcLvl.Width, 
								srcLvl.Height, 
								srcLvl.Depth, 
								PixelFormatUtils::GetMemorySize(srcLvl.Width, 1, 1, data.Format),
								PixelFormatUtils::GetMemorySize(srcLvl.Width, srcLvl.Height, 1, data.Format), 
								srcLvl.ContentData,
								data.Format);

							DataBox dst = DataBox(
								dstLvl.Width,
								dstLvl.Height, 
								dstLvl.Depth, 
								PixelFormatUtils::GetMemorySize(dstLvl.Width, 1, 1, newdata.Format),
								PixelFormatUtils::GetMemorySize(dstLvl.Width, dstLvl.Height, 1, newdata.Format), 
								dstLvl.ContentData,
								newdata.Format);

							int r = PixelFormatUtils::ConvertPixels(src, dst);
							assert(r);
							delete[] srcLvl.ContentData;

							newdata.Levels.Add(dstLvl);
						}

						data = newdata;
					}

				}
				
				// update using the checked data
				UpdateInfo(data);

			
				switch (data.Type)
				{
				case (int)TT_Texture1D:
				case (int)TT_Texture2D:
					{
						DWORD usage = D3D9Utils::ConvertTextureUsage(getUsage());
					
						HRESULT hr = dev->CreateTexture(getWidth(), getHeight(), getLevelCount(), 
							usage, newFmt, 
							D3DPOOL_MANAGED, &m_tex2D, NULL);
						assert(SUCCEEDED(hr));

						setData(data, m_tex2D);
					}					
					break;
				case (int)TT_CubeTexture:
					{
						DWORD usage = D3D9Utils::ConvertTextureUsage(getUsage());
						
						HRESULT hr = dev->CreateCubeTexture(getWidth(), getLevelCount(), 
							usage, newFmt, 
							D3DPOOL_MANAGED, &m_cube, NULL);
						assert(SUCCEEDED(hr));

						setData(data, m_cube);
					}
					
					break;
				case (int)TT_Texture3D:
					{
						DWORD usage = D3D9Utils::ConvertTextureUsage(getUsage());

						HRESULT hr = dev->CreateVolumeTexture(getWidth(), getHeight(), getDepth(), getLevelCount(),
							usage, newFmt, 
							D3DPOOL_MANAGED, &m_tex3D, NULL);
						assert(SUCCEEDED(hr));

						setData(data, m_tex3D);
					}
					break;
				}

				for (int i=0;i<data.LevelCount;i++)
				{
					delete[] data.Levels[i].ContentData;
				}
			}
			void D3D9Texture::unload()
			{
				if (m_tex2D)
				{
					m_tex2D->Release();
					m_tex2D = 0;
				}
				if (m_tex3D)
				{
					m_tex3D->Release();
					m_tex3D = 0;
				}
				if (m_cube)
				{
					m_cube->Release();
					m_cube = 0;
				}
			}


			void D3D9Texture::Save(Stream* strm)
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
				case (int)TT_Texture1D:
				case (int)TT_Texture2D:
					getData(data, m_tex2D);
					break;
				case (int)TT_CubeTexture:
					getData(data, m_cube);
					break;
				case (int)TT_Texture3D:
					getData(data, m_tex3D);
					break;
				}

				// The resource is free now.
				Unlock_Unloadable();

				data.Save(strm);

				for (int i=0;i<data.LevelCount;i++)
				{
					delete[] data.Levels[i].ContentData;
				}
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
						m_tex3D = 0;
					}
					else if (getType() == TT_CubeTexture)
					{
						m_cube->Release();
						m_cube = 0;
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


		}
	}
}