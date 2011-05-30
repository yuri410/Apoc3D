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


#include "Material.h"

#include "RenderSystem/Texture.h"
#include "EffectSystem/Effect.h"


namespace Apoc3D
{
	namespace Graphics
	{
		static String TAG_2_IsTransparent = L"IsTransparent";
		static String TAG_2_CullMode = L"CullMode";
		static String TAG_2_ZEnabled = L"ZEnabled";
		static String TAG_2_ZWriteEnabled = L"ZWriteEnabled";
		static String TAG_2_AlphaRef = L"AlphaRef";
		static String TAG_2_IsVegetation = L"IsVegetation";
		static String TAG_2_RenderPriority = L"RenderPriority";

		static String TAG_2_MaterialFlagTag = L"Flags";
		static String TAG_2_HasTextureTag = L"HasTexture";
		static String TAG_2_TextureTag = L"Texture";
		static String TAG_2_EffectTag = L"Effect";




		Material::Material()
			: m_passFlags(0), m_priority(2), 
			BlendFunction(BLFUN_Add), IsBlendTransparent(false), 
			SourceBlend(BLEND_SourceAlpha), DestinationBlend(BLEND_InverseSourceAlpha),
			AlphaTestEnable(false),
			DepthWriteEnable(true), DepthTestEnable(true),
			Ambient(0,0,0,0), Diffuse(1,1,1,1), Specular(0,0,0,0), Emissive(0,0,0,0), Power(0),
			Cull(CULL_None)
		{
			memset(m_tex, 0, sizeof(m_tex));
			memset(m_effects, 0, sizeof(m_effects));
		}


		Material::~Material(void)
		{

		}

		void Material::Load(istream &strm)
		{

		}
		void Material::Save(ostream &strm)
		{

		}
	}
};