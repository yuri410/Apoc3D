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

using namespace Apoc3D::EffectSystem;
using namespace Apoc3D::Core;
using namespace Apoc3D::Math;
using namespace Apoc3D::Graphics::RenderSystem;

using namespace std;

namespace Apoc3D
{
	namespace Graphics
	{
		/* Defines colors, textures and effect info for a geometry

		   Material have some Effects. Each Effect is a set of Effect Atoms. 
		   It is an indicator that tells the batching system all passes 
		   this Material supports.

		   The other parameters are auto mapped with parameters in atom effects.
		*/
		class APAPI Material : public HashHandleObject
		{
		public:
			
		private:
			unordered_map<uint64, Effect*> m_eff;
			vector<Texture*> m_tex;

			Color4 m_ambient;
			Color4 m_diffuse;
			Color4 m_emissive;
			Color4 m_Specular;
			float m_power;

			Blend m_srcBlend;
			Blend m_dstBlend;
			BlendFunction m_blendFunction;
			bool m_alphaBlendEnable;

			bool m_alphaTestEnable;

			bool m_zWriteEnable;
			bool m_zTestEnable;

			uint64 m_passFlags;

			int32 m_priority;
		public:
			/* Gets the texture at texture layer idx
			*/
			Texture* getTexture(int idx) const { return m_tex[idx]; }
			/* Sets the texture at texture layer idx
			*/
			void setTexture(int idx, Texture* value) { m_tex[idx] = value; }

			/* Gets the ambient component of this material
			*/
			const Color4& getAmbient() const { return m_ambient; }
			/* Gets the diffuse component of this material
			*/
			const Color4& getDiffuse() const { return m_diffuse; }
			/* Gets the emissive component of this material
			*/
			const Color4& getEmissive() const { return m_emissive; }
			/* Gets the specular component of this material
			*/
			const Color4& getSpecular() const { return m_Specular; }
			/* Gets the specular shineness
			*/
			const float getPower() const { return m_power; }

			const uint32 getPriority() const { return m_priority; }
			void setPriority(uint32 value) { m_priority = value; }

			uint64 getPassFlags() const { return m_passFlags; }
			void setPassFlags(uint64 val) { m_passFlags = val; }

			void setAmbient(const Color4& value) { m_ambient = value; }
			void setDiffuse(const Color4& value) { m_diffuse = value; }
			void setEmissive(const Color4& value) { m_emissive = value; }
			void setSpecular(const Color4& value) { m_Specular = value; }
			void setPower(const float value) { m_power = value; }

			void Load(istream &strm);
			void Save(ostream &strm);

			Material();
			~Material(void);

		};
	};
};
#endif