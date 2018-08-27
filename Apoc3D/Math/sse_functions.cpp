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

#include "sse_functions.h"

#if APOC3D_MATH_IMPL == APOC3D_SSE
class FieldInitializer
{
public:
	FieldInitializer()
	{
		uint Data2[4] = {0x80000000, 0x80000000, 0x80000000, 0x80000000};
		memcpy((void*)&_MASKSIGN_, Data2, sizeof(Data2));

	}

} Initializer;

#endif