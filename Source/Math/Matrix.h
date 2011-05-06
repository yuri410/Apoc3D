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
#ifndef MATRIX_H
#define MATRIX_H

#include "Common.h"

#pragma pack(push,16)

#define _mm_ror_ps(vec,i)	\
	(((i)%4) ? (_mm_shuffle_ps(vec,vec, _MM_SHUFFLE((unsigned char)(i+3)%4,(unsigned char)(i+2)%4,(unsigned char)(i+1)%4,(unsigned char)(i+0)%4))) : (vec))
#define _mm_rol_ps(vec,i)	\
	(((i)%4) ? (_mm_shuffle_ps(vec,vec, _MM_SHUFFLE((unsigned char)(7-i)%4,(unsigned char)(6-i)%4,(unsigned char)(5-i)%4,(unsigned char)(4-i)%4))) : (vec))

#define _mm_abs_ps(vec)		_mm_andnot_ps(_MASKSIGN_,vec)
#define _mm_neg_ps(vec)		_mm_xor_ps(_MASKSIGN_,vec)


namespace Apoc3D
{
	namespace Math
	{
		
		/* Defines a 4x4 matrix.
		*/
		class Matrix
		{
		private:

			const static __m128 _MASKSIGN_;	// - - - -
			const static __m128 _ZERONE_;		// 1 0 0 1
			const static __m128 _0FFF_;		// 0 * * *
			const static __m128 Sign_PNPN;	// + - + -
			const static __m128 Sign_NPNP;	// - + - +

		public:
			const static Matrix Identity;

			union {
				struct {
					__m128 _L1, _L2, _L3, _L4;
				};
				struct {
					float	M11, M12, M13, M14;
					float	M21, M22, M23, M24;
					float	M31, M32, M33, M34;
					float	M41, M42, M43, M44;
				};
			};

			Matrix(){}
			Matrix(const float* elements)
			{
				M11 = elements[0];
				M12 = elements[1];
				M13 = elements[2];
				M14 = elements[3];
				M21 = elements[4];
				M22 = elements[5];
				M23 = elements[6];
				M24 = elements[7];
				M31 = elements[8];
				M32 = elements[9];
				M33 = elements[10];
				M34 = elements[11];
				M41 = elements[12];
				M42 = elements[13];
				M43 = elements[14];
				M44 = elements[15];

			}


			void LoadIdentity() { memcpy(this, &Identity, sizeof(Identity)); }

