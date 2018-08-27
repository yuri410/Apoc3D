#pragma once
#ifndef APOC3D_MATH_H
#define APOC3D_MATH_H

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

#include "MathCommon.h"
#include "Point.h"
#include "Rectangle.h"

namespace Apoc3D
{
	namespace Math
	{
		APAPI uint16 R32ToR16I(uint32 value);
		APAPI uint32 R16ToR32I(uint16 value);

		inline uint16 R32ToR16(float value) { return R32ToR16I(reinterpret_cast<const uint32&>(value)); }

		// once a floating point value is formed, bits maybe altered implicitly due to floating point behavior
		inline float R16ToR32(uint16 value)
		{
			uint32 iv = R32ToR16I(value);
			return reinterpret_cast<const float&>(iv);
		}

		inline Point Round(PointF p)
		{
			return Point(Round(p.X), Round(p.Y));
		}

		inline Apoc3D::Math::Rectangle Round(RectangleF r)
		{
			return { Round(r.X), Round(r.Y), Round(r.Width), Round(r.Height) };
		}

	}
}

#endif