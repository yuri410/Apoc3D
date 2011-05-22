#ifndef BOUNDINGBOX_H
#define BOUNDINGBOX_H

#include "Common.h"
#include "Vector.h"
#include "Plane.h"

using namespace std;

namespace Apoc3D
{
	namespace Math
	{
		class APAPI BoundingBox
		{
		public:
			/* The highest corner of the box.
			*/
			Vector3 Maximum;

			/* The lowest corner of the box.
			*/
			Vector3 Minimum;

			BoundingBox() { }
			BoundingBox(Vector3 minimum, Vector3 maximum)
			{
				Minimum = minimum;
				Maximum = maximum;
			}

			Vector3 GetCorner(int index) const
			{
				switch (index)
				{
				case 0:
					return Vector3Utils::LDVector(
						Vector3Utils::GetX(Minimum), Vector3Utils::GetY(Maximum), Vector3Utils::GetZ(Maximum));
				case 1:
					return Vector3Utils::LDVector(
						Vector3Utils::GetX(Maximum), Vector3Utils::GetY(Maximum), Vector3Utils::GetZ(Maximum));
				case 2:
					return Vector3Utils::LDVector(
						Vector3Utils::GetX(Maximum), Vector3Utils::GetY(Minimum), Vector3Utils::GetZ(Maximum));
				case 3:
					return Vector3Utils::LDVector(
						Vector3Utils::GetX(Minimum), Vector3Utils::GetY(Minimum), Vector3Utils::GetZ(Maximum));
				case 4:
					return Vector3Utils::LDVector(
						Vector3Utils::GetX(Minimum), Vector3Utils::GetY(Maximum), Vector3Utils::GetZ(Minimum));
				case 5:
					return Vector3Utils::LDVector(
						Vector3Utils::GetX(Maximum), Vector3Utils::GetY(Maximum), Vector3Utils::GetZ(Minimum));
				case 6:
					return Vector3Utils::LDVector(
						Vector3Utils::GetX(Maximum), Vector3Utils::GetY(Minimum), Vector3Utils::GetZ(Minimum));
				case 7:
					return Vector3Utils::LDVector(
						Vector3Utils::GetX(Minimum), Vector3Utils::GetY(Minimum), Vector3Utils::GetZ(Minimum));
				}
				return Vector3Utils::Zero;
			}

			PlaneIntersectionType Intersects(const Plane& plane) const
			{
				return Intersects(this, plane);
			}
			ContainmentType Contains(Vector3 vector) const
			{
				if (Vector3Utils::IsGreaterEqual(vector, Minimum) && 
					Vector3Utils::IsLessEqual(vector, Maximum))
				{
					return CONTAIN_Contains;
				}
				return CONTAIN_Disjoint;
			}
			ContainmentType Contains(const BoundingBox& box) const
			{
				if (Maximum.X < box.Minimum.X || Minimum.X > box.Maximum.X)
				{
					return CONTAIN_Disjoint;
				}
				if (Maximum.Y < box.Minimum.Y || Minimum.Y > box.Maximum.Y)
				{
					return CONTAIN_Disjoint;
				}
				if (Maximum.Z < box.Minimum.Z || Minimum.Z > box.Maximum.Z)
				{
					return CONTAIN_Disjoint;
				}
				if (Vector3Utils::IsLessEqual(Minimum, box.Minimum) && 
					Vector3Utils::IsLessEqual(box.Maximum, Maximum))
				{
					return CONTAIN_Contains;
				}
				return CONTAIN_Intersects;
			}

			/* Determines whether the box contains the specified point.
			*/
			static ContainmentType Contains(const BoundingBox& box, Vector3 vector)
			{
				if (Vector3Utils::IsGreaterEqual(vector, box.Minimum) && 
					Vector3Utils::IsLessEqual(vector, box.Maximum))
				{
					return CONTAIN_Contains;
				}
				return CONTAIN_Disjoint;
			}

