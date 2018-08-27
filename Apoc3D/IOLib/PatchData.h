#pragma once
#ifndef APOC3D_PATCHDATA_H
#define APOC3D_PATCHDATA_H

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
#include "apoc3d/Math/Vector.h"
#include "apoc3d/Collections/List.h"

using namespace Apoc3D::Math;

namespace Apoc3D
{
	namespace IO
	{
		class APAPI PatchData
		{
		private:
			Apoc3D::Collections::List<Vector3> Points;
		};
	}
}

#endif