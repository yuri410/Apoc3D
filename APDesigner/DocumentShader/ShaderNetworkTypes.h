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

	enum ShaderInterfaceParamType
	{
		SIPT_AutoContant,
		SIPT_CustomConstant,
		/** used when the param is associated with data outside shaders. 
		 *  (i.e. vertex stream, final PS outputs)
		 */
		SIPT_OutsideIOVarying,
		SIPT_PreviousStageVarying
	};

	/** Defines type of shader input data.
	 *  This is only used in raw inputs.
	 */
	enum ShaderNetVaringType
	{

	};

	/** Shader final output type
	*/
	enum ShaderNetOutputType
	{
		/** Pixel shaders
		*/
		SNOUT_Color0,
		SNOUT_Depth0,

		/** Vertex shaders
		*/
		SNOUT_Position0
	};
	
	/** Auto generated input parameter node
	*/
	struct ShaderNetInputNode
	{
		String Name;


		ShaderInterfaceParamType InterfaceType;

		/** If the node is a effect param(constant or uniform)(SIPT_AutoContant), it is specified here
		*/
		EffectParamUsage Usage;
		/** OR */
		String CustomUsage; // when SIPT_CustomConstant
		/** OR */
		/** In an input node, this is only used when fetching data from vertex stream.
		*/
		ShaderNetVaringType VaringType; // when SIPT_OutsideIOVarying
		/** OR */
		/** Used when passing data between shaders.
		*/
		String VaringTypeName; // when SIPT_PreviousStageVarying

		void Parse(ConfigurationSection* sect);
	};

	struct ShaderNetOutputNode
	{
		String Name;


		ShaderNetOutputType Type; // when SIPT_OutsideIOVarying
		/** OR */
		/** If this is not empty, the output data will be passed via shader result. This name is used as identification.
		 *  varying data will be passed as texture coords to PS. 
		 *  The coord slot number will be determined based on this name and the output nodes in the previous stage.
		 */
		String VaringTypeName; // when SIPT_PreviousStageVarying

		void Parse(ConfigurationSection* sect);
	};
}

#endif