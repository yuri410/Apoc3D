#pragma once
#ifndef APOC3D_MATRIX_H
#define APOC3D_MATRIX_H

/**
 * -----------------------------------------------------------------------------
 * This source file is part of Apoc3D Engine
 * 
 * Copyright (c) 2009+ Tao Xin
 * 
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 * 
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 * 
 * You should have received a copy of the GNU General Public License
 * along with this program.  if not, write to the Free Software Foundation, 
 * Inc., 59 Temple Place - Suite 330, Boston, MA 02111-1307, USA, or go to
 * http://www.gnu.org/copyleft/gpl.txt.
 * 
 * -----------------------------------------------------------------------------
 */

#include "Vector.h"

#if APOC3D_MATH_IMPL == APOC3D_SSE
#define _mm_ror_ps(vec,i)	\
	(((i)%4) ? (_mm_shuffle_ps(vec,vec, _MM_SHUFFLE((unsigned char)(i+3)%4,(unsigned char)(i+2)%4,(unsigned char)(i+1)%4,(unsigned char)(i+0)%4))) : (vec))
#define _mm_rol_ps(vec,i)	\
	(((i)%4) ? (_mm_shuffle_ps(vec,vec, _MM_SHUFFLE((unsigned char)(7-i)%4,(unsigned char)(6-i)%4,(unsigned char)(5-i)%4,(unsigned char)(4-i)%4))) : (vec))

#define _mm_abs_ps(vec)		_mm_andnot_ps(_MASKSIGN_,vec)
#define _mm_neg_ps(vec)		_mm_xor_ps(_MASKSIGN_,vec)
#endif

namespace Apoc3D
{
	namespace Math
	{

		class APAPI MatrixStack
		{
		private:
			Apoc3D::Collections::Stack<Matrix>* m_stack;

		public:
			MatrixStack(int reserve);
			~MatrixStack();

			void PushMultply(const Matrix& mat) const;
			void PushMatrix(const Matrix& mat) const;
			bool PopMatrix() const;
			bool PopMatrix(Matrix& mat) const;

			Matrix& Peek() const;
			

			int getCount() const;
		};

#define COL_INDEX(x) (x-1)
#define ELEM_ADDR(i,j) ((i-1)*4*4 + (j-1)*4)
#define ME(i,j) ((i-1)*4+j-1)

#if APOC3D_MATH_IMPL == APOC3D_SSE
#pragma pack(push, 16)
#endif

#ifdef _MSC_VER
#pragma warning( push )
#pragma warning( disable : 4201 )
#endif

		/**
		 *  Defines a 4x4 matrix.
		 */
		class APAPI Matrix
		{
		public:
			const static Matrix Identity;

#if APOC3D_MATH_IMPL == APOC3D_SSE
			union
			{
				struct
				{
					float M11, M12, M13, M14;
					float M21, M22, M23, M24;
					float M31, M32, M33, M34;
					float M41, M42, M43, M44;
				};
				struct
				{
					__m128 Row1;
					__m128 Row2;
					__m128 Row3;
					__m128 Row4;
				};
			};
#elif APOC3D_MATH_IMPL == APOC3D_DEFAULT
			union
			{
				struct
				{
					float Elements[16];
				};
				struct
				{
					float M11, M12, M13, M14;
					float M21, M22, M23, M24;
					float M31, M32, M33, M34;
					float M41, M42, M43, M44;
				};
			};
#endif
			
			
				
			Matrix() {  ZeroMatrix(); }
			explicit Matrix(const float elements[16])
			{				
			#if APOC3D_MATH_IMPL == APOC3D_SSE
				SSEVecLoader buffer;
				memcpy(&buffer, &elements[0], sizeof(float) * 4);
				Row1 = _mm_load_ps(reinterpret_cast<const float*>(&buffer));
				memcpy(&buffer, &elements[4], sizeof(float) * 4);
				Row2 = _mm_load_ps(reinterpret_cast<const float*>(&buffer));
				memcpy(&buffer, &elements[8], sizeof(float) * 4);
				Row3 = _mm_load_ps(reinterpret_cast<const float*>(&buffer));
				memcpy(&buffer, &elements[12], sizeof(float) * 4);
				Row4 = _mm_load_ps(reinterpret_cast<const float*>(&buffer));
			#elif APOC3D_MATH_IMPL == APOC3D_DEFAULT
				memcpy(Elements, elements, sizeof(Elements));
			#endif
			}

#if APOC3D_MATH_IMPL == APOC3D_SSE
			Matrix(const Matrix &m) : Row1(m.Row1), Row2(m.Row2), Row3(m.Row3), Row4(m.Row4) {}
#elif APOC3D_MATH_IMPL == APOC3D_DEFAULT
			Matrix(const Matrix &m) { memcpy(Elements, m.Elements, sizeof(Elements)); } 
#endif

			Matrix(float f11, float f12, float f13, float f14,
				float f21, float f22, float f23, float f24,
				float f31, float f32, float f33, float f34,
				float f41, float f42, float f43, float f44)
				: M11(f11), M12(f12), M13(f13), M14(f14),
				 M21(f21), M22(f22), M23(f23), M24(f24),
				 M31(f31), M32(f32), M33(f33), M34(f34),
				 M41(f41), M42(f42), M43(f43), M44(f44)
			{
				/*__asm
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
				}*/
			}

			Vector3 GetX() const
			{
#if APOC3D_MATH_IMPL == APOC3D_SSE
				return Row1;
#elif APOC3D_MATH_IMPL == APOC3D_DEFAULT
				return Vector3(M11, M12, M13);
#endif
			}

			Vector3 GetY() const
			{
#if APOC3D_MATH_IMPL == APOC3D_SSE
				return Row2;
#elif APOC3D_MATH_IMPL == APOC3D_DEFAULT
				return Vector3(M21, M22, M23);
#endif
			}

			Vector3 GetZ() const
			{
#if APOC3D_MATH_IMPL == APOC3D_SSE
				return Row3;
#elif APOC3D_MATH_IMPL == APOC3D_DEFAULT
				return Vector3(M31, M32, M33);
#endif
			}


			Vector3 GetRight() const
			{
			#if APOC3D_MATH_IMPL == APOC3D_SSE
				return Row1;
			#elif APOC3D_MATH_IMPL == APOC3D_DEFAULT
				return Vector3(M11, M12, M13);
			#endif
			}
			Vector3 GetLeft() const
			{
			#if APOC3D_MATH_IMPL == APOC3D_SSE
				__m128 v = Row1;
				v = SIMDVecNegate(v);
				return v;
			#elif APOC3D_MATH_IMPL == APOC3D_DEFAULT
				return Vector3(-M11, -M12, -M13);
			#endif
			}

