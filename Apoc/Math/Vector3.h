#ifndef VECTOR3_H
#define VECTOR3_H

#pragma once

#include "Common.h"

class Vector3
{
public:
	Real x, y, z;

	Vector3() 
	{
	}

	Vector3( const Real X, const Real Y, const Real Z )
            : x( X ), y( Y ), z( Z )
    {
    }

	Real operator [] ( const size_t i ) const
    {
        assert( i < 3 );

        return *(&x+i);
    }

	Real& operator [] ( const size_t i )
    {
        assert( i < 3 );

        return *(&x+i);
    }
	bool operator == ( const Vector3& other ) const
	{
		return ( x == other.x && y == other.y && z == other.z );
	}

	bool operator != ( const Vector3& other ) const
	{
		return ( x != other.x || y != other.y || z != other.z );
	}


    // arithmetic operations
    Vector3 operator + ( const Vector3& vector ) const
    {
        return Vector3(
            x + vector.x,
            y + vector.y,
            z + vector.z);
    }

    Vector3 operator - ( const Vector3& vector ) const
    {
        return Vector3(
            x - vector.x,
            y - vector.y,
            z - vector.z);
    }

    Vector3 operator * ( const Real scalar ) const
    {
        return Vector3(
            x * scalar,
            y * scalar,
            z * scalar);
    }

    Vector3 operator * ( const Vector3& vector) const
    {
        return Vector3(
            x * vector.x,
            y * vector.y,
            z * vector.z);
    }

    Vector3 operator / ( const Real scalar ) const
    {
        Real inv = 1.0 / scalar;

		
        return Vector3(
            x * inv,
            y * inv,
            z * inv);
    }

    Vector3 operator / ( const Vector3& vector) const
    {
        return Vector3(
            x / vector.x,
            y / vector.y,
            z / vector.z);
    }

    const Vector3& operator + () const
    {
        return *this;
    }

    Vector3 operator - () const
    {
        return Vector3(-x, -y, -z);
    }

    // overloaded operators to help Vector3
    friend Vector3 operator * ( const Real scalar, const Vector3& vector )
    {
        return Vector3(
            scalar * vector.x,
            scalar * vector.y,
            scalar * vector.z);
    }

    friend Vector3 operator / ( const Real scalar, const Vector3& vector )
    {
        return Vector3(
            scalar / vector.x,
            scalar / vector.y,
            scalar / vector.z);
    }

    friend Vector3 operator + (const Vector3& vector, const Real scalar)
    {
        return Vector3(
            vector.x + scalar,
            vector.y + scalar,
            vector.z + scalar);
    }

    friend Vector3 operator + (const Real scalar, const Vector3& vector)
    {
        return Vector3(
            scalar + vector.x,
            scalar + vector.y,
            scalar + vector.z);
    }

    friend Vector3 operator - (const Vector3& vector, const Real scalar)
    {
        return Vector3(
            vector.x - scalar,
            vector.y - scalar,
            vector.z - scalar);
    }

    friend Vector3 operator - (const Real scalar, const Vector3& vector)
    {
        return Vector3(
            scalar - vector.x,
            scalar - vector.y,
            scalar - vector.z);
    }

    // arithmetic updates
    Vector3& operator += ( const Vector3& vector )
    {
        x += vector.x;
        y += vector.y;
        z += vector.z;

        return *this;
    }

    Vector3& operator += ( const Real scalar )
    {
        x += scalar;
        y += scalar;
        z += scalar;
        return *this;
    }

    Vector3& operator -= ( const Vector3& vector )
    {
        x -= vector.x;
        y -= vector.y;
        z -= vector.z;

        return *this;
    }

    Vector3& operator -= ( const Real scalar )
    {
        x -= scalar;
        y -= scalar;
        z -= scalar;
        return *this;
    }

    Vector3& operator *= ( const Real scalar )
    {
        x *= scalar;
        y *= scalar;
        z *= scalar;
        return *this;
    }

    Vector3& operator *= ( const Vector3& vector )
    {
        x *= vector.x;
        y *= vector.y;
        z *= vector.z;

        return *this;
    }

    Vector3& operator /= ( const Real scalar )
    {
        assert( scalar != 0.0 );

        Real fInv = 1.0 / scalar;

        x *= fInv;
        y *= fInv;
        z *= fInv;

        return *this;
    }

    Vector3& operator /= ( const Vector3& vector )
    {
        x /= vector.x;
        y /= vector.y;
        z /= vector.z;

        return *this;
    }
	~Vector3(void)
	{
	}
};
#endif