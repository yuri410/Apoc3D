#include "D3D9TextureUtils.h"
#include "D3D9Texture.h"
#include "D3D9RenderDevice.h"
#include "D3D9Utils.h"
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

			void getData(TextureData& data, D3DTextureCube* tex)
			{
				assert(data.Levels.getCount() == 0);
				data.ContentSize = 0;
				data.Levels.ReserveDiscard(data.LevelCount);

				for (int i = 0; i < data.LevelCount; i++)
				{
					int startPos = 0;

					D3DSURFACE_DESC desc;
					tex->GetLevelDesc(i, &desc);

					TextureLevelData& lvlData = data.Levels[i];
					lvlData.Width = (int32)desc.Width;
					lvlData.Height = (int32)desc.Width;
					lvlData.Depth = 1;
					lvlData.LevelSize = PixelFormatUtils::GetMemorySize(lvlData.Width, lvlData.Height, 1, data.Format) * 6;
					lvlData.ContentData = new char[lvlData.LevelSize];
					int faceSize = lvlData.LevelSize / 6;


					const D3DCUBEMAP_FACES faces[] =
					{
						D3DCUBEMAP_FACE_POSITIVE_X, D3DCUBEMAP_FACE_NEGATIVE_X,
						D3DCUBEMAP_FACE_POSITIVE_Y, D3DCUBEMAP_FACE_NEGATIVE_Y,
						D3DCUBEMAP_FACE_POSITIVE_Z, D3DCUBEMAP_FACE_NEGATIVE_Z,
					};

					for (D3DCUBEMAP_FACES cf : faces)
					{
						D3DLOCKED_RECT rect;
						HRESULT hr = tex->LockRect(cf, i, &rect, NULL, D3DLOCK_READONLY);
						assert(SUCCEEDED(hr));

						copyData(rect.pBits, rect.Pitch, lvlData.ContentData + startPos,
							data.Format, desc.Width, desc.Height, false);
						startPos += faceSize;

						hr = tex->UnlockRect(cf, i);
						assert(SUCCEEDED(hr));
					}

					// ======================================================================
					
					data.ContentSize += lvlData.LevelSize;
				}
			}
			void getData(TextureData& data, D3DTexture2D* tex)
			{
				assert(data.Levels.getCount() == 0);
				data.ContentSize = 0;
				data.Levels.ReserveDiscard(data.LevelCount);

				for (int i = 0; i < data.LevelCount; i++)
				{
					TextureLevelData& lvlData = data.Levels[i];

					D3DSURFACE_DESC desc;
					tex->GetLevelDesc(i, &desc);

					D3DLOCKED_RECT rect;
					HRESULT hr = tex->LockRect(i, &rect, NULL, D3DLOCK_READONLY);
					assert(SUCCEEDED(hr));

					lvlData.Width = (int32)desc.Width;
					lvlData.Height = (int32)desc.Height;
					lvlData.Depth = 1;
					lvlData.LevelSize = PixelFormatUtils::GetMemorySize(lvlData.Width, lvlData.Height, 1, data.Format);
					lvlData.ContentData = new char[lvlData.LevelSize];
					copyData(rect.pBits, rect.Pitch, lvlData.ContentData, data.Format, desc.Width, desc.Height, false);

					hr = tex->UnlockRect(i);
					assert(SUCCEEDED(hr));

					data.ContentSize += lvlData.LevelSize;
				}
			}
			void getData(TextureData& data, D3DTexture3D* tex)
			{
				assert(data.Levels.getCount() == 0);
				data.ContentSize = 0;
				data.Levels.ReserveDiscard(data.LevelCount);

				for (int i = 0; i < data.LevelCount; i++)
				{
					TextureLevelData& lvlData = data.Levels[i];

					D3DVOLUME_DESC desc;
					tex->GetLevelDesc(i, &desc);

					lvlData.Width = (int32)desc.Width;
					lvlData.Height = (int32)desc.Height;
					lvlData.Depth = (int32)desc.Depth;
					lvlData.LevelSize = PixelFormatUtils::GetMemorySize(lvlData.Width, lvlData.Height, desc.Depth, data.Format);
					lvlData.ContentData = new char[lvlData.LevelSize];

					D3DLOCKED_BOX box;
					HRESULT hr = tex->LockBox(i, &box, NULL, 0);
					assert(SUCCEEDED(hr));

					copyData(box.pBits, box.RowPitch, box.SlicePitch,
						lvlData.ContentData, data.Format,
						desc.Width, desc.Height, desc.Depth, false);

					hr = tex->UnlockBox(i);
					assert(SUCCEEDED(hr));
				}
			}

			void setData(const TextureData& data, D3DTextureCube* tex)
			{
				for (int i = 0; i < data.LevelCount; i++)
				{
					const TextureLevelData& lvlData = data.Levels[i];

					int startPos = 0;
					int faceSize = lvlData.LevelSize / 6;
					D3DSURFACE_DESC desc;
					tex->GetLevelDesc(i, &desc);

					const D3DCUBEMAP_FACES faces[] =
					{
						D3DCUBEMAP_FACE_POSITIVE_X, D3DCUBEMAP_FACE_NEGATIVE_X,
						D3DCUBEMAP_FACE_POSITIVE_Y, D3DCUBEMAP_FACE_NEGATIVE_Y,
						D3DCUBEMAP_FACE_POSITIVE_Z, D3DCUBEMAP_FACE_NEGATIVE_Z,
					};

					for (D3DCUBEMAP_FACES cf : faces)
					{
						D3DLOCKED_RECT rect;
						HRESULT hr = tex->LockRect(cf, i, &rect, NULL, 0);
						assert(SUCCEEDED(hr));

						copyData(rect.pBits, rect.Pitch, lvlData.ContentData + startPos,
							data.Format, desc.Width, desc.Height, true);
						startPos += faceSize;

						hr = tex->UnlockRect(cf, i);
						assert(SUCCEEDED(hr));
					}
				}
			}
			void setData(const TextureData& data, D3DTexture2D* tex)
			{
				for (int i = 0; i < data.LevelCount; i++)
				{
					const TextureLevelData& lvlData = data.Levels[i];

					D3DSURFACE_DESC desc;
					tex->GetLevelDesc(i, &desc);

					D3DLOCKED_RECT rect;
					HRESULT hr = tex->LockRect(i, &rect, NULL, 0);
					assert(SUCCEEDED(hr));

					copyData(rect.pBits, rect.Pitch, lvlData.ContentData, data.Format, desc.Width, desc.Height, true);

					hr = tex->UnlockRect(i);
					assert(SUCCEEDED(hr));
				}
			}
			void setData(const TextureData& data, D3DTexture3D* tex)
			{
				for (int i = 0; i < data.LevelCount; i++)
				{
					const TextureLevelData& lvlData = data.Levels[i];

					D3DVOLUME_DESC desc;
					tex->GetLevelDesc(i, &desc);

					D3DLOCKED_BOX box;
					HRESULT hr = tex->LockBox(i, &box, NULL, 0);
					assert(SUCCEEDED(hr));

					copyData(box.pBits, box.RowPitch, box.SlicePitch,
						lvlData.ContentData, data.Format,
						desc.Width, desc.Height, desc.Depth, true);

					hr = tex->UnlockBox(i);
					assert(SUCCEEDED(hr));
				}
			}



		}
	}
}