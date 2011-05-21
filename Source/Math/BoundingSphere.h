#ifndef BOUNDINGSPHERE_H
#define BOUNDINGSPHERE_H

#include "Common.h"
#include "Vector.h"

namespace Apoc3D
{
	namespace Math
	{
		enum ContainmentType
		{
			CONTAIN_Disjoint,
			CONTAIN_Contains,
			CONTAIN_Intersects
		};

		class APAPI BoundingSphere
		{
		public:
			/* Specifies the center point of the sphere.
			*/
			Vector3 Center;
			/* The radius of the sphere.
			*/
			float Radius;

			BoundingSphere(Vector3 center, float radius)
			{
				Center = center;
				Radius = radius;
			}

			/* Determines whether the sphere contains the specified box.
			*/
			static ContainmentType Contains(const BoundingSphere &sphere, const BoundingBox &box);
		};
	}
}

#endif