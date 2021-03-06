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

#include "VolumePathFinder.h"
#include "apoc3d/Math/Math.h"

namespace Apoc3D
{
	namespace AI
	{
		VolumePathFinderManager::VolumePathFinderManager(VolumePathFinderField* fld, int maxWidth, int maxHeight, int maxZ)
			: m_terrain(fld), m_maxWidth(maxWidth), m_maxHeight(maxHeight), m_maxZ(maxZ)
		{
			m_units = new AStarNode3***[maxZ];
			for (int i=0;i<maxZ;i++)
			{
				m_units[i] = new AStarNode3**[maxHeight];
				for (int j=0;j<maxHeight;j++)
				{
					m_units[i][j] = new AStarNode3*[maxWidth];
					for (int k=0;k<maxWidth;k++)
					{
						m_units[i][j][k] = new AStarNode3(k,j,i);
					}
				}
			}
		}

		VolumePathFinderManager::~VolumePathFinderManager()
		{
			for (int i=0;i<m_maxZ;i++)
			{
				for (int j=0;j<m_maxHeight;j++)
				{
					for (int k=0;k<m_maxWidth;k++)
					{
						delete m_units[i][j][k];
					}
					delete[] m_units[i][j];
				}
				delete[] m_units[i];
			}
			delete[] m_units;
		}

		VolumePathFinder* VolumePathFinderManager::CreatePathFinder() const
		{
			return new VolumePathFinder(m_terrain, m_units);
		}

		/************************************************************************/
		/*                                                                      */
		/************************************************************************/

		int VolumePathFinder::stateEnum[8][2] = 
		{
			{ 0, -1 }, { 0, 1 },
			{ -1, 0 }, { 1, 0 },
			{ -1, -1 }, { 1, 1 },
			{ -1, 1 }, { 1, -1 },
		};

		float VolumePathFinder::stateEnumCost[8] =
		{
			1,1,
			1,1,
			Math::Root2, Math::Root2,
			Math::Root2, Math::Root2,
		};


		VolumePathFinder::VolumePathFinder(VolumePathFinderField* terrain, AStarNode3**** units)
			: MaxStep(50), m_terrain(terrain), m_units(units)
		{

		}

		VolumePathFinder::VolumePathFinder(VolumePathFinderManager* mgr)
			: MaxStep(50), m_terrain(mgr->getFieldTable()), m_units(mgr->m_units)
		{

		}

		void VolumePathFinder::Reset()
		{
			m_queue.Clear();
			m_inQueueTable.Clear();
			m_passedTable.Clear();
			m_result.Clear();
		}

		void VolumePathFinder::Continue()
		{
			m_result.Clear();
		}

