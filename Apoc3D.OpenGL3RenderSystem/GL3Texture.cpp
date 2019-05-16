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

#include "GL3Texture.h"
#include "GL3RenderDevice.h"
#include "GL3Utils.h"
#include "apoc3d/Math/Rectangle.h"
#include "apoc3d/Math/Box.h"
#include "apoc3d/Vfs/ResourceLocation.h"
#include "apoc3d/IOLib/TextureData.h"
#include "apoc3d/Utility/StringUtils.h"
#include "apoc3d/Core/Logging.h"
#include "apoc3d/IOLib/Streams.h"
#include "apoc3d/Vfs/PathUtils.h"


using namespace Apoc3D::VFS;
using namespace Apoc3D::IO;
using namespace Apoc3D::Core;
using namespace Apoc3D::Utility;

namespace Apoc3D
{
	namespace Graphics
	{
		namespace GL3RenderSystem
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

			//GL1Texture::GL1Texture(GL1RenderDevice* device, GLuint texId, TextureType type)
			//	: Texture(device, 1,1,1,1, FMT_Unknown, TU_Static),  // initialize base class using dummy params
			//	m_renderDevice(device)
			//{
			//	GLenum target = GLUtils::GetTextureTarget(type);
			//	glBindTexture(target, texId);

			//	
			//	
			//	// retrieve and set the real param here
			//	switch (type)
			//	{
			//	case TT_Texture1D:
			//		{
			//			int width;
			//			glGetTexLevelParameteriv(target, 0, GL_TEXTURE_WIDTH, &width);
			//			
			//			

			//		}
			//		break;
			//	case TT_Texture2D:
			//		{
			//			
			//		}
			//		break;
			//	case TT_Texture3D:
			//		{
			//			
			//		}
			//		break;
			//	case TT_CubeTexture:
			//		{
			//			
			//		}
			//		break;
			//	}
			//	glBindTexture(target, 0);
			//}
			

			GL3Texture::GL3Texture(GL3RenderDevice* device, ResourceLocation* rl, TextureUsage usage, bool managed)
				: Texture(device, rl, usage, managed), 
				m_renderDevice(device)
			{
				if (!managed)
				{
					load();
				}
			}
			GL3Texture::GL3Texture(GL3RenderDevice* device, int32 width, int32 height, int32 depth, int32 level, 
				PixelFormat format, TextureUsage usage)
				: Texture(device, width, height, depth, level, format, usage),
				m_renderDevice(device)
			{
				InitializeGLTexture(width, height, depth, level, format);
			}

			GL3Texture::GL3Texture(GL3RenderDevice* device, int32 length, int32 level, PixelFormat format, TextureUsage usage)
				: Texture(device, length, level, usage, format),
				m_renderDevice(device)
			{
				InitializeGLTexture(length, length, 1, level, format);
			}
			GL3Texture::~GL3Texture()
			{
				if (m_textureID)
				{
					glDeleteTextures(1, &m_textureID);
					m_textureID = 0;
				}
			}

