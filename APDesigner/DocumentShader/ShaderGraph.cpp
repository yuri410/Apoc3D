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

#include "ShaderGraph.h"

#include "GraphNode.h"
#include "QuadTree.h"

namespace APDesigner
{
	class LeafNodeCreatedHandlerImpl : public LeafNodeCreatedHandler
	{
	public:
		LeafNodeCreatedHandlerImpl()
		{
			memset(m_leafNodes, 0, sizeof(m_leafNodes));
		}

		virtual void LeafNodeCreated(QuadTreeNode* node)
		{
			int qx, qy;

			Vector2 pos = Vector2(node->getArea().X, node->getArea().Y);
			ShaderGraph::ConvertCoord(pos, qx, qy);

			assert(qx>=0); assert(qy>=0);
			assert(qx<128); assert(qy<128);
			assert(!m_leafNodes[qy][qx]);
			m_leafNodes[qy][qx] = node;
		}
	
		QuadTreeNode* m_leafNodes[128][128];
	};

	ShaderGraph::ShaderGraph()
		: m_zooming(2), m_viewPos(0,0), m_quadTreeUpdateInterval(0), m_adaptiveLayoutIterationPerFrame(0),
		m_adaptiveTimeScale(1), m_currentKEnergy(0), m_technique(TECH_Quad), m_centerOfMass(Vector2::Zero),
		m_highlightingNode(0)
	{
		LeafNodeCreatedHandlerImpl handler;

		// initialize with a area of 2048x2048

		// ends up with 16x16 on leaf nodes
		Apoc3D::Math::RectangleF area(-1024, -1024, 2048, 2048);
		m_quadTree = new QuadTreeNode(0, area, 8, &handler);

		memcpy(m_leafNodes, handler.m_leafNodes, sizeof(m_leafNodes));
	}

	ShaderGraph::~ShaderGraph()
	{
		Reset();
		delete m_quadTree;
	}

	void ShaderGraph::Load(const String& filePath)
	{
		Reset();

		//std::unordered_map<String, GraphNode*> hashTable;

		//std::wifstream instrm(filePath.c_str(), std::ios::in);
		//
		//int index =0;
		//while (!instrm.eof())
		//{			
		//	String item;
		//	String dependent;

		//	{
		//		String line;
		//		std::getline(instrm, line);

		//		std::vector<String> list = StringUtils::Split(line, L" ,\t;");
		//		if (list.size()>1)
		//			dependent = list[1];
		//		if (list.size()>0)
		//			item = list[0];
		//	}

		//	if (item.empty())
		//		break;

		//	GraphNode* a = 0;
		//	GraphNode* b = 0;

		//	std::unordered_map<String, GraphNode*>::iterator iter = hashTable.find(item);
		//	if (iter == hashTable.end())
		//	{
		//		a = new GraphNode(this, m_quadTree, item,index++);
		//		hashTable.insert(std::make_pair(item, a));
		//	}
		//	else
		//	{
		//		a = iter->second;
		//	}

		//	// if the dependent node is not specified, just leave
		//	// "node a" as an isolated one.
		//	if (!dependent.empty())
		//	{
		//		iter = hashTable.find(dependent);
		//		if (iter == hashTable.end())
		//		{
		//			b = new GraphNode(this, m_quadTree, dependent,index++);
		//			hashTable.insert(std::make_pair(dependent, b));
		//		}
		//		else
		//		{
		//			b = iter->second;
		//		}

		//		// make sure its not linked before
		//		if (!b->IsDirectedTo(a))
		//		{
		//			// link on dual direction
		//			a->Link(b);
		//			b->Link(a);

		//			b->LinkTopological(a);
		//		}
		//	}
		//	
		//}

		//// preserve some space
		//m_nodes.ResizeDiscard((int)hashTable.size());

		//index = 0;
		//for (std::unordered_map<String, GraphNode*>::iterator iter = hashTable.begin();
		//	iter != hashTable.end();iter++)
		//{
		//	iter->second->setIndex(index++);
		//	m_nodes.Add(iter->second);
		//}

		InitialPlacement();

		m_isProcessingLayout = true;
		m_quadTreeUpdateInterval = 15;
		m_adaptiveTimeScale = 1;
		m_adaptiveLayoutIterationPerFrame = 1;
	}

