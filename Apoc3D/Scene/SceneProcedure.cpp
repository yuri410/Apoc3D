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
#include "SceneProcedure.h"

#include "SceneRenderScriptParser.h"
#include "ScenePass.h"
#include "apoc3d/Core/ResourceHandle.h"
#include "apoc3d/Core/Logging.h"
#include "apoc3d/Graphics/EffectSystem/EffectManager.h"
#include "apoc3d/Graphics/EffectSystem/Effect.h"
#include "apoc3d/Graphics/RenderSystem/RenderDevice.h"
#include "apoc3d/Graphics/RenderSystem/ObjectFactory.h"
#include "apoc3d/Graphics/RenderSystem/Texture.h"
#include "apoc3d/Graphics/RenderSystem/RenderTarget.h"
#include "apoc3d/Graphics/PixelFormat.h"
#include "apoc3d/Graphics/TextureManager.h"
#include "apoc3d/Math/GaussBlurFilter.h"
#include "apoc3d/Vfs/FileLocateRule.h"
#include "apoc3d/Vfs/FileSystem.h"
#include "apoc3d/Vfs/ResourceLocation.h"

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
			for (int i=0;i<m_createdGaussFilters.getCount();i++)
			{
				delete m_createdGaussFilters[i];
			}
			delete[] m_vars;
		}

		bool SceneProcedure::IsAvailable()
		{
			return m_isAvailable;
		}

		void SceneProcedure::Load(SceneRenderer* renderer, const ResourceLocation* rl)
		{
			SceneRenderScriptParser parser(m_renderDevice);
			parser.Parse(rl);
			m_name = parser.getSceneName();

			m_varCount = parser.GlobalVars.getCount();
			m_vars = new SceneVariable*[m_varCount];
			for (int i=0;i<m_varCount;i++)
			{
				m_vars[i] = parser.GlobalVars[i];
			}

			m_passes.ResizeDiscard(parser.PassData.getCount());

			for (int i=0;i<parser.PassData.getCount();i++)
			{
				ScenePass* pass = new ScenePass(m_renderDevice, renderer, this, &parser.PassData[i]);
				m_passes.Add(pass);
			}


			// check device caps and set m_isAvailable
			m_isAvailable = true;
			for (int i=0;i<m_varCount;i++)
			{
				switch (m_vars[i]->Type)
				{
					case VARTYPE_RenderTarget:
						{
							PixelFormat fmt = static_cast<PixelFormat>(m_vars[i]->Value[4]);
							DepthFormat depFmt = static_cast<DepthFormat>(m_vars[i]->Value[5]);

							uint sampleCount = m_vars[i]->Value[6];

							if (!m_renderDevice->getCapabilities()->SupportsRenderTarget(sampleCount, fmt, depFmt))
							{
								m_isAvailable = false;
							}
						}
						break;
					case VARTYPE_Effect:
						{
							if (EffectManager::getSingleton().HasEffect(m_vars[i]->DefaultStringValue))
							{
								Effect* fx = EffectManager::getSingleton().getEffect(m_vars[i]->DefaultStringValue);
								if (!fx->IsUnsupported())
									m_isAvailable = false;
							}
							else
							{
								m_isAvailable = false;
							}
						}
						break;
					
				}
			}
			


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
							uint width = m_vars[i]->Value[0];
							uint height = m_vars[i]->Value[1];

							if (!width || !height)
							{
								float wscale = reinterpret_cast<const float&>(m_vars[i]->Value[2]);
								float hscale = reinterpret_cast<const float&>(m_vars[i]->Value[3]);

								Viewport vp = m_renderDevice->getViewport();
								width = static_cast<uint>(vp.Width * wscale + 0.5f);
								height = static_cast<uint>(vp.Height * hscale + 0.5f);
							}

							PixelFormat fmt = static_cast<PixelFormat>(m_vars[i]->Value[4]);
							DepthFormat depFmt = static_cast<DepthFormat>(m_vars[i]->Value[5]);

							uint sampleCount = m_vars[i]->Value[6];

							RenderTarget* rt;
							if (depFmt == DEPFMT_Count)
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
							if (m_vars[i]->DefaultStringValue.size())
							{
								FileLocation* fl = FileSystem::getSingleton().TryLocate(m_vars[i]->DefaultStringValue, FileLocateRule::Textures);
								if (fl)
								{
									ResourceHandle<Texture>* tex = TextureManager::getSingleton().CreateInstance(m_renderDevice, fl, false);
									m_vars[i]->TextureValue = tex;
									m_createdTextures.Add(tex);
								}
								else
								{
									LogManager::getSingleton().Write(LOG_Scene, 
										L"Texture resource " + m_vars[i]->DefaultStringValue + L" for Scene Procedure " + m_name + L" is not found", LOGLVL_Warning);
								}
							}
							else
							{
								m_vars[i]->TextureValue = nullptr;
							}
							
						}
						break;
					case VARTYPE_Effect:
						{
							m_vars[i]->EffectValue = EffectManager::getSingleton().getEffect(m_vars[i]->DefaultStringValue);
						}
						break;
					case VARTYPE_GaussBlurFilter:
						{
							uint width = m_vars[i]->Value[0];
							uint height = m_vars[i]->Value[1];

							if (!width || !height)
							{
								float wscale = reinterpret_cast<const float&>(m_vars[i]->Value[2]);
								float hscale = reinterpret_cast<const float&>(m_vars[i]->Value[3]);

								Viewport vp = m_renderDevice->getViewport();
								width = static_cast<uint>(vp.Width * wscale + 0.5f);
								height = static_cast<uint>(vp.Height * hscale + 0.5f);
							}

							int sampleCount = m_vars[i]->Value[5];
							float blurAmount = reinterpret_cast<const float&>(m_vars[i]->Value[4]);
							GaussBlurFilter* filter = new GaussBlurFilter(sampleCount,blurAmount, (int32)width,(int32)height);
							m_vars[i]->ObjectValue = filter;
							
							m_createdGaussFilters.Add(filter);
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
			m_lastCamera = 0;
			// pass each scene pass
			for (int i=0;i<m_passes.getCount();i++)
			{
				m_passes[i]->Invoke(cameras, sceMgr, batchData);
				m_lastCamera = m_passes[i]->getCurrentCamera();
			}

		}

		RenderTarget* SceneProcedure::FindRenderTargetVar(const String& name) const
		{
			if (!m_isAvailable)
				return 0;
			for (int i=0;i<m_varCount;i++)
			{
				if (m_vars[i]->Name==name)
				{
					return m_vars[i]->RTValue;
				}
			}
			return nullptr;
		}

		void SceneProcedure::SetTextureVar(const String& name, ResourceHandle<Texture>* tex)
		{
			for (int i=0;i<m_varCount;i++)
			{
				if (m_vars[i]->Name == name)
				{
					assert(m_vars[i]->Type == VARTYPE_Texture);
					m_vars[i]->TextureValue = tex;
					break;
				}
			}
		}
		void SceneProcedure::SetBooleanVar(const String& name, bool val)
		{
			for (int i=0;i<m_varCount;i++)
			{
				if (m_vars[i]->Name == name)
				{
					assert(m_vars[i]->Type == VARTYPE_Boolean);
					m_vars[i]->Value[0] = val ? 1:0;
					break;
				}
			}
		}
		void SceneProcedure::SetVector4Var(const String& name, const Vector4& val)
		{
			for (int i=0;i<m_varCount;i++)
			{
				if (m_vars[i]->Name == name)
				{
					assert(m_vars[i]->Type == VARTYPE_Vector4);
					memcpy(m_vars[i]->Value, &val, sizeof(float)*4);
					break;
				}
			}
		}
		void SceneProcedure::SetVector2Var(const String& name, const Vector2& val)
		{
			for (int i=0;i<m_varCount;i++)
			{
				if (m_vars[i]->Name == name)
				{
					assert(m_vars[i]->Type == VARTYPE_Vector2);
					memcpy(m_vars[i]->Value, &val, sizeof(float)*2);
					break;
				}
			}
		}
		void SceneProcedure::SetFloatVar(const String& name, const float val)
		{
			for (int i=0;i<m_varCount;i++)
			{
				if (m_vars[i]->Name == name)
				{
					assert(m_vars[i]->Type == VARTYPE_Single);
					*reinterpret_cast<float*>(m_vars[i]->Value) = val;
					break;
				}
			}
		}

	};
};