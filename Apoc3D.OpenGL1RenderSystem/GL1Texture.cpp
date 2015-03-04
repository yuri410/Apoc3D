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

#include "GL1Texture.h"
#include "GL1RenderDevice.h"
#include "GL1Utils.h"
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
				InitializeEmptyGLTexture(width, height, depth, level, format);
			}

			GL1Texture::GL1Texture(GL1RenderDevice* device, int32 length, int32 level, PixelFormat format, TextureUsage usage)
				: Texture(device, length, level, usage, format),
				m_renderDevice(device)
			{
				InitializeEmptyGLTexture(length, length, 1, level, format);
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


			void GL1Texture::load()
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
				case (int)TT_Texture1D:
					{
						//glBindTexture()
						//glTexImage1D();
						break;
					}
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
			void GL1Texture::InitializeEmptyGLTexture(int32 width, int32 height, int32 depth, int32 level, PixelFormat format)
			{

				GLenum target = GLUtils::GetTextureTarget(getType());
				glGenTextures(1, &m_textureID);
				glBindTexture(target, m_textureID);
				if (GLEE_VERSION_1_2)
					glTexParameteri(target, GL_TEXTURE_MAX_LEVEL, level);

				glTexParameteri(target, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
				glTexParameteri(target, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

				if (GLEE_VERSION_1_2)
				{
					glTexParameteri(target, GL_TEXTURE_WRAP_S, GL_REPEAT);
					glTexParameteri(target, GL_TEXTURE_WRAP_T, GL_REPEAT);
				}

				GLenum glFmt = GLUtils::ConvertPixelFormat(format);

				if (PixelFormatUtils::IsCompressed(format))
				{
					int rootLevelSize = PixelFormatUtils::GetMemorySize(width, height, depth, format);
					char* tmpdata = new char[rootLevelSize];
					memset(tmpdata, 0, rootLevelSize);

					// fill all the levels with zero buffer
					// glCompressedTexImage do not allow 0 pointer as data

					// All levels are generated in this loop
					for(int i=0; i<level; i++)
					{
						int levelSize = PixelFormatUtils::GetMemorySize(width, height, depth, format);
						switch(getType())
						{
						case TT_Texture1D:
							glCompressedTexImage1DARB(GL_TEXTURE_1D, i, glFmt, 
								width,
								0, levelSize, tmpdata);
							break;
						case TT_Texture2D:
							glCompressedTexImage2DARB(GL_TEXTURE_2D, i, glFmt,
								width, height, 
								0, levelSize, tmpdata);
							break;
						case TT_Texture3D:
							glCompressedTexImage3DARB(GL_TEXTURE_3D, i, glFmt,
								width, height, depth, 
								0, levelSize, tmpdata);
							break;
						case TT_CubeTexture:
							// do every cube map face
							for(int j=0; j<6; j++)
								glCompressedTexImage2DARB(GL_TEXTURE_CUBE_MAP_POSITIVE_X + j, i, glFmt,
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
						// use GL_RGBA, GL_UNSIGNED_BYTE to fill format and type, as 
						// data is not provided
						switch(getType())
						{
						case TT_Texture1D:
							glTexImage1D(GL_TEXTURE_1D, i, format,
								width, 
								0, GL_RGBA, GL_UNSIGNED_BYTE, 0);

							break;
						case TT_Texture2D:
							glTexImage2D(GL_TEXTURE_2D, i, format,
								width, height,
								0, GL_RGBA, GL_UNSIGNED_BYTE, 0);
							break;
						case TT_Texture3D:
							glTexImage3D(GL_TEXTURE_3D, i, format,
								width, height, depth, 
								0, GL_RGBA, GL_UNSIGNED_BYTE, 0);
							break;
						case TT_CubeTexture:
							// do every cube map face
							for(int j=0; j<6; j++)
								glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + j, i, format,
								width, height, 
								0, GL_RGBA, GL_UNSIGNED_BYTE, 0);

							break;
						}
					}
				}

			}
		}
	}
}