	void ShaderGraph::Save(const String& filePath)
	{
		//std::wofstream outstrm(filePath.c_str(), std::ios::out);

		//for (int i=0;i<m_nodes.getCount();i++)
		//{
		//	const String& item = m_nodes[i]->getName();

		//	const List<GraphNode*>& neighbors = m_nodes[i]->getNeighbors();
		//	// find dependents among all linked nodes.
		//	for (int j=0;j<neighbors.getCount();j++)
		//	{
		//		const List<GraphNode*>& targets = neighbors[j]->getTargets();
		//		for (int k=0;k<targets.getCount();k++)
		//		{
		//			if (targets[k] == m_nodes[i])
		//			{
		//				const String& dependent = neighbors[j]->getName(); 
		//				
		//				outstrm << item << L" " << dependent << endl;
		//				break;
		//			}
		//		}
		//		
		//	}
		//}
	}
	struct Island
	{
		GraphNode** Nodes;
		int NodeCount;
	};
	/** The idea is if islands are initially placed apart rather than randomly
	 *  intersected with each other before going into the layout algorithm,
	 *  the distances between island the results may not be quite large, causing
	 *  the graph looks spacious and empty.
	 */
	void ShaderGraph::InitialPlacement()
	{
		bool* passed = new bool[m_nodes.getCount()];
		memset(passed, 0, sizeof(bool)*m_nodes.getCount());

		Queue<GraphNode*> bfsQueue;
		List<GraphNode*> islandNodes;


		List<Island*> islands;

		for (int i=0;i<m_nodes.getCount();i++)
		{
			if (!passed[i])
			{
				bfsQueue.Clear();
				islandNodes.Clear();

				// a board first pass is used here to pass the 
				// graph
				
				// beginning node
				bfsQueue.Enqueue(m_nodes[i]);
				passed[i] = true;

				while (bfsQueue.getCount()>0)
				{
					GraphNode* nde = bfsQueue.Dequeue();

					islandNodes.Add(nde);

					const List<GraphNode*>& neighb = nde->getNeighbors();
					for (int j=0;j<neighb.getCount();j++)
					{
						if (!passed[neighb[j]->getIndex()])
						{
							bfsQueue.Enqueue(neighb[j]);
							passed[neighb[j]->getIndex()] = true;
						}
					}
				}

				GraphNode** nodeList = new GraphNode*[islandNodes.getCount()];
				memcpy(nodeList, islandNodes.getElements(), islandNodes.getCount() * sizeof(GraphNode*));

				Island* isl = new Island();
				isl->NodeCount = islandNodes.getCount();
				isl->Nodes = nodeList;

				islands.Add(isl);
			}
		}
		delete[] passed;

		// each islands area in the final result is estimated to be a rectangle
		// the area is calculated based on the number of node inside.

		// calculate the total area
		float totalArea = 0;
		for (int i=0;i<islands.getCount();i++)
		{
			float area = (float)islands[i]->NodeCount;
			totalArea += area;
		}

		// sort island by the node count
		for (int i=0;i<islands.getCount();i++)
		{
			for (int j=i+1;j<islands.getCount();j++)
			{
				if (islands[i]->NodeCount < islands[j]->NodeCount)
				{
					std::swap(islands[i], islands[j]);
				}
			}
		}

		// fill the area using from large island to small island
		int edgeLen = (int)sqrtf(totalArea)+1;
		bool* areaUse = new bool[edgeLen*edgeLen];
		memset(areaUse, 0, sizeof(bool)*edgeLen*edgeLen);
		int index = 0;
		for (int i=0;i<edgeLen && index<islands.getCount();i++)
		{
			for (int j=0;j<edgeLen && index<islands.getCount();j++)
			{
				if (!areaUse[i*edgeLen+j])
				{
					//float area = islands[index]->NodeCount * islands[index]->NodeCount;
					int len = (int)sqrtf((float)islands[index]->NodeCount);

					if (i+len>=edgeLen || j+len>=edgeLen)
						continue;

					bool isAvailable = true;
					for (int ii=0;ii<len && isAvailable;ii++)
					{
						for (int jj=0;jj<len && isAvailable;jj++)
						{
							if (areaUse[(i+ii)*edgeLen+j+jj])
							{
								isAvailable= false;
							}
						}
					}

					if (!isAvailable)
						continue;

					for (int ii=0;ii<len;ii++)
					{
						for (int jj=0;jj<len;jj++)
						{
							areaUse[(i+ii)*edgeLen+j+jj] = true;
						}
					}

					// shift all the island's nodes
					Vector2 transl(j*2-edgeLen*0.5f,i*2-edgeLen*0.5f);
					for (int k=0;k<islands[index]->NodeCount;k++)
					{
						Vector2 pos = Vector2::Multiply(islands[index]->Nodes[k]->getPosition(), len*0.5f);
						islands[index]->Nodes[k]->setPosition(Vector2::Add(pos, transl));

						// expand the nodes further
						islands[index]->Nodes[k]->setPosition(Vector2::Multiply(islands[index]->Nodes[k]->getPosition(), 2.5f));
					}


					delete[] islands[index]->Nodes;
					delete islands[index];
					index++;
				}
			}
		}
		delete[] areaUse;
	}

