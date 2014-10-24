#include "Color.h"
#include "Math.h"

namespace Apoc3D
{
	namespace Math
	{
		void Color4::ClampInPlace()
		{
			Red = Math::Saturate(Red);
			Green = Math::Saturate(Green);
			Blue = Math::Saturate(Blue);
			Alpha = Math::Saturate(Alpha);
		}
	}
}