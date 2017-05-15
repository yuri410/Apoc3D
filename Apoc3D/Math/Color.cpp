#include "Color.h"
#include "Math.h"

namespace Apoc3D
{
	namespace Math
	{
		const Color4 Color4::Zero = Color4(0.0f, 0.0f, 0.0f, 0.0f);
		const Color4 Color4::One = Color4(1.0f, 1.0f, 1.0f, 1.0f);

		void Color4::ClampInPlace()
		{
			Red = Math::Saturate(Red);
			Green = Math::Saturate(Green);
			Blue = Math::Saturate(Blue);
			Alpha = Math::Saturate(Alpha);
		}
	}
}