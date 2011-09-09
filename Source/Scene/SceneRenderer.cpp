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

#include "SceneRenderer.h"

#include "Config/ConfigurationManager.h"
#include "Config/ConfigurationSection.h"

#include "Graphics/RenderSystem/RenderDevice.h"
#include "Graphics/EffectSystem/EffectParameter.h"
#include "Graphics/RenderOperationBuffer.h"
#include "Graphics/RenderOperation.h"
#include "Graphics/Material.h"
#include "Graphics/GeometryData.h"
#include "Core/GameTime.h"

#include "Vfs/FileSystem.h"
#include "Vfs/FileLocateRule.h"
#include "Vfs/ResourceLocation.h"

#include "SceneRenderScriptParser.h"
#include "SceneProcedure.h"
#include "SceneObject.h"
#include "ScenePass.h"

using namespace Apoc3D::Config;
using namespace Apoc3D::VFS;

namespace Apoc3D
{
	namespace Scene
	{
		void BatchData::AddVisisbleObject(SceneObject* obj, int level)
		{
			m_objectCount++;
			const RenderOperationBuffer* buffer = obj->GetRenderOperation(level);

			if (buffer)
			{
				for (int k=0;k<buffer->getCount();k++)
				{
					RenderOperation op = buffer->get(k);

					Material* mtrl = op.Material;
					GeometryData* geoData = op.GeometryData;

					if (mtrl && geoData)
					{
						Matrix temp;
						Matrix::Multiply(temp, obj->getTrasformation(), op.RootTransform);
						op.RootTransform = temp;

						MaterialTable* mtrlTable;
						if (!m_priTable.TryGetValue(mtrl->getPriority(), mtrlTable))
						{
							mtrlTable = new MaterialTable();
							m_priTable.Add(mtrl->getPriority(),mtrlTable);
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
			for (PriorityTable::Enumerator i = m_priTable.GetEnumerator();i.MoveNext();)
			{
				MaterialTable* mtrlTbl = *i.getCurrentValue();
				for (MaterialTable::Enumerator j = mtrlTbl->GetEnumerator(); j.MoveNext();)
				{
					GeometryTable* geoTbl = *j.getCurrentValue();

					for (GeometryTable::Enumerator k = geoTbl->GetEnumerator(); k.MoveNext(); )
					{
						(*(k.getCurrentValue()))->FastClear();
					}
				}
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
			: m_renderDevice(device)
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

			for (Configuration::Iterator iter = config->begin(); iter != config->end(); iter++)
			{
				String file = iter->second->getAttribute(L"Script");

				// load proc
				FileLocation* fl = FileSystem::getSingleton().Locate(file, FileLocateRule::Default);
				SceneProcedure* proc = new SceneProcedure(m_renderDevice);
				proc->Load(this, static_cast<ResourceLocation*>(fl));
				delete fl;

				m_procFallbacks.Add(proc);

				
				//m_procFallbacks.Reverse();
				
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
			uint64 selectorMask;
			if (selectorID == -1)
				selectorMask = 0xffffffffffffffff;
			else
				selectorMask = (uint64)1<<selectorID;

			const PriorityTable& table = m_batchData.getTable();

			for (PriorityTable::Enumerator i = table.GetEnumerator();i.MoveNext();)
			{
				MaterialTable* mtrlTbl = *i.getCurrentValue();
				for (MaterialTable::Enumerator j = mtrlTbl->GetEnumerator(); j.MoveNext();)
				{
					Material* mtrl = *j.getCurrentKey();

					if (mtrl->getPassFlags() & selectorMask)
					{
						GeometryTable* geoTbl = *(j.getCurrentValue());

						for (GeometryTable::Enumerator k = geoTbl->GetEnumerator(); k.MoveNext();)
						{
							const OperationList* opList = *k.getCurrentValue();
							if (opList->getCount())
							{
								m_renderDevice->Render(mtrl, opList->getInternalPointer(), opList->getCount(), selectorID);
							}
						}
					}
				}
			}
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
			return obj->getBatchHandle();
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
			return obj->getBatchHandle();
		}
	}
};