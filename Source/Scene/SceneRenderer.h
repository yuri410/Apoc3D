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
using namespace Apoc3D::Config;
using namespace Apoc3D::Graphics;
using namespace Apoc3D::Graphics::RenderSystem;
using namespace Apoc3D::VFS;

using namespace std;

namespace Apoc3D
{
	namespace Scene
	{
		typedef FastList<RenderOperation> OperationList;
		typedef FastMap<GeometryData*, OperationList*> GeometryTable;
		typedef FastMap<Material*, GeometryTable*> MaterialTable;
		typedef FastMap<uint32, MaterialTable*> PriorityTable;

		/** A hirerachy of tables to store classified render operations.
		 *  The classification is done by hashing, similar render op will be 
		 *  grouped together.
		 */
		class APAPI BatchData
		{
		public:
			//static const int MaxPriority = 10;

		public:

			BatchData() : m_objectCount(0) { }

			/** Adds an object's render operations into the internal table.
			*/
			void AddVisisbleObject(SceneObject* obj, int level);

			/** Check if any materials with passFlag corresponding to selectMask are already added
			 *  into the table.
			 */
			bool HasObject(uint64 selectMask);

			/** Clears all the render operations added.
			*/
			void Clear();

			/** Gets a reference to the internal table
			*/
			const PriorityTable& getTable() const { return m_priTable; }

			/** Gets the total number of objects added since last Clear
			*/
			int getObjectCount() const { return m_objectCount; }

		private:
			PriorityTable m_priTable;
			//MaterialList m_mtrlList;			

			int m_objectCount;

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

			void Load(Configuration* config);
			void Load(const String& configName);

			void RegisterCamera(Camera* camera)
			{
				m_cameraList.Add(camera);
			}
			void UnregisterCamera(Camera* camera)
			{
				m_cameraList.Remove(camera);
			}

			void RenderScene(SceneManager* sceMgr);

			/** Renders the current batch produced by scene pass
			*/
			void RenderBatch(int selectorID);

			int getSelectedProcID() const { return m_selectedProc; }
			SceneProcedure* getSelectedProc() const
			{
				if (m_selectedProc!=-1)
					return m_procFallbacks[m_selectedProc];
				return 0;
			}
		};
	};
	namespace Collections
	{
		typedef Material* LPMaterial;
		class MaterialEqualityComparer : public IEqualityComparer<LPMaterial>
		{
		private:
		public:
			class BuiltIn : public IBuiltInEqualityComparer<LPMaterial>
			{

			};

			virtual bool Equals(const LPMaterial& x, const LPMaterial& y) const;

			virtual int64 GetHashCode(const LPMaterial& obj) const;
		};

		typedef GeometryData* LPGeometryData;
		class GeometryDataEqualityComparer : public IEqualityComparer<LPGeometryData>
		{
		private:

		public:
			class BuiltIn : public IBuiltInEqualityComparer<LPGeometryData>
			{

			};

			virtual bool Equals(const LPGeometryData& x, const LPGeometryData& y) const;

			virtual int64 GetHashCode(const LPGeometryData& obj) const;
		};
	}
};
#endif