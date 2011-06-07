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

#include "Config\ConfigurationManager.h"
#include "Config\ConfigurationSection.h"

#include "Graphics\RenderOperationBuffer.h"
#include "Graphics\RenderOperation.h"
#include "Graphics\Material.h"
#include "Graphics\GeometryData.h"
#include "Core\GameTime.h"

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

					if (mtrl)
					{
						BatchHandle mtrlHandle = mtrl->getBatchHandle();
						m_mtrlList[mtrlHandle] = mtrl;
						m_priTable[mtrl->getPriority()]->
							operator[](mtrlHandle)->
							operator[](geoData->getBatchHandle())->Add(op);

					}
				}
			}
		}
		void BatchData::Clear()
		{
			for (PriorityTable::iterator i = m_priTable.begin();i!=m_priTable.end();++i)
			{
				MaterialTable* mtrlTbl = i->second;
				for (MaterialTable::iterator j = mtrlTbl->begin(); j!=mtrlTbl->end();j++)
				{
					GeometryTable* geoTbl = j->second;

					for (GeometryTable::iterator k = geoTbl->begin(); k != geoTbl->end(); k++)
					{
						k->second->FastClear();
					}
				}
			}
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

		void SceneRenderer::Load(const String& configName)
		{
			Configuration* config = ConfigurationManager::getSingleton().getConfiguration(configName);

			m_selectedProc = -1;

			for (Configuration::Iterator iter = config->begin(); iter != config->end(); iter++)
			{
				String file = iter->second->getAttribute(L"Script");

				// load proc
				FileLocation* fl = FileSystem::getSingleton().Locate(configName, FileLocateRule::Default);
				SceneProcedure* proc = new SceneProcedure(m_renderDevice);
				proc->Load(static_cast<ResourceLocation*>(fl));
				delete fl;

				m_procFallbacks.Add(proc);

				// check for compatibility
				if (m_selectedProc==-1)
				{
					if (proc->IsAvailable())
					{
						m_selectedProc = m_procFallbacks.getCount()-1;
					}
				}
			}
		}

		void SceneRenderer::RenderScene(SceneManager* sceMgr)
		{
			if (m_selectedProc !=-1)
			{
				m_procFallbacks[m_selectedProc]->Invoke(m_cameraList, sceMgr, &m_batchData);
			}
		}

		void SceneRenderer::RenderBatch()
		{

		}
	};
};