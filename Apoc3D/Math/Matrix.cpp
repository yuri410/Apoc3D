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
#include "apoc3d/ApocException.h"

using namespace Apoc3D::Collections;

namespace Apoc3D
{
	namespace Math
	{
		MatrixStack::MatrixStack(int reserve)
		{
			m_stack = new Stack<Matrix>(reserve);
		}
		MatrixStack::~MatrixStack()
		{
			delete m_stack;
		}

		void MatrixStack::PushMultply(const Matrix& mat) const
		{
			if (m_stack->getCount())
			{
				Matrix newMat;
				Matrix::Multiply(newMat, m_stack->Peek(), mat);
				m_stack->Push(newMat);
			}
			else
			{
				m_stack->Push(mat);
			}
		}
		void MatrixStack::PushMatrix(const Matrix& mat) const
		{
			m_stack->Push(mat);
		}
		bool MatrixStack::PopMatrix() const
		{
			if (m_stack->getCount())
			{
				m_stack->FastPop();
				return true;
			}
			return false;
		}
		bool MatrixStack::PopMatrix(Matrix& mat) const
		{
			if (m_stack->getCount())
			{
				mat = m_stack->Pop();
				return true;
			}
			return false;
		}
		Matrix& MatrixStack::Peek() const
		{
			if (m_stack->getCount())
			{
				return m_stack->Peek();
			}
			throw AP_EXCEPTION(EX_InvalidOperation, L"The stack is empty.");
		}

		int MatrixStack::getCount() const
		{
			return m_stack->getCount();
		}



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
		const Matrix Matrix::Identity = Matrix(1,0,0,0,0,1,0,0,0,0,1,0,0,0,0,1);

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
            float num23 = m33 * m44 - m34 * m43;
            float num22 = m32 * m44 - m34 * m42;
            float num21 = m32 * m43 - m33 * m42;
            float num20 = m31 * m44 - m34 * m41;
            float num19 = m31 * m43 - m33 * m41;
            float num18 = m31 * m42 - m32 * m41;
            float num39 = m22 * num23 - m23 * num22 + m24 * num21;
            float num38 = -(m21 * num23 - m23 * num20 + m24 * num19);
            float num37 = m21 * num22 - m22 * num20 + m24 * num18;
            float num36 = -(m21 * num21 - m22 * num19 + m23 * num18);
            float num = 1.0f / (m11 * num39 + m12 * num38 + m13 * num37 + m14 * num36);
            M11 = num39 * num;
            M21 = num38 * num;
            M31 = num37 * num;
            M41 = num36 * num;
            M12 = -(m12 * num23 - m13 * num22 + m14 * num21) * num;
            M22 = (m11 * num23 - m13 * num20 + m14 * num19) * num;
            M32 = -(m11 * num22 - m12 * num20 + m14 * num18) * num;
            M42 = (m11 * num21 - m12 * num19 + m13 * num18) * num;
            float num35 = m23 * m44 - m24 * m43;
            float num34 = m22 * m44 - m24 * m42;
            float num33 = m22 * m43 - m23 * m42;
            float num32 = m21 * m44 - m24 * m41;
            float num31 = m21 * m43 - m23 * m41;
            float num30 = m21 * m42 - m22 * m41;
            M13 = (m12 * num35 - m13 * num34 + m14 * num33) * num;
            M23 = -(m11 * num35 - m13 * num32 + m14 * num31) * num;
            M33 = (m11 * num34 - m12 * num32 + m14 * num30) * num;
            M43 = -(m11 * num33 - m12 * num31 + m13 * num30) * num;
            float num29 = m23 * m34 - m24 * m33;
            float num28 = m22 * m34 - m24 * m32;
            float num27 = m22 * m33 - m23 * m32;
            float num26 = m21 * m34 - m24 * m31;
            float num25 = m21 * m33 - m23 * m31;
            float num24 = m21 * m32 - m22 * m31;
            M14 = -(m12 * num29 - m13 * num28 + m14 * num27) * num;
            M24 = (m11 * num29 - m13 * num26 + m14 * num25) * num;
            M34 = -(m11 * num28 - m12 * num26 + m14 * num24) * num;
            M44 = (m11 * num27 - m12 * num25 + m13 * num24) * num;
			return 1.0f/num;
		#endif
			
		}



		void Matrix::CreateBillboard(const Vector3 &objectPosition, const Vector3 &cameraPosition, const Vector3 &cameraUpVector, const Vector3 &cameraForwardVector, Matrix& res)
		{
		#if APOC3D_MATH_IMPL == APOC3D_SSE
			Vector3 difference = _VecSub( objectPosition , cameraPosition);
			Vector3 crossed;
			Vector3 final;

			float len =  _Vec3Length(difference);
			if (len < 0.0001f)				
				difference = _VecNegate(cameraForwardVector);
			else
				difference = _VecDiv(difference, len);

			crossed = _Vec3Cross(cameraUpVector, difference);
			crossed = _Vec3Normalize(crossed);

			final = _Vec3Cross(difference,crossed);
			
			
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
			Vector3 difference = Vector3Utils::Subtract(objectPosition , cameraPosition);
            Vector3 crossed;
            Vector3 final;

            float lengthSq = Vector3Utils::LengthSquared( difference);
            if (lengthSq < 0.0001f)
                difference = Vector3Utils::Negate(cameraForwardVector);
            else
                difference = Vector3Utils::Multiply(difference,  1.0f / sqrtf(lengthSq));

            crossed = Vector3Utils::Cross(cameraUpVector, difference);
            crossed = Vector3Utils::Normalize(crossed);
            final = Vector3Utils::Cross(difference, crossed);

            res.M11 = final.X;
            res.M12 = final.Y;
            res.M13 = final.Z;
            res.M14 = 0.0f;
            res.M21 = crossed.X;
            res.M22 = crossed.Y;
            res.M23 = crossed.Z;
            res.M24 = 0.0f;
            res.M31 = difference.X;
            res.M32 = difference.Y;
            res.M33 = difference.Z;
            res.M34 = 0.0f;
            res.M41 = objectPosition.X;
            res.M42 = objectPosition.Y;
            res.M43 = objectPosition.Z;
            res.M44 = 1.0f;
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
			result.M11 = 1.0f - (2.0f * (yy + zz));
			result.M12 = 2.0f * (xy + zw);
			result.M13 = 2.0f * (zx - yw);

			result.M21 = 2.0f * (xy - zw);
			result.M22 = 1.0f - (2.0f * (zz + xx));
			result.M23 = 2.0f * (yz + xw);
			
			result.M31 = 2.0f * (zx + yw);
			result.M32 = 2.0f * (yz - xw);
			result.M33 = 1.0f - (2.0f * (yy + xx));


			result.M41 = result.M42 = result.M43 =
			result.M34 = result.M14 = result.M24 = 0.0f;

			result.M44 = 1.0f;
			
		}

		void Matrix::CreateRotationYawPitchRoll(Matrix& result, float yaw, float pitch, float roll)
		{
			Quaternion quaternion;
			Quaternion::CreateRotationYawPitchRoll(quaternion, yaw, pitch, roll);
			CreateRotationQuaternion(result, quaternion);
		}
	}
}