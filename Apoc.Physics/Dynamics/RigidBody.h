#include "btBulletDynamicsCommon.h"
#include "btBulletCollisionCommon.h"
#include "Collision\CollisionShapeBase.h"

#pragma once


#pragma managed

using namespace System;

namespace V3
{
	namespace PhysicsEngine
	{
		namespace Dynamics
		{
			public ref class RigidBody
			{
			internal:
				btRigidBody* m_rigidBody;
				btDefaultMotionState* m_montionState;

			public:

				RigidBody(float mass, CollisionShapeBase* shape)
				{
					m_montionState = new btDefaultMotionState();
					m_rigidBody = new btRigidBody(mass, m_montionState, shape->m_shape);
				}
			};
		}
	}
}