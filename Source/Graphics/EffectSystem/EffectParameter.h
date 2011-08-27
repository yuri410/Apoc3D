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

namespace Apoc3D
{
	namespace Graphics
	{
		namespace EffectSystem
		{
			/** Defines typical usage of a effect parameters.
				When the engine auto bind a parameter, it first checks the parameter's usage to
				find the corresponding data, then to assign to the param. 
				
				Alternatively, custom usage in String form is also accepted by EffectParameter.
				Internally, parameters that uses enum this to specify typical usage 
				can be manipulated faster than those with custom param usages.
			*/
			enum EffectParamUsage
			{
				EPUSAGE_Unknown,
				EPUSAGE_AmbientColor,
				EPUSAGE_DiffuseColor,
				EPUSAGE_EmissiveColor,
				EPUSAGE_SpecularColor,
				EPUSAGE_Power,
				EPUSAGE_LightDir,
				EPUSAGE_ViewPos,
			};

			/** Include all scene render resources such as the current camera, lighting that could 
				be used in effects.
			*/
			class APAPI RendererEffectParams
			{
			public:
				static Camera* CurrentCamera;


				static void Reset()
				{
					CurrentCamera = 0;
				}
			};

			/* Defines a parameter in an effect.
   
			   This also contains effect param mapping info.
			*/
			class APAPI EffectParameter
			{	
			public:
				String Name;
				EffectParamUsage TypicalUsage;
				String CustomUsage;
				bool IsCustomUsage;

				

				EffectParameter() { }
				EffectParameter(const String& name);
				~EffectParameter(void);

				static EffectParamUsage ParseParamUsage(const String& val);

			private:

			};
		};
	};
};
#endif
