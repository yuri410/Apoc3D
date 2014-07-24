#pragma once
#ifndef APOC3D_EFFECTPARAMETERS_H
#define APOC3D_EFFECTPARAMETERS_H

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


#include "apoc3d/Common.h"
#include "apoc3d/Collections/List.h"
#include "apoc3d/Collections/HashMap.h"
#include "apoc3d/Math/Color.h"
#include "apoc3d/Math/Vector.h"
#include "apoc3d/Graphics/RenderSystem/Shader.h"

using namespace Apoc3D::Collections;
using namespace Apoc3D::Graphics::RenderSystem;
using namespace Apoc3D::Math;

namespace Apoc3D
{
	namespace Graphics
	{
		namespace EffectSystem
		{
			/**
			 *   Defines usage of a effect parameters.
			 *
			 *   When the engine auto bind a parameter, it first checks the parameter's usage to
			 *   find the corresponding data, then assign it to the param. 
			 *
			 *   EffectParamUsage has 3 special usages as follows:
			 *   1. EPUSAGE_Unknow.
			 *   2. EPUSAGE_InstanceBlob
			 *   3. EPUSAGE_CustomMaterialParam
			 *   See the fields for detailed description.
			 */
			enum EffectParamUsage
			{
				/**
				 *  The unknown usage means the binding for typical usage is unavailable, 
				 *  the engine will do nothing.
				 *
				 *  This is useful in occasions like the effect is a post effect without materials, the value of custom
				 *  effect can be assigned by scene rendering scripts.
				 */
				EPUSAGE_Unknown=0,
				/** 
				 *  mc4_ambient
				 */
				EPUSAGE_MtrlC4_Ambient,
				/**
				 *  mc4_diffuse
				 */
				EPUSAGE_MtrlC4_Diffuse,
				/**
				 *  mc4_emissive
				 */
				EPUSAGE_MtrlC4_Emissive,
				/**
				 *  mc4_specular
				 */
				EPUSAGE_MtrlC4_Specular,
				/**
				 *  mc_power
				 */
				EPUSAGE_MtrlC_Power,
				

				/**
				 *  tr_worldviewproj
				 */
				EPUSAGE_Trans_WorldViewProj=100,
				/**
				 *  tr_worldvieworiproj
				 */
				EPUSAGE_Trans_WorldViewOriProj,
				/**
				 *  tr_world
				 */
				EPUSAGE_Trans_World,
				/**
				 * tr_worldview
				 */
				EPUSAGE_Trans_WorldView,
				/**
				 * tr_view
				 */
				EPUSAGE_Trans_View,
				/**
				 * tr_viewproj
				 */
				EPUSAGE_Trans_ViewProj,
				/**
				 * tr_invview
				 */
				EPUSAGE_Trans_InvView,

				/**
				 * tr_instanceworld
				 *  This parameter is supposed to be a 4x4 matrix array in the shader with a length of
				 *  InstancingData::MaxOneTimeInstances
				 */
				EPUSAGE_Trans_InstanceWorlds,
				/**
				 * tr_proj
				 */
				EPUSAGE_Trans_Projection,
				/**
				 * tr_invproj
				 */
				EPUSAGE_Trans_InvProj,

				
				/**
				 * m4x3_bonestransform
				 */
				EPUSAGE_M4X3_BoneTrans,
				/**
				 * m4x4_bonestransform
				 */
				EPUSAGE_M4X4_BoneTrans,


				/**
				 * v3_camerax
				 */
				EPUSAGE_V3_CameraX,
				/**
				 * v3_cameray
				 */
				EPUSAGE_V3_CameraY,
				/**
				 * v3_cameraz
				 */
				EPUSAGE_V3_CameraZ,


				/**
				 * tex_0
				 */
				EPUSAGE_Tex0=200,
				EPUSAGE_Tex1,
				EPUSAGE_Tex2,
				EPUSAGE_Tex3,
				EPUSAGE_Tex4,
				EPUSAGE_Tex5,
				EPUSAGE_Tex6,
				EPUSAGE_Tex7,
				EPUSAGE_Tex8,
				EPUSAGE_Tex9,
				EPUSAGE_Tex10,
				EPUSAGE_Tex11,
				EPUSAGE_Tex12,
				EPUSAGE_Tex13,
				EPUSAGE_Tex14,
				EPUSAGE_Tex15,
				EPUSAGE_Tex16,

				/**
				 *  lv3_lightDir
				 */
				EPUSAGE_LV3_LightDir=500,
				/**
				 *  lc4_ambient
				 */
				EPUSAGE_LC4_Ambient,
				/**
				 *  lc4_diffuse
				 */
				EPUSAGE_LC4_Diffuse,
				/** 
				 *  lc4_specular
				 */
				EPUSAGE_LC4_Specular,

				/**
				 *  pv3_viewPos
				 */
				EPUSAGE_PV3_ViewPos=1000,
				/** 
				 *  sv2_viewportSize
				 */
				EPUSAGE_SV2_ViewportSize,
				/**
				 *  sv2_invviewportSize
				 */
				EPUSAGE_SV2_InvViewportSize,

