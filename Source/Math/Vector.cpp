/*
-----------------------------------------------------------------------------
This source file is part of Apoc3D

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
#include "Vector.h"

#include "Matrix.h"

namespace Apoc3D
{
	namespace Math
	{
		const Vector2 Vector2Utils::Zero = Vector2Utils::LDVector(0);
		const Vector2 Vector2Utils::UnitX = Vector2Utils::LDVector(1,0);
		const Vector2 Vector2Utils::UnitY = Vector2Utils::LDVector(0,1);
		const Vector2 Vector2Utils::One = Vector2Utils::LDVector(1);

		const Vector3 Vector3Utils::Zero = Vector3Utils::LDVector(0);
		const Vector3 Vector3Utils::UnitX = Vector3Utils::LDVector(1, 0, 0);
		const Vector3 Vector3Utils::UnitY = Vector3Utils::LDVector(0, 1, 0);
		const Vector3 Vector3Utils::UnitZ = Vector3Utils::LDVector(0, 0, 1);
		const Vector3 Vector3Utils::One = Vector3Utils::LDVector(1);
		
		const Vector4 Vector4Utils::Zero = Vector4Utils::LDVector(0);
		const Vector4 Vector4Utils::UnitX = Vector4Utils::LDVector(1, 0, 0, 0);
		const Vector4 Vector4Utils::UnitY = Vector4Utils::LDVector(0, 1, 0, 0);
		const Vector4 Vector4Utils::UnitZ = Vector4Utils::LDVector(0, 0, 1, 0);
		const Vector4 Vector4Utils::UnitW = Vector4Utils::LDVector(0, 0, 0, 1);
		const Vector4 Vector4Utils::One = Vector4Utils::LDVector(1);

		/************************************************************************/
		/* Vector3Utils                                                         */
		/************************************************************************/
#if APOC3D_MATH_IMPL == APOC3D_SSE
		Vector4 Vector3Utils::Transform(Vector3 vector, const Matrix& transform)
		{
			__m128 v = vector;
			(*reinterpret_cast<float*>(v)+3)=1;

			float buffer[4];
			buffer[0] = Vec3Dot(v, transform.Row1);
			buffer[1] = Vec3Dot(v, transform.Row2);
			buffer[2] = Vec3Dot(v, transform.Row3);
			buffer[3] = Vec3Dot(v, transform.Row4);

			return Vector4Utils::LDVectorPtr(buffer);
		}

		Vector3 Vector3Utils::TransformSimple(Vector3 vector, const Matrix& transform)
		{
			__m128 v = vector;
			(*reinterpret_cast<float*>(v)+3)=1;

			float x = Vec3Dot(v, transform.Row1);
			float y = Vec3Dot(v, transform.Row2);
			float z = Vec3Dot(v, transform.Row3);
			return Vector3Utils::LDVector(x,y,z);
		}

		Vector3 Vector3Utils::TransformCoordinate(const Vector3& vector, const Matrix& transform)
		{
			__m128 v = vector;
			(*reinterpret_cast<float*>(v)+3)=1;

			float x = Vec3Dot(v, transform.Row1);
			float y = Vec3Dot(v, transform.Row2);
			float z = Vec3Dot(v, transform.Row3);

			
			__m128 col4 = VecLoad(transform.M14, transform.M24, transform.M34, transform.M44);
			
			float w = 1 / Vec3Dot(col4, v);

			__m128 result = Vector3Utils::LDVector(x,y,z);

			result = VecMul(result, w);

			return result;
		}

		Vector3 Vector3Utils::TransformNormal(const Vector3& vector, const Matrix& transform)
		{
			__m128 v = vector;
			(*reinterpret_cast<float*>(v)+3)=0;

			float x = Vec3Dot(v, transform.Row1);
			float y = Vec3Dot(v, transform.Row2);
			float z = Vec3Dot(v, transform.Row3);
			return Vector3Utils::LDVector(x,y,z);
		}
