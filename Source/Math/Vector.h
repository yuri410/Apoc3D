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

#define VEC_INDEX_X 3
#define VEC_INDEX_Y 2
#define VEC_INDEX_Z 1
#define VEC_INDEX_W 0

#define VEC_ADDR(x) (x*4)
#define VEC_ADDR_X VEC_ADDR(0)
#define VEC_ADDR_Y VEC_ADDR(1)
#define VEC_ADDR_Z VEC_ADDR(2)

#include <xmmintrin.h>

#define _MM_SHUFFLE1(x) _MM_SHUFFLE(x,x,x,x)

namespace Apoc3D
{
	namespace Math
	{
		const __m128 _MASKSIGN_;

		/* Defines a four component vector.
		*/
		typedef __m128 Vector;

		inline Vector VecLoad(const float vec[4])
		{
			__m128 v = _mm_load_ps(&vec[0]);
			return _mm_shuffle_ps(v, v, 
				_MM_SHUFFLE(VEC_INDEX_W, VEC_INDEX_Z, VEC_INDEX_Y, VEC_INDEX_X));
		};
		inline Vector VecLoad(float f)
		{
			return _mm_set1_ps(f);
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
			Vector scale = _mm_set1_ps(vb);
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
			Vector vv = _mm_shuffle_ps(v, v, 
				_MM_SHUFFLE(VEC_INDEX_W, VEC_INDEX_Z, VEC_INDEX_Y, VEC_INDEX_X));
			_mm_store_ps(pVec, vv);
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
		inline Vector VecCross(Vector va, Vector vb)
		{
			Vector l1, l2, m1, m2;
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
		inline float VecDot(Vector va, Vector vb)
		{
			Vector t0 = _mm_mul_ps(va, vb);

			Vector t1 = _mm_shuffle_ps(t0, t0, _MM_SHUFFLE(VEC_INDEX_Y,VEC_INDEX_Z,VEC_INDEX_X,VEC_INDEX_W));
			Vector t2 = _mm_add_ps(t0, t1);
			Vector t3 = _mm_shuffle_ps(t2, t2, _MM_SHUFFLE(VEC_INDEX_Z,VEC_INDEX_X,VEC_INDEX_Y,VEC_INDEX_W));
			Vector dot = _mm_add_ps(t3, t2);
			
			return GetX(dot);
		};

		
		/*  Calculates the dot product of two vectors.
		*/
		inline Vector VecDot2(Vector va, Vector vb)
		{
			Vector t0 = _mm_mul_ps(va, vb);
			
			Vector t1 = _mm_shuffle_ps(t0, t0, _MM_SHUFFLE(VEC_INDEX_Y,VEC_INDEX_Z,VEC_INDEX_X,VEC_INDEX_W));
			Vector t2 = _mm_add_ps(t0, t1);
			Vector t3 = _mm_shuffle_ps(t2, t2, _MM_SHUFFLE(VEC_INDEX_Z,VEC_INDEX_X,VEC_INDEX_Y,VEC_INDEX_W));
			Vector dot = _mm_add_ps(t3, t2);

			return dot;
		};

		
		/*	NewtonRaphson Reciprocal Square Root
	  	0.5 * rsqrtps * (3 - x * rsqrtps(x) * rsqrtps(x)) */
#pragma warning(push)
#pragma warning(disable : 4640)
		inline Vector rsqrt_nr(Vector a)
		{
			static const Vector fvecf0pt5 = VecLoad(0.5f);
			static const Vector fvecf3pt0 = VecLoad(3.0f);
			Vector Ra0 = _mm_rsqrt_ps(a);

			Vector l = _mm_mul_ps(fvecf0pt5 , Ra0);
			
			Vector r = _mm_mul_ps(a , Ra0);
			r = _mm_mul_ps(r , Ra0);

			r = _mm_sub_ps(fvecf3pt0, r);

			return _mm_sub_ps(l,r);// (fvecf0pt5 * Ra0) * (fvecf3pt0 - (a * Ra0) * Ra0);
		}
#pragma warning(pop)



		/* Calculates the squared length of a specified vector.
		*/
		inline float VecLengthSquared(Vector va)
		{
			return VecDot(va, va);
		};

		/* Calculates the length of a specified vector.
		*/
		inline float VecLength(Vector va)
		{			
			Vector dot = VecDot2(va,va);

			dot = _mm_sqrt_ps(dot);
			
			return GetX(dot);
		};


		
		/* Calculates the distance between two vectors.
		*/
		inline float VecDistance(Vector va, Vector vb)
		{
			Vector d = _mm_sub_ps(va, vb);
			return VecLength(d);
		}
		/* Calculates the squared distance between two vectors.
		*/
		inline float VecDistanceSquared(Vector va, Vector vb)
		{
			Vector d = _mm_sub_ps(va, vb);
			return VecLengthSquared(d);
		}


		/* Converts a specified vector into a unit vector.
		*/
		inline Vector VecNormalize(Vector va)
		{
			Vector r = _mm_mul_ps(va,va); // xx, yy, zz, ww			
			r = _mm_shuffle_ps(va,va, _MM_SHUFFLE(VEC_INDEX_W, VEC_INDEX_Z, VEC_INDEX_Y, VEC_INDEX_X)); // ww, zz, yy, xx

			Vector t0 = _mm_movehl_ps(r,r);// ww, zz, ww, zz
			Vector t1 = _mm_shuffle_ps(r,r, 1); // xx, xx, xx, yy


			r = _mm_add_ps(t0,r); // ww+ww, zz+zz, yy+ww, xx+zz
			
			Vector t = _mm_add_ss(t1, r); // ww+ww+xx, zz+zz+xx, yy+ww+xx, xx+yy+zz
#ifdef ZERO_VECTOR

			static const Vector vecZero = _mm_setzero_ps();
			t = _mm_cmpneq_ss(t, vecZero) & rsqrt_nr(t);
#else
			t = rsqrt_nr(t);
#endif
			return _mm_mul_ps(va, _mm_shuffle_ps(t,t,0x00));
		};
		

		/* Returns the reflection of a vector off a surface that has the specified normal. 
		*/
		inline Vector VecReflect(Vector Incident, Vector Normal)
		{
			// Result = Incident - (2 * dot(Incident, Normal)) * Normal
			Vector Result = VecDot2(Incident,Normal);
			Result = _mm_add_ps(Result,Result);
			Result = _mm_mul_ps(Result,Normal);
			Result = _mm_sub_ps(Incident,Result);
			return Result;
		};
		
		/* Returns a vector containing the smallest components of the specified vectors.
		*/
		inline Vector VecMin(Vector va, Vector vb)
		{
			return _mm_min_ps(va, vb);
		};

		/* Returns a vector containing the largest components of the specified vectors.
		*/
		inline Vector VecMax(Vector va, Vector vb)
		{
			return _mm_max_ps(va, vb);
		};

	}
}

#endif