			Vector3 GetUp() const
			{
			#if APOC3D_MATH_IMPL == APOC3D_SSE
				return Row2;
			#elif APOC3D_MATH_IMPL == APOC3D_DEFAULT
				return Vector3(M21, M22, M23);
			#endif
			}
			Vector3 GetDown() const
			{
			#if APOC3D_MATH_IMPL == APOC3D_SSE
				__m128 v = Row2;
				v = SIMDVecNegate(v);
				return v;
			#elif APOC3D_MATH_IMPL == APOC3D_DEFAULT
				return Vector3(-M21, -M22, -M23);
			#endif
			}

			Vector3 GetBackward() const
			{
			#if APOC3D_MATH_IMPL == APOC3D_SSE
				return Row3;
			#elif APOC3D_MATH_IMPL == APOC3D_DEFAULT
				return Vector3(M31, M32, M33);
			#endif
			}
			Vector3 GetForward() const
			{
			#if APOC3D_MATH_IMPL == APOC3D_SSE
				__m128 v = Row3;
				v = SIMDVecNegate(v);
				return v;
			#elif APOC3D_MATH_IMPL == APOC3D_DEFAULT
				return Vector3(-M31, -M32, -M33);
			#endif
			}
			
			Vector3 GetTranslation() const
			{
			#if APOC3D_MATH_IMPL == APOC3D_SSE
				return Row4;				
			#elif APOC3D_MATH_IMPL == APOC3D_DEFAULT
				return Vector3(M41, M42, M43);
			#endif
			}

			#if APOC3D_MATH_IMPL == APOC3D_SSE
			void SetRight(Vector3 v) { Row1 = v; *(reinterpret_cast<float*>(&Row1)+3)=0.0f; }
			void SetLeft(Vector3 v) { Row1 = v; SIMDVecNegate(Row1); *(reinterpret_cast<float*>(&Row1)+3)=0.0f; }

			void SetUp(Vector3 v) { Row2 = v; *(reinterpret_cast<float*>(&Row2)+3)=0.0f; }
			void SetDown(Vector3 v) { Row2 = v; SIMDVecNegate(Row2); *(reinterpret_cast<float*>(&Row2)+3)=0.0f; }

			void SetBackward(Vector3 v) { Row3 = v; *(reinterpret_cast<float*>(&Row3)+3)=0.0f; }
			void SetForward(Vector3 v) { Row3 = v; SIMDVecNegate(Row3); *(reinterpret_cast<float*>(&Row3)+3)=0.0f; }

			void SetTranslation(Vector3 v) { Row4 = v; *(reinterpret_cast<float*>(&Row4)+3)=1.0f; }


			void SetX(Vector3 v) { Row1 = v; *(reinterpret_cast<float*>(&Row1)+3)=0.0f; }
			void SetY(Vector3 v) { Row2 = v; *(reinterpret_cast<float*>(&Row2)+3)=0.0f; }
			void SetZ(Vector3 v) { Row3 = v; *(reinterpret_cast<float*>(&Row3)+3)=0.0f; }

			#elif APOC3D_MATH_IMPL == APOC3D_DEFAULT


			void SetX(const Vector3& v) { M11 = v.X; M12 = v.Y; M13 = v.Z; }
			void SetY(const Vector3& v) { M21 = v.X; M22 = v.Y; M23 = v.Z; }
			void SetZ(const Vector3& v) { M31 = v.X; M32 = v.Y; M33 = v.Z; }


			void SetRight(const Vector3& v) { M11 = v.X; M12 = v.Y; M13 = v.Z; }
			void SetLeft(const Vector3& v) { M11 = -v.X; M12 = -v.Y; M13 = -v.Z; } 

			void SetUp(const Vector3& v) { M21 = v.X; M22 = v.Y; M23 = v.Z; }
			void SetDown(const Vector3& v) { M21 = -v.X; M22 = -v.Y; M23 = -v.Z; }

			void SetBackward(const Vector3& v) { M31 = v.X; M32 = v.Y; M33 = v.Z; }
			void SetForward(const Vector3& v) { M31 = -v.X; M32 = -v.Y; M33 = -v.Z; }

			void SetTranslation(const Vector3& v) { M41 = v.X; M42 = v.Y; M43 = v.Z; }
			void SetTranslation(float x, float y, float z) { M41 = x; M42 = y; M43 = z; }

			#endif
			void SetXYZ(const Vector3& x, const Vector3& y, const Vector3& z)
			{
				SetX(x); SetY(y); SetZ(z);
			}

			void LoadIdentity()
			{
			#if APOC3D_MATH_IMPL == APOC3D_SSE
				Row1 = Identity.Row1;
				Row2 = Identity.Row2;
				Row3 = Identity.Row3;
				Row4 = Identity.Row4;
			#elif APOC3D_MATH_IMPL == APOC3D_DEFAULT
				M11 = M22 = M33 = M44 = 1;
				M12 = M13 = M14 = 0;
				M21 = M23 = M24 = 0;
				M31 = M32 = M34 = 0;
				M41 = M42 = M43 = 0;
				//memcpy(Elements, Identity.Elements, sizeof(Elements));
			#endif
			}

			/**
			 * Transposes the matrix
			 */
			void Transpose()
			{
			#if APOC3D_MATH_IMPL == APOC3D_SSE
				_MM_TRANSPOSE4_PS(Row1, Row2, Row3, Row4);
			#elif APOC3D_MATH_IMPL == APOC3D_DEFAULT
				float e[16];
				e[ME(1,1)] = Elements[ME(1,1)];
				e[ME(1,2)] = Elements[ME(2,1)];
				e[ME(1,3)] = Elements[ME(3,1)];
				e[ME(1,4)] = Elements[ME(4,1)];
				e[ME(2,1)] = Elements[ME(1,2)];
				e[ME(2,2)] = Elements[ME(2,2)];
				e[ME(2,3)] = Elements[ME(3,2)];
				e[ME(2,4)] = Elements[ME(4,2)];
				e[ME(3,1)] = Elements[ME(1,3)];
				e[ME(3,2)] = Elements[ME(2,3)];
				e[ME(3,3)] = Elements[ME(3,3)];
				e[ME(3,4)] = Elements[ME(4,3)];
				e[ME(4,1)] = Elements[ME(1,4)];
				e[ME(4,2)] = Elements[ME(2,4)];
				e[ME(4,3)] = Elements[ME(3,4)];
				e[ME(4,4)] = Elements[ME(4,4)];
				memcpy(Elements, e, sizeof(e));				
			#endif
			}
			/**
			 *  Inverts the matrix.
			 */
			float Inverse(); // TODO: Rewrite
			

