/*
-----------------------------------------------------------------------------
This source file is part of Apoc3D

Copyright (c) 2009+ Tao Xin

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
#include "Matrix.h"

#include "Quaternion.h"
#include "apoc3d/Collections/Stack.h"

using namespace Apoc3D::Collections;

namespace Apoc3D
{
	namespace Math
	{
		/************************************************************************/
		/*                                                                      */
		/************************************************************************/

#if APOC3D_MATH_IMPL == APOC3D_SSE
		//const __m128 _MASKSIGN_;	// - - - -
		__m128 _ZERONE_;	// 1 0 0 1
		__m128 _0FFF_;		// 0 * * *
		__m128 Sign_PNPN;	// + - + -
		__m128 Sign_NPNP;

		class FieldInitializer
		{
		public:
			FieldInitializer()
			{
				uint Data0[4] = {0x00000000, 0x80000000, 0x00000000, 0x80000000}; // +-+-
				memcpy((void*)&Sign_PNPN, Data0, sizeof(Data0));

				uint Data1[4] = {0x80000000, 0x00000000, 0x80000000, 0x00000000};
				memcpy((void*)&Sign_NPNP, Data1, sizeof(Data1));

				//uint Data2[4] = {0x80000000, 0x80000000, 0x80000000, 0x80000000};
				//memcpy((void*)&_MASKSIGN_, Data2, sizeof(Data2));


				float Data3[4] = {1.0f, 0.0f, 0.0f, 1.0f};
				memcpy((void*)&_ZERONE_, Data3, sizeof(Data3));

				uint Data4[4] = {0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0x00000000};
				memcpy((void*)&_0FFF_, Data4, sizeof(Data4));

			}

		} Initializer;
