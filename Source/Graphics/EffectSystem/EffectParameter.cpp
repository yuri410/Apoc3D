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

#include "Utility/StringUtils.h"

using namespace Apoc3D::Utility;

namespace Apoc3D
{
	namespace Graphics
	{
		namespace EffectSystem
		{
			EffectParameter::EffectParameter(const String& name)
				: Name(name), IsCustomUsage(false), TypicalUsage(EPUSAGE_Unknown)
			{
			}


			EffectParameter::~EffectParameter(void)
			{
			}

			EffectParamUsage EffectParameter::ParseParamUsage(const String& val)
			{
				String v = val;
				StringUtils::ToLowerCase(v);

				if (v == L"ambient_color")
				{
					return EPUSAGE_AmbientColor;
				}
				else if (v == L"diffuse_color")
				{
					return EPUSAGE_DiffuseColor;
				}
				else if (v == L"emissive_color")
				{
					return EPUSAGE_EmissiveColor;
				}
				else if (v == L"specular_color")
				{
					return EPUSAGE_SpecularColor;
				}
				else if (v == L"power_color")
				{
					return EPUSAGE_Power;
				}
				return EPUSAGE_Unknown;
			}


			/************************************************************************/
			/*                                                                      */
			/************************************************************************/

			Camera* RendererEffectParams::CurrentCamera = 0;
		}
	}
}