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

#endif

namespace Apoc3D
{
	namespace Math
	{

#if APOC3D_MATH_IMPL == APOC3D_SSE
		/* Defines a two component vector.
		*/
		typedef __m128 Vector2;
		/* Defines a three component vector.
		*/
		typedef __m128 Vector3;
		/* Defines a four component vector.
		*/
		typedef __m128 Vector4;
#elif APOC3D_MATH_IMPL == APOC3D_DEFAULT
		/* Defines a two component vector.
		*/
		class APAPI Vector2
		{
		public:
			/* the X component of the vector
			*/
			float X;
			/* the Y component of the vector
			*/
			float Y;
			
			Vector2(){}
			Vector2(float value)
				: X(value), Y(value)
			{ }

			Vector2(float x, float y)
				: X(x), Y(y)
			{ }
		};
		/* Defines a three component vector.
		*/
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


			Vector3(){}
			
			Vector3(float value)
				: X(value), Y(value), Z(value)
			{				
			}
			Vector3(float x,float y,float z)
				: X(x), Y(y), Z(z)
			{

			}
		};
		/* Defines a four component vector.
		*/
		class APAPI Vector4
		{
		public:
			/* The X component of the vector.
			*/
			float X;

			/* The Y component of the vector
			*/
			float Y;

			/* The Z component of the vector.
			*/
			float Z;

			/* The W component of the vector.
			*/
			float W;


			Vector4(){}

			Vector4(float value)
				: X(value), Y(value), Z(value), W(value)
			{				
			}
			Vector4(float x,float y,float z,float w)
				: X(x), Y(y), Z(z), W(w)
			{

			}
		};

#endif







		class APAPI Vector2Utils
		{
		public:
			
			/* a Vector2 with all of its components set to zero.
			*/
			static const Vector2 Zero;
			/* X unit Vector2 (1, 0).
			*/
			static const Vector2 UnitX;
			/* Y unit Vector2 (0, 1).
			*/
			static const Vector3 UnitY;



			/* Adds two vectors.
			*/
			static Vector2 Add(const Vector2& left, const Vector2& right)
			{
				return Vector2(left.X + right.X, left.Y + right.Y);
			}
			/* Subtracts two vectors.
			*/
			static Vector2 Subtract(const Vector2& left, const Vector2& right)
			{
				return Vector2(left.X - right.X, left.Y - right.Y);
			}
			/* Scales a vector by the given value.
			*/
			static Vector2 Multiply(const Vector2& value, float scale)
			{
				return Vector2(value.X * scale, value.Y * scale);
			}
			/* Modulates a vector by another.
			*/
			static Vector2 Modulate(const Vector2& left, const Vector2& right)
			{
				return Vector2(left.X * right.X, left.Y * right.Y);
			}
			/* Scales a vector by the given value.
			*/
			static Vector2 Divide(const Vector2& value, float scale)
			{
				return Vector2(value.X / scale, value.Y / scale);
			}

			static float Cross(const Vector2& a, const Vector2& b)
			{
				return a.X * b.Y - b.X * a.Y;
			}

			/* Reverses the direction of a given vector.
			*/
			static Vector2 Negate(const Vector2& value)
			{
				return Vector2(-value.X, -value.Y);
			}