			DataRectangle GL3Texture::lock(int32 surface, LockMode mode, const Apoc3D::Math::Rectangle& rect)
			{
				assert(m_tex2D);

				glBindBuffer(GL_PIXEL_UNPACK_BUFFER, m_mapPBO);

				//void* dataPtr = glMapBuffer(GL_PIXEL_UNPACK_BUFFER, GL_WRITE_ONLY);
				void* dataPtr = glMapBufferRange(GL_PIXEL_UNPACK_BUFFER, , , GL_WRITE_ONLY);

				
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
			DataBox GL3Texture::lock(int32 surface, LockMode mode, const Box& box)
			{
				assert(m_tex3D);
				D3DLOCKED_BOX rbox;
				D3DBOX box0;

				HRESULT hr = m_tex3D->LockBox(surface, &rbox, &box0, D3D9Utils::ConvertLockMode(mode));
				assert(SUCCEEDED(hr));
				
				DataBox result(box.getWidth(), box.getHeight(), box.getDepth(), rbox.RowPitch, rbox.SlicePitch, rbox.pBits, getFormat());
				return result;
			}
			DataRectangle GL3Texture::lock(int32 surface, CubeMapFace cubemapFace, LockMode mode, const Apoc3D::Math::Rectangle& rect)
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
			void GL3Texture::unlock(int32 surface)
			{
				TextureType type = getType();

				glBindTexture(target, m_textureID);
				glBindBuffer(GL_PIXEL_UNPACK_BUFFER, m_mapPBO);
				glUnmapBuffer(GL_PIXEL_UNPACK_BUFFER);
				if (type == TextureType::Texture3D)
				{
					//m_tex3D->UnlockBox(surface);
				}
				else if (type != TextureType::CubeTexture)
				{
					//m_tex2D->UnlockRect(surface);
				}
				else
				{
					//m_cube->UnlockRect(m_lockedCubeFace, surface);
				}
				glTexSubImage2D(target, surface, , , , , , , nullptr);

				glBindBuffer(GL_PIXEL_UNPACK_BUFFER, 0);

			}
			void GL3Texture::unlock(CubeMapFace cubemapFace, int32 surface)
			{
				glBindTexture(target, m_textureID);
				glBindBuffer(GL_PIXEL_UNPACK_BUFFER, m_mapPBO);
				glUnmapBuffer(GL_PIXEL_UNPACK_BUFFER);

				glTexSubImage2D(target, surface, , , , , , , nullptr);
				//m_cube->UnlockRect(D3D9Utils::ConvertCubeMapFace(cubemapFace), surface);
				
				glBindBuffer(GL_PIXEL_UNPACK_BUFFER, 0);
			}


			void GL3Texture::load()
			{
				TextureData data;
				data.Load(getResourceLocation());
				

				// TODO: possible pre-conversions including resizing
				//D3DFORMAT newFmt;
				//
				//{
				//	DWORD usage = D3D9Utils::ConvertTextureUsage(getUsage());
				//	D3DFORMAT fmt = D3D9Utils::ConvertPixelFormat(data.Format);
				//	UINT width = data.Levels[0].Width;
				//	UINT height = data.Levels[0].Height;
				//	UINT depth = data.Levels[0].Depth;
				//	UINT levels = data.LevelCount;



				//	newFmt = fmt;
				//	UINT newWidth = width;
				//	UINT newHeight = height;
				//	UINT newDepth = depth;


				//	switch (data.Type)
				//	{
				//	case (int)TT_Texture1D:
				//	case (int)TT_Texture2D:
				//		{
				//			HRESULT hr = D3DXCheckTextureRequirements(dev, &newWidth, &newHeight, &levels,
				//				0, &newFmt, 
				//				D3DPOOL_MANAGED);
				//			assert(SUCCEEDED(hr));
				//			break;
				//		}
				//	case (int)TT_CubeTexture:
				//		{
				//			HRESULT hr = D3DXCheckCubeTextureRequirements(dev, &newWidth, &levels,
				//				0, &fmt, 
				//				D3DPOOL_MANAGED);
				//			assert(SUCCEEDED(hr));
				//			break;
				//		}
				//	case (int)TT_Texture3D:
				//		{
				//			HRESULT hr = D3DXCheckVolumeTextureRequirements(dev,
				//				&newWidth, &newHeight, &newDepth,
				//				&levels, 0, &newFmt, D3DPOOL_MANAGED);
				//			assert(SUCCEEDED(hr));
				//			break;
				//		}
				//	}

				//	if (newHeight != height || newWidth != width || newDepth != depth)
				//	{
				//		String name;
				//		const FileLocation* fl = dynamic_cast<const FileLocation*>(getResourceLocation());

				//		if (fl)
				//		{
				//			name = PathUtils::GetFileNameNoExt( fl->getPath());
				//		}
				//		else
				//		{
				//			name = getResourceLocation()->getName();
				//		}
				//		// TODO:resize here
				//		LogManager::getSingleton().Write(LOG_Graphics, 
				//			L"[D3D9Texture]" + name +  L" Dimension " +
				//			StringUtils::ToString(width) + L"x" + StringUtils::ToString(height)
				//			+ L" is not supported by hardware. Resizing to " +
				//			StringUtils::ToString(newWidth) + L"x" + StringUtils::ToString(newHeight)
				//			, LOGLVL_Warning);
				//	}

				//	if (newFmt != fmt)
				//	{
				//		TextureData newdata;
				//		newdata.Format = D3D9Utils::ConvertBackPixelFormat(newFmt);
				//		newdata.ContentSize = 0;
				//		newdata.LevelCount = data.LevelCount;
				//		newdata.Type = data.Type;
				//		newdata.Levels.reserve(data.LevelCount);

				//		String name;
				//		const FileLocation* fl = dynamic_cast<const FileLocation*>(getResourceLocation());

				//		if (fl)
				//		{
				//			name = PathUtils::GetFileNameNoExt( fl->getPath());
				//		}
				//		else
				//		{
				//			name = getResourceLocation()->getName();
				//		}
				//		LogManager::getSingleton().Write(LOG_Graphics, 
				//			L"[D3D9Texture]" + name + L" " + PixelFormatUtils::ToString(data.Format) 
				//			+ L" Pixel format is not supported by hardware. Converting to " +
				//			PixelFormatUtils::ToString(D3D9Utils::ConvertBackPixelFormat(newFmt)), LOGLVL_Warning);

				//		for (int i=0;i<newdata.LevelCount;i++)
				//		{
				//			TextureLevelData& srcLvl = data.Levels[i];
				//			
				//			TextureLevelData dstLvl;
				//			dstLvl.Depth = srcLvl.Depth;
				//			dstLvl.Width = srcLvl.Width;
				//			dstLvl.Height = srcLvl.Height;

				//			int lvlSize = PixelFormatUtils::GetMemorySize(
				//				dstLvl.Width, dstLvl.Height, dstLvl.Depth, newdata.Format);
				//			dstLvl.LevelSize = lvlSize;

				//			dstLvl.ContentData = new char[lvlSize];
				//			newdata.ContentSize += lvlSize;

				//			DataBox src = DataBox(
				//				srcLvl.Width, 
				//				srcLvl.Height, 
				//				srcLvl.Depth, 
				//				PixelFormatUtils::GetMemorySize(srcLvl.Width, 1, 1, data.Format),
				//				PixelFormatUtils::GetMemorySize(srcLvl.Width, srcLvl.Height, 1, data.Format), 
				//				srcLvl.ContentData,
				//				data.Format);

				//			DataBox dst = DataBox(
				//				dstLvl.Width,
				//				dstLvl.Height, 
				//				dstLvl.Depth, 
				//				PixelFormatUtils::GetMemorySize(dstLvl.Width, 1, 1, newdata.Format),
				//				PixelFormatUtils::GetMemorySize(dstLvl.Width, dstLvl.Height, 1, newdata.Format), 
				//				dstLvl.ContentData,
				//				newdata.Format);

				//			int r = PixelFormatUtils::ConvertPixels(src, dst);
				//			assert(r);
				//			delete[] srcLvl.ContentData;

				//			newdata.Levels.push_back(dstLvl);
				//		}

				//		data = newdata;
				//	}

				//}
				//
				UpdateInfo(data);

				GLenum format, type;


				switch (data.Type)
				{
				case TextureType::Texture1D:
					{
						//glBindTexture()
						//glTexImage1D();
						break;
					}
				case TextureType::Texture2D:
					{
						
						DWORD usage = D3D9Utils::ConvertTextureUsage(getUsage());
					
						HRESULT hr = dev->CreateTexture(getWidth(), getHeight(), getLevelCount(), 
							usage, newFmt, 
							D3DPOOL_MANAGED, &m_tex2D, NULL);
						assert(SUCCEEDED(hr));

						setData(data, m_tex2D);
					}					
					break;
				case TextureType::CubeTexture:
					{
						DWORD usage = D3D9Utils::ConvertTextureUsage(getUsage());
						
						HRESULT hr = dev->CreateCubeTexture(getWidth(), getLevelCount(), 
							usage, newFmt, 
							D3DPOOL_MANAGED, &m_cube, NULL);
						assert(SUCCEEDED(hr));

						setData(data, m_cube);
					}
					
					break;
				case TextureType::Texture3D:
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

			void GL3Texture::unload()
			{
				if (m_textureID)
				{
					glDeleteTextures(1, &m_textureID);
					m_textureID = 0;
				}
			}


			void GL3Texture::Save(Stream* strm)
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
				case TextureType::Texture1D:
				case TextureType::Texture2D:
					getData(data, m_tex2D);
					break;
				case TextureType::CubeTexture:
					getData(data, m_cube);
					break;
				case TextureType::Texture3D:
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

			void GL3Texture::InitializeGLTexture(int32 width, int32 height, int32 depth, int32 level, PixelFormat format)
			{
				GLenum target = GLUtils::GetTextureTarget(getType());
				glGenTextures(1, &m_textureID);
				glBindTexture(target, m_textureID);

				glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAX_LEVEL, level - 1);
				glTexParameteri(target, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
				glTexParameteri(target, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
				glTexParameteri(target, GL_TEXTURE_WRAP_S, GL_REPEAT);
				glTexParameteri(target, GL_TEXTURE_WRAP_T, GL_REPEAT);

				GLenum glInternalFormat;
				GLenum glFormat, glType;
				GLUtils::ConvertPixelFormat(format, glFormat, glType, glInternalFormat);

				// TODO: For textures, upload data directly.
				// So PBO can use pre-allocated textures later.
				// https://community.khronos.org/t/pbo-performances-compressed-textures/54618

				if (PixelFormatUtils::IsCompressed(format))
				{
					int rootLevelSize = PixelFormatUtils::GetMemorySize(width, height, depth, format);
					char* tmpdata = new char[rootLevelSize]();
					
					// fill all the levels with zero buffer
					// glCompressedTexImage do not allow 0 pointer as data

					// All levels are generated in this loop
					for(int i=0; i<level; i++)
					{
						int levelSize = PixelFormatUtils::GetMemorySize(width, height, depth, format);
						switch(getType())
						{
						case TextureType::Texture1D:
							glCompressedTexImage1D(GL_TEXTURE_1D, i, glInternalFormat,
								width,
								0, levelSize, tmpdata);
							break;
						case TextureType::Texture2D:
							glCompressedTexImage2D(GL_TEXTURE_2D, i, glInternalFormat,
								width, height, 
								0, levelSize, tmpdata);
							break;
						case TextureType::Texture3D:
							glCompressedTexImage3D(GL_TEXTURE_3D, i, glInternalFormat,
								width, height, depth, 
								0, levelSize, tmpdata);
							break;
						case TextureType::CubeTexture:
							// do every cube map face
							for(int j=0; j<6; j++)
								glCompressedTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + j, i, glInternalFormat,
								width, height, 
								0, levelSize, tmpdata);

							break;
						}

						// the w/h/d will remain 1 if they are small enough
						if(width>1)	 width = width/2;
						if(height>1) height = height/2;
						if(depth>1)	 depth = depth/2;
					}
					delete[] tmpdata;
				}
				else
				{
					// All levels are generated in this loop
					for(int i=0; i<level; i++)
					{
						switch(getType())
						{
						case TextureType::Texture1D:
							glTexImage1D(GL_TEXTURE_1D, i, glInternalFormat,
								width, 
								0, glFormat, glType, 0);
							break;

						case TextureType::Texture2D:
							glTexImage2D(GL_TEXTURE_2D, i, format,
								width, height,
								0, glFormat, glType, 0);
							break;

						case TextureType::Texture3D:
							glTexImage3D(GL_TEXTURE_3D, i, format,
								width, height, depth, 
								0, glFormat, glType, 0);
							break;

						case TextureType::CubeTexture:
							// do every cube map face
							for(int j=0; j<6; j++)
								glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + j, i, format,
								width, height, 
								0, glFormat, glType, 0);

							break;
						}
					}
				}

				int level0Size = PixelFormatUtils::GetMemorySize(width, height, depth, format);
				glGenBuffers(1, &m_mapPBO);
				glBindBuffer(GL_PIXEL_UNPACK_BUFFER, m_mapPBO);
				glBufferData(GL_PIXEL_UNPACK_BUFFER, level0Size, NULL, usage == TU_Dynamic ? GL_DYNAMIC_DRAW : GL_STREAM_DRAW);

			}
		}
	}
}