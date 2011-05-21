#include "Vector.h"

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
		class FieldInitializer
		{
		public:
			FieldInitializer()
			{
				uint Data2[4] = {0x80000000, 0x80000000, 0x80000000, 0x80000000};
				memcpy((void*)&_MASKSIGN_, Data2, sizeof(Data2));

			}

		} Initializer;

#endif

	}
}