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

#ifndef SHADERNETWORKTYPES_H
#define SHADERNETWORKTYPES_H

#include "APDCommon.h"
#include "Graphics/EffectSystem/EffectParameter.h"

using namespace Apoc3D;
using namespace Apoc3D::Graphics::EffectSystem;

namespace APDesigner
{
	enum ShaderAtomDataExchangeType
	{

	};

	enum ShaderNetOutputType
	{
		/** Pixel shaders
		*/
		SNOUT_Color,
		SNOUT_Depth,

		/** Vertex shaders
		*/
		SNOUT_Position,
		SNOUT_TexCoord0,
		SNOUT_TexCoord1,
		SNOUT_TexCoord2,
		SNOUT_TexCoord3,
		SNOUT_TexCoord4,
		SNOUT_TexCoord5,
		SNOUT_TexCoord6,
		SNOUT_TexCoord7,

	};
	
	/** Auto generated input parameter node
	*/
	struct ShaderNetInputNode
	{
		EffectParamUsage Usage;
	};

	struct ShaderNetOutputNode
	{
		int RequiredSMVersionMajor;
		int RequiredSMVersionMinor;

		ShaderNetOutputType Type;
	};
}

#endif