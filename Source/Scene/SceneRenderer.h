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
#ifndef SCENERENDERER_H
#define SCENERENDERER_H

#include "Common.h"
#include "Collections/FastList.h"
#include "Collections/FastMap.h"

using namespace Apoc3D::Collections;
using namespace Apoc3D::Graphics;
using namespace Apoc3D::Graphics::RenderSystem;
using namespace Apoc3D::VFS;

using namespace std;

namespace Apoc3D
{
	namespace Collections
	{
		typedef Material* LPMaterial;
		class MaterialEqualityComparer : public IEqualityComparer<LPMaterial>
		{
		public:
			virtual bool Equals(const LPMaterial& x, const LPMaterial& y) const;

			virtual int64 GetHashCode(const LPMaterial& obj) const;
		};

		typedef GeometryData* LPGeometryData;
		class GeometryDataEqualityComparer : public IEqualityComparer<LPGeometryData>
		{
		public:
			virtual bool Equals(const LPGeometryData& x, const LPGeometryData& y) const;

			virtual int64 GetHashCode(const LPGeometryData& obj) const;
		};
	}
	namespace Scene
	{
		typedef FastList<RenderOperation> OperationList;
		//class GeometryTable;
		//class MaterialTable;
		//class PriorityTable;
		//class MaterialList;;

		typedef FastMap<GeometryData*, OperationList*> GeometryTable;
		typedef FastMap<Material*, GeometryTable*> MaterialTable;
		typedef FastMap<uint32, MaterialTable*> PriorityTable;


		class APAPI BatchData
		{
		public:
			static const int MaxPriority = 10;

		private:
			PriorityTable m_priTable;
			//MaterialList m_mtrlList;			

			int m_objectCount;

		public:
			const PriorityTable& getTable() const { return m_priTable; }

			BatchData() : m_objectCount(0) { }

			int getObjectCount() const { return m_objectCount; }

			void AddVisisbleObject(SceneObject* obj, int level);

			bool HasObject(uint64 selectMask);

			void Clear();
		};

		/* Renders a scene with a particular render script.
		*/
		class APAPI SceneRenderer
		{
		private:
			RenderDevice* m_renderDevice;
			//FastList<ScenePass*> m_passes;
			BatchData m_batchData;

			FastList<SceneProcedure*> m_procFallbacks;
			int m_selectedProc;

			FastList<Camera*> m_cameraList;

		public:
			SceneRenderer(RenderDevice* dev);
			~SceneRenderer(void);

			void Load(const String& configName);

			void RenderScene(SceneManager* sceMgr);

			/** Renders the current batch produced by scene pass
			*/
			void RenderBatch(int selectorID);
		};
	};
};
#endif