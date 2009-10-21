#pragma once

#include "CollisionShapeBase.h"

namespace V3
{
	namespace PhysicsEngine
	{
		namespace Collision
		{
			public ref class BoxShape : CollisionShapeBase
			{
			private:
				btBoxShape* m_boxShape;

			public:

				BoxShape(float length)
				{
					
				}
			};
		}
	}
}