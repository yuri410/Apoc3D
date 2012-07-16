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
#ifndef MESHBUILD_H
#define MESHBUILD_H

#include "APBCommon.h"

using namespace Apoc3D;
using namespace Apoc3D::Config;
using namespace Apoc3D::IO;

namespace APBuild
{
	class MeshBuild
	{
	private:
		static void BuildByASS(const MeshBuildConfig& config);
		static void BuildByFBX(const MeshBuildConfig& config);
		static void BuildByD3D(const MeshBuildConfig& config);
	public:
		static void Build(const ConfigurationSection* sect);

		static void ConvertVertexData(ModelData* data, const MeshBuildConfig& config);
		static void CollapseMeshs(ModelData* data, const MeshBuildConfig& config);
	};

	class TAnimBuild
	{
	public:
		static void Build(const ConfigurationSection* sect);
	};
}

#endif