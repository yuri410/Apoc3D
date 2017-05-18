
/* -----------------------------------------------------------------------
 * This source file is part of Apoc3D Framework
 * 
 * Copyright (c) 2014-2017 Tao Xin
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
#include "IniConfigurationFormat.h"

#include "ConfigurationSection.h"
#include "ConfigurationManager.h"

#include "apoc3d/Core/Logging.h"
#include "apoc3d/IOLib/Streams.h"
#include "apoc3d/IOLib/BinaryReader.h"
#include "apoc3d/IOLib/BinaryWriter.h"
#include "apoc3d/IOLib/TaggedData.h"
#include "apoc3d/Utility/StringUtils.h"
#include "apoc3d/VFS/PathUtils.h"
#include "apoc3d/Vfs/ResourceLocation.h"
#include "apoc3d/IOLib/TextData.h"

#include "apoc3d/Library/ConvertUTF.h"

using namespace Apoc3D::Utility;

namespace Apoc3D
{
	namespace Config
	{
		IniConfigurationFormat IniConfigurationFormat::Instance;

		void IniConfigurationFormat::Load(const ResourceLocation& rl, Configuration* config)
		{
			String allText = Encoding::ReadAllText(rl, Encoding::TEC_Unknown);

			if (allText.size())
			{
				ConfigurationSection* curSect = nullptr;
				String curSectName;

				String::size_type pos = 0;
				while (pos != String::npos)
				{
					String::size_type lastPos = pos;
					pos = allText.find('\n', pos+1);

					String line;

					if (pos == String::npos)
					{
						line = allText.substr(lastPos);
					}
					else if (pos > lastPos)
					{
						line = allText.substr(lastPos, pos-lastPos);
					}

					String::size_type cpos = line.find(';');
					if (cpos != String::npos)
						line = line.substr(0, cpos);

					StringUtils::Trim(line, L"\n\r\t ");

					if (line.size()>1)
					{
						if (line[0] == '[' && line[line.size()-1]==']')
						{
							curSectName = line.substr(1, line.size()-2);
							StringUtils::Trim(curSectName);

							curSect = config->get(curSectName);
							if (curSect == nullptr)
							{
								curSect = new ConfigurationSection(curSectName);
								config->Add(curSect);
							}
							else
							{
								ApocLog(LOG_System, L"[INI] Section '" + curSectName + L"' already exists, merging.", LOGLVL_Warning);
							}
						}
						else if (curSect)
						{
							cpos = line.find('=');
							if (cpos != String::npos)
							{
								String key = line.substr(0, cpos);
								String value = line.substr(cpos+1);

								StringUtils::Trim(key);
								StringUtils::Trim(value);

								ConfigurationSection* valSect = curSect->getSection(key);

								if (valSect)
								{
									valSect->SetValue(value);

									ApocLog(LOG_System, L"[INI] Key '" + key + L"' already exists in section '" + curSect->getName() + L"', overwriting.", LOGLVL_Warning);
								}
								else
									curSect->AddStringValue(key, value);
							}
							else
							{
								curSect->AddStringValue(line, L"");
							}
						}
					}

				}
			}
		}
		void IniConfigurationFormat::Save(Configuration* config, Stream& strm)
		{
			bool firstSection = true;
			String resultBuffer;
			for (ConfigurationSection* sect : config->getSubSections())
			{
				if (!firstSection)
				{
					resultBuffer.append(L"\n");
				}

				resultBuffer.append(1, '[');
				resultBuffer.append(sect->getName());
				resultBuffer.append(L"]\n");

				firstSection = false;

				for (ConfigurationSection* ss : sect->getSubSections())
				{
					resultBuffer.append(ss->getName());
					resultBuffer.append(L" = ");
					resultBuffer.append(ss->getValue());
					resultBuffer.append(L"\n");
				}
			}

			Encoding::WriteAllText(strm, resultBuffer, Encoding::TEC_UTF8);
		}

		List<String> IniConfigurationFormat::GetSupportedFileSystemExtensions()
		{
			return{ L"ini" };
		}
	}
}