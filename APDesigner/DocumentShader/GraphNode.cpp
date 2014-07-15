/*
-----------------------------------------------------------------------------
This source file is part of Apoc3D Engine

Copyright (c) 2009+ Tao Xin

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

namespace APDesigner
{
	const float Damping = 2.2f;
	const float K = 4.0f;
	const float NodeMass = 1.0f;
	const float OringialSpringLength = 1.5f;
	const float Eplison = 0.00001f;
	const float MaxVel = 50;
	const float RepelRatio = 1.5f;

	GraphNode::GraphNode(ShaderGraph* graph, QuadTreeNode* tree, const String& name, int index)
		: m_name(name), m_tree(tree), m_graph(graph), m_dockingNode(0),
		m_position(Vector2::Zero), m_velocity(Vector2::Zero),m_kineticEnergy(0),
		m_index(index), m_useable(true)
	{
		m_position = Vector2(Randomizer::NextFloat() * 2-1, Randomizer::NextFloat() * 2-1);
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

			Vector2 d = Vector2::Subtract(nb->m_position, m_position);
			float distance = Vector2::Length(d);
			float force = (distance-OringialSpringLength) * K;

			float scale = (force * dt);
			
			Vector2 dv = Vector2::Multiply(d, scale);
			m_velocity = Vector2::Add(m_velocity, dv);
		}

		Vector2 force = Vector2::Zero;
		
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
				Vector2 d = Vector2::Subtract(nde->getPosition(), getPosition());
				float dist = Vector2::Length(d);

				if (dist<0.01f || dist>MaxDist)
					continue;

				d = Vector2::Multiply(d, RepelRatio * (getMass()*nde->getMass())/(dist*dist*dist));
				force = Vector2::Subtract(force, d);
			}
			Vector2 dv = Vector2::Multiply(force, dt);
			m_velocity = Vector2::Add(m_velocity, dv);
		}
		else if (m_graph->getTechnique() == ShaderGraph::TECH_Fuzzy)
		{
			// Here, the neighbor forces, as a part of the TECH_Fuzzy method,
			// are calculated. The center of mass's force is done by the Graph class.
			for (int i=0;i<m_neighbour.getCount();i++)
			{
				const GraphNode* nde = m_neighbour[i];
				Vector2 d = Vector2::Subtract(nde->getPosition(), getPosition());
				float dist = Vector2::LengthSquared(d);

				if (dist<0.01f) continue;

				d = Vector2::Multiply(d, RepelRatio * (getMass()*nde->getMass())/(dist));
				force = Vector2::Subtract(force, d);
			}
		}

		Vector2 dv = Vector2::Multiply(force, dt);
		m_velocity = Vector2::Add(m_velocity, dv);


		// the ordinary way of Euler numerical integration
		m_velocity = Vector2::Multiply(m_velocity, 1-Damping*dt);

		float vl=Vector2::LengthSquared(m_velocity);
		m_kineticEnergy = vl * NodeMass;


		//if (vl<Eplison)
		//	m_velocity = Vector2Utils::Zero;
		//else
		{
			// limits the velocity.
			// This can help stabilize the simulation
			if (vl>MaxVel*MaxVel)
			{
				m_velocity = Vector2::Multiply(m_velocity, MaxVel / sqrtf(vl));
			}
			Vector2 dp = Vector2::Multiply(m_velocity, dt);
			m_position = Vector2::Add(m_position, dp);

			m_isPositionDirty = true;
		}
	}

	void GraphNode::RecursiveRepulsiveForce(QuadTreeNode* node, Vector2& force)
	{
		static const float Threshold = 1.0f;

		Vector2 d = Vector2::Subtract(node->getCenterOfMass(), getPosition());

		float dist = Vector2::Length(d);
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

					const LinkedList<GraphNode*>& nodes = node->getAttachedNodes();
					for (LinkedList<GraphNode*>::Iterator iter = nodes.Begin();
						iter!=nodes.End();iter++)
					{
						const GraphNode* nde = *iter;
						d = Vector2::Subtract(nde->getPosition(), getPosition());
						dist = Vector2::Length(d);

						if (dist<0.0001f)// || dist>MaxDist)
							continue;

						d = Vector2::Multiply(d, RepelRatio * (getMass()*nde->getMass())/(dist*dist*dist));
						force = Vector2::Subtract(force, d);
					}
					break;
				}
			}
		}
		else// if (dist<=MaxDist)
		{
			// otherwise, use the big QuadTreeNode as a body
			d = Vector2::Multiply(d, RepelRatio * (getMass()*node->getMass())/(dist*dist*dist));
			force =  Vector2::Subtract(force, d);
		}
	}

	float GraphNode::getMass() const { return NodeMass; }
}