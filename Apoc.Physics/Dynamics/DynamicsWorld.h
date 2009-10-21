#include "btBulletDynamicsCommon.h"
#include "btBulletCollisionCommon.h"

#pragma once

#include "RigidBody.h"

#pragma managed

using namespace System;

namespace V3
{
	namespace PhysicsEngine
	{
		namespace Dynamics
		{
			public ref class DynamicsWorld
			{
			private:
				btDiscreteDynamicsWorld* m_world;
				btDispatcher* m_dispatcher;
				btAxisSweep3* m_broadPhase;
				btSequentialImpulseConstraintSolver* m_conSolver;

			public:

				DynamicsWorld(void)
				{			
					m_dispatcher = new btDispatcher();
					m_broadPhase = new btAxisSweep3(
						btVector3(btScalar(-10000), btScalar(-10000), btScalar(-10000)),
						btVector3(btScalar( 10000), btScalar( 10000), btScalar( 10000)), 1024);
					m_conSolver = new btSequentialImpulseConstraintSolver();

					m_world = new btDiscreteDynamicsWorld(m_dispatcher, m_broadPhase, m_conSolver, 0); 
				}

				void AddRigidBody(RigidBody* body)
				{
					m_world->addRigidBody(body->m_rigidBody);					
				}

				void RemoveRigidBody(RigidBody* body)
				{
					m_world->removeRigodBody(body->m_rigidBody);
				}

				void AddConstraint()
				{
					
				}

				void RemoveConstraint()
				{

				}
			};
		}
	}
}