	void ShaderGraph::Reset()
	{
		m_highlightingNode = 0;
		m_quadTree->Clear();
		for (int i=0;i<m_nodes.getCount();i++)
		{
			delete m_nodes[i];
		}
		m_nodes.Clear();
		m_currentVisisbleNodes.Clear();
	}

	void ShaderGraph::Update(const GameTime* time)
	{
		if (m_isProcessingLayout)
		{
			// estimate the number of iterations acceptable not to
			// make the application quite laggy
			float ratio = m_nodes.getCount()>0? 1666.7f/m_nodes.getCount() : 1.0f;
			if (ratio<1)ratio=1; // must run at least 1 time
			if (ratio>10) ratio = 10;
			m_adaptiveLayoutIterationPerFrame = (int)ratio;

			float maxKEnergy = 0;
			float kEnergy = 0;

			
			for (int k=0;k<m_adaptiveLayoutIterationPerFrame;k++)
			{
				maxKEnergy = 0;
				kEnergy = 0;

				// when the maximum kinetic energy is low, the time step can be
				// scaled up to increase simulation speed, getting the result
				// in a fewer time
				GameTime time2 = *time;
				time2.ElapsedTime *= 3 * m_adaptiveTimeScale;

				// TECH_Fuzzy first calculated the center of mass of the entire graph.
				// Then only the repulsive forces from this point and neighbor graph nodes are taken into account.
				if (m_technique == TECH_Fuzzy)
					m_centerOfMass = Vector2::Zero;
				for (int i=0;i<m_nodes.getCount();i++)
				{
					m_nodes[i]->UpdatePhysicsSimulation(&time2);

					if (m_nodes[i]->getIsPositionDirty())
					{
						// update the quad tree
						PutGraphNode(m_nodes[i]);

						kEnergy += m_nodes[i]->getKineticEnergy();

						if (m_nodes[i]->getKineticEnergy()>maxKEnergy)
							maxKEnergy = m_nodes[i]->getKineticEnergy();
					}

					if (m_technique == TECH_Fuzzy)
						m_centerOfMass = Vector2::Add(m_centerOfMass, m_nodes[i]->getPosition());
				}

				if (m_technique == TECH_Fuzzy)
				{
					// do the work on center of mass
					m_centerOfMass = Vector2::Divide(m_centerOfMass, (float)m_nodes.getCount());
					static const float RepelRatio = 1.5f;
					float totalMass = (float)m_nodes.getCount();

					for (int i=0;i<m_nodes.getCount();i++)
					{
						const GraphNode* nde = m_nodes[i];

						Vector2 d = Vector2::Subtract(m_centerOfMass, nde->getPosition());
						float dist = Vector2::Length(d);
						if (dist<1) dist = 1.0f/dist;
						d = Vector2::Multiply(d, -time2.getElapsedTime() * (totalMass*nde->getMass())/(dist*dist*dist));
						
						m_nodes[i]->ApplyImpulse(d);
					}
				}

				if (maxKEnergy<3.5f)
				{
					// dramatically, I found stacked nodes, which are trapped in some situation
					// by forces, can get out during these long time steps
					float incr;// = 1.0f /maxKEnergy;
					
					if (maxKEnergy>0.001f)
						incr = 3.5f/maxKEnergy;
					else
						incr = 1.0f;

					if (incr>7.0f)
						incr = 7.0f;
					m_adaptiveTimeScale = 1 * incr;
				}
				else
				{
					m_adaptiveTimeScale = 1;
				}


				if (--m_quadTreeUpdateInterval<0)
				{
					m_quadTree->Update(time);
					m_quadTreeUpdateInterval = 15;
				}
			}


			// layout is done when the total energy is small enough
			// thresholds are made based on experiments
			if (kEnergy<=0.00275f*m_nodes.getCount() && maxKEnergy<0.02f)
			{
				m_isProcessingLayout = false;
				m_quadTree->Update(time);
			}
			m_currentKEnergy = kEnergy;
		}
		else
		{
			// still keep placing the GraphNodes in the right QuadTree leaf
			for (int i=0;i<m_nodes.getCount();i++)
			{
				if (m_nodes[i]->getIsPositionDirty())
					PutGraphNode(m_nodes[i]);
			}
			if (--m_quadTreeUpdateInterval<0)
			{
				m_quadTree->Update(time);
				m_quadTreeUpdateInterval = 15;
			}
		}
	}

