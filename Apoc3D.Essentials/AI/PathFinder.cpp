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
			m_units = new AStarNode**[fld->getWidth()];
			for (int i=0;i<fld->getWidth();i++)
			{
				m_units[i] = new AStarNode*[fld->getHeight()];
				for (int j=0;j<fld->getHeight();j++)
				{
					m_units[i][j] = new AStarNode(i,j);
				}
			}
		}

		PathFinderManager::~PathFinderManager()
		{
			for (int i=0;i<m_terrain->getWidth();i++)
			{
				for (int j=0;j<m_terrain->getHeight();j++)
				{
					delete m_units[i][j];
				}
				delete[] m_units[i];
			}
			delete[] m_units;
		}

		PathFinder* PathFinderManager::CreatePathFinder()
		{
			return new PathFinder(m_terrain, m_units);
		}

		/************************************************************************/
		/*                                                                      */
		/************************************************************************/

		int PathFinder::stateEnum[8][2] = 
		{
			{ 0, -1 }, { 0, 1 },
			{ -1, 0 }, { 1, 0 },
			{ -1, -1 }, { 1, 1 },
			{ -1, 1 }, { 1, -1 },
		};

		float PathFinder::stateEnumCost[8] =
		{
			1,1,
			1,1,
			Math::Root2, Math::Root2,
			Math::Root2, Math::Root2,
		};


		PathFinder::PathFinder(PathFinderField* terrain, AStarNode*** units)
			: MaxStep(50), m_terrain(terrain), m_units(units), 
			m_width(terrain->getWidth()), m_height(terrain->getHeight())
		{

		}

		PathFinder::PathFinder(PathFinderManager* mgr)
			: MaxStep(50), m_terrain(mgr->getFieldTable()), m_units(mgr->m_units),
			m_width(mgr->m_terrain->getWidth()), m_height(mgr->m_terrain->getHeight())
		{

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

			//int ofsX = min(sx, tx);
			//int ofxY = min(sy, ty);

			FastList<AStarNode*> enQueueBuffer(10);

			AStarNode* startNode = m_units[sx][sy];
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

				if (curPos->depth > MaxStep)
				{
					rcpf = true;
					finalNode = curPos;
					break;
				}

				m_inQueueTable.Remove(curHash);
				m_passedTable.Add(curHash, curPos);

				int cx = curPos->X;
				int cy = curPos->Y;

				// bfs expand new nodes
				for (int i=0;i<8;i++)
				{
					int nx = cx + stateEnum[i][0];
					int ny = cy + stateEnum[i][1];

					if (nx >= 0 && nx < m_width && ny >= 0 && ny<m_height)
					{
						AStarNode* np = m_units[nx][ny];
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
							if (!m_terrain->IsPassable(nx, ny))
							{
								bool isNPInQueue = false;
								AStarNode* temp;
								if (m_inQueueTable.TryGetValue(npHash, temp) && temp == np)
								{
									if (np->g > curPos->g + stateEnumCost[i])
									{
										np->g = curPos->g + stateEnumCost[i];
										np->f = np->g + np->h;
									}
									isNPInQueue = true;
								}

								if (!isNPInQueue &&
									(!m_passedTable.TryGetValue(npHash, temp) && temp != np))
									// if the node is neither in m_inQueueTable or m_passedTable
								{
									np->parent = curPos;

									np->g = curPos->g + stateEnumCost[i];
									np->h = (float)abs(tx-nx) + (float)abs(ty-ny);
									np->f = np->g + np->h;
									np->depth = curPos->depth + 1;

									enQueueBuffer.Add(np);
									m_inQueueTable.Add(npHash, np);
								}
							}
						}
					}
				}

				if (enQueueBuffer.getCount())
				{
					QuickSort(enQueueBuffer,0,enQueueBuffer.getCount()-1);
					for (int i=0;i<enQueueBuffer.getCount();i++)
					{
						m_queue.Enqueue(enQueueBuffer[i]);
					}
					enQueueBuffer.Clear();
				}
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

		void PathFinder::QuickSort(FastList<AStarNode*>& list, int left, int right)
		{
			int i = left, j = right;
			const AStarNode* pivot = list[(left + right) / 2];

			while (i<=j)
			{
				while (list[i]->f < pivot->f)
					i++;
				while (list[j]->f > pivot->f)
					j--;

				if (i<=j)
				{
					AStarNode* tmp = list[i];
					list[i] = list[j];
					list[j] = tmp;
					i++; j--;
				}
			}

			if (left < j) QuickSort(list, left, j);
			if (i < right) QuickSort(list, i, right);
		}

		/************************************************************************/
		/*                                                                      */
		/************************************************************************/

		PathFinderField::PathFinderField(int w, int h)
			: m_width(w), m_height(h)
		{
			m_passTable = new bool*[w];
			for (int i=0;i<w;i++)
			{
				m_passTable[i] = new bool[h];
				for (int j=0;j<h;j++)
				{
					m_passTable[i][j] = true;
				}
			}
		}

		PathFinderField::~PathFinderField()
		{
			for (int i=0;i<m_width;i++)
			{
				delete[] m_passTable[i];
			}
			delete[] m_passTable;
		}
	}
}