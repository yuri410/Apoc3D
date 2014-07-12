
/**
 * -----------------------------------------------------------------------------
 * This source file is part of Apoc3D Engine
 * 
 * Copyright (c) 2009+ Tao Xin
 * 
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 * 
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 * 
 * You should have received a copy of the GNU General Public License
 * along with this program.  if not, write to the Free Software Foundation, 
 * Inc., 59 Temple Place - Suite 330, Boston, MA 02111-1307, USA, or go to
 * http://www.gnu.org/copyleft/gpl.txt.
 * 
 * -----------------------------------------------------------------------------
 */

#include "IniConfigurationFormat.h"

#include "ConfigurationSection.h"

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
		IniConfigurationFormat IniConfigurationFormat::Instance = IniConfigurationFormat();

		

		Configuration* IniConfigurationFormat::Load(const ResourceLocation& rl)
		{
			Configuration* config = new Configuration(rl.getName());

			BinaryReader* br = new BinaryReader(rl);

			int32 length = (int32)br->getBaseStream()->getLength();
			char* rawBytes = new char[length+1];
			rawBytes[length] = 0;
			br->ReadBytes(rawBytes, length);

			String allText = Encoding::ConvertRawData(rawBytes, length, Encoding::TEC_Unknown, true);

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

			delete[] rawBytes;

			br->Close();
			delete br;
			return config;
		}
		void IniConfigurationFormat::Save(Configuration* config, Stream* strm)
		{
			bool firstSection = true;
			String resultBuffer;
			for (Configuration::ChildTable::Enumerator e1 = config->GetEnumerator(); e1.MoveNext();)
			{
				if (!firstSection)
				{
					resultBuffer.append(L"\n");
				}

				resultBuffer.append(1, '[');
				resultBuffer.append(*e1.getCurrentKey());
				resultBuffer.append(L"]\n");

				firstSection = false;

				ConfigurationSection* sect = *e1.getCurrentValue();
				for (ConfigurationSection::SubSectionEnumerator e2 = sect->GetSubSectionEnumrator();e2.MoveNext();)
				{
					resultBuffer.append(*e2.getCurrentKey());
					resultBuffer.append(L" = ");
					resultBuffer.append((*e2.getCurrentValue())->getValue());
					resultBuffer.append(L"\n");
				}
			}

			std::string utf8 = StringUtils::UTF16toUTF8(resultBuffer);

			int32 bomLen;
			const char* bom = GetEncodingBOM(Encoding::TEC_UTF8,bomLen);

			if (bomLen>0)
				strm->Write((const char*)bom, bomLen);
			strm->Write((const char*)utf8.c_str(), utf8.length());

			strm->Close();
			delete strm;
		}
	}
}