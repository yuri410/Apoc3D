#include "Graph.h"
#include "apoc3d/Collections/Queue.h"
#include "apoc3d/Collections/HashMap.h"

#include "apoc3d/Math/RandomUtils.h"
#include "apoc3d/Math/Math.h"
#include "apoc3d/Library/tinythread.h"
#include "apoc3d/Platform/Thread.h"
#include "apoc3d/Utility/StringUtils.h"

namespace Apoc3D
{
	namespace Utility
	{
		//////////////////////////////////////////////////////////////////////////

		GraphLayout::GraphLayout(int32 areaEdgeLength, int32 workerThreads)
			: m_areaEdgeLength(areaEdgeLength), m_quadTreeUpdateInterval(0), m_adaptiveLayoutIterationPerFrame(0),
			m_adaptiveTimeScale(1), m_currentKEnergy(0)
		{
			int32 depth = 8;

			m_areaEdgeLeafNodeCount = 1 << (depth - 1);
			m_leafNodeEdgeLength = areaEdgeLength / m_areaEdgeLeafNodeCount;

			m_leafNodes = new QuadTreeNode*[m_areaEdgeLeafNodeCount * m_areaEdgeLeafNodeCount]();
			
			// 2048x2048 ends up with 16x16 on leaf nodes
			Apoc3D::Math::RectangleF area;
			area.Inflate(areaEdgeLength * 0.5f, areaEdgeLength * 0.5f);

			m_quadTree = new QuadTreeNode(nullptr, area, 8, [this](QuadTreeNode* node)
			{
				int qx, qy;

				Vector2 pos = Vector2(node->getArea().X, node->getArea().Y);
				ConvertCoord(pos, qx, qy);

				assert(qx >= 0); assert(qy >= 0);
				assert(qx < m_areaEdgeLeafNodeCount); assert(qy < m_areaEdgeLeafNodeCount);
				assert(!m_leafNodes[qy * m_areaEdgeLeafNodeCount + qx]);
				m_leafNodes[qy * m_areaEdgeLeafNodeCount + qx] = node;
			});

			for (int32 i = 0; i < workerThreads;i++)
			{
				m_physicsWorkers.Add(new PhysicsWorker(i));
			}

			m_layoutThread = new tthread::thread(&GraphLayout::LayoutThreadEntry, this);
			Platform::SetThreadName(m_layoutThread, L"Layout");
		}

		GraphLayout::~GraphLayout()
		{
			m_isShuttingDown = true;
			m_layoutThread->join();
			delete m_layoutThread;

			m_physicsWorkers.DeleteAndClear();

			Reset();
			delete m_quadTree;

			delete[] m_leafNodes;
		}

		void GraphLayout::Load(List<GraphNodeDefinition>& graphInfo, bool forceRandom)
		{
			Reset();
			PrepareAndWaitForIO();

			bool allHasPosition = true;
			for (const GraphNodeDefinition& gnd : graphInfo)
			{
				GraphNode* n = new GraphNode(this, m_quadTree, gnd.ID, gnd.HitTestRadius);
				if (gnd.HasPosition)
					n->setPosition(gnd.Position);
				else allHasPosition = false;

				m_idMapping.Add(gnd.ID, n);

				m_nodes.Add(n);
			}

			for (const GraphNodeDefinition& gnd : graphInfo)
			{
				for (int32 id : gnd.Neighbors)
				{
					GraphNode* thisNode = m_idMapping[gnd.ID];
					GraphNode* thatNode = m_idMapping[id];

					thisNode->Link(thatNode);
					thatNode->Link(thisNode);
				}
			}

			if (!allHasPosition || forceRandom)
				InitialPlacement();

			m_isProcessingLayout = true;
			m_quadTreeUpdateInterval = 0;
			m_adaptiveTimeScale = 1;
			m_adaptiveLayoutIterationPerFrame = 1;

			CompleteIO();
		}