				/**
				 *  s_unifiedTime. A time from 0 to 5pi will be assigned to the parameter
				 */
				EPUSAGE_S_UnifiedTime,

				/**
				 *  s_farPlane
				 */
				EPUSAGE_S_FarPlane,
				/**
				 *  s_nearPlane
				 */
				EPUSAGE_S_NearPlane,

				/**
				 *  The engine will get the value for the parameter from UserData pointer 
				 *  which is expected pointing to a InstanceInfoBlob struct.
				 *  The value will be obtained from the list inside the struct.
				*/
				EPUSAGE_InstanceBlob,

				/** 
				 *  The engine will look up the entry(MaterialCustomParameter)
				 *	with the same name from material's CustomParamTable in render operation.
				 *  Custom material param need to be specified in a String format in EffectParameter 
				 *  as CustomMaterialParamName. 
				 * 
				 *  Internally, parameters using automatic(typical usages) will be manipulated 
				 *  faster than this.
				*/
				EPUSAGE_CustomMaterialParam
			};

			/**
			 *  Includes all global scene render resources such as the current camera, lighting that could 
			 *  be used in effects.
			 */
			class APAPI RendererEffectParams
			{
			public:
				static Camera* CurrentCamera;
				static Vector3 LightDirection;
				static Color4 LightAmbient;
				static Color4 LightDiffuse;
				static Color4 LightSpecular;

				static void Reset()
				{
					CurrentCamera = 0;
				}
			};


			enum CustomEffectParameterType
			{
				CEPT_Float = 0,
				CEPT_Vector2 = 1,
				CEPT_Vector4 = 2,
				CEPT_Boolean = 3,
				CEPT_Integer = 4,
				CEPT_Ref_Vector2 = 5,
				CEPT_Ref_Vector3 = 6,
				CEPT_Ref_Vector4 = 7,
				CEPT_Ref_Texture = 8,
				CEPT_Ref_TextureHandle = 9,
			};

			/** 
			 *  Defines a parameter in an effect containing effect param mapping info.
			 * 
			 *  CustomMaterialParamName and InstanceBlobIndex will be used based on the Usage of the EffectParameter. 
			 *  See EPUSAGE_CustomMaterialParam for details.
			 *
			 *  When EPUSAGE_InstanceBlob or EPUSAGE_CustomMaterialParam is unused in effect, 
			 *  no more attention from application developer is needed on setting the effect parameter.
			 *
			 *  The parameter's usage is configured by the effect parameter listing file along with the shader code,
			 *  when building resources using APBuild or APDesigner.
			 */
			class APAPI EffectParameter
			{	
			public:
				String Name;
				EffectParamUsage Usage;

				String CustomMaterialParamName;
				int32 InstanceBlobIndex;

				ShaderType ProgramType;

				int RegisterIndex;
				
				int SamplerIndex;
				ShaderSamplerState SamplerState;

				EffectParameter() : RegisterIndex(-1), SamplerIndex(-1) { }
				EffectParameter(const String& name);
				~EffectParameter(void);

				static EffectParamUsage ParseParamUsage(const String& val);
				static String ToString(EffectParamUsage usage);

				//static HashMap<String, EffectParamUsage>::Enumerator getParameterUsageEnumeration();
				static void FillParameterUsageNames(List<String>& results);


				static bool IsReference(CustomEffectParameterType t)
				{
					return t == CEPT_Ref_Texture || t == CEPT_Ref_Vector2 || t == CEPT_Ref_Vector3 || t == CEPT_Ref_Vector4;
				}
			private:

			};


			struct APAPI InstanceInfoBlob
			{
				struct CustomValue
				{
					/** 
					 *  The data type of the value.
					 */
					CustomEffectParameterType Type;
					uint Value[16];
					void* RefValue;

					CustomValue() 
					{ }

					CustomValue(float val)
						: RefValue(nullptr), Type(CEPT_Float)
					{ AsSingle() = val; }

					CustomValue(const Vector2& val)
						: RefValue(nullptr), Type(CEPT_Vector2)
					{ AsVector2() = val; }

					CustomValue(const Vector4& val)
						: RefValue(nullptr), Type(CEPT_Vector4)
					{ AsVector4() = val; }

					CustomValue(bool val)
						: RefValue(nullptr), Type(CEPT_Boolean)
					{ AsBoolean() = val; }

					CustomValue(int val)
						: RefValue(nullptr), Type(CEPT_Integer)
					{ AsInteger() = val; }

					float& AsSingle() { return reinterpret_cast<float&>(Value); }
					Vector2& AsVector2() { return reinterpret_cast<Vector2&>(Value); }
					Vector4& AsVector4() { return reinterpret_cast<Vector4&>(Value); }
					bool& AsBoolean() { return reinterpret_cast<bool&>(Value); }
					int& AsInteger() { return reinterpret_cast<int&>(Value); }

					Vector2& AsVector2Ref() { return *reinterpret_cast<Vector2*>(RefValue); }
					Vector4& AsVector4Ref() { return *reinterpret_cast<Vector4*>(RefValue); }
					Vector3& AsVector3Ref() { return *reinterpret_cast<Vector3*>(RefValue); }
				};

				List<CustomValue> DataList;
			};
		};
	};
};
#endif
