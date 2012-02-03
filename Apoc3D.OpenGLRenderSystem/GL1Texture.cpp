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

#include "GL1Texture.h"
#include "GL1RenderDevice.h"
#include "GLUtils.h"
#include "Math/Rectangle.h"
#include "Math/Box.h"
#include "Vfs/ResourceLocation.h"
#include "IOLib/TextureData.h"
#include "Utility/StringUtils.h"
#include "Apoc3DException.h"
#include "Core/Logging.h"
#include "IOLib/Streams.h"
#include "Vfs/PathUtils.h"


using namespace Apoc3D::VFS;
using namespace Apoc3D::IO;
using namespace Apoc3D::Core;
using namespace Apoc3D::Utility;

namespace Apoc3D
{
	namespace Graphics
	{
		namespace GL1RenderSystem
		{
			/**
			  You are probably looking for glGetTexImage
			  (see http://www.opengl.org/sdk/docs/man/xhtml/glGetTexImage.xml for further information).

			  Before using glGetTexImage, don't forget to use 
			  glBindTexture (http://www.opengl.org/sdk/docs/man/xhtml/glBindTexture.xml) with your texture ID.
			*/


			/** Assuming that OpenGL will select an alternative 
				internalFormat if the suggested one is not supported,
				no manual pixel format conversion is required.
				 -This could be wrong, confirmation needed.

				see: http://www.opengl.org/wiki/GLAPI/glTexImage2D

				Since so, the pixel format provided here will be 
				only based on "GLenum format, GLenum type",
				as these params determines the interface for accessing the pixel data.
			*/

			GL1Texture::GL1Texture(GL1RenderDevice* device, GLuint texId, TextureType type)
				: Texture(device, 1,1,1,1, FMT_Unknown, TU_Static),  // initialize base class using dummy params
				m_renderDevice(device)
			{
				GLenum target = GLUtils::GetTextureTarget(type);
				glBindTexture(target, texId);

				
				
				// retrieve and set the real param here
				switch (type)
				{
				case TT_Texture1D:
					{
						int width;
						glGetTexLevelParameteriv(target, 0, GL_TEXTURE_WIDTH, &width);
						
						

					}
					break;
				case TT_Texture2D:
					{
						
					}
					break;
				case TT_Texture3D:
					{
						
					}
					break;
				case TT_CubeTexture:
					{
						
					}
					break;
				}
				glBindTexture(target, 0);
			}
			

			GL1Texture::GL1Texture(GL1RenderDevice* device, ResourceLocation* rl, TextureUsage usage, bool managed)
				: Texture(device, rl, usage, managed), 
				m_renderDevice(device)
			{
				if (!managed)
				{
					load();
				}
			}
			GL1Texture::GL1Texture(GL1RenderDevice* device, int32 width, int32 height, int32 depth, int32 level, 
				PixelFormat format, TextureUsage usage)
				: Texture(device, width, height, depth, level, format, usage),
				m_renderDevice(device)
			{
				

				if (getType() == TT_Texture2D || getType() == TT_Texture1D)
				{

					//HRESULT hr = dev->CreateTexture(width, height, level, 
					//	D3D9Utils::ConvertTextureUsage(usage), D3D9Utils::ConvertPixelFormat(format), 
					//	(usage & TU_Dynamic) ? D3DPOOL_DEFAULT : D3DPOOL_MANAGED, &m_tex2D, NULL);;
					//assert(SUCCEEDED(hr));
				}
				else if (getType() == TT_Texture3D)
				{
					//HRESULT hr = dev->CreateVolumeTexture(width, height,depth, level, 
					//	D3D9Utils::ConvertTextureUsage(usage), D3D9Utils::ConvertPixelFormat(format), 
					//	(usage & TU_Dynamic) ? D3DPOOL_DEFAULT : D3DPOOL_MANAGED, &m_tex3D, NULL);;
					//assert(SUCCEEDED(hr));
				}
			}

