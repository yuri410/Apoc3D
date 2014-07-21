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

#ifndef GRAPHNODE_H
#define GRAPHNODE_H

#include "APDesigner/APDCommon.h"

namespace APDesigner
{
	/** Represents a node in the graph, used to represent a
	 *  atom in the shader network
	 */
	class GraphNode
	{
	public:
		GraphNode(ShaderGraph* graph, QuadTreeNode* tree, const String& name, int index);

		/** Do the physics work for the graph visualization algorithm
		*/
		void UpdatePhysicsSimulation(const GameTime* const time);

		/** Adds a node to this one's neighbor
		 */
		void Link(GraphNode* n)
		{
			assert(n != this);
			if (!m_neighbour.Contains(n))
				m_neighbour.Add(n);
		}
		/** Removes a node from this one's neighbor list
		*/
		void Unlink(GraphNode* n)
		{
			assert(n!=this);
			m_neighbour.Remove(n);
		}

		/** Establish a link to a neighbor node, directional.
		 * 
		 * @remark
		 *  The neighbors here are used to store the actual directed edge information
		 */
		void LinkTopological(GraphNode* n)
		{
			assert(n!=this);
			m_targets.Add(n);
		}
		void UnlonkTopological(GraphNode* n)
		{
			assert(n!=this);
			m_targets.Remove(n);
		}

		bool IsDirectedTo(GraphNode* n)
		{
			return m_targets.Contains(n);
		}

		void ApplyImpulse(const Vector2& imp)
		{
			// apply directly on the velocity as the mass is 1
			m_velocity = Vector2::Add(m_velocity, imp);
		}

		const String& getName() const { return m_name; }
		int getIndex() const { return m_index; }
		void setIndex(int idx) { m_index = idx; }

		bool getIsPositionDirty() const { return m_isPositionDirty; }
		void setIsPositionDirty(bool v) { m_isPositionDirty = v; }

		void setDockingNode(QuadTreeNode* n) { m_dockingNode = n; }
		QuadTreeNode* getDockingNode() const { return m_dockingNode; }

		const Vector2& getPosition() const { return m_position; }
		void setPosition(const Vector2& pos) { m_position = pos; m_isPositionDirty = true; }
		const List<GraphNode*>& getNeighbors() const { return m_neighbour; }
		const List<GraphNode*>& getTargets() const { return m_targets; }
		float getMass() const;
		float getKineticEnergy() const { return m_kineticEnergy; }

		bool IsUseable() const { return m_useable; }
		void setIsUseable(bool v) { m_useable = v; }
	private:


		Vector2 m_position;
		Vector2 m_velocity;
		float m_kineticEnergy;

		bool m_isPositionDirty;

		String m_name;
		int m_index;

		List<GraphNode*> m_neighbour;
		List<GraphNode*> m_targets;

		bool m_useable;

		ShaderGraph* m_graph;
		QuadTreeNode* m_tree;
		/** The leaf node the GraphNode is attached
		*/
		QuadTreeNode* m_dockingNode;

		/** Calculate the repulsive force by walking the quad tree
		*/
		void RecursiveRepulsiveForce(QuadTreeNode* node, Vector2& force);
	};
}

#endif