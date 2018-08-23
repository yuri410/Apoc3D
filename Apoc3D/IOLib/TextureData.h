#pragma once
#ifndef APOC3D_TEXTUREDATA_H
#define APOC3D_TEXTUREDATA_H

/* -----------------------------------------------------------------------
 * This source file is part of Apoc3D Framework
 * 
 * Copyright (c) 2011-2017 Tao Xin
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

#include "apoc3d/ApocCommon.h"
#include "apoc3d/Collections/List.h"
#include "apoc3d/Graphics/PixelFormat.h"
#include "apoc3d/Graphics/GraphicsCommon.h"

using namespace Apoc3D::Collections;
using namespace Apoc3D::Graphics::RenderSystem;
using namespace Apoc3D::Graphics;
using namespace Apoc3D::VFS;

namespace Apoc3D
{
	namespace IO
	{
		/** 
		 * Defines one entire texture level's data stored in binary form and procedures to load/save them.
		 */
		class APAPI TextureLevelData
		{
		public:
			int32 Width;
			int32 Height;
			int32 Depth;
			int32 LevelSize;		/**  The size of the content data in bytes. */

			char* ContentData = nullptr;		/** Packed pixel data. */

			TextureLevelData();
			~TextureLevelData();
			
			TextureLevelData(TextureLevelData&& rhs);
			TextureLevelData& operator=(TextureLevelData&& rhs);

			TextureLevelData(const TextureLevelData&) = delete;
			TextureLevelData& operator=(const TextureLevelData&) = delete;

			void LoadData(BinaryReader& br, bool doNotLoadContent = false, int32 flags = 0);
			void SaveData(BinaryWriter& bw, int32 flags = 0) const;

			void LoadData(TaggedDataReader* data, bool doNotLoadContent = false, int32 flags = 0);
			void SaveData(TaggedDataWriter* data, int32 flags = 0) const;

		};

		/** 
		 *  Defines one entire texture's data stored in binary form and procedures to load/save them
		 *  from ResourceLocations and Streams.
		 */
		class APAPI TextureData
		{
		public:
			static const uint32 FileID1 = FourCC("ATEX");
			static const uint32 FileID2 = FourCC("1TEX");

			enum TextureDataFlags
			{
				TDF_None = 0,
				TDF_RLECompressed = 1U << 0,
				TDF_LZ4Compressed = 1U << 1
			};

			TextureType Type;
			List<TextureLevelData> Levels;
			PixelFormat Format;
			int32 ContentSize;
			int32 LevelCount;
			uint32 Flags;

			TextureData();
			~TextureData();
			
			TextureData(TextureData&& rhs);
			TextureData& operator=(TextureData&& rhs);

			TextureData(const TextureData&) = delete;
			TextureData& operator=(const TextureData&) = delete;

			void Load(const ResourceLocation& rl, bool doNotLoadLevel = false, bool doNotLoadContent = false);
			void Save(Stream& strm) const;
			void SaveAsTagged(Stream& strm) const;

			void ConvertInPlace(PixelFormat fmt);
			void ResizeInPlace(int32 newWidth, int32 newHeight);

		};
	}
}

#endif
