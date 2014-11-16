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

#include "ConfigurationManager.h"
#include "Configuration.h"

#include "apoc3d/Core/Logging.h"
#include "apoc3d/Config/XmlConfigurationFormat.h"
#include "apoc3d/Config/ABCConfigurationFormat.h"
#include "apoc3d/Config/IniConfigurationFormat.h"
#include "apoc3d/Config/ConfigurationSection.h"
#include "apoc3d/Vfs/ResourceLocation.h"
#include "apoc3d/Vfs/PathUtils.h"
#include "apoc3d/Utility/StringUtils.h"

using namespace Apoc3D::Utility;

namespace Apoc3D
{
	namespace Config
	{
		SINGLETON_IMPL(ConfigurationManager);

		ConfigurationManager::ConfigurationManager()
		{
			RegisterFormat(new ABCConfigurationFormat());
			RegisterFormat(new XMLConfigurationFormat());
			RegisterFormat(new IniConfigurationFormat());

			for (int32 i = 0; i < 50; i++)
			{
				m_sectionPool.Enqueue(new ConfigurationSection(L""));
			}
		}
		ConfigurationManager::~ConfigurationManager()
		{
			m_configs.DeleteValuesAndClear();
			m_formats.DeleteValuesAndClear();
		}

		void ConfigurationManager::LoadConfig(const String& name, const ResourceLocation& rl, ConfigurationFormat* fmt)
		{
			Configuration* conf = CreateInstance(rl, fmt);
			//XMLConfiguration* conf = new XMLConfiguration(rl);
			m_configs.Add(name, conf);
		}

		Configuration* ConfigurationManager::CreateInstance(const ResourceLocation& rl, ConfigurationFormat* fmt)
		{
			if (fmt)
			{
				return fmt->Load(rl);
			}

			const FileLocation* fl = up_cast<const FileLocation*>(&rl);
			if (fl)
			{
				String temp;
				String ext;
				PathUtils::SplitFileNameExtension(fl->getPath(), temp, ext);
				StringUtils::ToLowerCase(ext);

				if (m_formats.TryGetValue(ext, fmt))
				{
					return fmt->Load(rl);
				}
				throw AP_EXCEPTION(ExceptID::NotSupported, ext + L" files are not supported");
			}
			throw AP_EXCEPTION(ExceptID::Argument, L"Either a FileLocation or a ConfigurationFormat is required.");
		}


		void ConfigurationManager::RegisterFormat(ConfigurationFormat* fmt)
		{
			List<String> exts = fmt->GetSupportedFileSystemExtensions();
			for (int32 i=0;i<exts.getCount();i++)
			{
				if (!m_formats.Contains(exts[i]))
					m_formats.Add(exts[i], fmt);
				else
				{
					LogManager::getSingleton().Write(LOG_System, L"[ConfigurationManager] FileSys extension " + exts[i] + L" cannot be registered because it is already supported.", LOGLVL_Warning);
				}
			}
		}
		void ConfigurationManager::UnregisterFormat(ConfigurationFormat* fmt)
		{
			List<String> exts = fmt->GetSupportedFileSystemExtensions();
			for (int32 i=0;i<exts.getCount();i++)
			{
				ConfigurationFormat* cf;
				if (m_formats.TryGetValue(exts[i], cf) && cf == fmt)
				{
					m_formats.Remove(exts[i]);
				}
			}
		}

		ConfigurationSection* ConfigurationManager::NewConfigSection(const String& name)
		{
			if (!ConfigurationManager::isInitialized())
				return new ConfigurationSection(name);
			return ConfigurationManager::getSingleton().CreateSection(name, 0);
		}
		ConfigurationSection* ConfigurationManager::NewConfigSection(const String& name, int32 capacity)
		{
			if (!ConfigurationManager::isInitialized())
				return new ConfigurationSection(name);
			return ConfigurationManager::getSingleton().CreateSection(name, capacity);
		}
		ConfigurationSection* ConfigurationManager::NewConfigSection(const ConfigurationSection& other)
		{
			if (!ConfigurationManager::isInitialized())
				return new ConfigurationSection(other);
			return ConfigurationManager::getSingleton().CreateSection(other);
		}

		void ConfigurationManager::FreeConfigSection(ConfigurationSection* s)
		{
			if (!ConfigurationManager::isInitialized())
				delete s;
			else
				ConfigurationManager::getSingleton().RecycleSection(s);
		}


		ConfigurationSection* ConfigurationManager::CreateSection(const String& name, int32 capacity)
		{
			ConfigurationSection* result = nullptr;
			m_sectPoolLock.lock();

			if (m_sectionPool.getCount() > 0)
			{
				result = m_sectionPool.Dequeue();
				result->_SetName(name);
				result->_EnsureCapacity(capacity);
			}

			m_sectPoolLock.unlock();

			if (result == nullptr)
			{
				if (capacity != 0)
					result = new ConfigurationSection(name, capacity);
				else
					result = new ConfigurationSection(name);
			}

			return result;
		}
		ConfigurationSection* ConfigurationManager::CreateSection(const ConfigurationSection& other)
		{
			ConfigurationSection* result = nullptr;
			m_sectPoolLock.lock();

			if (m_sectionPool.getCount() > 0)
			{
				result = m_sectionPool.Dequeue();
				result->_CopyFrom(other);
			}

			m_sectPoolLock.unlock();

			if (result == nullptr)
			{
				result = new ConfigurationSection(other);
			}

			return result;
		}



		void ConfigurationManager::RecycleSection(ConfigurationSection* sect)
		{
			for (ConfigurationSection* s : sect->m_subSection.getValueAccessor())
			{
				RecycleSection(s);
			}

			sect->_ShallowClear();

			m_sectPoolLock.lock();
			m_sectionPool.Enqueue(sect);
			m_sectPoolLock.unlock();
		}
	}
}