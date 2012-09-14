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
#include "Core/Logging.h"
#include "Graphics/RenderSystem/RenderDevice.h"
#include "Graphics/RenderSystem/RenderTarget.h"
#include "Graphics/RenderSystem/ObjectFactory.h"
#include "Graphics/RenderSystem/Buffer/HardwareBuffer.h"
#include "Graphics/RenderSystem/VertexDeclaration.h"
#include "Graphics/RenderSystem/VertexElement.h"
#include "Graphics/EffectSystem/EffectParameter.h"
#include "Graphics/EffectSystem/Effect.h"
#include "Graphics/Material.h"
#include "Graphics/GeometryData.h"
#include "Math/GaussBlurFilter.h"
#include "Math/Math.h"

using namespace Apoc3D::Core;
using namespace Apoc3D::Math;

namespace Apoc3D
{
	namespace Scene
	{
		/** Vertex structure for screen quad
		 *  The vertices are in screen space; the tex coords will be calculated in vertex shader
		 */
		struct QuadVertex
		{
			float Position[3];
		};

		ScenePass::ScenePass(RenderDevice* device, SceneRenderer* renderer, SceneProcedure* parnetProc, const ScenePassData* passData)
			: m_parentProc(parnetProc), m_cameraID(passData->CameraID), m_name(passData->Name), m_selectorID(passData->SelectorID),
			m_renderDevice(device), m_renderer(renderer)
		{
			for (size_t i=0;i<passData->Instructions.size();i++)
			{
				m_instuctions.Add(passData->Instructions[i]);
			}

			// generate the quad vertices
			ObjectFactory* fac = device->getObjectFactory();


			FastList<VertexElement> elements;
			elements.Add(VertexElement(0, VEF_Vector3, VEU_Position));
			m_quadVtxDecl = fac->CreateVertexDeclaration(elements);

			m_quadBuffer = fac->CreateVertexBuffer(6, m_quadVtxDecl, BU_Static);
			QuadVertex* vtxData = reinterpret_cast<QuadVertex*>(m_quadBuffer->Lock(LOCK_None));
			vtxData[0].Position[0] = 0; vtxData[0].Position[1] = 0; vtxData[0].Position[2] = 0;
			vtxData[1].Position[0] = 1; vtxData[1].Position[1] = 0; vtxData[1].Position[2] = 0;
			vtxData[2].Position[0] = 1; vtxData[2].Position[1] = 1; vtxData[2].Position[2] = 0;
			
			vtxData[3].Position[0] = 0; vtxData[3].Position[1] = 0; vtxData[3].Position[2] = 0;
			vtxData[4].Position[0] = 1; vtxData[4].Position[1] = 1; vtxData[4].Position[2] = 0;
			vtxData[5].Position[0] = 0; vtxData[5].Position[1] = 1; vtxData[5].Position[2] = 0;

			m_quadBuffer->Unlock();


		}


