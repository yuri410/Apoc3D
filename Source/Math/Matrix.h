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

#include "Vector.h"

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
#define COL_INDEX(x) (x-1)
#define ELEM_ADDR(i,j) ((i-1)*4*4 + (j-1)*4)

#pragma pack(push, 16)
		/* Defines a 4x4 matrix.
		*/
		class APAPI Matrix
		{
		private:
			

		public:
			const static Matrix Identity;

			__m128 Row1;
			__m128 Row2;
			__m128 Row3;
			__m128 Row4;
				
			Matrix() {  ZeroMatrix(); }
			Matrix(const float elements[16])
			{				
				Row1 = _mm_load_ps(&elements[0]);
				//_mm_shuffle_ps(Row1, Row1, 
					//_MM_SHUFFLE(COL_INDEX(4),COL_INDEX(3),COL_INDEX(2),COL_INDEX(1)));

				Row2 = _mm_load_ps(&elements[4]);
				//_mm_shuffle_ps(Row2, Row2, 
					//_MM_SHUFFLE(COL_INDEX(4),COL_INDEX(3),COL_INDEX(2),COL_INDEX(1)));

				Row3 = _mm_load_ps(&elements[8]);
				//_mm_shuffle_ps(Row3, Row3, 
					//_MM_SHUFFLE(COL_INDEX(4),COL_INDEX(3),COL_INDEX(2),COL_INDEX(1)));

				Row4 = _mm_load_ps(&elements[12]);
				//_mm_shuffle_ps(Row4, Row4, 
					//_MM_SHUFFLE(COL_INDEX(4),COL_INDEX(3),COL_INDEX(2),COL_INDEX(1)));
			}
			Matrix(const Matrix &m) : Row1(m.Row1), Row2(m.Row2), Row3(m.Row3), Row4(m.Row4) {}
			Matrix(float f11, float f12, float f13, float f14,
				float f21, float f22, float f23, float f24,
				float f31, float f32, float f33, float f34,
				float f41, float f42, float f43, float f44)
			{
				__asm
				{
					mov		eax, this
					fld		float ptr f11
					fstp	float ptr [eax+ELEM_ADDR(1,1)]
					fld		float ptr f12;
					fstp	float ptr [eax+ELEM_ADDR(1,2)]
					fld		float ptr f13;
					fstp	float ptr [eax+ELEM_ADDR(1,3)]
					fld		float ptr f14;
					fstp	float ptr [eax+ELEM_ADDR(1,4)]

					fld		float ptr f21;
					fstp	float ptr [eax+ELEM_ADDR(2,1)]
					fld		float ptr f22;
					fstp	float ptr [eax+ELEM_ADDR(2,2)]
					fld		float ptr f23;
					fstp	float ptr [eax+ELEM_ADDR(2,3)]
					fld		float ptr f24;
					fstp	float ptr [eax+ELEM_ADDR(2,4)]

					fld		float ptr f31;
					fstp	float ptr [eax+ELEM_ADDR(3,1)]
					fld		float ptr f32;
					fstp	float ptr [eax+ELEM_ADDR(3,2)]
					fld		float ptr f33;
					fstp	float ptr [eax+ELEM_ADDR(3,3)]
					fld		float ptr f34;
					fstp	float ptr [eax+ELEM_ADDR(3,4)]

					fld		float ptr f41;
					fstp	float ptr [eax+ELEM_ADDR(4,1)]
					fld		float ptr f42;
					fstp	float ptr [eax+ELEM_ADDR(4,2)]
					fld		float ptr f43;
					fstp	float ptr [eax+ELEM_ADDR(4,3)]
					fld		float ptr f44;
					fstp	float ptr [eax+ELEM_ADDR(4,4)]
				}
			}


			void LoadIdentity()
			{
				Row1 = Identity.Row1;
				Row2 = Identity.Row2;
				Row3 = Identity.Row3;
				Row4 = Identity.Row4;
			}

			/* Transposes the matrix
			*/
			void Transpose() { _MM_TRANSPOSE4_PS(Row1, Row2, Row3, Row4); }
			/* Inverts the matrix.
			*/
			float Inverse(); // TODO: Rewrite
			

			/* Calculates the determinant of the matrix.
			*/
			float Determinant() const// TODO: Rewrite
			{
				__m128 Va,Vb,Vc;
				__m128 r1,r2,r3,t1,t2,sum;
				__m128 Det;

				// First, Let's calculate the first four minterms of the first line
				t1 = Row4; t2 = _mm_ror_ps(Row3,1); 
				Vc = _mm_mul_ps(t2,_mm_ror_ps(t1,0));					// V3'·V4
				Va = _mm_mul_ps(t2,_mm_ror_ps(t1,2));					// V3'·V4"
				Vb = _mm_mul_ps(t2,_mm_ror_ps(t1,3));					// V3'·V4^

				r1 = _mm_sub_ps(_mm_ror_ps(Va,1),_mm_ror_ps(Vc,2));		// V3"·V4^ - V3^·V4"
				r2 = _mm_sub_ps(_mm_ror_ps(Vb,2),_mm_ror_ps(Vb,0));		// V3^·V4' - V3'·V4^
				r3 = _mm_sub_ps(_mm_ror_ps(Va,0),_mm_ror_ps(Vc,1));		// V3'·V4" - V3"·V4'

				Va = _mm_ror_ps(Row2,1);		sum = _mm_mul_ps(Va,r1);
				Vb = _mm_ror_ps(Va,1);		sum = _mm_add_ps(sum,_mm_mul_ps(Vb,r2));
				Vc = _mm_ror_ps(Vb,1);		sum = _mm_add_ps(sum,_mm_mul_ps(Vc,r3));

				// Now we can calculate the determinant:
				Det = _mm_mul_ps(sum,Row1);
				Det = _mm_add_ps(Det,_mm_movehl_ps(Det,Det));
				Det = _mm_sub_ss(Det,_mm_shuffle_ps(Det,Det,1));

				float result;
				_mm_store_ss(&result, Det);
				return result;
			}

			void ZeroMatrix() { Row1 = Row2 = Row3 = Row4 = _mm_setzero_ps(); }

			/* Determines the product of two matrices.
			*/
			static void Multiply(Matrix& res, const Matrix& ma, const Matrix& mb)
			{
				__m128 Result;
				__m128 B1 = mb.Row1, B2 = mb.Row2, B3 = mb.Row3, B4 = mb.Row4;
				__m128 t0, t1;


				t0 = _mm_shuffle_ps(ma.Row1,ma.Row1, _MM_SHUFFLE1(COL_INDEX(1)));// m11
				Result = _mm_mul_ps(t0, B1);

				t0 = _mm_shuffle_ps(ma.Row1,ma.Row1, _MM_SHUFFLE1(COL_INDEX(2)));// m12
				t1 = _mm_mul_ps(t0, B2);
				Result = _mm_add_ps(Result, t1);

				t0 = _mm_shuffle_ps(ma.Row1,ma.Row1, _MM_SHUFFLE1(COL_INDEX(3)));// m13
				t1 = _mm_mul_ps(t0, B3);
				Result = _mm_add_ps(Result, t1);

				t0 = _mm_shuffle_ps(ma.Row1,ma.Row1, _MM_SHUFFLE1(COL_INDEX(4)));// m14
				t1 = _mm_mul_ps(t0, B4);
				Result = _mm_add_ps(Result, t1);

				res.Row1 = Result;

				//_mm_store_ps(buffer, ma.Row2);
				t0 = _mm_shuffle_ps(ma.Row2,ma.Row2, _MM_SHUFFLE1(COL_INDEX(1)));// m21
				Result = _mm_mul_ps(t0, B1);

				t0 = _mm_shuffle_ps(ma.Row2,ma.Row2, _MM_SHUFFLE1(COL_INDEX(2)));// m21
				t1 = _mm_mul_ps(t0, B2);
				Result = _mm_add_ps(Result, t1);

				t0 = _mm_shuffle_ps(ma.Row2,ma.Row2, _MM_SHUFFLE1(COL_INDEX(3)));// m21
				t1 = _mm_mul_ps(t0, B3);
				Result = _mm_add_ps(Result, t1);

				t0 = _mm_shuffle_ps(ma.Row2,ma.Row2, _MM_SHUFFLE1(COL_INDEX(4)));// m21
				t1 = _mm_mul_ps(t0, B4);
				Result = _mm_add_ps(Result, t1);

				res.Row2 = Result;



				//_mm_store_ps(buffer, ma.Row3);
				t0 = _mm_shuffle_ps(ma.Row3,ma.Row3, _MM_SHUFFLE1(COL_INDEX(1)));// M31
				Result = _mm_mul_ps(t0, B1);

				t0 = _mm_shuffle_ps(ma.Row3,ma.Row3, _MM_SHUFFLE1(COL_INDEX(2)));// M32
				t1 = _mm_mul_ps(t0, B2);
				Result = _mm_add_ps(Result, t1);

				t0 = _mm_shuffle_ps(ma.Row3,ma.Row3, _MM_SHUFFLE1(COL_INDEX(3)));// M33
				t1 = _mm_mul_ps(t0, B3);
				Result = _mm_add_ps(Result, t1);

				t0 = _mm_shuffle_ps(ma.Row3,ma.Row3, _MM_SHUFFLE1(COL_INDEX(4)));// M34
				t1 = _mm_mul_ps(t0, B4);
				Result = _mm_add_ps(Result, t1);

				res.Row3 = Result;



				//_mm_store_ps(buffer, ma.Row4);
				t0 = _mm_shuffle_ps(ma.Row4,ma.Row4, _MM_SHUFFLE1(COL_INDEX(1)));// M41
				Result = _mm_mul_ps(t0, B1);

				t0 = _mm_shuffle_ps(ma.Row4,ma.Row4, _MM_SHUFFLE1(COL_INDEX(2)));// M42
				t1 = _mm_mul_ps(t0, B2);
				Result = _mm_add_ps(Result, t1);

				t0 = _mm_shuffle_ps(ma.Row4,ma.Row4, _MM_SHUFFLE1(COL_INDEX(3)));// M43
				t1 = _mm_mul_ps(t0, B3);
				Result = _mm_add_ps(Result, t1);

				t0 = _mm_shuffle_ps(ma.Row4,ma.Row4, _MM_SHUFFLE1(COL_INDEX(4)));// M44
				t1 = _mm_mul_ps(t0, B4);
				Result = _mm_add_ps(Result, t1);

				res.Row4 = Result;

			}

			/* Scales a matrix by the given value.
			*/
			static void Multiply(Matrix& res, const Matrix& ma, float mb)
			{
				__m128 b = _mm_set1_ps(mb);
				res.Row1 = _mm_mul_ps(ma.Row1,b);
				res.Row2 = _mm_mul_ps(ma.Row2,b);
				res.Row3 = _mm_mul_ps(ma.Row3,b);
				res.Row4 = _mm_mul_ps(ma.Row4,b);
			}

			/* Determines the sum of two matrices.
			*/
			static void Add(Matrix& res, const Matrix& ma, const Matrix& mb)
			{
				res.Row1 = _mm_add_ps(ma.Row1,  mb.Row1);
				res.Row2 = _mm_add_ps(ma.Row2,  mb.Row2);
				res.Row3 = _mm_add_ps(ma.Row3,  mb.Row3);
				res.Row4 = _mm_add_ps(ma.Row4,  mb.Row4);
			}

			/* Determines the difference between two matrices.
			*/
			static void Subtract(Matrix& res, const Matrix& ma, const Matrix& mb)
			{
				res.Row1 = _mm_sub_ps(ma.Row1,  mb.Row1);
				res.Row2 = _mm_sub_ps(ma.Row2,  mb.Row2);
				res.Row3 = _mm_sub_ps(ma.Row3,  mb.Row3);
				res.Row4 = _mm_sub_ps(ma.Row4,  mb.Row4);
			}

			/* Creates a matrix that rotates around the x-axis.
			*/
			static void CreateRotationX(Matrix& res, float angle)
			{
				__asm
				{
					xorps	xmm0,xmm0
					mov 	eax, res;
					fld		float ptr angle
					movaps	[eax+ELEM_ADDR(2,1)], xmm0		// clear line _L2
					fsincos
					fst		float ptr [eax+ELEM_ADDR(2,2)]	// set element _22
					movaps	[eax+ELEM_ADDR(3,1)], xmm0		// clear line _L3
					fstp	float ptr [eax+ELEM_ADDR(3,3)]	// set element _33
					fst		float ptr [eax+ELEM_ADDR(2,3)]	// set element _23
					fchs
					movaps	[eax+ELEM_ADDR(1,1)], xmm0		// clear line _L1
					fstp	float ptr [eax+ELEM_ADDR(3,2)]	// set element _32
					fld1
					fst		float ptr [eax+ELEM_ADDR(1,1)]	// set element _11
					movaps	[eax+ELEM_ADDR(4,1)], xmm0		// clear line _L4
					fstp	float ptr [eax+ELEM_ADDR(4,4)]	// set element _44
				}
			}
			/* Creates a matrix that rotates around the y-axis.
			*/
			static void CreateRotationY(Matrix& res, float angle)
			{
				__asm
				{ 
					xorps	xmm0,xmm0
					mov 	eax, res
					fld		float ptr angle
					movaps	[eax+ELEM_ADDR(1,1)], xmm0		// clear line _L1
					fsincos
					fst		float ptr [eax+ELEM_ADDR(1,1)]	// set element _11
					movaps	[eax+ELEM_ADDR(3,1)], xmm0		// clear line _L3
					fstp	float ptr [eax+ELEM_ADDR(3,3)]	// set element _33
					fst		float ptr [eax+ELEM_ADDR(3,1)]	// set element _31
					fchs
					movaps	[eax+ELEM_ADDR(2,1)], xmm0		// clear line _L2
					fstp	float ptr [eax+ELEM_ADDR(1,3)]	// set element _13
					fld1
					fst		float ptr [eax+ELEM_ADDR(2,2)]	// set element _22
					movaps	[eax+ELEM_ADDR(4,1)], xmm0		// clear line _L4
					fstp	float ptr [eax+ELEM_ADDR(4,4)]	// set element _44
				}
			}
			/* Creates a matrix that rotates around the z-axis.
			*/
			static void CreateRotationZ(Matrix& res, float angle)
			{
				__asm { 
					xorps	xmm0,xmm0
					mov 	eax, res
					fld		float ptr angle
					movaps	[eax+ELEM_ADDR(1,1)], xmm0		// clear line _L1
					fsincos
					fst		float ptr [eax+ELEM_ADDR(1,1)]	// set element _11
					movaps	[eax+ELEM_ADDR(2,1)], xmm0		// clear line _L2
					fstp	float ptr [eax+ELEM_ADDR(2,2)]	// set element _22
					fst		float ptr [eax+ELEM_ADDR(1,2)]	// set element _12
					fchs
					movaps	[eax+ELEM_ADDR(3,1)], xmm0		// clear line _L3
					fstp	float ptr [eax+ELEM_ADDR(2,1)]	// set element _21
					fld1
					fst		float ptr [eax+ELEM_ADDR(3,3)]	// set element _33
					movaps	[eax+ELEM_ADDR(4,1)], xmm0		// clear line _L4
					fstp	float ptr [eax+ELEM_ADDR(4,4)]	// set element _44
				}
			}

			/* Creates a matrix that rotates around an arbitary axis.
			*/
			static void CreateRotationAxis(Matrix& res, Vector3 axis, float angle)
			{
				Vector3 dotAxis = Vec3Dot2(axis,axis);
				Vector3 t0 = _mm_shuffle_ps(axis,axis, _MM_SHUFFLE(VEC_INDEX_X, VEC_INDEX_X, VEC_INDEX_Y, 0));
				Vector3 t1 = _mm_shuffle_ps(axis,axis, _MM_SHUFFLE(VEC_INDEX_Y, VEC_INDEX_Z, VEC_INDEX_Z, 0));
				Vector3 dotAxis2 = Vec3Dot2(t0,t1);

				float sins, coss;
				__asm
				{
					fld		float ptr angle
					fsincos
					fstp		float ptr coss
					fstp		float ptr sins
				}

				Vector3 sin = VecLoad(sins);
				Vector3 cos = VecLoad(coss);
				Vector3 one = VecLoad(1);

				Vector3 r0 = VecSub(one, dotAxis);
				r0 = VecMul(cos, r0);
				r0 = VecAdd(dotAxis, r0);

				Vector3 r1left = _mm_shuffle_ps(dotAxis2,dotAxis2,_MM_SHUFFLE(VEC_INDEX_X, VEC_INDEX_Z, VEC_INDEX_Y, 0));
				Vector3 r1right = _mm_shuffle_ps(dotAxis2,dotAxis2,_MM_SHUFFLE(VEC_INDEX_Z, VEC_INDEX_X, VEC_INDEX_Y, 0));
				
				Vector3 tmp = VecMul(cos, r1left);
				r1left = VecSub(r1left, tmp);
				r1right = VecMul(sin, r1right);
				Vector3 r1 = VecAdd(r1left, r1right);


				
				r1left = _mm_shuffle_ps(dotAxis2,dotAxis2,_MM_SHUFFLE(VEC_INDEX_Y, VEC_INDEX_X, VEC_INDEX_Z, 0));
				r1right = _mm_shuffle_ps(dotAxis2,dotAxis2,_MM_SHUFFLE(VEC_INDEX_Y, VEC_INDEX_Z, VEC_INDEX_X, 0));
				tmp = VecMul(cos, r1left);
				r1left = VecSub(r1left, tmp);
				r1right = VecMul(sin, r1right);
				Vector3 r2 = VecSub(r1left, r1right);

				
				__asm { 
					xorps	xmm0,xmm0
					mov 	eax, res
					lea		esi, r0
					fld		float ptr [esi+VEC_ADDR_X]
					movaps	[eax+0x00], xmm0				// clear line _L1					
					fstp	float ptr [eax+ELEM_ADDR(1,1)]	// set element _11

					fld		float ptr [esi+VEC_ADDR_Y]
					movaps	[eax+0x10], xmm0				// clear line _L2					
					fstp	float ptr [eax+ELEM_ADDR(2,2)]	// set element _22

					fld		float ptr [esi+VEC_ADDR_Z]
					movaps	[eax+0x20], xmm0				// clear line _L3					
					fst		float ptr [eax+ELEM_ADDR(3,3)]	// set element _33

					fld1
					movaps	[eax+0x30], xmm0				// clear line _L4					
					fstp	float ptr [eax+ELEM_ADDR(4,4)]	// set element _44
				
				
					lea		esi, r1
					fld		float ptr [esi+VEC_ADDR_X]
					fstp	float ptr [eax+ELEM_ADDR(1,2)]	// set element _12
					fld		float ptr [esi+VEC_ADDR_Y]
					fstp	float ptr [eax+ELEM_ADDR(2,3)]	// set element _23
					fld		float ptr [esi+VEC_ADDR_Z]
					fstp	float ptr [eax+ELEM_ADDR(3,1)]	// set element _31
		
					lea		esi, r2
					fld		float ptr [esi+VEC_ADDR_X]
					fstp	float ptr [eax+ELEM_ADDR(1,3)]	// set element _13
					fld		float ptr [esi+VEC_ADDR_Y]
					fstp	float ptr [eax+ELEM_ADDR(2,1)]	// set element _21
					fld		float ptr [esi+VEC_ADDR_Z]
					fstp	float ptr [eax+ELEM_ADDR(3,2)]	// set element _32
				}

			}




			/* Creates a translation matrix using the specified offsets.
			*/
			static void CreateTranslation(Matrix& res, float x, float y, float z)
			{
				__asm
				{ 
					xorps	xmm0,xmm0
					mov 	eax, res					
					fld1
					movaps	[eax+ELEM_ADDR(1,1)], xmm0		// clear line _L1
					fst		float ptr [eax+ELEM_ADDR(1,1)]	// set element _11
					movaps	[eax+ELEM_ADDR(2,1)], xmm0		// clear line _L2
					fst		float ptr [eax+ELEM_ADDR(2,2)]	// set element _22
					movaps	[eax+ELEM_ADDR(3,1)], xmm0		// clear line _L3
					fst		float ptr [eax+ELEM_ADDR(3,3)]	// set element _33
					fstp	float ptr [eax+ELEM_ADDR(4,4)]	// set element _44

					fld		float ptr x;
					fstp	float ptr [eax+ELEM_ADDR(4,1)]
					fld		float ptr y;
					fstp	float ptr [eax+ELEM_ADDR(4,2)]
					fld		float ptr z;
					fstp	float ptr [eax+ELEM_ADDR(4,3)]
				}
			}
			/* Creates a matrix that scales along the x-axis, y-axis, and y-axis.
			*/
			static void CreateScale(Matrix& res, float x, float y, float z)
			{
				__asm
				{ 
					xorps	xmm0,xmm0
					mov 	eax, res					
					
					fld		float ptr x;
					movaps	[eax+ELEM_ADDR(1,1)], xmm0		// clear line _L1
					fst		float ptr [eax+ELEM_ADDR(1,1)]	// set element _11
					fld		float ptr y;
					movaps	[eax+ELEM_ADDR(2,1)], xmm0		// clear line _L2
					fst		float ptr [eax+ELEM_ADDR(2,2)]	// set element _22
					fld		float ptr z;
					movaps	[eax+ELEM_ADDR(3,1)], xmm0		// clear line _L3
					fst		float ptr [eax+ELEM_ADDR(3,3)]	// set element _33										
					fld1
					movaps	[eax+ELEM_ADDR(4,1)], xmm0		// clear line _L4
					fstp	float ptr [eax+ELEM_ADDR(4,4)]	// set element _44
				}
			}

			/* Creates a left-handed, look-at matrix.
			*/
			static void CreateLookAtLH(Matrix& res, Vector3 cameraPosition, Vector3 cameraTarget, Vector3 up)
			{
				Vector3 zaxis = VecSub(cameraTarget, cameraPosition);
				zaxis = Vec3Normalize(zaxis);

				Vector3 xaxis = Vec3Cross(up, zaxis);
				xaxis = Vec3Normalize(xaxis);
				Vector3 yaxis = Vec3Cross(zaxis, xaxis);
				
				float tx = Vec3Dot(xaxis, cameraPosition);
				float ty = Vec3Dot(yaxis, cameraPosition);
				float tz = Vec3Dot(zaxis, cameraPosition);

				res.Row1 = xaxis;
				res.Row2 = yaxis;
				res.Row3 = zaxis;

				__asm 
				{	
					mov 	eax, res 
					fld		float ptr tx
					fstp	float ptr [eax+ELEM_ADDR(4,1)]
					fld		float ptr ty
					fstp	float ptr [eax+ELEM_ADDR(4,2)]
					fld		float ptr tz
					fstp	float ptr [eax+ELEM_ADDR(4,3)]
					fld1
					fstp	float ptr [eax+ELEM_ADDR(4,4)]

					fldz	
					fst		float ptr [eax+ELEM_ADDR(1,4)]
					fst		float ptr [eax+ELEM_ADDR(2,4)]
					fstp	float ptr [eax+ELEM_ADDR(3,4)]
				}
			}
			/* Creates a right-handed, look-at matrix.
			*/
			static void CreateLookAtRH(Matrix& res, Vector3 cameraPosition, Vector3 cameraTarget, Vector3 up)
			{
				Vector3 zaxis = VecSub(cameraTarget, cameraPosition);
				zaxis = Vec3Normalize(zaxis);

				Vector3 xaxis = Vec3Cross(up, zaxis);
				xaxis = Vec3Normalize(xaxis);
				Vector3 yaxis = Vec3Cross(zaxis, xaxis);

				float tx = Vec3Dot(xaxis, cameraPosition);
				float ty = Vec3Dot(yaxis, cameraPosition);
				float tz = Vec3Dot(zaxis, cameraPosition);
				
				__asm 
				{
					mov 	eax, res 
					lea		esi, xaxis

					fld		float ptr [esi+VEC_ADDR_X]
					fstp	float ptr [eax+ELEM_ADDR(1,1)]
					fld		float ptr [esi+VEC_ADDR_Y]
					fstp	float ptr [eax+ELEM_ADDR(2,1)]
					fld		float ptr [esi+VEC_ADDR_Z]
					fstp	float ptr [eax+ELEM_ADDR(3,1)]
					
					lea		esi, yaxis
					fld		float ptr [esi+VEC_ADDR_X]
					fstp	float ptr [eax+ELEM_ADDR(1,2)]
					fld		float ptr [esi+VEC_ADDR_Y]
					fstp	float ptr [eax+ELEM_ADDR(2,2)]
					fld		float ptr [esi+VEC_ADDR_Z]
					fstp	float ptr [eax+ELEM_ADDR(3,2)]
					
					lea		esi, zaxis
					fld		float ptr [esi+VEC_ADDR_X]
					fstp	float ptr [eax+ELEM_ADDR(1,3)]
					fld		float ptr [esi+VEC_ADDR_Y]
					fstp	float ptr [eax+ELEM_ADDR(2,3)]
					fld		float ptr [esi+VEC_ADDR_Z]
					fstp	float ptr [eax+ELEM_ADDR(3,3)]
					
					fld		float ptr tx
					fstp	float ptr [eax+ELEM_ADDR(4,1)]
					fld		float ptr ty
					fstp	float ptr [eax+ELEM_ADDR(4,2)]
					fld		float ptr tz
					fstp	float ptr [eax+ELEM_ADDR(4,3)]

					fld1
					fstp	float ptr [eax+ELEM_ADDR(4,4)]


					fldz
					fst		float ptr [eax+ELEM_ADDR(1,4)]
					fst		float ptr [eax+ELEM_ADDR(2,4)]
					fstp	float ptr [eax+ELEM_ADDR(3,4)]
				}
			}
			
			/* Creates a left-handed, perspective projection matrix based on a field of view.
			*  
			*  This function computes the returned matrix as shown:
			*  
			*  xScale     0          0               0
			*  0        yScale       0               0
			*  0          0       zf/(zf-zn)         1
			*  0          0       -zn*zf/(zf-zn)     0
			*  where:
			*  yScale = cot(fovY/2)
			*  
			*  xScale = yScale / aspect ratio
			*/
			static void CreatePerspectiveFovLH(Matrix& res, float fieldOfView, float aspectRatio, float nearPlaneDistance, float farPlaneDistance)
			{
				static const float half = 0.5f;
				__asm
				{
					xorps	xmm0,xmm0
					mov 	eax, res;

					fld		float ptr fieldOfView
					fld		half
					movaps	[eax+ELEM_ADDR(1,1)], xmm0		// clear line _L1
					fmul
					fsincos
					fdiv
					fst		float ptr [eax+ELEM_ADDR(1,1)] 

					fld		float ptr aspectRatio
					movaps	[eax+ELEM_ADDR(2,1)], xmm0		// clear line _L1
					fdivr
					fstp	float ptr [eax+ELEM_ADDR(2,2)] 
					
					movaps	[eax+ELEM_ADDR(3,1)], xmm0		// clear line _L3
					fld1
					fst	float ptr [eax+ELEM_ADDR(3,4)]

					fld		float ptr nearPlaneDistance					
					fld		float ptr farPlaneDistance
					fsub
					fdivr
					fld 	float ptr farPlaneDistance
					fmul
					fst		float ptr [eax+ELEM_ADDR(3,3)] 
					fld 	float ptr nearPlaneDistance
					movaps	[eax+ELEM_ADDR(4,1)], xmm0		// clear line _L4
					fmul
					fchs
					fstp	float ptr [eax+ELEM_ADDR(4,3)] 

				}
			}

			/* Creates a right-handed, perspective projection matrix based on a field of view.
			*  
			*  This function computes the returned matrix as shown:
			*  
			*  xScale     0          0               0
			*  0        yScale       0               0
			*  0          0       zf/(zn-zf)        -1
			*  0          0       zn*zf/(zn-zf)      0
			*  where:
			*  yScale = cot(fovY/2)
			*  
			*  xScale = yScale / aspect ratio
			*/
			static void CreatePerspectiveFovRH(Matrix& res, float fieldOfView, float aspectRatio, float nearPlaneDistance, float farPlaneDistance)
			{
				static const float half = 0.5f;
				__asm
				{
					xorps	xmm0,xmm0
					mov 	eax, res;

					fld		float ptr fieldOfView
					fld		half
					movaps	[eax+ELEM_ADDR(1,1)], xmm0		// clear line _L1
					fmul
					fsincos
					fdiv
					fst		float ptr [eax+ELEM_ADDR(1,1)] 

					fld		float ptr aspectRatio
					movaps	[eax+ELEM_ADDR(2,1)], xmm0		// clear line _L1
					fdivr
					fstp	float ptr [eax+ELEM_ADDR(2,2)] 
					
					movaps	[eax+ELEM_ADDR(3,1)], xmm0		// clear line _L3
					fld1
					fchs
					fst	float ptr [eax+ELEM_ADDR(3,4)]
					fchs
					fld		float ptr farPlaneDistance					
					fld		float ptr nearPlaneDistance
					fsub
					fdivr
					fld 	float ptr farPlaneDistance
					fmul
					fst		float ptr [eax+ELEM_ADDR(3,3)] 
					fld 	float ptr nearPlaneDistance
					movaps	[eax+ELEM_ADDR(4,1)], xmm0		// clear line _L4
					fmul
					fstp	float ptr [eax+ELEM_ADDR(4,3)] 

				}
			}


			/* Creates a left-handed, orthographic projection matrix.
			*  
			*  All the parameters of the function are distances in camera space. The parameters describe the dimensions of the view volume.
			*   2/w  0    0           0
			*   0    2/h  0           0
			*   0    0    1/(zf-zn)   0
			*   0    0   -zn/(zf-zn)  1
			*/
			static void CreateOrthoLH(Matrix& res, float width, float height, float zNearPlane, float zFarPlane)
			{
				static const float two = 2;

				__asm
				{					
					xorps	xmm0,xmm0
					mov 	eax, res;
										
					fld		float ptr two
					fst		st(1)
					fld		float ptr width
					movaps	[eax+ELEM_ADDR(1,1)], xmm0		// clear line _L1
					// w 2 2
					fdivr
					fstp	float ptr [eax+ELEM_ADDR(1,1)] 
					fld		float ptr height					
					movaps	[eax+ELEM_ADDR(2,1)], xmm0		// clear line _L2
					fdivr
					fstp	float ptr [eax+ELEM_ADDR(2,2)] 

					fld1					
					fld		float ptr zNearPlane					
					fld		float ptr zFarPlane
					movaps	[eax+ELEM_ADDR(3,1)], xmm0		// clear line _L3
					fsub
					fdivr
					fst		float ptr [eax+ELEM_ADDR(3,3)] 
					movaps	[eax+ELEM_ADDR(4,1)], xmm0		// clear line _L4
					fchs
					fld		float ptr zNearPlane
					fmul
					fstp		float ptr [eax+ELEM_ADDR(4,3)] 
				}

			}

			/*  Creates a right-handed, orthographic projection matrix.
			*  
			*  All the parameters of the function are distances in camera space.
			*  The parameters describe the dimensions of the view volume.
			*  
			*  This function uses the following formula to compute the result matrix. 
			*   2/w  0    0           0
			*   0    2/h  0           0
			*   0    0    1/(zn-zf)   0
			*   0    0    zn/(zn-zf)  l
			*/
			static void CreateOrthoRH(Matrix& res, float width, float height, float zNearPlane, float zFarPlane)
			{
				static const float two = 2;

				__asm
				{					
					xorps	xmm0,xmm0
					mov 	eax, res;

					fld		float ptr two
					fst		st(1)
					fld		float ptr width
					movaps	[eax+ELEM_ADDR(1,1)], xmm0		// clear line _L1
					// w 2 2
					fdivr
					fstp	float ptr [eax+ELEM_ADDR(1,1)] 
					fld		float ptr height					
					movaps	[eax+ELEM_ADDR(2,1)], xmm0		// clear line _L2
					fdivr
					fstp	float ptr [eax+ELEM_ADDR(2,2)] 

					fld1					
					fld		float ptr zNearPlane					
					fld		float ptr zFarPlane
					movaps	[eax+ELEM_ADDR(3,1)], xmm0		// clear line _L3
					fsubr
					fdivr
					fst		float ptr [eax+ELEM_ADDR(3,3)] 
					movaps	[eax+ELEM_ADDR(4,1)], xmm0		// clear line _L4
					fld		float ptr zNearPlane
					fmul
					fstp	float ptr [eax+ELEM_ADDR(4,3)] 
				}

			}

			/* Creates a spherical billboard that rotates around a specified object position.
			*/
			static void CreateBillboard(const Vector3 &objectPosition, const Vector3 &cameraPosition, const Vector3 &cameraUpVector, const Vector3 &cameraForwardVector, Matrix& res);

			
		};
	
#pragma pack(pop)

	}
}

#endif