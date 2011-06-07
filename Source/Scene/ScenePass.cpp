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
#include "ScenePass.h"

#include "SceneProcedure.h"
#include "SceneRenderer.h"
#include "SceneManager.h"

namespace Apoc3D
{
	namespace Scene
	{
		ScenePass::ScenePass(SceneProcedure* parnetProc, const ScenePassData* passData)
			: m_parentProc(parnetProc), m_cameraID(passData->CameraID), m_name(passData->Name), m_selectorID(passData->SelectorID)
		{
			for (size_t i=0;i<passData->Instructions.size();i++)
			{
				m_instuctions.Add(passData->Instructions[i]);
			}
		}


		ScenePass::~ScenePass(void)
		{
		}
		
		
		void ScenePass::Invoke(const FastList<Camera*> cameras, SceneManager* sceMgr, BatchData* batchData)
		{
			m_currentCamera = cameras[m_cameraID];
			if (m_currentCamera != m_parentProc->getLastCamera())
			{
				batchData->Clear();
				sceMgr->PrepareVisibleObjects(m_currentCamera, batchData);
			}

			// execute scene pass render script
			for (int i=0;i<m_instuctions.getCount();i++)
			{

			}
		}
	};
};