			GL1Texture::GL1Texture(GL1RenderDevice* device, int32 length, int32 level, PixelFormat format, TextureUsage usage)
				: Texture(device, length, level, usage, format),
				m_renderDevice(device)
			{
				glGenTextures(1, &m_textureID);
				glBindTexture(GL_TEXTURE_CUBE_MAP, m_textureID);

				//D3DDevice* dev = device->getDevice();

				//HRESULT hr = dev->CreateCubeTexture(length, level, 
				//	D3D9Utils::ConvertTextureUsage(usage), D3D9Utils::ConvertPixelFormat(format), 
				//	(usage & TU_Dynamic) ? D3DPOOL_DEFAULT : D3DPOOL_MANAGED, &m_cube, NULL);
				//assert(SUCCEEDED(hr));
			}
			GL1Texture::~GL1Texture()
			{


			}

			DataRectangle GL1Texture::lock(int32 surface, LockMode mode, const Apoc3D::Math::Rectangle& rect)
			{
				assert(m_tex2D);
				D3DLOCKED_RECT rrect;
				RECT rect0;
				rect0.left = rect.X;
				rect0.top = rect.Y;
				rect0.right = rect.getRight();
				rect0.bottom = rect.getBottom();

				HRESULT hr = m_tex2D->LockRect(surface, &rrect, &rect0, D3D9Utils::ConvertLockMode(mode));
				assert(SUCCEEDED(hr));
				DataRectangle result(rrect.Pitch, rrect.pBits, rect.Width, rect.Height, getFormat());
				return result;
			}
			DataBox GL1Texture::lock(int32 surface, LockMode mode, const Box& box)
			{
				assert(m_tex3D);
				D3DLOCKED_BOX rbox;
				D3DBOX box0;

				HRESULT hr = m_tex3D->LockBox(surface, &rbox, &box0, D3D9Utils::ConvertLockMode(mode));
				assert(SUCCEEDED(hr));
				
				DataBox result(box.getWidth(), box.getHeight(), box.getDepth(), rbox.RowPitch, rbox.SlicePitch, rbox.pBits, getFormat());
				return result;
			}
			DataRectangle GL1Texture::lock(int32 surface, CubeMapFace cubemapFace, LockMode mode, const Apoc3D::Math::Rectangle& rect)
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
			void GL1Texture::unlock(int32 surface)
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
			void GL1Texture::unlock(CubeMapFace cubemapFace, int32 surface)
			{
				m_cube->UnlockRect(D3D9Utils::ConvertCubeMapFace(cubemapFace), surface);
			}

