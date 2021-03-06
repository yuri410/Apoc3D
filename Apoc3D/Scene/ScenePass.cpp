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

#include "ScenePass.h"

#include "SceneProcedure.h"
#include "SceneRenderer.h"
#include "SceneManager.h"

#include "apoc3d/Core/Logging.h"
#include "apoc3d/Graphics/RenderSystem/RenderDevice.h"
#include "apoc3d/Graphics/RenderSystem/RenderStateManager.h"
#include "apoc3d/Graphics/RenderSystem/RenderTarget.h"
#include "apoc3d/Graphics/RenderSystem/HardwareBuffer.h"
#include "apoc3d/Graphics/RenderSystem/VertexDeclaration.h"
#include "apoc3d/Graphics/RenderSystem/VertexElement.h"
#include "apoc3d/Graphics/EffectSystem/EffectParameter.h"
#include "apoc3d/Graphics/EffectSystem/Effect.h"
#include "apoc3d/Graphics/Material.h"
#include "apoc3d/Graphics/GeometryData.h"
#include "apoc3d/Math/GaussBlurFilter.h"

using namespace Apoc3D::Core;
using namespace Apoc3D::Math;

namespace Apoc3D
{
	namespace Scene
	{
		ScenePass::ScenePass(RenderDevice* device, SceneRenderer* renderer, SceneProcedure* parnetProc, const ScenePassData* passData)
			: m_parentProc(parnetProc), m_cameraID(passData->CameraID), m_name(passData->Name), m_selectorID(passData->SelectorID),
			m_renderDevice(device), m_renderer(renderer), m_instuctions(passData->Instructions)
		{
			// generate the quad vertices
			ObjectFactory* fac = device->getObjectFactory();

			m_quadVtxDecl = fac->CreateVertexDeclaration(
			{
				{ 0, VEF_Vector3, VEU_Position }
			});

			const Vector3 pos[6] =
			{
				{ 0, 0, 0 },
				{ 1, 0, 0 },
				{ 1, 1, 0 },
				{ 0, 0, 0 },
				{ 1, 1, 0 },
				{ 0, 1, 0 },
			};

			m_quadBuffer = fac->CreateVertexBuffer(6, m_quadVtxDecl, BU_Static);
			void* vtxData = m_quadBuffer->Lock(LOCK_None);
			memcpy(vtxData, pos, sizeof(pos));

			m_quadBuffer->Unlock();
		}


