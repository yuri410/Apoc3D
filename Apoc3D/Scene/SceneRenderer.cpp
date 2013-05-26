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

#include "SceneRenderer.h"

#include "SceneRenderScriptParser.h"
#include "SceneProcedure.h"
#include "SceneObject.h"
#include "ScenePass.h"

#include "apoc3d/Core/Logging.h"

#include "apoc3d/Config/Configuration.h"
#include "apoc3d/Config/ConfigurationManager.h"
#include "apoc3d/Config/ConfigurationSection.h"

#include "apoc3d/Graphics/RenderSystem/RenderDevice.h"
#include "apoc3d/Graphics/EffectSystem/EffectParameter.h"
#include "apoc3d/Graphics/RenderOperationBuffer.h"
#include "apoc3d/Graphics/RenderOperation.h"
#include "apoc3d/Graphics/Material.h"
#include "apoc3d/Graphics/GeometryData.h"
#include "apoc3d/Core/GameTime.h"

#include "apoc3d/Vfs/FileSystem.h"
#include "apoc3d/Vfs/FileLocateRule.h"
#include "apoc3d/Vfs/ResourceLocation.h"


using namespace Apoc3D::Config;
using namespace Apoc3D::VFS;

namespace Apoc3D
{
	namespace Scene
	{
		BatchData::BatchData()
			: m_objectCount(0), m_priTable(5, IBuiltInEqualityComparer<uint>::Default)
		{
			for (int i=0;i<MaxPriority;i++)
			{
				MaterialTable* mtrlTable;
				if (!m_priTable.TryGetValue(i, mtrlTable))
				{
					mtrlTable = new MaterialTable();
					m_priTable.Add(i,mtrlTable);
				}
			}
			
		}

		void BatchData::AddVisisbleObject(SceneObject* obj, int level)
		{
			m_objectCount++;
			const RenderOperationBuffer* buffer = obj->GetRenderOperation(level);

			if (buffer)
			{
				// add rops one by one
				for (int k=0;k<buffer->getCount();k++)
				{
					RenderOperation op = buffer->get(k);

					Material* mtrl = op.Material;
					GeometryData* geoData = op.GeometryData;

					// rop with no material or geo is not eligible
					if (mtrl && geoData)
					{
						if (!op.RootTransformIsFinal)
						{
							Matrix temp;
							Matrix::Multiply(temp, op.RootTransform, obj->getTrasformation());
							op.RootTransform = temp;
						}
						
						uint priority = std::min<uint>(mtrl->getPriority(), MaxPriority);

						// add the rop from outer table to inner table(top down)
						MaterialTable* mtrlTable;
						if (!m_priTable.TryGetValue(priority, mtrlTable))
						{
							mtrlTable = new MaterialTable();
							m_priTable.Add(priority,mtrlTable);
						}

						GeometryTable* geoTable;
						if (!mtrlTable->TryGetValue(mtrl, geoTable))
						{
							geoTable = new GeometryTable();
							mtrlTable->Add(mtrl, geoTable);
						}

						OperationList* opList;
						if (!geoTable->TryGetValue(geoData, opList))
						{
							opList = new OperationList();
							geoTable->Add(geoData, opList);
						}

						opList->Add(op);
					}
				}
			}
		}
		void BatchData::Clear()
		{
			m_objectCount = 0;

			// this will only clear the rop list inside. The hashtables are remained as 
			// it is highly possible the next time the buckets in them are reused.
			for (PriorityTable::Enumerator i = m_priTable.GetEnumerator();i.MoveNext();)
			{
				MaterialTable* mtrlTbl = *i.getCurrentValue();
				for (MaterialTable::Enumerator j = mtrlTbl->GetEnumerator(); j.MoveNext();)
				{
					GeometryTable* geoTbl = *j.getCurrentValue();

					for (GeometryTable::Enumerator k = geoTbl->GetEnumerator(); k.MoveNext(); )
					{
						(*(k.getCurrentValue()))->Clear();
					}
				}

			}
		}
		void BatchData::Reset()
		{
			for (PriorityTable::Enumerator i = m_priTable.GetEnumerator();i.MoveNext();)
			{
				MaterialTable* mtrlTbl = *i.getCurrentValue();
				for (MaterialTable::Enumerator j = mtrlTbl->GetEnumerator(); j.MoveNext();)
				{
					GeometryTable* geoTbl = *j.getCurrentValue();

					for (GeometryTable::Enumerator k = geoTbl->GetEnumerator(); k.MoveNext(); )
					{
						(*(k.getCurrentValue()))->Clear();
					}
					geoTbl->Clear();
				}
				mtrlTbl->Clear();
			}
		}
		bool BatchData::HasObject(uint64 selectMask)
		{
			for (PriorityTable::Enumerator i = m_priTable.GetEnumerator();i.MoveNext();)
			{
				MaterialTable* mtrlTbl = *i.getCurrentValue();
				for (MaterialTable::Enumerator j = mtrlTbl->GetEnumerator(); j.MoveNext();)
				{
					Material* m = *(j.getCurrentKey());

					if (m->getPassFlags() & selectMask)
					{
						return true;
					}
				}
			}
			return false;
		}