			/**
			 *  Calculates the determinant of the matrix.
			 */
			float Determinant() const// TODO: Rewrite
			{
			#if APOC3D_MATH_IMPL == APOC3D_SSE
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
			#elif APOC3D_MATH_IMPL == APOC3D_DEFAULT

				float temp1 = (M33 * M44) - (M34 * M43);
				float temp2 = (M32 * M44) - (M34 * M42);
				float temp3 = (M32 * M43) - (M33 * M42);
				float temp4 = (M31 * M44) - (M34 * M41);
				float temp5 = (M31 * M43) - (M33 * M41);
				float temp6 = (M31 * M42) - (M32 * M41);

				return M11 * (M22 * temp1 - M23 * temp2 + M24 * temp3) - 
					M12 * (M21 * temp1 - M23 * temp4 + M24 * temp5) + 
					M13 * (M21 * temp2 - M22 * temp4 + M24 * temp6) -
					M14 * (M21 * temp3 - M22 * temp5 + M23 * temp6);
			#endif
			}

			void ZeroMatrix()
			{
			#if APOC3D_MATH_IMPL == APOC3D_SSE
				Row1 = Row2 = Row3 = Row4 = _mm_setzero_ps();
			#elif APOC3D_MATH_IMPL == APOC3D_DEFAULT
				for (int i=0;i<16;i++)
					Elements[i] = 0;
			#endif
			}

