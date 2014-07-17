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
#include "PathFinder.h"
#include "apoc3d/Math/MathCommon.h"

namespace Apoc3DEx
{
	namespace AI
	{
		PathFinderManager::PathFinderManager(PathFinderField* fld)
			: m_terrain(fld)
		{
			m_units = new AStarNode[fld->getWidth() * fld->getHeight()];
			for (int32 i=0;i<fld->getHeight();i++)
			{
				for (int32 j=0;j<fld->getWidth();j++)
				{
					m_units[i * fld->getWidth() + j] = AStarNode(j,i);
				}
			}
		}

		PathFinderManager::~PathFinderManager()
		{
			delete[] m_units;
		}

		PathFinder* PathFinderManager::CreatePathFinder()
		{
			return new PathFinder(m_terrain, m_units);
		}

		/************************************************************************/
		/*   PathFinder                                                         */
		/************************************************************************/

		PathFinder::PathFinder(PathFinderField* terrain, AStarNode* units)
			: MaxStep(-1), TurnCost(1), ConsiderFieldWeightCost(false), ConsiderFieldDifferencialWeightCost(false), UseManhattanDistance(false),
			m_terrain(terrain), m_units(units), 
			m_width(terrain->getWidth()), m_height(terrain->getHeight())
		{
			Set8DirectionTable();
		}

		PathFinder::PathFinder(PathFinderManager* mgr)
			: MaxStep(-1), TurnCost(1), ConsiderFieldWeightCost(false), ConsiderFieldDifferencialWeightCost(false), UseManhattanDistance(false),
			m_terrain(mgr->getFieldTable()), m_units(mgr->m_units),
			m_width(mgr->m_terrain->getWidth()), m_height(mgr->m_terrain->getHeight())
		{
			Set8DirectionTable();
		}

		void PathFinder::Set8DirectionTable()
		{
			Set4DirectionTable();

			AddExpansionDirection(-1, -1, Math::Root2);
			AddExpansionDirection( 1,  1, Math::Root2);
			AddExpansionDirection(-1,  1, Math::Root2);
			AddExpansionDirection( 1, -1, Math::Root2);
		}

		void PathFinder::Set4DirectionTable()
		{
			ResetExpansionDirections();

			AddExpansionDirection( 0, -1, 1.0f);
			AddExpansionDirection( 0,  1, 1.0f);
			AddExpansionDirection(-1,  0, 1.0f);
			AddExpansionDirection( 1,  0, 1.0f);
		}
		void PathFinder::Reset()
		{
			m_queue.Clear();
			
			m_inQueueTable.Clear();
			m_passedTable.Clear();
			m_result.Clear();
		}

		void PathFinder::Continue()
		{
			m_result.Clear();
		}

