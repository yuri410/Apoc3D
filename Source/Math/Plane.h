/*
-----------------------------------------------------------------------------
This source file is part of labtd

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
#ifndef PLANE_H
#define PLANE_H

#include "Common.h"
#include "Vector.h"
#include "Matrix.h"

namespace Apoc3D
{
	namespace Math
	{
		class APOC3D_API Plane
		{
		public:
			union
			{
				struct  
				{
					float X;
					float Y;
					float Z;
					float D;

				};
				Vector Normal;
			};
			Plane(const Plane &another)
			{
				Normal = another.Normal;
			}
			Plane(float a, float b, float c, float d)
			{
				X = a; Y = b; Z = c;
				D = d;
			}
			Plane()
			{
				X = Y = Z = D = 0;
			}
			Plane(Vector3 v)
			{
				Normal = v;
			}
			Plane(Vector3 normal, float d)
			{
				Normal = normal;
				D = d;
			}
			Plane(Vector3 point, Vector3 normal)
			{
				Normal = normal;
				D = -Vec3Dot(normal, point);
			}

			Plane(Vector3 point1, Vector3 point2, Vector3 point3)
			{
				Vector3 p12 = VecSub(point2, point1);
				Vector3 p13 = VecSub(point3, point1);
				Vector3 cross = Vec3Cross(p12, p13);
				cross = Vec3Normalize(cross);
				D = -Vec3Dot(cross, point1);
			}

			/* Calculates the dot product of a specified vector and the normal of the plane plus the distance value of the plane.
			*/
			float Dot3(Vector3 a)
			{
				return Vec3Dot(a, Normal) + D;
			}
			/* Calculates the dot product of the specified vector and plane.
			*/
			float Dot4(Vector a)
			{
				return Vec4Dot(a, Normal);
			}
			/* Calculates the dot product of the specified vector and the normal of the plane.
			*/
			float DotNormal(Vector3 n)
			{
				return Vec3Dot(n, Normal);
			}
			/* Changes the coefficients of the normal vector of the plane to make it of unit length.
			*/
			void Normalize()
			{
				float mag = Vec3Length(Normal);

				if (mag>EPSILON)
				{
					X *= mag;
					Y *= mag;
					Z *= mag;
					D *= mag;
				}
			}


			/* Changes the coefficients of the normal vector of the plane to make it of unit length.
			*/
			static Plane Normalize(const Plane &plane)
			{
				Plane np = plane;
				np.Normalize();
				return np;
			}
			/* Transforms a normalized plane by a quaternion rotation.
			*/
			static Plane Transform(const Plane &plane, const Matrix &transformation)
			{
				Plane result;
				Matrix trans = transformation;
				trans.Inverse();

				result.X = Vec4Dot( transformation.Row1, plane.Normal);
				result.Y = Vec4Dot( transformation.Row2, plane.Normal);
				result.Z = Vec4Dot( transformation.Row3, plane.Normal);
				result.D = Vec4Dot( transformation.Row4, plane.Normal);
				return result;
			}
			/* Transforms an array of normalized planes by a quaternion rotation.
			*/
			static void Transform(Plane* result, const Plane* plane, int count, const Matrix &transformation)
			{
				
				Matrix trans = transformation;
				trans.Inverse();

				for (int i=0;i<count;i++)
				{
					Plane &r = result[i];
					r.X = Vec4Dot( transformation.Row1, plane[i].Normal);
					r.Y = Vec4Dot( transformation.Row2, plane[i].Normal);
					r.Z = Vec4Dot( transformation.Row3, plane[i].Normal);
					r.D = Vec4Dot( transformation.Row4, plane[i].Normal);					
				}
			}
			/* Finds the intersection between a plane and a line.
			*/
			static void Transform(const Plane &plane, const Quaternion &rotation)
			{

			}
			static bool Intersects(const Plane &plane, Vector3 start, Vector3 end, Vector3 &intersectPoint)
			{
				Vector3 dir = VecSub( end, start);

				float cos = Vec3Dot(dir, plane.Normal);

				if (cos < EPSILON)
				{
					intersectPoint = ZeroVec;
					return false;
				}

				float d1 = Vec3Dot(start, plane.Normal);
				float d2 = Vec3Dot(end, plane.Normal);

				if (d1 * d2 < 0)
				{
					intersectPoint = ZeroVec;
					return false;
				}

				cos /= Vec3Length(dir);
				float sin = (float)sqrtf(1 - cos * cos);

				float dist = d1 / sin;


				Vector off = VecMul(dir , dist);

				intersectPoint = VecAdd(start, off);
				return true;
			}

		};
	}
}


#endif