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
#include "SceneProcedure.h"

#include "SceneRenderScriptParser.h"
#include "ScenePass.h"

#include "Vfs/ResourceLocation.h"
#include "Graphics/RenderSystem/RenderDevice.h"
#include "Graphics/RenderSystem/ObjectFactory.h"
#include "Graphics/PixelFormat.h"

namespace Apoc3D
{
	namespace Scene
	{
		SceneProcedure::SceneProcedure(RenderDevice* dev)
			: m_renderDevice(dev), m_isAvailable(false)
		{
		}


		SceneProcedure::~SceneProcedure(void)
		{
			for (int i=0;i<m_passes.getCount();i++)
			{
				delete m_passes[i];
			}
			for (int i=0;i<m_varCount;i++)
			{
				delete m_vars[i];
			}

			for (int i=0;i<m_createdTextures.getCount();i++)
			{
				delete m_createdTextures[i];
			}
			for (int i=0;i<m_createdRenderTarget.getCount();i++)
			{
				delete m_createdRenderTarget[i];
			}

			delete[] m_vars;
		}

		bool SceneProcedure::IsAvailable()
		{
			return m_isAvailable;
		}

		void SceneProcedure::Load(const ResourceLocation* rl)
		{
			SceneRenderScriptParser parser(m_renderDevice);
			parser.Parse(rl);

			m_varCount = parser.GlobalVars.getCount();
			m_vars = new SceneVariable*[m_varCount];
			for (int i=0;i<m_varCount;i++)
			{
				m_vars[i] = parser.GlobalVars[i];
			}

			m_passes.ResizeDiscard(parser.PassData.getCount());

			for (int i=0;i<parser.PassData.getCount();i++)
			{
				ScenePass* pass = new ScenePass(this, &parser.PassData[i]);
				m_passes.Add(pass);
			}


			// check and set m_isAvailable



			// initialize resources
			if (m_isAvailable)
			{
				ObjectFactory* factory = m_renderDevice->getObjectFactory();

				for (int i=0;i<m_varCount;i++)
				{
					switch (m_vars[i]->Type)
					{
					case VARTYPE_RenderTarget:
						{
							uint width = m_vars[i]->DefaultValue[0];
							uint height = m_vars[i]->DefaultValue[1];

							if (!width || !height)
							{
								float wscale = reinterpret_cast<const float&>(m_vars[i]->DefaultValue[2]);
								float hscale = reinterpret_cast<const float&>(m_vars[i]->DefaultValue[3]);

								Viewport vp = m_renderDevice->getViewport();
								width = static_cast<uint>(vp.Width * wscale + 0.5f);
								height = static_cast<uint>(vp.Height * hscale + 0.5f);
							}

							PixelFormat fmt = static_cast<PixelFormat>(m_vars[i]->DefaultValue[4]);
							DepthFormat depFmt = static_cast<DepthFormat>(m_vars[i]->DefaultValue[5]);

							uint sampleCount = m_vars[i]->DefaultValue[6];

							RenderTarget* rt;
							if (depFmt != DEPFMT_Count)
							{
								rt = factory->CreateRenderTarget(width, height, fmt);
							}
							else
							{
								rt = factory->CreateRenderTarget(width, height, fmt, depFmt, sampleCount);
							}
							m_vars[i]->RTValue = rt;
							m_createdRenderTarget.Add(rt);
						}
						break;
							////VARTYPE_Matrix,
							////VARTYPE_Vector4,
							//VARTYPE_Vector3,
							//VARTYPE_Vector2,
					case VARTYPE_Texture:
						{

						}
						break;
					case VARTYPE_Effect:
						{

						}
						break;
							//VARTYPE_Integer,
							//VARTYPE_Boolean,
							//VARTYPE_Effect
					}
				}
			}
		}

		void SceneProcedure::Invoke(const FastList<Camera*> cameras, SceneManager* sceMgr, BatchData* batchData)
		{
			// pass each scene pass
			for (int i=0;i<m_passes.getCount();i++)
			{
				m_passes[i]->Invoke(cameras, sceMgr, batchData);
				m_lastCamera = m_passes[i]->getCurrentCamera();
			}

		}
	};
};