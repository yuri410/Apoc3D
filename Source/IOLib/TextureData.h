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
#ifndef TEXTUREDATA_H
#define TEXTUREDATA_H

#include "Common.h"
#include "Graphics/PixelFormat.h"
#include "Graphics/GraphicsCommon.h"

using namespace Apoc3D::Graphics::RenderSystem;
using namespace Apoc3D::Graphics;
using namespace Apoc3D::VFS;

using namespace std;

namespace Apoc3D
{
	namespace IO
	{
		class APAPI TextureLevelData
		{
		public:
			int32 Height;
			int32 Width;
			int32 Depth;
			int32 LevelSize;
			char* ContentData;

			TextureLevelData()
			{ }
			~TextureLevelData() 
			{

			}



			void LoadData(TaggedDataReader* data);
			void SaveData(TaggedDataWriter* data) const;
		};
		class APAPI TextureData
		{
		public:
			TextureType Type;
			vector<TextureLevelData> Levels;
			PixelFormat Format;
			int32 ContentSize;
			int32 LevelCount;

			TextureData() {}
			~TextureData() {}
			/*TextureData(const TextureData& data)
				: Type(data.Type), Format(data.Format), ContentSize(data.ContentSize), LevelCount(data.LevelCount)
			{
				Levels.reserve(data.LevelCount);
				for (int i=0;i<data.LevelCount;i++)
				{
					Levels.push_back(TextureLevelData(Levels[i]))]
				}
			}*/


			void Load(const ResourceLocation* rl);
			void Save(Stream* strm) const;
		};
	}
}

#endif