		ScenePass::~ScenePass()
		{
			delete m_quadBuffer;
			delete m_quadVtxDecl;
		}
		
		
		void ScenePass::Invoke(const List<Camera*>& cameras, SceneManager* sceMgr, BatchData* batchData)
		{
			//uint64 selectorMask = 1<<m_selectorID;
			if (m_renderer->GlobalCameraOverride != -1)
			{
				m_currentCamera = cameras[m_renderer->GlobalCameraOverride];
			}
			else
			{
				m_currentCamera = cameras[m_cameraID];
			}
			RendererEffectParams::CurrentCamera = m_currentCamera;

			if (m_currentCamera != m_parentProc->getLastCamera())
			{
				// only re add visible objects into table when the camera is changed.
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
					{
						ExecutionValue val1 = m_execStack.Pop();
						ExecutionValue val2 = m_execStack.Pop();
						ExecutionValue result;

						result.Value[0] = val1.Value[0] & val2.Value[0];
						result.Value[1] = val1.Value[1] & val2.Value[1];

						m_execStack.Push(result);
						break;
					}
					case SOP_Or:
					{
						ExecutionValue val1 = m_execStack.Pop();
						ExecutionValue val2 = m_execStack.Pop();
						ExecutionValue result;

						result.Value[0] = val1.Value[0] | val2.Value[0];
						result.Value[1] = val1.Value[1] | val2.Value[1];

						m_execStack.Push(result);
						break;
					}
					case SOP_Not:
					{
						ExecutionValue val = m_execStack.Pop();
						ExecutionValue result;
						result.Value[0] = !val.Value[0];
						result.Value[1] = !val.Value[1];
						m_execStack.Push(result);
						break;
					}
					case SOP_Load:
					{
						ExecutionValue val;
						if (inst.Args[0].IsImmediate)
						{
							val.Value[0] = inst.Args[0].DefaultValue[0];
							val.Value[1] = inst.Args[0].DefaultValue[1];
						}
						else
						{
							val.Value[0] = inst.Args[0].Var->Value[0];
							val.Value[1] = inst.Args[0].Var->Value[1];
						}

						m_execStack.Push(val);
						break;
					}
					case SOP_JNZ:
					{
						assert(m_execStack.getCount());
						ExecutionValue val = m_execStack.Pop();
						if (val.Value[0] || val.Value[1])
						{
							i = inst.Next - 1;
						}
						break;
					}
					case SOP_JZ:
					{
						assert(m_execStack.getCount());
						ExecutionValue val = m_execStack.Pop();
						if (!val.Value[0] && !val.Value[1])
						{
							i = inst.Next - 1;
						}
						break;
					}
					case SOP_Clear:
						Clear(inst);
						break;
					case SOP_Render:
						m_renderer->RenderBatch(m_selectorID);
						break;
					case SOP_RenderQuad:
						RenderQuad(inst);
						break;
					case SOP_VisibleTo:
					{
						int selectorID;
						if (inst.Args[0].IsImmediate)
						{
							selectorID = reinterpret_cast<const int&>(inst.Args[0].DefaultValue[0]);
						}
						else
						{
							selectorID = reinterpret_cast<const int&>(inst.Args[0].Var->Value[0]);
						}
						uint64 selectMask = 1ULL << selectorID;
						bool result = batchData->HasObject(selectMask);
						inst.Args[1].Var->Value[0] = result ? 1 : 0;
						break;
					}

					case SOP_UseDS:
						UseDS(inst);
						break;

					case SOP_UseRT:
						UseRT(inst);
						break;
				}
			}
		}