			/**
			 *  Determines the product of two matrices.
			 *  The result matrix cannot reference to the memory that ma or mb is referring to, or the result is not correct.
			*/
			static void Multiply(Matrix& res, const Matrix& ma, const Matrix& mb)
			{
				assert(&ma != &res && &mb != &res);
			#if APOC3D_MATH_IMPL == APOC3D_SSE
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
			#elif APOC3D_MATH_IMPL == APOC3D_DEFAULT

				/*__m128 a;
				__m128 b;
				__m128 c;
				__m128 d;

				if (((uintptr_t)&mb.M11) & 15)
				{
					a = _mm_loadu_ps(&mb.M11);
					b = _mm_loadu_ps(&mb.M21);
					c = _mm_loadu_ps(&mb.M31);
					d = _mm_loadu_ps(&mb.M41);
				}
				else
				{
					a = mb.Row1;// _mm_load_ps(&mb.M11);
					b = mb.Row2;//_mm_load_ps(&mb.M21);
					c = mb.Row3;// _mm_load_ps(&mb.M31);
					d = mb.Row4;// _mm_load_ps(&mb.M41);
				}
				
				__m128 t1, t2;

				t1 = _mm_set1_ps(ma.M11);
				t2 = _mm_mul_ps(a, t1);
				t1 =_mm_set1_ps(ma.M12);
				t2 = _mm_add_ps(_mm_mul_ps(b, t1), t2);
				t1 =_mm_set1_ps(ma.M13);
				t2 = _mm_add_ps(_mm_mul_ps(c, t1), t2);
				t1 =_mm_set1_ps(ma.M14);
				t2 = _mm_add_ps(_mm_mul_ps(d, t1), t2);

				_mm_store_ps(&res.M11, t2);
				//_mm_store_ps(&m[0], t2);

				t1 = _mm_set1_ps(ma.M21);
				t2 = _mm_mul_ps(a, t1);
				t1 =_mm_set1_ps(ma.M22);
				t2 = _mm_add_ps(_mm_mul_ps(b, t1), t2);
				t1 =_mm_set1_ps(ma.M23);
				t2 = _mm_add_ps(_mm_mul_ps(c, t1), t2);
				t1 =_mm_set1_ps(ma.M24);
				t2 = _mm_add_ps(_mm_mul_ps(d, t1), t2);

				_mm_store_ps(&res.M21, t2);
				//_mm_store_ps(&m[4], t2);

				t1 = _mm_set1_ps(ma.M31);
				t2 = _mm_mul_ps(a, t1);
				t1 =_mm_set1_ps(ma.M32);
				t2 = _mm_add_ps(_mm_mul_ps(b, t1), t2);
				t1 =_mm_set1_ps(ma.M33);
				t2 = _mm_add_ps(_mm_mul_ps(c, t1), t2);
				t1 =_mm_set1_ps(ma.M34);
				t2 = _mm_add_ps(_mm_mul_ps(d, t1), t2);

				_mm_store_ps(&res.M31, t2);
				//_mm_store_ps(&m[8], t2);

				t1 = _mm_set1_ps(ma.M41);
				t2 = _mm_mul_ps(a, t1);
				t1 =_mm_set1_ps(ma.M42);
				t2 = _mm_add_ps(_mm_mul_ps(b, t1), t2);
				t1 =_mm_set1_ps(ma.M43);
				t2 = _mm_add_ps(_mm_mul_ps(c, t1), t2);
				t1 =_mm_set1_ps(ma.M44);
				t2 = _mm_add_ps(_mm_mul_ps(d, t1), t2);

				_mm_store_ps(&res.M41, t2);
				*/
#if _DEBUG
				res.M11 = (ma.M11 * mb.M11) + (ma.M12 * mb.M21) + (ma.M13 * mb.M31) + (ma.M14 * mb.M41);
				res.M12 = (ma.M11 * mb.M12) + (ma.M12 * mb.M22) + (ma.M13 * mb.M32) + (ma.M14 * mb.M42);
				res.M13 = (ma.M11 * mb.M13) + (ma.M12 * mb.M23) + (ma.M13 * mb.M33) + (ma.M14 * mb.M43);
				res.M14 = (ma.M11 * mb.M14) + (ma.M12 * mb.M24) + (ma.M13 * mb.M34) + (ma.M14 * mb.M44);

				res.M21 = (ma.M21 * mb.M11) + (ma.M22 * mb.M21) + (ma.M23 * mb.M31) + (ma.M24 * mb.M41);
				res.M22 = (ma.M21 * mb.M12) + (ma.M22 * mb.M22) + (ma.M23 * mb.M32) + (ma.M24 * mb.M42);
				res.M23 = (ma.M21 * mb.M13) + (ma.M22 * mb.M23) + (ma.M23 * mb.M33) + (ma.M24 * mb.M43);
				res.M24 = (ma.M21 * mb.M14) + (ma.M22 * mb.M24) + (ma.M23 * mb.M34) + (ma.M24 * mb.M44);

				res.M31 = (ma.M31 * mb.M11) + (ma.M32 * mb.M21) + (ma.M33 * mb.M31) + (ma.M34 * mb.M41);
				res.M32 = (ma.M31 * mb.M12) + (ma.M32 * mb.M22) + (ma.M33 * mb.M32) + (ma.M34 * mb.M42);
				res.M33 = (ma.M31 * mb.M13) + (ma.M32 * mb.M23) + (ma.M33 * mb.M33) + (ma.M34 * mb.M43);
				res.M34 = (ma.M31 * mb.M14) + (ma.M32 * mb.M24) + (ma.M33 * mb.M34) + (ma.M34 * mb.M44);

				res.M41 = (ma.M41 * mb.M11) + (ma.M42 * mb.M21) + (ma.M43 * mb.M31) + (ma.M44 * mb.M41);
				res.M42 = (ma.M41 * mb.M12) + (ma.M42 * mb.M22) + (ma.M43 * mb.M32) + (ma.M44 * mb.M42);
				res.M43 = (ma.M41 * mb.M13) + (ma.M42 * mb.M23) + (ma.M43 * mb.M33) + (ma.M44 * mb.M43);
				res.M44 = (ma.M41 * mb.M14) + (ma.M42 * mb.M24) + (ma.M43 * mb.M34) + (ma.M44 * mb.M44);
#else
				float r11 =	(ma.M11 * mb.M11) + (ma.M12 * mb.M21) + (ma.M13 * mb.M31) + (ma.M14 * mb.M41);
				float r12 =	(ma.M11 * mb.M12) + (ma.M12 * mb.M22) + (ma.M13 * mb.M32) + (ma.M14 * mb.M42);
				float r13 =	(ma.M11 * mb.M13) + (ma.M12 * mb.M23) + (ma.M13 * mb.M33) + (ma.M14 * mb.M43);
				float r14 =	(ma.M11 * mb.M14) + (ma.M12 * mb.M24) + (ma.M13 * mb.M34) + (ma.M14 * mb.M44);

				float r21 =	(ma.M21 * mb.M11) + (ma.M22 * mb.M21) + (ma.M23 * mb.M31) + (ma.M24 * mb.M41);
				float r22 =	(ma.M21 * mb.M12) + (ma.M22 * mb.M22) + (ma.M23 * mb.M32) + (ma.M24 * mb.M42);
				float r23 =	(ma.M21 * mb.M13) + (ma.M22 * mb.M23) + (ma.M23 * mb.M33) + (ma.M24 * mb.M43);
				float r24 =	(ma.M21 * mb.M14) + (ma.M22 * mb.M24) + (ma.M23 * mb.M34) + (ma.M24 * mb.M44);

				float r31 =	(ma.M31 * mb.M11) + (ma.M32 * mb.M21) + (ma.M33 * mb.M31) + (ma.M34 * mb.M41);
				float r32 =	(ma.M31 * mb.M12) + (ma.M32 * mb.M22) + (ma.M33 * mb.M32) + (ma.M34 * mb.M42);
				float r33 =	(ma.M31 * mb.M13) + (ma.M32 * mb.M23) + (ma.M33 * mb.M33) + (ma.M34 * mb.M43);
				float r34 =	(ma.M31 * mb.M14) + (ma.M32 * mb.M24) + (ma.M33 * mb.M34) + (ma.M34 * mb.M44);

				float r41 =	(ma.M41 * mb.M11) + (ma.M42 * mb.M21) + (ma.M43 * mb.M31) + (ma.M44 * mb.M41);
				float r42 =	(ma.M41 * mb.M12) + (ma.M42 * mb.M22) + (ma.M43 * mb.M32) + (ma.M44 * mb.M42);
				float r43 =	(ma.M41 * mb.M13) + (ma.M42 * mb.M23) + (ma.M43 * mb.M33) + (ma.M44 * mb.M43);
				float r44 =	(ma.M41 * mb.M14) + (ma.M42 * mb.M24) + (ma.M43 * mb.M34) + (ma.M44 * mb.M44);

				res.M11 = r11; res.M12 = r12; res.M13 = r13; res.M14 = r14;
				res.M21 = r21; res.M22 = r22; res.M23 = r23; res.M24 = r24;
				res.M31 = r31; res.M32 = r32; res.M33 = r33; res.M34 = r34;
				res.M41 = r41; res.M42 = r42; res.M43 = r43; res.M44 = r44;

#endif
				/*__m128 maR1 = _mm_loadu_ps(&ma.M11);
				__m128 maR2 = _mm_loadu_ps(&ma.M21);
				__m128 maR3 = _mm_loadu_ps(&ma.M31);
				__m128 maR4 = _mm_loadu_ps(&ma.M41);

				__declspec(align(16)) float temp[4] = { mb.M11, mb.M21, mb.M31, mb.M41 };
				__m128 mbC1 = _mm_load_ps(temp);
				temp[0] = mb.M12; temp[1] = mb.M22; temp[2] = mb.M32; temp[3] = mb.M42;
				__m128 mbC2 = _mm_load_ps(temp);
				temp[0] = mb.M13; temp[1] = mb.M23; temp[2] = mb.M33; temp[3] = mb.M43;
				__m128 mbC3 = _mm_load_ps(temp);
				temp[0] = mb.M14; temp[1] = mb.M24; temp[2] = mb.M34; temp[3] = mb.M44;
				__m128 mbC4 = _mm_load_ps(temp);

				res.M11 = reinterpret_cast<const float&>(_mm_dp_ps(maR1, mbC1, SIMDDot4MaskX));
				res.M12 = reinterpret_cast<const float&>(_mm_dp_ps(maR1, mbC2, SIMDDot4MaskX));
				res.M13 = reinterpret_cast<const float&>(_mm_dp_ps(maR1, mbC3, SIMDDot4MaskX));
				res.M14 = reinterpret_cast<const float&>(_mm_dp_ps(maR1, mbC4, SIMDDot4MaskX));

				res.M21 = reinterpret_cast<const float&>(_mm_dp_ps(maR2, mbC1, SIMDDot4MaskX));
				res.M22 = reinterpret_cast<const float&>(_mm_dp_ps(maR2, mbC2, SIMDDot4MaskX));
				res.M23 = reinterpret_cast<const float&>(_mm_dp_ps(maR2, mbC3, SIMDDot4MaskX));
				res.M24 = reinterpret_cast<const float&>(_mm_dp_ps(maR2, mbC4, SIMDDot4MaskX));

				res.M31 = reinterpret_cast<const float&>(_mm_dp_ps(maR3, mbC1, SIMDDot4MaskX));
				res.M32 = reinterpret_cast<const float&>(_mm_dp_ps(maR3, mbC2, SIMDDot4MaskX));
				res.M33 = reinterpret_cast<const float&>(_mm_dp_ps(maR3, mbC3, SIMDDot4MaskX));
				res.M34 = reinterpret_cast<const float&>(_mm_dp_ps(maR3, mbC4, SIMDDot4MaskX));

				res.M41 = reinterpret_cast<const float&>(_mm_dp_ps(maR4, mbC1, SIMDDot4MaskX));
				res.M42 = reinterpret_cast<const float&>(_mm_dp_ps(maR4, mbC2, SIMDDot4MaskX));
				res.M43 = reinterpret_cast<const float&>(_mm_dp_ps(maR4, mbC3, SIMDDot4MaskX));
				res.M44 = reinterpret_cast<const float&>(_mm_dp_ps(maR4, mbC4, SIMDDot4MaskX));*/

				/*res.M11 = SIMDVec4Dot(maR1, mbC1);
				res.M12 = SIMDVec4Dot(maR1, mbC2);
				res.M13 = SIMDVec4Dot(maR1, mbC3);
				res.M14 = SIMDVec4Dot(maR1, mbC4);

				res.M21 = SIMDVec4Dot(maR2, mbC1);
				res.M22 = SIMDVec4Dot(maR2, mbC2);
				res.M23 = SIMDVec4Dot(maR2, mbC3);
				res.M24 = SIMDVec4Dot(maR2, mbC4);

				res.M31 = SIMDVec4Dot(maR3, mbC1);
				res.M32 = SIMDVec4Dot(maR3, mbC2);
				res.M33 = SIMDVec4Dot(maR3, mbC3);
				res.M34 = SIMDVec4Dot(maR3, mbC4);

				res.M41 = SIMDVec4Dot(maR4, mbC1);
				res.M42 = SIMDVec4Dot(maR4, mbC2);
				res.M43 = SIMDVec4Dot(maR4, mbC3);
				res.M44 = SIMDVec4Dot(maR4, mbC4);*/
			#endif
			}

