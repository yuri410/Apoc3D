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

using namespace Apoc3D::Collections;
using namespace Apoc3D::Graphics;
using namespace Apoc3D::Graphics::RenderSystem;
using namespace Apoc3D::VFS;

using namespace std;

namespace Apoc3D
{
	namespace Scene
	{
		

		typedef FastList<RenderOperation> OperationList;
		typedef FastList<RenderOperation> OperationList;
		typedef FastList<RenderOperation> OperationList;
		typedef FastList<RenderOperation> OperationList;
		typedef FastList<RenderOperation> OperationList;
		class GeometryTable;
		class MaterialTable;
		class PriorityTable;
		class MaterialList;

		//template class APAPI unordered_map<BatchHandle, OperationList*>;
		//template class APAPI unordered_map<BatchHandle, GeometryTable*>;
		//template class APAPI unordered_map<uint32, MaterialTable*>;
		//template class APAPI unordered_map<BatchHandle, Material*>;

		class GeometryTable : public unordered_map<BatchHandle, OperationList*> { };
		class MaterialTable : public unordered_map<BatchHandle, GeometryTable*> { };
		class PriorityTable : public unordered_map<uint32, MaterialTable*> { };

		class MaterialList : public unordered_map<BatchHandle, Material*> { };

		class APAPI BatchData
		{
		public:
			static const int MaxPriority = 10;

		private:
			PriorityTable m_priTable;
			MaterialList m_mtrlList;			

			int m_objectCount;

		public:
			BatchData() { }

			int getObjectCount() const { return m_objectCount; }

			void AddVisisbleObject(SceneObject* obj, int level);

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
			void RenderBatch();
		};
	};
};
#endif