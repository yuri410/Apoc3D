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
				When the engine auto bind a parameter, it checks the parameter's usage.
				This enum is used to fast check common usages.
				Custom usage is also accepted by micro effect code.
			*/
			enum EffectParamUsage
			{
				EPUSAGE_Unknown,
				EPUSAGE_AmbientColor,
				EPUSAGE_DiffuseColor,
				EPUSAGE_EmissiveColor,
				EPUSAGE_SpecularColor,
				EPUSAGE_Power,
				
			};

			/* Defines a parameter in an micro effect code.
   
			   This also contains effect param mapping info.
			*/
			class APAPI EffectParameter
			{
			private:
				
			public:
				String Name;
				EffectParamUsage TypicalUsage;
				String CustomUsage;
				bool IsCustomUsage;

				

				EffectParameter() { }
				EffectParameter(const String& name);
				~EffectParameter(void);

				static EffectParamUsage ParseParamUsage(const String& val);
			};
		};
	};
};
#endif
