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
#ifndef PATHFINDER_H
#define PATHFINDER_H


#include "../EssentialCommon.h"
#include "apoc3d/Collections/List.h"
#include "apoc3d/Collections/HashMap.h"
#include "apoc3d/Collections/Queue.h"
#include "apoc3d/Math/Point.h"

using namespace Apoc3D;
using namespace Apoc3D::Core;
using namespace Apoc3D::Collections;
using namespace Apoc3D::Math;

namespace Apoc3DEx
{
	namespace AI
	{
		class AStarNode;
		class PathFinderField;
		class PathFinder;
		class PathFinderResult;
	
		class APEXAPI PathFinderManager
		{
		public:
			PathFinderManager(PathFinderField* fld);
			~PathFinderManager();

			PathFinder* CreatePathFinder();
			PathFinderField* getFieldTable() const { return m_terrain; }
		private:
			AStarNode*** m_units;
			PathFinderField* m_terrain;

			friend class PathFinder;
		};

		class APEXAPI PathFinder
		{
		public:
			int MaxStep;

			PathFinder(PathFinderManager* mgr);
			PathFinder(PathFinderField* terrain, AStarNode*** units);

			void Reset();
			void Continue();
			PathFinderResult* FindPath(int sx, int sy, int tx, int ty);
		private:
			AStarNode*** m_units;
			PathFinderField* m_terrain;

			/** bfs search queue
			*/
			Queue<AStarNode*> m_queue;
			
			/** used to fast check if a node is in the queue
			*/
			HashMap<int, AStarNode*> m_inQueueTable;
			
			/** used to fast check if a node is already passed by the algorithm
			*/
			HashMap<int, AStarNode*> m_passedTable;

			int m_width;
			int m_height;

			FastList<Point> m_result;

			static int stateEnum[8][2];
			static float stateEnumCost[];

			void QuickSort(FastList<AStarNode*>& list, int l, int r);
		};

		class APEXAPI PathFinderField
		{
		public:
			PathFinderField(int w, int h);
			~PathFinderField();

			bool IsPassable(int x, int y) const { return m_passTable[x][y]; }
			int getWidth() { return m_width; }
			int getHeight() { return m_height; }

			bool& Passable(int x, int y) { return m_passTable[x][y]; }
		private:
			int m_width;
			int m_height;

			bool** m_passTable;
		};

		class APEXAPI PathFinderResult
		{
		public:

			PathFinderResult(const FastList<Point>& path, bool rcpf)
				: m_path(path), m_requiresContinuePathFinding(rcpf)
			{
			}

			const Point& operator [](int idx) const
			{
				return m_path[idx];
			}
			int getNodeCount() const { return m_path.getCount(); }

			bool RequiresContinePathFinding() const { return m_requiresContinuePathFinding; }
		private:
			bool m_requiresContinuePathFinding;
			FastList<Point> m_path;
		};

		class APEXAPI AStarNode
		{
		public:
			int X;
			int Y;

			float f;
			float g;
			float h;

			int depth;

			AStarNode* parent;

			AStarNode(int x, int y)
				: X(x), Y(y), f(0), g(0), h(0), depth(0), parent(0)
			{

			}

			int GetHashCode() const { return (X<<16) | Y; }
		};

	}
}
#endif