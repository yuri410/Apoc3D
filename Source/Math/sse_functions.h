

#if APOC3D_MATH_IMPL == APOC3D_SSE

#include "Common.h"

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

const __m128 _MASKSIGN_;

inline __m128 VecLoad(const float vec[4])
{
	return _mm_load_ps(&vec[0]);
};
inline __m128 VecLoad(float f)
{
	return _mm_set1_ps(f);
};
inline __m128 VecLoad(float x, float y, float z)
{
	float vec[4] = {x,y,z,0};
	return _mm_load_ps(vec);
};
/* Adds two vectors.
*/
inline __m128 VecAdd(__m128 va, __m128 vb)
{
	return _mm_add_ps(va, vb);
};

/* Subtracts two vectors.
*/
inline __m128 VecSub(__m128 va, __m128 vb)
{
	return _mm_sub_ps(va, vb);
};

/* Modulates a vector by another.
*/
inline __m128 VecMul(__m128 va, __m128 vb)
{
	return _mm_mul_ps(va, vb);
};

/* Scales a vector by the given value.
*/
inline __m128 VecMul(__m128 va, float vb)
{
	__m128 scale = _mm_set1_ps(vb);
	return _mm_mul_ps(va, scale);			
}


inline __m128 VecDiv(__m128 va, __m128 vb)
{
	return _mm_div_ps(va, vb);
};
		
inline __m128 VecDiv(__m128 va, float vb)
{
	__m128 dd = _mm_set1_ps(vb);
	return _mm_div_ps(va, dd);
};

inline __m128 VecStore(float* pVec, __m128 v)
{
	_mm_store_ps(pVec, v);
};

/* Reverses the direction of a given vector.
*/
inline __m128 VecNegate(__m128 va)
{
	return _mm_xor_ps(_MASKSIGN_, va);
}


/*	NewtonRaphson Reciprocal Square Root
0.5 * rsqrtps * (3 - x * rsqrtps(x) * rsqrtps(x)) */
#pragma warning(push)
#pragma warning(disable : 4640)
inline __m128 rsqrt_nr(__m128 a)
{
	static const __m128 fvecf0pt5 = VecLoad(0.5f);
	static const __m128 fvecf3pt0 = VecLoad(3.0f);
	__m128 Ra0 = _mm_rsqrt_ps(a);

	__m128 l = _mm_mul_ps(fvecf0pt5 , Ra0);
			
	__m128 r = _mm_mul_ps(a , Ra0);
	r = _mm_mul_ps(r , Ra0);

	r = _mm_sub_ps(fvecf3pt0, r);

	return _mm_sub_ps(l,r);// (fvecf0pt5 * Ra0) * (fvecf3pt0 - (a * Ra0) * Ra0);
}
#pragma warning(pop)


		
/* Returns a vector containing the smallest components of the specified vectors.
*/
inline __m128 VecMin(__m128 va, __m128 vb)
{
	return _mm_min_ps(va, vb);
};

/* Returns a vector containing the largest components of the specified vectors.
*/
inline __m128 VecMax(__m128 va, __m128 vb)
{
	return _mm_max_ps(va, vb);
};


/* Element Count Dependent */


