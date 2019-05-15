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

#ifndef QUADTREE_H
#define QUADTREE_H

#include "APDesigner/APDCommon.h"

namespace APDesigner
{
	/** Handler for the creation of a leaf node in quad tree
	*/
	class LeafNodeCreatedHandler
	{
	public:
		virtual void LeafNodeCreated(QuadTreeNode* node) = 0;
	};

	class QuadTreeNode
	{
	public:
		enum NodeCorner
		{
			QTN_TopLeft		=0,
			QTN_TopRight	=1,
			QTN_BottomLeft	=2,
			QTN_BottomRight	=3
		};

		/** Creates a quad tree root, and recursively build the entire structure. 
		 * @param handler Called back once the leaf node are created, providing the leaf node as a parameter
		 * @param maxDepth Specifies the depth of the tree.
		 * @param parent The parent node of the node being created. Use 0 if the node is a root node.
		 * @param rect The 2D region the quad tree covers.
		 */
		QuadTreeNode(QuadTreeNode* parent, const Apoc3D::Math::RectangleF& rect, int maxDepth, LeafNodeCreatedHandler* handler);
		~QuadTreeNode();

		QuadTreeNode* getNode(NodeCorner corner) const { return m_subNodes[(int)corner]; }

		/** Clears the graph nodes attached on the nodes in sub tree
		*/
		void Clear();

		/** [leaf node only]
		 *  Attaches a GraphNode to this leaf quad tree node.
		 *  Attached objects are GraphNodes within the area of 
		 *  this leaf node.
		 */
		void Attach(GraphNode* node)
		{
			assert(m_isLeafNode);
			m_attachedGraphNodes.PushBack(node);
			MarkDirty();
		}
		/** [leaf node only]
		 *  Detaches a GraphNode to this leaf quad tree node.
		 */
		void Detach(GraphNode* node)
		{
			assert(m_isLeafNode);
			
			m_attachedGraphNodes.Remove(node);
			MarkDirty();
		}

		/** Update on properties like the number of graph node the sub tree contains, the center of mass,
		 *  and total mass.
		 */
		void Update(const AppTime* time);


		/** Find the node intersected with the given point
		*/
		GraphNode* IntersectNodes(const Vector2& pt);

		/** Fill all sub nodes' attachment if a node is intersecting the given area.
		*/
		void FillIntersectingNodesAttachment(List<GraphNode*>& list, const Apoc3D::Math::RectangleF& area);

		const RectangleF& getArea() const { return m_area; }
		const LinkedList<GraphNode*>& getAttachedNodes() { return m_attachedGraphNodes; }
		const Vector2& getCenterOfMass() const { return m_centerOfMass; }
		const float getMass() const { return m_equviliantMass; }

		void MakeAllDirty();
	private:
		QuadTreeNode* m_parent;
		QuadTreeNode* m_subNodes[4];

		LinkedList<GraphNode*> m_attachedGraphNodes;
		bool m_isLeafNode;

		RectangleF m_area;

		/** Dirty means that the node's m_totalAttachedInSubtree, m_equviliantMass
		 *  and m_centerOfMass need to be updated and recalculated based on the 
		 *  attached GraphNodes
		 */
		bool m_isDirty;

		float m_equviliantMass;
		Vector2 m_centerOfMass;
		int m_totalAttachedInSubtree;


		void MarkDirty()
		{
			assert(m_isLeafNode);
			PopDirty();
		}

		/** Mark the node as well all the way from its parent node to the root node dirty
		*/
		void PopDirty()
		{
			m_isDirty = true;
			if (m_parent)
				m_parent->PopDirty();
		}
		void MakeAllDirty(QuadTreeNode* node);
	};
}

#endif