		void GraphLayout::Save(List<GraphNodeDefinition>& graphInfo)
		{
			PrepareAndWaitForIO();

			graphInfo.ResizeDiscard(m_nodes.getCount());

			for (const GraphNode* gn : m_nodes)
			{
				GraphNodeDefinition gnd;
				gnd.ID = gn->getID();
				gnd.HasPosition = true;
				gnd.Position = gn->getPosition();
				gnd.HitTestRadius = gn->getHitTestRadius();

				for (const GraphNode* nn : gn->getNeighbors())
				{
					gnd.Neighbors.Add(nn->getID());
				}

				graphInfo.Add(gnd);
			}

			CompleteIO();
		}

		void GraphLayout::PrepareAndWaitForIO()
		{
			m_layoutCommandLock.lock();
			m_isDoingIO = true;
			m_layoutCommandLock.unlock();

			for (;;)
			{
				m_layoutCommandLock.lock();
				bool ready = m_isReadyForIO;
				m_layoutCommandLock.unlock();

				if (ready)
					break;

				Platform::ApocSleep(10);
			}
		}
		void GraphLayout::CompleteIO()
		{
			m_layoutCommandLock.lock();
			m_isDoingIO = false;
			m_layoutCommandLock.unlock();
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
		void GraphLayout::InitialPlacement()
		{
			for (GraphNode* gn : m_nodes)
			{
				float x = Randomizer::NextFloat(-m_areaEdgeLength * 0.25f, m_areaEdgeLength*0.25f);
				float y = Randomizer::NextFloat(-m_areaEdgeLength * 0.25f, m_areaEdgeLength*0.25f);

				gn->setPosition({ x, y });
			}

			return;

			bool* passed = new bool[m_nodes.getCount()];
			memset(passed, 0, sizeof(bool)*m_nodes.getCount());

			Queue<GraphNode*> bfsQueue;
			List<GraphNode*> islandNodes;
			List<Island*> islands;

			HashMap<GraphNode*, int32> indexMapping(m_nodes.getCount());
			for (int i = 0; i < m_nodes.getCount(); i++)
			{
				indexMapping.Add(m_nodes[i], i);
			}

			for (int i = 0; i < m_nodes.getCount(); i++)
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

						for (GraphNode* neighb : nde->getNeighbors())
						{
							int32 nodeIdx = indexMapping[neighb];

							if (!passed[nodeIdx])
							{
								bfsQueue.Enqueue(neighb);
								passed[nodeIdx] = true;
							}
						}
					}

					Island* isl = new Island();
					isl->NodeCount = islandNodes.getCount();
					isl->Nodes = islandNodes.AllocateArrayCopy();

					islands.Add(isl);
				}
			}
			delete[] passed;

			// each islands area in the final result is estimated to be a rectangle
			// the area is calculated based on the number of node inside.

			// calculate the total area
			float totalArea = 0;
			for (int i = 0; i < islands.getCount(); i++)
			{
				totalArea += (float)islands[i]->NodeCount;
			}

			// sort island by the node count
			islands.Sort([](const Island* a, const Island* b)
			{
				return -OrderComparer(a->NodeCount, b->NodeCount);
			});

