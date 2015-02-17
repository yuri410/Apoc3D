#include "DoubleMath.h"
#include "Ray.h"

namespace Apoc3D
{
	namespace DoubleMath
	{
		const Vector3d Vector3d::Zero = Vector3d::Set(0.0);
		const Vector3d Vector3d::UnitX = Vector3d(1, 0, 0);
		const Vector3d Vector3d::UnitY = Vector3d(0, 1, 0);
		const Vector3d Vector3d::UnitZ = Vector3d(0, 0, 1);
		const Vector3d Vector3d::One = Vector3d::Set(1.0);

		const Vector4d Vector4d::Zero = Vector4d::Set(0.0);
		const Vector4d Vector4d::UnitX = Vector4d(1, 0, 0, 0);
		const Vector4d Vector4d::UnitY = Vector4d(0, 1, 0, 0);
		const Vector4d Vector4d::UnitZ = Vector4d(0, 0, 1, 0);
		const Vector4d Vector4d::UnitW = Vector4d(0, 0, 0, 1);
		const Vector4d Vector4d::One = Vector4d::Set(1.0);

		Vector4d Vector4d::Transform(const Vector4d& vector, const Matrixd& transform)
		{
			double x = vector.X * transform.M11 + vector.Y * transform.M21 + vector.Z * transform.M31 + vector.W * transform.M41;
			double y = vector.X * transform.M12 + vector.Y * transform.M22 + vector.Z * transform.M32 + vector.W * transform.M42;
			double z = vector.X * transform.M13 + vector.Y * transform.M23 + vector.Z * transform.M33 + vector.W * transform.M43;
			double w = vector.X * transform.M14 + vector.Y * transform.M24 + vector.Z * transform.M34 + vector.W * transform.M44;
			return Vector4d(x, y, z, w);
		}


		Vector4d Vector3d::Transform(const Vector3d& vector, const Matrixd& transform)
		{
			double x = vector.X * transform.M11 + vector.Y * transform.M21 + vector.Z * transform.M31 + transform.M41;
			double y = vector.X * transform.M12 + vector.Y * transform.M22 + vector.Z * transform.M32 + transform.M42;
			double z = vector.X * transform.M13 + vector.Y * transform.M23 + vector.Z * transform.M33 + transform.M43;
			double w = vector.X * transform.M14 + vector.Y * transform.M24 + vector.Z * transform.M34 + transform.M44;

			return Vector4d(x, y, z, w);
		}
		Vector3d Vector3d::TransformSimple(const Vector3d& vector, const Matrixd& transform)
		{
			double x = vector.X * transform.M11 + vector.Y * transform.M21 + vector.Z * transform.M31 + transform.M41;
			double y = vector.X * transform.M12 + vector.Y * transform.M22 + vector.Z * transform.M32 + transform.M42;
			double z = vector.X * transform.M13 + vector.Y * transform.M23 + vector.Z * transform.M33 + transform.M43;
			return Vector3d(x, y, z);
		}
		Vector3d Vector3d::TransformCoordinate(const Vector3d& vector, const Matrixd& transform)
		{
			double x = vector.X * transform.M11 + vector.Y * transform.M21 + vector.Z * transform.M31 + transform.M41;
			double y = vector.X * transform.M12 + vector.Y * transform.M22 + vector.Z * transform.M32 + transform.M42;
			double z = vector.X * transform.M13 + vector.Y * transform.M23 + vector.Z * transform.M33 + transform.M43;
			double w = 1 / (vector.X * transform.M14 + vector.Y * transform.M24 + vector.Z * transform.M34 + transform.M44);
			return Vector3d(x*w, y*w, z*w);
		}
		Vector3d Vector3d::TransformNormal(const Vector3d& vector, const Matrixd& transform)
		{
			double x = vector.X * transform.M11 + vector.Y * transform.M21 + vector.Z * transform.M31;
			double y = vector.X * transform.M12 + vector.Y * transform.M22 + vector.Z * transform.M32;
			double z = vector.X * transform.M13 + vector.Y * transform.M23 + vector.Z * transform.M33;
			return Vector3d(x, y, z);
		}


		const Matrixd Matrixd::Identity = 
		{
			1, 0, 0, 0,
			0, 1, 0, 0,
			0, 0, 1, 0, 
			0, 0, 0, 1 
		};

		double Matrixd::Inverse()
		{
			double m11 = M11;
			double m12 = M12;
			double m13 = M13;
			double m14 = M14;
			double m21 = M21;
			double m22 = M22;
			double m23 = M23;
			double m24 = M24;
			double m31 = M31;
			double m32 = M32;
			double m33 = M33;
			double m34 = M34;
			double m41 = M41;
			double m42 = M42;
			double m43 = M43;
			double m44 = M44;

			double det01 = m11 * m22 - m12 * m21;
			double det02 = m11 * m23 - m13 * m21;
			double det03 = m11 * m24 - m14 * m21;
			double det04 = m12 * m23 - m13 * m22;
			double det05 = m12 * m24 - m14 * m22;
			double det06 = m13 * m24 - m14 * m23;
			double det07 = m31 * m42 - m32 * m41;
			double det08 = m31 * m43 - m33 * m41;
			double det09 = m31 * m44 - m34 * m41;
			double det10 = m32 * m43 - m33 * m42;
			double det11 = m32 * m44 - m34 * m42;
			double det12 = m33 * m44 - m34 * m43;

			double detMatrix = det01 * det12 - det02 * det11 + det03 * det10 + det04 * det09 - det05 * det08 + det06 * det07;
			double invDetMatrix = 1.0 / detMatrix;

			M11 = (+m22 * det12 - m23 * det11 + m24 * det10) * invDetMatrix;
			M12 = (-m12 * det12 + m13 * det11 - m14 * det10) * invDetMatrix;
			M13 = (+m42 * det06 - m43 * det05 + m44 * det04) * invDetMatrix;
			M14 = (-m32 * det06 + m33 * det05 - m34 * det04) * invDetMatrix;
			M21 = (-m21 * det12 + m23 * det09 - m24 * det08) * invDetMatrix;
			M22 = (+m11 * det12 - m13 * det09 + m14 * det08) * invDetMatrix;
			M23 = (-m41 * det06 + m43 * det03 - m44 * det02) * invDetMatrix;
			M24 = (+m31 * det06 - m33 * det03 + m34 * det02) * invDetMatrix;
			M31 = (+m21 * det11 - m22 * det09 + m24 * det07) * invDetMatrix;
			M32 = (-m11 * det11 + m12 * det09 - m14 * det07) * invDetMatrix;
			M33 = (+m41 * det05 - m42 * det03 + m44 * det01) * invDetMatrix;
			M34 = (-m31 * det05 + m32 * det03 - m34 * det01) * invDetMatrix;
			M41 = (-m21 * det10 + m22 * det08 - m23 * det07) * invDetMatrix;
			M42 = (+m11 * det10 - m12 * det08 + m13 * det07) * invDetMatrix;
			M43 = (-m41 * det04 + m42 * det02 - m43 * det01) * invDetMatrix;
			M44 = (+m31 * det04 - m32 * det02 + m33 * det01) * invDetMatrix;

			return detMatrix;

		}

		//////////////////////////////////////////////////////////////////////////

		RaySegmentd raysegdtof(const Math::RaySegment& m)
		{
			RaySegmentd r;
			r.Start = vec3ftod(m.Start);
			r.End = vec3ftod(m.End);
			return r;
		}
	}

}