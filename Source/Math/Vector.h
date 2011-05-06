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



#include "Common.h"

namespace Apoc3D
{
	namespace Math
	{
		const __m128 _MASKSIGN_;

		/* Defines a four component vector.
		*/
		typedef __m128 Vector;

		inline Vector VecLoad(const float* pVec)
		{
			return _mm_load_ps(pVec);
		};
		inline Vector VecLoad(float f)
		{
			return _mm_set_ps1(f);
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
			Vector scale = _mm_set_ps(vb,vb,vb,vb);
			return _mm_mul_ps(va, scale);			
		}


		inline Vector VecDiv(Vector va, Vector vb)
		{
			return _mm_div_ps(va, vb);
		};
		
		inline Vector VecDiv(Vector va, float vb)
		{
			__m128 dd = _mm_set_ps1(vb);
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
			 

		inline Vector VecBc(Vector v)
		{
			return _mm_shuffle_ps(v,v, _MM_SHUFFLE(3,3,3,3));
		};

		/* Calculates the cross product of two vectors.
		*/
		inline Vector VecCross(Vector va, Vector vb)
		{
			Vector l1, l2, m1, m2;
			l1 = _mm_shuffle_ps(va,va, _MM_SHUFFLE(3,1,0,2));
			l2 = _mm_shuffle_ps(vb,vb, _MM_SHUFFLE(3,0,2,1));
			m2 = _mm_mul_ps(l1,l2);
			l1 = _mm_shuffle_ps(va,va, _MM_SHUFFLE(3,0,2,1));
			l2 = _mm_shuffle_ps(vb,vb, _MM_SHUFFLE(3,1,0,2));
			m1 = _mm_mul_ps(l1,l2);
			return _mm_sub_ps( m1,m2);
		}

		/*  Calculates the dot product of two vectors.
		*/
		inline Vector VecDot(Vector va, Vector vb)
		{
			Vector t0 = _mm_mul_ps(va, vb);
			Vector t1 = _mm_shuffle_ps(t0, t0, _MM_SHUFFLE(1,0,3,2));
			Vector t2 = _mm_add_ps(t0, t1);
			Vector t3 = _mm_shuffle_ps(t2, t2, _MM_SHUFFLE(2,3,0,1));
			Vector dot = _mm_add_ps(t3, t2);
			return (dot);
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
			Vector t0 = _mm_mul_ps(va, va);
			Vector t1 = _mm_shuffle_ps(t0, t0, _MM_SHUFFLE(1,0,3,2));
			Vector t2 = _mm_add_ps(t0, t1);
			Vector t3 = _mm_shuffle_ps(t2, t2, _MM_SHUFFLE(2,3,0,1));
			Vector dot = _mm_add_ps(t3, t2);

			float result;
			_mm_store_ss(&result, dot);
			return result;
		};

		/* Calculates the length of a specified vector.
		*/
		inline float VecLength(Vector va)
		{
			Vector t0 = _mm_mul_ps(va, va);
			Vector t1 = _mm_shuffle_ps(t0, t0, _MM_SHUFFLE(1,0,3,2));
			Vector t2 = _mm_add_ps(t0, t1);
			Vector t3 = _mm_shuffle_ps(t2, t2, _MM_SHUFFLE(2,3,0,1));
			Vector dot = _mm_add_ps(t3, t2);
			dot = _mm_sqrt_ps(dot);

			float result;
			_mm_store_ss(&result, dot);
			return result;
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
			Vector r = _mm_mul_ps(va,va);
			Vector t0 = _mm_movehl_ps(r,r);

			r = _mm_add_ps(t0,r);

			Vector t1 = _mm_shuffle_ps(r,r, 1);
			Vector t = _mm_add_ss(t1, r);
#ifdef ZERO_VECTOR

			static const Vector vecZero = _mm_setzero_ps();
			t = _mm_cmpneq_ss(t, vecZero) & rsqrt_nr(t);
#else
			t = rsqrt_nr(t);
#endif
			return _mm_mul_ps(va, _mm_shuffle_ps(t,t,0x00));

			//Vector t0 = _mm_mul_ps(va, va);
			//Vector t1 = _mm_shuffle_ps(t0, t0, _MM_SHUFFLE(1,0,3,2));
			//Vector t2 = _mm_add_ps(t0, t1);
			//Vector t3 = _mm_shuffle_ps(t2, t2, _MM_SHUFFLE(2,3,0,1));
			//Vector dot = _mm_add_ps(t3, t2);
			//dot = _mm_rsqrt_ps(dot);
			//
			//dot = _mm_mul_ps(va, dot);
			//return (dot);
		};


		inline void VecGetX(float *p, Vector v)
		{
			_mm_store_ss(p, v);
		};

		/* Returns the reflection of a vector off a surface that has the specified normal. 
		*/
		inline Vector VecReflect(Vector Incident, Vector Normal)
		{
			// Result = Incident - (2 * dot(Incident, Normal)) * Normal
			Vector Result = VecDot(Incident,Normal);
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