			/* Transposes the matrix
			*/
			void Transpose() { _MM_TRANSPOSE4_PS(_L1, _L2, _L3, _L4); }
			/* Inverts the matrix.
			*/
			float Inverse()
			{
				__m128 Va,Vb,Vc;
				__m128 r1,r2,r3,tt,tt2;
				__m128 sum,Det,RDet;
				Matrix Minterms;
				__m128 trns0,trns1,trns2,trns3;

				// Calculating the minterms for the first line.

				// _mm_ror_ps is just a macro using _mm_shuffle_ps().
				tt = _L4; tt2 = _mm_ror_ps(_L3,1); 
				Vc = _mm_mul_ps(tt2,_mm_ror_ps(tt,0));					// V3'·V4
				Va = _mm_mul_ps(tt2,_mm_ror_ps(tt,2));					// V3'·V4"
				Vb = _mm_mul_ps(tt2,_mm_ror_ps(tt,3));					// V3'·V4^

				r1 = _mm_sub_ps(_mm_ror_ps(Va,1),_mm_ror_ps(Vc,2));		// V3"·V4^ - V3^·V4"
				r2 = _mm_sub_ps(_mm_ror_ps(Vb,2),_mm_ror_ps(Vb,0));		// V3^·V4' - V3'·V4^
				r3 = _mm_sub_ps(_mm_ror_ps(Va,0),_mm_ror_ps(Vc,1));		// V3'·V4" - V3"·V4'

				tt = _L2;
				Va = _mm_ror_ps(tt,1);		sum = _mm_mul_ps(Va,r1);
				Vb = _mm_ror_ps(tt,2);		sum = _mm_add_ps(sum,_mm_mul_ps(Vb,r2));
				Vc = _mm_ror_ps(tt,3);		sum = _mm_add_ps(sum,_mm_mul_ps(Vc,r3));

				// Calculating the determinant.
				Det = _mm_mul_ps(sum,_L1);
				Det = _mm_add_ps(Det,_mm_movehl_ps(Det,Det));

				Minterms._L1 = _mm_xor_ps(sum, Sign_PNPN);

				// Calculating the minterms of the second line (using previous results).
				tt = _mm_ror_ps(_L1,1);		sum = _mm_mul_ps(tt,r1);
				tt = _mm_ror_ps(tt,1);		sum = _mm_add_ps(sum,_mm_mul_ps(tt,r2));
				tt = _mm_ror_ps(tt,1);		sum = _mm_add_ps(sum,_mm_mul_ps(tt,r3));
				Minterms._L2 = _mm_xor_ps(sum, Sign_NPNP);

				// Testing the determinant.
				Det = _mm_sub_ss(Det,_mm_shuffle_ps(Det,Det,1));
#ifdef ZERO_SINGULAR
				int flag = _mm_comieq_ss(Det,_mm_sub_ss(tt,tt));
				// Using _mm_sub_ss, as only the first element has to be zeroed.
#endif

				// Calculating the minterms of the third line.
				tt = _mm_ror_ps(_L1,1);
				Va = _mm_mul_ps(tt,Vb);									// V1'·V2"
				Vb = _mm_mul_ps(tt,Vc);									// V1'·V2^
				Vc = _mm_mul_ps(tt,_L2);								// V1'·V2

				r1 = _mm_sub_ps(_mm_ror_ps(Va,1),_mm_ror_ps(Vc,2));		// V1"·V2^ - V1^·V2"
				r2 = _mm_sub_ps(_mm_ror_ps(Vb,2),_mm_ror_ps(Vb,0));		// V1^·V2' - V1'·V2^
				r3 = _mm_sub_ps(_mm_ror_ps(Va,0),_mm_ror_ps(Vc,1));		// V1'·V2" - V1"·V2'

				tt = _mm_ror_ps(_L4,1);		sum = _mm_mul_ps(tt,r1);
				tt = _mm_ror_ps(tt,1);		sum = _mm_add_ps(sum,_mm_mul_ps(tt,r2));
				tt = _mm_ror_ps(tt,1);		sum = _mm_add_ps(sum,_mm_mul_ps(tt,r3));
				Minterms._L3 = _mm_xor_ps(sum,Sign_PNPN);

				// Dividing is FASTER than rcp_nr! (Because rcp_nr causes many register-memory RWs).
				RDet = _mm_div_ss(_mm_load_ss((float *)&_ZERONE_), Det);
				RDet = _mm_shuffle_ps(RDet,RDet,0x00);

				// Devide the first 12 minterms with the determinant.
				Minterms._L1 = _mm_mul_ps(Minterms._L1, RDet);
				Minterms._L2 = _mm_mul_ps(Minterms._L2, RDet);
				Minterms._L3 = _mm_mul_ps(Minterms._L3, RDet);

				// Calculate the minterms of the forth line and devide by the determinant.
				tt = _mm_ror_ps(_L3,1);		sum = _mm_mul_ps(tt,r1);
				tt = _mm_ror_ps(tt,1);		sum = _mm_add_ps(sum,_mm_mul_ps(tt,r2));
				tt = _mm_ror_ps(tt,1);		sum = _mm_add_ps(sum,_mm_mul_ps(tt,r3));
				Minterms._L4 = _mm_xor_ps(sum,Sign_NPNP);
				Minterms._L4 = _mm_mul_ps(Minterms._L4, RDet);

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
				trns0 = _mm_unpacklo_ps(Minterms._L1,Minterms._L2);
				trns1 = _mm_unpacklo_ps(Minterms._L3,Minterms._L4);
				trns2 = _mm_unpackhi_ps(Minterms._L1,Minterms._L2);
				trns3 = _mm_unpackhi_ps(Minterms._L3,Minterms._L4);
				_L1 = _mm_movelh_ps(trns0,trns1);
				_L2 = _mm_movehl_ps(trns1,trns0);
				_L3 = _mm_movelh_ps(trns2,trns3);
				_L4 = _mm_movehl_ps(trns3,trns2);

				// That's all folks!
				return *(float *)&Det;	// Det[0]
			}

