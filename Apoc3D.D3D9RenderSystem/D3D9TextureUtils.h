#pragma once

#include "D3D9Common.h"

using namespace Apoc3D::IO;

namespace Apoc3D
{
	namespace Graphics
	{
		namespace D3D9RenderSystem
		{
			PLUGINAPI bool FindCompatibleTextureFormat(IDirect3DDevice9* device, int32 width, int32 height, int32 miplevels, PixelFormat format,
				int32& newWidth, int32& newHeight, int32& newMiplevels, PixelFormat& newFormat);

			PLUGINAPI bool FindCompatibleCubeTextureFormat(IDirect3DDevice9* device, int32 length, int32 miplevels, PixelFormat format,
				int32& newLength, int32& newMiplevels, PixelFormat& newFormat);

			PLUGINAPI bool FindCompatibleVolumeTextureFormat(IDirect3DDevice9* device, int32 width, int32 height, int32 depth, int32 miplevels, PixelFormat format,
				int32& newWidth, int32& newHeight, int32& newDepth, int32& newMiplevels, PixelFormat& newFormat);


			
			// These are 2 way(getting and setting) utility functions
			// for accessing the content of D3D9 textures.
			PLUGINAPI void copyData(void* tex, 
						  int pitch, void* texData, PixelFormat surfaceFormat,
						  DWORD dwLockWidth, DWORD dwLockHeight, bool isSetting);
			PLUGINAPI void copyData(void* tex,
						  int rowPitch, int slicePitch, void* texData,
						  PixelFormat surfaceFormat,
						  DWORD dwLockWidth, DWORD dwLockHeight, DWORD dwLockDepth,
						  bool isSetting);

			PLUGINAPI void getData(TextureData& data, D3DTexture2D* tex);
			PLUGINAPI void getData(TextureData& data, D3DTextureCube* tex);
			PLUGINAPI void getData(TextureData& data, D3DTexture3D* tex);

			PLUGINAPI void setData(const TextureData& data, D3DTextureCube* tex);
			PLUGINAPI void setData(const TextureData& data, D3DTexture2D* tex);
			PLUGINAPI void setData(const TextureData& data, D3DTexture3D* tex);

		}
	}
}