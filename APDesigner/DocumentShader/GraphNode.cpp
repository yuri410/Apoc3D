/*
-----------------------------------------------------------------------------
This source file is part of Apoc3D Engine

Copyright (c) 2009+ Tao Games

This program is free software: you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation, either version 3 of the License, or
(at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program.  if not, write to the Free Software Foundation, 
Inc., 59 Temple Place - Suite 330, Boston, MA 02111-1307, USA, or go to
http://www.gnu.org/copyleft/gpl.txt.

-----------------------------------------------------------------------------
*/

#include "GraphNode.h"

#include "ShaderGraph.h"
#include "QuadTree.h"

#include "apoc3d/Core/GameTime.h"
#include "apoc3d/Math/RandomUtils.h"
#include "apoc3d/Math/MathCommon.h"


namespace APDesigner
{
	static const float Damping = 2.2f;
	static const float K = 4.0f;
	static const float NodeMass = 1.0f;
	static const float OringialSpringLength = 1.5f;
	static const float Eplison = 0.00001f;
	static const float MaxVel = 50;
	static const float RepelRatio = 1.5f;

	GraphNode::GraphNode(ShaderGraph* graph, QuadTreeNode* tree, const String& name, int index)
		: m_name(name), m_tree(tree), m_graph(graph), m_dockingNode(0),
		m_position(Vector2Utils::Zero), m_velocity(Vector2Utils::Zero),m_kineticEnergy(0),
		m_index(index), m_useable(true)
	{
		m_position = Vector2Utils::LDVector(Randomizer::NextFloat() * 2-1, Randomizer::NextFloat() * 2-1);
		//// node is initially randomly placed on a identity circle
		//float r = Randomizer::NextFloat() * 1024;
		//float angle = Randomizer::NextFloat() * Apoc3D::Math::PI * 2;
		//m_position = Vector2Utils::LDVector(r*cosf(angle), r*sinf(angle));
		//m_velocity = Vector2Utils::LDVector(-Vector2Utils::GetY(m_position), Vector2Utils::GetX(m_position));
		//m_velocity = Vector2Utils::Normalize(m_velocity);
		//m_velocity = Vector2Utils::Multiply(m_velocity, 5);

	}

	void GraphNode::UpdatePhysicsSimulation(const GameTime* const time)
	{
		float dt = time->getElapsedTime();

		// spring force from adjacent nodes
		for (int i=0;i<m_neighbour.getCount();i++)
		{
			const GraphNode* nb = m_neighbour[i];

			Vector2 d = Vector2Utils::Subtract(nb->m_position, m_position);
			float distance = Vector2Utils::Length(d);
			float force = (distance-OringialSpringLength) * K;

			float scale = (force * dt);
			
			Vector2 dv = Vector2Utils::Multiply(d, scale);
			m_velocity = Vector2Utils::Add(m_velocity, dv);
		}

		Vector2 force = Vector2Utils::Zero;
		
		if (m_graph->getTechnique() == ShaderGraph::TECH_Quad)
		{
			// gravitational force by recursively walking in the quad tree
			RecursiveRepulsiveForce(m_tree, force);
		}
		else if (m_graph->getTechnique() == ShaderGraph::TECH_BruteForce)
		{
			// the brute force method just calculate repulsive force from other nodes one by one
			// the time is O(n^2) for each iteration
			static const float MaxDist = 512;
			
			const FastList<GraphNode*>& nodes = m_graph->getNodes();

			for (int i=0;i<nodes.getCount();i++)
			{
				const GraphNode* nde = nodes[i];
				Vector2 d = Vector2Utils::Subtract(nde->getPosition(), getPosition());
				float dist = Vector2Utils::Length(d);

				if (dist<0.01f || dist>MaxDist)
					continue;

				d = Vector2Utils::Multiply(d, RepelRatio * (getMass()*nde->getMass())/(dist*dist*dist));
				force = Vector2Utils::Subtract(force, d);
			}
			Vector2 dv = Vector2Utils::Multiply(force, dt);
			m_velocity = Vector2Utils::Add(m_velocity, dv);
		}
		else if (m_graph->getTechnique() == ShaderGraph::TECH_Fuzzy)
		{
			// Here, the neighbor forces, as a part of the TECH_Fuzzy method,
			// are calculated. The center of mass's force is done by the Graph class.
			for (int i=0;i<m_neighbour.getCount();i++)
			{
				const GraphNode* nde = m_neighbour[i];
				Vector2 d = Vector2Utils::Subtract(nde->getPosition(), getPosition());
				float dist = Vector2Utils::LengthSquared(d);

				if (dist<0.01f) continue;

				d = Vector2Utils::Multiply(d, RepelRatio * (getMass()*nde->getMass())/(dist));
				force = Vector2Utils::Subtract(force, d);
			}
		}

		Vector2 dv = Vector2Utils::Multiply(force, dt);
		m_velocity = Vector2Utils::Add(m_velocity, dv);


		// the ordinary way of Euler numerical integration
		m_velocity = Vector2Utils::Multiply(m_velocity, 1-Damping*dt);

		float vl=Vector2Utils::LengthSquared(m_velocity);
		m_kineticEnergy = vl * NodeMass;


		//if (vl<Eplison)
		//	m_velocity = Vector2Utils::Zero;
		//else
		{
			// limits the velocity.
			// This can help stabilize the simulation
			if (vl>MaxVel*MaxVel)
			{
				m_velocity = Vector2Utils::Multiply(m_velocity, MaxVel / sqrtf(vl));
			}
			Vector2 dp = Vector2Utils::Multiply(m_velocity, dt);
			m_position = Vector2Utils::Add(m_position, dp);

			m_isPositionDirty = true;
		}
	}

	void GraphNode::RecursiveRepulsiveForce(QuadTreeNode* node, Vector2& force)
	{
		static const float Threshold = 1.0f;

		Vector2 d = Vector2Utils::Subtract(node->getCenterOfMass(), getPosition());

		float dist = Vector2Utils::Length(d);
		if (dist<0.001f)
			return;


		if ((node->getArea().Width/dist) >= Threshold)
		{
			// the node is too big, break into small parts to increase precision.
			for (int i=0;i<4;i++)
			{
				QuadTreeNode* subNode = node->getNode((QuadTreeNode::NodeCorner)i);
				if (subNode && subNode->getMass()<0.1f)
					continue;

				if (subNode)
				{
					RecursiveRepulsiveForce(subNode, force);
				}
				else
				{
					// no sub node.
					// That mean this node is a leaf node

					const std::list<GraphNode*>& nodes = node->getAttachedNodes();
					for (std::list<GraphNode*>::const_iterator iter = nodes.cbegin();
						iter!=nodes.cend();iter++)
					{
						const GraphNode* nde = *iter;
						d = Vector2Utils::Subtract(nde->getPosition(), getPosition());
						dist = Vector2Utils::Length(d);

						if (dist<0.0001f)// || dist>MaxDist)
							continue;

						d = Vector2Utils::Multiply(d, RepelRatio * (getMass()*nde->getMass())/(dist*dist*dist));
						force = Vector2Utils::Subtract(force, d);
					}
					break;
				}
			}
		}
		else// if (dist<=MaxDist)
		{
			// otherwise, use the big QuadTreeNode as a body
			d = Vector2Utils::Multiply(d, RepelRatio * (getMass()*node->getMass())/(dist*dist*dist));
			force =  Vector2Utils::Subtract(force, d);
		}
	}

	float GraphNode::getMass() const { return NodeMass; }
}