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

#include "SceneProcedure.h"

#include "SceneRenderScriptParser.h"
#include "ScenePass.h"
#include "apoc3d/Core/ResourceHandle.h"
#include "apoc3d/Core/Logging.h"
#include "apoc3d/Graphics/EffectSystem/EffectManager.h"
#include "apoc3d/Graphics/EffectSystem/Effect.h"
#include "apoc3d/Graphics/RenderSystem/RenderDevice.h"
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
			: m_renderDevice(dev) { }

		SceneProcedure::~SceneProcedure()
		{
			m_passes.DeleteAndClear();
			m_variables.DeleteAndClear();

			m_createdTextures.DeleteAndClear();
			m_createdRenderTarget.DeleteAndClear();
			m_createDepthStencil.DeleteAndClear();
			
			for (ProcGaussBlurFilter& f : m_createdGaussFilters)
			{
				delete f.Object;
			}
		}

		bool SceneProcedure::IsAvailable()
		{
			return m_isAvailable;
		}

		void SceneProcedure::Load(SceneRenderer* renderer, const ResourceLocation& rl)
		{
			SceneRenderScriptParser parser(m_renderDevice);
			parser.Parse(rl);
			m_name = parser.getSceneName();

			m_variables = parser.GlobalVars;

			m_passes.ResizeDiscard(parser.PassData.getCount());

			for (const auto& p : parser.PassData)
			{
				ScenePass* pass = new ScenePass(m_renderDevice, renderer, this, &p);
				m_passes.Add(pass);
			}


			// check device caps and set m_isAvailable
			m_isAvailable = true;
			for (SceneVariable* var : m_variables)
			{
				switch (var->Type)
				{
					case SceneVariableType::RenderTarget:
					case SceneVariableType::DepthStencil:
					{
						PixelFormat colorFmt = static_cast<PixelFormat>(var->Value[4]);
						DepthFormat depFmt = static_cast<DepthFormat>(var->Value[4]);
						
						if (var->Type == SceneVariableType::RenderTarget)
							depFmt = DEPFMT_Count;
						else
							colorFmt = FMT_Count;

						uint sampleCount = var->Value[5];

						if (sampleCount != 0)
						{
							const String* profile = m_renderDevice->getCapabilities()->FindClosesetMultisampleMode(sampleCount, colorFmt, depFmt);

							if (profile == nullptr || !m_renderDevice->getCapabilities()->SupportsRenderTarget(*profile, colorFmt, depFmt))
							{
								m_isAvailable = false;
							}
						}
						break;
					}

					case SceneVariableType::Effect:
					{
						if (EffectManager::getSingleton().HasEffect(var->DefaultStringValue))
						{
							Effect* fx = EffectManager::getSingleton().getEffect(var->DefaultStringValue);
							if (!fx->IsUnsupported())
								m_isAvailable = false;
						}
						else
						{
							m_isAvailable = false;
						}
						break;
					}

				}
			}


			// initialize resources
			if (m_isAvailable)
			{
				ObjectFactory* factory = m_renderDevice->getObjectFactory();

				for (SceneVariable* var : m_variables)
				{
					switch (var->Type)
					{
						case SceneVariableType::DepthStencil:
						case SceneVariableType::RenderTarget:
						{
							uint width = var->Value[0];
							uint height = var->Value[1];
							float wscale;
							float hscale;
							bool usePercentageLock = false;

							if (!width || !height)
							{
								wscale = reinterpret_cast<const float&>(var->Value[2]);
								hscale = reinterpret_cast<const float&>(var->Value[3]);

								Viewport vp = m_renderDevice->getViewport();
								width = static_cast<uint>(vp.Width * wscale + 0.5f);
								height = static_cast<uint>(vp.Height * hscale + 0.5f);
								usePercentageLock = true;
							}

							uint32 sampleCount = var->Value[5];

							if (var->Type == SceneVariableType::RenderTarget)
							{
								PixelFormat fmt = static_cast<PixelFormat>(var->Value[4]);

								RenderTarget* rt;
								if (sampleCount != 0)
								{
									const String* profile = m_renderDevice->getCapabilities()->FindClosesetMultisampleMode(sampleCount, fmt, DEPFMT_Count);
									assert(profile);

									rt = factory->CreateRenderTarget(width, height, fmt, *profile);
								}
								else
								{
									rt = factory->CreateRenderTarget(width, height, fmt, L"");
								}

								if (usePercentageLock)
								{
									rt->SetPercentageLock(wscale, hscale);
								}

								var->RTValue = rt;
								m_createdRenderTarget.Add(rt);
							}
							else if (var->Type == SceneVariableType::DepthStencil)
							{
								DepthFormat fmt = static_cast<DepthFormat>(var->Value[4]);

								DepthStencilBuffer* dsb;
								if (sampleCount != 0)
								{
									const String* profile = m_renderDevice->getCapabilities()->FindClosesetMultisampleMode(sampleCount, FMT_Count, fmt);
									assert(profile);

									dsb = factory->CreateDepthStencilBuffer(width, height, fmt, *profile);
								}
								else
								{
									dsb = factory->CreateDepthStencilBuffer(width, height, fmt, L"");
								}

								if (usePercentageLock)
								{
									dsb->SetPercentageLock(wscale, hscale);
								}

								var->DSValue = dsb;
								m_createDepthStencil.Add(dsb);
							}
							break;
						}

							////VARTYPE_Matrix,
							////VARTYPE_Vector4,
							//VARTYPE_Vector3,
							//VARTYPE_Vector2,
						case SceneVariableType::Texture:
						{
							if (var->DefaultStringValue.size())
							{
								FileLocation fl;// = FileSystem::getSingleton().TryLocate(m_vars[i]->DefaultStringValue, FileLocateRule::Textures);
								if (FileSystem::getSingleton().TryLocate(var->DefaultStringValue, FileLocateRule::Textures, fl))
								{
									ResourceHandle<Texture>* tex = TextureManager::getSingleton().CreateInstance(m_renderDevice, fl);
									var->TextureValue = tex;
									m_createdTextures.Add(tex);
								}
								else
								{
									LogManager::getSingleton().Write(LOG_Scene, 
										L"Texture resource " + var->DefaultStringValue + L" for Scene Procedure " + m_name + L" is not found", LOGLVL_Warning);
								}
							}
							else
							{
								var->TextureValue = nullptr;
							}
							break;
						}
						case SceneVariableType::Effect:
						{
							var->EffectValue = EffectManager::getSingleton().getEffect(var->DefaultStringValue);
							break;
						}
						
						case SceneVariableType::GaussBlurFilter:
						{
							uint width = var->Value[0];
							uint height = var->Value[1];

							ProcGaussBlurFilter pgbf;
							if (!width || !height)
							{
								float wscale = reinterpret_cast<const float&>(var->Value[2]);
								float hscale = reinterpret_cast<const float&>(var->Value[3]);

								Viewport vp = m_renderDevice->getViewport();
								width = static_cast<uint>(vp.Width * wscale + 0.5f);
								height = static_cast<uint>(vp.Height * hscale + 0.5f);
								pgbf.HasPercentageLock = true;
								pgbf.HeightPercentage = hscale;
								pgbf.WidthPercentage = wscale;
							}
							else
							{
								pgbf.HasPercentageLock = false;
							}

							int sampleCount = var->Value[5];
							float blurAmount = reinterpret_cast<const float&>(var->Value[4]);
							GaussBlurFilter* filter = new GaussBlurFilter(sampleCount,blurAmount, (int32)width,(int32)height);
							var->ObjectValue = filter;
							
							pgbf.Object = filter;
							
							m_createdGaussFilters.Add(pgbf);
							break;
						}
							//VARTYPE_Integer,
							//VARTYPE_Boolean,
							//VARTYPE_Effect
					}
				}
			}
		}

		void SceneProcedure::Invoke(const List<Camera*> cameras, SceneManager* sceMgr, BatchData* batchData)
		{
			CheckDimensions();

			m_lastCamera = 0;
			// pass each scene pass
			for (ScenePass* pass : m_passes)
			{
				pass->Invoke(cameras, sceMgr, batchData);
				m_lastCamera = pass->getCurrentCamera();
			}
		}

		void SceneProcedure::CheckDimensions()
		{
			Viewport vp = m_renderDevice->getViewport();
			for (ProcGaussBlurFilter& pgbf : m_createdGaussFilters)
			{
				if (pgbf.HasPercentageLock)
				{
					int expectedWidth = (int)(pgbf.WidthPercentage * vp.Width + 0.5f);
					int expectedHeight = (int)(pgbf.HeightPercentage * vp.Height + 0.5f);

					if (expectedHeight != pgbf.Object->getMapHeight() || expectedWidth != pgbf.Object->getMapWidth())
					{
						pgbf.Object->ChangeSettings(pgbf.Object->getBlurAmount(), expectedWidth, expectedHeight);
					}
				}
			}
		}

		RenderTarget* SceneProcedure::FindRenderTargetVar(const String& name) const
		{
			if (!m_isAvailable)
				return nullptr;

			for (SceneVariable* var : m_variables)
				if (var->Name == name)
					return var->RTValue;
			return nullptr;
		}
		DepthStencilBuffer* SceneProcedure::FindDepthStencilVar(const String& name) const
		{
			if (!m_isAvailable)
				return nullptr;

			for (SceneVariable* var : m_variables)
				if (var->Name == name)
					return var->DSValue;
			return nullptr;
		}

		void SceneProcedure::SetTextureVar(const String& name, ResourceHandle<Texture>* tex)
		{
			for (SceneVariable* var : m_variables)
			{
				if (var->Name == name)
				{
					assert(var->Type == SceneVariableType::Texture);
					var->TextureValue = tex;
					break;
				}
			}
		}
		void SceneProcedure::SetBooleanVar(const String& name, bool val)
		{
			for (SceneVariable* var : m_variables)
			{
				if (var->Name == name)
				{
					assert(var->Type == SceneVariableType::Boolean);
					var->Value[0] = val ? 1 : 0;
					break;
				}
			}
		}
		void SceneProcedure::SetVector4Var(const String& name, const Vector4& val)
		{
			for (SceneVariable* var : m_variables)
			{
				if (var->Name == name)
				{
					assert(var->Type == SceneVariableType::Vector4);
					memcpy(var->Value, &val, sizeof(float) * 4);
					break;
				}
			}
		}
		void SceneProcedure::SetVector2Var(const String& name, const Vector2& val)
		{
			for (SceneVariable* var : m_variables)
			{
				if (var->Name == name)
				{
					assert(var->Type == SceneVariableType::Vector2);
					memcpy(var->Value, &val, sizeof(float) * 2);
					break;
				}
			}
		}
		void SceneProcedure::SetFloatVar(const String& name, const float val)
		{
			for (SceneVariable* var : m_variables)
			{
				if (var->Name == name)
				{
					assert(var->Type == SceneVariableType::Single);
					*reinterpret_cast<float*>(var->Value) = val;
					break;
				}
			}
		}

	};
};