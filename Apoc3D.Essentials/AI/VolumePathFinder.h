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

#ifndef VOLUMEPATHFINDER_H
#define VOLUMEPATHFINDER_H


#include "../EssentialCommon.h"

#include "apoc3d/Collections/List.h"
#include "apoc3d/Collections/HashMap.h"
#include "apoc3d/Collections/Queue.h"
#include "apoc3d/Math/Point.h"
#include "apoc3d/Math/Vector.h"

using namespace Apoc3D;
using namespace Apoc3D::Core;
using namespace Apoc3D::Collections;
using namespace Apoc3D::Math;

namespace Apoc3DEx
{
	namespace AI
	{
		class AStarNode3;
		class VolumePathFinderResult;
		class VolumePathFinder;
		class VolumePathFinderField;

		struct APEXAPI VolumePathFinderResultPoint
		{
			int X;
			int Y;
			int Z;

			VolumePathFinderResultPoint() : X(0), Y(0), Z(0) {}
			VolumePathFinderResultPoint(int _x, int _y, int _z) : X(_x), Y(_y), Z(_z) { }
		};
		struct APEXAPI PathFinderLevelPortal
		{
			int TargetZ;
			int TargetX;
			int TargetY;
		};

		class APEXAPI VolumePathFinderManager
		{
		public:
			VolumePathFinderManager(VolumePathFinderField* fld, int maxWidth, int maxHeight, int maxZ);
			~VolumePathFinderManager();

			VolumePathFinder* CreatePathFinder() const;
			VolumePathFinderField* getFieldTable() const { return m_terrain; }
		private:
			AStarNode3**** m_units;
			VolumePathFinderField* m_terrain;

			int m_maxWidth;
			int m_maxHeight;
			int m_maxZ;

			friend class VolumePathFinder;
		};

		class APEXAPI VolumePathFinder
		{
		public:
			int MaxStep;

			VolumePathFinder(VolumePathFinderManager* mgr);
			VolumePathFinder(VolumePathFinderField* terrain, AStarNode3**** units);

			void Reset();
			void Continue();
			VolumePathFinderResult* FindPath(int sx, int sy, int sz, int tx, int ty, int tz);
		private:
			AStarNode3**** m_units;
			VolumePathFinderField* m_terrain;

			/** bfs search queue
			*/
			Queue<AStarNode3*> m_queue;
			
			/** used to fast check if a node is in the queue
			*/
			HashMap<int, AStarNode3*> m_inQueueTable;
			
			/** used to fast check if a node is already passed by the algorithm
			*/
			HashMap<int, AStarNode3*> m_passedTable;


			List<VolumePathFinderResultPoint> m_result;

			static int stateEnum[8][2];
			static float stateEnumCost[];

			bool ExpandNode(int nx, int ny, int nz, int tx, int ty, int tz, 
				float cost, AStarNode3* curPos, AStarNode3*& finalNode, List<AStarNode3*>& enQueueBuffer);
			void QuickSort(List<AStarNode3*>& list, int l, int r);
		};

		class APEXAPI VolumePathFinderField
		{
		public:
			virtual bool Passable(int x, int y, int z) = 0;;
			virtual const List<PathFinderLevelPortal>& GetPortals(int x, int y, int z) = 0;
			virtual bool IsInBound(int x, int y, int z) = 0;
		};

	

		class APEXAPI VolumePathFinderResult
		{
		public:

			VolumePathFinderResult(const List<VolumePathFinderResultPoint>& path, bool rcpf)
				: m_path(path), m_requiresContinuePathFinding(rcpf)
			{
			}

			const VolumePathFinderResultPoint& operator [](int idx) const
			{
				return m_path[idx];
			}
			int getNodeCount() const { return m_path.getCount(); }

			bool RequiresContinePathFinding() const { return m_requiresContinuePathFinding; }
		private:
			bool m_requiresContinuePathFinding;
			List<VolumePathFinderResultPoint> m_path;
		};

		class APEXAPI AStarNode3
		{
		public:
			int X;
			int Y;
			int Z;


			float f;
			float g;
			float h;

			int depth;

			AStarNode3* parent;

			AStarNode3(int x, int y, int z)
				: X(x), Y(y), Z(z), f(0), g(0), h(0), depth(0), parent(0)
			{

			}

			int GetHashCode() const { return (X<<20)|(Y<<10) | Z; }
		};

	}
}
#endif