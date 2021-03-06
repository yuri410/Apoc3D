/* -----------------------------------------------------------------------
 * This source file is part of Apoc3D Framework
 * 
 * Copyright (c) 2011-2017 Tao Xin
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
#include "ConfigurationManager.h"
#include "Configuration.h"

#include "apoc3d/Core/Logging.h"
#include "apoc3d/Config/XmlConfigurationFormat.h"
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
			RegisterFormat(new XMLConfigurationFormat());
			RegisterFormat(new IniConfigurationFormat());
		}
		ConfigurationManager::~ConfigurationManager()
		{
			m_configs.DeleteValuesAndClear();
			m_formats.DeleteValuesAndClear();
		}

		void ConfigurationManager::LoadConfig(const String& name, const ResourceLocation& rl, ConfigurationFormat* fmt)
		{
			Configuration* conf = CreateInstance(rl, fmt);

			m_configs.Add(name, conf);
		}

		Configuration* ConfigurationManager::CreateInstance(const ResourceLocation& rl, ConfigurationFormat* fmt)
		{
			if (fmt)
			{
				Configuration* result = new Configuration(rl.getName());
				fmt->Load(rl, result);
				return result;
			}

			const FileLocation* fl = up_cast<const FileLocation*>(&rl);
			if (fl)
			{
				String fn;
				String ext;
				PathUtils::SplitFileNameExtension(fl->getPath(), fn, ext);
				StringUtils::ToLowerCase(ext);

				ConfigurationFormat* fmt;
				if (m_formats.TryGetValue(ext, fmt))
				{
					Configuration* result = new Configuration(fn);
					fmt->Load(rl, result);
					return result;
				}
				else
				{
					AP_EXCEPTION(ErrorID::NotSupported, ext + L" files are not supported");
				}
			}
			else
			{
				AP_EXCEPTION(ErrorID::Argument, L"Either a FileLocation or a ConfigurationFormat is required.");
			}
			return nullptr;
		}
		
		Configuration* ConfigurationManager::getConfiguration(const String& name) const
		{
			Configuration* result = nullptr;
			m_configs.TryGetValue(name, result);
			return result;
		}

		void ConfigurationManager::RegisterFormat(ConfigurationFormat* fmt)
		{
			List<String> exts = fmt->GetSupportedFileSystemExtensions();
			for (const String& ex : exts)
			{
				if (!m_formats.Contains(ex))
					m_formats.Add(ex, fmt);
				else
				{
					LogManager::getSingleton().Write(LOG_System, L"[ConfigurationManager] FileSys extension " + ex + L" cannot be registered because it is already supported.", LOGLVL_Warning);
				}
			}
		}
		void ConfigurationManager::UnregisterFormat(ConfigurationFormat* fmt)
		{
			List<String> exts = fmt->GetSupportedFileSystemExtensions();
			for (const String& ex : exts)
			{
				ConfigurationFormat* cf;
				if (m_formats.TryGetValue(ex, cf) && cf == fmt)
				{
					m_formats.Remove(ex);
				}
			}
		}

	}
}