			void copyData(void* tex, 
				int pitch, void* texData, D3DFORMAT surfaceFormat,
				DWORD dwLockWidth, DWORD dwLockHeight, bool isSetting)
			{
				byte* texPtr = reinterpret_cast<byte*>(tex);
				byte* texDataPtr = reinterpret_cast<byte*>(texData);

				byte bytesPerPixel = GetExpectedByteSizeFromFormat(surfaceFormat);


				bool isDxt = false;
				if (surfaceFormat == D3DFMT_DXT1 || 
					surfaceFormat == D3DFMT_DXT2 || 
					surfaceFormat == D3DFMT_DXT3 || 
					surfaceFormat == D3DFMT_DXT4 ||
					surfaceFormat == D3DFMT_DXT5)
				{
					isDxt = true;
					
					dwLockWidth = (dwLockWidth + 3) >> 2;
					dwLockHeight = (dwLockHeight + 3) >> 2;

					bytesPerPixel = surfaceFormat == D3DFMT_DXT1 ? 8 : 16;
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
				D3DFORMAT surfaceFormat,
				DWORD dwLockWidth, DWORD dwLockHeight, DWORD dwLockDepth,
				bool isSetting)
			{
				byte* texPtr = reinterpret_cast<byte*>(tex);
				byte* texDataPtr = reinterpret_cast<byte*>(texData);

				byte bytesPerPixel = GetExpectedByteSizeFromFormat(surfaceFormat);

				bool isDxt = false;
				if (surfaceFormat == D3DFMT_DXT1 || 
					surfaceFormat == D3DFMT_DXT2 || 
					surfaceFormat == D3DFMT_DXT3 || 
					surfaceFormat == D3DFMT_DXT4 ||
					surfaceFormat == D3DFMT_DXT5)
				{
					isDxt = true;

					dwLockWidth = (dwLockWidth + 3) >> 2;
					dwLockHeight = (dwLockHeight + 3) >> 2;

					bytesPerPixel = surfaceFormat == D3DFMT_DXT1 ? 8 : 16;
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
					copyData(rect.pBits, rect.Pitch, lvlData.ContentData, desc.Format, desc.Width, desc.Height, false);

					hr = tex->UnlockRect(i);
					assert(SUCCEEDED(hr));

					data.Levels.push_back(lvlData);
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
						desc.Format, desc.Width, desc.Height, false);
					startPos += faceSize;

					hr = tex->UnlockRect(D3DCUBEMAP_FACE_POSITIVE_X, i);
					assert(SUCCEEDED(hr));

					// ======================================================================

					hr = tex->LockRect(D3DCUBEMAP_FACE_NEGATIVE_X, i, &rect, NULL, D3DLOCK_READONLY);
					assert(SUCCEEDED(hr));

					copyData(rect.pBits, rect.Pitch, data.Levels[i].ContentData+startPos,
						desc.Format, desc.Width, desc.Height, false);
					startPos += faceSize;

					hr = tex->UnlockRect(D3DCUBEMAP_FACE_NEGATIVE_X, i);
					assert(SUCCEEDED(hr));

					// ======================================================================

					hr = tex->LockRect(D3DCUBEMAP_FACE_POSITIVE_Y, i, &rect, NULL, D3DLOCK_READONLY);
					assert(SUCCEEDED(hr));

					copyData(rect.pBits, rect.Pitch, data.Levels[i].ContentData+startPos,
						desc.Format, desc.Width, desc.Height, false);
					startPos += faceSize;

					hr = tex->UnlockRect(D3DCUBEMAP_FACE_POSITIVE_Y, i);
					assert(SUCCEEDED(hr));

					// ======================================================================
					hr = tex->LockRect(D3DCUBEMAP_FACE_NEGATIVE_Y, i, &rect, NULL, D3DLOCK_READONLY);
					assert(SUCCEEDED(hr));

					copyData(rect.pBits, rect.Pitch, data.Levels[i].ContentData+startPos,
						desc.Format, desc.Width, desc.Height, false);
					startPos += faceSize;

					hr = tex->UnlockRect(D3DCUBEMAP_FACE_NEGATIVE_Y, i);
					assert(SUCCEEDED(hr));

					// ======================================================================

					hr = tex->LockRect(D3DCUBEMAP_FACE_POSITIVE_Z, i, &rect, NULL, D3DLOCK_READONLY);
					assert(SUCCEEDED(hr));

					copyData(rect.pBits, rect.Pitch, data.Levels[i].ContentData+startPos,
						desc.Format, desc.Width, desc.Height, false);
					startPos += faceSize;

					hr = tex->UnlockRect(D3DCUBEMAP_FACE_POSITIVE_Z, i);
					assert(SUCCEEDED(hr));
					// ======================================================================
					hr = tex->LockRect(D3DCUBEMAP_FACE_NEGATIVE_Z, i, &rect, NULL, D3DLOCK_READONLY);
					assert(SUCCEEDED(hr));

					copyData(rect.pBits, rect.Pitch, data.Levels[i].ContentData+startPos,
						desc.Format, desc.Width, desc.Height, false);
					startPos += faceSize;

					hr = tex->UnlockRect(D3DCUBEMAP_FACE_NEGATIVE_Z, i);
					assert(SUCCEEDED(hr));

					// ======================================================================
					data.Levels.push_back(lvlData);
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
						data.Levels[i].ContentData, desc.Format,
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
						desc.Format, desc.Width, desc.Height, true);
					startPos += faceSize;

					hr = tex->UnlockRect(D3DCUBEMAP_FACE_POSITIVE_X, i);
					assert(SUCCEEDED(hr));

					// ======================================================================

					hr = tex->LockRect(D3DCUBEMAP_FACE_NEGATIVE_X, i, &rect, NULL, 0);
					assert(SUCCEEDED(hr));

					copyData(rect.pBits, rect.Pitch, data.Levels[i].ContentData+startPos,
						desc.Format, desc.Width, desc.Height, true);
					startPos += faceSize;

					hr = tex->UnlockRect(D3DCUBEMAP_FACE_NEGATIVE_X, i);
					assert(SUCCEEDED(hr));

					// ======================================================================

					hr = tex->LockRect(D3DCUBEMAP_FACE_POSITIVE_Y, i, &rect, NULL, 0);
					assert(SUCCEEDED(hr));

					copyData(rect.pBits, rect.Pitch, data.Levels[i].ContentData+startPos,
						desc.Format, desc.Width, desc.Height, true);
					startPos += faceSize;

					hr = tex->UnlockRect(D3DCUBEMAP_FACE_POSITIVE_Y, i);
					assert(SUCCEEDED(hr));

					// ======================================================================
					hr = tex->LockRect(D3DCUBEMAP_FACE_NEGATIVE_Y, i, &rect, NULL, 0);
					assert(SUCCEEDED(hr));

					copyData(rect.pBits, rect.Pitch, data.Levels[i].ContentData+startPos,
						desc.Format, desc.Width, desc.Height, true);
					startPos += faceSize;

					hr = tex->UnlockRect(D3DCUBEMAP_FACE_NEGATIVE_Y, i);
					assert(SUCCEEDED(hr));

					// ======================================================================

					hr = tex->LockRect(D3DCUBEMAP_FACE_POSITIVE_Z, i, &rect, NULL, 0);
					assert(SUCCEEDED(hr));

					copyData(rect.pBits, rect.Pitch, data.Levels[i].ContentData+startPos,
						desc.Format, desc.Width, desc.Height, true);
					startPos += faceSize;

					hr = tex->UnlockRect(D3DCUBEMAP_FACE_POSITIVE_Z, i);
					assert(SUCCEEDED(hr));
					// ======================================================================
					hr = tex->LockRect(D3DCUBEMAP_FACE_NEGATIVE_Z, i, &rect, NULL, 0);
					assert(SUCCEEDED(hr));

					copyData(rect.pBits, rect.Pitch, data.Levels[i].ContentData+startPos,
						desc.Format, desc.Width, desc.Height, true);
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

					copyData(rect.pBits, rect.Pitch, data.Levels[i].ContentData, desc.Format, desc.Width, desc.Height, true);

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
						data.Levels[i].ContentData, desc.Format,
						desc.Width, desc.Height, desc.Depth, true);