#endif

		const Matrix Matrix::Identity =
		{
			1, 0, 0, 0,
			0, 1, 0, 0,
			0, 0, 1, 0,
			0, 0, 0, 1
		};

		float Matrix::Inverse()
		{
		#if APOC3D_MATH_IMPL == APOC3D_SSE
			__m128 Va,Vb,Vc;
			__m128 r1,r2,r3,tt,tt2;
			__m128 sum,Det,RDet;
			Matrix Minterms;
			__m128 trns0,trns1,trns2,trns3;

			// Calculating the minterms for the first line.

			// _mm_ror_ps is just a macro using _mm_shuffle_ps().
			tt = Row4; tt2 = _mm_ror_ps(Row3,1); 
			Vc = _mm_mul_ps(tt2,_mm_ror_ps(tt,0));					// V3'·V4
			Va = _mm_mul_ps(tt2,_mm_ror_ps(tt,2));					// V3'·V4"
			Vb = _mm_mul_ps(tt2,_mm_ror_ps(tt,3));					// V3'·V4^

			r1 = _mm_sub_ps(_mm_ror_ps(Va,1),_mm_ror_ps(Vc,2));		// V3"·V4^ - V3^·V4"
			r2 = _mm_sub_ps(_mm_ror_ps(Vb,2),_mm_ror_ps(Vb,0));		// V3^·V4' - V3'·V4^
			r3 = _mm_sub_ps(_mm_ror_ps(Va,0),_mm_ror_ps(Vc,1));		// V3'·V4" - V3"·V4'

			tt = Row2;
			Va = _mm_ror_ps(tt,1);		sum = _mm_mul_ps(Va,r1);
			Vb = _mm_ror_ps(tt,2);		sum = _mm_add_ps(sum,_mm_mul_ps(Vb,r2));
			Vc = _mm_ror_ps(tt,3);		sum = _mm_add_ps(sum,_mm_mul_ps(Vc,r3));

			// Calculating the determinant.
			Det = _mm_mul_ps(sum,Row1);
			Det = _mm_add_ps(Det,_mm_movehl_ps(Det,Det));

			Minterms.Row1 = _mm_xor_ps(sum, Sign_PNPN);

			// Calculating the minterms of the second line (using previous results).
			tt = _mm_ror_ps(Row1,1);		sum = _mm_mul_ps(tt,r1);
			tt = _mm_ror_ps(tt,1);		sum = _mm_add_ps(sum,_mm_mul_ps(tt,r2));
			tt = _mm_ror_ps(tt,1);		sum = _mm_add_ps(sum,_mm_mul_ps(tt,r3));
			Minterms.Row2 = _mm_xor_ps(sum, Sign_NPNP);

			// Testing the determinant.
			Det = _mm_sub_ss(Det,_mm_shuffle_ps(Det,Det,1));
		#ifdef ZERO_SINGULAR
			int flag = _mm_comieq_ss(Det,_mm_sub_ss(tt,tt));
			// Using _mm_sub_ss, as only the first element has to be zeroed.
		#endif

			// Calculating the minterms of the third line.
			tt = _mm_ror_ps(Row1,1);
			Va = _mm_mul_ps(tt,Vb);									// V1'·V2"
			Vb = _mm_mul_ps(tt,Vc);									// V1'·V2^
			Vc = _mm_mul_ps(tt,Row2);								// V1'·V2

			r1 = _mm_sub_ps(_mm_ror_ps(Va,1),_mm_ror_ps(Vc,2));		// V1"·V2^ - V1^·V2"
			r2 = _mm_sub_ps(_mm_ror_ps(Vb,2),_mm_ror_ps(Vb,0));		// V1^·V2' - V1'·V2^
			r3 = _mm_sub_ps(_mm_ror_ps(Va,0),_mm_ror_ps(Vc,1));		// V1'·V2" - V1"·V2'

			tt = _mm_ror_ps(Row4,1);		sum = _mm_mul_ps(tt,r1);
			tt = _mm_ror_ps(tt,1);		sum = _mm_add_ps(sum,_mm_mul_ps(tt,r2));
			tt = _mm_ror_ps(tt,1);		sum = _mm_add_ps(sum,_mm_mul_ps(tt,r3));
			Minterms.Row3 = _mm_xor_ps(sum,Sign_PNPN);

			// Dividing is FASTER than rcp_nr! (Because rcp_nr causes many register-memory RWs).
			RDet = _mm_div_ss(_mm_load_ss((float *)&_ZERONE_), Det);
			RDet = _mm_shuffle_ps(RDet,RDet,0x00);

			// Devide the first 12 minterms with the determinant.
			Minterms.Row1 = _mm_mul_ps(Minterms.Row1, RDet);
			Minterms.Row2 = _mm_mul_ps(Minterms.Row2, RDet);
			Minterms.Row3 = _mm_mul_ps(Minterms.Row3, RDet);

			// Calculate the minterms of the forth line and devide by the determinant.
			tt = _mm_ror_ps(Row3,1);		sum = _mm_mul_ps(tt,r1);
			tt = _mm_ror_ps(tt,1);		sum = _mm_add_ps(sum,_mm_mul_ps(tt,r2));
			tt = _mm_ror_ps(tt,1);		sum = _mm_add_ps(sum,_mm_mul_ps(tt,r3));
			Minterms.Row4 = _mm_xor_ps(sum,Sign_NPNP);
			Minterms.Row4 = _mm_mul_ps(Minterms.Row4, RDet);

		#ifdef ZERO_SINGULAR
			// Check if the matrix is inversable.
			// Uses a delayed branch here, so the test would not interfere the calculations.
			// Assuming most of the matrices are inversable, the previous calculations are 
			// not wasted. It is faster this way.
			if (flag) {
				ZeroMatrix();
				return 0.0f;
			}
		#endif

			// Now we just have to transpose the minterms matrix.
			trns0 = _mm_unpacklo_ps(Minterms.Row1,Minterms.Row2);
			trns1 = _mm_unpacklo_ps(Minterms.Row3,Minterms.Row4);
			trns2 = _mm_unpackhi_ps(Minterms.Row1,Minterms.Row2);
			trns3 = _mm_unpackhi_ps(Minterms.Row3,Minterms.Row4);
			Row1 = _mm_movelh_ps(trns0,trns1);
			Row2 = _mm_movehl_ps(trns1,trns0);
			Row3 = _mm_movelh_ps(trns2,trns3);
			Row4 = _mm_movehl_ps(trns3,trns2);

			// That's all folks!
			return *(float *)&Det;	// Det[0]
		#elif APOC3D_MATH_IMPL == APOC3D_DEFAULT
			float m11 = M11;
            float m12 = M12;
            float m13 = M13;
            float m14 = M14;
            float m21 = M21;
            float m22 = M22;
            float m23 = M23;
            float m24 = M24;
            float m31 = M31;
            float m32 = M32;
            float m33 = M33;
            float m34 = M34;
            float m41 = M41;
            float m42 = M42;
            float m43 = M43;
            float m44 = M44;

			float det01 = m11 * m22 - m12 * m21;
			float det02 = m11 * m23 - m13 * m21;
			float det03 = m11 * m24 - m14 * m21;
			float det04 = m12 * m23 - m13 * m22;
			float det05 = m12 * m24 - m14 * m22;
			float det06 = m13 * m24 - m14 * m23;
			float det07 = m31 * m42 - m32 * m41;
			float det08 = m31 * m43 - m33 * m41;
			float det09 = m31 * m44 - m34 * m41;
			float det10 = m32 * m43 - m33 * m42;
			float det11 = m32 * m44 - m34 * m42;
			float det12 = m33 * m44 - m34 * m43;

			float detMatrix = det01 * det12 - det02 * det11 + det03 * det10 + det04 * det09 - det05 * det08 + det06 * det07;
			float invDetMatrix = 1 / detMatrix;

			M11 = (+m22 * det12 - m23 * det11 + m24 * det10) * invDetMatrix;
			M12 = (-m12 * det12 + m13 * det11 - m14 * det10) * invDetMatrix;
			M13 = (+m42 * det06 - m43 * det05 + m44 * det04) * invDetMatrix;
			M14 = (-m32 * det06 + m33 * det05 - m34 * det04) * invDetMatrix;

			M21 = (-m21 * det12 + m23 * det09 - m24 * det08) * invDetMatrix;
			M22 = (+m11 * det12 - m13 * det09 + m14 * det08) * invDetMatrix;
			M23 = (-m41 * det06 + m43 * det03 - m44 * det02) * invDetMatrix;
			M24 = (+m31 * det06 - m33 * det03 + m34 * det02) * invDetMatrix;

			M31 = (+m21 * det11 - m22 * det09 + m24 * det07) * invDetMatrix;
			M32 = (-m11 * det11 + m12 * det09 - m14 * det07) * invDetMatrix;
			M33 = (+m41 * det05 - m42 * det03 + m44 * det01) * invDetMatrix;
			M34 = (-m31 * det05 + m32 * det03 - m34 * det01) * invDetMatrix;

			M41 = (-m21 * det10 + m22 * det08 - m23 * det07) * invDetMatrix;
			M42 = (+m11 * det10 - m12 * det08 + m13 * det07) * invDetMatrix;
			M43 = (-m41 * det04 + m42 * det02 - m43 * det01) * invDetMatrix;
			M44 = (+m31 * det04 - m32 * det02 + m33 * det01) * invDetMatrix;

			return detMatrix;
		#endif
			
		}

		float Matrix::ExtractProjectionNearPlane() const
		{
			if (M34 != 0)
			{
				// perspective

				return -M34 * M43 / M33;
			}

			// ortho
			if (M33 > 0)
				return -M43 / M33; // RH
			return M43 / M33; // LH
		}

		float Matrix::ExtractProjectionFarPlane() const
		{
			if (M34 != 0)
			{
				// perspective

				if (M34 < 0)
					return -M43 * M34 / (M33 + 1);	// RH
				return -M43 * M34 / (M33 - 1);	// LH
			}

			// ortho
			if (M33 > 0)
				return (1 - M43) / M33; // LH
			return (M43 - 1) / M33;  // RH
		}

		void Matrix::ReAdjustProjectionNearFarPlane(float near, float far)
		{
			Matrix temp;
			if (M34 != 0)
			{
				// perspective

				if (M34 < 0)
					Matrix::CreatePerspectiveFovRH(temp, 1, 1, near, far);		// RH
				else
					Matrix::CreatePerspectiveFovLH(temp, 1, 1, near, far);		// RH
			}
			else
			{
				if (M33 > 0)
					Matrix::CreateOrthoLH(temp, 1, 1, near, far);
				else
					Matrix::CreateOrthoRH(temp, 1, 1, near, far);
			}

			M33 = temp.M33;
			M43 = temp.M43;
		}

		void Matrix::CreateBillboard(const Vector3 &objectPosition, const Vector3 &cameraPosition, const Vector3 &cameraUpVector, const Vector3 &cameraForwardVector, Matrix& res)
		{
		#if APOC3D_MATH_IMPL == APOC3D_SSE
			Vector3 difference = SIMDVecSub( objectPosition , cameraPosition);
			Vector3 crossed;
			Vector3 final;

			float len =  SIMDVec3Length(difference);
			if (len < 0.0001f)				
				difference = SIMDVecNegate(cameraForwardVector);
			else
				difference = SIMDVecDiv(difference, len);

			crossed = SIMDVec3Cross(cameraUpVector, difference);
			crossed = SIMDVec3Normalize(crossed);

			final = SIMDVec3Cross(difference,crossed);
			
			
			res.Row1 = final;
			res.Row2 = crossed;
			res.Row3 = difference;
			res.Row4 = objectPosition;

			__asm 
			{	
				mov 	eax, res 				
				fldz	
				fst		float ptr [eax+ELEM_ADDR(1,4)]	
				fst		float ptr [eax+ELEM_ADDR(2,4)]	
				fstp	float ptr [eax+ELEM_ADDR(3,4)]	
				fld1	
				fstp	float ptr [eax+ELEM_ADDR(4,4)]	
			}
		#elif APOC3D_MATH_IMPL == APOC3D_DEFAULT
			Vector3 difference = objectPosition - cameraPosition;
            Vector3 crossed;
            Vector3 final;

            float lengthSq = difference.LengthSquared();
            if (lengthSq < 0.0001f)
                difference = -cameraForwardVector;
            else
                difference /= sqrtf(lengthSq);

            crossed = Vector3::Cross(cameraUpVector, difference);
            crossed.NormalizeInPlace();
            final = Vector3::Cross(difference, crossed);

            res.M11 = final.X;
            res.M12 = final.Y;
            res.M13 = final.Z;
            res.M14 = 0;

            res.M21 = crossed.X;
            res.M22 = crossed.Y;
            res.M23 = crossed.Z;
			res.M24 = 0;

            res.M31 = difference.X;
            res.M32 = difference.Y;
            res.M33 = difference.Z;
			res.M34 = 0;

            res.M41 = objectPosition.X;
            res.M42 = objectPosition.Y;
            res.M43 = objectPosition.Z;
            res.M44 = 1;
		#endif
		}

		void Matrix::CreateRotationQuaternion(Matrix& result, const Quaternion& rotation)
		{
			float xx = rotation.X * rotation.X;
			float yy = rotation.Y * rotation.Y;
			float zz = rotation.Z * rotation.Z;
			float xy = rotation.X * rotation.Y;
			float zw = rotation.Z * rotation.W;
			float zx = rotation.Z * rotation.X;
			float yw = rotation.Y * rotation.W;
			float yz = rotation.Y * rotation.Z;
			float xw = rotation.X * rotation.W;

			result.M11 = 1 - 2 * (yy + zz);
			result.M12 = 2 * (xy + zw);
			result.M13 = 2 * (zx - yw);
			result.M14 = 0;

			result.M21 = 2 * (xy - zw);
			result.M22 = 1 - 2 * (zz + xx);
			result.M23 = 2 * (yz + xw);
			result.M24 = 0;

			result.M31 = 2 * (zx + yw);
			result.M32 = 2 * (yz - xw);
			result.M33 = 1 - 2 * (yy + xx);
			result.M34 = 0;

			result.M41 = 0;
			result.M42 = 0;
			result.M43 = 0;
			result.M44 = 1;
			
		}

		void Matrix::CreateRotationYawPitchRoll(Matrix& result, float yaw, float pitch, float roll)
		{
			Quaternion quaternion;
			Quaternion::CreateRotationYawPitchRoll(quaternion, yaw, pitch, roll);
			CreateRotationQuaternion(result, quaternion);
		}
	}
}