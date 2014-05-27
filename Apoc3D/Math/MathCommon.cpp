/*
-----------------------------------------------------------------------------
This source file is part of Apoc3D

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
#include "MathCommon.h"

namespace Apoc3D
{
	namespace Math
	{

		const float PI = L_PIf;//float( 4.0 * atan( 1.0 ) );
		const float Two_PI = L_Two_PIf; // PI * 2
		const float Half_PI = L_Half_PIf;// float(PI * 0.5);
		const float Root2 = 1.41421f;
		const float Root3 = 1.73205f;

		
		//extern const float MaxFloat = std::numeric_limits<float>::max();
		//extern const float MinFloat = std::numeric_limits<float>::min();
		//extern const float POSINF_Float = std::numeric_limits<float>::infinity();
		//extern const float NEGINF_Float = -std::numeric_limits<float>::infinity();


		//float Lerp(float a, float b, float amount)
		//{
		//	return a + (b-a)*amount;
		//}
	}
}