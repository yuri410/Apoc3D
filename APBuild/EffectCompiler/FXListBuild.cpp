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

#include "../BuildConfig.h"
#include "../BuildEngine.h"
#include "../CompileLog.h"

#include "apoc3d/Config/ConfigurationSection.h"
#include "apoc3d/Config/XmlConfigurationFormat.h"
#include "apoc3d/Collections/FastList.h"
#include "apoc3d/Collections/ExistTable.h"
#include "apoc3d/Collections/FastMap.h"
#include "apoc3d/Collections/CollectionsCommon.h"
#include "apoc3d/IOLib/Streams.h"
#include "apoc3d/IOLib/BinaryWriter.h"
#include "apoc3d/IOLib/EffectData.h"
#include "apoc3d/Utility/StringUtils.h"
#include "apoc3d/Vfs/File.h"
#include "apoc3d/Vfs/ResourceLocation.h"
#include "apoc3d/Vfs/PathUtils.h"



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

		Configuration* xml = new Configuration(L"FXList");

		for (ConfigurationSection::SubSectionEnumerator iter = sect->GetSubSectionEnumrator();iter.MoveNext();)
		{
			ConfigurationSection* ss = new ConfigurationSection((*iter.getCurrentValue())->getName());
			xml->Add(ss);
		}

		//xml->Save(config.DestFile);
		XMLConfigurationFormat::Instance.Save(xml, new FileOutStream(config.DestFile));
		delete xml;
	}
}