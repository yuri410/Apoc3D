/* -----------------------------------------------------------------------
 * This source file is part of Apoc3D Framework
 * 
 * Copyright (c) 2009-2018 Tao Xin
 * 
 * This content of this file is subject to the terms of the Mozilla Public 
 * License v2.0. If a copy of the MPL was not distributed with this file, 
 * you can obtain one at http://mozilla.org/MPL/2.0/.
 * 
 * This program is distributed in the hope that it will be useful, 
 * WITHOUT WARRANTY OF ANY KIND; either express or implied. See the 
 * Mozilla Public License for more details.
 * 
 * ------------------------------------------------------------------------
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
		void UpdatePhysicsSimulation(const AppTime* time);

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