		/************************************************************************/
		/*                                                                      */
		/************************************************************************/
		SceneRenderer::SceneRenderer(RenderDevice* device)
			: m_renderDevice(device), GlobalCameraOverride(-1)
		{
		}


		SceneRenderer::~SceneRenderer(void)
		{
			for (int i=0;i<m_procFallbacks.getCount();i++)
			{
				delete m_procFallbacks[i];
			}
		}

		void SceneRenderer::Load(Configuration* config)
		{
			m_selectedProc = -1;

			// when loading, each script listed will be loaded
			// then the engine will do a check one them
			for (Configuration::ChildTable::Enumerator iter = config->GetEnumerator(); iter.MoveNext();)
			{
				String file = (*iter.getCurrentValue())->getAttribute(L"Script");

				// load proc
				FileLocation* fl = FileSystem::getSingleton().Locate(file, FileLocateRule::Default);
				SceneProcedure* proc = new SceneProcedure(m_renderDevice);
				proc->Load(this, static_cast<ResourceLocation*>(fl));
				delete fl;

				m_procFallbacks.Add(proc);

				
				if (m_selectedProc==-1)
				{
					// check if the current render system support this proc
					if (proc->IsAvailable())
					{
						m_selectedProc = m_procFallbacks.getCount()-1;
					}
				}
			}
		}
		void SceneRenderer::Load(const String& configName)
		{
			Configuration* config = ConfigurationManager::getSingleton().getConfiguration(configName);
			Load(config);
		}

		void SceneRenderer::RenderScene(SceneManager* sceMgr)
		{
			if (m_selectedProc !=-1)
			{
				m_procFallbacks[m_selectedProc]->Invoke(m_cameraList, sceMgr, &m_batchData);
			}
			RendererEffectParams::Reset();
		}

		void SceneRenderer::RenderBatch(int selectorID)
		{
			RenderBatch(m_renderDevice, m_batchData, selectorID);
		}

