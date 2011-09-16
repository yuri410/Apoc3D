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

#include "FXListBuild.h"

#include "Collections/CollectionsCommon.h"
#include "Config/ConfigurationSection.h"
#include "Config/XmlConfiguration.h"
#include "Collections/FastList.h"
#include "Collections/ExistTable.h"
#include "Collections/FastMap.h"
#include "IOLib/Streams.h"
#include "IOLib/BinaryWriter.h"
#include "IOLib/EffectData.h"
#include "BuildConfig.h"
#include "BuildEngine.h"
#include "CompileLog.h"
#include "Utility/StringUtils.h"
#include "Vfs/File.h"
#include "Vfs/ResourceLocation.h"
#include "Vfs/PathUtils.h"



using namespace Apoc3D::IO;
using namespace Apoc3D::Utility;
using namespace Apoc3D::VFS;

namespace APBuild
{
	void FXListBuild::Build(const ConfigurationSection* sect)
	{
		FXListBuildConfig config;
		config.Parse(sect);

		EnsureDirectory(PathUtils::GetDirectory(config.Name));

		XMLConfiguration* xml = new XMLConfiguration(L"FXList");

		for (ConfigurationSection::SubSectionIterator iter = sect->SubSectionBegin();iter != sect->SubSectionEnd();iter++)
		{
			ConfigurationSection* ss = new ConfigurationSection(iter->second->getName());
			xml->Add(ss);
		}

		xml->Save(config.DestFile);
		delete xml;
	}
}