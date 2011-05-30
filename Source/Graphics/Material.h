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

#pragma once

#include "Common.h"
#include "Core\HashHandleObject.h"
#include "Math\Color.h"
#include "GraphicsCommon.h"
#include "Core/ResourceHandle.h"

using namespace Apoc3D::Graphics::EffectSystem;
using namespace Apoc3D::Core;
using namespace Apoc3D::Math;
using namespace Apoc3D::Graphics::RenderSystem;

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
			MTRLPT_Int32
		};

		struct MaterialCustomParameter
		{
			MaterialCustomParameterType Type;
			byte buffer[16];
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
			Effect* m_effects[MaxScenePass];
			String m_effectName[MaxScenePass];

			unordered_map<String, MaterialCustomParameter> m_customParametrs;
			ResourceHandle<Texture>* m_tex[MaxTextures];
			String m_texName[MaxTextures];

			uint64 m_passFlags;

			int32 m_priority;

			ResourceHandle<Texture>* LoadTexture(BinaryReader* br);
			void SaveTexture(BinaryWriter* bw, ResourceHandle<Texture>* tex);
		public:

			Blend SourceBlend;
			Blend DestinationBlend;
			BlendFunction BlendFunction;
			bool IsBlendTransparent;

			CullMode Cull;

			bool AlphaTestEnable;

			bool DepthWriteEnable;
			bool DepthTestEnable;

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


			const MaterialCustomParameter& getCustomParameter() const;
			void setCustomParameter(const MaterialCustomParameter& value);

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


			void Load(Stream* strm);
			void Save(Stream* strm);

			Material();
			~Material(void);

		};
	};
};
#endif