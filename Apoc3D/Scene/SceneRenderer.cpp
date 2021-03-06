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
#include "apoc3d/Core/AppTime.h"

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
			m_retiredOpList.DeleteAndClear();
			m_retiredGeoTable.DeleteAndClear();
			m_retiredMtrlTable.DeleteAndClear();

			delete m_invalidMtrlPointers;
			delete m_invalidGeoPointers;
		}

		OperationList* BatchDataBufferCache::ObtainNewOperationList()
		{
			if (m_retiredOpList.getCount() > 0)
				return m_retiredOpList.Dequeue();

			return new OperationList(m_minOpListSize);
		}
		GeometryTable* BatchDataBufferCache::ObtainNewGeometryTable()
		{
			if (m_retiredGeoTable.getCount() > 0)
				return m_retiredGeoTable.Dequeue();

			return new GeometryTable(m_minGeoTableSize);
		}
		MaterialTable* BatchDataBufferCache::ObtainNewMaterialTable()
		{
			if (m_retiredMtrlTable.getCount() > 0)
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
			for (int32 i = 0; i < m_retiredOpList.getCount(); i++)
			{
				OperationList* opList = m_retiredOpList[i];
				if (opList->getCapacity() < m_minOpListSize)
					opList->ResizeDiscard(m_minOpListSize);
			}
			for (int32 i = 0; i < m_retiredGeoTable.getCount(); i++)
			{
				GeometryTable* geoTable = m_retiredGeoTable[i];
				if (geoTable->getPrimeCapacity() < m_minGeoTableSize)
					geoTable->Resize(m_minGeoTableSize);
			}

			for (int32 i = 0; i < m_retiredMtrlTable.getCount(); i++)
			{
				MaterialTable* mtrlTable = m_retiredMtrlTable[i];
				if (mtrlTable->getPrimeCapacity() < m_minMtrlTableSize)
					mtrlTable->Resize(m_minMtrlTableSize);
			}


			// create to target count
			int32 numToCreate = opListCount - m_retiredOpList.getCount();
			for (int32 i = 0; i < numToCreate; i++)
			{
				OperationList* opList = new OperationList(m_minOpListSize);
				m_retiredOpList.Enqueue(opList);
			}

			numToCreate = geoTableCount - m_retiredGeoTable.getCount();
			for (int32 i = 0; i < numToCreate; i++)
			{
				GeometryTable* geoTable = new GeometryTable(m_minGeoTableSize);
				m_retiredGeoTable.Enqueue(geoTable);
			}

			numToCreate = mtrlTableCount - m_retiredMtrlTable.getCount();
			for (int32 i = 0; i < numToCreate; i++)
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
			for (int i = 0; i < MaxPriority; i++)
			{
				MaterialTable* mtrlTable;
				if (!m_priTable.TryGetValue(i, mtrlTable))
				{
					mtrlTable = m_bufferCache.ObtainNewMaterialTable();//new MaterialTable();
					m_priTable.Add(i, mtrlTable);
				}
			}
		}
		BatchData::~BatchData()
		{
			for (MaterialTable* mtrlTbl : m_priTable.getValueAccessor())
			{
				for (GeometryTable* geoTbl : mtrlTbl->getValueAccessor())
				{
					geoTbl->DeleteValuesAndClear();
				}
				mtrlTbl->DeleteValuesAndClear();
			}
			m_priTable.DeleteValuesAndClear();
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

			for (MaterialTable* mtrlTbl : m_priTable.getValueAccessor())
			{
				MaterialTable& mtrlTblRef = *mtrlTbl;
				for (auto me : mtrlTblRef)
				{
					Material* mtrl = me.Key;
					GeometryTable& geoTbl = *me.Value;

					bool isMtrlUsed = false;
					for (auto ge : geoTbl)
					{
						GeometryData* geoData = ge.Key;
						const OperationList* opList = ge.Value;

						if (opList->getCount())
						{
							if (geoData->Discard)
								break;

							isMtrlUsed = true;
						}
						else
						{
							invalidGeoPointers.Add(geoData);
						}
					}

					if (invalidGeoPointers.getCount() > 0)
					{
						for (int32 j = 0; j < invalidGeoPointers.getCount(); j++)
						{
							GeometryData* item = invalidGeoPointers[j];
							OperationList* list = geoTbl[item];
							assert(list->getCount() == 0);

							m_bufferCache.RecycleOperationList(list);

							geoTbl.Remove(item);
						}

						invalidGeoPointers.Clear();
					}


					if (isMtrlUsed)
					{
						if (mtrl->getPassFlags() & selectorMask)
						{
							for (const OperationList* opList : geoTbl.getValueAccessor())
							{
								if (opList->getCount())
								{
									device->Render(mtrl, opList->getElements(), opList->getCount(), selectorID);
								}
							}
						}
					}
					else
					{
						invalidMtrlPointers.Add(mtrl);
					}
				}

				if (invalidMtrlPointers.getCount() > 0)
				{
					for (int32 i = 0; i < invalidMtrlPointers.getCount(); i++)
					{
						Material* item = invalidMtrlPointers[i];
						GeometryTable* geoTbl = mtrlTblRef[item];

						for (OperationList* opList : geoTbl->getValueAccessor())
						{
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
				for (int k = 0; k < buffer->getCount(); k++)
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

						uint priority = Math::Min(mtrl->getPriority(), MaxPriority - 1);

						// add the rop from outer table to inner table(top down)
						MaterialTable* mtrlTable;
						if (!m_priTable.TryGetValue(priority, mtrlTable))
						{
							mtrlTable = m_bufferCache.ObtainNewMaterialTable();
							m_priTable.Add(priority, mtrlTable);
						}

						GeometryTable* geoTable;
						if (!mtrlTable->TryGetValue(mtrl, geoTable))
						{
							geoTable = m_bufferCache.ObtainNewGeometryTable();
							mtrlTable->Add(mtrl, geoTable);
						}

						OperationList* opList;
						if (!geoTable->TryGetValue(geoData, opList))
						{
							opList = m_bufferCache.ObtainNewOperationList();
							geoTable->Add(geoData, opList);
						}

						opList->Add(op);
					}
				}
			}
		}
		
		void BatchData::AddRenderOperation(const RenderOperationBuffer& ops)
		{
			for (RenderOperation op : ops)
			{
				Material* mtrl = op.Material;
				GeometryData* geoData = op.GeometryData;

				if (mtrl && geoData)
				{
					uint priority = Math::Min(mtrl->getPriority(), MaxPriority - 1);

					// add the rop from outer table to inner table(top down)
					MaterialTable* mtrlTable;
					if (!m_priTable.TryGetValue(priority, mtrlTable))
					{
						mtrlTable = m_bufferCache.ObtainNewMaterialTable();
						m_priTable.Add(priority, mtrlTable);
					}

					GeometryTable* geoTable;
					if (!mtrlTable->TryGetValue(mtrl, geoTable))
					{
						geoTable = m_bufferCache.ObtainNewGeometryTable();
						mtrlTable->Add(mtrl, geoTable);
					}

					OperationList* opList;
					if (!geoTable->TryGetValue(geoData, opList))
					{
						opList = m_bufferCache.ObtainNewOperationList();
						geoTable->Add(geoData, opList);
					}

					opList->Add(op);
				}
			}
		}

		void BatchData::Clear()
		{
			m_objectCount = 0;

			// this will only clear the rop list inside. The hashtables are remained as 
			// it is highly possible the next time the buckets in them are reused.
			for (MaterialTable* mtrlTbl : m_priTable.getValueAccessor())
			{
				for (GeometryTable* geoTbl : mtrlTbl->getValueAccessor())
				{
					for (OperationList* opList : geoTbl->getValueAccessor())
					{
						opList->Clear();
					}
				}

			}
		}
		void BatchData::Reset()
		{
			for (MaterialTable* mtrlTbl : m_priTable.getValueAccessor())
			{
				for (GeometryTable* geoTbl : mtrlTbl->getValueAccessor())
				{
					for (OperationList* opList : geoTbl->getValueAccessor())
					{
						m_bufferCache.RecycleOperationList(opList);
					}
					m_bufferCache.RecycleGeometryTable(geoTbl);
				}
				mtrlTbl->Clear();
			}
		}
		bool BatchData::HasObject(uint64 selectMask)
		{
			for (MaterialTable* mtrlTbl : m_priTable.getValueAccessor())
			{
				for (Material* m : mtrlTbl->getKeyAccessor())
				{
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


		SceneRenderer::~SceneRenderer()
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
			for (ConfigurationSection* sect : config->getSubSections())
			{
				String file = sect->getAttribute(L"Script");

				// load proc
				FileLocation fl = FileSystem::getSingleton().Locate(file, FileLocateRule::Default);
				SceneProcedure* proc = new SceneProcedure(m_renderDevice);
				proc->Load(this, fl);

				m_procFallbacks.Add(proc);

				
				if (m_selectedProc == -1)
				{
					// check if the current render system support this proc
					if (proc->IsAvailable())
					{
						m_selectedProc = m_procFallbacks.getCount() - 1;
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