					hr = tex->UnlockBox(i);
					assert(SUCCEEDED(hr));
				}
			}

			void GL1Texture::load()
			{
				TextureData data;
				data.Load(getResourceLocation());
				
				
				D3DDevice* dev = m_renderDevice->getDevice();


				D3DFORMAT newFmt;
				
				{
					DWORD usage = D3D9Utils::ConvertTextureUsage(getUsage());
					D3DFORMAT fmt = D3D9Utils::ConvertPixelFormat(data.Format);
					UINT width = data.Levels[0].Width;
					UINT height = data.Levels[0].Height;
					UINT depth = data.Levels[0].Depth;
					UINT levels = data.LevelCount;



					newFmt = fmt;
					UINT newWidth = width;
					UINT newHeight = height;
					UINT newDepth = depth;


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
								0, &fmt, 
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
						// TODO:resize here
						LogManager::getSingleton().Write(LOG_Graphics, 
							L"[D3D9Texture]" + name +  L" Dimension " +
							StringUtils::ToString(width) + L"x" + StringUtils::ToString(height)
							+ L" is not supported by hardware. Resizing to " +
							StringUtils::ToString(newWidth) + L"x" + StringUtils::ToString(newHeight)
							, LOGLVL_Warning);
					}

					if (newFmt != fmt)
					{
						TextureData newdata;
						newdata.Format = D3D9Utils::ConvertBackPixelFormat(newFmt);
						newdata.ContentSize = 0;
						newdata.LevelCount = data.LevelCount;
						newdata.Type = data.Type;
						newdata.Levels.reserve(data.LevelCount);

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

							newdata.Levels.push_back(dstLvl);
						}

						data = newdata;
					}

				}
				
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
			void GL1Texture::unload()
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


			void GL1Texture::Save(Stream* strm)
			{
				Lock_Unloadable();
				UseSync();
				
				TextureData data;
				data.LevelCount = getLevelCount();
				data.Type = getType();
				data.Format = getFormat();
				data.ContentSize = 0;
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

				Unlock_Unloadable();

				data.Save(strm);

				for (int i=0;i<data.LevelCount;i++)
				{
					delete[] data.Levels[i].ContentData;
				}
			}

		}
	}
}