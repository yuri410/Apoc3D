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

#include "ShaderAtomType.h"
#include "ShaderDataIO.h"


namespace APDesigner
{
	/************************************************************************/
	/*  ShaderAtomPort                                                      */
	/************************************************************************/

	// 0 NA
	// 1 direct copy
	// 2 expand copy
	// 3 narrow copy
	// 4 
	static const byte CompatibleTable[ATOMDATA_Count][ATOMDATA_Count]=
	{
		// Int1 2 3 4  2x1 2x2 2x3 2x4  3x1 3x2 3x3 3x4  4x1 4x2 4x3 4x4    Float1 2 3 4  2x1 2x2 2x3 2x4  3x1 3x2 3x3 3x4  4x1 4x2 4x3 4x4   Other
/*Int1  */{   1,2,2,2,   2,  2,  2,  2,   2,  2,  2,  2,   2,  2,  2,  2,        1,2,2,2,   2,  2,  2,  2,   2,  2,  2,  2,   2,  2,  2,  2,      0},
/*Int2  */{   3,1,2,2,   2,  2,  2,  2,   2,  2,  2,  2,   2,  2,  2,  2,        3,1,2,2,   2,  2,  2,  2,   2,  2,  2,  2,   2,  2,  2,  2,      0},
/*Int3  */{   3,3,1,2,   2,  2,  2,  2,   2,  2,  2,  2,   2,  2,  2,  2,        3,3,1,2,   2,  2,  2,  2,   2,  2,  2,  2,   2,  2,  2,  2,      0},
/*Int4  */{   3,3,3,1,   2,  2,  2,  2,   2,  2,  2,  2,   2,  2,  2,  2,        3,3,3,1,   2,  2,  2,  2,   2,  2,  2,  2,   2,  2,  2,  2,      0},
/*Int2x1*/{   0,0,0,0,   1,  0,  0,  0,   0,  0,  0,  0,   0,  0,  0,  0,        0,0,0,0,   1,  0,  0,  0,   0,  0,  0,  0,   0,  0,  0,  0,      0},
/*Int2x2*/{   0,0,0,0,   0,  1,  0,  0,   0,  0,  0,  0,   0,  0,  0,  0,        0,0,0,0,   0,  1,  0,  0,   0,  0,  0,  0,   0,  0,  0,  0,      0},
/*Int2x3*/{   0,0,0,0,   0,  0,  1,  0,   0,  0,  0,  0,   0,  0,  0,  0,        0,0,0,0,   0,  0,  1,  0,   0,  0,  0,  0,   0,  0,  0,  0,      0},
/*Int2x4*/{   0,0,0,0,   0,  0,  0,  1,   0,  0,  0,  0,   0,  0,  0,  0,        0,0,0,0,   0,  0,  0,  1,   0,  0,  0,  0,   0,  0,  0,  0,      0},
/*Int3x1*/{   0,0,0,0,   0,  0,  0,  0,   1,  0,  0,  0,   0,  0,  0,  0,        0,0,0,0,   0,  0,  0,  0,   1,  0,  0,  0,   0,  0,  0,  0,      0},
/*Int3x2*/{   0,0,0,0,   0,  0,  0,  0,   0,  1,  0,  0,   0,  0,  0,  0,        0,0,0,0,   0,  0,  0,  0,   0,  1,  0,  0,   0,  0,  0,  0,      0},
/*Int3x3*/{   0,0,0,0,   0,  0,  0,  0,   0,  0,  1,  0,   0,  0,  0,  0,        0,0,0,0,   0,  0,  0,  0,   0,  0,  1,  0,   0,  0,  0,  0,      0},
/*Int3x4*/{   0,0,0,0,   0,  0,  0,  0,   0,  0,  0,  1,   0,  0,  0,  0,        0,0,0,0,   0,  0,  0,  0,   0,  0,  0,  1,   0,  0,  0,  0,      0},
/*Int4x1*/{   0,0,0,0,   0,  0,  0,  0,   0,  0,  0,  0,   1,  0,  0,  0,        0,0,0,0,   0,  0,  0,  0,   0,  0,  0,  0,   1,  0,  0,  0,      0},
/*Int4x2*/{   0,0,0,0,   0,  0,  0,  0,   0,  0,  0,  0,   0,  1,  0,  0,        0,0,0,0,   0,  0,  0,  0,   0,  0,  0,  0,   0,  1,  0,  0,      0},
/*Int4x3*/{   0,0,0,0,   0,  0,  0,  0,   0,  0,  0,  0,   0,  0,  1,  0,        0,0,0,0,   0,  0,  0,  0,   0,  0,  0,  0,   0,  0,  1,  0,      0},
/*Int4x4*/{   0,0,0,0,   0,  0,  0,  0,   0,  0,  0,  0,   0,  0,  0,  1,        0,0,0,0,   0,  0,  0,  0,   0,  0,  0,  0,   0,  0,  0,  1,      0},
/*FLT1  */{   1,2,2,2,   2,  2,  2,  2,   2,  2,  2,  2,   2,  2,  2,  2,        1,2,2,2,   2,  2,  2,  2,   2,  2,  2,  2,   2,  2,  2,  2,      0},
/*FLT2  */{   3,1,2,2,   2,  2,  2,  2,   2,  2,  2,  2,   2,  2,  2,  2,        3,1,2,2,   2,  2,  2,  2,   2,  2,  2,  2,   2,  2,  2,  2,      0},
/*FLT3  */{   3,3,1,2,   2,  2,  2,  2,   2,  2,  2,  2,   2,  2,  2,  2,        3,3,1,2,   2,  2,  2,  2,   2,  2,  2,  2,   2,  2,  2,  2,      0},
/*FLT4  */{   3,3,3,1,   2,  2,  2,  2,   2,  2,  2,  2,   2,  2,  2,  2,        3,3,3,1,   2,  2,  2,  2,   2,  2,  2,  2,   2,  2,  2,  2,      0},
/*FLT2x1*/{   0,0,0,0,   1,  0,  0,  0,   0,  0,  0,  0,   0,  0,  0,  0,        0,0,0,0,   1,  0,  0,  0,   0,  0,  0,  0,   0,  0,  0,  0,      0},
/*FLT2x2*/{   0,0,0,0,   0,  1,  0,  0,   0,  0,  0,  0,   0,  0,  0,  0,        0,0,0,0,   0,  1,  0,  0,   0,  0,  0,  0,   0,  0,  0,  0,      0},
/*FLT2x3*/{   0,0,0,0,   0,  0,  1,  0,   0,  0,  0,  0,   0,  0,  0,  0,        0,0,0,0,   0,  0,  1,  0,   0,  0,  0,  0,   0,  0,  0,  0,      0},
/*FLT2x4*/{   0,0,0,0,   0,  0,  0,  1,   0,  0,  0,  0,   0,  0,  0,  0,        0,0,0,0,   0,  0,  0,  1,   0,  0,  0,  0,   0,  0,  0,  0,      0},
/*FLT3x1*/{   0,0,0,0,   0,  0,  0,  0,   1,  0,  0,  0,   0,  0,  0,  0,        0,0,0,0,   0,  0,  0,  0,   1,  0,  0,  0,   0,  0,  0,  0,      0},
/*FLT3x2*/{   0,0,0,0,   0,  0,  0,  0,   0,  1,  0,  0,   0,  0,  0,  0,        0,0,0,0,   0,  0,  0,  0,   0,  1,  0,  0,   0,  0,  0,  0,      0},
/*FLT3x3*/{   0,0,0,0,   0,  0,  0,  0,   0,  0,  1,  0,   0,  0,  0,  0,        0,0,0,0,   0,  0,  0,  0,   0,  0,  1,  0,   0,  0,  0,  0,      0},
/*FLT3x4*/{   0,0,0,0,   0,  0,  0,  0,   0,  0,  0,  1,   0,  0,  0,  0,        0,0,0,0,   0,  0,  0,  0,   0,  0,  0,  1,   0,  0,  0,  0,      0},
/*FLT4x1*/{   0,0,0,0,   0,  0,  0,  0,   0,  0,  0,  0,   1,  0,  0,  0,        0,0,0,0,   0,  0,  0,  0,   0,  0,  0,  0,   1,  0,  0,  0,      0},
/*FLT4x2*/{   0,0,0,0,   0,  0,  0,  0,   0,  0,  0,  0,   0,  1,  0,  0,        0,0,0,0,   0,  0,  0,  0,   0,  0,  0,  0,   0,  1,  0,  0,      0},
/*FLT4x3*/{   0,0,0,0,   0,  0,  0,  0,   0,  0,  0,  0,   0,  0,  1,  0,        0,0,0,0,   0,  0,  0,  0,   0,  0,  0,  0,   0,  0,  1,  0,      0},
/*FLT4x4*/{   0,0,0,0,   0,  0,  0,  0,   0,  0,  0,  0,   0,  0,  0,  1,        0,0,0,0,   0,  0,  0,  0,   0,  0,  0,  0,   0,  0,  0,  1,      0},

		  {   0,0,0,0,   0,  0,  0,  0,   0,  0,  0,  0,   0,  0,  0,  0,        0,0,0,0,   0,  0,  0,  0,   0,  0,  0,  0,   0,  0,  0,  0,      4}

	};

