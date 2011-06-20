
#ifndef MOUSEINTERFACE_H
#define MOUSEINTERFACE_H

#include "Common.h"
#include "Math/Point.h"

using namespace Apoc3D::Math;

namespace Apoc3D
{
	namespace UI
	{
		class APAPI MouseInterface
		{
		public:
			static Point GetCurrentPosition();
			static bool IsLeftPressed();
			static bool IsLeftUp();
			static bool IsRightPressed();
			static bool IsRightUp();
		};
	}
}
#endif