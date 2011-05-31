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

#ifndef MATERIAL_H
#define MATERIAL_H

#include "Common.h"
#include "Core\HashHandleObject.h"
#include "Math\Color.h"
#include "GraphicsCommon.h"

using namespace Apoc3D::Graphics::EffectSystem;
using namespace Apoc3D::Core;
using namespace Apoc3D::Math;
using namespace Apoc3D::Graphics::RenderSystem;
using namespace Apoc3D::IO;

using namespace std;

namespace Apoc3D
{
	namespace Graphics
	{
		enum MaterialCustomParameterType
		{
			MTRLPT_Float,
			MTRLPT_Vector2,
			MTRLPT_Vector4,
			MTRLPT_Boolean,
			MTRLPT_Integer,
			
		};

		/** Defines custom material parameters. 
			The value can be 16 bytes maximum.
		*/
		struct MaterialCustomParameter
		{
			/** The data type of the parameter.
			*/
			MaterialCustomParameterType Type;
			byte Value[16];

			/** The usage of this parameter. Effect check this for auto binding effect parameters.
			*/
			String Usage;

			MaterialCustomParameter() { }
			MaterialCustomParameter(bool value, const String usage = L"")
				: Type(MTRLPT_Boolean), Usage(usage)
			{
				*reinterpret_cast<bool*>(Value) = value;
			}
		};

		template class APAPI unordered_map<uint64, Effect*>;
		template class APAPI unordered_map<String, uint64>;
		template class APAPI vector<Texture*>;

		/* Defines colors, textures, effect and etc. for a geometry.

		   Material have some Effects. Scene will be rendered in single or
		   multiple pass. Each pass has sequence number and its corresponding mask value.
		   
		   First the mask value will do and op with pass flags of the material. If that passes, 
		   a pass effect will be retrieved from the material with the pass sequence number
		   as index.
		*/
		class APAPI Material : public HashHandleObject
		{
		public:
			static const int32 MaxTextures = 16;
		private:
			typedef unordered_map<String, MaterialCustomParameter> CustomParamTable;

			RenderDevice* m_device;

			Effect* m_effects[MaxScenePass];
			String m_effectName[MaxScenePass];

			CustomParamTable m_customParametrs;
			ResourceHandle<Texture>* m_tex[MaxTextures];
			String m_texName[MaxTextures];
			bool m_texDirty[MaxTextures];

			uint64 m_passFlags;

			int32 m_priority;

			void LoadTexture(BinaryReader* br, int32 index);
			void SaveTexture(BinaryWriter* bw, int32 index);

			void LoadEffect(BinaryReader* br, int32 index);
			void SaveEffect(BinaryWriter* bw, int32 index);

		public:

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


			const MaterialCustomParameter* getCustomParameter(const String& usage) const;
			void AddCustomParameter(const MaterialCustomParameter& value);

			Effect* getPassEffect(int index) const { return m_effects[index]; }
			void setPassEffect(int index, Effect* eff) { m_effects[index] = eff; }

			/* Gets the texture at texture layer idx
			*/
			ResourceHandle<Texture>* getTexture(int idx) const { return m_tex[idx]; }
			/* Sets the texture at texture layer idx
			*/
			void setTexture(int idx, ResourceHandle<Texture>* value) { m_tex[idx] = value; }

			const uint32 getPriority() const { return m_priority; }
			void setPriority(uint32 value) { m_priority = value; }

			/** Gets the pass flags.
				The pass flag is a bit field used for selecting material when rendering.
				If a bit is one, objects with this material will be rendered when the 
				scene render script has requested that bit.
			*/
			uint64 getPassFlags() const { return m_passFlags; }
			void setPassFlags(uint64 val) { m_passFlags = val; }


			/** Load with format version 2. The version used in making ZoneLink 2
			*/
			void LoadV2(TaggedDataReader* data);
			/** Load with format version 3
			*/
			void LoadV3(TaggedDataReader* data);
			void Load(TaggedDataReader* data);
			TaggedDataWriter* Save();

			Material(RenderDevice* device);
			~Material(void);

		};
	};
};
#endif