		PathFinderResult* PathFinder::FindPath(int sx, int sy, int tx, int ty)
		{
			if (sx == tx && sy == ty)
			{
				FastList<Point> emptyList;
				return new PathFinderResult(emptyList, false);
			}

			int32 maxStep = MaxStep;
			if (maxStep == -1) maxStep = 0x7fffffff;

			const bool considerFieldWeightCost = ConsiderFieldWeightCost;
			const bool considerFieldDifferencialWeightCost = ConsiderFieldDifferencialWeightCost;

			//int ofsX = min(sx, tx);
			//int ofxY = min(sy, ty);

			//FastList<AStarNode*> enQueueBuffer(10);

			AStarNode* startNode = getNode(sx, sy);
			startNode->parent = 0;
			startNode->h = 0;
			startNode->g = 0;
			startNode->f = 0;
			startNode->depth = 0;

			m_queue.Enqueue(startNode);
			m_inQueueTable.Add(startNode->GetHashCode(), startNode);

			bool found = false;
			bool rcpf = false;

			AStarNode* finalNode = 0;

			while (m_queue.getCount()>0 && !(found || rcpf))
			{
				AStarNode* curPos = m_queue.Dequeue();

				int curHash = curPos->GetHashCode();

				if (curPos->depth > maxStep)
				{
					rcpf = true;
					finalNode = curPos;
					break;
				}

				m_inQueueTable.Remove(curHash);
				m_passedTable.Add(curHash, curPos);

				int cx = curPos->X;
				int cy = curPos->Y;

				int lastDx = 0;
				int lastDy = 0;
				if (curPos->parent)
				{
					lastDx = curPos->X - curPos->parent->X;
					lastDy = curPos->Y - curPos->parent->Y;
				}

				// bfs expand new nodes
				for (int i=0;i<m_pathExpansionEnum.getCount();i++)
				{
					const ExpansionDirection& ed = m_pathExpansionEnum[i];
					int nx = cx + ed.dx;
					int ny = cy + ed.dy;
					float cost = ed.cost;

					if (TurnCost != 1.0f &&
						(ed.dx != lastDx || ed.dy != lastDy) && (lastDx || lastDy))
					{
						cost *= TurnCost;
					}

					if (nx >= 0 && nx < m_width && ny >= 0 && ny<m_height)
					{
						AStarNode* np = getNode(nx, ny);
						int npHash = np->GetHashCode();

						if (nx == tx && ny == ty) // if this next coord equals to the destination, then we found it
						{
							found = true;
							finalNode = np;

							np->depth = curPos->depth + 1;
							np->parent = curPos;
							break;
						}
						else
						{
							if (m_terrain->isPassable(nx, ny))
							{
								if (considerFieldWeightCost)
								{
									float fldWeight = m_terrain->getFieldWeight(nx, ny);
									
									if (m_neighborCosts.getCount()>0)
									{
										for (int32 j=0;j<m_neighborCosts.getCount();j++)
										{
											int samx = nx + m_neighborCosts[j].dx;
											int samy = ny + m_neighborCosts[j].dy;
											
											if (samx >= 0 && samx < m_width && samy >= 0 && samy<m_height)
											{
												fldWeight += m_terrain->getFieldWeight(samx, samy);
											}
										}
										fldWeight /= m_neighborCosts.getCount()+1;
									}

									cost *= fldWeight;
								}

								if (considerFieldDifferencialWeightCost)
									cost *= m_terrain->calculateDifferencialWeight(cx, cy, nx, ny);

								bool isNPInQueue = false;
								AStarNode* temp;
								if (m_inQueueTable.TryGetValue(npHash, temp) && temp == np)
								{
									if (np->g > curPos->g + cost)
									{
										np->g = curPos->g + cost;
										np->f = np->g + np->h;

										int32 indexInQueue = m_queue.getInternalList().IndexOf(np);
										if (indexInQueue != -1)
										{
											m_queue.UpdatePriorityForChange(indexInQueue);
										}
									}
									isNPInQueue = true;
								}

								if (!isNPInQueue &&
									(!m_passedTable.TryGetValue(npHash, temp) && temp != np))
									// if the node is neither in m_inQueueTable or m_passedTable
								{
									np->parent = curPos;

									np->g = curPos->g + cost;

									if (UseManhattanDistance)
									{
										np->h = (float)(abs(tx-nx) + abs(ty-ny));
									}
									else
									{
										float _dx = (float)tx-nx;
										float _dy = (float)ty-ny;

										np->h = sqrtf(_dx*_dx + _dy*_dy);
									}
									
									np->f = np->g + np->h;
									np->depth = curPos->depth + 1;

									//enQueueBuffer.Add(np);
									m_inQueueTable.Add(npHash, np);
									m_queue.Enqueue(np);
								}
							}
						}
					}
				}

				//if (enQueueBuffer.getCount())
				//{
				//	QuickSort(enQueueBuffer,0,enQueueBuffer.getCount()-1);
				//	for (int i=0;i<enQueueBuffer.getCount();i++)
				//	{
				//		m_queue.Enqueue(enQueueBuffer[i]);
				//	}
				//	enQueueBuffer.Clear();
				//}
			}

			if (rcpf)
			{
				AStarNode* curNode = finalNode;
				int baseOffset = m_result.getCount();
				for (int i=0;i<curNode->depth+1;i++)
				{
					m_result.Add(Point::Zero);
				}
				do 
				{
					m_result[baseOffset+curNode->depth] = Point(curNode->X, curNode->Y);
					curNode = curNode->parent;
				} while (curNode->parent);

				m_result[0] = Point(sx,sy);
				return new PathFinderResult(m_result, true);
			}
			if (found)
			{
				AStarNode* curNode = finalNode;
				for (int i=0;i<curNode->depth+1;i++)
				{
					m_result.Add(Point::Zero);
				}
				do 
				{
					m_result[curNode->depth] = Point(curNode->X, curNode->Y);
					curNode = curNode->parent;
				} while (curNode->parent);

				m_result[0] = Point(sx,sy);
				return new PathFinderResult(m_result, true);
			}
			return 0;
		}

		void PathFinder::AddExpansionDirection(int32 dx, int32 dy, float cost)
		{
			ExpansionDirection dir;
			dir.dx = dx;
			dir.dy = dy;
			dir.cost = cost;

			m_pathExpansionEnum.Add(dir);
		}

		void PathFinder::AddNeighborCostInclusion(int32 dx, int32 dy)
		{
			NeighorCostInclusion nci;
			nci.dx = dx;
			nci.dy = dy;
			//nci.weight = weight;

			m_neighborCosts.Add(nci);
		}

		/************************************************************************/
		/*   PathFinderField                                                    */
		/************************************************************************/

		PathFinderField::PathFinderField(int32 w, int32 h)
			: m_width(w), m_height(h)
		{
			m_fieldPassable = new bool[w * h];
			m_fieldWeight = new float[w * h];
			m_fieldDifferencialWeight = new float[w * h];

			for (int32 i=0;i<h;i++)
			{
				for (int32 j=0;j<j;j++)
				{
					setPassable(j,i, true);
					setFieldWeight(j,i, 1.0f);
					setDifferencialFieldWeight(j,i,0.0f);
				}
			}
		}

		PathFinderField::~PathFinderField()
		{
			delete[] m_fieldPassable;
			delete[] m_fieldWeight;
			delete[] m_fieldDifferencialWeight;
		}
	}
}