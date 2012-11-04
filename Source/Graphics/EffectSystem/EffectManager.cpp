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

#include "EffectManager.h"
#include "Effect.h"
#include "Core/Logging.h"
#include "Config/XmlConfiguration.h"
#include "Config/ConfigurationSection.h"
#include "Vfs/FileSystem.h"
#include "Vfs/FileLocateRule.h"
#include "Vfs/ResourceLocation.h"

SINGLETON_DECL(Apoc3D::Graphics::EffectSystem::EffectManager);

namespace Apoc3D
{
	namespace Graphics
	{
		namespace EffectSystem
		{
			bool EffectManager::HasEffect(const String& name) const
			{
				EffectTable::const_iterator iter = m_fxTable.find(name);
				return (iter != m_fxTable.end());
			}
			Effect* EffectManager::getEffect(const String& name) const
			{
				EffectTable::const_iterator iter = m_fxTable.find(name);
				if (iter != m_fxTable.end())
				{
					return iter->second;
				}
				LogManager::getSingleton().Write(LOG_Graphics, 
					L"EffectManager: Attempted to get effect '" + name + L"' which does not exist.", LOGLVL_Warning);
				return 0;
			}

			void EffectManager::LoadEffectFromList(RenderDevice* device, const ResourceLocation* rl)
			{
				XMLConfiguration* config = new XMLConfiguration(rl);
				for (XMLConfiguration::ChildTable::Enumerator iter = config->GetEnumerator();iter.MoveNext();)
				{
					ConfigurationSection* lstEntry = *iter.getCurrentValue();

					FileLocation* fl = FileSystem::getSingleton().TryLocate(lstEntry->getName() + L".afx", FileLocateRule::Effects);
					if (!fl)
					{
						LogManager::getSingleton().Write(LOG_Graphics, L"Effect " + lstEntry->getName() + L" is not found.", LOGLVL_Error);
						continue;
					}
					LoadEffect(device, fl);
					delete fl;
				}
			}
			void EffectManager::LoadEffect(RenderDevice* device, const ResourceLocation* rl)
			{
				AutomaticEffect* effect = new AutomaticEffect(device, rl);

				if (!HasEffect(effect->getName()))
				{
					m_fxTable.insert(std::make_pair(effect->getName(), effect));
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
					m_fxTable.insert(std::make_pair(effect->getName(), effect));
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
				for (EffectTable::iterator iter = m_fxTable.begin(); iter != m_fxTable.end(); iter++)
				{
					Effect* e = iter->second;
					e->Update(time);
				}
			}
		}
	}
}