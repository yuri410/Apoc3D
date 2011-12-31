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
#ifndef EFFECTPARAMETERS_H
#define EFFECTPARAMETERS_H

#include "Common.h"
#include "Math/Color.h"
#include "Math/Vector.h"
#include "Graphics/RenderSystem/Shader.h"

using namespace Apoc3D::Graphics::RenderSystem;
using namespace Apoc3D::Math;

namespace Apoc3D
{
	namespace Graphics
	{
		namespace EffectSystem
		{
			enum ShaderType
			{
				SHDT_Vertex,
				SHDT_Pixel
			};

			/** Defines typical usage of a effect parameters.
				When the engine auto bind a parameter, it first checks the parameter's usage to
				find the corresponding data, then assign it to the param. 
				
				Alternatively, custom usage in a String format is also accepted by EffectParameter.
				Internally, parameters using automatic(normal usages) can be manipulated 
				faster than those using custom param usages. See EffectParameter for the definition.
			*/
			enum EffectParamUsage
			{
				/** The unknown usage means the binding for typical usage is unavailable, 
					the engine will check custom usage.
				*/
				EPUSAGE_Unknown=0,
				/** mc4_ambient
				*/
				EPUSAGE_MtrlC4_Ambient,
				/** mc4_diffuse
				*/
				EPUSAGE_MtrlC4_Diffuse,
				/** mc4_emissive
				*/
				EPUSAGE_MtrlC4_Emissive,
				/** mc4_specular
				*/
				EPUSAGE_MtrlC4_Specular,
				/** mc_power
				*/
				EPUSAGE_MtrlC_Power,
				
				/** tr_worldviewproj
				*/
				EPUSAGE_Trans_WorldViewProj=100,
				/** tr_worldvieworiproj
				*/
				EPUSAGE_Trans_WorldViewOriProj,
				/** tr_world
				*/
				EPUSAGE_Trans_World,
				/** m4x3_bonestransform
				*/
				EPUSAGE_M4X3_BoneTrans,
				/** m4x4_bonestransform
				*/
				EPUSAGE_M4X4_BoneTrans,
				/** tex_0
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

				/** lv3_lightDir
				*/
				EPUSAGE_LV3_LightDir=500,
				/** lc4_ambient
				*/
				EPUSAGE_LC4_Ambient,
				/** lc4_diffuse
				*/
				EPUSAGE_LC4_Diffuse,
				/** lc4_specular
				*/
				EPUSAGE_LC4_Specular,

				/** pv3_viewPos
				*/
				EPUSAGE_PV3_ViewPos=1000,

			};

			/** Includes all global scene render resources such as the current camera, lighting that could 
				be used in effects.
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

			/** Defines a parameter in an effect containing effect param mapping info.

				A custom parameter is recognized when the usage string in the effect parameter listing file can not be parsed
				by this class. A custom parameter is bond to the parameter with the same name in the corresponding material if 
				material is used. In other occasions like the effect is a post effect without materials, the value of custom
				effect can be assigned by scene rendering scripts.

				A typical parameter's usage string can be understood by the engine as a matter of fact the engine supports
				a range of predefined usage names. When this type of parameters are used, no more attention from developer is needed.

				The so called parameter's usage string is written in the effect parameter listing file when building resources using APBuild or APDesigner.
			*/
			class APAPI EffectParameter
			{	
			public:
				String Name;
				EffectParamUsage TypicalUsage;
				String CustomUsage;
				bool IsCustomUsage;

				ShaderType ProgramType;

				int RegisterIndex;
				
				int SamplerIndex;
				ShaderSamplerState SamplerState;

				EffectParameter() : RegisterIndex(-1), SamplerIndex(-1), IsCustomUsage(false) { }
				EffectParameter(const String& name);
				~EffectParameter(void);

				static EffectParamUsage ParseParamUsage(const String& val);

			private:

			};
		};
	};
};
#endif
