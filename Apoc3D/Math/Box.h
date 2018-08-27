#pragma once
#ifndef APOC3D_BOX_H
#define APOC3D_BOX_H

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

#include "apoc3d/ApocCommon.h"

namespace Apoc3D
{
	namespace Math
	{
		class Box
		{
		public:
			int32 Left;
			int32 Top;
			int32 Right;

			int32 Bottom;
			int32 Front;
			int32 Back;

			int32 getWidth() const { return Right - Left; }
			int32 getHeight() const { return Bottom - Top; }
			int32 getDepth() const { return Back - Front; }

			Box(int32 top, int32 left, int32 front, int32 width, int32 height, int32 depth)
				: Top(top), Left(left), Front(front),
				Bottom(top + height), Right(left + width), Back(front + depth)
			{

			}
		};


	}
}
#endif