		ScenePass::~ScenePass(void)
		{
			delete m_quadBuffer;
			delete m_quadVtxDecl;
		}
		
		
		void ScenePass::Invoke(const FastList<Camera*>& cameras, SceneManager* sceMgr, BatchData* batchData)
		{
			//uint64 selectorMask = 1<<m_selectorID;
			m_currentCamera = cameras[m_cameraID];
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
					}
					break;
				case SOP_Or:
					{
						ExecutionValue val1 = m_execStack.Pop();
						ExecutionValue val2 = m_execStack.Pop();
						ExecutionValue result;

						result.Value[0] = val1.Value[0] | val2.Value[0];
						result.Value[1] = val1.Value[1] | val2.Value[1];

						m_execStack.Push(result);
					}
					break;
				case SOP_Not:
					{
						ExecutionValue val = m_execStack.Pop();
						ExecutionValue result;
						result.Value[0] = !val.Value[0];
						result.Value[1] = !val.Value[1];
						m_execStack.Push(result);
					}
					break;
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
					}
					break;
				case SOP_JNZ:
					{
						assert(m_execStack.getCount());
						ExecutionValue val = m_execStack.Pop();
						if (val.Value[0] || val.Value[1])
						{
							i = inst.Next-1;
						}
					}
					break;
				case SOP_JZ:
					{
						assert(m_execStack.getCount());
						ExecutionValue val = m_execStack.Pop();
						if (!val.Value[0] && !val.Value[1])
						{
							i = inst.Next-1;
						}
					}
					break;
				case SOP_Clear:
					Clear(inst);
					break;
				case SOP_Render:
					{
						m_renderer->RenderBatch(m_selectorID);
					}
					break;
				case SOP_RenderQuad:
					RenderQuad(inst);
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
							selectorID = reinterpret_cast<const int&>(inst.Args[0].Var->Value[0]);
						}
						uint64 selectMask = 1 << m_selectorID;
						result = batchData->HasObject(selectMask);
						inst.Args[1].Var->Value[0] = result ? 1 : 0;
					}
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
			mtrl.Cull = CULL_None;
			//mtrl.IsBlendTransparent = true;

			// the post here is expected to be an AutomaticEffect
			AutomaticEffect* autoFx = dynamic_cast<AutomaticEffect*>(effect);

			// assign the parameters
			for (size_t i=2;i<inst.Args.size();i++)
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

					bool propNotFountErr = false;
					int idx = static_cast<int>(arg.DefaultValue[15]);
					switch (var->Type)
					{
					case VARTYPE_Boolean:
						autoFx->SetParameterValue(idx, reinterpret_cast<const bool*>(var->Value), 1);
						break;
					case VARTYPE_RenderTarget:
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
								//arg.StrData = L"";
								LogManager::getSingleton().Write(LOG_Scene, L"Property " + arg.StrData + L" is not found on " + var->Name, LOGLVL_Error);
							}
						}
						else
						{
							autoFx->SetParameterTexture(idx, var->RTValue->GetColorTexture());
						}
						break;
					case VARTYPE_Matrix:
						//if (arg.StrData.size())
						//{
						//}
						//else
						{
							autoFx->SetParameterValue(idx, reinterpret_cast<const float*>(var->Value), 16);
						}
						break;
					case VARTYPE_Vector4:
						autoFx->SetParameterValue(idx, reinterpret_cast<const float*>(var->Value), 4);
						break;
					case VARTYPE_Vector3:
						autoFx->SetParameterValue(idx, reinterpret_cast<const float*>(var->Value), 3);
						break;
					case VARTYPE_Vector2:
						autoFx->SetParameterValue(idx, reinterpret_cast<const float*>(var->Value), 2);
						break;
					case VARTYPE_Texture:
						autoFx->SetParameterTexture(idx, var->TextureValue);
						break;
							
					case VARTYPE_Integer:
						autoFx->SetParameterValue(idx, reinterpret_cast<const int*>(var->Value), 1);
						break;
					case VARTYPE_GaussBlurFilter:
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
								memset(m_floatBuffer,0, sizeof(m_floatBuffer));
								for (int j=0;j<filter->getSampleCount();j++)
								{
									assert((j*4+3) < (sizeof(m_floatBuffer) / sizeof(m_floatBuffer[0])) );
									m_floatBuffer[j*4   ] = weights[j];
									m_floatBuffer[j*4+1 ] = weights[j];
									m_floatBuffer[j*4+2 ] = weights[j];
									m_floatBuffer[j*4+3 ] = weights[j];
									
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

			mtrl.setPassEffect(m_selectorID, effect);
			mtrl.setPassFlags((uint64)1<<(uint64)m_selectorID);
			
			//mtrl.IsBlendTransparent = true;

			GeometryData geoData;
			geoData.BaseVertex = 0;
			geoData.IndexBuffer = 0;
			geoData.PrimitiveCount = 2;
			geoData.PrimitiveType = PT_TriangleList;
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
			int index;
			if (inst.Args[0].IsImmediate)
			{
				index = reinterpret_cast<const int&>(inst.Args[0].DefaultValue[0]);
			}
			else
			{
				index = reinterpret_cast<const int&>(inst.Args[0].Var->Value[0]);
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
	};
};