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

#include "QuadTree.h"

#include "GraphNode.h"

#include "apoc3d/Core/GameTime.h"
#include "apoc3d/Math/Rectangle.h"

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

		m_centerOfMass = Vector2Utils::LDVector(rect.X+rect.Width/2, rect.Y+rect.Height/2);
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
		m_attachedGraphNodes.clear();

		for (int i=0;i<4;i++)
		{
			if (m_subNodes[i])
			{
				m_subNodes[i]->Clear();
			}
		}
	}

	void QuadTreeNode::Update(const GameTime* const time)
	{
		if (m_isDirty)
		{
			m_totalAttachedInSubtree = 0;
			m_equviliantMass = 0;
			m_centerOfMass = Vector2Utils::Zero;

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
			Vector2 expectedPos = Vector2Utils::Zero;

			// Go through all GraphNode
			for (std::list<GraphNode*>::iterator iter = m_attachedGraphNodes.begin();
				iter != m_attachedGraphNodes.end(); iter++)
			{
				GraphNode* nde = *iter;
				const float m = nde->getMass();
				totalMass += nde->getMass();

				expectedPos = Vector2Utils::Add(expectedPos, Vector2Utils::Multiply(nde->getPosition(), m));
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

					expectedPos = Vector2Utils::Add(expectedPos, Vector2Utils::Multiply(m_subNodes[i]->m_centerOfMass, m_subNodes[i]->getMass()));
				}
			}

			// div by 0 check
			if (totalMass>0.1f)
			{
				expectedPos = Vector2Utils::Divide(expectedPos, totalMass);

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
		for (std::list<GraphNode*>::iterator iter = m_attachedGraphNodes.begin();
			iter != m_attachedGraphNodes.end(); iter++)
		{
			GraphNode* nde = *iter;
			
			Vector2 d = Vector2Utils::Subtract(pt, nde->getPosition());
			float x = Vector2Utils::GetX(d);
			float y = Vector2Utils::GetY(d);

			// use the ellipse's equation to check the intersection

			if (((x*x)/(0.5f*0.5f) + (y*y)/(0.2f*0.2f) - 1) <= 0)
				return nde;
		}

		// used to add extent to m_area when checking
		Apoc3D::Math::RectangleF pointRegion(
			Vector2Utils::GetX(pt)-m_area.Width*0.5f, 
			Vector2Utils::GetY(pt)-m_area.Height*0.5f, 
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

	void QuadTreeNode::FillIntersectingNodesAttachment(FastList<GraphNode*>& list, const Apoc3D::Math::RectangleF& area)
	{
		for (std::list<GraphNode*>::iterator iter = m_attachedGraphNodes.begin();
			iter != m_attachedGraphNodes.end(); iter++)
		{
			list.Add(*iter);
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