		void SceneRenderer::RenderBatch(RenderDevice* device, const BatchData& data, int selectorID)
		{
			uint64 selectorMask;
			if (selectorID == -1)
				selectorMask = 0xffffffffffffffff;
			else
				selectorMask = (uint64)1<<selectorID;

			//const PriorityTable& table = data.getTable();

			//for (PriorityTable::Enumerator i = table.GetEnumerator();i.MoveNext();)
			//{
			//	MaterialTable* mtrlTbl = *(i.getCurrentValue());
			//	for (MaterialTable::Enumerator j = mtrlTbl->GetEnumerator(); j.MoveNext();)
			//	{
			//		Material* mtrl = *j.getCurrentKey();
			//		GeometryTable* geoTbl = *(j.getCurrentValue());

			//		if (mtrl->getPassFlags() & selectorMask)
			//		{
			//			for (GeometryTable::Enumerator k = geoTbl->GetEnumerator(); k.MoveNext();)
			//			{
			//				const OperationList* opList = *k.getCurrentValue();
			//				if (opList->getCount())
			//				{
			//					device->Render(mtrl, opList->getInternalPointer(), opList->getCount(), selectorID);
			//				}
			//			}
			//		}
			//	}
			//}

			typedef LinkedList<GeometryData*> InvalidGeoPointerList;
			typedef LinkedList<Material*> InvalidMtrlPointerList;

			InvalidMtrlPointerList* invalidMtrlPointers = nullptr;
			InvalidGeoPointerList* invalidGeoPointers = nullptr;

			const PriorityTable& table = data.getTable();

			for (PriorityTable::Enumerator i = table.GetEnumerator();i.MoveNext();)
			{
				MaterialTable* mtrlTbl = *(i.getCurrentValue());
				for (MaterialTable::Enumerator j = mtrlTbl->GetEnumerator(); j.MoveNext();)
				{
					bool isMtrlUsed = false;
					GeometryTable* geoTbl = *(j.getCurrentValue());
					for (GeometryTable::Enumerator k = geoTbl->GetEnumerator(); k.MoveNext();)
					{
						GeometryData* geoData = *k.getCurrentKey();

						const OperationList* opList = *k.getCurrentValue();
						if (opList->getCount())
						{
							if (geoData->Discard)
							{
								break;
							}

							isMtrlUsed = true;
						}
						else
						{
							if (!invalidGeoPointers)
								invalidGeoPointers = new InvalidGeoPointerList();
							
							invalidGeoPointers->PushBack(geoData);
						}
					}


					if (invalidGeoPointers && invalidGeoPointers->getCount())
					{
						for (InvalidGeoPointerList::Iterator iter = invalidGeoPointers->Begin(); iter != invalidGeoPointers->End(); iter++)
						{
							GeometryData* item = *iter;
							OperationList* list = geoTbl->operator[](item);
							assert(list->getCount()==0);
							delete list;
							geoTbl->Remove(item);
						}

						invalidGeoPointers->Clear();
					}


					Material* mtrl = *j.getCurrentKey();
					if (isMtrlUsed)
					{
						if (mtrl->getPassFlags() & selectorMask)
						{
							for (GeometryTable::Enumerator k = geoTbl->GetEnumerator(); k.MoveNext();)
							{
								const OperationList* opList = *k.getCurrentValue();
								if (opList->getCount())
								{
									device->Render(mtrl, opList->getInternalPointer(), opList->getCount(), selectorID);
								}
							}
						}
					}
					else
					{
						if (!invalidMtrlPointers)
							invalidMtrlPointers = new InvalidMtrlPointerList();
						invalidMtrlPointers->PushBack(mtrl);
					}
				}

				if (invalidMtrlPointers && invalidMtrlPointers->getCount())
				{
					for (InvalidMtrlPointerList::Iterator iter = invalidMtrlPointers->Begin(); iter != invalidMtrlPointers->End(); iter++)
					{
						Material* item = *iter;
						GeometryTable* geoTbl = mtrlTbl->operator[](item);
						for (GeometryTable::Enumerator k = geoTbl->GetEnumerator(); k.MoveNext();)
						{
							OperationList* opList = *k.getCurrentValue();
							delete opList;
						}
						delete geoTbl;
						mtrlTbl->Remove(item);
					}
					invalidMtrlPointers->Clear();
				}

			}
			
			if (invalidGeoPointers)
			{
				delete invalidGeoPointers;
			}

			if (invalidMtrlPointers)
			{
				delete invalidMtrlPointers;
			}
		}

		void SceneRenderer::ResetBatchTable()
		{
			m_batchData.Reset();
		}
	};

	namespace Collections
	{
		const IEqualityComparer<LPMaterial>* MaterialEqualityComparer::BuiltIn::Default =
			new MaterialEqualityComparer();

		bool MaterialEqualityComparer::Equals(const LPMaterial& x, const LPMaterial& y) const
		{
			const void* a = x;
			const void* b = y;
			return a==b;
		}
		int64 MaterialEqualityComparer::GetHashCode(const LPMaterial& obj) const
		{
			return static_cast<int64>(reinterpret_cast<uintptr>(obj));//->getBatchHandle();
		}



		const IEqualityComparer<LPGeometryData>* GeometryDataEqualityComparer::BuiltIn::Default = 
			new GeometryDataEqualityComparer();

		bool GeometryDataEqualityComparer::Equals(const LPGeometryData& x, const LPGeometryData& y) const
		{
			const void* a = x;
			const void* b = y;
			return a==b;
		}
		int64 GeometryDataEqualityComparer::GetHashCode(const LPGeometryData& obj) const
		{
			return static_cast<int64>(reinterpret_cast<uintptr>(obj));//->getBatchHandle();
		}
	}
};