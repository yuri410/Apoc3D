#include "BoundingSphere.h"

#include "BoundingBox.h"

namespace Apoc3D
{
	namespace Math
	{
		ContainmentType BoundingSphere::Contains(const BoundingSphere &sphere, const BoundingBox &box)
		{
			Vector3 vector;

			if (!BoundingBox::Intersects(box, sphere))
				return CONTAIN_Disjoint;

			float radius = sphere.Radius * sphere.Radius;
			Vector3 d;
			vector.X = sphere.Center.X - box.Minimum.X;
			vector.Y = sphere.Center.Y - box.Maximum.Y;
			vector.Z = sphere.Center.Z - box.Maximum.Z;

			if (Vec3LengthSquared(vector) > radius)
				return CONTAIN_Intersects;

			vector.X = sphere.Center.X - box.Maximum.X;
			vector.Y = sphere.Center.Y - box.Maximum.Y;
			vector.Z = sphere.Center.Z - box.Maximum.Z;

			if (Vec3LengthSquared(vector) > radius)
				return CONTAIN_Intersects;

			vector.X = sphere.Center.X - box.Maximum.X;
			vector.Y = sphere.Center.Y - box.Minimum.Y;
			vector.Z = sphere.Center.Z - box.Maximum.Z;

			if (Vec3LengthSquared(vector) > radius)
				return CONTAIN_Intersects;

			vector.X = sphere.Center.X - box.Minimum.X;
			vector.Y = sphere.Center.Y - box.Minimum.Y;
			vector.Z = sphere.Center.Z - box.Maximum.Z;

			if (Vec3LengthSquared(vector) > radius)
				return CONTAIN_Intersects;

			vector.X = sphere.Center.X - box.Minimum.X;
			vector.Y = sphere.Center.Y - box.Maximum.Y;
			vector.Z = sphere.Center.Z - box.Minimum.Z;

			if (Vec3LengthSquared(vector) > radius)
				return CONTAIN_Intersects;

			vector.X = sphere.Center.X - box.Maximum.X;
			vector.Y = sphere.Center.Y - box.Maximum.Y;
			vector.Z = sphere.Center.Z - box.Minimum.Z;

			if (Vec3LengthSquared(vector) > radius)
				return CONTAIN_Intersects;

			vector.X = sphere.Center.X - box.Maximum.X;
			vector.Y = sphere.Center.Y - box.Minimum.Y;
			vector.Z = sphere.Center.Z - box.Minimum.Z;

			if (Vec3LengthSquared(vector) > radius)
				return CONTAIN_Intersects;

			vector.X = sphere.Center.X - box.Minimum.X;
			vector.Y = sphere.Center.Y - box.Minimum.Y;
			vector.Z = sphere.Center.Z - box.Minimum.Z;

			if (Vec3LengthSquared(vector) > radius)
				return CONTAIN_Intersects;

			return CONTAIN_Contains;
		}
	}
}
