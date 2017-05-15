#pragma once
#ifndef APOC3D_MATERIALDATA_H
#define APOC3D_MATERIALDATA_H

/**
 * -----------------------------------------------------------------------------
 * This source file is part of Apoc3D Engine
 * 
 * Copyright (c) 2009+ Tao Xin
 * 
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 * 
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 * 
 * You should have received a copy of the GNU General Public License
 * along with this program.  if not, write to the Free Software Foundation, 
 * Inc., 59 Temple Place - Suite 330, Boston, MA 02111-1307, USA, or go to
 * http://www.gnu.org/copyleft/gpl.txt.
 * 
 * -----------------------------------------------------------------------------
 */


#include "apoc3d/Common.h"

#include "apoc3d/Graphics/GraphicsCommon.h"
#include "apoc3d/Graphics/MaterialTypes.h"
#include "apoc3d/Math/Color.h"

using namespace Apoc3D::Graphics;
using namespace Apoc3D::Graphics::RenderSystem;
using namespace Apoc3D::Math;
using namespace Apoc3D::VFS;

namespace Apoc3D
{
	namespace IO
	{
		/** Defines all the data in one material for serialization. */
		class APAPI MaterialData
		{
		public:

#if _DEBUG
			String DebugName;
#endif

			String ExternalRefName;		/** Name for retiring material reference  */

			CustomParamTable CustomParametrs;
			HashMap<int32, String> EffectNames;
			HashMap<int32, String> TextureNames;

			uint64 PassFlags = 1;

			uint32 Priority = DefaultMaterialPriority;  // [0, 31] Lower the higher the priority
			
			Blend SourceBlend = Blend::One;
			Blend DestinationBlend = Blend::Zero;
			BlendFunction BlendFunction = BlendFunction::Add;
			bool IsBlendTransparent = false;

			uint64 ColorWriteMasks = 0xffffffffffffffffull;

			CullMode Cull = CullMode::CounterClockwise;

			uint32 AlphaReference = 0;
			bool AlphaTestEnabled = false;

			bool UsePointSprite = false;

			bool DepthWriteEnabled = true;
			bool DepthTestEnabled = true;

			
			Color4 Ambient = Color4::Zero;		/**  the ambient component of this material  */
			Color4 Diffuse = Color4::One;		/**  the diffuse component of this material */
			Color4 Emissive = Color4::Zero;		/**  the emissive component of this material */
			Color4 Specular = Color4::Zero;		/**  the specular component of this material */
			float Power = 0;					/**  the specular shininess */


			MaterialData();
			~MaterialData();

			MaterialData(const MaterialData& other) = default;

			void AddCustomParameter(const MaterialCustomParameter& value);

			void LoadData(TaggedDataReader *data);
			TaggedDataWriter* SaveData();

			void Load(const ResourceLocation& rl);
			void Save(Stream& strm);
			
			void Parse(const ConfigurationSection* sect, const String& baseName = L"", FunctorReference<Color4(const String&)> colorParser = nullptr);

			void CopyNonDefaultFieldsFrom(MaterialData* mtrl, bool copyTextureNames);

			void SetTextureName(int32 index, const String& name);
		private:

			void ParseMaterialCustomParams(const String& value, FunctorReference<Color4(const String&)> colorParser);

			void CheckObsoleteProps();
		};

	}
}
#endif