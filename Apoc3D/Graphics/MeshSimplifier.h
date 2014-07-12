#pragma once
#ifndef APOC3D_MESHSIMPLIFIER_H
#define APOC3D_MESHSIMPLIFIER_H

#include "ModelTypes.h"

#include "apoc3d/Math/MathCommon.h"

using namespace Apoc3D::Collections;
using namespace Apoc3D::Math;

namespace Apoc3D
{
	namespace Graphics
	{
		namespace MeshSimplifier
		{
			APAPI void ProgressiveMesh(const FastList<Vector3>& vert, const FastList<MeshFace>& tri, FastList<int>& map, FastList<int>& permutation);
		}
	}
}

#endif