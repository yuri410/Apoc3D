/*
-----------------------------------------------------------------------------
This source file is part of Apoc3D Engine

Copyright (c) 2009+ Tao Xin

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
#include "TextureManager.h"

#include "apoc3d/Core/ResourceHandle.h"
#include "apoc3d/Core/Logging.h"
#include "apoc3d/Utility/StringUtils.h"
#include "apoc3d/IOLib/MaterialData.h"
#include "apoc3d/Vfs/FileSystem.h"
#include "apoc3d/Vfs/FileLocateRule.h"
#include "apoc3d/Vfs/ResourceLocation.h"

using namespace Apoc3D::VFS;
using namespace Apoc3D::Utility;

namespace Apoc3D
{
	namespace Graphics
	{
		Material::Material(RenderDevice* device)
			: m_device(device),
			m_passFlags(0), m_priority(DefaultMaterialPriority), 
			BlendFunction(BLFUN_Add), IsBlendTransparent(false), 
			SourceBlend(BLEND_SourceAlpha), DestinationBlend(BLEND_InverseSourceAlpha),
			AlphaTestEnabled(false), AlphaReference(0),
			DepthWriteEnabled(true), DepthTestEnabled(true),
			Ambient(0,0,0,0), Diffuse(1.f,1.f,1.f,1.f), Specular(0,0,0,0), Emissive(0,0,0,0), Power(0),
			Cull(CULL_None),
			UsePointSprite(false)
		{
			memset(m_tex, 0, sizeof(m_tex));
			memset(m_effects, 0, sizeof(m_effects));
			memset(m_texDirty, 0, sizeof(m_texDirty));
		}

		Material::Material(const Material& m)
			: m_device(m.m_device), 
			m_passFlags(m.m_passFlags), m_priority(m.m_priority),
			BlendFunction(m.BlendFunction), IsBlendTransparent(m.IsBlendTransparent),
			SourceBlend(m.SourceBlend), DestinationBlend(m.DestinationBlend),
			AlphaTestEnabled(m.AlphaTestEnabled), AlphaReference(m.AlphaReference),
			DepthWriteEnabled(m.DepthWriteEnabled), DepthTestEnabled(m.DepthTestEnabled),
			Ambient(m.Ambient), Diffuse(m.Diffuse), Specular(m.Specular), Emissive(m.Emissive), Power(m.Power),
			Cull(m.Cull),
			UsePointSprite(m.UsePointSprite),
			ExternalReferenceName(m.ExternalReferenceName),
			m_customParametrs(m.m_customParametrs), m_effectName(m.m_effectName), m_texName(m.m_texName)
		{
			memcpy(m_texDirty, m.m_texDirty, sizeof(m_texDirty));
			memset(m_tex, 0, sizeof(m_tex));
			memset(m_effects, 0, sizeof(m_effects));

			for (int i=0;i<MaxScenePass;i++)
			{
				LoadEffect(i);
			}
			for (int i=0;i<MaxTextures;i++)
			{
				LoadTexture(i);
			}

#if _DEBUG
			DebugName = m.DebugName;
#endif
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
			return m_customParametrs.TryGetValue(usage);
		}
		void Material::AddCustomParameter(const MaterialCustomParameter& value)
		{
			if (value.Usage.empty())
			{
				throw AP_EXCEPTION(ApocExceptionType::Argument, L"usage can not be empty");
			}
			m_customParametrs.Add(value.Usage, value);
		}

		void Material::LoadEffect(int32 index)
		{
			const String* name = m_effectName.TryGetValue(index);
			if (name && name->size())
			{
				m_effects[index] = EffectManager::getSingleton().getEffect(*name);
			}	
		}


		void Material::LoadTexture(int32 index)
		{
			const String* name = m_texName.TryGetValue(index);
			if (name == nullptr)
			{
				m_tex[index] = nullptr;
				return;
			}
			// load texture
			//FileLocation* fl = FileSystem::getSingleton().TryLocate(*name, FileLocateRule::Textures);
			FileLocation fl;
			if (FileSystem::getSingleton().TryLocate(*name, FileLocateRule::Textures, fl))
			{
				m_tex[index] = TextureManager::getSingleton().CreateInstance(m_device, fl);
			}
			else
			{
				m_tex[index] = nullptr;

#if _DEBUG
				LogManager::getSingleton().Write(LOG_Graphics, L"Missing texture '" + *name + L"' when loading " + DebugName + L".",
					LOGLVL_Error);
#else
				LogManager::getSingleton().Write(LOG_Graphics, L"Missing texture '" + *name + L"'. ",
					LOGLVL_Error);
#endif
			}
			
		}

		void Material::LoadReferencedMaterial(const String& mtrlName)
		{
			// re load using a newly loaded MaterialData
			MaterialData newData;
			//FileLocation* fl = FileSystem::getSingleton().TryLocate(mtrlName, FileLocateRule::Materials);
			FileLocation fl;
			if (FileSystem::getSingleton().TryLocate(mtrlName, FileLocateRule::Materials, fl))
			{
				newData.Load(fl);
				Load(newData);

				ExternalReferenceName = mtrlName;
			}
			else
			{
#if _DEBUG
				ApocLog(LOG_Graphics, L"[Material] External reference material '" + mtrlName + L"' not found when loading " + DebugName + L".", LOGLVL_Error);
#else
				ApocLog(LOG_Graphics, L"[Material] External reference material '" + mtrlName + L"' not found.", LOGLVL_Error);
#endif
			}
		}

		void Material::Load(const MaterialData& mdata)
		{
#if _DEBUG
			DebugName = mdata.DebugName;
#endif
			if (mdata.ExternalRefName.size())
			{
				// re load using a newly loaded MaterialData
				LoadReferencedMaterial(mdata.ExternalRefName);
				return;
			}
			ExternalReferenceName = mdata.ExternalRefName;

			m_customParametrs = mdata.CustomParametrs;
			m_passFlags = mdata.PassFlags;
			m_priority = mdata.Priority;

			SourceBlend = mdata.SourceBlend;
			DestinationBlend = mdata.DestinationBlend;
			BlendFunction = mdata.BlendFunction;
			IsBlendTransparent = mdata.IsBlendTransparent;

			UsePointSprite = mdata.UsePointSprite;

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

			m_effectName = mdata.EffectName;
			m_texName = mdata.TextureName;

			for (HashMap<int, String>::Enumerator e = m_effectName.GetEnumerator(); e.MoveNext();)
			{
				LoadEffect(e.getCurrentKey());
			}
			for (HashMap<int, String>::Enumerator e = m_texName.GetEnumerator(); e.MoveNext();)
			{
				LoadTexture(e.getCurrentKey());
			}
		}
		void Material::Save(MaterialData& data)
		{
			data.ExternalRefName = ExternalReferenceName;
			data.CustomParametrs = m_customParametrs;
			data.PassFlags = m_passFlags;
			data.Priority = m_priority;

			data.SourceBlend = SourceBlend;
			data.DestinationBlend = DestinationBlend;
			data.BlendFunction = BlendFunction;
			data.IsBlendTransparent = IsBlendTransparent;

			data.UsePointSprite = UsePointSprite;

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

			data.EffectName = m_effectName;
			data.TextureName = m_texName;
		}
		void Material::Load(TaggedDataReader* data)
		{
			MaterialData mdata;
			mdata.LoadData(data);

			Load(mdata);
		}
		TaggedDataWriter* Material::Save()
		{
			MaterialData data;
			
			Save(data);

			return data.SaveData();
		}

		void Material::Reload()
		{
			for (int i = 0; i < MaxTextures; i++)
			{
				if (m_texDirty[i])
				{
					delete m_tex[i];
					LoadTexture(i);
					m_texDirty[i] = false;
				}
			}
		}

		Effect* Material::GetFirstValidEffect() const
		{
			for (int i=0;i<MaxScenePass;i++)
			{
				if (m_effects[i])
					return m_effects[i];
			}
			return 0;
		}

		const String& Material::getTextureName(int index) const 
		{
			static String Empty = L"";
			assert(index<MaxTextures);
			if (!m_texName.Contains(index))
				return Empty;
			return m_texName[index];
		}

		void Material::setTextureName(int index, const String& name)
		{
			assert(index<MaxTextures);
			if (!m_texName.Contains(index))
				m_texName.Add(index, name);
			else
			{
				String& tn = m_texName[index];
				if (tn != name)
				{
					tn.operator=(name);
					m_texDirty[index] = true;
				}
			}
		}

		const String& Material::getPassEffectName(int index) 
		{
			static String Empty = L"";
			assert(index<MaxScenePass); 
			if (!m_effectName.Contains(index))
				return Empty;
			return m_effectName[index]; 
		}
		void Material::setPassEffectName(int index, const String& en) 
		{
			assert(index<MaxScenePass); 

			if (!m_effectName.Contains(index))
				m_effectName.Add(index, en);
			else
			{
				m_effectName[index] = en;
			}
			
		}
	}
};