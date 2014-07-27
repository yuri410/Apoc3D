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
		BatchDataBufferCache::BatchDataBufferCache(int32 opListCount, int32 geoTableCount, int32 mtrlTableCount, 
			int32 minGeoTableSize, int32 minMtrlTableSize, int32 minOpListSize)
		{
			m_invalidMtrlPointers = new InvalidMtrlPointerList(50);
			m_invalidGeoPointers = new InvalidGeoPointerList(50);

			Reserve(opListCount, geoTableCount, mtrlTableCount,
				minGeoTableSize, minMtrlTableSize, minOpListSize);
		}

		BatchDataBufferCache::~BatchDataBufferCache()
		{
			for (int32 i=0;i<m_retiredOpList.getCount();i++)
			{
				OperationList* opList = m_retiredOpList.GetElement(i);
				delete opList;
			}
			for (int32 i=0;i<m_retiredGeoTable.getCount();i++)
			{
				GeometryTable* geoTable = m_retiredGeoTable.GetElement(i);
				delete geoTable;
			}

			for (int32 i=0;i<m_retiredMtrlTable.getCount();i++)
			{
				MaterialTable* mtrlTable = m_retiredMtrlTable.GetElement(i);
				delete mtrlTable;
			}

			m_retiredOpList.Clear();
			m_retiredGeoTable.Clear();
			m_retiredMtrlTable.Clear();

			delete m_invalidMtrlPointers;
			delete m_invalidGeoPointers;
		}

		OperationList* BatchDataBufferCache::ObtainNewOperationList()
		{
			if (m_retiredOpList.getCount())
				return m_retiredOpList.Dequeue();

			return new OperationList(m_minOpListSize);
		}
		GeometryTable* BatchDataBufferCache::ObtainNewGeometryTable()
		{
			if (m_retiredGeoTable.getCount())
				return m_retiredGeoTable.Dequeue();

			return new GeometryTable(m_minGeoTableSize);
		}
		MaterialTable* BatchDataBufferCache::ObtainNewMaterialTable()
		{
			if (m_retiredMtrlTable.getCount())
				return m_retiredMtrlTable.Dequeue();

			return new MaterialTable(m_minMtrlTableSize);
		}

		void BatchDataBufferCache::RecycleOperationList(OperationList* obj)
		{
			obj->Clear();
			m_retiredOpList.Enqueue(obj); 
		}
		void BatchDataBufferCache::RecycleGeometryTable(GeometryTable* obj) 
		{
			obj->Clear();
			m_retiredGeoTable.Enqueue(obj); 
		}
		void BatchDataBufferCache::RecycleMaterialTable(MaterialTable* obj)
		{
			obj->Clear();
			m_retiredMtrlTable.Enqueue(obj); 
		}


		void BatchDataBufferCache::Reserve(int32 opListCount, int32 geoTableCount, int32 mtrlTableCount,
			int32 minGeoTableSize, int32 minMtrlTableSize, int32 minOpListSize)
		{
			m_minGeoTableSize = minGeoTableSize;
			m_minMtrlTableSize = minMtrlTableSize;
			m_minOpListSize = minOpListSize;

			// ensure current buffer's capacity
			for (int32 i=0;i<m_retiredOpList.getCount();i++)
			{
				OperationList* opList = m_retiredOpList.GetElement(i);
				if (opList->getCapacity()<m_minOpListSize)
					opList->ResizeDiscard(m_minOpListSize);
			}
			for (int32 i=0;i<m_retiredGeoTable.getCount();i++)
			{
				GeometryTable* geoTable = m_retiredGeoTable.GetElement(i);
				if (geoTable->getPrimeCapacity()<m_minGeoTableSize)
					geoTable->Resize(m_minGeoTableSize);
			}

			for (int32 i=0;i<m_retiredMtrlTable.getCount();i++)
			{
				MaterialTable* mtrlTable = m_retiredMtrlTable.GetElement(i);
				if (mtrlTable->getPrimeCapacity()<m_minMtrlTableSize)
					mtrlTable->Resize(m_minMtrlTableSize);
			}


			// create to target count
			int32 numToCreate = opListCount - m_retiredOpList.getCount();
			for (int32 i=0;i<numToCreate;i++)
			{
				OperationList* opList = new OperationList(m_minOpListSize);
				m_retiredOpList.Enqueue(opList);
			}

			numToCreate = geoTableCount - m_retiredGeoTable.getCount();
			for (int32 i=0;i<numToCreate;i++)
			{
				GeometryTable* geoTable = new GeometryTable(m_minGeoTableSize);
				m_retiredGeoTable.Enqueue(geoTable);
			}

			numToCreate = mtrlTableCount - m_retiredMtrlTable.getCount();
			for (int32 i=0;i<numToCreate;i++)
			{
				MaterialTable* mtrlTable = new MaterialTable(m_minMtrlTableSize);
				m_retiredMtrlTable.Enqueue(mtrlTable);
			}
		}

		/************************************************************************/
		/*                                                                      */
		/************************************************************************/
		

		BatchData::BatchData()
			: m_bufferCache(0, 0, MaxPriority+1, 50, 50, 50), m_objectCount(0), m_priTable(MaxPriority+1)
		{
			for (int i=0;i<MaxPriority;i++)
			{
				MaterialTable* mtrlTable;
				if (!m_priTable.TryGetValue(i, mtrlTable))
				{
					mtrlTable = m_bufferCache.ObtainNewMaterialTable();//new MaterialTable();
					m_priTable.Add(i,mtrlTable);
				}
			}
		}
		BatchData::~BatchData()
		{
			for (PriorityTable::Enumerator i = m_priTable.GetEnumerator();i.MoveNext();)
			{
				MaterialTable* mtrlTbl = i.getCurrentValue();
				for (MaterialTable::Enumerator j = mtrlTbl->GetEnumerator(); j.MoveNext();)
				{
					GeometryTable* geoTbl = j.getCurrentValue();

					for (GeometryTable::Enumerator k = geoTbl->GetEnumerator(); k.MoveNext(); )
					{
						OperationList* opList = k.getCurrentValue();
						delete opList;
					}
					delete geoTbl;
				}
				delete mtrlTbl;
			}
			m_priTable.Clear();
		}



		void BatchData::RenderBatch(RenderDevice* device, int selectorID)
		{
			uint64 selectorMask;
			if (selectorID == -1)
				selectorMask = 0xffffffffffffffff;
			else
				selectorMask = (uint64)1<<selectorID;

			BatchDataBufferCache::InvalidGeoPointerList& invalidGeoPointers = *m_bufferCache.getInvalidGeoPointerBuffer();
			BatchDataBufferCache::InvalidMtrlPointerList& invalidMtrlPointers = *m_bufferCache.getInvalidMtrlPointerBuffer();

			for (PriorityTable::Enumerator pe = m_priTable.GetEnumerator();pe.MoveNext();)
			{
				MaterialTable* mtrlTbl = pe.getCurrentValue();
				for (MaterialTable::Enumerator me = mtrlTbl->GetEnumerator(); me.MoveNext();)
				{
					GeometryTable* geoTbl = me.getCurrentValue();

					bool isMtrlUsed = false;
					for (GeometryTable::Enumerator ge = geoTbl->GetEnumerator(); ge.MoveNext();)
					{
						GeometryData* geoData = ge.getCurrentKey();
						const OperationList* opList = ge.getCurrentValue();

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
							invalidGeoPointers.Add(geoData);
						}
					}

					if (invalidGeoPointers.getCount()>0)
					{
						for (int32 j=0;j<invalidGeoPointers.getCount();j++)
						{
							GeometryData* item = invalidGeoPointers[j];
							OperationList* list = geoTbl->operator[](item);
							assert(list->getCount()==0);
							
							m_bufferCache.RecycleOperationList(list);

							geoTbl->Remove(item);
						}

						invalidGeoPointers.Clear();
					}


					Material* mtrl = me.getCurrentKey();
					if (isMtrlUsed)
					{
						if (mtrl->getPassFlags() & selectorMask)
						{
							for (GeometryTable::Enumerator k = geoTbl->GetEnumerator(); k.MoveNext();)
							{
								const OperationList* opList = k.getCurrentValue();
								if (opList->getCount())
								{
									device->Render(mtrl, opList->getInternalPointer(), opList->getCount(), selectorID);
								}
							}
						}
					}
					else
					{
						invalidMtrlPointers.Add(mtrl);
					}
				}

				if (invalidMtrlPointers.getCount()>0)
				{
					for (int32 i=0;i<invalidMtrlPointers.getCount();i++)
					{
						Material* item = invalidMtrlPointers[i];//*iter;
						GeometryTable* geoTbl = mtrlTbl->operator[](item);
						for (GeometryTable::Enumerator k = geoTbl->GetEnumerator(); k.MoveNext();)
						{
							OperationList* opList = k.getCurrentValue();
							m_bufferCache.RecycleOperationList(opList);
						}
						m_bufferCache.RecycleGeometryTable(geoTbl);
						mtrlTbl->Remove(item);
					}
					invalidMtrlPointers.Clear();
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
						
						uint priority = Math::Min(mtrl->getPriority(), MaxPriority);

						// add the rop from outer table to inner table(top down)
						MaterialTable* mtrlTable;
						if (!m_priTable.TryGetValue(priority, mtrlTable))
						{
							mtrlTable = m_bufferCache.ObtainNewMaterialTable();// new MaterialTable();
							m_priTable.Add(priority,mtrlTable);
						}

						GeometryTable* geoTable;
						if (!mtrlTable->TryGetValue(mtrl, geoTable))
						{
							geoTable = m_bufferCache.ObtainNewGeometryTable();// new GeometryTable();
							mtrlTable->Add(mtrl, geoTable);
						}

						OperationList* opList;
						if (!geoTable->TryGetValue(geoData, opList))
						{
							opList = m_bufferCache.ObtainNewOperationList();// new OperationList();
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
				MaterialTable* mtrlTbl = i.getCurrentValue();
				for (MaterialTable::Enumerator j = mtrlTbl->GetEnumerator(); j.MoveNext();)
				{
					GeometryTable* geoTbl = j.getCurrentValue();

					for (GeometryTable::Enumerator k = geoTbl->GetEnumerator(); k.MoveNext(); )
					{
						k.getCurrentValue()->Clear();
					}
				}

			}
		}
		void BatchData::Reset()
		{
			for (PriorityTable::Enumerator i = m_priTable.GetEnumerator();i.MoveNext();)
			{
				MaterialTable* mtrlTbl = i.getCurrentValue();
				for (MaterialTable::Enumerator j = mtrlTbl->GetEnumerator(); j.MoveNext();)
				{
					GeometryTable* geoTbl = j.getCurrentValue();

					for (GeometryTable::Enumerator k = geoTbl->GetEnumerator(); k.MoveNext(); )
					{
						m_bufferCache.RecycleOperationList(k.getCurrentValue());
					}
					m_bufferCache.RecycleGeometryTable(geoTbl);
				}
				mtrlTbl->Clear();
			}
			
		}
		bool BatchData::HasObject(uint64 selectMask)
		{
			for (PriorityTable::Enumerator i = m_priTable.GetEnumerator();i.MoveNext();)
			{
				MaterialTable* mtrlTbl = i.getCurrentValue();
				for (MaterialTable::Enumerator j = mtrlTbl->GetEnumerator(); j.MoveNext();)
				{
					Material* m = j.getCurrentKey();

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
				String file = iter.getCurrentValue()->getAttribute(L"Script");

				// load proc
				FileLocation fl = FileSystem::getSingleton().Locate(file, FileLocateRule::Default);
				SceneProcedure* proc = new SceneProcedure(m_renderDevice);
				proc->Load(this, fl);

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
			m_batchData.RenderBatch(m_renderDevice, selectorID);
		}

		void SceneRenderer::ResetBatchTable()
		{
			m_batchData.Reset();
		}
	};
};