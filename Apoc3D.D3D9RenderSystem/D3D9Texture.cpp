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

#include "D3D9Texture.h"
#include "D3D9RenderDevice.h"
#include "D3D9Utils.h"
#include "Math/Rectangle.h"
#include "Math/Box.h"
#include "Vfs/ResourceLocation.h"
#include "IO/TextureData.h"

#include "Apoc3DException.h"

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
				m_renderDevice(device), m_tex2D(tex2D), m_tex3D(0), m_cube(0)
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
				m_renderDevice(device), m_tex2D(0), m_tex3D(tex3D), m_cube(0)
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
				m_renderDevice(device), m_tex2D(0), m_tex3D(0), m_cube(texCube)
			{

			}

			D3D9Texture::D3D9Texture(D3D9RenderDevice* device, ResourceLocation* rl, TextureUsage usage, bool managed)
				: Texture(device, rl, usage, managed),
				m_renderDevice(device)
			{
				if (!managed)
				{
					load();
				}
			}
			D3D9Texture::D3D9Texture(D3D9RenderDevice* device, int32 width, int32 height, int32 depth, int32 level, 
				PixelFormat format, TextureUsage usage)
				: Texture(device, width, height, depth, level, format, usage),
				m_renderDevice(device)
			{
				D3DDevice* dev = device->getDevice();

				if (getType() == TT_Texture2D || getType() == TT_Texture1D)
				{
					HRESULT hr = dev->CreateTexture(width, height, level, 
						D3D9Utils::ConvertTextureUsage(usage), D3D9Utils::ConvertPixelFormat(format), D3DPOOL_MANAGED, &m_tex2D, NULL);;
					assert(SUCCEEDED(hr));
				}
				else if (getType() == TT_Texture3D)
				{
					HRESULT hr = dev->CreateVolumeTexture(width, height,depth, level, 
						D3D9Utils::ConvertTextureUsage(usage), D3D9Utils::ConvertPixelFormat(format), D3DPOOL_MANAGED, &m_tex3D, NULL);;
					assert(SUCCEEDED(hr));
				}
			}

			D3D9Texture::D3D9Texture(D3D9RenderDevice* device, int32 length, int32 level, PixelFormat format, TextureUsage usage)
				: Texture(device, length, level, usage, format), 
				m_renderDevice(device), m_tex2D(0), m_tex3D(0)
			{
				D3DDevice* dev = device->getDevice();

				HRESULT hr = dev->CreateCubeTexture(length, level, 
					D3D9Utils::ConvertTextureUsage(usage), D3D9Utils::ConvertPixelFormat(format), D3DPOOL_MANAGED, &m_cube, NULL);
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
			}

			void D3D9Texture::Save(Stream* strm)
			{
				Apoc3DException::createException(EX_NotSupported, L"");
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

				HRESULT hr = m_cube->LockRect(D3D9Utils::ConvertCubeMapFace(cubemapFace), surface, &rrect, &rect0, D3D9Utils::ConvertLockMode(mode));
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
				}
			}
			void D3D9Texture::unlock(CubeMapFace cubemapFace, int32 surface)
			{
				m_cube->UnlockRect(D3D9Utils::ConvertCubeMapFace(cubemapFace), surface);
			}

			void copyData(const D3DLOCKED_BOX& box, const TextureLevelData& ldata)
			{
				if (box.RowPitch == ldata.Width && box.SlicePitch == (ldata.Height * ldata.Width))
				{
					memcpy(box.pBits, 
						ldata.ContentData, 
						ldata.LevelSize);
				}
				else
				{
					int32 lineSize = ldata.LevelSize / (ldata.Depth*ldata.Height);
					int32 sliceSize = ldata.LevelSize / ldata.Depth;

					int ofs = 0;
					char* dest = reinterpret_cast<char*>(box.pBits);

					for (int i=0;i<ldata.Depth;i++)
					{
						for (int j=0;j<ldata.Height;j++)
						{
							memcpy(dest, ldata.ContentData + (j*lineSize), lineSize);
							ofs += box.RowPitch;
						}
						ofs = box.SlicePitch * i;
					}
				}
			}
			void copyData(const D3DLOCKED_RECT& rect, const TextureLevelData& ldata, int32 offset = 0)
			{
				if (rect.Pitch == ldata.Width)
				{
					memcpy(rect.pBits, 
						ldata.ContentData+offset, 
						ldata.LevelSize);
				}
				else
				{
					char* dest = reinterpret_cast<char*>(rect.pBits);
					int32 lineSize = ldata.LevelSize / ldata.Height;

					for (int j=0;j<ldata.Height;j++)
					{
						memcpy(dest, ldata.ContentData + (j*lineSize)+offset, lineSize);
						dest += rect.Pitch;
					}
				}
			}
			void setData(const TextureData& data, D3DTextureCube* tex)
			{
				for (int i=0;i<data.LevelCount;i++)
				{
					int startPos = 0;
					int levelSize = data.Levels[i].LevelSize / 6;

					D3DLOCKED_RECT rect;
					HRESULT hr = tex->LockRect(D3DCUBEMAP_FACE_POSITIVE_X, i, &rect, NULL, 0);
					assert(SUCCEEDED(hr));

					copyData(rect, data.Levels[i], startPos);
					startPos += levelSize;

					hr = tex->UnlockRect(D3DCUBEMAP_FACE_POSITIVE_X, i);
					assert(SUCCEEDED(hr));

					// ======================================================================

					hr = tex->LockRect(D3DCUBEMAP_FACE_NEGATIVE_X, i, &rect, NULL, 0);
					assert(SUCCEEDED(hr));

					copyData(rect, data.Levels[i], startPos);
					startPos += levelSize;

					hr = tex->UnlockRect(D3DCUBEMAP_FACE_NEGATIVE_X, i);
					assert(SUCCEEDED(hr));

					// ======================================================================
					// ======================================================================
					hr = tex->LockRect(D3DCUBEMAP_FACE_POSITIVE_Y, i, &rect, NULL, 0);
					assert(SUCCEEDED(hr));

					copyData(rect, data.Levels[i], startPos);
					startPos += levelSize;

					hr = tex->UnlockRect(D3DCUBEMAP_FACE_POSITIVE_Y, i);
					assert(SUCCEEDED(hr));

					// ======================================================================
					hr = tex->LockRect(D3DCUBEMAP_FACE_NEGATIVE_Y, i, &rect, NULL, 0);
					assert(SUCCEEDED(hr));

					copyData(rect, data.Levels[i], startPos);
					startPos += levelSize;

					hr = tex->UnlockRect(D3DCUBEMAP_FACE_NEGATIVE_Y, i);
					assert(SUCCEEDED(hr));

					// ======================================================================
					// ======================================================================

					hr = tex->LockRect(D3DCUBEMAP_FACE_POSITIVE_Z, i, &rect, NULL, 0);
					assert(SUCCEEDED(hr));

					copyData(rect, data.Levels[i], startPos);
					startPos += levelSize;

					hr = tex->UnlockRect(D3DCUBEMAP_FACE_POSITIVE_Z, i);
					assert(SUCCEEDED(hr));
					// ======================================================================
					hr = tex->LockRect(D3DCUBEMAP_FACE_NEGATIVE_Z, i, &rect, NULL, 0);
					assert(SUCCEEDED(hr));

					copyData(rect, data.Levels[i], startPos);
					startPos += levelSize;

					hr = tex->UnlockRect(D3DCUBEMAP_FACE_NEGATIVE_Z, i);
					assert(SUCCEEDED(hr));

					// ======================================================================


				}
			}
			void setData(const TextureData& data, D3DTexture2D* tex)
			{
				for (int i=0;i<data.LevelCount;i++)
				{
					D3DLOCKED_RECT rect;
					HRESULT hr = tex->LockRect(i, &rect, NULL, 0);
					assert(SUCCEEDED(hr));

					copyData(rect, data.Levels[i]);

					hr = tex->UnlockRect(i);
					assert(SUCCEEDED(hr));
				}
			}
			void setData(const TextureData& data, D3DTexture3D* tex)
			{
				for (int i=0;i<data.LevelCount;i++)
				{
					D3DLOCKED_BOX rect;
					HRESULT hr = tex->LockBox(i, &rect, NULL, 0);
					assert(SUCCEEDED(hr));

					copyData(rect, data.Levels[i]);

					hr = tex->UnlockBox(i);
					assert(SUCCEEDED(hr));
				}
			}

			void D3D9Texture::load()
			{
				TextureData data;
				data.Load(getResourceLocation());
				
				UpdateInfo(data);

				D3DDevice* dev = m_renderDevice->getDevice();

				switch (data.Type)
				{
				case (int)TT_Texture1D:
				case (int)TT_Texture2D:
					{
						HRESULT hr = dev->CreateTexture(getWidth(), getHeight(), getLevelCount(), 
							D3D9Utils::ConvertTextureUsage(getUsage()), D3D9Utils::ConvertPixelFormat(getFormat()), 
							D3DPOOL_MANAGED, &m_tex2D, NULL);
						assert(SUCCEEDED(hr));
						
						setData(data, m_tex2D);
					}					
					break;
				case (int)TT_CubeTexture:
					{
						HRESULT hr = dev->CreateCubeTexture(getWidth(), getLevelCount(), 
							D3D9Utils::ConvertTextureUsage(getUsage()), D3D9Utils::ConvertPixelFormat(getFormat()), 
							D3DPOOL_MANAGED, &m_cube, NULL);
						assert(SUCCEEDED(hr));

						setData(data, m_cube);
					}
					
					break;
				case (int)TT_Texture3D:
					{
						HRESULT hr = dev->CreateCubeTexture(getWidth(), getLevelCount(), 
							D3D9Utils::ConvertTextureUsage(getUsage()), D3D9Utils::ConvertPixelFormat(getFormat()), 
							D3DPOOL_MANAGED, &m_cube, NULL);
						assert(SUCCEEDED(hr));

						setData(data, m_tex3D);
					}
					break;
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
		}
	}
}