/*
-----------------------------------------------------------------------------
This source file is part of Apoc3D Engine

Copyright (c) 2009+ Tao Xin

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
			Matrix temp;
			Matrix matrix;
			Matrix::Multiply(temp, world, view);
			Matrix::Multiply(matrix, temp, projection);
			matrix.Inverse();

			float x = 2 * (source.X - X) / static_cast<float>(Width) - 1.0f;
			float y = -(2 * (source.Y - Y) / static_cast<float>(Height) - 1.0f);
			float z = (source.Z - MinZ) / (MaxZ - MinZ);

			Vector3 ts(x,y,z);

			Vector3 vector = Vector3::TransformSimple(ts, matrix);
			float a = source.X * matrix.M14 + source.Y * matrix.M24 + source.Z * matrix.M34 + matrix.M44;

			if (!WithinEpsilon(a, 1.0f))
			{
				vector /= a;
			}
			return vector;
		}

		Vector3 Viewport::Project(const Vector3& source, const Matrix& projection, const Matrix& view, const Matrix& world)
		{
			Matrix temp;
			Matrix matrix;
			Matrix::Multiply(temp, world, view);
			Matrix::Multiply(matrix, temp, projection);

			Vector3 vector = Vector3::TransformSimple(source, matrix);

			float a = source.X * matrix.M14 + source.Y * matrix.M24 + source.Z * matrix.M34 + matrix.M44;
			if (!WithinEpsilon(a, 1.0f))
			{
				vector /= a;
			}
			vector.X = (vector.X + 1.0f) * 0.5f * Width + X;
			vector.Y = (-vector.Y + 1.0f) * 0.5f * Height + Y;
			vector.Z = vector.Z * (MaxZ - MinZ) + MinZ;
			return vector;
		}

	}
}