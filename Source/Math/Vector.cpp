#include "Vector.h"

#include "Matrix.h"

namespace Apoc3D
{
	namespace Math
	{
		const Vector2 Vector2Utils::Zero = Vector2Utils::LDVector(0);
		const Vector2 Vector2Utils::UnitX = Vector2Utils::LDVector(1,0);
		const Vector2 Vector2Utils::UnitY = Vector2Utils::LDVector(0,1);
		const Vector2 Vector2Utils::One = Vector2Utils::LDVector(1);

		const Vector3 Vector3Utils::Zero = Vector3Utils::LDVector(0);
		const Vector3 Vector3Utils::UnitX = Vector3Utils::LDVector(1, 0, 0);
		const Vector3 Vector3Utils::UnitY = Vector3Utils::LDVector(0, 1, 0);
		const Vector3 Vector3Utils::UnitZ = Vector3Utils::LDVector(0, 0, 1);
		const Vector3 Vector3Utils::One = Vector3Utils::LDVector(1);
		
		const Vector4 Vector4Utils::Zero = Vector4Utils::LDVector(0);
		const Vector4 Vector4Utils::UnitX = Vector4Utils::LDVector(1, 0, 0, 0);
		const Vector4 Vector4Utils::UnitY = Vector4Utils::LDVector(0, 1, 0, 0);
		const Vector4 Vector4Utils::UnitZ = Vector4Utils::LDVector(0, 0, 1, 0);
		const Vector4 Vector4Utils::UnitW = Vector4Utils::LDVector(0, 0, 0, 1);
		const Vector4 Vector4Utils::One = Vector4Utils::LDVector(1);


#if APOC3D_MATH_IMPL == APOC3D_SSE
		Vector4 Vector3Utils::Transform(Vector3 vector, const Matrix& transform)
		{

		}
#elif APOC3D_MATH_IMPL == APOC3D_DEFAULT

		Vector4 Vector3Utils::Transform(const Vector3& vector, const Matrix& transform)
		{
			float x = vector.X * transform.M11 + vector.Y * transform.M21 + vector.Z * transform.M31 + transform.M41;
			float y = vector.X * transform.M12 + vector.Y * transform.M22 + vector.Z * transform.M32 + transform.M42;
			float z = vector.X * transform.M13 + vector.Y * transform.M23 + vector.Z * transform.M33 + transform.M43;
			float w = vector.X * transform.M14 + vector.Y * transform.M24 + vector.Z * transform.M34 + transform.M44;
			return Vector4Utils::LDVector(x,y,z,w);
		}

		Vector3 Vector3Utils::TransformSimple(const Vector3& vector, const Matrix& transform)
		{
			float x = vector.X * transform.M11 + vector.Y * transform.M21 + vector.Z * transform.M31 + transform.M41;
			float y = vector.X * transform.M12 + vector.Y * transform.M22 + vector.Z * transform.M32 + transform.M42;
			float z = vector.X * transform.M13 + vector.Y * transform.M23 + vector.Z * transform.M33 + transform.M43;
			return Vector3Utils::LDVector(x,y,z);
		}

		Vector3 Vector3Utils::TransformCoordinate(const Vector3& vector, const Matrix& transform)
		{
			float x = vector.X * transform.M11 + vector.Y * transform.M21 + vector.Z * transform.M31 + transform.M41;
			float y = vector.X * transform.M12 + vector.Y * transform.M22 + vector.Z * transform.M32 + transform.M42;
			float z = vector.X * transform.M13 + vector.Y * transform.M23 + vector.Z * transform.M33 + transform.M43;
			float w = 1 / (vector.X * transform.M14 + vector.Y * transform.M24 + vector.Z * transform.M34 + transform.M44);
			return Vector3Utils::LDVector(x*w,y*w,z*w);
		}

		Vector3 Vector3Utils::TransformNormal(const Vector3& vector, const Matrix& transform)
		{
			float x = vector.X * transform.M11 + vector.Y * transform.M21 + vector.Z * transform.M31;
			float y = vector.X * transform.M12 + vector.Y * transform.M22 + vector.Z * transform.M32;
			float z = vector.X * transform.M13 + vector.Y * transform.M23 + vector.Z * transform.M33;
			return Vector3Utils::LDVector(x,y,z);
		}
#endif
	}
}