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
#include "io/Streams.h"
#include "io/TaggedData.h"
#include "Core\ResourceHandle.h"

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

		static String TAG_2_MaterialColorTag = L"MaterialColor";

		static String TAG_2_MaterialFlag = L"Flags";
		static String TAG_2_HasTexture = L"HasTexture";
		static String TAG_2_Texture = L"Texture";
		static String TAG_2_Effect = L"Effect";

		
		// =============================================================

		static String TAG_3_CustomParamCount = L"CustomParamCount";
		static String TAG_3_CustomParam = L"CustomParam";

		static String TAG_3_HasTexture = L"HasTexture";
		static String TAG_3_Texture = L"Texture";
		static String TAG_3_HasEffect = L"HasEffect";
		static String TAG_3_Effect = L"Effect";


		static String TAG_3_RenderPriority = L"RenderPriority";
		static String TAG_3_PassFlags = L"PassFlags";

		static String TAG_3_IsBlendTransparent = L"IsBlendTransparent";
		static String TAG_3_SourceBlend = L"SourceBlend";
		static String TAG_3_DestinationBlend = L"DestinationBlend";
		static String TAG_3_BlendFunction = L"BlendFunction";

		static String TAG_3_CullMode = L"CullMode";


		static String TAG_3_AlphaReference = L"AlphaReference";
		static String TAG_3_AlphaTestEnable = L"AlphaTestEnable";

		static String TAG_3_DepthTestEnabled = L"DepthTestEnabled";
		static String TAG_3_DepthWriteEnabled = L"DepthWriteEnabled";

		static String TAG_2_MaterialColorTag = L"MaterialColor";


		static String TAG_3_AlphaRef = L"AlphaReference";


		//static String TAG_3_Effect = L"Effect";



		Material::Material(RenderDevice* device)
			: m_device(device),
			m_passFlags(0), m_priority(2), 
			BlendFunction(BLFUN_Add), IsBlendTransparent(false), 
			SourceBlend(BLEND_SourceAlpha), DestinationBlend(BLEND_InverseSourceAlpha),
			AlphaTestEnabled(false),
			DepthWriteEnabled(true), DepthTestEnabled(true),
			Ambient(0,0,0,0), Diffuse(1,1,1,1), Specular(0,0,0,0), Emissive(0,0,0,0), Power(0),
			Cull(CULL_None)
		{
			memset(m_tex, 0, sizeof(m_tex));
			memset(m_effects, 0, sizeof(m_effects));
		}


		Material::~Material(void)
		{
			for (int i=0;i<MaxTextures;i++)
			{
				if (m_tex[i])
				{
					delete m_tex[i];
					m_tex[i] = 0;
				}
			}
		}

		void Material::AddCustomParameter(const MaterialCustomParameter& value)
		{
			if (value.Usage.empty())
			{

			}
		}

		Effect* Material::LoadEffect(const String& name)
		{
			return 0;
		}

		ResourceHandle<Texture>* Material::LoadTexture(BinaryReader* br)
		{
			return 0;
		}
		void Material::SaveTexture(BinaryWriter* bw, ResourceHandle<Texture>* tex)
		{

		}

		void Material::LoadV2(TaggedDataReader* data)
		{
			AlphaReference = static_cast<uint32>(data->GetDataSingle(TAG_2_AlphaRef)*255);
			if (AlphaReference > 255)
				AlphaReference = 255;

			AlphaTestEnabled = !!AlphaReference;


			bool isVegetation = false;
			data->TryGetDataBool(TAG_2_IsVegetation, isVegetation);
			if (isVegetation)
			{
				this->AddCustomParameter(MaterialCustomParameter(isVegetation, L"veg"));
			}


			int32 val = static_cast<int32>(CULL_None);
			data->TryGetDataInt32(TAG_2_CullMode, val);
			Cull = static_cast<CullMode>(val);


			IsBlendTransparent = false;
			data->TryGetDataBool(TAG_2_IsTransparent, IsBlendTransparent);
			
			DepthTestEnabled = true;
			data->TryGetDataBool(TAG_2_ZEnabled, DepthTestEnabled);

			
			DepthWriteEnabled = true;
			data->TryGetDataBool(TAG_2_ZWriteEnabled, DepthWriteEnabled);

			m_priority = 1;
			data->TryGetDataInt32(TAG_2_RenderPriority, m_priority);
			m_priority++;
			


		}
		void Material::LoadV3(TaggedDataReader* data)
		{

		}

		void Material::Load(TaggedDataReader* data)
		{
			int version = 3;
			if (data->Contains(TAG_2_AlphaRef))
			{
				version = 2;
			}

			if (version == 2)
				LoadV2(data);
			else
				LoadV3(data);
			
		}
		TaggedDataWriter* Material::Save()
		{
			return 0;
		}

	}
};