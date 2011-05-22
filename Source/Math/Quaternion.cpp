#include "Quaternion.h"

#include "Matrix.h"

namespace Apoc3D
{
	namespace Math
	{
		Quaternion Quaternion::Identity = Quaternion(0,0,0,1);

		void Quaternion::CreateRotationMatrix(Quaternion& result, const Matrix& matrix)
		{
			float scale = matrix.M11 + matrix.M22 + matrix.M33;
			float sqrt;
			float half;

			if (scale > 0.0f)
			{
				sqrt = sqrtf(scale + 1);

				result.W = sqrt * 0.5f;
				sqrt = 0.5f / sqrt;

				result.X = (matrix.M23 - matrix.M32) * sqrt;
				result.Y = (matrix.M31 - matrix.M13) * sqrt;
				result.Z = (matrix.M12 - matrix.M21) * sqrt;
				return;
			}

			if ((matrix.M11 >= matrix.M22) && (matrix.M11 >= matrix.M33))
			{
				sqrt = sqrtf(1 + matrix.M11 - matrix.M22 - matrix.M33);
				half = 0.5f / sqrt;

				result.X = 0.5f * sqrt;
				result.Y = (matrix.M12 + matrix.M21) * half;
				result.Z = (matrix.M13 + matrix.M31) * half;
				result.W = (matrix.M23 - matrix.M32) * half;
				return;
			}

			if (matrix.M22 > matrix.M33)
			{
				sqrt = sqrtf(1 + matrix.M22 - matrix.M11 - matrix.M33);
				half = 0.5f / sqrt;

				result.X = (matrix.M21 + matrix.M12) * half;
				result.Y = 0.5f * sqrt;
				result.Z = (matrix.M32 + matrix.M23) * half;
				result.W = (matrix.M31 - matrix.M13) * half;
				return;
			}

			sqrt = sqrtf(1 + matrix.M33 - matrix.M11 - matrix.M22);
			half = 0.5f / sqrt;

			result.X = (matrix.M31 + matrix.M13) * half;
			result.Y = (matrix.M32 + matrix.M23) * half;
			result.Z = 0.5f * sqrt;
			result.W = (matrix.M12 - matrix.M21) * half;
		}
	}
}