			/**
			 *  Scales a matrix by the given value.
			 */
			static void Multiply(Matrix& res, const Matrix& ma, float mb)
			{
			#if APOC3D_MATH_IMPL == APOC3D_SSE
				__m128 b = _mm_set1_ps(mb);
				res.Row1 = _mm_mul_ps(ma.Row1,b);
				res.Row2 = _mm_mul_ps(ma.Row2,b);
				res.Row3 = _mm_mul_ps(ma.Row3,b);
				res.Row4 = _mm_mul_ps(ma.Row4,b);
			#elif APOC3D_MATH_IMPL == APOC3D_DEFAULT
				res.M11 = ma.M11 * mb;
				res.M12 = ma.M12 * mb;
				res.M13 = ma.M13 * mb;
				res.M14 = ma.M14 * mb;
				res.M21 = ma.M21 * mb;
				res.M22 = ma.M22 * mb;
				res.M23 = ma.M23 * mb;
				res.M24 = ma.M24 * mb;
				res.M31 = ma.M31 * mb;
				res.M32 = ma.M32 * mb;
				res.M33 = ma.M33 * mb;
				res.M34 = ma.M34 * mb;
				res.M41 = ma.M41 * mb;
				res.M42 = ma.M42 * mb;
				res.M43 = ma.M43 * mb;
				res.M44 = ma.M44 * mb;
			#endif
			}

			/** 
			 *  Determines the sum of two matrices.
			 */
			static void Add(Matrix& res, const Matrix& ma, const Matrix& mb)
			{
			#if APOC3D_MATH_IMPL == APOC3D_SSE
				res.Row1 = _mm_add_ps(ma.Row1,  mb.Row1);
				res.Row2 = _mm_add_ps(ma.Row2,  mb.Row2);
				res.Row3 = _mm_add_ps(ma.Row3,  mb.Row3);
				res.Row4 = _mm_add_ps(ma.Row4,  mb.Row4);
			#elif APOC3D_MATH_IMPL == APOC3D_DEFAULT
				res.M11 = ma.M11 + mb.M11;
				res.M12 = ma.M12 + mb.M12;
				res.M13 = ma.M13 + mb.M13;
				res.M14 = ma.M14 + mb.M14;
				res.M21 = ma.M21 + mb.M21;
				res.M22 = ma.M22 + mb.M22;
				res.M23 = ma.M23 + mb.M23;
				res.M24 = ma.M24 + mb.M24;
				res.M31 = ma.M31 + mb.M31;
				res.M32 = ma.M32 + mb.M32;
				res.M33 = ma.M33 + mb.M33;
				res.M34 = ma.M34 + mb.M34;
				res.M41 = ma.M41 + mb.M41;
				res.M42 = ma.M42 + mb.M42;
				res.M43 = ma.M43 + mb.M43;
				res.M44 = ma.M44 + mb.M44;
			#endif
			}

			/**
			 *  Determines the difference between two matrices.
			 */
			static void Subtract(Matrix& res, const Matrix& ma, const Matrix& mb)
			{
			#if APOC3D_MATH_IMPL == APOC3D_SSE
				res.Row1 = _mm_sub_ps(ma.Row1,  mb.Row1);
				res.Row2 = _mm_sub_ps(ma.Row2,  mb.Row2);
				res.Row3 = _mm_sub_ps(ma.Row3,  mb.Row3);
				res.Row4 = _mm_sub_ps(ma.Row4,  mb.Row4);
			#elif APOC3D_MATH_IMPL == APOC3D_DEFAULT
				res.M11 = ma.M11 - mb.M11;
				res.M12 = ma.M12 - mb.M12;
				res.M13 = ma.M13 - mb.M13;
				res.M14 = ma.M14 - mb.M14;
				res.M21 = ma.M21 - mb.M21;
				res.M22 = ma.M22 - mb.M22;
				res.M23 = ma.M23 - mb.M23;
				res.M24 = ma.M24 - mb.M24;
				res.M31 = ma.M31 - mb.M31;
				res.M32 = ma.M32 - mb.M32;
				res.M33 = ma.M33 - mb.M33;
				res.M34 = ma.M34 - mb.M34;
				res.M41 = ma.M41 - mb.M41;
				res.M42 = ma.M42 - mb.M42;
				res.M43 = ma.M43 - mb.M43;
				res.M44 = ma.M44 - mb.M44;
			#endif
			}

			/** 
			 *  Negates a matrix.
			 */
			static void Negate(Matrix& res, const Matrix& m)
			{
			#if APOC3D_MATH_IMPL == APOC3D_SSE
				res.Row1 = _mm_xor_ps(_MASKSIGN_,  m.Row1);
				res.Row2 = _mm_xor_ps(_MASKSIGN_,  m.Row2);
				res.Row3 = _mm_xor_ps(_MASKSIGN_,  m.Row3);
				res.Row4 = _mm_xor_ps(_MASKSIGN_,  m.Row4);
			#elif APOC3D_MATH_IMPL == APOC3D_DEFAULT
				res.M11 = -m.M11;
				res.M12 = -m.M12;
				res.M13 = -m.M13;
				res.M14 = -m.M14;
				res.M21 = -m.M21;
				res.M22 = -m.M22;
				res.M23 = -m.M23;
				res.M24 = -m.M24;
				res.M31 = -m.M31;
				res.M32 = -m.M32;
				res.M33 = -m.M33;
				res.M34 = -m.M34;
				res.M41 = -m.M41;
				res.M42 = -m.M42;
				res.M43 = -m.M43;
				res.M44 = -m.M44;
			#endif
			}

