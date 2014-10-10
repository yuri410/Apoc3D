/*
-----------------------------------------------------------------------------
This source file is part of Apoc3D Engine

Copyright (c) 2009+ Tao Xin

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
#include "EffectParameter.h"

#include "apoc3d/Core/Logging.h"
#include "apoc3d/Utility/StringUtils.h"
#include "apoc3d/Utility/TypeConverter.h"

using namespace Apoc3D::Core;
using namespace Apoc3D::Utility;

namespace Apoc3D
{
	namespace Graphics
	{
		namespace EffectSystem
		{
			const TypeDualConverter<EffectParamUsage> EffectParameterUsageConverter =
			{
				{ L"unknown", EPUSAGE_Unknown },
				{ L"mc4_ambient", EPUSAGE_MtrlC4_Ambient },
				{ L"mc4_diffuse", EPUSAGE_MtrlC4_Diffuse },
				{ L"mc4_emissive", EPUSAGE_MtrlC4_Emissive },
				{ L"mc4_specular", EPUSAGE_MtrlC4_Specular },
				{ L"mc_power", EPUSAGE_MtrlC_Power },

				{ L"tr_worldviewproj", EPUSAGE_Trans_WorldViewProj },
				{ L"tr_worldvieworiproj", EPUSAGE_Trans_WorldViewOriProj },
				{ L"tr_world", EPUSAGE_Trans_World },
				{ L"tr_worldview", EPUSAGE_Trans_WorldView },
				{ L"tr_view", EPUSAGE_Trans_View },
				{ L"tr_viewproj", EPUSAGE_Trans_ViewProj },
				{ L"tr_invview", EPUSAGE_Trans_InvView },
				{ L"tr_instanceworld", EPUSAGE_Trans_InstanceWorlds },
				{ L"tr_proj", EPUSAGE_Trans_Projection },
				{ L"tr_invproj", EPUSAGE_Trans_InvProj },

				{ L"m4x3_bonestransform", EPUSAGE_M4X3_BoneTrans },
				{ L"m4x4_bonestransform", EPUSAGE_M4X4_BoneTrans },

				{ L"v3_camerax", EPUSAGE_V3_CameraX },
				{ L"v3_cameray", EPUSAGE_V3_CameraY },
				{ L"v3_cameraz", EPUSAGE_V3_CameraZ },


				{ L"tex_0", EPUSAGE_Tex0 },
				{ L"tex_1", EPUSAGE_Tex1 },
				{ L"tex_2", EPUSAGE_Tex2 },
				{ L"tex_3", EPUSAGE_Tex3 },
				{ L"tex_4", EPUSAGE_Tex4 },
				{ L"tex_5", EPUSAGE_Tex5 },
				{ L"tex_6", EPUSAGE_Tex6 },
				{ L"tex_7", EPUSAGE_Tex7 },
				{ L"tex_8", EPUSAGE_Tex8 },
				{ L"tex_9", EPUSAGE_Tex9 },
				{ L"tex_10", EPUSAGE_Tex10 },
				{ L"tex_11", EPUSAGE_Tex11 },
				{ L"tex_12", EPUSAGE_Tex12 },
				{ L"tex_13", EPUSAGE_Tex13 },
				{ L"tex_14", EPUSAGE_Tex14 },
				{ L"tex_15", EPUSAGE_Tex15 },

				{ L"lv3_lightdir", EPUSAGE_LV3_LightDir },
				{ L"lc4_ambient", EPUSAGE_LC4_Ambient },
				{ L"lc4_diffuse", EPUSAGE_LC4_Diffuse },
				{ L"lc4_specular", EPUSAGE_LC4_Specular },
				{ L"pv3_viewpos", EPUSAGE_PV3_ViewPos },
				{ L"sv2_viewportsize", EPUSAGE_SV2_ViewportSize },
				{ L"sv2_invviewportsize", EPUSAGE_SV2_InvViewportSize },
				{ L"s_unifiedtime", EPUSAGE_S_UnifiedTime },

				{ L"s_farplane", EPUSAGE_S_FarPlane },
				{ L"s_nearplane", EPUSAGE_S_NearPlane },

				{ L"instanceblob", EPUSAGE_InstanceBlob },
				{ L"cmtrlparam", EPUSAGE_CustomMaterialParam },
			};
			
			EffectParameter::EffectParameter(const String& name)
				: Name(name) { }

			EffectParameter::~EffectParameter()
			{
			}

			EffectParamUsage EffectParameter::ParseParamUsage(const String& val)
			{
				String v = val;
				StringUtils::ToLowerCase(v);

				EffectParamUsage usage;
				if (EffectParameterUsageConverter.TryParse(v, usage))
					return usage;
				
				if (val.size())
				{
					LogManager::getSingleton().Write(LOG_Graphics, L"Unknown effect parameter usage: " + val, LOGLVL_Warning );
				}
				return EPUSAGE_Unknown;
			}
			String EffectParameter::ToString(EffectParamUsage usage)
			{
				String result;
				if (EffectParameterUsageConverter.TryToString(usage, result))
					return result;
				return L"unknown";
			}

			void EffectParameter::FillParameterUsageNames(List<String>& results)
			{
				EffectParameterUsageConverter.DumpNames(results);
			}

			/************************************************************************/
			/*                                                                      */
			/************************************************************************/

			Camera* RendererEffectParams::CurrentCamera = 0;
			Vector3 RendererEffectParams::LightDirection(0.707f, 0.707f,0);
			Color4 RendererEffectParams::LightAmbient(0.5f,0.5f,0.5f);
			Color4 RendererEffectParams::LightDiffuse(1.0f,1.0f,1.0f);
			Color4 RendererEffectParams::LightSpecular(1.0f,1.0f,1.0f);
		}
	}
}