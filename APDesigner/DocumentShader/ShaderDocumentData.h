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

#ifndef SHADERDOCUMENTDATA_H
#define SHADERDOCUMENTDATA_H

#include "APDCommon.h"
#include "ShaderNetworkTypes.h"

using namespace Apoc3D;

namespace APDesigner
{
	struct ShaderAtomLinkInfo
	{
		int SourcePortID;
		int TargetPortID;

		int SourceNodeIndex;
		int TargetNodeIndex;

		int InputNodeIndex;
		int OutputNodeIndex;
	};

	/** Class for loading and saving the data used in a shader network document.
	*/
	class ShaderDocumentData
	{
	public:
		List<String> Nodes;
		List<ShaderAtomLinkInfo> Links;

		int MajorSMVersion;
		int MinorSMVersion;

		List<ShaderNetInputNode> InputNodes;
		List<ShaderNetOutputNode> OutputNodes;

		void Load(const String& filePath);
		void Save(const String& filePath);
	};
}

#endif