			static void Lerp(Matrix& res, const Matrix& ma, const Matrix& mb, float amount)
			{
			#if APOC3D_MATH_IMPL == APOC3D_SSE
				__m128 r1 = _mm_sub_ps(mb.Row1,  ma.Row1);
				__m128 r2 = _mm_sub_ps(mb.Row2,  ma.Row2);
				__m128 r3 = _mm_sub_ps(mb.Row3,  ma.Row3);
				__m128 r4 = _mm_sub_ps(mb.Row4,  ma.Row4);
				
				r1 = SIMDVecMul(r1, amount);
				r2 = SIMDVecMul(r2, amount);
				r3 = SIMDVecMul(r3, amount);
				r4 = SIMDVecMul(r4, amount);

				res.Row1 = _mm_add_ps(ma.Row1, r1);
				res.Row2 = _mm_add_ps(ma.Row2, r2);
				res.Row3 = _mm_add_ps(ma.Row3, r3);
				res.Row4 = _mm_add_ps(ma.Row4, r4);

			#elif APOC3D_MATH_IMPL == APOC3D_DEFAULT

				res.M11 = ma.M11 + (mb.M11 - ma.M11) * amount;
				res.M12 = ma.M12 + (mb.M12 - ma.M12) * amount;
				res.M13 = ma.M13 + (mb.M13 - ma.M13) * amount;
				res.M14 = ma.M14 + (mb.M14 - ma.M14) * amount;
				res.M21 = ma.M21 + (mb.M21 - ma.M21) * amount;
				res.M22 = ma.M22 + (mb.M22 - ma.M22) * amount;
				res.M23 = ma.M23 + (mb.M23 - ma.M23) * amount;
				res.M24 = ma.M24 + (mb.M24 - ma.M24) * amount;
				res.M31 = ma.M31 + (mb.M31 - ma.M31) * amount;
				res.M32 = ma.M32 + (mb.M32 - ma.M32) * amount;
				res.M33 = ma.M33 + (mb.M33 - ma.M33) * amount;
				res.M34 = ma.M34 + (mb.M34 - ma.M34) * amount;
				res.M41 = ma.M41 + (mb.M41 - ma.M41) * amount;
				res.M42 = ma.M42 + (mb.M42 - ma.M42) * amount;
				res.M43 = ma.M43 + (mb.M43 - ma.M43) * amount;
				res.M44 = ma.M44 + (mb.M44 - ma.M44) * amount;

			#endif
			}

			/**
			 *  Creates a matrix that rotates around the x-axis.
			 */
			static void CreateRotationX(Matrix& res, float angle)
			{
			#if APOC3D_MATH_IMPL == APOC3D_SSE
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
			#elif APOC3D_MATH_IMPL == APOC3D_DEFAULT
				float cos = cosf(angle);
				float sin = sinf(angle);
				
				res.M22 = res.M33 = cos;
				res.M23 = sin;
				res.M32 = -sin;
				 
				res.M44 = res.M11 = 1.0f;

				res.M12 = res.M13 = res.M14 = res.M21 = 
				res.M24 = res.M31 = res.M34 = res.M41 = 
				res.M42 = res.M43 =  0.0f;


			#endif
			}
			/**
			 *  Creates a matrix that rotates around the y-axis.
			 */
			static void CreateRotationY(Matrix& res, float angle)
			{
			#if APOC3D_MATH_IMPL == APOC3D_SSE
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
			#elif APOC3D_MATH_IMPL == APOC3D_DEFAULT
				float cos = cosf(angle);
				float sin = sinf(angle);

				res.M13 = -sin;
				res.M31 = sin;
				res.M11 = res.M33 = cos;

				res.M44 = res.M22 = 1.0f;


				res.M12 = res.M14 = res.M21 = res.M23 = 
				res.M24 = res.M32 = res.M34 = res.M41 = 
				res.M42 = res.M43 = 0.0f;
			#endif
			}
			/**
			 *  Creates a matrix that rotates around the z-axis.
			 */
			static void CreateRotationZ(Matrix& res, float angle)
			{
			#if APOC3D_MATH_IMPL == APOC3D_SSE
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
			#elif APOC3D_MATH_IMPL == APOC3D_DEFAULT
				float cos = cosf(angle);
				float sin = sinf(angle);
				
				res.M12 = sin;				
				res.M21 = -sin;
				res.M11 = res.M22 = cos;
				
				
				res.M33 = res.M44 = 1.0f;				

				res.M13 = res.M14 = 
				res.M23 = res.M24 =
				res.M31 = res.M32 = res.M34 =
				res.M41 = res.M42 = res.M43 = 0.0f;
			#endif
			}

			/** 
			 *  Creates a matrix that rotates around an arbitary axis.
			 */
			static void CreateRotationAxis(Matrix& res, Vector3 axis, float angle)
			{
			#if APOC3D_MATH_IMPL == APOC3D_SSE
				Vector3 dotAxis = SIMDVec3Dot2(axis,axis);
				Vector3 t0 = _mm_shuffle_ps(axis,axis, _MM_SHUFFLE(VEC_INDEX_X, VEC_INDEX_X, VEC_INDEX_Y, 0));
				Vector3 t1 = _mm_shuffle_ps(axis,axis, _MM_SHUFFLE(VEC_INDEX_Y, VEC_INDEX_Z, VEC_INDEX_Z, 0));
				Vector3 dotAxis2 = SIMDVec3Dot2(t0,t1);

				float sins, coss;
				__asm
				{
					fld		float ptr angle
					fsincos
					fstp		float ptr coss
					fstp		float ptr sins
				}

				Vector3 sin = SIMDVecLoad(sins);
				Vector3 cos = SIMDVecLoad(coss);
				Vector3 one = SIMDVecLoad(1);

				Vector3 r0 = SIMDVecSub(one, dotAxis);
				r0 = SIMDVecMul(cos, r0);
				r0 = SIMDVecAdd(dotAxis, r0);

				Vector3 r1left = _mm_shuffle_ps(dotAxis2,dotAxis2,_MM_SHUFFLE(VEC_INDEX_X, VEC_INDEX_Z, VEC_INDEX_Y, 0));
				Vector3 r1right = _mm_shuffle_ps(dotAxis2,dotAxis2,_MM_SHUFFLE(VEC_INDEX_Z, VEC_INDEX_X, VEC_INDEX_Y, 0));
				
				Vector3 tmp = SIMDVecMul(cos, r1left);
				r1left = SIMDVecSub(r1left, tmp);
				r1right = SIMDVecMul(sin, r1right);
				Vector3 r1 = SIMDVecAdd(r1left, r1right);


				
				r1left = _mm_shuffle_ps(dotAxis2,dotAxis2,_MM_SHUFFLE(VEC_INDEX_Y, VEC_INDEX_X, VEC_INDEX_Z, 0));
				r1right = _mm_shuffle_ps(dotAxis2,dotAxis2,_MM_SHUFFLE(VEC_INDEX_Y, VEC_INDEX_Z, VEC_INDEX_X, 0));
				tmp = SIMDVecMul(cos, r1left);
				r1left = SIMDVecSub(r1left, tmp);
				r1right = SIMDVecMul(sin, r1right);
				Vector3 r2 = SIMDVecSub(r1left, r1right);

				
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
			#elif APOC3D_MATH_IMPL == APOC3D_DEFAULT
				if (axis.LengthSquared() != 1.0f)
				{
					axis.NormalizeInPlace();
				}
				float x = axis.X;
				float y = axis.Y;
				float z = axis.Z;
				float cos = cosf(angle);
				float sin = sinf(angle);
				float xx = x * x;
				float yy = y * y;
				float zz = z * z;
				float xy = x * y;
				float xz = x * z;
				float yz = y * z;

				res.M11 = xx + (cos * (1.0f - xx));
				res.M12 = (xy - (cos * xy)) + (sin * z);
				res.M13 = (xz - (cos * xz)) - (sin * y);

				res.M21 = (xy - (cos * xy)) - (sin * z);
				res.M22 = yy + (cos * (1.0f - yy));
				res.M23 = (yz - (cos * yz)) + (sin * x);

				res.M31 = (xz - (cos * xz)) + (sin * y);
				res.M32 = (yz - (cos * yz)) - (sin * x);
				res.M33 = zz + (cos * (1.0f - zz));
				res.M34 = res.M41 = res.M42 = res.M43 = res.M14 = res.M24 = 0.0f;
				res.M44 = 1.0f;

			#endif
			}