	bool ShaderAtomPort::IsTypeCompatible(ShaderAtomDataFormat other)
	{
		byte r = CompatibleTable[DataType][other];
		if (r==4)
		{

		}
		return !!r;
	}

	void ShaderAtomPort::Parse(ConfigurationSection* sect)
	{
		Name = sect->getName();
		IsInputOrOutput = sect->GetAttributeBool(L"Input");

		sect->tryGetAttribute(L"Varying", VaringTypeName);

		String usage;
		if (sect->tryGetAttribute(L"Usage", usage))
		{
			Usage = EffectParameter::ParseParamUsage(usage);
		}
		else
		{
			Usage = EPUSAGE_Unknown;
		}

		DataType = ShaderNetUtils::ParseAtomDataFormat(sect->getAttribute(L"Format"));
	}
	ConfigurationSection* ShaderAtomPort::Save()
	{
		ConfigurationSection* sect = new ConfigurationSection(Name);
		sect->AddAttributeBool(L"Input", IsInputOrOutput);
		sect->AddAttributeString(L"Varying", VaringTypeName);
		sect->AddAttributeString(L"Usage", EffectParameter::ToString(Usage));
		sect->AddAttributeString(L"Format", ShaderNetUtils::ToString(DataType));

		return sect;
	}

	/************************************************************************/
	/*  ShaderAtomType                                                      */
	/************************************************************************/
	bool ShaderAtomType::MatchPorts(const ShaderAtomType* other)
	{
		return false;
	}