			/* Calculates the determinant of the matrix.
			*/
			float Determinant()
			{
				__m128 Va,Vb,Vc;
				__m128 r1,r2,r3,t1,t2,sum;
				__m128 Det;

				// First, Let's calculate the first four minterms of the first line
				t1 = _L4; t2 = _mm_ror_ps(_L3,1); 
				Vc = _mm_mul_ps(t2,_mm_ror_ps(t1,0));					// V3'·V4
				Va = _mm_mul_ps(t2,_mm_ror_ps(t1,2));					// V3'·V4"
				Vb = _mm_mul_ps(t2,_mm_ror_ps(t1,3));					// V3'·V4^

				r1 = _mm_sub_ps(_mm_ror_ps(Va,1),_mm_ror_ps(Vc,2));		// V3"·V4^ - V3^·V4"
				r2 = _mm_sub_ps(_mm_ror_ps(Vb,2),_mm_ror_ps(Vb,0));		// V3^·V4' - V3'·V4^
				r3 = _mm_sub_ps(_mm_ror_ps(Va,0),_mm_ror_ps(Vc,1));		// V3'·V4" - V3"·V4'

				Va = _mm_ror_ps(_L2,1);		sum = _mm_mul_ps(Va,r1);
				Vb = _mm_ror_ps(Va,1);		sum = _mm_add_ps(sum,_mm_mul_ps(Vb,r2));
				Vc = _mm_ror_ps(Vb,1);		sum = _mm_add_ps(sum,_mm_mul_ps(Vc,r3));

				// Now we can calculate the determinant:
				Det = _mm_mul_ps(sum,_L1);
				Det = _mm_add_ps(Det,_mm_movehl_ps(Det,Det));
				Det = _mm_sub_ss(Det,_mm_shuffle_ps(Det,Det,1));
				return Det[0];
			}

			void ZeroMatrix() { _L1 = _L2 = _L3 = _L4 = _mm_setzero_ps(); }

			static void Multiply(Matrix& res, const Matrix& ma, const Matrix& mb)
			{
				__m128 Result;
				__m128 B1 = mb._L1, B2 = mb._L2, B3 = mb._L3, B4 = mb._L4;
				__m128 t0, t1;

				
				t0 = _mm_set_ps1(ma.M11);				
				Result = _mm_mul_ps(t0, B1);

				t0 = _mm_set_ps1(ma.M12);
				t1 = _mm_mul_ps(t0, B2);
				Result = _mm_add_ps(Result, t1);

				t0 = _mm_set_ps1(ma.M13);
				t1 = _mm_mul_ps(t0, B3);
				Result = _mm_add_ps(Result, t1);

				t0 = _mm_set_ps1(ma.M14);
				t1 = _mm_mul_ps(t0, B4);
				Result = _mm_add_ps(Result, t1);

				res._L1 = Result;


				t0 = _mm_set_ps1(ma.M21);
				Result = _mm_mul_ps(t0, B1);

				t0 = _mm_set_ps1(ma.M22);
				t1 = _mm_mul_ps(t0, B2);
				Result = _mm_add_ps(Result, t1);

				t0 = _mm_set_ps1(ma.M23);
				t1 = _mm_mul_ps(t0, B3);
				Result = _mm_add_ps(Result, t1);

				t0 = _mm_set_ps1(ma.M24);
				t1 = _mm_mul_ps(t0, B4);
				Result = _mm_add_ps(Result, t1);

				res._L2 = Result;




				t0 = _mm_set_ps1(ma.M31);
				Result = _mm_mul_ps(t0, B1);

				t0 = _mm_set_ps1(ma.M32);
				t1 = _mm_mul_ps(t0, B2);
				Result = _mm_add_ps(Result, t1);

				t0 = _mm_set_ps1(ma.M33);
				t1 = _mm_mul_ps(t0, B3);
				Result = _mm_add_ps(Result, t1);

				t0 = _mm_set_ps1(ma.M34);
				t1 = _mm_mul_ps(t0, B4);
				Result = _mm_add_ps(Result, t1);

				res._L3 = Result;




				t0 = _mm_set_ps1(ma.M41);
				Result = _mm_mul_ps(t0, B1);

				t0 = _mm_set_ps1(ma.M42);
				t1 = _mm_mul_ps(t0, B2);
				Result = _mm_add_ps(Result, t1);

				t0 = _mm_set_ps1(ma.M43);
				t1 = _mm_mul_ps(t0, B3);
				Result = _mm_add_ps(Result, t1);

				t0 = _mm_set_ps1(ma.M44);
				t1 = _mm_mul_ps(t0, B4);
				Result = _mm_add_ps(Result, t1);

				res._L4 = Result;

			}
			static void Multiply(Matrix& res, const Matrix& ma, float mb)
			{
				__m128 b = _mm_set_ps(mb,mb,mb,mb);
				res._L1 = _mm_mul_ps(ma._L1,b);
				res._L2 = _mm_mul_ps(ma._L2,b);
				res._L3 = _mm_mul_ps(ma._L3,b);
				res._L4 = _mm_mul_ps(ma._L4,b);
			}


