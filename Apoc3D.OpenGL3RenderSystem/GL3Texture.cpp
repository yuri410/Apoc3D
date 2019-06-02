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
			GL3Texture::GL3Texture(GL3RenderDevice* device, const ResourceLocation& rl, TextureUsage usage, bool managed)
				: Texture(device, rl, usage, managed)
				, m_renderDevice(device)
			{
				if (!managed)
				{
					LoadTexture(&rl);
				}
			}
			GL3Texture::GL3Texture(GL3RenderDevice* device, int32 width, int32 height, int32 depth, int32 level, 
				PixelFormat format, TextureUsage usage)
				: Texture(device, width, height, depth, level, format, usage)
				, m_renderDevice(device)
			{
				InitializeGLTexture(nullptr);
			}

			GL3Texture::GL3Texture(GL3RenderDevice* device, int32 length, int32 level, PixelFormat format, TextureUsage usage)
				: Texture(device, length, level, usage, format)
				, m_renderDevice(device)
			{
				InitializeGLTexture(nullptr);
			}

			GL3Texture::~GL3Texture()
			{
				if (m_textureID)
				{
					glDeleteTextures(1, &m_textureID);
					m_textureID = 0;
				}
				if (m_levelOffsets)
				{
					delete[] m_levelOffsets;
					m_levelOffsets = nullptr;
				}
			}

			DataRectangle GL3Texture::lock(int32 level, LockMode mode, const Apoc3D::Math::Rectangle& rect)
			{
				assert(m_textureID && (getType() == TextureType::Texture2D || getType() == TextureType::Texture1D));

				glBindBuffer(GL_PIXEL_UNPACK_BUFFER, m_mapPBO);

				GLbitfield access = GLUtils::ConvertLockMode(mode);
				PixelFormat format = getFormat();

				m_lockedRect = rect;
				m_lockedLevel = level;
				m_lockedPboOffset = PixelFormatUtils::GetMemorySize(rect.X, rect.Y, 1, format) + m_levelOffsets[level];

				int areaSize = PixelFormatUtils::GetMemorySize(rect.Width, rect.Height, 1, format);
				int rowPitch = PixelFormatUtils::GetMemorySize(rect.Width, 1, 1, format);

				void* dataPtr = glMapBufferRange(GL_PIXEL_UNPACK_BUFFER, m_lockedPboOffset, areaSize, access);

				return DataRectangle(rowPitch, dataPtr, rect.Width, rect.Height, format);
			}

			DataBox GL3Texture::lock(int32 level, LockMode mode, const Box& box)
			{
				assert(m_textureID && getType() == TextureType::Texture3D);

				glBindBuffer(GL_PIXEL_UNPACK_BUFFER, m_mapPBO);

				GLbitfield access = GLUtils::ConvertLockMode(mode);
				PixelFormat format = getFormat();

				m_lockedBox = box;
				m_lockedLevel = level;
				m_lockedPboOffset = PixelFormatUtils::GetMemorySize(box.Left, box.Top, box.Front, format) + m_levelOffsets[level];

				int areaSize = PixelFormatUtils::GetMemorySize(box.getWidth(), box.getHeight(), box.getDepth(), format);
				int rowPitch = PixelFormatUtils::GetMemorySize(box.getWidth(), 1, 1, format);
				int slicePitch = PixelFormatUtils::GetMemorySize(box.getWidth(), box.getHeight(), 1, format);

				void* dataPtr = glMapBufferRange(GL_PIXEL_UNPACK_BUFFER, m_lockedPboOffset, areaSize, access);

				return DataBox(box.getWidth(), box.getHeight(), box.getDepth(), rowPitch, slicePitch, dataPtr, format);
			}

			DataRectangle GL3Texture::lock(int32 level, CubeMapFace cubemapFace, LockMode mode, const Apoc3D::Math::Rectangle& rect)
			{
				assert(m_textureID && getType() == TextureType::CubeTexture);

				glBindBuffer(GL_PIXEL_UNPACK_BUFFER, m_mapPBO);

				GLbitfield access = GLUtils::ConvertLockMode(mode);
				PixelFormat format = getFormat();

				m_lockedRect = rect;
				m_lockedCubemapFace = GLUtils::ConvertCubemapFace(cubemapFace);
				m_lockedLevel = level;
				m_lockedPboOffset = PixelFormatUtils::GetMemorySize(rect.X, rect.Y, 1, format) * cubemapFace + m_levelOffsets[level];

				int areaSize = PixelFormatUtils::GetMemorySize(rect.Width, rect.Height, 1, format);
				int rowPitch = PixelFormatUtils::GetMemorySize(rect.Width, 1, 1, format);

				void* dataPtr = glMapBufferRange(GL_PIXEL_UNPACK_BUFFER, 0, areaSize, access);

				return DataRectangle(rowPitch, dataPtr, rect.Width, rect.Height, format);
			}
			void GL3Texture::unlock(int32 level)
			{
				TextureType type = getType();

				glBindTexture(m_glTarget, m_textureID);
				glBindBuffer(GL_PIXEL_UNPACK_BUFFER, m_mapPBO);
				glUnmapBuffer(GL_PIXEL_UNPACK_BUFFER);
				
				glPixelStorei(GL_UNPACK_ALIGNMENT, 1);

				switch (type)
				{
					case TextureType::Texture1D:
						glTexSubImage1D(m_glTarget, level,
										m_lockedRect.X, m_lockedRect.Width, 
										m_glFormat, m_glType,
										(const GLvoid*)m_lockedPboOffset);
						break;

					case TextureType::Texture2D:
						glTexSubImage2D(m_glTarget, level,
										m_lockedRect.X, m_lockedRect.Y, m_lockedRect.Width, m_lockedRect.Height,
										m_glFormat, m_glType,
										(const GLvoid*)m_lockedPboOffset);
						break;

					case TextureType::Texture3D:
						glTexSubImage3D(m_glTarget, level,
										m_lockedBox.Left, m_lockedBox.Top, m_lockedBox.Front,
										m_lockedBox.getWidth(), m_lockedBox.getHeight(), m_lockedBox.getDepth(),
										m_glFormat, m_glType,
										(const GLvoid*)m_lockedPboOffset);
						break;

					case TextureType::CubeTexture:
						glTexSubImage2D(m_lockedCubemapFace, level,
										m_lockedRect.X, m_lockedRect.Y, m_lockedRect.Width, m_lockedRect.Height,
										m_glFormat, m_glType,
										(const GLvoid*)m_lockedPboOffset);
						break;
				}

				glBindBuffer(GL_PIXEL_UNPACK_BUFFER, 0);

			}
			void GL3Texture::unlock(CubeMapFace cubemapFace, int32 level)
			{
				glBindTexture(m_glTarget, m_textureID);
				glBindBuffer(GL_PIXEL_UNPACK_BUFFER, m_mapPBO);
				glUnmapBuffer(GL_PIXEL_UNPACK_BUFFER);

				glPixelStorei(GL_UNPACK_ALIGNMENT, 1);

				glTexSubImage2D(GLUtils::ConvertCubemapFace(cubemapFace), level,
								m_lockedRect.X, m_lockedRect.Y, m_lockedRect.Width, m_lockedRect.Height, 
								m_glFormat, m_glType, 
								(const GLvoid*)m_lockedPboOffset);
				
				glBindBuffer(GL_PIXEL_UNPACK_BUFFER, 0);
			}

			void GL3Texture::LoadTexture(const ResourceLocation* rl)
			{
				TextureData data;
				data.Load(*rl);

				UpdateInfo(data);

				InitializeGLTexture(&data);
			}

			void GL3Texture::load()
			{
				LoadTexture(getResourceLocation());
			}

			void GL3Texture::unload()
			{
				if (m_textureID)
				{
					glDeleteTextures(1, &m_textureID);
					m_textureID = 0;
				}
			}

			void GL3Texture::Save(Stream& strm)
			{
				Lock_Unloadable();
				UseSync();
				
				glBindTexture(m_glTarget, m_textureID);
				glPixelStorei(GL_UNPACK_ALIGNMENT, 1);

				TextureData data;
				data.LevelCount = getLevelCount();
				data.Levels.ReserveDiscard(data.LevelCount);
				data.Type = getType();
				data.Format = getFormat();
				data.ContentSize = 0;

				for (int i = 0; i < data.LevelCount; i++)
				{
					TextureLevelData& lvlData = data.Levels[i];
					lvlData.LevelSize = 0;

					GLint lvlWidth = 0;
					GLint lvlHeight = 0;
					GLint lvlDepth = 0;
					glGetTexLevelParameteriv(m_glTarget, i, GL_TEXTURE_WIDTH, &lvlWidth);
					glGetTexLevelParameteriv(m_glTarget, i, GL_TEXTURE_WIDTH, &lvlHeight);
					glGetTexLevelParameteriv(m_glTarget, i, GL_TEXTURE_WIDTH, &lvlDepth);

					switch (data.Type)
					{
						case TextureType::Texture1D:
							lvlHeight = 1;
						case TextureType::Texture2D:
							lvlDepth = 1;
						case TextureType::Texture3D:
							lvlData.Width = lvlWidth;
							lvlData.Height = lvlHeight;
							lvlData.Depth = lvlDepth;
							lvlData.LevelSize = PixelFormatUtils::GetMemorySize(lvlData.Width, lvlData.Height, lvlData.Depth, data.Format);
							lvlData.ContentData = new char[lvlData.LevelSize];
							glGetTexImage(m_glTarget, i, m_glFormat, m_glType, lvlData.ContentData);
							break;

						case TextureType::CubeTexture:
							int faceSize = PixelFormatUtils::GetMemorySize(lvlWidth, lvlHeight, 1, data.Format);

							lvlData.Width = lvlWidth;
							lvlData.Height = lvlHeight;
							lvlData.Depth = 1;
							lvlData.LevelSize = faceSize * 6;
							lvlData.ContentData = new char[lvlData.LevelSize];
							for (int j = 0; j < 6; j++)
							{
								glGetTexImage(GL_TEXTURE_CUBE_MAP_POSITIVE_X + j, i, m_glFormat, m_glType, lvlData.ContentData + faceSize*j);
							}

							break;
					}

					data.ContentSize += lvlData.LevelSize;
				}

				Unlock_Unloadable();

				data.Save(strm);
			}

			void GL3Texture::InitializeGLTexture(const TextureData* data)
			{
				const int32 levelCount = getLevelCount();
				const PixelFormat format = getFormat();

				assert(m_textureID);

				m_glTarget = GLUtils::GetTextureTarget(getType());
				glGenTextures(1, &m_textureID);
				glBindTexture(m_glTarget, m_textureID);
				glBindBuffer(GL_PIXEL_UNPACK_BUFFER, 0);

				glTexParameteri(m_glTarget, GL_TEXTURE_MAX_LEVEL, levelCount - 1);
				glTexParameteri(m_glTarget, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
				glTexParameteri(m_glTarget, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
				glTexParameteri(m_glTarget, GL_TEXTURE_WRAP_S, GL_REPEAT);
				glTexParameteri(m_glTarget, GL_TEXTURE_WRAP_T, GL_REPEAT);

				GLenum glInternalFormat;
				GLUtils::ConvertPixelFormat(format, m_glFormat, m_glType, glInternalFormat);

				// For textures, upload data directly.
				// So PBO can use pre-allocated textures later.
				// https://community.khronos.org/t/pbo-performances-compressed-textures/54618

				m_levelOffsets = new GLint[levelCount]();

				if (PixelFormatUtils::IsCompressed(format))
				{
					char* levelData = nullptr;
					if (data == nullptr)
					{
						levelData = new char[PixelFormatUtils::GetMemorySize(getWidth(), getHeight(), getDepth(), format)]();
					}

					int width = getWidth();
					int height = getHeight();
					int depth = getDepth();

					// fill all the levels with zero buffer
					// glCompressedTexImage do not allow 0 pointer as data

					// All levels are generated in this loop
					for (int i = 0; i < levelCount; i++)
					{
						int levelSize;
						int faceSize = 0;
						const char* levelData = nullptr;

						if (data)
						{
							TextureLevelData& lvl = data->Levels[i];

							width = lvl.Width;
							height = lvl.Height;
							depth = lvl.Depth;
							levelData = lvl.ContentData;
							levelSize = lvl.LevelSize;
							faceSize = levelSize / 6;
						}
						else
						{
							levelSize = PixelFormatUtils::GetMemorySize(width, height, depth, format);
						}

						switch (getType())
						{
							case TextureType::Texture1D:
								glCompressedTexImage1D(GL_TEXTURE_1D, i, glInternalFormat,
													   width,
													   0, levelSize, levelData);
								break;
							case TextureType::Texture2D:
								glCompressedTexImage2D(GL_TEXTURE_2D, i, glInternalFormat,
													   width, height,
													   0, levelSize, levelData);
								break;
							case TextureType::Texture3D:
								glCompressedTexImage3D(GL_TEXTURE_3D, i, glInternalFormat,
													   width, height, depth,
													   0, levelSize, levelData);
								break;
							case TextureType::CubeTexture:
								// do every cube map face
								for (int j = 0; j < 6; j++)
								{
									glCompressedTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + j, i, glInternalFormat,
														   width, height,
														   0, levelSize, levelData + faceSize * j);
								}
								break;
						}

						// the w/h/d will remain 1 if they are small enough
						if (width > 1)	 width = width / 2;
						if (height > 1)  height = height / 2;
						if (depth > 1)	 depth = depth / 2;

						for (int j = i+1; j < levelCount; j++)
							m_levelOffsets[j] += levelSize;
					}

					if (levelData)
						delete[] levelData;
				}
				else
				{
					int width = getWidth();
					int height = getHeight();
					int depth = getDepth();

					// All levels are generated in this loop
					for (int i = 0; i < levelCount; i++)
					{
						const char* levelData = nullptr;
						int levelSize;
						int faceSize = 0;

						if (data)
						{
							TextureLevelData& lvl = data->Levels[i];

							width = lvl.Width;
							height = lvl.Height;
							depth = lvl.Depth;
							levelData = lvl.ContentData;
							levelSize = lvl.LevelSize;
							faceSize = lvl.LevelSize / 6;
						}
						else
						{
							levelSize = PixelFormatUtils::GetMemorySize(width, height, depth, format);
						}

						switch (getType())
						{
							case TextureType::Texture1D:
								glTexImage1D(GL_TEXTURE_1D, i, glInternalFormat,
											 width,
											 0, m_glFormat, m_glType, levelData);
								break;

							case TextureType::Texture2D:
								glTexImage2D(GL_TEXTURE_2D, i, format,
											 width, height,
											 0, m_glFormat, m_glType, levelData);
								break;

							case TextureType::Texture3D:
								glTexImage3D(GL_TEXTURE_3D, i, format,
											 width, height, depth,
											 0, m_glFormat, m_glType, levelData);
								break;

							case TextureType::CubeTexture:
								// do every cube map face
								for (int j = 0; j < 6; j++)
									glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + j, i, format,
												 width, height,
												 0, m_glFormat, m_glType, levelData + faceSize*j);

								break;
						}

						if (width > 1)	 width = width / 2;
						if (height > 1)  height = height / 2;
						if (depth > 1)	 depth = depth / 2;

						for (int j = i + 1; j < levelCount; j++)
							m_levelOffsets[j] += levelSize;
					}
				}

				if (getUsage() & TU_AutoMipMap)
				{
					glGenerateMipmap(GL_TEXTURE_2D);
				}

				int32 totalSize = 0;
				for (int i = 0; i < getLevelCount(); i++)
				{
					totalSize += m_levelOffsets[i];
				}

				glGenBuffers(1, &m_mapPBO);
				glBindBuffer(GL_PIXEL_UNPACK_BUFFER, m_mapPBO);
				glBufferData(GL_PIXEL_UNPACK_BUFFER, totalSize, NULL, getUsage() == TU_Dynamic ? GL_DYNAMIC_DRAW : GL_STREAM_DRAW);

			}
		}
	}
}