			static void CreateRotationQuaternion(Matrix& result, const Quaternion& rotation);
			static void CreateRotationYawPitchRoll(Matrix& result, float yaw, float pitch, float roll);

			static void CreateTranslation(Matrix& res, const Vector3& pos)
			{
				CreateTranslation(res, pos.X, pos.Y, pos.Z);
			}
			/**
			 *  Creates a translation matrix using the specified offsets.
			 */
			static void CreateTranslation(Matrix& res, float x, float y, float z)
			{
			#if APOC3D_MATH_IMPL == APOC3D_SSE
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
			#elif APOC3D_MATH_IMPL == APOC3D_DEFAULT
				res.M11 = 1.0f; res.M12 = 0.0f; res.M13 = 0.0f; res.M14 = 0.0f;
				res.M21 = 0.0f; res.M22 = 1.0f; res.M23 = 0.0f; res.M24 = 0.0f;
				res.M31 = 0.0f; res.M32 = 0.0f; res.M33 = 1.0f; res.M34 = 0.0f;
				res.M41 = x;	res.M42 = y;	res.M43 = z;	res.M44 = 1.0f;
			#endif
			}
			/**
			 *  Creates a matrix that scales along the x-axis, y-axis, and y-axis.
			 */
			static void CreateScale(Matrix& res, float x, float y, float z)
			{
			#if APOC3D_MATH_IMPL == APOC3D_SSE
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
			#elif APOC3D_MATH_IMPL == APOC3D_DEFAULT
				res.M11 = x;
				res.M22 = y;
				res.M33 = z;
				res.M44 = 1.0f;


				res.M12 = res.M13 = res.M14 = res.M21 =
				res.M23 = res.M24 = res.M31 = res.M32 = 
				res.M34 = res.M41 = res.M42 = res.M43 = 0.0f;
			#endif
			}

			/**
			 *  Creates a left-handed, look-at matrix.
			 */
			static void CreateLookAtLH(Matrix& res, Vector3 cameraPosition, Vector3 cameraTarget, Vector3 up)
			{
			#if APOC3D_MATH_IMPL == APOC3D_SSE
				Vector3 zaxis = SIMDVecSub(cameraTarget, cameraPosition);
				zaxis = SIMDVec3Normalize(zaxis);

				Vector3 xaxis = SIMDVec3Cross(up, zaxis);
				xaxis = SIMDVec3Normalize(xaxis);
				Vector3 yaxis = SIMDVec3Cross(zaxis, xaxis);
				
				float tx = SIMDVec3Dot(xaxis, cameraPosition);
				float ty = SIMDVec3Dot(yaxis, cameraPosition);
				float tz = SIMDVec3Dot(zaxis, cameraPosition);

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
			#elif APOC3D_MATH_IMPL == APOC3D_DEFAULT
				Vector3 zaxis = cameraTarget - cameraPosition;
				zaxis.NormalizeInPlace();

				Vector3 xaxis = Vector3::Cross(up, zaxis);
				xaxis.NormalizeInPlace();

				Vector3 yaxis = Vector3::Cross(zaxis, xaxis);

				res.M11 = xaxis.X;
				res.M12 = yaxis.X;
				res.M13 = zaxis.X;

				res.M21 = xaxis.Y;
				res.M22 = yaxis.Y;
				res.M23 = zaxis.Y;

				res.M31 = xaxis.Z;
				res.M32 = yaxis.Z;
				res.M33 = zaxis.Z;
				res.M14 = res.M24 = res.M34 = 0.0f;
				res.M41 = -Vector3::Dot(xaxis, cameraPosition);
				res.M42 = -Vector3::Dot(yaxis, cameraPosition);
				res.M43 = -Vector3::Dot(zaxis, cameraPosition);
				res.M44 = 1.0f;
			#endif
			}
			/** 
			 *  Creates a right-handed, look-at matrix.
			 */
			static void CreateLookAtRH(Matrix& res, Vector3 cameraPosition, Vector3 cameraTarget, Vector3 up)
			{
			#if APOC3D_MATH_IMPL == APOC3D_SSE
				Vector3 zaxis = SIMDVecSub(cameraPosition, cameraTarget);
				zaxis = SIMDVec3Normalize(zaxis);

				Vector3 xaxis = SIMDVec3Cross(up, zaxis);
				xaxis = SIMDVec3Normalize(xaxis);
				Vector3 yaxis = SIMDVec3Cross(zaxis, xaxis);

				float tx = SIMDVec3Dot(xaxis, cameraPosition);
				float ty = SIMDVec3Dot(yaxis, cameraPosition);
				float tz = SIMDVec3Dot(zaxis, cameraPosition);
				
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
			#elif APOC3D_MATH_IMPL == APOC3D_DEFAULT
				Vector3 zaxis = cameraPosition - cameraTarget;
				zaxis.NormalizeInPlace();

				Vector3 xaxis = Vector3::Cross(up, zaxis);
				xaxis.NormalizeInPlace();

				Vector3 yaxis = Vector3::Cross(zaxis, xaxis);

				res.M11 = xaxis.X;
				res.M12 = yaxis.X;
				res.M13 = zaxis.X;

				res.M21 = xaxis.Y;
				res.M22 = yaxis.Y;
				res.M23 = zaxis.Y;

				res.M31 = xaxis.Z;
				res.M32 = yaxis.Z;
				res.M33 = zaxis.Z;
				res.M14 = res.M24 = res.M34 = 0.0f;
				res.M41 = -Vector3::Dot(xaxis, cameraPosition);
				res.M42 = -Vector3::Dot(yaxis, cameraPosition);
				res.M43 = -Vector3::Dot(zaxis, cameraPosition);
				res.M44 = 1.0f;
			#endif
			}
			
