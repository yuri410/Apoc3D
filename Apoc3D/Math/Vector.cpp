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

#include "Vector.h"

#include "Matrix.h"
#include "Point.h"

#include "apoc3d/Utility/StringUtils.h"
#include "apoc3d/Collections/List.h"

using namespace Apoc3D::Utility;

namespace Apoc3D
{
	namespace Math
	{
		const Vector2 Vector2::Zero = Vector2::Set(0.0f);
		const Vector2 Vector2::UnitX = Vector2(1, 0);
		const Vector2 Vector2::UnitY = Vector2(0, 1);
		const Vector2 Vector2::One = Vector2::Set(1.0f);

		const Vector3 Vector3::Zero = Vector3::Set(0.0f);
		const Vector3 Vector3::UnitX = Vector3(1, 0, 0);
		const Vector3 Vector3::UnitY = Vector3(0, 1, 0);
		const Vector3 Vector3::UnitZ = Vector3(0, 0, 1);
		const Vector3 Vector3::One = Vector3::Set(1.0f);

		const Vector4 Vector4::Zero = Vector4::Set(0.0f);
		const Vector4 Vector4::UnitX = Vector4(1, 0, 0, 0);
		const Vector4 Vector4::UnitY = Vector4(0, 1, 0, 0);
		const Vector4 Vector4::UnitZ = Vector4(0, 0, 1, 0);
		const Vector4 Vector4::UnitW = Vector4(0, 0, 0, 1);
		const Vector4 Vector4::One = Vector4::Set(1.0f);

		PointF::PointF(const Vector2& v)
			: X(v.X), Y(v.Y) { }

		Vector2::Vector2() { }

		Vector2::Vector2(const PointF& p)
			: X(p.X), Y(p.Y) { }

		void Vector2::Parse(const String& str, bool loosy)
		{
			if (loosy)
				StringUtils::SplitParseSingles(str, this->operator float *(), 2, L", ");
			else
				StringUtils::SplitParseSinglesChecked(str, this->operator float *(), 2, L", ");
		}
		String Vector2::ToTextString(int precision) const
		{
			uint64 flags = precision != -1 ? StrFmt::FP(precision) : StringUtils::SF_Default;
			
			String result = L"[";
			result.append(StringUtils::SingleToString(X, flags));
			result.append(L", ");
			result.append(StringUtils::SingleToString(Y, flags));
			result.append(L"]");
			return result;
		}
		String Vector2::ToParsableString(int precision) const
		{
			uint64 flags = precision != -1 ? StrFmt::FP(precision) : StringUtils::SF_Default;

			String result = StringUtils::SingleToString(X, flags);
			result.append(L", ");
			result.append(StringUtils::SingleToString(Y, flags));
			return result;
		}

		Vector3::Vector3() { }

		void Vector3::Parse(const String& str, bool loosy)
		{
			if (loosy)
				StringUtils::SplitParseSingles(str, this->operator float *(), 3, L", ");
			else
				StringUtils::SplitParseSinglesChecked(str, this->operator float *(), 3, L", ");
		}
		String Vector3::ToTextString(int precision) const
		{
			uint64 flags = precision != -1 ? StrFmt::FP(precision) : StringUtils::SF_Default;

			String result = L"[";
			result.append(StringUtils::SingleToString(X, flags));
			result.append(L", ");
			result.append(StringUtils::SingleToString(Y, flags));
			result.append(L", ");
			result.append(StringUtils::SingleToString(Z, flags));
			result.append(L"]");
			return result;
		}
		String Vector3::ToParsableString(int precision) const
		{
			uint64 flags = precision != -1 ? StrFmt::FP(precision) : StringUtils::SF_Default;

			String result = StringUtils::SingleToString(X, flags);
			result.append(L", ");
			result.append(StringUtils::SingleToString(Y, flags));
			result.append(L", ");
			result.append(StringUtils::SingleToString(Z, flags));
			return result;
		}

		Vector4 Vector3::Transform(const Vector3& vector, const Matrix& transform)
		{
			float x = vector.X * transform.M11 + vector.Y * transform.M21 + vector.Z * transform.M31 + transform.M41;
			float y = vector.X * transform.M12 + vector.Y * transform.M22 + vector.Z * transform.M32 + transform.M42;
			float z = vector.X * transform.M13 + vector.Y * transform.M23 + vector.Z * transform.M33 + transform.M43;
			float w = vector.X * transform.M14 + vector.Y * transform.M24 + vector.Z * transform.M34 + transform.M44;
			
			return Vector4(x, y, z, w);
		}
		Vector3 Vector3::TransformSimple(const Vector3& vector, const Matrix& transform)
		{
			float x = vector.X * transform.M11 + vector.Y * transform.M21 + vector.Z * transform.M31 + transform.M41;
			float y = vector.X * transform.M12 + vector.Y * transform.M22 + vector.Z * transform.M32 + transform.M42;
			float z = vector.X * transform.M13 + vector.Y * transform.M23 + vector.Z * transform.M33 + transform.M43;
			return Vector3(x, y, z);
		}
		Vector3 Vector3::TransformCoordinate(const Vector3& vector, const Matrix& transform)
		{
			float x = vector.X * transform.M11 + vector.Y * transform.M21 + vector.Z * transform.M31 + transform.M41;
			float y = vector.X * transform.M12 + vector.Y * transform.M22 + vector.Z * transform.M32 + transform.M42;
			float z = vector.X * transform.M13 + vector.Y * transform.M23 + vector.Z * transform.M33 + transform.M43;
			float w = 1 / (vector.X * transform.M14 + vector.Y * transform.M24 + vector.Z * transform.M34 + transform.M44);
			return Vector3(x*w, y*w, z*w);
		}
		Vector3 Vector3::TransformNormal(const Vector3& vector, const Matrix& transform)
		{
			float x = vector.X * transform.M11 + vector.Y * transform.M21 + vector.Z * transform.M31;
			float y = vector.X * transform.M12 + vector.Y * transform.M22 + vector.Z * transform.M32;
			float z = vector.X * transform.M13 + vector.Y * transform.M23 + vector.Z * transform.M33;
			return Vector3(x, y, z);
		}