	void ShaderAtomType::CopyFrom(const ShaderAtomType* newOne)
	{
		m_codeBody = newOne->m_codeBody;
		m_name = newOne->m_name;
		m_minorSMVersion = newOne->m_minorSMVersion;
		m_majorSMVersion = newOne->m_majorSMVersion;
	}

	void ShaderAtomType::Load(const String& filePath)
	{
		Configuration config;
		XMLConfigurationFormat::Instance.Load(FileLocation(filePath), &config);

		ConfigurationSection* sect = config[L"Basic"];
		m_name = sect->getValue(L"Name");
		m_type = ShaderTypeConverter.Parse(sect->getValue(L"Type"));
		m_majorSMVersion = sect->GetInt(L"MajorSMVersion");
		m_minorSMVersion = sect->GetInt(L"MinorSMVersion");
		m_codeBody = sect->getValue(L"Code");
		
		for (ConfigurationSection* ss : config[L"Ports"]->getSubSections())
		{
			ShaderAtomPort port;
			port.Parse(ss);
			m_ports.Add(port);
		}
	}
	void ShaderAtomType::Save(const String& filePath)
	{
		Configuration config(L"Root");

		config.Add(new ConfigurationSection(L"Basic"));

		ConfigurationSection* sect = new ConfigurationSection(L"Ports");
		config.Add(new ConfigurationSection(L"Ports"));
		for (ShaderAtomPort& p : m_ports)
		{
			sect->AddSection(p.Save());
		}

		//config->Save(filePath);
		XMLConfigurationFormat::Instance.Save(&config, FileOutStream(filePath));
		
	}
	/************************************************************************/
	/*  ShaderAtomLibraryManager                                            */
	/************************************************************************/

	SINGLETON_IMPL(ShaderAtomLibraryManager);

	void ShaderAtomLibraryManager::Load(const FileLocation& fl)
	{
		String basePath = PathUtils::GetDirectory(fl.getPath());

		Configuration config;
		XMLConfigurationFormat::Instance.Load(fl, &config);

		for (ConfigurationSection* ss : config.getSubSections())
		{
			String file = ss->getValue();
			String filePath = PathUtils::Combine(basePath, file);

			ShaderAtomType* type = new ShaderAtomType();
			type->Load(filePath);

			m_table.Add(type->getName(), type);
		}
	}

	void ShaderAtomLibraryManager::AddAtomType(ShaderAtomType* type)
	{
		m_table.Add(type->getName(), type);
	}
	void ShaderAtomLibraryManager::RemoveAtomType(ShaderAtomType* type)
	{
		m_table.Remove(type->getName());
	}

	ShaderAtomType* ShaderAtomLibraryManager::FindAtomType(const String& name)
	{
		ShaderAtomType* result = 0;
		m_table.TryGetValue(name, result);
		return result;
	}
}