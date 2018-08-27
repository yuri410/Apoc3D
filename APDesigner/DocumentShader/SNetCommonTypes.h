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

#ifndef SHADERNETWORKTYPES_H
#define SHADERNETWORKTYPES_H

#include "APDesigner/APDCommon.h"

namespace APDesigner
{
	enum ShaderAtomDataFormat
	{
		ATOMDATA_Int1,
		ATOMDATA_Int2,
		ATOMDATA_Int3,
		ATOMDATA_Int4,

		ATOMDATA_Int2x1,
		ATOMDATA_Int2x2,
		ATOMDATA_Int2x3,
		ATOMDATA_Int2x4,

		ATOMDATA_Int3x1,
		ATOMDATA_Int3x2,
		ATOMDATA_Int3x3,
		ATOMDATA_Int3x4,

		ATOMDATA_Int4x1,
		ATOMDATA_Int4x2,
		ATOMDATA_Int4x3,
		ATOMDATA_Int4x4,


		
		ATOMDATA_Float1,
		ATOMDATA_Float2,
		ATOMDATA_Float3,
		ATOMDATA_Float4,

		ATOMDATA_Float2x1,
		ATOMDATA_Float2x2,
		ATOMDATA_Float2x3,
		ATOMDATA_Float2x4,

		ATOMDATA_Float3x1,
		ATOMDATA_Float3x2,
		ATOMDATA_Float3x3,
		ATOMDATA_Float3x4,

		ATOMDATA_Float4x1,
		ATOMDATA_Float4x2,
		ATOMDATA_Float4x3,
		ATOMDATA_Float4x4,

		ATOMDATA_Other,
		ATOMDATA_Count
	};

	
	enum ShaderInterfaceParamType
	{
		SIPT_AutoContant,
		SIPT_CustomConstant,
		/** used when the param is associated with data outside shaders. 
		 *  (i.e. vertex stream, final PS outputs)
		 *  cases where either ShaderNetVaringType or ShaderNetOutputType is used
		 */
		SIPT_OutsideIOVarying,
		SIPT_PreviousStageVarying
	};
	
	///** Auto generated input parameter node
	//*/
	//struct ShaderNetInputNode
	//{
	//	String Name;


	//	ShaderInterfaceParamType InterfaceType;

	//	/** If the node is a effect param(constant or uniform)(SIPT_AutoContant), it is specified here
	//	*/
	//	EffectParamUsage Usage;
	//	/** OR when SIPT_CustomConstant. 
	//		EffectParamUsage falls back to this when the usage name is not built in supported. 
	//	*/
	//	String CustomUsage; 
	//	/** OR when SIPT_OutsideIOVarying 
	//		In an input node, this is only used when fetching data from vertex stream.
	//	*/
	//	ShaderNetVaryingType VaringType;
	//	/** OR when SIPT_PreviousStageVarying 
	//		Used when passing data between shaders. 
	//		ShaderNetVaryingType falls back to this when the varying name is not built in supported.
	//	*/
	//	String VaringTypeName;

	//	void Parse(ConfigurationSection* sect);
	//};

	//struct ShaderNetOutputNode
	//{
	//	String Name;

	//	/** when SIPT_OutsideIOVarying
	//	*/
	//	ShaderNetOutputType Type; 
	//	/** OR when SIPT_PreviousStageVarying
	//	 *  If this is not empty, the output data will be passed via shader result. This name is used as identification.
	//	 *  varying data will be passed as texture coords to PS. 
	//	 *  The coord slot number will be determined based on this name and the output nodes in the previous stage.
	//	 */
	//	String VaringTypeName; 

	//	void Parse(ConfigurationSection* sect);
	//};



	enum ShaderNetVaryingType
	{
		/** Common
		*/
		SNVT_COLOR0,
		SNVT_COLOR1,

		SNVT_TEXCOORD0,
		SNVT_TEXCOORD1,
		SNVT_TEXCOORD2,
		SNVT_TEXCOORD3,
		SNVT_TEXCOORD4,
		SNVT_TEXCOORD5,
		SNVT_TEXCOORD6,
		SNVT_TEXCOORD7,

		/** Pixel shaders
		*/
		
		SNVT_DEPTH0,

		SNVT_VFACE,

		/** Vertex shaders
		*/
		SNVT_POSITION0,
		SNVT_POSITION1,

		SNVT_POSITIONT,

		SNVT_NORMAL0,
		SNVT_NORMAL1,

		SNVT_BINORMAL0,
		SNVT_BINORMAL1,

		SNVT_TANGENT0,
		SNVT_TANGENT1,

		SNVT_PSIZE0,
		SNVT_PSIZE1,

		SNVT_BLENDINDICES0,
		SNVT_BLENDINDICES1,
		SNVT_BLENDWEIGHT0,
		SNVT_BLENDWEIGHT1,

		/** In this case custom varying name string will be matched
		*/
		SNVT_OTHER
	};

	enum ShaderNetVaryingStage
	{
		SNVStage_PreVS,
		SNVStage_PrePS,
		SNVStage_Final
	};

	
	/** Represents any input/output nodes with stream data.
	 *  Used when passing data between shaders. 
	 */
	struct ShaderNetVaryingNode
	{
		String Name;

		ShaderNetVaryingStage Stage;

		ShaderNetVaryingType Type;
		/** If this is not empty, this will be the name used as identification.
		 *  And varying data will be passed as texture coords to PS. 
		 *  The coord slot number will be determined based on this name and the output nodes in the previous stage.
		 */
		String CustomVaryingType;

		void Parse(ConfigurationSection* sect);
	};

	/** Represents any effect parameter(constant or aka uniform)
	*/
	struct ShaderNetConstantNode
	{
		String Name;


		EffectParamUsage Usage;
		/** EffectParamUsage falls back to this when the usage name is not built in supported. 
		*/
		String CustomUsage;

		void Parse(ConfigurationSection* sect);
	};

	class ShaderNetUtils
	{
	public:
		static ShaderAtomDataFormat ParseAtomDataFormat(const String& value);
		static String ToString(ShaderAtomDataFormat value);

		static ShaderNetVaryingStage ParseVaryingStage(const String& value);
		static String ToString(ShaderNetVaryingStage value);

		static ShaderNetVaryingType ParseVaryingType(const String& value);
		static String ToString(ShaderNetVaryingType value);
	};
}

#endif