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

#ifndef MATERIALTPYES_H
#define MATERIALTPYES_H

#include "Common.h"

using namespace std;

namespace Apoc3D
{
	namespace Graphics
	{
		static const int32 MaxTextures = 16;

		enum MaterialCustomParameterType
		{
			MTRLPT_Float = 0,
			MTRLPT_Vector2 = 1,
			MTRLPT_Vector4 = 2,
			MTRLPT_Boolean = 3,
			MTRLPT_Integer = 4,
			MTRLPT_Ref_Vector2 = 5,
			MTRLPT_Ref_Vector3 = 6,
			MTRLPT_Ref_Vector4 = 7,
			MTRLPT_Ref_Texture = 8,
		};

		/** Defines custom material parameters. 
		*/
		struct MaterialCustomParameter
		{
			/** The data type of the parameter.
			*/
			MaterialCustomParameterType Type;
			byte Value[16];

			void* RefValue;

			/** The usage of this parameter. Effect check this for auto binding effect parameters.
			*/
			String Usage;

			bool IsReference() const 
			{
				return Type == MTRLPT_Ref_Texture || Type == MTRLPT_Ref_Vector2 || Type == MTRLPT_Ref_Vector3 || Type == MTRLPT_Ref_Vector4;
			}

			MaterialCustomParameter() { }
			MaterialCustomParameter(bool value, const String usage = L"")
				: Type(MTRLPT_Boolean), Usage(usage)
			{
				*reinterpret_cast<bool*>(Value) = value;
			}


		};
		typedef unordered_map<String, MaterialCustomParameter> CustomParamTable;
	}
}

#endif