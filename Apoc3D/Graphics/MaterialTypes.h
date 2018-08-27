#pragma once
#ifndef APOC3D_MATERIALTPYES_H
#define APOC3D_MATERIALTPYES_H

/* -----------------------------------------------------------------------
 * This source file is part of Apoc3D Framework
 * 
 * Copyright (c) 2009-2018 Tao Xin
 * 
 * This content of this file is subject to the terms of the Mozilla Public 
 * License v2.0. If a copy of the MPL was not distributed with this file, 
 * you can obtain one at http://mozilla.org/MPL/2.0/.
 * 
 * This program is distributed in the hope that it will be useful, 
 * WITHOUT WARRANTY OF ANY KIND; either express or implied. See the 
 * Mozilla Public License for more details.
 * 
 * ------------------------------------------------------------------------
 */

#include "apoc3d/Math/Vector.h"
#include "apoc3d/Graphics/EffectSystem/EffectParameter.h"

using namespace Apoc3D::Math;

namespace Apoc3D
{
	namespace Graphics
	{
		const int32 MaxTextures = 16;

		/** Defines custom material parameters. */
		struct APAPI MaterialCustomParameter
		{
			EffectSystem::CustomEffectParameterType Type;		/** The data type of the parameter. */
			uint Value[16];

			void* RefValue;

			/** The usage of this parameter. Effect check this for auto binding effect parameters. */
			String Usage;


			MaterialCustomParameter() { }
			MaterialCustomParameter(bool value, const String& usage = L"")
				: Type(EffectSystem::CEPT_Boolean), Usage(usage)
			{
				*reinterpret_cast<bool*>(Value) = value;
			}
			MaterialCustomParameter(float value, const String& usage = L"")
				: Type(EffectSystem::CEPT_Float), Usage(usage)
			{
				*reinterpret_cast<float*>(Value) = value;
			}
			MaterialCustomParameter(int value, const String& usage = L"")
				: Type(EffectSystem::CEPT_Integer), Usage(usage)
			{
				*reinterpret_cast<int*>(Value) = value;
			}
			MaterialCustomParameter(Vector2 value, const String& usage = L"")
				: Type(EffectSystem::CEPT_Vector2), Usage(usage)
			{
				*reinterpret_cast<Vector2*>(Value) = value;
			}
			MaterialCustomParameter(Vector4 value, const String& usage = L"")
				: Type(EffectSystem::CEPT_Vector4), Usage(usage)
			{
				*reinterpret_cast<Vector4*>(Value) = value;
			}
		};
		typedef HashMap<String, MaterialCustomParameter> CustomParamTable;
	}
}

#endif