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

#ifndef MATERIALDATA_H
#define MATERIALDATA_H

#include "Common.h"
#include "Graphics/GraphicsCommon.h"
#include "Graphics/MaterialTypes.h"
#include "Math/Color.h"

using namespace Apoc3D::Graphics;
using namespace Apoc3D::Graphics::RenderSystem;
using namespace Apoc3D::Math;

using namespace std;

namespace Apoc3D
{
	namespace IO
	{
		/** Defines all of one material's data stored in binary form and procedures to load them.
		*/
		class APAPI MaterialData
		{
		private:
			void LoadTexture(BinaryReader* br, int32 index);
			void SaveTexture(BinaryWriter* bw, int32 index);

			void LoadEffect(BinaryReader* br, int32 index);
			void SaveEffect(BinaryWriter* bw, int32 index);

			/** Load with format version 2. The version used in making ZoneLink 2
			*/
			void LoadV2(TaggedDataReader* data);
			/** Load with format version 3
			*/
			void LoadV3(TaggedDataReader* data);
		public:
			String EffectName[MaxScenePass];

			CustomParamTable CustomParametrs;
			String TextureName[MaxTextures];

			uint64 PassFlags;

			/** 
			*/
			int32 Priority;
			
			Blend SourceBlend;
			Blend DestinationBlend;
			BlendFunction BlendFunction;
			bool IsBlendTransparent;

			CullMode Cull;

			bool AlphaTestEnabled;
			uint32 AlphaReference;

			bool DepthWriteEnabled;
			bool DepthTestEnabled;

			/** the ambient component of this material
			*/
			Color4 Ambient;
			/** the diffuse component of this material
			*/
			Color4 Diffuse;
			/** the emissive component of this material
			*/
			Color4 Emissive;
			/** the specular component of this material
			*/
			Color4 Specular;
			/** the specular shininess
			*/
			float Power;

			
			void AddCustomParameter(const MaterialCustomParameter& value);

			void SetDefaults()
			{
				DepthTestEnabled = DepthWriteEnabled = true;
				Cull = CULL_CounterClockwise;
				IsBlendTransparent = false;
				BlendFunction = BLFUN_Add;
				SourceBlend = BLEND_One;
				DestinationBlend = BLEND_Zero;
				Priority = 64;
				PassFlags = 1;

				AlphaReference = 0;
				AlphaTestEnabled = false;

				Ambient = Color4(0,0,0,0);
				Diffuse = Color4(1,1,1,1);
				Emissive = Color4(0,0,0,0);
				Specular = Color4(0,0,0,0);
				Power = 0;
			}

			//MaterialData(const Material* mtrl);
			MaterialData(void) { }
			~MaterialData(void) { }

			void Load(TaggedDataReader *data);
			TaggedDataWriter* Save();
		};

	}
}
#endif