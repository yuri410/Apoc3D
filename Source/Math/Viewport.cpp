/*
-----------------------------------------------------------------------------
This source file is part of Apoc3D Engine

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
#include "Viewport.h"

#include "Matrix.h"

namespace Apoc3D
{
	namespace Math
	{
		static bool WithinEpsilon(float a, float b)
		{
			float diff = a - b;
			return ((-EPSILON <= diff) && (diff <= EPSILON));
		}

		Vector3 Viewport::Unproject(const Vector3& source, const Matrix& projection, const Matrix& view, const Matrix& world)
		{
			Matrix matrix;
			Matrix::Multiply(matrix, world, view);
			Matrix::Multiply(matrix, matrix, projection);
			matrix.Inverse();

			float x = 2 * (_V3X(source) - X) / static_cast<float>(Width) - 1.0f;
			float y = -(2 * (_V3Y(source) - Y) / static_cast<float>(Height) - 1.0f);
			float z = (_V3Z(source) - MinZ) / (MaxZ - MinZ);

			Vector3 ts = Vector3Utils::LDVector(x,y,z);

			Vector3 vector = Vector3Utils::TransformSimple(ts, matrix);
			float a = _V3X(source) * matrix.M14 + _V3Y(source) * matrix.M24 + _V3Z(source) * matrix.M34 + matrix.M44;

			if (!WithinEpsilon(a, 1.0f))
			{
				vector = Vector3Utils::Divide(vector, a);
			}
			return vector;
		}

		Vector3 Viewport::Project(const Vector3& source, const Matrix& projection, const Matrix& view, const Matrix& world)
		{
			Matrix matrix;
			Matrix::Multiply(matrix, world, view);
			Matrix::Multiply(matrix, matrix, projection);

			Vector3 vector = Vector3Utils::TransformSimple(source, matrix);

			float a = _V3X(source) * matrix.M14 + _V3Y(source) * matrix.M24 + _V3Z(source) * matrix.M34 + matrix.M44;
			if (!WithinEpsilon(a, 1.0f))
			{
				vector = Vector3Utils::Divide(vector, a);
			}
			_V3X(vector) = (_V3X(vector) + 1.0f) * 0.5f * Width + X;
			_V3X(vector) = (-_V3Y(vector) + 1.0f) * 0.5f * Height + Y;
			_V3X(vector) = _V3Z(vector) * (MaxZ - MinZ) + MinZ;
			return vector;
		}

	}
}