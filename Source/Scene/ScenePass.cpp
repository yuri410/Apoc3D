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

#include "Graphics/RenderSystem/RenderDevice.h"
#include "Math/Math.h"

using namespace Apoc3D::Math;

namespace Apoc3D
{
	namespace Scene
	{
		ScenePass::ScenePass(RenderDevice* device, SceneRenderer* renderer, SceneProcedure* parnetProc, const ScenePassData* passData)
			: m_parentProc(parnetProc), m_cameraID(passData->CameraID), m_name(passData->Name), m_selectorID(passData->SelectorID),
			m_renderDevice(device), m_renderer(renderer)
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
				const SceneInstruction& inst = m_instuctions[i];
				switch (inst.Operation)
				{
				case SOP_And:
					break;
				case SOP_Or:
					break;
				case SOP_Not:
					break;
				case SOP_Clear:
					{
						ClearFlags flags;

						if (inst.Args[0].IsImmediate)
						{
							flags = reinterpret_cast<const ClearFlags&>(inst.Args[0].DefaultValue[0]);
						}
						else
						{
							flags = reinterpret_cast<const ClearFlags&>(inst.Args[0].Var->DefaultValue[0]);
						}

						float depth;
						if (inst.Args[1].IsImmediate)
						{
							depth = reinterpret_cast<const float&>(inst.Args[1].DefaultValue[0]);
						}
						else
						{
							depth = reinterpret_cast<const float&>(inst.Args[1].Var->DefaultValue[0]);
						}

						int stencil;
						if (inst.Args[2].IsImmediate)
						{
							stencil = reinterpret_cast<const int&>(inst.Args[2].DefaultValue[0]);
						}
						else
						{
							stencil = reinterpret_cast<const int&>(inst.Args[2].Var->DefaultValue[0]);
						}

						ColorValue color;
						if (inst.Args[3].IsImmediate)
						{
							color = reinterpret_cast<const ColorValue&>(inst.Args[3].DefaultValue[0]);
						}
						else
						{
							color = reinterpret_cast<const ColorValue&>(inst.Args[3].Var->DefaultValue[0]);
						}
						m_renderDevice->Clear(flags, color, depth, stencil);
					}
					break;
				case SOP_Render:
					{
						m_renderer->RenderBatch();
					}
					break;
				case SOP_VisibleTo:
					{
						bool result;
						int selectorID;
						if (inst.Args[0].IsImmediate)
						{
							selectorID = reinterpret_cast<const int&>(inst.Args[0].DefaultValue[0]);
						}
						else
						{
							selectorID = reinterpret_cast<const int&>(inst.Args[0].Var->DefaultValue[0]);
						}
						uint64 selectMask = 1 << m_selectorID;
						
					}
					break;
				case SOP_UseRT:
					{
						int index;
						if (inst.Args[0].IsImmediate)
						{
							index = reinterpret_cast<const int&>(inst.Args[0].DefaultValue[0]);
						}
						else
						{
							index = reinterpret_cast<const int&>(inst.Args[0].Var->DefaultValue[0]);
						}

						
						if (inst.Args.size() > 1)
						{
							m_renderDevice->SetRenderTarget(
								index,
								inst.Args[1].Var->RTValue);
						}
						else
						{
							
							m_renderDevice->SetRenderTarget(
								index,
								0);
						}
					}
					break;
				}
			}
		}
	};
};