			/**
			 *  Creates a left-handed, perspective projection matrix based on a field of view.
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
			#if APOC3D_MATH_IMPL == APOC3D_SSE
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
			#elif APOC3D_MATH_IMPL == APOC3D_DEFAULT
				float yScale = 1.0f / (tanf(fieldOfView * 0.5f));
				float xScale = yScale / aspectRatio;
				res.M11 = xScale;
				res.M22 = yScale;

				res.M12 = res.M13 = res.M14 =
				res.M21 = res.M23 = res.M24 =
				res.M41 = res.M42 = res.M44 =
				res.M31 = res.M32 = 0.0f;

				res.M33 = farPlaneDistance / (farPlaneDistance - nearPlaneDistance);
				res.M34 = 1.0f;

				res.M43 = -(nearPlaneDistance * farPlaneDistance) / (farPlaneDistance - nearPlaneDistance);
			#endif
			}

			/**
			 *  Creates a right-handed, perspective projection matrix based on a field of view.
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
			#if APOC3D_MATH_IMPL == APOC3D_SSE
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
			#elif APOC3D_MATH_IMPL == APOC3D_DEFAULT
				float yScale = 1.0f / (tanf(fieldOfView * 0.5f));
				float xScale = yScale / aspectRatio;
				res.M11 = xScale;
				res.M22 = yScale;

				res.M12 = res.M13 = res.M14 =
				res.M21 = res.M23 = res.M24 =
				res.M41 = res.M42 = res.M44 =
				res.M31 = res.M32 = 0.0f;

				res.M33 = farPlaneDistance / (nearPlaneDistance - farPlaneDistance);
				res.M34 = -1.0f;

				res.M43 = (nearPlaneDistance * farPlaneDistance) / (nearPlaneDistance - farPlaneDistance);

			#endif
			}


			/**
			 *  Creates a left-handed, orthographic projection matrix.
			 *  
			 *  All the parameters of the function are distances in camera space. The parameters describe the dimensions of the view volume.
			 *   2/w  0    0           0
			 *   0    2/h  0           0
			 *   0    0    1/(zf-zn)   0
			 *   0    0   -zn/(zf-zn)  1
			 */
			static void CreateOrthoLH(Matrix& res, float width, float height, float zNearPlane, float zFarPlane)
			{
			#if APOC3D_MATH_IMPL == APOC3D_SSE
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
			#elif APOC3D_MATH_IMPL == APOC3D_DEFAULT
				res.M11 = 2.0f / width;
				res.M22 = 2.0f / height;

				res.M12 = res.M13 = res.M14 =
				res.M21 = res.M23 = res.M24 =
				res.M31 = res.M32 = res.M34 =
				res.M41 = res.M42 = 0.0f;

				res.M33 = 1.0f / (zFarPlane - zNearPlane);
				res.M43 = -zNearPlane / (zFarPlane - zNearPlane);
				res.M44 = 1.0f;
			#endif
			}

			/**
			 *  Creates a right-handed, orthographic projection matrix.
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
			#if APOC3D_MATH_IMPL == APOC3D_SSE
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
			#elif APOC3D_MATH_IMPL == APOC3D_DEFAULT
				res.M11 = 2.0f / width;
				res.M22 = 2.0f / height;

				res.M12 = res.M13 = res.M14 =
				res.M21 = res.M23 = res.M24 =
				res.M31 = res.M32 = res.M34 =
				res.M41 = res.M42 = 0.0f;

				res.M33 = 1.0f / (zNearPlane - zFarPlane);
				res.M43 = zNearPlane / (zNearPlane - zFarPlane);
				res.M44 = 1.0f;
			#endif
			}

			/**
			 *  Creates a spherical billboard that rotates around a specified object position.
			 */
			static void CreateBillboard(const Vector3 &objectPosition, const Vector3 &cameraPosition, const Vector3 &cameraUpVector, const Vector3 &cameraForwardVector, Matrix& res);

			static void Inverse(Matrix& res, const Matrix& matrix)
			{
				res = matrix;
				res.Inverse();
			}

			operator float* ()
			{
#if APOC3D_MATH_IMPL == APOC3D_SSE
#pragma error "Not implemented"
#else
				return reinterpret_cast<float*>(this);
#endif
			}
			operator const float*() const
			{
#if APOC3D_MATH_IMPL == APOC3D_SSE
#pragma error "Not implemented"
#else
				return reinterpret_cast<const float*>(this);
#endif
			}

			bool operator ==(const Matrix& value) const
			{
				return (M11 == value.M11 && M12 == value.M12 && M13 == value.M13 && M14 == value.M14 &&
					M21 == value.M21 && M22 == value.M22 && M23 == value.M23 && M24 == value.M24 &&
					M31 == value.M31 && M32 == value.M32 && M33 == value.M33 && M34 == value.M34 &&
					M41 == value.M41 && M42 == value.M42 && M43 == value.M43 && M44 == value.M44);
			}
			bool operator!=(const Matrix& rhs) const { return !(*this == rhs); }

//			friend static bool operator ==(const Matrix& left, const Matrix& right)
//			{
//				if (&left == &right)
//					return true;
//			#if APOC3D_MATH_IMPL == APOC3D_SSE
//#pragma error "Not implemented"
//			#else
//				return (left.M11 == right.M11 && left.M12 == right.M12 && left.M13 == right.M13 && left.M14 == right.M14 &&
//					left.M21 == right.M21 && left.M22 == right.M22 && left.M23 == right.M23 && left.M24 == right.M24 &&
//					left.M31 == right.M31 && left.M32 == right.M32 && left.M33 == right.M33 && left.M34 == right.M34 &&
//					left.M41 == right.M41 && left.M42 == right.M42 && left.M43 == right.M43 && left.M44 == right.M44);
//			#endif
//			}

		};

#ifdef _MSC_VER
#pragma warning (pop)
#endif

#if APOC3D_MATH_IMPL == APOC3D_SSE
#pragma pack(pop)
#endif


	}
}

#endif