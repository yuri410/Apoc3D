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
#include "EffectParameter.h"

#include "Core/Logging.h"
#include "Collections/FastMap.h"
#include "Utility/StringUtils.h"

using namespace Apoc3D::Core;
using namespace Apoc3D::Collections;
using namespace Apoc3D::Utility;

namespace Apoc3D
{
	namespace Graphics
	{
		namespace EffectSystem
		{
			class EParamConvHelper
			{
			public:
				EParamConvHelper()
					: CastTable(50, &m_comparer1), InvCastTable(50, &m_comparer2)
				{
					AddPair(L"mc4_ambient", EPUSAGE_MtrlC4_Ambient);
					AddPair(L"mc4_diffuse", EPUSAGE_MtrlC4_Diffuse);
					AddPair(L"mc4_emissive", EPUSAGE_MtrlC4_Emissive);
					AddPair(L"mc4_specular", EPUSAGE_MtrlC4_Specular);
					AddPair(L"mc_power", EPUSAGE_MtrlC_Power);

					AddPair(L"tr_worldviewproj", EPUSAGE_Trans_WorldViewProj);
					AddPair(L"tr_worldvieworiproj", EPUSAGE_Trans_WorldViewOriProj);
					AddPair(L"tr_world", EPUSAGE_Trans_World);
					AddPair(L"tr_worldview", EPUSAGE_Trans_WorldView);
					AddPair(L"tr_view", EPUSAGE_Trans_View);
					AddPair(L"tr_instanceworld", EPUSAGE_Trans_InstanceWorlds);
					AddPair(L"tr_proj", EPUSAGE_Trans_Projection);

					AddPair(L"m4x3_bonestransform", EPUSAGE_M4X3_BoneTrans);
					AddPair(L"m4x4_bonestransform", EPUSAGE_M4X4_BoneTrans);

					AddPair(L"tex_0", EPUSAGE_Tex0);
					AddPair(L"tex_1", EPUSAGE_Tex1);
					AddPair(L"tex_2", EPUSAGE_Tex2);
					AddPair(L"tex_3", EPUSAGE_Tex3);
					AddPair(L"tex_4", EPUSAGE_Tex4);
					AddPair(L"tex_5", EPUSAGE_Tex5);
					AddPair(L"tex_6", EPUSAGE_Tex6);
					AddPair(L"tex_7", EPUSAGE_Tex7);
					AddPair(L"tex_8", EPUSAGE_Tex8);
					AddPair(L"tex_9", EPUSAGE_Tex9);
					AddPair(L"tex_10", EPUSAGE_Tex10);
					AddPair(L"tex_11", EPUSAGE_Tex11);
					AddPair(L"tex_12", EPUSAGE_Tex12);
					AddPair(L"tex_13", EPUSAGE_Tex13);
					AddPair(L"tex_14", EPUSAGE_Tex14);
					AddPair(L"tex_15", EPUSAGE_Tex15);

					AddPair(L"lv3_lightdir", EPUSAGE_LV3_LightDir);
					AddPair(L"lc4_ambient", EPUSAGE_LC4_Ambient);
					AddPair(L"lc4_diffuse", EPUSAGE_LC4_Diffuse);
					AddPair(L"lc4_specular", EPUSAGE_LC4_Specular);
					AddPair(L"pv3_viewpos", EPUSAGE_PV3_ViewPos);
					AddPair(L"sv2_viewportsize", EPUSAGE_SV2_ViewportSize);
					AddPair(L"s_unifiedtime", EPUSAGE_S_UnifiedTime);
				}

				StringEuqlityComparer m_comparer1;
				Int32EqualityComparer m_comparer2;

				FastMap<String, EffectParamUsage> CastTable;
				FastMap<int, String> InvCastTable;
			private:
				void AddPair(const String& name, EffectParamUsage usage)
				{
					CastTable.Add(name, usage);
					InvCastTable.Add((int)usage, name);
				}
			};
			static EParamConvHelper EffectParameterUsageConverter;
			
			EffectParameter::EffectParameter(const String& name)
				: Name(name), IsCustomUsage(false), TypicalUsage(EPUSAGE_Unknown),
				RegisterIndex(-1), SamplerIndex(-1)
			{
			}


			EffectParameter::~EffectParameter(void)
			{
			}

			EffectParamUsage EffectParameter::ParseParamUsage(const String& val)
			{
				String v = val;
				StringUtils::ToLowerCase(v);

				EffectParamUsage usage;
				if (EffectParameterUsageConverter.CastTable.TryGetValue(v, usage))
					return usage;
				
				LogManager::getSingleton().Write(LOG_Graphics, L"Unknown effect parameter usage: " + val, LOGLVL_Infomation );
				return EPUSAGE_Unknown;
			}
			String EffectParameter::ToString(EffectParamUsage usage)
			{
				String result;
				if (EffectParameterUsageConverter.InvCastTable.TryGetValue(usage, result))
				{
					return result;
				}
				return L"unknown";
			}

			FastMap<String, EffectParamUsage>::Enumerator EffectParameter::getParameterUsageEnumeration()
			{
				return EffectParameterUsageConverter.CastTable.GetEnumerator();
			}
			/************************************************************************/
			/*                                                                      */
			/************************************************************************/

			Camera* RendererEffectParams::CurrentCamera = 0;
			Vector3 RendererEffectParams::LightDirection = Vector3Utils::LDVector(0.707f, 0.707f,0);
			Color4 RendererEffectParams::LightAmbient(0.5f,0.5f,0.5f);
			Color4 RendererEffectParams::LightDiffuse(1.0f,1.0f,1.0f);
			Color4 RendererEffectParams::LightSpecular(1.0f,1.0f,1.0f);
		}
	}
}