	void ShaderGraph::SetUpNodeColor(bool isolated, float scale, float ascale)
	{
		if (isolated)
		{
			//glColor4f(72 * scale / 255.0f, 231 * scale / 255.0f, 162 * scale / 255.0f, ascale);
		}
		else
		{
			//glColor4f(246 * scale / 255.0f, 196 * scale / 255.0f,  60 * scale / 255.0f, ascale);
		}
	}

	void ShaderGraph::Draw()
	{
		//GLint viewport[4];
		//glGetIntegerv(GL_VIEWPORT, viewport);

		//// apply the current view
		//glPushAttrib(GL_ALL_ATTRIB_BITS);
		//glEnable(GL_BLEND);
		//glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

		//glPushMatrix();
		//glLoadIdentity();
		//

		//glBegin(GL_QUADS);
		//glColor3ub(89,89,89);
		//glVertex2i(viewport[0], viewport[1]);
		//glVertex2i(viewport[0]+viewport[2], viewport[1]);
		//glColor3ub(58,58,58);
		//glVertex2i(viewport[0]+viewport[2], viewport[1]+viewport[3]);
		//glVertex2i(viewport[0], viewport[1]+viewport[3]);
		//glEnd();

		//float scaling = powf(2,m_zooming);
		//Matrix result;
		//GetViewMatrix(result);

		//glLoadMatrixf(reinterpret_cast<const float*>(&result));
		//
		//DrawBackground();
		//glDisable(GL_BLEND);

		//glLineWidth(0.017f * scaling);
		//glColor4f(0.2f,0.2f,0.2f,1);
		//DrawLinks();

		//glLineWidth(0.01f * scaling);
		//glColor4f(0.8f,0.8f,0.8f,1);
		//DrawLinks();

		//glLineWidth(1);


		//FindVisisbleNodes();

		//for (int i=0;i<m_currentVisisbleNodes.getCount();i++)
		//{
		//	SetUpNodeColor(!m_currentVisisbleNodes[i]->IsUseable(), 0.2f);
		//	DrawEllipse(m_currentVisisbleNodes[i]->getPosition(), 0.51f, 0.21f, 36);

		//	SetUpNodeColor(!m_currentVisisbleNodes[i]->IsUseable(), 1.0f);
		//	DrawEllipse(m_currentVisisbleNodes[i]->getPosition(), 0.49f, 0.20f, 36);
		//}

		//// draw highlighted node: the mouse selected one
		//if (m_highlightingNode)
		//{
		//	glEnable(GL_BLEND);
		//	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

		//	// draw glow
		//	glBegin(GL_TRIANGLE_FAN);

		//	const Vector2& pos = m_highlightingNode->getPosition();
		//	SetUpNodeColor(!m_highlightingNode->IsUseable(), 2.5f, 0.5f);
		//	glVertex2f(Vector2Utils::GetX(pos), Vector2Utils::GetY(pos));
		//	glColor4f(0.5f,0.5f,0.5f,0.1f);
		//	int segments = 72;
		//	float a = 5; float b = 2;
		//	for (int j=0;j<=segments;j++)
		//	{
		//		float ang = (360/segments) * j * Apoc3D::Math::PI / 180.0f;
		//		float x = a * cosf(ang) + Vector2Utils::GetX(pos);
		//		float y = b * sinf(ang) + Vector2Utils::GetY(pos);
		//		glVertex2f(x,y);
		//	}

		//	glEnd();
		//	glDisable(GL_BLEND);

		//	SetUpNodeColor(!m_highlightingNode->IsUseable(), 0.2f * 2.5f);
		//	DrawEllipse(m_highlightingNode->getPosition(), 0.51f * 1.2f, 0.21f * 1.2f, 36);

		//	SetUpNodeColor(!m_highlightingNode->IsUseable(), 2.5f);
		//	DrawEllipse(m_highlightingNode->getPosition(), 0.49f * 1.2f, 0.20f * 1.2f, 36);

		//}

		//glPopMatrix();
		//glPopAttrib();
	}
	void ShaderGraph::DrawEllipse(const Vector2& pos, float a, float b, int segments)
	{
		//glBegin(GL_TRIANGLE_FAN);

		//
		//glVertex2f(Vector2Utils::GetX(pos), Vector2Utils::GetY(pos));
		//for (int j=0;j<=segments;j++)
		//{
		//	float ang = (360/segments) * j * Apoc3D::Math::PI / 180.0f;
		//	float x = a * cosf(ang) + Vector2Utils::GetX(pos);
		//	float y = b * sinf(ang) + Vector2Utils::GetY(pos);
		//	glVertex2f(x,y);
		//}

		//glEnd();
	}
	void ShaderGraph::DrawLinks()
	{
		//glBegin(GL_LINES);
		//for (int i=0;i<m_currentVisisbleNodes.getCount();i++)
		//{
		//	const GraphNode* node = m_currentVisisbleNodes[i];
		//	const Vector2& pos = m_currentVisisbleNodes[i]->getPosition();
		//	for (int j=0;j<node->getTargets().getCount();j++)
		//	{
		//		glVertex2f(Vector2Utils::GetX(pos), Vector2Utils::GetY(pos));
		//		glVertex2f(Vector2Utils::GetX(node->getTargets()[j]->getPosition()), Vector2Utils::GetY(node->getTargets()[j]->getPosition()));
		//	}

		//}

		//glEnd();

		//if (m_zooming>4)
		//{

		//	// draw arrows
		//	glColor4f(1,1,1,1);
		//	glBegin(GL_TRIANGLES);
		//	for (int i=0;i<m_currentVisisbleNodes.getCount();i++)
		//	{
		//		const GraphNode* node = m_currentVisisbleNodes[i];
		//		const Vector2& pos = m_currentVisisbleNodes[i]->getPosition();
		//		for (int j=0;j<node->getTargets().getCount();j++)
		//		{
		//			Vector2 d = Vector2Utils::Subtract(pos, node->getTargets()[j]->getPosition());
		//			d = Vector2Utils::Normalize(d);
		//			// use the ellipse's equation to check the intersection
		//			// enumerate the possible points
		//			for (float k=0.2f; k<=0.6f;k+=0.1f)
		//			{
		//				Vector2 pt = Vector2Utils::Multiply(d, k);
		//				float dx = Vector2Utils::GetX(pt);
		//				float dy = Vector2Utils::GetY(pt);

		//				if (((dx*dx)/(0.5f*0.5f) + (dy*dy)/(0.2f*0.2f) - 1) > 0)
		//				{
		//					pt = Vector2Utils::Add(node->getTargets()[j]->getPosition(), Vector2Utils::Multiply(d, k+0.05f));
		//					dx = Vector2Utils::GetX(pt);
		//					dy = Vector2Utils::GetY(pt);

		//					Vector2 n = Vector2Utils::LDVector(-Vector2Utils::GetY(d), Vector2Utils::GetX(d));

		//					// the triangle is 0.2x0.2
		//					n = Vector2Utils::Multiply(n, 0.1f);

		//					Vector2 back = Vector2Utils::Multiply(d, 0.1f);

		//					glVertex2f(dx,dy);
		//					glVertex2f(dx + Vector2Utils::GetX(n) + Vector2Utils::GetX(back),dy + Vector2Utils::GetY(n) + Vector2Utils::GetY(back));
		//					glVertex2f(dx - Vector2Utils::GetX(n) + Vector2Utils::GetX(back),dy - Vector2Utils::GetY(n) + Vector2Utils::GetY(back));


		//					break;
		//				}
		//			}
		//		}

		//	}
		//	glEnd();
		//	// draw arrows
		//	glColor4f(1,1,1,1);
		//	glBegin(GL_TRIANGLES);
		//	for (int i=0;i<m_currentVisisbleNodes.getCount();i++)
		//	{
		//		const GraphNode* node = m_currentVisisbleNodes[i];
		//		const Vector2& pos = m_currentVisisbleNodes[i]->getPosition();
		//		for (int j=0;j<node->getTargets().getCount();j++)
		//		{
		//			Vector2 d = Vector2Utils::Subtract(pos, node->getTargets()[j]->getPosition());
		//			d = Vector2Utils::Normalize(d);
		//			// use the ellipse's equation to check the intersection
		//			// enumerate the possible points
		//			for (float k=0.2f; k<=0.6f;k+=0.1f)
		//			{
		//				Vector2 pt = Vector2Utils::Multiply(d, k);
		//				float dx = Vector2Utils::GetX(pt);
		//				float dy = Vector2Utils::GetY(pt);

		//				if (((dx*dx)/(0.5f*0.5f) + (dy*dy)/(0.2f*0.2f) - 1) > 0)
		//				{
		//					pt = Vector2Utils::Add(node->getTargets()[j]->getPosition(), Vector2Utils::Multiply(d, k+0.05f));
		//					dx = Vector2Utils::GetX(pt);
		//					dy = Vector2Utils::GetY(pt);

		//					Vector2 n = Vector2Utils::LDVector(-Vector2Utils::GetY(d), Vector2Utils::GetX(d));

		//					// the triangle is 0.2x0.2
		//					n = Vector2Utils::Multiply(n, 0.1f);

		//					Vector2 back = Vector2Utils::Multiply(d, 0.1f);

		//					glVertex2f(dx,dy);
		//					glVertex2f(dx + Vector2Utils::GetX(n) + Vector2Utils::GetX(back),dy + Vector2Utils::GetY(n) + Vector2Utils::GetY(back));
		//					glVertex2f(dx - Vector2Utils::GetX(n) + Vector2Utils::GetX(back),dy - Vector2Utils::GetY(n) + Vector2Utils::GetY(back));


		//					break;
		//				}
		//			}
		//		}

		//	}
		//	glEnd();
		//}
	}
	void ShaderGraph::DrawBackground()
	{
		//glLineWidth(0.5f);

		//glColor4f(0.65f,0.65f,0.65f, 0.5f);
		//glBegin(GL_LINES);
		//for (int i=-1024;i<=1024;i+=128)
		//{
		//	glVertex2i(-1024, i);
		//	glVertex2i(1024, i);
		//}
		//for (int i=-1024;i<=1024;i+=128)
		//{
		//	glVertex2i(i,-1024);
		//	glVertex2i(i,1024);
		//}

		//glEnd();
	}

