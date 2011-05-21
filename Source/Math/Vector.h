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
#ifndef VECTOR_H
#define VECTOR_H

#define EPSILON 0.00001f

#if APOC3D_MATH_IMPL == APOC3D_SSE
#define ZERO_VECTOR

#define VEC_INDEX_X 0
#define VEC_INDEX_Y 1
#define VEC_INDEX_Z 2
#define VEC_INDEX_W 3

#define VEC_ADDR(x) (x*4)
#define VEC_ADDR_X VEC_ADDR(0)
#define VEC_ADDR_Y VEC_ADDR(1)
#define VEC_ADDR_Z VEC_ADDR(2)

#include <xmmintrin.h>

#define _MM_SHUFFLE1(x) _MM_SHUFFLE(x,x,x,x)
#elif APOC3D_MATH_IMPL == APOC3D_DEFAULT

#endif

namespace Apoc3D
{
	namespace Math
	{

#if APOC3D_MATH_IMPL == APOC3D_SSE
		const __m128 _MASKSIGN_;

		/* Defines a four component vector.
		*/
		typedef __m128 Vector;

		typedef __m128 Vector3;

		const Vector ZeroVec;
		const Vector UnitXVec;
		const Vector UnitYVec;
		const Vector UnitZVec;

		inline Vector VecLoad(const float vec[4])
		{
			return _mm_load_ps(&vec[0]);
		};
		inline Vector VecLoad(float f)
		{
			return _mm_set1_ps(f);
		};
		inline Vector VecLoad(float x, float y, float z)
		{
			float vec[4] = {x,y,z,0};
			return _mm_load_ps(vec);
		};
		/* Adds two vectors.
		*/
		inline Vector VecAdd(Vector va, Vector vb)
		{
			return _mm_add_ps(va, vb);
		};

		/* Subtracts two vectors.
		*/
		inline Vector VecSub(Vector va, Vector vb)
		{
			return _mm_sub_ps(va, vb);
		};

		/* Modulates a vector by another.
		*/
		inline Vector VecMul(Vector va, Vector vb)
		{
			return _mm_mul_ps(va, vb);
		};

		/* Scales a vector by the given value.
		*/
		inline Vector VecMul(Vector va, float vb)
		{
			Vector3 scale = _mm_set1_ps(vb);
			return _mm_mul_ps(va, scale);			
		}


		inline Vector VecDiv(Vector va, Vector vb)
		{
			return _mm_div_ps(va, vb);
		};
		
		inline Vector VecDiv(Vector va, float vb)
		{
			__m128 dd = _mm_set1_ps(vb);
			return _mm_div_ps(va, dd);
		};

		inline Vector VecStore(float* pVec, Vector v)
		{
			_mm_store_ps(pVec, v);
		};

		/* Reverses the direction of a given vector.
		*/
		inline Vector VecNegate(Vector va)
		{
			return _mm_xor_ps(_MASKSIGN_, va);
		}
			 
		inline float GetX(Vector va)
		{
			float result;
			__asm
			{
				lea		eax, va
				fld		float ptr [eax+VEC_ADDR_X]
				fstp	float ptr result
			}
			return result;
		}
		inline float GetY(Vector va)
		{
			float result;
			__asm
			{
				lea eax, va
				fld float ptr [eax+VEC_ADDR_Y]
				fstp float ptr result;
			}
			return result;
		}
		inline float GetZ(Vector va)
		{
			float result;
			__asm
			{
				lea eax, va
				fld float ptr [eax+VEC_ADDR_Z]
				fstp float ptr result;
			}
			return result;
		}

		//inline Vector VecBc(Vector v)
		//{
		//	return _mm_shuffle_ps(v,v, _MM_SHUFFLE(3,3,3,3));
		//};

		/* Calculates the cross product of two vectors.
		*/
		inline Vector3 Vec3Cross(Vector3 va, Vector3 vb)
		{
			Vector3 l1, l2, m1, m2;
			l1 = _mm_shuffle_ps(va,va, _MM_SHUFFLE(VEC_INDEX_Y,VEC_INDEX_Z,VEC_INDEX_X,VEC_INDEX_W));
			l2 = _mm_shuffle_ps(vb,vb, _MM_SHUFFLE(VEC_INDEX_Z,VEC_INDEX_X,VEC_INDEX_Y,VEC_INDEX_W));
			m2 = _mm_mul_ps(l1,l2);
			l1 = _mm_shuffle_ps(va,va, _MM_SHUFFLE(VEC_INDEX_Z,VEC_INDEX_X,VEC_INDEX_Y,VEC_INDEX_W));
			l2 = _mm_shuffle_ps(vb,vb, _MM_SHUFFLE(VEC_INDEX_Y,VEC_INDEX_Z,VEC_INDEX_X,VEC_INDEX_W));
			m1 = _mm_mul_ps(l1,l2);
			return _mm_sub_ps( m1,m2);
		}