		void ScenePass::Clear(const SceneInstruction& inst)
		{
			int flags=0;

			// retrieve the arguments
			if (inst.Args[0].IsImmediate)
			{
				if (inst.Args[0].DefaultValue[0])
				{
					flags |= CLEAR_ColorBuffer;
				}
			}
			else if (inst.Args[0].Var->Value[0])
			{
				flags |= CLEAR_ColorBuffer;
			}

			if (inst.Args[1].IsImmediate)
			{
				if (inst.Args[1].DefaultValue[0])
				{
					flags |= CLEAR_DepthBuffer;
				}
			}
			else if (inst.Args[1].Var->Value[0])
			{
				flags |= CLEAR_DepthBuffer;
			}

			if (inst.Args[2].IsImmediate)
			{
				if (inst.Args[2].DefaultValue[0])
				{
					flags |= CLEAR_Stencil;
				}
			}
			else if (inst.Args[2].Var->Value[0])
			{
				flags |= CLEAR_Stencil;
			}




			float depth;
			if (inst.Args[3].IsImmediate)
			{
				depth = reinterpret_cast<const float&>(inst.Args[3].DefaultValue[0]);
			}
			else
			{
				depth = reinterpret_cast<const float&>(inst.Args[3].Var->Value[0]);
			}

			int stencil;
			if (inst.Args[4].IsImmediate)
			{
				stencil = reinterpret_cast<const int&>(inst.Args[4].DefaultValue[0]);
			}
			else
			{
				stencil = reinterpret_cast<const int&>(inst.Args[4].Var->Value[0]);
			}

			ColorValue color;
			if (inst.Args[5].IsImmediate)
			{
				color = reinterpret_cast<const ColorValue&>(inst.Args[5].DefaultValue[0]);
			}
			else
			{
				color = reinterpret_cast<const ColorValue&>(inst.Args[5].Var->Value[0]);
			}
			m_renderDevice->Clear(static_cast<ClearFlags>(flags), color, depth, stencil);
		}
		void ScenePass::RenderQuad(const SceneInstruction& inst)
		{
			float xScale = reinterpret_cast<const float&>(inst.Args[0].DefaultValue[0]);
			float yScale = reinterpret_cast<const float&>(inst.Args[0].DefaultValue[1]);

			Effect* effect;
			memcpy(&effect, inst.Args[1].DefaultValue, sizeof(void*));

			Material mtrl(m_renderDevice);
			mtrl.Cull = CullMode::None;
			//mtrl.IsBlendTransparent = true;

			// the post here is expected to be an AutomaticEffect
			AutomaticEffect* autoFx = up_cast<AutomaticEffect*>(effect);

			// assign the parameters
			for (int32 i=2;i<inst.Args.getCount();i++)
			{
				const SceneOpArg& arg = inst.Args[i];
				if (arg.IsImmediate)
				{
					// Check the type of this immediate value.
					if ((arg.DefaultValue[0] & arg.DefaultValue[1] ) == 0xffffffff)
					{
						// the type is material
						if (arg.StrData == L"IsBlendTransparent")
						{
							mtrl.IsBlendTransparent = arg.DefaultValue[15] ? true : false;
						}
						else if (arg.StrData == L"DepthTest")
						{
							mtrl.DepthTestEnabled = arg.DefaultValue[15] ? true : false;
						}

					}
					else
					{

						int idx = static_cast<int>(arg.DefaultValue[0]);
						int count = static_cast<int>(arg.DefaultValue[1] & 0xffff);
						ScenePostEffectParamType type = 
							static_cast<ScenePostEffectParamType>(arg.DefaultValue[1] >> 16);

						switch (type)
						{
						case SPFX_TYPE_BOOLS:
							autoFx->SetParameterValue(idx, reinterpret_cast<const bool*>(&arg.DefaultValue[2]), count);
							break;
						case SPFX_TYPE_FLOATS:
							autoFx->SetParameterValue(idx, reinterpret_cast<const float*>(&arg.DefaultValue[2]), count);
							break;
						case SPFX_TYPE_INTS:
							autoFx->SetParameterValue(idx, reinterpret_cast<const int*>(&arg.DefaultValue[2]), count);
							break;
						}
					}
				}
				else
				{
					SceneVariable* var = arg.Var;
					assert(var);

					//bool propNotFountErr = false;
					int idx = static_cast<int>(arg.DefaultValue[15]);
					switch (var->Type)
					{
						case SceneVariableType::Boolean:
							autoFx->SetParameterValue(idx, reinterpret_cast<const bool*>(var->Value), 1);
							break;
						case SceneVariableType::RenderTarget:
						{
							if (arg.StrData.size())
							{
								if (arg.StrData == L"Width")
								{
									int width = var->RTValue->getWidth();
									autoFx->SetParameterValue(idx, &width, 1);
								}
								else if (arg.StrData == L"Height")
								{
									int height = var->RTValue->getHeight();
									autoFx->SetParameterValue(idx, &height, 1);
								}
								else
								{
									LogManager::getSingleton().Write(LOG_Scene, L"Property " + arg.StrData + L" is not found on " + var->Name, LOGLVL_Error);
								}
							}
							else
							{
								autoFx->SetParameterTexture(idx, var->RTValue->GetColorTexture());
							}
							break;
						}
						case SceneVariableType::Matrix:
							//if (arg.StrData.size())
							//{
							//}
							//else
						{
							autoFx->SetParameterValue(idx, reinterpret_cast<const float*>(var->Value), 16);
						}
						break;
						case SceneVariableType::Vector4:
							autoFx->SetParameterValue(idx, reinterpret_cast<const float*>(var->Value), 4);
							break;
						case SceneVariableType::Vector3:
							autoFx->SetParameterValue(idx, reinterpret_cast<const float*>(var->Value), 3);
							break;
						case SceneVariableType::Vector2:
							autoFx->SetParameterValue(idx, reinterpret_cast<const float*>(var->Value), 2);
							break;
						case SceneVariableType::Single:
							autoFx->SetParameterValue(idx, reinterpret_cast<const float*>(var->Value), 1);
							break;
						case SceneVariableType::Texture:
							autoFx->SetParameterTexture(idx, var->TextureValue);
							break;

						case SceneVariableType::Integer:
							autoFx->SetParameterValue(idx, reinterpret_cast<const int*>(var->Value), 1);
							break;
						case SceneVariableType::GaussBlurFilter:
							if (arg.StrData.size())
							{
								GaussBlurFilter* filter = static_cast<GaussBlurFilter*>(var->ObjectValue);
								if (arg.StrData == L"SampleOffsetsX")
								{
									const Vector4* weights = filter->getSampleOffsetX();
									autoFx->SetParameterValue(idx, reinterpret_cast<const float*>(weights), 4 * filter->getSampleCount());
								}
								else if (arg.StrData == L"SampleOffsetsY")
								{
									const Vector4* weights = filter->getSampleOffsetY();
									autoFx->SetParameterValue(idx, reinterpret_cast<const float*>(weights), 4 * filter->getSampleCount());
								}
								else if (arg.StrData == L"SampleWeights")
								{
									const float* weights = filter->getSampleWeights();
									memset(m_floatBuffer, 0, sizeof(m_floatBuffer));
									for (int j = 0; j < filter->getSampleCount(); j++)
									{
										assert((j * 4 + 3) < (sizeof(m_floatBuffer) / sizeof(m_floatBuffer[0])));
										m_floatBuffer[j * 4] = weights[j];
										m_floatBuffer[j * 4 + 1] = weights[j];
										m_floatBuffer[j * 4 + 2] = weights[j];
										m_floatBuffer[j * 4 + 3] = weights[j];

									}
									autoFx->SetParameterValue(idx, m_floatBuffer, filter->getSampleCount() * 4);
								}
								else if (arg.StrData == L"SampleCount")
								{
									int val = filter->getSampleCount();
									autoFx->SetParameterValue(idx, &val, 1);
								}
								else
								{
									//arg.StrData = L"";
									LogManager::getSingleton().Write(LOG_Scene, L"Property " + arg.StrData + L" is not found on " + var->Name, LOGLVL_Error);
								}
							}

							break;

					}
				}

			}

			mtrl.SetPassEffect(m_selectorID, effect);
			mtrl.setPassFlags((uint64)1<<(uint64)m_selectorID);
			
			//mtrl.IsBlendTransparent = true;

			GeometryData geoData;
			geoData.BaseVertex = 0;
			geoData.IndexBuffer = 0;
			geoData.PrimitiveCount = 2;
			geoData.PrimitiveType = PrimitiveType::TriangleList;
			geoData.VertexBuffer = m_quadBuffer;
			geoData.VertexCount = 6;
			geoData.VertexDecl = m_quadVtxDecl;
			geoData.VertexSize = m_quadVtxDecl->GetVertexSize();

			RenderOperation rop;
			rop.Material = &mtrl;
			rop.GeometryData = &geoData;
			
			Matrix::CreateScale(rop.RootTransform, xScale, yScale, 1);


			m_renderDevice->Render(&mtrl, &rop, 1, m_selectorID);

		}
		void ScenePass::UseRT(const SceneInstruction& inst)
		{
			assert(inst.Args.getCount() == 2);

			int32 index;
			if (inst.Args[0].IsImmediate)
			{
				index = reinterpret_cast<const int32&>(inst.Args[0].DefaultValue[0]);
			}
			else
			{
				index = reinterpret_cast<const int32&>(inst.Args[0].Var->Value[0]);
			}

			if (!inst.Args[1].IsImmediate)
			{
				m_renderDevice->SetRenderTarget(index, inst.Args[1].Var->RTValue);
			}
			else
			{
				// if Immediate then it can only be null
				m_renderDevice->SetRenderTarget(index, nullptr);
			}
		}

		void ScenePass::UseDS(const SceneInstruction& inst)
		{
			assert(inst.Args.getCount() == 1);

			if (!inst.Args[0].IsImmediate)
			{
				m_renderDevice->SetDepthStencilBuffer(inst.Args[0].Var->DSValue);
			}
			else
			{
				// if Immediate then it can only be null
				m_renderDevice->SetDepthStencilBuffer(nullptr);
			}
		}
	};
};