	void ShaderGraph::FindVisisbleNodes()
	{
		Matrix result;
		GetViewMatrix(result);
		result.Inverse();

		m_currentVisisbleNodes.Clear();

		//GLint viewport[4];
		//glGetIntegerv(GL_VIEWPORT, viewport);

		// get the world coord of 2 corner of viewport
		Vector3 topLeft((float)Viewport.X,(float)Viewport.Y,0);
		topLeft = Vector3::TransformCoordinate(topLeft, result);

		Vector3 buttomRight((float)Viewport.getBottom(),(float)Viewport.getRight(),0);
		buttomRight = Vector3::TransformCoordinate(buttomRight, result);

		Apoc3D::Math::RectangleF area(topLeft.X-16, topLeft.Y-16, 
			buttomRight.X-topLeft.X + 32, buttomRight.Y-topLeft.Y + 32);

		m_quadTree->FillIntersectingNodesAttachment(m_currentVisisbleNodes, area);
	}

	void ShaderGraph::PutGraphNode(GraphNode* gn)
	{
		QuadTreeNode* currentNode = gn->getDockingNode();

		// calculate the new leaf node's coord
		int qx, qy;
		ShaderGraph::ConvertCoord(gn->getPosition(), qx, qy);

		QuadTreeNode* newNode = 0;
		if (qx>=0 && qy>=0
		 && qx<128 && qy<128)
		{
			newNode = m_leafNodes[qy][qx];
		}

		// check if the GraphNode does moved across 2 quad tree leaf nodes
		if (currentNode != newNode)
		{
			if (currentNode)
				currentNode->Detach(gn);
			if (newNode)
				newNode->Attach(gn);
			gn->setDockingNode(newNode);
		}
		
		gn->setIsPositionDirty(false);
	}

