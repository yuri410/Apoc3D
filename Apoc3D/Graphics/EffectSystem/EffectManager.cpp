/* -----------------------------------------------------------------------
 * This source file is part of Apoc3D Framework
 * 
 * Copyright (c) 2009-2018 Tao Xin
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

#include "EffectManager.h"
#include "Effect.h"

#include "apoc3d/Core/Logging.h"
#include "apoc3d/Config/XmlConfigurationFormat.h"
#include "apoc3d/Config/ConfigurationSection.h"
#include "apoc3d/Vfs/FileSystem.h"
#include "apoc3d/Vfs/FileLocateRule.h"
#include "apoc3d/Vfs/ResourceLocation.h"

namespace Apoc3D
{
	namespace Graphics
	{
		namespace EffectSystem
		{
			SINGLETON_IMPL(EffectManager);

			EffectManager::~EffectManager() 
			{
				UnloadAllEffects();
			}

			bool EffectManager::HasEffect(const String& name) const
			{
				return m_fxTable.Contains(name);
			}
			Effect* EffectManager::getEffect(const String& name) const
			{
				Effect* fx;
				if (m_fxTable.TryGetValue(name, fx))
				{
					return fx;
				}

				LogManager::getSingleton().Write(LOG_Graphics, 
					L"EffectManager: Attempted to get effect '" + name + L"' which does not exist.", LOGLVL_Warning);
				return nullptr;
			}

			void EffectManager::LoadEffectsFromList(RenderDevice* device, const ResourceLocation& rl)
			{
				Configuration config(rl.getName());
				XMLConfigurationFormat::Instance.Load(rl, &config);

				for (ConfigurationSection* lstEntry : config.getSubSections())
				{
					FileLocation fl;
					if (!FileSystem::getSingleton().TryLocate(lstEntry->getName() + L".afx", FileLocateRule::Effects, fl))
					{
						LogManager::getSingleton().Write(LOG_Graphics, L"Effect " + lstEntry->getName() + L" is not found.", LOGLVL_Error);
						continue;
					}
					LoadAutomaticEffect(device, fl);
				}
			}
			void EffectManager::LoadAutomaticEffect(RenderDevice* device, const ResourceLocation& rl)
			{
				AutomaticEffect* effect = new AutomaticEffect(device, rl);

				const String& name = effect->getName();

				if (!HasEffect(name))
				{
					m_fxTable.Add(name, effect);

					m_autoEffectInfo.Add(AutomaticEffectInfo(name, &rl));
				}
				else
				{
					Effect* eff = getEffect(name);

					AutomaticEffect* ae = up_cast<AutomaticEffect*>(eff);
					if (ae)
					{
						delete m_fxTable[name];
						m_fxTable[name] = effect;
						LogManager::getSingleton().Write(LOG_Graphics, 
							L"EffectManager: An automatic effect with the same name '" + name + L"' is already loaded. Full reloading...",
							LOGLVL_Infomation);
					}
					else
					{
						LogManager::getSingleton().Write(LOG_Graphics, 
							L"EffectManager: A custom effect with the same name '" + name + L"' is already loaded. Cannot load effect.", LOGLVL_Error);
						delete effect;
					}
				}
			}

			void EffectManager::RegisterCustomEffect(CustomShaderEffect* effect)
			{
				if (!HasEffect(effect->getName()))
				{
					m_fxTable.Add(effect->getName(), effect);
				}
				else
				{
					LogManager::getSingleton().Write(LOG_Graphics, 
						L"EffectManager: A custom effect with the same name '" + effect->getName() + L"' is already loaded. Cannot register.", LOGLVL_Error);
				}
			}

			void EffectManager::ReloadAutomaticEffects()
			{
				for (auto& e : m_autoEffectInfo)
				{
					if (HasEffect(e.Name))
					{
						Effect* eff = getEffect(e.Name);
						AutomaticEffect* ae = up_cast<AutomaticEffect*>(eff);

						ae->Reload(*e.Location);
					}
				}
			}
			void EffectManager::UnloadAllEffects()
			{
				m_fxTable.DeleteValuesAndClear();
			}

			void EffectManager::Update(const AppTime* time)
			{
				for (Effect* fx : m_fxTable.getValueAccessor())
				{
					fx->Update(time);
				}
			}

			void EffectManager::FillEffects(List<Effect*>& list)
			{
				for (Effect* fx : m_fxTable.getValueAccessor())
				{
					list.Add(fx);
				}
			}
			//////////////////////////////////////////////////////////////////////////

			EffectManager::AutomaticEffectInfo::AutomaticEffectInfo(const String& name, const ResourceLocation* loc)
				: Name(name)
			{
				Location = loc->Clone();
			}
			EffectManager::AutomaticEffectInfo::~AutomaticEffectInfo()
			{
				DELETE_AND_NULL(Location);
			}

			EffectManager::AutomaticEffectInfo::AutomaticEffectInfo(AutomaticEffectInfo&& rhs)
				: Name(std::move(rhs.Name)), Location(rhs.Location)
			{
				rhs.Location = nullptr;
			}

			EffectManager::AutomaticEffectInfo& EffectManager::AutomaticEffectInfo::operator=(AutomaticEffectInfo&& rhs)
			{
				if (this != &rhs)
				{
					Name = std::move(rhs.Name);
					Location = rhs.Location;

					rhs.Location = nullptr;
				}
				return *this;
			}
		}
	}
}