			/*  Determines whether the box contains the specified sphere.
			*/
			static ContainmentType Contains(const BoundingBox& box, const BoundingSphere& sphere)
			{				
				Vector3 clamped = Vector3Utils::Clamp(sphere.Center, box.Minimum, box.Maximum);

				float dist = Vector3Utils::DistanceSquared(sphere.Center, clamped);
				float radius = sphere.Radius;

				if (dist > (radius * radius))
					return CONTAIN_Disjoint;

				if (box.Minimum.X + radius <= sphere.Center.X && sphere.Center.X <= box.Maximum.X - radius &&
					box.Maximum.X - box.Minimum.X > radius && box.Minimum.Y + radius <= sphere.Center.Y &&
					sphere.Center.Y <= box.Maximum.Y - radius && box.Maximum.Y - box.Minimum.Y > radius &&
					box.Minimum.Z + radius <= sphere.Center.Z && sphere.Center.Z <= box.Maximum.Z - radius &&
					box.Maximum.X - box.Minimum.X > radius)
					return CONTAIN_Contains;

				return CONTAIN_Intersects;
			}
			/* Determines whether the box contains the specified box.
			*/
			static ContainmentType Contains(const BoundingBox& box1, const BoundingBox& box2)
			{
				if ((box1.Maximum.X < box2.Minimum.X) || (box1.Minimum.X > box2.Maximum.X))
				{
					return CONTAIN_Disjoint;
				}
				if ((box1.Maximum.Y < box2.Minimum.Y) || (box1.Minimum.Y > box2.Maximum.Y))
				{
					return CONTAIN_Disjoint;
				}
				if ((box1.Maximum.Z < box2.Minimum.Z) || (box1.Minimum.Z > box2.Maximum.Z))
				{
					return CONTAIN_Disjoint;
				}
				if (Vector3Utils::IsLessEqual(box1.Minimum, box2.Minimum) && 
					Vector3Utils::IsLessEqual(box2.Maximum, box1.Maximum))
				{
					return CONTAIN_Contains;
				}
				return CONTAIN_Intersects;
			}
			/* Constructs a BoundingBox that fully contains the given points.
			*/
			static void CreateFromPoints(BoundingBox& res, const Vector3* points, int count)
			{
				Vector3 min = Vector3Utils::LDVector(numeric_limits<float>::max());
				Vector3 max = Vector3Utils::LDVector(numeric_limits<float>::min());

				for (int i = 0; i < count; i++)
				{
					min = Vector3Utils::Minimize(min, points[i]);
					max = Vector3Utils::Maximize(max, points[i]);
				}

				res = BoundingBox(min, max);
			}
			/* Constructs a BoundingBox from a given sphere.
			*/
			static BoundingBox CreateFromSphere(BoundingBox& res, const BoundingSphere& sphere)
			{
				Vector3 r = Vector3Utils::LDVector(sphere.Radius);
				res.Minimum = Vector3Utils::Subtract(sphere.Center, r);
				res.Maximum = Vector3Utils::Add(sphere.Center, r);
				
			}
			/* Constructs a BoundingBox that is the as large as the total combined area of the two specified boxes.
			*/
			static BoundingBox Merge(const BoundingBox& box1, const BoundingBox& box2)
			{
				BoundingBox box3;
				box3.Minimum = Vector3Utils::Minimize(box1.Minimum, box2.Minimum);
				box3.Maximum = Vector3Utils::Maximize(box1.Maximum, box2.Maximum);
				return box3;
			}
			/* Finds the intersection between a plane and a box.
			*/
			static PlaneIntersectionType Intersects(const BoundingBox& box, const Plane& plane)
			{
				return Plane::Intersects(plane, box);
			}
			static bool Intersects(BoundingBox box, Ray ray, out float distance)
			{
				return Ray::Intersects(ray, box, out distance);
			}

			/*  Determines whether a box intersects the specified object.
			*/
			static bool Intersects(const BoundingBox& box, const BoundingSphere& sphere)
			{
				Vector3 clamped = Vector3Utils::Clamp(sphere.Center, box.Minimum, box.Maximum);
				float dist = Vector3Utils::DistanceSquared(sphere.Center, clamped);

				return (dist <= (sphere.Radius * sphere.Radius));
			}
			/* Determines whether a box intersects the specified object.
			*/
			static bool Intersects(const BoundingBox& box1, const BoundingBox& box2)
			{
				if (box1.Maximum.X < box2.Minimum.X || box1.Minimum.X > box2.Maximum.X)
					return false;

				if (box1.Maximum.Y < box2.Minimum.Y || box1.Minimum.Y > box2.Maximum.Y)
					return false;

				return (box1.Maximum.Z >= box2.Minimum.Z && box1.Minimum.Z <= box2.Maximum.Z);
			}
		};
	}
}

#endif