			// fill the area using from large island to small island
			int edgeLen = (int)sqrtf(totalArea) + 1;
			bool* areaUse = new bool[edgeLen*edgeLen];
			memset(areaUse, 0, sizeof(bool)*edgeLen*edgeLen);
			int index = 0;
			for (int i = 0; i<edgeLen && index<islands.getCount(); i++)
			{
				for (int j = 0; j<edgeLen && index<islands.getCount(); j++)
				{
					if (!areaUse[i*edgeLen + j])
					{
						//float area = islands[index]->NodeCount * islands[index]->NodeCount;
						int len = (int)sqrtf((float)islands[index]->NodeCount);

						if (i + len >= edgeLen || j + len >= edgeLen)
							continue;

						bool isAvailable = true;
						for (int ii = 0; ii<len && isAvailable; ii++)
						{
							for (int jj = 0; jj<len && isAvailable; jj++)
							{
								if (areaUse[(i + ii)*edgeLen + j + jj])
								{
									isAvailable = false;
								}
							}
						}

						if (!isAvailable)
							continue;

						for (int ii = 0; ii<len; ii++)
						{
							for (int jj = 0; jj<len; jj++)
							{
								areaUse[(i + ii)*edgeLen + j + jj] = true;
							}
						}

						// shift all the island's nodes
						Vector2 transl(j * 2 - edgeLen*0.5f, i * 2 - edgeLen*0.5f);
						for (int k = 0; k < islands[index]->NodeCount; k++)
						{
							Vector2 pos = islands[index]->Nodes[k]->getPosition() * (len*0.5f);
							islands[index]->Nodes[k]->setPosition(pos + transl);

							// expand the nodes further
							islands[index]->Nodes[k]->setPosition(islands[index]->Nodes[k]->getPosition() * 2.5f);
						}


						delete[] islands[index]->Nodes;
						delete islands[index];
						index++;
					}
				}
			}
			delete[] areaUse;
		}


		void GraphLayout::Reset()
		{
			LayoutCommand c;
			c.Cmd = CommandType::Reset;

			m_layoutCommandLock.lock();
			m_commandQueue.Enqueue(c);
			m_layoutCommandLock.unlock();
		}

		void GraphLayout::AddNode(int32 id, float x, float y, float hitTestRadius)
		{
			LayoutCommand c;
			c.Cmd = CommandType::Add;
			c.ID = id;
			c.X = x;
			c.Y = y;
			c.R = hitTestRadius;

			m_layoutCommandLock.lock();
			m_commandQueue.Enqueue(c);
			m_layoutCommandLock.unlock();
		}
		void GraphLayout::DeleteNode(int32 id)
		{
			LayoutCommand c;
			c.Cmd = CommandType::Remove;
			c.ID = id;

			m_layoutCommandLock.lock();
			m_commandQueue.Enqueue(c);
			m_layoutCommandLock.unlock();
		}

		void GraphLayout::MoveNode(int32 id, float dx, float dy)
		{
			LayoutCommand c;
			c.Cmd = CommandType::Move;
			c.ID = id;
			c.X = dx; 
			c.Y = dy;

			m_layoutCommandLock.lock();
			m_commandQueue.Enqueue(c);
			m_layoutCommandLock.unlock();
		}

		void GraphLayout::IntersectNodes(int mx, int my)
		{
			LayoutCommand c;
			c.Cmd = CommandType::Intersect;
			c.X = (float)mx;
			c.Y = (float)my;

			m_layoutCommandLock.lock();
			m_commandQueue.Enqueue(c);
			m_layoutCommandLock.unlock();
		}

		void GraphLayout::setTechnique(Technique tech)
		{
			m_layoutCommandLock.lock();
			m_technique = tech;
			m_layoutCommandLock.unlock();
		}

		List<GraphNodeInfo> GraphLayout::getVisibleNodes()
		{
			tthread::lock_guard<tthread::mutex> locker(m_visibleNodesLock);
			return *m_currentVisisbleNodes;
		}
		List<GraphNodeInfo> GraphLayout::getIntersectingNodes()
		{
			tthread::lock_guard<tthread::mutex> locker(m_intersectingNodesLock);
			return *m_intersectingNodes;
		}

		void GraphLayout::ConvertCoord(const Vector2& pos, int& qx, int& qy)
		{
			qx = (int)floor((pos.X + m_areaEdgeLength * 0.5f) / m_leafNodeEdgeLength);
			qy = (int)floor((pos.Y + m_areaEdgeLength * 0.5f) / m_leafNodeEdgeLength);

			if (qx < 0) qx = 0;
			if (qy < 0) qy = 0;
			if (qx > m_areaEdgeLeafNodeCount - 1) qx = m_areaEdgeLeafNodeCount - 1;
			if (qy > m_areaEdgeLeafNodeCount - 1) qy = m_areaEdgeLeafNodeCount - 1;
		}


		void GraphLayout::LayoutThreadEntry(void* t) { ((GraphLayout*)t)->LayoutThreadMain(); }

		void GraphLayout::LayoutThreadMain()
		{
			while (!m_isShuttingDown)
			{
				const float dt = 0.016f;
				UpdateLayout(dt);

				if (!m_isProcessingLayout)
					Platform::ApocSleep(10);

				FindVisisbleNodes();

				for (;;)
				{
					volatile bool hasCmd = false;
					LayoutCommand cmd;

					m_layoutCommandLock.lock();
					if (m_commandQueue.getCount()>0)
					{
						cmd = m_commandQueue.Dequeue();
						hasCmd = true;
					}
					m_layoutCommandLock.unlock();

					if (hasCmd)
					{
						switch (cmd.Cmd)
						{
							case CommandType::Add:
							{
								GraphNode* n = new GraphNode(this, m_quadTree, cmd.ID, cmd.R);
								m_idMapping.Add(cmd.ID, n);

								MoveNode(cmd.ID, cmd.X, cmd.Y);
								m_nodes.Add(n);

								break;
							}
							case CommandType::Remove:
							{
								GraphNode* node = m_idMapping[cmd.ID];
								m_idMapping.Remove(cmd.ID);

								// clear the reference manually
								// doing in destructor will probably cause performance issues because
								// of the large number of nodes needed to clean as the app exits, when
								// the references do not necessarily need to be cleaned.
								if (node->getDockingNode())
								{
									node->getDockingNode()->Detach(node);
								}

								m_nodes.Remove(node);

								for (GraphNode* gn : node->getNeighbors())
								{
									gn->Unlink(node);
									//gn->UnlonkTopological(node);
								}

								delete node;
								break;
							}
							case CommandType::Move:
							{
								GraphNode* node = m_idMapping[cmd.ID];
								node->setPosition(node->getPosition() + Vector2(cmd.X, cmd.Y));
								break;
							}
							case CommandType::Reset:
							{
								m_quadTree->Clear();
								m_nodes.DeleteAndClear();
								m_idMapping.Clear();
								break;
							}
							case CommandType::Intersect:
							{
								m_intersectingNodes2->Clear();
								m_quadTree->IntersectNodes(Vector2(cmd.X, cmd.Y), *m_intersectingNodes2);

								m_intersectingNodesLock.lock();
								std::swap(m_intersectingNodes, m_intersectingNodes2);
								m_intersectingNodesLock.unlock();
								break;
							}
						}
					}
					else break;
				}

				for (;;)
				{
					m_layoutCommandLock.lock();
					bool isDoingIO = m_isDoingIO;
					m_layoutCommandLock.unlock();

					if (!isDoingIO)
					{
						m_layoutCommandLock.lock();
						m_isReadyForIO = false;
						m_layoutCommandLock.unlock();
						break;
					}

					m_layoutCommandLock.lock();
					m_isReadyForIO = true;
					m_layoutCommandLock.unlock();

					Platform::ApocSleep(10);
				}
			}
		}

		void GraphLayout::UpdateLayout(float dt)
		{
			Technique selectedTech;
			m_layoutCommandLock.lock();
			selectedTech = m_technique;
			m_layoutCommandLock.unlock();

			{
				Vector2 minPos = { -m_areaEdgeLength * 0.5f, -m_areaEdgeLength * 0.5f };
				Vector2 maxPos = -minPos;
				for (int i = 0; i < m_nodes.getCount(); i++)
				{
					GraphNode* gn = m_nodes[i];
					Vector2 pos = gn->getPosition();

					pos.X = Math::Clamp(pos.X, minPos.X, maxPos.X);
					pos.Y = Math::Clamp(pos.Y, minPos.Y, maxPos.Y);
					gn->setPosition(pos);
				}
			}
			

			if (m_isProcessingLayout)
			{
				// estimate the number of iterations acceptable not to
				// make the application quite laggy
				float ratio = m_nodes.getCount() > 0 ? 1666.7f / m_nodes.getCount() : 1.0f;
				if (ratio < 1) ratio = 1; // must run at least 1 time
				if (ratio > 10) ratio = 10;
				m_adaptiveLayoutIterationPerFrame = (int)ratio;

				float maxKEnergy = 0;
				float kEnergy = 0;

				for (int k = 0; k < m_adaptiveLayoutIterationPerFrame; k++)
				{
					maxKEnergy = 0;
					kEnergy = 0;

					// when the maximum kinetic energy is low, the time step can be
					// scaled up to increase simulation speed, getting the result faster
					float dt2 = dt * 3 * m_adaptiveTimeScale;
					
					// TECH_Fuzzy first calculated the center of mass of the entire graph.
					// Then only the repulsive forces from this point and neighbor graph nodes are taken into account.
					if (selectedTech == TECH_Fuzzy)
						m_centerOfMass = Vector2::Zero;

					//////////////////////////////////////////////////////////////////////////

					int32 currentWorkerIndex = 0;
					for (int32 i = 0; i < m_nodes.getCount();i++)
					{
						m_physicsWorkers[currentWorkerIndex++]->AddWorkItem({ PhysicsTask::Pre, m_nodes[i], dt2, selectedTech });
						currentWorkerIndex %= m_physicsWorkers.getCount();
					}

					for (auto p : m_physicsWorkers)
						p->WaitUntilClear();

					m_frameProgress = 0;
					currentWorkerIndex = 0;
					for (int32 i = 0; i < m_nodes.getCount(); i++)
					{
						m_physicsWorkers[currentWorkerIndex++]->AddWorkItem({ PhysicsTask::Mid, m_nodes[i], dt2, selectedTech });
						currentWorkerIndex %= m_physicsWorkers.getCount();
					}

					for (;;)
					{
						int32 countRemaining = 0;
						for (auto p : m_physicsWorkers)
							countRemaining += p->WaitUntilClear(10);

						if (countRemaining == 0)
							break;

						m_frameProgress = 1 - ((float)countRemaining / m_nodes.getCount());
					}

					m_frameProgress = 1;

					currentWorkerIndex = 0;
					for (int32 i = 0; i < m_nodes.getCount(); i++)
					{
						m_physicsWorkers[currentWorkerIndex++]->AddWorkItem({ PhysicsTask::Post, m_nodes[i], dt2, selectedTech });
						currentWorkerIndex %= m_physicsWorkers.getCount();
					}

					for (auto p : m_physicsWorkers)
						p->WaitUntilClear();

					//////////////////////////////////////////////////////////////////////////

					for (int i = 0; i < m_nodes.getCount(); i++)
					{
						GraphNode* gn = m_nodes[i];

						if (gn->getIsPositionDirty())
						{
							// update the quad tree
							PutGraphNode(gn);

							kEnergy += gn->getKineticEnergy();

							if (gn->getKineticEnergy() > maxKEnergy)
								maxKEnergy = gn->getKineticEnergy();
						}

						if (selectedTech == TECH_Fuzzy)
							m_centerOfMass += gn->getPosition();
					}

					if (selectedTech == TECH_Fuzzy)
					{
						// do the work on center of mass
						m_centerOfMass /= (float)m_nodes.getCount();
						static const float RepelRatio = 1.5f;
						float totalMass = (float)m_nodes.getCount();

						for (int i = 0; i < m_nodes.getCount(); i++)
						{
							GraphNode* gn = m_nodes[i];

							Vector2 d = m_centerOfMass - gn->getPosition();
							float dist = d.Length();
							if (dist < 1) dist = 1.0f / dist;
							d *= -dt2 * (totalMass*gn->getMass()) / (dist*dist*dist);

							gn->ApplyImpulse(d);
						}
					}

					if (maxKEnergy < 3.5f)
					{
						// dramatically, I found stacked nodes, which are trapped in some situation
						// by forces, can get out during these long time steps
						float incr;// = 1.0f /maxKEnergy;

						if (maxKEnergy>0.001f)
							incr = 3.5f / maxKEnergy;
						else
							incr = 1.0f;

						if (incr > 7.0f)
							incr = 7.0f;
						m_adaptiveTimeScale = 1 * incr;
					}
					else
					{
						m_adaptiveTimeScale = 1;
					}


					if (--m_quadTreeUpdateInterval < 0)
					{
						m_quadTree->Update(dt);
						m_quadTreeUpdateInterval = 15;
					}
				}

				// layout is done when the total energy is small enough
				// thresholds are made based on experiments
				if (kEnergy <= 0.00275f*m_nodes.getCount() && maxKEnergy < 0.02f)
				{
					m_isProcessingLayout = false;
					m_quadTree->Update(dt);
				}
				m_currentKEnergy = kEnergy;
			}
			else
			{
				// still keep placing the GraphNodes in the right QuadTree leaf
				for (int i = 0; i < m_nodes.getCount(); i++)
				{
					if (m_nodes[i]->getIsPositionDirty())
						PutGraphNode(m_nodes[i]);
				}
				if (--m_quadTreeUpdateInterval < 0)
				{
					m_quadTree->Update(dt);
					m_quadTreeUpdateInterval = 15;
				}
			}
		}

		void GraphLayout::PutGraphNode(GraphNode* gn)
		{
			QuadTreeNode* currentNode = gn->getDockingNode();

			// calculate the new leaf node's coord
			int qx, qy;
			ConvertCoord(gn->getPosition(), qx, qy);

			QuadTreeNode* newNode = nullptr;
			if (qx >= 0 && qy >= 0 && qx < m_areaEdgeLeafNodeCount && qy < m_areaEdgeLeafNodeCount)
			{
				newNode = m_leafNodes[qy * m_areaEdgeLeafNodeCount + qx];
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


		void GraphLayout::FindVisisbleNodes()
		{
			m_currentVisisbleNodes2->Clear();

			float ll = (float)m_leafNodeEdgeLength;

			Apoc3D::Math::RectangleF area = Viewport;
			area.Inflate(ll, ll);
			
			m_quadTree->FillIntersectingNodesAttachment(*m_currentVisisbleNodes2, area);

			m_visibleNodesLock.lock();
			std::swap(m_currentVisisbleNodes, m_currentVisisbleNodes2);
			m_visibleNodesLock.unlock();
		}



		GraphLayout::PhysicsWorker::PhysicsWorker(int32 idx)
		{
			StartBackground(L"PhysicsWorker " + StringUtils::IntToString(idx), 1);
		}
		void GraphLayout::PhysicsWorker::BackgroundMainProcess(PhysicsTask& item)
		{
			switch (item.TaskType)
			{
				case PhysicsTask::Pre:
					item.Subject->BeginPhysicsStep();
					break;
				case PhysicsTask::Mid:
					item.Subject->PhysicsStep(item.DT, item.SelectedTechnique);
					break;
				case PhysicsTask::Post:
					item.Subject->EndPhysicsStep();
					break;
			}
		}


		//////////////////////////////////////////////////////////////////////////

		const float Damping = 4.2f;
		const float K = 2.0f;
		const float NodeMass = 1.0f;
		const float OringialSpringLength = 1.5f;
		const float Eplison = 0.00001f;
		const float MaxVel = 50;
		const float RepelRatio = 1.5f;

		GraphNode::GraphNode(GraphLayout* graph, QuadTreeNode* tree, int32 id, float hitTestRadius)
			: m_nodeID(id), m_tree(tree), m_graph(graph), m_hitTestRadius(hitTestRadius)
		{
			m_position = Vector2(Randomizer::NextFloat() * 2 - 1, Randomizer::NextFloat() * 2 - 1);
			
			m_info.ID = id;
			m_info.Position = m_position;
		}

		void GraphNode::BeginPhysicsStep()
		{
			assert(!m_isInPhysicsStep);
			m_isInPhysicsStep = true;

			m_newPosition = m_position;
			m_newVelocity = m_velocity;
		}
		void GraphNode::EndPhysicsStep()
		{
			assert(m_isInPhysicsStep);
			m_isInPhysicsStep = false;

			m_velocity = m_newVelocity;
			m_position = m_newPosition;
			m_kineticEnergy = m_newKineticEnergy;

			m_isPositionDirty = true;

			m_info.Position = m_position;
		}

		void GraphNode::PhysicsStep(float dt, GraphLayout::Technique selectedTech)
		{
			// edge repulsive force
			{
				Vector2 force = Vector2::Zero;

				PointF minPos, maxPos;
				m_graph->getGraphArea(minPos, maxPos);

				float xdist = minPos.X - m_position.X;
				float ydist = minPos.Y - m_position.Y;

				if (xdist > -1) xdist = -1;
				if (ydist > -1) ydist = -1;

				force.X -= 1.0f / xdist;
				force.Y -= 1.0f / ydist;


				xdist = maxPos.X - m_position.X;
				ydist = maxPos.Y - m_position.Y;

				if (xdist < 1) xdist = 1;
				if (ydist < 1) ydist = 1;

				force.X -= 1.0f / xdist;
				force.Y -= 1.0f / ydist;

				force.X *= (maxPos.X - minPos.X) * 0.5f;
				force.Y *= (maxPos.Y - minPos.Y) * 0.5f;

				//float xdist = Math::Min(minPos.X - m_position.X, maxPos.X - m_position.X);
				//float ydist = Math::Min(minPos.Y - m_position.Y, maxPos.Y - m_position.Y);

				//if (xdist < -10 || xdist == 0) xdist = -10;
				//if (ydist < -10 || ydist == 0) ydist = -10;

				//force -= d * (RepelRatio * (getMass()) / (dist*dist*dist));
				
				m_newVelocity += force * (dt * RepelRatio * getMass() );
			}

			// spring force from adjacent nodes
			for (int i = 0; i < m_neighbour.getCount(); i++)
			{
				const GraphNode* nb = m_neighbour[i];

				Vector2 d = nb->m_position - m_position;
				float distance = d.Length();
				float force = (distance - OringialSpringLength) * K;

				m_newVelocity += d * (force * dt);
			}

			Vector2 force = Vector2::Zero;

			if (selectedTech == GraphLayout::TECH_Quad)
			{
				// gravitational force by recursively walking in the quad tree
				RecursiveRepulsiveForce(m_tree, force);
			}
			else if (selectedTech == GraphLayout::TECH_BruteForce)
			{
				// the brute force method just calculate repulsive force from other nodes one by one
				// the time is O(n^2) for each iteration
				const float MaxDist = 512;

				const List<GraphNode*>& nodes = m_graph->getInternalNodes();

				for (int i = 0; i < nodes.getCount(); i++)
				{
					const GraphNode* gn = nodes[i];
					Vector2 d = gn->m_position - m_position;
					float dist = d.Length();

					if (dist<0.01f || dist>MaxDist)
						continue;

					force -= d * (RepelRatio * (getMass()*gn->getMass()) / (dist*dist*dist));
				}
			}
			else if (selectedTech == GraphLayout::TECH_Fuzzy)
			{
				// Here, the neighbor forces, as a part of the TECH_Fuzzy method,
				// are calculated. The center of mass's force is done by the Graph class.
				for (int i = 0; i < m_neighbour.getCount(); i++)
				{
					const GraphNode* gn = m_neighbour[i];
					Vector2 d = gn->m_position - m_position;
					float dist = d.LengthSquared();

					if (dist < 0.01f) continue;

					force -= d * (RepelRatio * (getMass()*gn->getMass()) / (dist));
				}
			}

			m_newVelocity += force * dt;

			// the ordinary way of Euler numerical integration
			m_newVelocity *= 1 - Damping*dt;

			float vl = m_newVelocity.LengthSquared();
			m_newKineticEnergy = vl * NodeMass;



			if (vl > MaxVel*MaxVel)
			{
				m_newVelocity *= MaxVel / sqrtf(vl);
			}

			m_newPosition += m_newVelocity * dt;
		}

		void GraphNode::RecursiveRepulsiveForce(QuadTreeNode* node, Vector2& force)
		{
			static const float Threshold = 1.0f;

			Vector2 d = node->getCenterOfMass() - m_position;

			float dist = d.Length();
			if (dist < 0.001f)
				return;


			if ((node->getArea().Width / dist) >= Threshold)
			{
				// the node is too big, break into small parts to increase precision.
				for (int i = 0; i < 4; i++)
				{
					QuadTreeNode* subNode = node->getNode((QuadTreeNode::NodeCorner)i);
					if (subNode && subNode->getMass() < 0.1f)
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
						for (const GraphNode* gn : nodes)
						{
							d = gn->m_position - m_position;
							dist = d.Length();

							if (dist < 0.0001f)// || dist>MaxDist)
								continue;

							force -= d * RepelRatio * (getMass()*gn->getMass()) / (dist*dist*dist);
						}
						break;
					}
				}
			}
			else// if (dist<=MaxDist)
			{
				// otherwise, use the big QuadTreeNode as a body
				force -= d * (RepelRatio * (getMass()*node->getMass()) / (dist*dist*dist));
			}
		}

		float GraphNode::getMass() const { return NodeMass; }

		//////////////////////////////////////////////////////////////////////////


		QuadTreeNode::QuadTreeNode(QuadTreeNode* parent, const Apoc3D::Math::RectangleF& rect, int maxDepth, FunctorReference<void(QuadTreeNode*)> leafNodeCreated)
			: m_isDirty(false), m_parent(parent), m_totalAttachedInSubtree(0), m_area(rect), m_equviliantMass(0)
		{
			// not deep enough?
			if (maxDepth > 1)
			{
				RectangleF subRegion(rect.X, rect.Y, rect.Width / 2, rect.Height / 2);

				m_subNodes[QTN_TopLeft] = new QuadTreeNode(this, subRegion, maxDepth - 1, leafNodeCreated);

				subRegion.X += rect.Width / 2;
				m_subNodes[QTN_TopRight] = new QuadTreeNode(this, subRegion, maxDepth - 1, leafNodeCreated);

				subRegion.Y += rect.Height / 2;
				m_subNodes[QTN_BottomRight] = new QuadTreeNode(this, subRegion, maxDepth - 1, leafNodeCreated);

				subRegion.X -= rect.Width / 2;
				m_subNodes[QTN_BottomLeft] = new QuadTreeNode(this, subRegion, maxDepth - 1, leafNodeCreated);

				m_isLeafNode = false;
			}
			else
			{
				ZeroArray(m_subNodes);

				m_isLeafNode = true;

				leafNodeCreated(this);
			}

			m_centerOfMass = rect.getCenter();
		}

		QuadTreeNode::~QuadTreeNode()
		{
			for (QuadTreeNode* node : m_subNodes)
			{
				if (node)
				{
					delete node;
				}
			}
			ZeroArray(m_subNodes);
		}



		void QuadTreeNode::Clear()
		{
			m_attachedGraphNodes.Clear();

			for (QuadTreeNode* node : m_subNodes)
			{
				if (node)
				{
					node->Clear();
				}
			}
		}

		void QuadTreeNode::Update(float dt)
		{
			if (m_isDirty)
			{
				m_totalAttachedInSubtree = 0;
				m_equviliantMass = 0;
				m_centerOfMass = Vector2::Zero;

				for (QuadTreeNode* node : m_subNodes)
				{
					if (node)
					{
						node->Update(dt);
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

					expectedPos += nde->getPosition() * m;
					m_totalAttachedInSubtree++;
				}

				// these on internal nodes.
				// Go through all virtual-Nodes: the internal tree node,
				// which is treated a sub tree of bodies as an entire body
				for (QuadTreeNode* node : m_subNodes)
				{
					if (node)
					{
						m_totalAttachedInSubtree += node->m_totalAttachedInSubtree;
						totalMass += node->m_equviliantMass;

						expectedPos += node->m_centerOfMass * node->getMass();
					}
				}

				// div by 0 check
				if (totalMass > 0.1f)
				{
					expectedPos /= totalMass;

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
			for (QuadTreeNode* node : m_subNodes)
			{
				if (node)
				{
					node->MakeAllDirty();
				}
			}
		}

		void QuadTreeNode::IntersectNodes(const Vector2& pt, List<GraphNodeInfo>& nodes)
		{
			for (GraphNode* gn : m_attachedGraphNodes)
			{
				const GraphNodeInfo& info = gn->getInfo();

				Vector2 d = pt - info.Position;// gn->getPosition();

				if (d.LengthSquared() < gn->getHitTestRadius())
					nodes.Add(info);
			}

			// used to add extent to m_area when checking

			Apoc3D::Math::RectangleF pointRegion(
				pt.X - m_area.Width*0.5f,
				pt.Y - m_area.Height*0.5f,
				m_area.Width, m_area.Height);

			for (QuadTreeNode* node : m_subNodes)
			{
				if (node && (m_area.Intersects(pointRegion) || m_area.Contains(pointRegion)))
				{
					node->IntersectNodes(pt, nodes);
				}
			}
		}

		void QuadTreeNode::FillIntersectingNodesAttachment(List<GraphNodeInfo>& list, const Apoc3D::Math::RectangleF& area)
		{
			for (GraphNode* nde : m_attachedGraphNodes)
			{
				list.Add(nde->getInfo());
			}

			for (QuadTreeNode* node : m_subNodes)
			{
				if (node && (m_area.Intersects(area) || m_area.Contains(area)))
				{
					node->FillIntersectingNodesAttachment(list, area);
				}
			}
		}

	}
}