#elif APOC3D_MATH_IMPL == APOC3D_DEFAULT

		Vector4 Vector3Utils::Transform(const Vector3& vector, const Matrix& transform)
		{
			float x = vector.X * transform.M11 + vector.Y * transform.M21 + vector.Z * transform.M31 + transform.M41;
			float y = vector.X * transform.M12 + vector.Y * transform.M22 + vector.Z * transform.M32 + transform.M42;
			float z = vector.X * transform.M13 + vector.Y * transform.M23 + vector.Z * transform.M33 + transform.M43;
			float w = vector.X * transform.M14 + vector.Y * transform.M24 + vector.Z * transform.M34 + transform.M44;
			return Vector4Utils::LDVector(x,y,z,w);
		}

		Vector3 Vector3Utils::TransformSimple(const Vector3& vector, const Matrix& transform)
		{
			float x = vector.X * transform.M11 + vector.Y * transform.M21 + vector.Z * transform.M31 + transform.M41;
			float y = vector.X * transform.M12 + vector.Y * transform.M22 + vector.Z * transform.M32 + transform.M42;
			float z = vector.X * transform.M13 + vector.Y * transform.M23 + vector.Z * transform.M33 + transform.M43;
			return Vector3Utils::LDVector(x,y,z);
		}

		Vector3 Vector3Utils::TransformCoordinate(const Vector3& vector, const Matrix& transform)
		{
			float x = vector.X * transform.M11 + vector.Y * transform.M21 + vector.Z * transform.M31 + transform.M41;
			float y = vector.X * transform.M12 + vector.Y * transform.M22 + vector.Z * transform.M32 + transform.M42;
			float z = vector.X * transform.M13 + vector.Y * transform.M23 + vector.Z * transform.M33 + transform.M43;
			float w = 1 / (vector.X * transform.M14 + vector.Y * transform.M24 + vector.Z * transform.M34 + transform.M44);
			return Vector3Utils::LDVector(x*w,y*w,z*w);
		}

		Vector3 Vector3Utils::TransformNormal(const Vector3& vector, const Matrix& transform)
		{
			float x = vector.X * transform.M11 + vector.Y * transform.M21 + vector.Z * transform.M31;
			float y = vector.X * transform.M12 + vector.Y * transform.M22 + vector.Z * transform.M32;
			float z = vector.X * transform.M13 + vector.Y * transform.M23 + vector.Z * transform.M33;
			return Vector3Utils::LDVector(x,y,z);
		}
#endif
		/************************************************************************/
		/* Vector4Utils                                                         */
		/************************************************************************/
#if APOC3D_MATH_IMPL == APOC3D_SSE
		Vector4 Vector4Utils::Transform(Vector4 vector, const Matrix& transform)
		{
			float buffer[4];
			buffer[0] = Vec3Dot(v, transform.Row1);
			buffer[1] = Vec3Dot(v, transform.Row2);
			buffer[2] = Vec3Dot(v, transform.Row3);
			buffer[3] = Vec3Dot(v, transform.Row4);
			return Vector4Utils::LDVectorPtr(buffer);
		}

#elif APOC3D_MATH_IMPL == APOC3D_DEFAULT
		Vector4 Vector4Utils::Transform(const Vector4& vector, const Matrix& transform)
		{
			float x = vector.X * transform.M11 + vector.Y * transform.M21 + vector.Z * transform.M31 + vector.W * transform.M41;
			float y = vector.X * transform.M12 + vector.Y * transform.M22 + vector.Z * transform.M32 + vector.W * transform.M42;
			float z = vector.X * transform.M13 + vector.Y * transform.M23 + vector.Z * transform.M33 + vector.W * transform.M43;
			float w = vector.X * transform.M14 + vector.Y * transform.M24 + vector.Z * transform.M34 + vector.W * transform.M44;
			return Vector4Utils::LDVector(x,y,z,w);
		}

#endif

//#if APOC3D_MATH_IMPL == APOC3D_SSE
//
//#elif APOC3D_MATH_IMPL == APOC3D_DEFAULT
//
//#endif
	}
}