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
#include "Matrix.h"


namespace Apoc3D
{
	namespace Math
	{
		//const __m128 _MASKSIGN_;	// - - - -
		const __m128 _ZERONE_;	// 1 0 0 1
		const __m128 _0FFF_;		// 0 * * *
		const __m128 Sign_PNPN;	// + - + -
		const __m128 Sign_NPNP;

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

		const Matrix Matrix::Identity = Matrix(1,0,0,0,0,1,0,0,0,0,1,0,0,0,0,1);

		float Matrix::Inverse()
		{
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
		}



		void Matrix::CreateBillboard(const Vector3 &objectPosition, const Vector3 &cameraPosition, const Vector3 &cameraUpVector, const Vector3 &cameraForwardVector, Matrix& res)
		{
			Vector3 difference = VecSub( objectPosition , cameraPosition);
			Vector3 crossed;
			Vector3 final;

			float len =  Vec3Length(difference);
			if (len < 0.0001f)				
				difference = VecNegate(cameraForwardVector);
			else
				difference = VecDiv(difference, len);

			crossed = Vec3Cross(cameraUpVector, difference);
			crossed = Vec3Normalize(crossed);

			final = Vec3Cross(difference,crossed);
			
			
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
		}
	}
}