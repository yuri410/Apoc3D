/* -----------------------------------------------------------------------
 * This source file is part of Apoc3D Framework
 * 
 * Copyright (c) 2010-2017 Tao Xin
 * 
 * This content of this file is subject to the terms of the Mozilla Public 
 * License v2.0. If a copy of the MPL was not distributed with this file, 
 * you can obtain one at http://mozilla.org/MPL/2.0/.
 * 
 * This program is distributed in the hope that it will be useful, 
 * WITHOUT WARRANTY OF ANY KIND; either express or implied. See the 
 * Mozilla Public License for more details.
 * 
 * ------------------------------------------------------------------------
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
			: m_device(device)
		{
			ZeroArray(m_tex);
			ZeroArray(m_effects);
			ZeroArray(m_texDirty);
		}

		Material::Material(const Material& m)
			: m_device(m.m_device), 
			m_passFlags(m.m_passFlags), m_priority(m.m_priority), m_colorWriteMasks(m.m_colorWriteMasks),
			BlendFunction(m.BlendFunction), IsBlendTransparent(m.IsBlendTransparent),
			SourceBlend(m.SourceBlend), DestinationBlend(m.DestinationBlend),
			AlphaTestEnabled(m.AlphaTestEnabled), AlphaReference(m.AlphaReference),
			DepthWriteEnabled(m.DepthWriteEnabled), DepthTestEnabled(m.DepthTestEnabled),
			UsePointSprite(m.UsePointSprite),
			Ambient(m.Ambient), Diffuse(m.Diffuse), Specular(m.Specular), Emissive(m.Emissive), Power(m.Power),
			Cull(m.Cull),
			ExternalReferenceName(m.ExternalReferenceName),
			m_customParametrs(m.m_customParametrs), m_effectName(m.m_effectName), m_texName(m.m_texName)
		{
			CopyArray(m_texDirty, m.m_texDirty);
			ZeroArray(m_tex);
			ZeroArray(m_effects);

			for (int i = 0; i < MaxScenePass; i++)
			{
				LoadEffect(i);
			}
			for (int i = 0; i < MaxTextures; i++)
			{
				LoadTexture(i);
			}

#if _DEBUG
			DebugName = m.DebugName;
#endif
		}

		Material::~Material()
		{
			for (ResourceHandle<Texture>*& tex : m_tex)
			{
				DELETE_AND_NULL(tex);
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
				throw AP_EXCEPTION(ExceptID::Argument, L"usage can not be empty");
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
			m_colorWriteMasks = mdata.ColorWriteMasks;
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

			m_effectName = mdata.EffectNames;
			m_texName = mdata.TextureNames;

			for (int idx : m_effectName.getKeyAccessor())
			{
				LoadEffect(idx);
			}
			for (int idx : m_texName.getKeyAccessor())
			{
				LoadTexture(idx);
			}
		}
		void Material::Save(MaterialData& data)
		{
			data.ExternalRefName = ExternalReferenceName;
			data.CustomParametrs = m_customParametrs;
			data.PassFlags = m_passFlags;
			data.ColorWriteMasks = m_colorWriteMasks;
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

			data.EffectNames = m_effectName;
			data.TextureNames = m_texName;
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
			for (int32 i = 0; i < MaxTextures; i++)
			{
				if (m_texDirty[i])
				{
					m_texDirty[i] = false;

					DELETE_AND_NULL(m_tex[i]);
					LoadTexture(i);
				}
			}
		}

		Effect* Material::GetFirstValidEffect() const
		{
			for (Effect* fx : m_effects)
			{
				if (fx)
					return fx;
			}
			return nullptr;
		}
		Effect* Material::GetPassEffect(int32 index) const
		{
			if (index == -1) 
				return GetFirstValidEffect(); 
			return m_effects[index]; 
		}

		const String& Material::GetTextureName(int32 index) const
		{
			assert(index >= 0 && index < MaxTextures);
			if (!m_texName.Contains(index))
				return StringUtils::Empty;
			return m_texName[index];
		}

		void Material::SetTextureName(int32 index, const String& name)
		{
			assert(index >= 0 && index < MaxTextures);

			if (!m_texName.Contains(index))
			{
				if (name.size())
				{
					m_texName.Add(index, name);
					m_texDirty[index] = true;
				}
			}
			else
			{
				if (name.size())
				{
					String& tn = m_texName[index];
					if (tn != name)
					{
						tn = name;
						m_texDirty[index] = true;
					}
				}
				else
				{
					m_texName.Remove(index);
					m_texDirty[index] = true;
				}
			}
		}

		const String& Material::GetPassEffectName(int32 index)
		{
			assert(index<MaxScenePass); 
			if (!m_effectName.Contains(index))
				return StringUtils::Empty;
			return m_effectName[index]; 
		}
		void Material::SetPassEffectName(int32 index, const String& en)
		{
			assert(index<MaxScenePass); 

			if (!m_effectName.Contains(index))
				m_effectName.Add(index, en);
			else
			{
				m_effectName[index] = en;
			}
		}

		ColorWriteMasks Material::GetTargetWriteMask(uint32 rtIndex) const
		{
			return (ColorWriteMasks)MaterialData::GetTargetWriteMaskBits(m_colorWriteMasks, rtIndex);
		}
		void Material::SetTargetWriteMask(uint32 rtIndex, ColorWriteMasks masks)
		{
			MaterialData::SetTargetWriteMaskBits(m_colorWriteMasks, rtIndex, masks);
		}
	}
};