/* Calculates the cross product of two vectors.
*/
inline __m128 Vec3Cross(__m128 va, __m128 vb)
{
	__m128 l1, l2, m1, m2;
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
inline float Vec2Dot(__m128 va, __m128 vb)
{
	__m128 t0 = _mm_mul_ps(va, vb);

	__m128 t1 = _mm_shuffle_ps(t0, t0, _MM_SHUFFLE(VEC_INDEX_Y,VEC_INDEX_X,VEC_INDEX_Z,VEC_INDEX_W));
	__m128 dot = _mm_add_ps(t0, t1);

	return reinterpret_cast<const float&>(dot);
};
inline float Vec3Dot(__m128 va, __m128 vb)
{
	__m128 t0 = _mm_mul_ps(va, vb);

	__m128 t1 = _mm_shuffle_ps(t0, t0, _MM_SHUFFLE(VEC_INDEX_Y,VEC_INDEX_Z,VEC_INDEX_X,VEC_INDEX_W));
	__m128 t2 = _mm_add_ps(t0, t1);
	__m128 t3 = _mm_shuffle_ps(t2, t2, _MM_SHUFFLE(VEC_INDEX_Z,VEC_INDEX_X,VEC_INDEX_Y,VEC_INDEX_W));
	__m128 dot = _mm_add_ps(t3, t2);
			
	return reinterpret_cast<const float&>(dot);
};
inline float Vec4Dot(__m128 va, __m128 vb)
{
	__m128 t0 = _mm_mul_ps(va, vb);

	__m128 t1 = _mm_shuffle_ps(t0, t0, _MM_SHUFFLE(VEC_INDEX_Y,VEC_INDEX_Z,VEC_INDEX_X,VEC_INDEX_X));
	__m128 t2 = _mm_add_ps(t0, t1);
	__m128 t3 = _mm_shuffle_ps(t2, t2, _MM_SHUFFLE(VEC_INDEX_Z,VEC_INDEX_X,VEC_INDEX_Y,VEC_INDEX_Y));
	__m128 t4 = _mm_add_ps(t3, t2);
	__m128 t5 = _mm_shuffle_ps(t2, t2, _MM_SHUFFLE(VEC_INDEX_W,VEC_INDEX_W,VEC_INDEX_W,VEC_INDEX_Z));
	__m128 dot = _mm_add_ps(t4, t5);

	return reinterpret_cast<const float&>(dot);
};
		
/*  Calculates the dot product of two vectors.
*/
inline __m128 Vec2Dot2(__m128 va, __m128 vb)
{
	__m128 t0 = _mm_mul_ps(va, vb);
			
	__m128 t1 = _mm_shuffle_ps(t0, t0, _MM_SHUFFLE(VEC_INDEX_Y,VEC_INDEX_Z,VEC_INDEX_X,VEC_INDEX_W));
	__m128 dot = _mm_add_ps(t0, t1);

	return dot;
};
inline __m128 Vec3Dot2(__m128 va, __m128 vb)
{
	__m128 t0 = _mm_mul_ps(va, vb);
			
	__m128 t1 = _mm_shuffle_ps(t0, t0, _MM_SHUFFLE(VEC_INDEX_Y,VEC_INDEX_Z,VEC_INDEX_X,VEC_INDEX_W));
	__m128 t2 = _mm_add_ps(t0, t1);
	__m128 t3 = _mm_shuffle_ps(t2, t2, _MM_SHUFFLE(VEC_INDEX_Z,VEC_INDEX_X,VEC_INDEX_Y,VEC_INDEX_W));
	__m128 dot = _mm_add_ps(t3, t2);

	return dot;
};
inline __m128 Vec4Dot2(__m128 va, __m128 vb)
{
	__m128 t0 = _mm_mul_ps(va, vb);

	__m128 t1 = _mm_shuffle_ps(t0, t0, _MM_SHUFFLE(VEC_INDEX_Y,VEC_INDEX_Z,VEC_INDEX_X,VEC_INDEX_X));
	__m128 t2 = _mm_add_ps(t0, t1);
	__m128 t3 = _mm_shuffle_ps(t2, t2, _MM_SHUFFLE(VEC_INDEX_Z,VEC_INDEX_X,VEC_INDEX_Y,VEC_INDEX_Y));
	__m128 t4 = _mm_add_ps(t3, t2);
	__m128 t5 = _mm_shuffle_ps(t2, t2, _MM_SHUFFLE(VEC_INDEX_W,VEC_INDEX_W,VEC_INDEX_W,VEC_INDEX_Z));
	__m128 dot = _mm_add_ps(t4, t5);

	return dot;
};
		
/* Calculates the length of a specified vector.
*/
inline float Vec2Length(__m128 va)
{			
	__m128 dot = Vec2Dot2(va,va);

	dot = _mm_sqrt_ps(dot);
			
	return reinterpret_cast<const float&>(dot);
};
inline float Vec3Length(__m128 va)
{			
	__m128 dot = Vec3Dot2(va,va);

	dot = _mm_sqrt_ps(dot);
			
	return reinterpret_cast<const float&>(dot);
};
inline float Vec4Length(__m128 va)
{			
	__m128 dot = Vec4Dot2(va,va);

	dot = _mm_sqrt_ps(dot);
			
	return reinterpret_cast<const float&>(dot);
};

/* Calculates the squared length of a specified vector.
*/
inline float Vec2LengthSquared(__m128 va)
{
	return Vec2Dot(va, va);
};
inline float Vec3LengthSquared(__m128 va)
{
	return Vec3Dot(va, va);
};
inline float Vec4LengthSquared(__m128 va)
{
	return Vec4Dot(va, va);
};

/* Calculates the distance between two vectors.
*/
inline float Vec2Distance(__m128 va, __m128 vb)
{
	__m128 d = _mm_sub_ps(va, vb);
	return Vec2Length(d);
}
inline float Vec3Distance(__m128 va, __m128 vb)
{
	__m128 d = _mm_sub_ps(va, vb);
	return Vec3Length(d);
}
inline float Vec4Distance(__m128 va, __m128 vb)
{
	__m128 d = _mm_sub_ps(va, vb);
	return Vec4Length(d);
}

/* Calculates the squared distance between two vectors.
*/
inline float Vec2DistanceSquared(__m128 va, __m128 vb)
{
	__m128 d = _mm_sub_ps(va, vb);
	return Vec2LengthSquared(d);
}
inline float Vec3DistanceSquared(__m128 va, __m128 vb)
{
	__m128 d = _mm_sub_ps(va, vb);
	return Vec3LengthSquared(d);
}
inline float Vec4DistanceSquared(__m128 va, __m128 vb)
{
	__m128 d = _mm_sub_ps(va, vb);
	return Vec4LengthSquared(d);
}

/* Converts a specified vector into a unit vector.
*/
inline __m128 Vec2Normalize(__m128 va)
{
	__m128 t = Vec2Dot2(va, va);
#ifdef ZERO_VECTOR

	static const __m128 vecZero = _mm_setzero_ps();
	t = _mm_and_ps(_mm_cmpneq_ss(_mm_shuffle_ps( t, t, VEC_INDEX_X), vecZero), rsqrt_nr(t));
#else
	t = rsqrt_nr(t);
#endif
	return _mm_mul_ps(va, _mm_shuffle_ps(t,t,0x00));
};
inline __m128 Vec3Normalize(__m128 va)
{
	__m128 t = Vec3Dot2(va, va);
#ifdef ZERO_VECTOR

	static const __m128 vecZero = _mm_setzero_ps();
	t = _mm_and_ps(_mm_cmpneq_ss(_mm_shuffle_ps( t, t, VEC_INDEX_X), vecZero), rsqrt_nr(t));
#else
	t = rsqrt_nr(t);
#endif
	return _mm_mul_ps(va, _mm_shuffle_ps(t,t,0x00));
};
inline __m128 Vec4Normalize(__m128 va)
{
	__m128 t = Vec4Dot2(va, va);
#ifdef ZERO_VECTOR

	static const __m128 vecZero = _mm_setzero_ps();
	t = _mm_and_ps(_mm_cmpneq_ss(t, vecZero), rsqrt_nr(t));
#else
	t = rsqrt_nr(t);
#endif
	return _mm_mul_ps(va, _mm_shuffle_ps(t,t,0x00));
};




/* Returns the reflection of a vector off a surface that has the specified normal. 
*/
inline __m128 Vec2Reflect(__m128 Incident, __m128 Normal)
{
	// Result = Incident - (2 * dot(Incident, Normal)) * Normal
	__m128 Result = Vec2Dot2(Incident,Normal);
	Result = _mm_add_ps(Result,Result);
	Result = _mm_mul_ps(Result,Normal);
	Result = _mm_sub_ps(Incident,Result);
	return Result;
};
inline __m128 Vec3Reflect(__m128 Incident, __m128 Normal)
{
	// Result = Incident - (2 * dot(Incident, Normal)) * Normal
	__m128 Result = Vec3Dot2(Incident,Normal);
	Result = _mm_add_ps(Result,Result);
	Result = _mm_mul_ps(Result,Normal);
	Result = _mm_sub_ps(Incident,Result);
	return Result;
};
inline __m128 Vec4Reflect(__m128 Incident, __m128 Normal)
{
	// Result = Incident - (2 * dot(Incident, Normal)) * Normal
	__m128 Result = Vec4Dot2(Incident,Normal);
	Result = _mm_add_ps(Result,Result);
	Result = _mm_mul_ps(Result,Normal);
	Result = _mm_sub_ps(Incident,Result);
	return Result;
};
#endif
		