		VolumePathFinderResult* VolumePathFinder::FindPath(int sx, int sy, int sz, int tx, int ty, int tz)
		{
			if (sx == tx && sy == ty)
			{
				List<VolumePathFinderResultPoint> emptyList;
				return new VolumePathFinderResult(emptyList, false);
			}

			//int ofsX = min(sx, tx);
			//int ofxY = min(sy, ty);

			List<AStarNode3*> enQueueBuffer(10);

			AStarNode3* startNode = m_units[sx][sy][sz];
			startNode->parent = 0;
			startNode->h = 0;
			startNode->g = 0;
			startNode->f = 0;
			startNode->depth = 0;

			m_queue.Enqueue(startNode);
			m_inQueueTable.Add(startNode->GetHashCode(), startNode);

			bool found = false;
			bool rcpf = false;

			AStarNode3* finalNode = 0;

			while (m_queue.getCount()>0 && !(found || rcpf))
			{
				AStarNode3* curPos = m_queue.Dequeue();
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
				int cz = curPos->Z;


				// bfs expand new nodes
				{
					const List<PathFinderLevelPortal>& portals = m_terrain->GetPortals(cx,cy,cz);
					for (int i=0;i<portals.getCount();i++)
					{
						int nx = portals[i].TargetX;
						int ny = portals[i].TargetY;
						int nz = portals[i].TargetZ;

						// cost is stateEnumCost[0]
						// expand

						if (ExpandNode(nx,ny,nz,tx,ty,tz, stateEnumCost[0], curPos, finalNode, enQueueBuffer))
						{
							found = true;
							break;
						}
					}
				}

				// expand at the same level
				for (int i=0;i<8;i++)
				{
					int nx = cx + stateEnum[i][0];
					int ny = cy + stateEnum[i][1];
					int nz = cz;

					if (ExpandNode(nx,ny,nz,tx,ty,tz, stateEnumCost[i], curPos, finalNode, enQueueBuffer))
					{
						found = true;
						break;
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
				AStarNode3* curNode = finalNode;
				int baseOffset = m_result.getCount();
				for (int i=0;i<curNode->depth+1;i++)
				{
					m_result.Add(VolumePathFinderResultPoint());
				}
				do 
				{
					m_result[baseOffset+curNode->depth] = VolumePathFinderResultPoint(curNode->X, curNode->Y, curNode->Z);
					curNode = curNode->parent;
				} while (curNode->parent);

				m_result[0] = VolumePathFinderResultPoint(sx,sy,sz);
				return new VolumePathFinderResult(m_result, true);
			}
			if (found)
			{
				AStarNode3* curNode = finalNode;
				for (int i=0;i<curNode->depth+1;i++)
				{
					m_result.Add(VolumePathFinderResultPoint());
				}
				do 
				{
					m_result[curNode->depth] = VolumePathFinderResultPoint(curNode->X, curNode->Y, curNode->Z);
					curNode = curNode->parent;
				} while (curNode->parent);

				m_result[0] = VolumePathFinderResultPoint(sx,sy, sz);
				return new VolumePathFinderResult(m_result, true);
			}
			return 0;
		}
		bool VolumePathFinder::ExpandNode(int nx, int ny, int nz, int tx, int ty, int tz, 
			float cost, AStarNode3* curPos, AStarNode3*& finalNode, List<AStarNode3*>& enQueueBuffer)
		{
			if (m_terrain->IsInBound(nx, ny, nz))
			{
				AStarNode3* np = m_units[nx][ny][nz];
				int npHash = np->GetHashCode();

				if (nx == tx && ny == ty && nz == tz) // if this next coord equals to the destination, then we found it
				{

					finalNode = np;

					np->depth = curPos->depth + 1;
					np->parent = curPos;
					return true;
				}
				
				
				if (!m_terrain->Passable(nx, ny, nz))
				{
					bool isNPInQueue = false;
					AStarNode3* temp;
					if (m_inQueueTable.TryGetValue(npHash, temp) && temp == np)
					{
						if (np->g > curPos->g + cost)//stateEnumCost[i])
						{
							np->g = curPos->g + cost;//stateEnumCost[i];
							np->f = np->g + np->h;
						}
						isNPInQueue = true;
					}

					if (!isNPInQueue &&
						(!m_passedTable.TryGetValue(npHash, temp) && temp != np))
						// if the node is neither in m_inQueueTable or m_passedTable
					{
						np->parent = curPos;

						np->g = curPos->g + cost;//stateEnumCost[i];
						np->h = (float)abs(tx-nx) + (float)abs(ty-ny);
						np->f = np->g + np->h;
						np->depth = curPos->depth + 1;

						enQueueBuffer.Add(np);
						m_inQueueTable.Add(npHash, np);
					}
				}
			}
			return false;
		}
		void VolumePathFinder::QuickSort(List<AStarNode3*>& list, int l, int r)
		{
			int i;
			int j;
			do 
			{
				i = l;
				j = r;
				AStarNode3* p = list[(l+r) >> 1];

				do 
				{
					while (list[i]->f < p->f)
						i++;
					while (list[i]->f > p->f)
						j++;

					if (i<=j)
					{
						AStarNode3* t = list[i];
						list[i] = list[j];
						list[j] = t;
						i++;
						j--;
					}

				} while (i<=j);
				if (l<j)
					QuickSort(list,l,j);
				l = i;
			} while (i<r);
		}

	}
}