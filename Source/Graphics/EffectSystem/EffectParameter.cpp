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
#include "Utility/StringUtils.h"

using namespace Apoc3D::Core;
using namespace Apoc3D::Utility;

namespace Apoc3D
{
	namespace Graphics
	{
		namespace EffectSystem
		{
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

				if (v == L"mc4_ambient")
				{
					return EPUSAGE_MtrlC4_Ambient;
				}
				else if (v == L"mc4_diffuse")
				{
					return EPUSAGE_MtrlC4_Diffuse;
				}
				else if (v == L"mc4_emissive")
				{
					return EPUSAGE_MtrlC4_Emissive;
				}
				else if (v == L"mc4_specular")
				{
					return EPUSAGE_MtrlC4_Specular;
				}
				else if (v == L"mc_power")
				{
					return EPUSAGE_MtrlC_Power;
				}
				

				else if (v == L"tr_worldviewproj")
				{
					return EPUSAGE_Trans_WorldViewProj;
				}
				else if (v == L"tr_worldvieworiproj")
				{
					return EPUSAGE_Trans_WorldViewOriProj;
				}
				else if (v == L"tr_world")
				{
					return EPUSAGE_Trans_World;
				}
				else if (v == L"m4x3_bonestransform")
				{
					return EPUSAGE_M4X3_BoneTrans;
				}
				else if (v == L"m4x4_bonestransform")
				{
					return EPUSAGE_M4X4_BoneTrans;
				}


				
				else if (v == L"tex_0")
				{
					return EPUSAGE_Tex0;
				}
				else if (v == L"tex_1")
				{
					return EPUSAGE_Tex1;
				}
				else if (v == L"tex_2")
				{
					return EPUSAGE_Tex2;
				}
				else if (v == L"tex_3")
				{
					return EPUSAGE_Tex3;
				}
				else if (v == L"tex_4")
				{
					return EPUSAGE_Tex4;
				}
				else if (v == L"tex_5")
				{
					return EPUSAGE_Tex5;
				}
				else if (v == L"tex_6")
				{
					return EPUSAGE_Tex6;
				}
				else if (v == L"tex_7")
				{
					return EPUSAGE_Tex7;
				}
				else if (v == L"tex_8")
				{
					return EPUSAGE_Tex8;
				}
				else if (v == L"tex_9")
				{
					return EPUSAGE_Tex9;
				}
				else if (v == L"tex_10")
				{
					return EPUSAGE_Tex10;
				}
				else if (v == L"tex_11")
				{
					return EPUSAGE_Tex11;
				}
				else if (v == L"tex_12")
				{
					return EPUSAGE_Tex12;
				}
				else if (v == L"tex_13")
				{
					return EPUSAGE_Tex13;
				}
				else if (v == L"tex_14")
				{
					return EPUSAGE_Tex14;
				}
				else if (v == L"tex_15")
				{
					return EPUSAGE_Tex15;
				}



				else if (v==L"lv3_lightdir")
				{
					return EPUSAGE_LV3_LightDir;
				}
				else if (v==L"lc4_ambient")
				{
					return EPUSAGE_LC4_Ambient;
				}
				else if (v==L"lc4_diffuse")
				{
					return EPUSAGE_LC4_Diffuse;
				}
				else if (v==L"lc4_specular")
				{
					return EPUSAGE_LC4_Specular;
				}
				else if (v==L"pv3_viewpos")
				{
					return EPUSAGE_PV3_ViewPos;
				}
				else if (v==L"sv2_viewportsize")
				{
					return EPUSAGE_SV2_ViewportSize;
				}
				LogManager::getSingleton().Write(LOG_Graphics, L"Unknown effect parameter usage: " + val, LOGLVL_Infomation );
				return EPUSAGE_Unknown;
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