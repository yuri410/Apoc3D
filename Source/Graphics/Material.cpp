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
#include "EffectSystem/EffectManager.h"
#include "Core/ResourceHandle.h"
#include "Core/Logging.h"
#include "Utility/StringUtils.h"
#include "IO/MaterialData.h"
#include "Vfs/FileSystem.h"
#include "Vfs/FileLocateRule.h"
#include "TextureManager.h"

using namespace Apoc3D::VFS;
using namespace Apoc3D::Utility;

namespace Apoc3D
{
	namespace Graphics
	{
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
		const MaterialCustomParameter* Material::getCustomParameter(const String& usage) const
		{
			CustomParamTable::const_iterator iter =  m_customParametrs.find(usage);
			if (iter != m_customParametrs.end())
			{
				return &iter->second;
			}
			return 0;
		}
		void Material::AddCustomParameter(const MaterialCustomParameter& value)
		{
			if (value.Usage.empty())
			{
				throw Apoc3DException::createException(EX_Argument, L"usage can not be empty");
			}
			m_customParametrs.insert(CustomParamTable::value_type(value.Usage, value));
		}

		void Material::LoadEffect(int32 index)
		{
			m_effects[index] = EffectManager::getSingleton().getEffect(m_effectName[index]);
		}


		void Material::LoadTexture(int32 index)
		{
			// load texture
			FileLocation* fl = FileSystem::getSingleton().TryLocate(m_texName[index], FileLocateRule::Textures);

			if (fl)
			{
				m_tex[index] = TextureManager::getSingleton().CreateInstance(m_device, fl, false);
			}
			else
			{
				LogManager::getSingleton().Write(LOG_Graphics, L"Missing texture '" + m_texName[index] + L"'. ",
					LOGLVL_Error);
			}
			
		}

		void Material::Load(const MaterialData& mdata)
		{
			m_customParametrs = mdata.CustomParametrs;
			m_passFlags = mdata.PassFlags;
			m_priority = mdata.Priority;

			SourceBlend = mdata.SourceBlend;
			DestinationBlend = mdata.DestinationBlend;
			BlendFunction = mdata.BlendFunction;
			IsBlendTransparent = mdata.IsBlendTransparent;

			Cull = mdata.Cull;
			AlphaTestEnabled = mdata.AlphaTestEnabled;
			AlphaReference = mdata.AlphaReference;
			DepthWriteEnabled = mdata.DepthWriteEnabled;
			DepthTestEnabled = mdata.DepthTestEnabled;
			Ambient = mdata.Ambient;
			Diffuse = mdata.Diffuse;
			Emissive = mdata.Emissive;
			Specular = mdata.Specular;
			Power = mdata.Power;

			for (int i=0;i<MaxScenePass;i++)
			{
				m_effectName[i] = mdata.EffectName[i];
				LoadEffect(i);
			}
			for (int i=0;i<MaxScenePass;i++)
			{
				m_texName[i] = mdata.TextureName[i];
				LoadTexture(i);
			}
		}
		void Material::Save(MaterialData& data)
		{
			data.CustomParametrs = m_customParametrs;
			data.PassFlags = m_passFlags;
			data.Priority = m_priority;

			data.SourceBlend = SourceBlend;
			data.DestinationBlend = DestinationBlend;
			data.BlendFunction = BlendFunction;
			data.IsBlendTransparent = IsBlendTransparent;

			data.Cull = Cull;
			data.AlphaTestEnabled = AlphaTestEnabled;
			data.AlphaReference = AlphaReference;
			data.DepthWriteEnabled = DepthWriteEnabled;
			data.DepthTestEnabled = DepthTestEnabled;
			data.Ambient = Ambient;
			data.Diffuse = Diffuse;
			data.Emissive = Emissive;
			data.Specular = Specular;
			data.Power = Power;


			for (int i=0;i<MaxScenePass;i++)
			{
				data.EffectName[i] = m_effectName[i];
			}
			for (int i=0;i<MaxScenePass;i++)
			{
				data.TextureName[i] = m_texName[i];
			}

		}
		void Material::Load(TaggedDataReader* data)
		{
			MaterialData mdata;
			mdata.Load(data);

			m_customParametrs = mdata.CustomParametrs;
			m_passFlags = mdata.PassFlags;
			m_priority = mdata.Priority;

			SourceBlend = mdata.SourceBlend;
			DestinationBlend = mdata.DestinationBlend;
			BlendFunction = mdata.BlendFunction;
			IsBlendTransparent = mdata.IsBlendTransparent;

			Cull = mdata.Cull;
			AlphaTestEnabled = mdata.AlphaTestEnabled;
			AlphaReference = mdata.AlphaReference;
			DepthWriteEnabled = mdata.DepthWriteEnabled;
			DepthTestEnabled = mdata.DepthTestEnabled;
			Ambient = mdata.Ambient;
			Diffuse = mdata.Diffuse;
			Emissive = mdata.Emissive;
			Specular = mdata.Specular;
			Power = mdata.Power;

			for (int i=0;i<MaxScenePass;i++)
			{
				m_effectName[i] = mdata.EffectName[i];
				LoadEffect(i);
			}
			for (int i=0;i<MaxScenePass;i++)
			{
				m_texName[i] = mdata.TextureName[i];
				LoadTexture(i);
			}
		}
		TaggedDataWriter* Material::Save()
		{
			MaterialData data;

			data.CustomParametrs = m_customParametrs;
			data.PassFlags = m_passFlags;
			data.Priority = m_priority;

			data.SourceBlend = SourceBlend;
			data.DestinationBlend = DestinationBlend;
			data.BlendFunction = BlendFunction;
			data.IsBlendTransparent = IsBlendTransparent;

			data.Cull = Cull;
			data.AlphaTestEnabled = AlphaTestEnabled;
			data.AlphaReference = AlphaReference;
			data.DepthWriteEnabled = DepthWriteEnabled;
			data.DepthTestEnabled = DepthTestEnabled;
			data.Ambient = Ambient;
			data.Diffuse = Diffuse;
			data.Emissive = Emissive;
			data.Specular = Specular;
			data.Power = Power;


			for (int i=0;i<MaxScenePass;i++)
			{
				data.EffectName[i] = m_effectName[i];
			}
			for (int i=0;i<MaxScenePass;i++)
			{
				data.TextureName[i] = m_texName[i];
			}

			return data.Save();
		}
	}
};