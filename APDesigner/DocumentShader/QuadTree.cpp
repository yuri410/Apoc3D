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

#include "QuadTree.h"

#include "GraphNode.h"

namespace APDesigner
{
	QuadTreeNode::QuadTreeNode(QuadTreeNode* parent, const Apoc3D::Math::RectangleF& rect, int maxDepth, LeafNodeCreatedHandler* handler)
		: m_isDirty(false), m_parent(parent), m_totalAttachedInSubtree(0), m_area(rect), m_equviliantMass(0)
	{
		// not deep enough?
		if (maxDepth>1)
		{
			
			Apoc3D::Math::RectangleF subRegion(rect.X, rect.Y, rect.Width/2,rect.Height/2);

			m_subNodes[QTN_TopLeft] = new QuadTreeNode(this, subRegion, maxDepth-1, handler);

			subRegion.X += rect.Width/2;
			m_subNodes[QTN_TopRight] = new QuadTreeNode(this, subRegion, maxDepth-1, handler);

			subRegion.Y += rect.Height/2;
			m_subNodes[QTN_BottomRight] = new QuadTreeNode(this, subRegion, maxDepth-1, handler);

			subRegion.X -= rect.Width/2;
			m_subNodes[QTN_BottomLeft] = new QuadTreeNode(this, subRegion, maxDepth-1, handler);

			m_isLeafNode = false;
		}
		else
		{
			memset(m_subNodes, 0, sizeof(m_subNodes));

			m_isLeafNode = true;

			if (handler)
			{
				handler->LeafNodeCreated(this);
			}
		}

		m_centerOfMass = Vector2(rect.X+rect.Width/2, rect.Y+rect.Height/2);
	}

	QuadTreeNode::~QuadTreeNode()
	{
		for (int i=0;i<4;i++)
		{
			if (m_subNodes[i])
			{
				delete m_subNodes[i];
			}
		}
	}



	void QuadTreeNode::Clear()
	{
		m_attachedGraphNodes.Clear();

		for (int i=0;i<4;i++)
		{
			if (m_subNodes[i])
			{
				m_subNodes[i]->Clear();
			}
		}
	}

	void QuadTreeNode::Update(const GameTime* time)
	{
		if (m_isDirty)
		{
			m_totalAttachedInSubtree = 0;
			m_equviliantMass = 0;
			m_centerOfMass = Vector2::Zero;

			for (int i=0;i<4;i++)
			{
				if (m_subNodes[i])
				{
					m_subNodes[i]->Update(time);
				}
			}

			// these calculation happens on leaf nodes
			float totalMass = 0;
			// calculate the center of mass using expectation value.
			// This can be proved mathematically to also work on hierarchy
			// structures
			Vector2 expectedPos = Vector2::Zero;

			// Go through all GraphNode
			for (GraphNode* nde : m_attachedGraphNodes)
			{
				const float m = nde->getMass();
				totalMass += nde->getMass();

				expectedPos = Vector2::Add(expectedPos, Vector2::Multiply(nde->getPosition(), m));
				m_totalAttachedInSubtree++;
			}

			// these on internal nodes.
			// Go through all virtual-Nodes: the internal tree node,
			// which is treated a sub tree of bodies as an entire body
			for (int i=0;i<4;i++)
			{
				if (m_subNodes[i])
				{
					m_totalAttachedInSubtree += m_subNodes[i]->m_totalAttachedInSubtree;
					totalMass += m_subNodes[i]->m_equviliantMass;

					expectedPos = Vector2::Add(expectedPos, Vector2::Multiply(m_subNodes[i]->m_centerOfMass, m_subNodes[i]->getMass()));
				}
			}

			// div by 0 check
			if (totalMass>0.1f)
			{
				expectedPos = Vector2::Divide(expectedPos, totalMass);

				m_centerOfMass = expectedPos;
			}
			m_equviliantMass = totalMass;
			m_isDirty = false;
		}

	}

	void QuadTreeNode::MakeAllDirty()
	{
		MakeAllDirty(this);
	}
	void QuadTreeNode::MakeAllDirty(QuadTreeNode* node)
	{
		node->m_isDirty = true;
		for (int i=0;i<4;i++)
		{
			if (m_subNodes[i])
			{
				m_subNodes[i]->MakeAllDirty();
			}
		}
	}

	GraphNode* QuadTreeNode::IntersectNodes(const Vector2& pt)
	{
		for (GraphNode* nde: m_attachedGraphNodes)
		{
			Vector2 d = Vector2::Subtract(pt, nde->getPosition());
			float x = d.X;
			float y = d.Y;

			// use the ellipse's equation to check the intersection

			if (((x*x)/(0.5f*0.5f) + (y*y)/(0.2f*0.2f) - 1) <= 0)
				return nde;
		}

		// used to add extent to m_area when checking
		Apoc3D::Math::RectangleF pointRegion(
			pt.X-m_area.Width*0.5f, 
			pt.Y-m_area.Height*0.5f, 
			m_area.Width, m_area.Height);

		for (int i=0;i<4;i++)
		{
			if (m_subNodes[i] && m_area.Intersects(pointRegion))
			{
				GraphNode* result = m_subNodes[i]->IntersectNodes(pt);
				if (result)
					return result;
			}
		}
		return 0;
	}

	void QuadTreeNode::FillIntersectingNodesAttachment(List<GraphNode*>& list, const Apoc3D::Math::RectangleF& area)
	{
		for (GraphNode* nde : m_attachedGraphNodes)
		{
			list.Add(nde);
		}

		for (int i=0;i<4;i++)
		{
			if (m_subNodes[i] && m_area.Intersects(area))
			{
				m_subNodes[i]->FillIntersectingNodesAttachment(list, area);
			}
		}
	}
}