			/* Returns a Vector2 containing the 2D Cartesian coordinates of a point 
			* specified in Barycentric coordinates relative to a 2D triangle.
			*/
			static Vector2 Barycentric(const Vector2& value1, const Vector2& value2, 
				const Vector2& value3, float amount1, float amount2)
			{
				Vector2 vector;
				vector.X = (value1.X + (amount1 * (value2.X - value1.X))) + (amount2 * (value3.X - value1.X));
				vector.Y = (value1.Y + (amount1 * (value2.Y - value1.Y))) + (amount2 * (value3.Y - value1.Y));
				return vector;
			}
			/* Performs a Catmull-Rom interpolation using the specified positions.
			*/
			static Vector2 CatmullRom(const Vector2& value1, const Vector2& value2, 
				const Vector2& value3, const Vector2& value4, float amount)
			{
				Vector2 vector;
				float squared = amount * amount;
				float cubed = amount * squared;

				vector.X = 0.5f * ((((2.0f * value2.X) + ((-value1.X + value3.X) * amount)) +
					(((((2.0f * value1.X) - (5.0f * value2.X)) + (4.0f * value3.X)) - value4.X) * squared)) +
					((((-value1.X + (3.0f * value2.X)) - (3.0f * value3.X)) + value4.X) * cubed));

				vector.Y = 0.5f * ((((2.0f * value2.Y) + ((-value1.Y + value3.Y) * amount)) +
					(((((2.0f * value1.Y) - (5.0f * value2.Y)) + (4.0f * value3.Y)) - value4.Y) * squared)) +
					((((-value1.Y + (3.0f * value2.Y)) - (3.0f * value3.Y)) + value4.Y) * cubed));

				return vector;
			}
			/* Restricts a value to be within a specified range.
			*/
			static Vector2 Clamp(const Vector2& value, const Vector2& min, const Vector2& max)
			{
				float x = value.X;
				x = (x > max.X) ? max.X : x;
				x = (x < min.X) ? min.X : x;

				float y = value.Y;
				y = (y > max.Y) ? max.Y : y;
				y = (y < min.Y) ? min.Y : y;

				return Vector2(x, y);
			}
			/* Performs a Hermite spline interpolation.
			*/
			static Vector2 Hermite(const Vector2& value1, const Vector2& tangent1,
				const Vector2& value2, const Vector2& tangent2, float amount)
			{
				Vector2 vector;
				float squared = amount * amount;
				float cubed = amount * squared;
				float part1 = ((2.0f * cubed) - (3.0f * squared)) + 1.0f;
				float part2 = (-2.0f * cubed) + (3.0f * squared);
				float part3 = (cubed - (2.0f * squared)) + amount;
				float part4 = cubed - squared;

				vector.X = (((value1.X * part1) + (value2.X * part2)) + (tangent1.X * part3)) + (tangent2.X * part4);
				vector.Y = (((value1.Y * part1) + (value2.Y * part2)) + (tangent1.Y * part3)) + (tangent2.Y * part4);

				return vector;
			}
			/* Performs a linear interpolation between two vectors.
			*/
			static Vector2 Lerp(const Vector2& start, const Vector2& end, float amount)
			{
				Vector2 vector;

				vector.X = start.X + ((end.X - start.X) * amount);
				vector.Y = start.Y + ((end.Y - start.Y) * amount);

				return vector;
			}
			/* Performs a cubic interpolation between two vectors.
			*/
			static Vector2 SmoothStep(const Vector2& start, const Vector2& end, float amount)
			{
				Vector2 vector;

				amount = (amount > 1.0f) ? 1.0f : ((amount < 0.0f) ? 0.0f : amount);
				amount = (amount * amount) * (3.0f - (2.0f * amount));

				vector.X = start.X + ((end.X - start.X) * amount);
				vector.Y = start.Y + ((end.Y - start.Y) * amount);

				return vector;
			}

			/* Calculates the distance between two vectors.
			*/
			static float Distance(const Vector2& value1, const Vector2& value2)
			{
				float x = value1.X - value2.X;
				float y = value1.Y - value2.Y;

				return sqrtf(x * x + y * y);
			}
			/* Calculates the dot product of two vectors.
			*/
			static float Dot(const Vector2& left, const Vector2& right)
			{
				return left.X * right.X + left.Y * right.Y;
			}


			
			/* Calculates the length of the vector.
			*/
			static float Length(const Vector2& v)
			{
				return sqrtf(v.X*v.X + v.Y*v.Y);
			}
			/* Calculates the squared length of the vector.
			*/
			static float LengthSquared(const Vector2& v)
			{
				return v.X*v.X + v.Y*v.Y;
			}

			/* Converts the vector into a unit vector.
			*/
			static Vector2 Normalize(const Vector2& vector)
			{
				float length = Length(vector);
				if (length < EPSILON)					
				{
					return Zero;
				}
				float num = 1.0f / length;
				return Vector2(vector.X * num, vector.Y * num);
			}

			static Vector2 Reflect(const Vector2& vector, const Vector2& normal)
			{
				Vector2 vector2;
				float num = (vector.X * normal.X) + (vector.Y * normal.Y);
				vector2.X = vector.X - ((2.0f * num) * normal.X);
				vector2.Y = vector.Y - ((2.0f * num) * normal.Y);
				return vector2;
			}
			/* Returns a vector containing the smallest components of the specified vectors.
			*/
			static Vector2 Minimize(const Vector2& left, const Vector2& right)
			{
				Vector2 vector;
				vector.X = (left.X < right.X) ? left.X : right.X;
				vector.Y = (left.Y < right.Y) ? left.Y : right.Y;
				return vector;
			}
			/* Returns a vector containing the largest components of the specified vectors.
			*/
			static Vector2 Maximize(const Vector2& left, const Vector2& right)
			{
				Vector2 vector;
				vector.X = (left.X > right.X) ? left.X : right.X;
				vector.Y = (left.Y > right.Y) ? left.Y : right.Y;
				return vector;
			}

		};
		class APAPI Vector3Utils
		{
		public:			
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


		};
		class APAPI Vector4Utils
		{
		public:			
			/* a Vector3 with all of its components set to zero.
			*/
			static const Vector4 Zero;
			/* X unit Vector4 (1, 0, 0, 0).
			*/
			static const Vector4 UnitX;
			/* Y unit Vector4 (0, 1, 0, 0).
			*/
			static const Vector4 UnitY;
			/* Z unit Vector4 (0, 0, 1, 0).
			*/
			static const Vector4 UnitZ;
			/* W unit Vector4 (0, 0, 0, 1).
			*/
			static const Vector4 UnitW;
		};




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