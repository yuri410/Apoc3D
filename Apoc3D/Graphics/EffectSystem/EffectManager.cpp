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

#include "EffectManager.h"
#include "Effect.h"

#include "apoc3d/Core/Logging.h"
#include "apoc3d/Config/XmlConfigurationFormat.h"
#include "apoc3d/Config/ConfigurationSection.h"
#include "apoc3d/Vfs/FileSystem.h"
#include "apoc3d/Vfs/FileLocateRule.h"
#include "apoc3d/Vfs/ResourceLocation.h"

SINGLETON_DECL(Apoc3D::Graphics::EffectSystem::EffectManager);

namespace Apoc3D
{
	namespace Graphics
	{
		namespace EffectSystem
		{
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

			void EffectManager::LoadEffectFromList(RenderDevice* device, const ResourceLocation& rl)
			{
				Configuration* config = XMLConfigurationFormat::Instance.Load(rl);
				for (Configuration::ChildTable::Enumerator iter = config->GetEnumerator();iter.MoveNext();)
				{
					ConfigurationSection* lstEntry = *iter.getCurrentValue();

					FileLocation fl;
					if (!FileSystem::getSingleton().TryLocate(lstEntry->getName() + L".afx", FileLocateRule::Effects, fl))
					{
						LogManager::getSingleton().Write(LOG_Graphics, L"Effect " + lstEntry->getName() + L" is not found.", LOGLVL_Error);
						continue;
					}
					LoadEffect(device, fl);
				}
			}
			void EffectManager::LoadEffect(RenderDevice* device, const ResourceLocation& rl)
			{
				AutomaticEffect* effect = new AutomaticEffect(device, rl);

				if (!HasEffect(effect->getName()))
				{
					m_fxTable.Add(effect->getName(), effect);
				}
				else
				{
					Effect* eff = getEffect(effect->getName());

					AutomaticEffect* ae = dynamic_cast<AutomaticEffect*>(eff);
					if (ae)
					{
						delete m_fxTable[effect->getName()];
						m_fxTable[effect->getName()] = effect;
						LogManager::getSingleton().Write(LOG_Graphics, 
							L"EffectManager: An automatic effect with the same name '" + effect->getName() + L"' is already loaded. Reloading...",
							LOGLVL_Infomation);
					}
					else
					{
						LogManager::getSingleton().Write(LOG_Graphics, 
							L"EffectManager: A custom effect with the same name '" + effect->getName() + L"' is already loaded. Cannot load effect.", LOGLVL_Error);
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
					//Effect* eff = getEffect(effect->getName());

					LogManager::getSingleton().Write(LOG_Graphics, 
						L"EffectManager: A custom effect with the same name '" + effect->getName() + L"' is already loaded. Cannot load effect.", LOGLVL_Error);
					//delete effect;
				}
			}

			void EffectManager::Update(const GameTime* const time)
			{
				for (EffectTable::Enumerator e = m_fxTable.GetEnumerator(); e.MoveNext();)
				{
					Effect* fx = *e.getCurrentValue();
					fx->Update(time);
				}
			}
		}
	}
}