		Vector4::Vector4() { }
		
		void Vector4::Parse(const String& str, bool loosy)
		{
			if (loosy)
				StringUtils::SplitParseSingles(str, this->operator float *(), 4, L", ");
			else
				StringUtils::SplitParseSinglesChecked(str, this->operator float *(), 4, L", ");
		}
		String Vector4::ToTextString(int precision) const
		{
			uint64 flags = precision != -1 ? StrFmt::FP(precision) : StringUtils::SF_Default;

			String result = L"[";
			result.append(StringUtils::SingleToString(X, flags));
			result.append(L", ");
			result.append(StringUtils::SingleToString(Y, flags));
			result.append(L", ");
			result.append(StringUtils::SingleToString(Z, flags));
			result.append(L", ");
			result.append(StringUtils::SingleToString(W, flags));
			result.append(L"]");
			return result;
		}
		String Vector4::ToParsableString(int precision) const
		{
			uint64 flags = precision != -1 ? StrFmt::FP(precision) : StringUtils::SF_Default;

			String result = StringUtils::SingleToString(X, flags);
			result.append(L", ");
			result.append(StringUtils::SingleToString(Y, flags));
			result.append(L", ");
			result.append(StringUtils::SingleToString(Z, flags));
			result.append(L", ");
			result.append(StringUtils::SingleToString(W, flags));
			return result;
		}

		Vector4 Vector4::Transform(const Vector4& vector, const Matrix& transform)
		{
			float x = vector.X * transform.M11 + vector.Y * transform.M21 + vector.Z * transform.M31 + vector.W * transform.M41;
			float y = vector.X * transform.M12 + vector.Y * transform.M22 + vector.Z * transform.M32 + vector.W * transform.M42;
			float z = vector.X * transform.M13 + vector.Y * transform.M23 + vector.Z * transform.M33 + vector.W * transform.M43;
			float w = vector.X * transform.M14 + vector.Y * transform.M24 + vector.Z * transform.M34 + vector.W * transform.M44;
			return Vector4(x, y, z, w);
		}



		/************************************************************************/
		/* Vector3Utils                                                         */
		/************************************************************************/
#if APOC3D_MATH_IMPL == APOC3D_SSE
		Vector4 Vector3Utils::Transform(Vector3 vector, const Matrix& transform)
		{
			__m128 v = vector;
			*(reinterpret_cast<float*>(&v)+3)=1;

			float buffer[4];
			buffer[0] = SIMDVec3Dot(v, transform.Row1);
			buffer[1] = SIMDVec3Dot(v, transform.Row2);
			buffer[2] = SIMDVec3Dot(v, transform.Row3);
			buffer[3] = SIMDVec3Dot(v, transform.Row4);

			return Vector4Utils::LDVectorPtr(buffer);
		}

		Vector3 Vector3Utils::TransformSimple(Vector3 vector, const Matrix& transform)
		{
			__m128 v = vector;
			*(reinterpret_cast<float*>(&v)+3)=1.0f;

			float x = SIMDVec3Dot(v, transform.Row1);
			float y = SIMDVec3Dot(v, transform.Row2);
			float z = SIMDVec3Dot(v, transform.Row3);
			return Vector3Utils::LDVector(x,y,z);
		}

		Vector3 Vector3Utils::TransformCoordinate(Vector3 vector, const Matrix& transform)
		{
			__m128 v = vector;
			*(reinterpret_cast<float*>(&v)+3)=1.0f;

			float x = SIMDVec3Dot(v, transform.Row1);
			float y = SIMDVec3Dot(v, transform.Row2);
			float z = SIMDVec3Dot(v, transform.Row3);

			
			__m128 col4 = SIMDVecLoad(transform.M14, transform.M24, transform.M34, transform.M44);
			
			float w = 1 / SIMDVec3Dot(col4, v);

			__m128 result = Vector3Utils::LDVector(x,y,z);

			result = SIMDVecMul(result, w);

			return result;
		}

		Vector3 Vector3Utils::TransformNormal(Vector3 vector, const Matrix& transform)
		{
			__m128 v = vector;
			*(reinterpret_cast<float*>(&v)+3)=0;

			float x = SIMDVec3Dot(v, transform.Row1);
			float y = SIMDVec3Dot(v, transform.Row2);
			float z = SIMDVec3Dot(v, transform.Row3);
			return Vector3Utils::LDVector(x,y,z);
		}
#elif APOC3D_MATH_IMPL == APOC3D_DEFAULT

#endif
		/************************************************************************/
		/* Vector4Utils                                                         */
		/************************************************************************/
#if APOC3D_MATH_IMPL == APOC3D_SSE
		Vector4 Vector4Utils::Transform(Vector4 vector, const Matrix& transform)
		{
			float buffer[4];
			buffer[0] = SIMDVec4Dot(vector, transform.Row1);
			buffer[1] = SIMDVec4Dot(vector, transform.Row2);
			buffer[2] = SIMDVec4Dot(vector, transform.Row3);
			buffer[3] = SIMDVec4Dot(vector, transform.Row4);
			return Vector4Utils::LDVectorPtr(buffer);
		}

#elif APOC3D_MATH_IMPL == APOC3D_DEFAULT
		

#endif

	}
}