			static void Add(Matrix& res, const Matrix& ma, const Matrix& mb)
			{
				res._L1 = _mm_add_ps(ma._L1,  mb._L1);
				res._L2 = _mm_add_ps(ma._L2,  mb._L2);
				res._L3 = _mm_add_ps(ma._L3,  mb._L3);
				res._L4 = _mm_add_ps(ma._L4,  mb._L4);
			}
			static void Subtract(Matrix& res, const Matrix& ma, const Matrix& mb)
			{
				res._L1 = _mm_sub_ps(ma._L1,  mb._L1);
				res._L2 = _mm_sub_ps(ma._L2,  mb._L2);
				res._L3 = _mm_sub_ps(ma._L3,  mb._L3);
				res._L4 = _mm_sub_ps(ma._L4,  mb._L4);
			}

			static void CreateRotationX(Matrix& res, float angle)
			{
				__asm { 
					xorps	xmm0,xmm0
					mov 	eax, res;
					fld		float ptr angle
					movaps	[eax+0x10], xmm0		// clear line _L2
					fsincos
					fst		float ptr [eax+0x14]	// set element _22
					movaps	[eax+0x20], xmm0		// clear line _L3
					fstp	float ptr [eax+0x28]	// set element _33
					fst		float ptr [eax+0x18]	// set element _23
					fchs
					movaps	[eax+0x00], xmm0		// clear line _L1
					fstp	float ptr [eax+0x24]	// set element _32
					fld1
					fst		float ptr [eax+0x00]	// set element _11
					movaps	[eax+0x30], xmm0		// clear line _L4
					fstp	float ptr [eax+0x3C]	// set element _44
				}
			}
			static void CreateRotationY(Matrix& res, float angle)
			{
				__asm { 
					xorps	xmm0,xmm0
					mov 	eax, res
					fld		float ptr angle
					movaps	[eax+0x00], xmm0		// clear line _L1
					fsincos
					fst		float ptr [eax+0x00]	// set element _11
					movaps	[eax+0x20], xmm0		// clear line _L3
					fstp	float ptr [eax+0x28]	// set element _33
					fst		float ptr [eax+0x20]	// set element _31
					fchs
					movaps	[eax+0x10], xmm0		// clear line _L2
					fstp	float ptr [eax+0x08]	// set element _13
					fld1
					fst		float ptr [eax+0x14]	// set element _22
					movaps	[eax+0x30], xmm0		// clear line _L4
					fstp	float ptr [eax+0x3C]	// set element _44
				}
			}
			static void CreateRotationZ(Matrix& res, float angle)
			{
				__asm { 
					xorps	xmm0,xmm0
					mov 	eax, res
					fld		float ptr angle
					movaps	[eax+0x00], xmm0		// clear line _L1
					fsincos
					fst		float ptr [eax+0x00]	// set element _11
					movaps	[eax+0x10], xmm0		// clear line _L2
					fstp	float ptr [eax+0x14]	// set element _22
					fst		float ptr [eax+0x04]	// set element _12
					fchs
					movaps	[eax+0x20], xmm0		// clear line _L3
					fstp	float ptr [eax+0x10]	// set element _21
					fld1
					fst		float ptr [eax+0x28]	// set element _33
					movaps	[eax+0x30], xmm0		// clear line _L4
					fstp	float ptr [eax+0x3C]	// set element _44
				}
			}

			static void CreateTranslation(Matrix& res, float x, float y, float z)
			{
				res.LoadIdentity();
				res.M41 = x;
				res.M42 = y;
				res.M43 = z;
			}
			static void CreateScale(Matrix& res, float x, float y, float z)
			{
				res.LoadIdentity();
				res.M11 = x;
				res.M22 = y;
				res.M33 = z;
			}



		};
	

	}
}

#pragma pack(pop)
#endif