#ifndef BOUNDINGBOX_H
#define BOUNDINGBOX_H

#include "Common.h"
#include "Vector.h"


namespace Apoc3D
{
	namespace Math
	{
		class APAPI BoundingBox
		{
		public:
			/* The highest corner of the box.
			*/
			Vector3 Maximum;

			/* The lowest corner of the box.
			*/
			Vector3 Minimum;

			BoundingBox(Vector3 minimum, Vector3 maximum)
			{
				Minimum = minimum;
				Maximum = maximum;
			}

			Vector3 GetCorner(int index)
			{
				switch (index)
				{
				case 0:
					return VecLoad(GetX(Minimum), GetY(Maximum), GetZ(Maximum));
				case 1:
					return VecLoad(GetX(Maximum), GetY(Maximum), GetZ(Maximum));
				case 2:
					return VecLoad(GetX(Maximum), GetY(Minimum), GetZ(Maximum));
				case 3:
					return VecLoad(GetX(Minimum), GetY(Minimum), GetZ(Maximum));
				case 4:
					return VecLoad(GetX(Minimum), GetY(Maximum), GetZ(Minimum));
				case 5:
					return VecLoad(GetX(Maximum), GetY(Maximum), GetZ(Minimum));
				case 6:
					return VecLoad(GetX(Maximum), GetY(Minimum), GetZ(Minimum));
				case 7:
					return VecLoad(GetX(Minimum), GetY(Minimum), GetZ(Minimum));
				}
				return ZeroVec;
			}
		};
	}
}

#endif