		/*  Calculates the dot product of two vectors.
		*/
		inline float Vec3Dot(Vector3 va, Vector3 vb)
		{
			Vector3 t0 = _mm_mul_ps(va, vb);

			Vector3 t1 = _mm_shuffle_ps(t0, t0, _MM_SHUFFLE(VEC_INDEX_Y,VEC_INDEX_Z,VEC_INDEX_X,VEC_INDEX_W));
			Vector3 t2 = _mm_add_ps(t0, t1);
			Vector3 t3 = _mm_shuffle_ps(t2, t2, _MM_SHUFFLE(VEC_INDEX_Z,VEC_INDEX_X,VEC_INDEX_Y,VEC_INDEX_W));
			Vector3 dot = _mm_add_ps(t3, t2);
			
			return reinterpret_cast<float&>(dot);
		};
		/*  Calculates the dot product of two vectors.
		*/
		inline float Vec4Dot(Vector va, Vector vb)
		{
			Vector t0 = _mm_mul_ps(va, vb);

			Vector t1 = _mm_shuffle_ps(t0, t0, _MM_SHUFFLE(VEC_INDEX_Y,VEC_INDEX_Z,VEC_INDEX_X,VEC_INDEX_X));
			Vector t2 = _mm_add_ps(t0, t1);
			Vector t3 = _mm_shuffle_ps(t2, t2, _MM_SHUFFLE(VEC_INDEX_Z,VEC_INDEX_X,VEC_INDEX_Y,VEC_INDEX_Y));
			Vector t4 = _mm_add_ps(t3, t2);
			Vector t5 = _mm_shuffle_ps(t2, t2, _MM_SHUFFLE(VEC_INDEX_W,VEC_INDEX_W,VEC_INDEX_W,VEC_INDEX_Z));
			Vector dot = _mm_add_ps(t4, t5);

			return reinterpret_cast<float&>(dot);
		};
		
		/*  Calculates the dot product of two vectors.
		*/
		inline Vector3 Vec3Dot2(Vector3 va, Vector3 vb)
		{
			Vector3 t0 = _mm_mul_ps(va, vb);
			
			Vector3 t1 = _mm_shuffle_ps(t0, t0, _MM_SHUFFLE(VEC_INDEX_Y,VEC_INDEX_Z,VEC_INDEX_X,VEC_INDEX_W));
			Vector3 t2 = _mm_add_ps(t0, t1);
			Vector3 t3 = _mm_shuffle_ps(t2, t2, _MM_SHUFFLE(VEC_INDEX_Z,VEC_INDEX_X,VEC_INDEX_Y,VEC_INDEX_W));
			Vector3 dot = _mm_add_ps(t3, t2);

			return dot;
		};
		/*  Calculates the dot product of two vectors.
		*/
		inline Vector Vec4Dot2(Vector va, Vector vb)
		{
			Vector t0 = _mm_mul_ps(va, vb);

			Vector t1 = _mm_shuffle_ps(t0, t0, _MM_SHUFFLE(VEC_INDEX_Y,VEC_INDEX_Z,VEC_INDEX_X,VEC_INDEX_X));
			Vector t2 = _mm_add_ps(t0, t1);
			Vector t3 = _mm_shuffle_ps(t2, t2, _MM_SHUFFLE(VEC_INDEX_Z,VEC_INDEX_X,VEC_INDEX_Y,VEC_INDEX_Y));
			Vector t4 = _mm_add_ps(t3, t2);
			Vector t5 = _mm_shuffle_ps(t2, t2, _MM_SHUFFLE(VEC_INDEX_W,VEC_INDEX_W,VEC_INDEX_W,VEC_INDEX_Z));
			Vector dot = _mm_add_ps(t4, t5);

			return dot;
		};
		
