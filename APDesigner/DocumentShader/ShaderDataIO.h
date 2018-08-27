/* -----------------------------------------------------------------------
 * This source file is part of Apoc3D Framework
 * 
 * Copyright (c) 2009-2018 Tao Xin
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

#ifndef SHADERDOCUMENTDATA_H
#define SHADERDOCUMENTDATA_H

#include "APDesigner/APDCommon.h"
#include "SNetCommonTypes.h"
#include "ShaderAtomType.h"

namespace APDesigner
{
	struct ShaderAtomLinkInfo
	{
		int SourcePortID;
		int TargetPortID;

		int SourceNodeIndex;
		int TargetNodeIndex;

		int VaryingNodeIndex;
		int ConstNodeIndex;

		void Parse(ConfigurationSection* sect);
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

		List<ShaderNetVaryingNode> VaryingNodes;
		List<ShaderNetConstantNode> ConstantNodes;

		void Load(const String& filePath);
		void Save(const String& filePath);
	};



}

#endif