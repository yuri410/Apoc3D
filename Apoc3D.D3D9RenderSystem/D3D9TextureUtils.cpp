#include "D3D9TextureUtils.h"
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
			bool IsPowerOfTwo(int32 x)
			{
				if (x == 0)
					return false;

				return (x & (x - 1)) == 0;
			}
			int32 SmallestGreaterPoT(int32 x)
			{
				if (x < 0)
					return 0;
				--x;
				x |= x >> 1;
				x |= x >> 2;
				x |= x >> 4;
				x |= x >> 8;
				x |= x >> 16;
				return x+1;
			}

			bool FindCompatibleTextureFormat(IDirect3DDevice9* device, int32 width, int32 height, int32 miplevels, PixelFormat format,
				int32& newWidth, int32& newHeight, int32& newMiplevels, PixelFormat& newFormat)
			{
				assert(device);

				HRESULT hr;
			
				IDirect3D9* d3d9;
				hr = device->GetDirect3D(&d3d9);
				assert(SUCCEEDED(hr));
				
				D3DDEVICE_CREATION_PARAMETERS params;
				hr = device->GetCreationParameters(&params);
				assert(SUCCEEDED(hr));

				D3DDISPLAYMODE mode;
				hr = device->GetDisplayMode(0, &mode);
				assert(SUCCEEDED(hr));

				newFormat = format;

				if (newFormat == FMT_Unknown)
					newFormat = FMT_A8R8G8B8;

				newWidth = width;
				newHeight = height;
				newMiplevels = miplevels;

				hr = d3d9->CheckDeviceFormat(params.AdapterOrdinal, params.DeviceType, mode.Format,
					0, D3DRTYPE_TEXTURE, D3D9Utils::ConvertPixelFormat(newFormat));
				if (FAILED(hr))
				{
					int bestscore = 0;
					PixelFormat bestFormat = FMT_Unknown;

					int32 originalChBitDepths[4];
					PixelFormatUtils::GetChannelBitDepth(newFormat, originalChBitDepths);

					int32 originalChCount = PixelFormatUtils::GetChannelCount(newFormat);
					bool allow_24bits = PixelFormatUtils::GetBPP(newFormat) == 3;

					for (int32 i=FMT_Unknown+1;i<FMT_Count;i++)
					{
						PixelFormat curFmt = (PixelFormat)i;

						if (PixelFormatUtils::IsCompressed(curFmt))
							continue;

						int32 chnCount = PixelFormatUtils::GetChannelCount(curFmt);
						if (chnCount<originalChCount)
							continue;

						if (!allow_24bits && PixelFormatUtils::GetBPP(curFmt)==3)
							continue;


						hr = d3d9->CheckDeviceFormat(params.AdapterOrdinal, params.DeviceType,
							mode.Format, 0, D3DRTYPE_TEXTURE, D3D9Utils::ConvertPixelFormat(curFmt));
						if (FAILED(hr))
							continue;

						int32 score = 16 - 4 * (chnCount - originalChCount);

						int32 curChBitDepths[4];
						PixelFormatUtils::GetChannelBitDepth(curFmt, curChBitDepths);

						for (int32 j = 0; j < 4; j++)
						{
							int penalty = curChBitDepths[j] - originalChBitDepths[j];
							if (penalty<0)
								penalty = -penalty*4;
							score += 16 - penalty;
						}

						if (score > bestscore)
						{
							bestscore = score;
							bestFormat = curFmt;
							newFormat = curFmt;
						}
					}
				}

				D3DCAPS9 caps;
				hr = device->GetDeviceCaps(&caps);
				assert(SUCCEEDED(hr));


				if ((caps.TextureCaps & D3DPTEXTURECAPS_POW2) && (!IsPowerOfTwo(newWidth)))
					newWidth = SmallestGreaterPoT(newWidth);

				if (newWidth > (int32)caps.MaxTextureWidth)
					newWidth = (int32)caps.MaxTextureWidth;

				if ((caps.TextureCaps & D3DPTEXTURECAPS_POW2) && (!IsPowerOfTwo(newHeight)))
					newHeight = SmallestGreaterPoT(newHeight);

				if (newHeight > (int32)caps.MaxTextureHeight)
					newHeight = (int32)caps.MaxTextureHeight;


				if (caps.TextureCaps & D3DPTEXTURECAPS_SQUAREONLY)
				{
					newWidth = newHeight = Math::Max(newWidth, newHeight);
				}

				if (newMiplevels>1)
				{
					int32 mipCount = 1;

					int32 max_dimen = Math::Max(newWidth, newHeight);
					while (max_dimen > 1)
					{
						max_dimen >>= 1;
						mipCount++;
					}

					if (newMiplevels>mipCount)
						newMiplevels = mipCount;
				}

				d3d9->Release();

				return newFormat != FMT_Unknown;
			}

			bool FindCompatibleCubeTextureFormat(IDirect3DDevice9* device, int32 length, int32 miplevels, PixelFormat format,
				int32& newLength, int32& newMiplevels, PixelFormat& newFormat)
			{
				HRESULT hr;
				D3DCAPS9 caps;
				hr = device->GetDeviceCaps(&caps);
				assert(SUCCEEDED(hr));

				if (!(caps.TextureCaps & D3DPTEXTURECAPS_CUBEMAP))
					return false;

				newLength = length;
				newMiplevels = miplevels;

				if ((caps.TextureCaps & D3DPTEXTURECAPS_CUBEMAP_POW2) && (!IsPowerOfTwo(newLength)))
					newLength = SmallestGreaterPoT(newLength);

				int32 dummy1;
				if (!FindCompatibleTextureFormat(device, newLength, newLength, miplevels, format, dummy1, newLength, newMiplevels, format))
					return false;

				if (!(caps.TextureCaps & D3DPTEXTURECAPS_MIPCUBEMAP))
					newMiplevels = 1;

				return true;
			}

			bool FindCompatibleVolumeTextureFormat(IDirect3DDevice9* device, int32 width, int32 height, int32 depth, int32 miplevels, PixelFormat format,
				int32& newWidth, int32& newHeight, int32& newDepth, int32& newMiplevels, PixelFormat& newFormat)
			{
				HRESULT hr;
				D3DCAPS9 caps;
				hr = device->GetDeviceCaps(&caps);
				assert(SUCCEEDED(hr));

				if (!(caps.TextureCaps & D3DPTEXTURECAPS_VOLUMEMAP))
					return false;

				if (!FindCompatibleTextureFormat(device, width, height, miplevels, format, newWidth, newHeight, newMiplevels, format))
					return false;

				newDepth = depth;

				/* ensure width/height is power of 2 */
				if (caps.TextureCaps & D3DPTEXTURECAPS_VOLUMEMAP_POW2)
				{
					if (!IsPowerOfTwo(newWidth)) 
						newWidth = SmallestGreaterPoT(newWidth);

					if (!IsPowerOfTwo(newHeight)) 
						newHeight = SmallestGreaterPoT(newHeight);

					if (!IsPowerOfTwo(newDepth)) 
						newDepth = SmallestGreaterPoT(newDepth);
				}

				if (newWidth > (int32)caps.MaxVolumeExtent)
					newWidth = (int32)caps.MaxVolumeExtent;
				if (newHeight > (int32)caps.MaxVolumeExtent)
					newHeight = (int32)caps.MaxVolumeExtent;
				if (newDepth > (int32)caps.MaxVolumeExtent)
					newDepth = (int32)caps.MaxVolumeExtent;

				if (newMiplevels > 1)
				{
					if (!(caps.TextureCaps & D3DPTEXTURECAPS_MIPVOLUMEMAP))
						newMiplevels = 1;
					else
					{
						int32 mipCount = 1;
						int32 maxDimension = Math::Max(Math::Max(newWidth, newHeight), newDepth);

						while (maxDimension > 1)
						{
							maxDimension >>= 1;
							mipCount++;
						}

						if (newMiplevels > mipCount)
							newMiplevels = mipCount;
					}
				}

				return true;

			}


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


					const D3DCUBEMAP_FACES faces[] = 
					{
						D3DCUBEMAP_FACE_POSITIVE_X, D3DCUBEMAP_FACE_NEGATIVE_X,
						D3DCUBEMAP_FACE_POSITIVE_Y, D3DCUBEMAP_FACE_NEGATIVE_Y,
						D3DCUBEMAP_FACE_POSITIVE_Z, D3DCUBEMAP_FACE_NEGATIVE_Z,
					};

					for (int32 j=0;j<ARRAYSIZE(faces);j++)
					{
						D3DLOCKED_RECT rect;
						HRESULT hr = tex->LockRect(faces[j], i, &rect, NULL, D3DLOCK_READONLY);
						assert(SUCCEEDED(hr));

						copyData(rect.pBits, rect.Pitch, data.Levels[i].ContentData+startPos,
							data.Format, desc.Width, desc.Height, false);
						startPos += faceSize;

						hr = tex->UnlockRect(faces[j], i);
						assert(SUCCEEDED(hr));
					}

					// ======================================================================
					data.Levels.Add(lvlData);
					data.ContentSize += lvlData.LevelSize;

				}
			}
			void getData(TextureData& data, D3DTexture2D* tex)
			{
				assert(data.Levels.getCount() == 0);
				data.ContentSize = 0;

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
			void getData(TextureData& data, D3DTexture3D* tex)
			{
				assert(data.Levels.getCount() == 0);
				data.ContentSize = 0;

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

					const D3DCUBEMAP_FACES faces[] = 
					{
						D3DCUBEMAP_FACE_POSITIVE_X, D3DCUBEMAP_FACE_NEGATIVE_X,
						D3DCUBEMAP_FACE_POSITIVE_Y, D3DCUBEMAP_FACE_NEGATIVE_Y,
						D3DCUBEMAP_FACE_POSITIVE_Z, D3DCUBEMAP_FACE_NEGATIVE_Z,
					};

					for (int32 j=0;j<ARRAYSIZE(faces);j++)
					{
						D3DLOCKED_RECT rect;
						HRESULT hr = tex->LockRect(faces[j], i, &rect, NULL, 0);
						assert(SUCCEEDED(hr));

						copyData(rect.pBits, rect.Pitch, data.Levels[i].ContentData+startPos,
							data.Format, desc.Width, desc.Height, true);
						startPos += faceSize;

						hr = tex->UnlockRect(faces[j], i);
						assert(SUCCEEDED(hr));
					}

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



		}
	}
}