		/*	NewtonRaphson Reciprocal Square Root
	  	0.5 * rsqrtps * (3 - x * rsqrtps(x) * rsqrtps(x)) */
#pragma warning(push)
#pragma warning(disable : 4640)
		inline Vector3 rsqrt_nr(Vector3 a)
		{
			static const Vector3 fvecf0pt5 = VecLoad(0.5f);
			static const Vector3 fvecf3pt0 = VecLoad(3.0f);
			Vector3 Ra0 = _mm_rsqrt_ps(a);

			Vector3 l = _mm_mul_ps(fvecf0pt5 , Ra0);
			
			Vector3 r = _mm_mul_ps(a , Ra0);
			r = _mm_mul_ps(r , Ra0);

			r = _mm_sub_ps(fvecf3pt0, r);

			return _mm_sub_ps(l,r);// (fvecf0pt5 * Ra0) * (fvecf3pt0 - (a * Ra0) * Ra0);
		}
#pragma warning(pop)



		/* Calculates the squared length of a specified vector.
		*/
		inline float Vec3LengthSquared(Vector3 va)
		{
			return Vec3Dot(va, va);
		};

		/* Calculates the length of a specified vector.
		*/
		inline float Vec3Length(Vector3 va)
		{			
			Vector3 dot = Vec3Dot2(va,va);

			dot = _mm_sqrt_ps(dot);
			
			return GetX(dot);
		};


		
		/* Calculates the distance between two vectors.
		*/
		inline float Vec3Distance(Vector3 va, Vector3 vb)
		{
			Vector3 d = _mm_sub_ps(va, vb);
			return Vec3Length(d);
		}
		/* Calculates the squared distance between two vectors.
		*/
		inline float Vec3DistanceSquared(Vector3 va, Vector3 vb)
		{
			Vector3 d = _mm_sub_ps(va, vb);
			return Vec3LengthSquared(d);
		}


		/* Converts a specified vector into a unit vector.
		*/
		inline Vector3 Vec3Normalize(Vector3 va)
		{
			Vector3 t = Vec3Dot2(va, va);
#ifdef ZERO_VECTOR

			static const Vector3 vecZero = _mm_setzero_ps();
			t = _mm_and_ps(_mm_cmpneq_ss(_mm_shuffle_ps( t, t, VEC_INDEX_X), vecZero), rsqrt_nr(t));
#else
			t = rsqrt_nr(t);
#endif
			return _mm_mul_ps(va, _mm_shuffle_ps(t,t,0x00));
		};

		/* Converts a specified vector into a unit vector.
		*/
		inline Vector Vec4Normalize(Vector va)
		{
			Vector t = Vec4Dot2(va, va);
#ifdef ZERO_VECTOR

			static const Vector vecZero = _mm_setzero_ps();
			t = _mm_and_ps(_mm_cmpneq_ss(t, vecZero), rsqrt_nr(t));
#else
			t = rsqrt_nr(t);
#endif
			return _mm_mul_ps(va, _mm_shuffle_ps(t,t,0x00));
		};

		/* Returns the reflection of a vector off a surface that has the specified normal. 
		*/
		inline Vector3 VecReflect(Vector3 Incident, Vector3 Normal)
		{
			// Result = Incident - (2 * dot(Incident, Normal)) * Normal
			Vector3 Result = Vec3Dot2(Incident,Normal);
			Result = _mm_add_ps(Result,Result);
			Result = _mm_mul_ps(Result,Normal);
			Result = _mm_sub_ps(Incident,Result);
			return Result;
		};
		
		/* Returns a vector containing the smallest components of the specified vectors.
		*/
		inline Vector3 VecMin(Vector3 va, Vector3 vb)
		{
			return _mm_min_ps(va, vb);
		};

		/* Returns a vector containing the largest components of the specified vectors.
		*/
		inline Vector3 VecMax(Vector3 va, Vector3 vb)
		{
			return _mm_max_ps(va, vb);
		};
#elif APOC3D_MATH_IMPL == APOC3D_DEFAULT
		class APAPI Vector3
		{
		public:
			/* the X component of the vector
			*/
			float X;
			/* the Y component of the vector
			*/
			float Y;
			/* the Z component of the vector
			*/
			float Z;

			/* a Vector3 with all of its components set to zero.
			*/
			static const Vector3 Zero;
			/* X unit Vector3 (1, 0, 0).
			*/
			static const Vector3 UnitX;
			/* Y unit Vector3 (0, 1, 0).
			*/
			static const Vector3 UnitY;
			/* Z unit Vector3 (0, 0, 1).
			*/
			static const Vector3 UnitZ;

			float* GetElement(int index)
			{				
				return reinterpret_cast<float*>(&X) + index;
			}
			const float* GetElement(int index) const
			{				
				return reinterpret_cast<const float*>(&X) + index;
			}

			Vector3(float value)
				: X(value), Y(value), Z(value)
			{				
			}
		};
#endif
		

	}
}

#endif