	/************************************************************************/
	/*                                                                      */
	/************************************************************************/

	void ShaderGraph::PanView(float dx, float dy)
	{
		m_viewPos = Vector2::Add(m_viewPos, 
			Vector2(dx/powf(2,m_zooming),dy/powf(2,m_zooming)));
	}

	GraphNode* ShaderGraph::IntersectNodes(int mx, int my)
	{
		Matrix result;
		GetViewMatrix(result);
		result.Inverse();

		Vector3 pos = Vector3((float)mx,(float)my,0);
		pos = Vector3::TransformCoordinate(pos, result);


		return m_quadTree->IntersectNodes(Vector2(pos.X, pos.Y));
	}

	void ShaderGraph::AddNode(const String& name, float x, float y)
	{
		// the index is only useful for InitialLayout(), 
		// use 0 here because InitialLayout will be only used
		// when opening a file
		GraphNode* n = new GraphNode(this, m_quadTree, name, 0);
		MoveNode(n, x,y);
		m_nodes.Add(n);
	}
	void ShaderGraph::DeleteNode(GraphNode* node)
	{
		// do clearing the reference manually
		// doing in destructor will probably cause performance issues because
		// of the large number of nodes needed to clean as the app exits, when
		// the references do not necessarily be cleaned.
		if (node->getDockingNode())
		{
			node->getDockingNode()->Detach(node);
		}

		m_nodes.Remove(node);

		for (int j=0;j<node->getTargets().getCount();j++)
		{
			node->getTargets()[j]->setIsUseable(false);
		}

		for (int j=0;j<node->getNeighbors().getCount();j++)
		{
			node->getNeighbors()[j]->Unlink(node);
			node->getNeighbors()[j]->UnlonkTopological(node);
		}

		if (m_highlightingNode == node)
			m_highlightingNode = 0;
		delete node;
	}

	void ShaderGraph::MoveNode(GraphNode* node, float dx, float dy)
	{
		node->setPosition(Vector2::Add(node->getPosition(), 
			Vector2(dx/powf(2,m_zooming),dy/powf(2,m_zooming))));
	}

	void ShaderGraph::GetViewMatrix(Matrix& mtrx)
	{
		//GLint viewport[4];
		//glGetIntegerv(GL_VIEWPORT, viewport);

		Matrix preTrans;
		preTrans.LoadIdentity();
		Matrix::CreateTranslation(preTrans, 
			m_viewPos.X, 
			m_viewPos.Y, 0);
		Matrix view;

		float scaling = powf(2,m_zooming);

		Matrix::CreateScale(view, scaling, scaling,1);

		view.SetTranslation(
			Vector3(Viewport.getRight()*0.5f, 
			Viewport.getBottom()*0.5f, 0));

		
		Matrix::Multiply(mtrx, preTrans, view);
		
	}
}