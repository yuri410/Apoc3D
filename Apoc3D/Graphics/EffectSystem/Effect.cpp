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

#include "Effect.h"

#include "apoc3d/Core/AppTime.h"
#include "apoc3d/Core/Logging.h"
#include "apoc3d/Core/Resource.h"
#include "apoc3d/Core/ResourceHandle.h"
#include "apoc3d/Graphics/Camera.h"
#include "apoc3d/Graphics/Material.h"
#include "apoc3d/Graphics/RenderSystem/InstancingData.h"
#include "apoc3d/Graphics/RenderSystem/RenderDevice.h"
#include "apoc3d/Graphics/RenderSystem/Shader.h"
#include "apoc3d/Graphics/RenderSystem/Texture.h"
#include "apoc3d/Graphics/TextureManager.h"
#include "apoc3d/IOLib/BinaryReader.h"
#include "apoc3d/IOLib/EffectData.h"
#include "apoc3d/IOLib/Streams.h"
#include "apoc3d/Math/Matrix.h"
#include "apoc3d/Vfs/FileSystem.h"
#include "apoc3d/Vfs/ResourceLocation.h"
#include "apoc3d/Vfs/FileLocateRule.h"

using namespace Apoc3D::Math;
using namespace Apoc3D::IO;

namespace Apoc3D
{
	namespace Graphics
	{
		namespace EffectSystem
		{
			Effect::Effect()
			{
			}


			Effect::~Effect()
			{
			}

			int Effect::Begin()
			{
				if (!m_begun)
				{
					m_begun = true;
					return begin();
				}
				return -1;
			}

			void Effect::End()
			{
				if (m_begun)
				{
					end();
					m_begun = false;
				}
			}

			/*
			Shader* Effect::LoadVertexShader(RenderDevice* rs, const ResourceLocation& vs)
			{
				ObjectFactory* objFac = rs->getObjectFactory();
				return objFac->CreateVertexShader(vs);
			}
			Shader* Effect::LoadPixelShader(RenderDevice* rs, const ResourceLocation& ps)
			{
				ObjectFactory* objFac = rs->getObjectFactory();
				return objFac->CreatePixelShader(ps);
			}*/

			/************************************************************************/
			/*  AutomaticEffect                                                     */
			/************************************************************************/

			static Matrix s_instancingMatrixBuffer[InstancingData::MaxOneTimeInstances];
			static float s_instancingFloatBuffer[InstancingData::MaxOneTimeInstances];
			static float s_instancingVector2Buffer[InstancingData::MaxOneTimeInstances*2];
			static Vector4 s_instancingVector4Buffer[InstancingData::MaxOneTimeInstances];
			
			
			template void AutomaticEffect::SetParameterValue<bool>(int32 index, const bool* value, int32 count);
			template void AutomaticEffect::SetParameterValue<int>(int32 index, const int* value, int32 count);
			template void AutomaticEffect::SetParameterValue<float>(int32 index, const float* value, int32 count);



			AutomaticEffect::AutomaticEffect(RenderDevice* device, const ResourceLocation& rl)
				: m_device(device)
			{
				Reload(rl);

				ObjectFactory* objFac = m_device->getObjectFactory();
				m_whiteTexture = objFac->CreateTexture(1, 1, 1, TU_Static, FMT_A8R8G8B8);
				m_whiteTexture->FillColor(CV_White);
			}

			AutomaticEffect::~AutomaticEffect()
			{
				for (ResolvedEffectParameter& ep : m_parameters)
				{
					ep.Free();
				}

				DELETE_AND_NULL(m_vertexShader);
				DELETE_AND_NULL(m_pixelShader);
				DELETE_AND_NULL(m_whiteTexture);
			}

			bool AutomaticEffect::SupportsInstancing()
			{
				return m_supportsInstancing;
			}

			void AutomaticEffect::Reload(const ResourceLocation& rl)
			{
				DELETE_AND_NULL(m_vertexShader);
				DELETE_AND_NULL(m_pixelShader);

				m_parameters.Clear();
				m_supportsInstancing = false;
				m_isUnsupported = false;

				EffectData data;
				data.Load(rl);
				m_name = data.Name;
				assert(!data.IsCFX);

				Capabilities* caps = m_device->getCapabilities();

				const EffectProfileData* profileSelected = nullptr;
				for (const EffectProfileData& pd : data.Profiles)
				{
					if (caps->SupportsVertexShader(pd.ImplementationType, pd.MajorVer, pd.MinorVer) && 
						caps->SupportsPixelShader(pd.ImplementationType, pd.MajorVer, pd.MinorVer))
					{
						profileSelected = &pd;
					}
				}

				if (profileSelected == nullptr)
				{
					ApocLog(LOG_Graphics, L"Effect not supported: " + m_name, LOGLVL_Warning);
					m_isUnsupported = true;
					return;
				}

				ObjectFactory* objFac = m_device->getObjectFactory();
				m_vertexShader = objFac->CreateVertexShader(reinterpret_cast<const byte*>(profileSelected->VSCode));
				m_pixelShader = objFac->CreatePixelShader(reinterpret_cast<const byte*>(profileSelected->PSCode));
				
				m_vertexShader->NotifyLinkage({ m_vertexShader, m_pixelShader });

				bool hasShaderIssues = false;

				m_parametersSrc = profileSelected->Parameters;
				m_parameters.ResizeDiscard(profileSelected->Parameters.getCount());
				for (const EffectParameter& srcEp : m_parametersSrc)
				{
					Shader* shader = nullptr;
					if (srcEp.ProgramType == ShaderType::Vertex)
					{
						shader = m_vertexShader;
					}
					else if (srcEp.ProgramType == ShaderType::Pixel)
					{
						shader = m_pixelShader;
					}

					assert(shader);

					ResolvedEffectParameter rep(m_device, m_name, &srcEp, shader, hasShaderIssues);
					m_parameters.Add(rep);
				}
				
				// instancing check
				for (const EffectParameter& srcEp : m_parametersSrc)
				{
					if (srcEp.Usage == EPUSAGE_Trans_InstanceWorlds)
					{
						m_supportsInstancing = true;
						break;
					}
				}

				if (hasShaderIssues)
				{
					ApocLog(LOG_Graphics, L"[AutomaticEffect][" + m_name + L"] Effect disabled due to invalid data.", LOGLVL_Warning);
					m_isUnsupported = true;
				}
			}


			void AutomaticEffect::Update(const AppTime* time)
			{
				float t = m_lastTime + time->ElapsedTime;
				m_unifiedTime += t - m_lastTime;
				m_lastTime = t;

				while (m_unifiedTime > Math::PI * 10)
				{
					m_unifiedTime -= Math::PI * 10;
				}
			}

			void AutomaticEffect::Setup(Material* mtrl, const RenderOperation* rop, int32 count)
			{
				// set the param's value one by one. 

				// the material for all the render operations in the list is the same.
				// it is better to be set only once. by checking m_previousMaterialPointer

				bool materialChanged = m_previousMaterialPointer != mtrl;
				m_previousMaterialPointer = mtrl;

				for (int32 i = 0; i < m_parameters.getCount();i++)
				{
					const ResolvedEffectParameter& ep = m_parameters[i];

					if (ep.RS_SetupAtBegining && ep.RS_SetupAtBeginingOnly)
						continue;

					switch (ep.Usage)
					{
						case EPUSAGE_MtrlC4_Ambient:
							if (materialChanged)
								ep.SetColor4(mtrl->Ambient);
							break;
						case EPUSAGE_MtrlC4_Diffuse:
							if (materialChanged)
								ep.SetColor4(mtrl->Diffuse);
							break;
						case EPUSAGE_MtrlC4_Emissive:
							if (materialChanged)
								ep.SetColor4(mtrl->Emissive);
							break;
						case EPUSAGE_MtrlC4_Specular:
							if (materialChanged)
								ep.SetColor4(mtrl->Specular);
							break;
						case EPUSAGE_MtrlC_Power:
							if (materialChanged)
								ep.SetFloat(mtrl->Power);
							break;
						case EPUSAGE_Tex0:
						case EPUSAGE_Tex1:
						case EPUSAGE_Tex2:
						case EPUSAGE_Tex3:
						case EPUSAGE_Tex4:
						case EPUSAGE_Tex5:
						case EPUSAGE_Tex6:
						case EPUSAGE_Tex7:
						case EPUSAGE_Tex8:
						case EPUSAGE_Tex9:
						case EPUSAGE_Tex10:
						case EPUSAGE_Tex11:
						case EPUSAGE_Tex12:
						case EPUSAGE_Tex13:
						case EPUSAGE_Tex14:
						case EPUSAGE_Tex15:
							if (materialChanged)
							{
								SetTexture(ep, mtrl->getTexture(ep.Usage - EPUSAGE_Tex0));
							}
							break;
						case EPUSAGE_Trans_WorldViewProj:
							if (RendererEffectParams::CurrentCamera)
							{
								Matrix mvp;
								Matrix::Multiply(mvp, rop->RootTransform, RendererEffectParams::CurrentCamera->getViewProjMatrix());

								ep.SetMatrix(mvp);
							}
							break;
						case EPUSAGE_Trans_WorldView:
							if (RendererEffectParams::CurrentCamera)
							{
								Matrix mv;
								Matrix::Multiply(mv, rop->RootTransform, RendererEffectParams::CurrentCamera->getViewMatrix());

								ep.SetMatrix(mv);
							}
							break;
						case EPUSAGE_Trans_World:
							ep.SetMatrix(rop->RootTransform);
							break;
						case EPUSAGE_Trans_WorldViewOriProj:
							if (RendererEffectParams::CurrentCamera)
							{
								Matrix view = RendererEffectParams::CurrentCamera->getViewMatrix();
								view.SetTranslation(Vector3::Zero);

								Matrix temp;
								Matrix::Multiply(temp, rop->RootTransform, view);

								Matrix mvp;
								Matrix::Multiply(mvp, temp, RendererEffectParams::CurrentCamera->getProjMatrix());

								ep.SetMatrix(mvp);
							}
							break;
						case EPUSAGE_Trans_InstanceWorlds:
						{
							int32 legalCount = Math::Min(count, InstancingData::MaxOneTimeInstances);

							for (int32 i = 0; i < legalCount; i++)
							{
								s_instancingMatrixBuffer[i] = rop[i].RootTransform;
							}

							ep.SetMatrix(s_instancingMatrixBuffer, count);
							break;
						}

						case EPUSAGE_M4X3_BoneTrans:
							ep.Set4X3Matrix(rop->PartTransform.Transfroms, rop->PartTransform.Count);
							break;
						case EPUSAGE_M4X4_BoneTrans:
							ep.SetMatrix(rop->PartTransform.Transfroms, rop->PartTransform.Count);
							break;

						case EPUSAGE_InstanceBlob:
							if (rop->UserData)
							{
								const InstanceInfoBlob& firstBlob = *reinterpret_cast<const InstanceInfoBlob*>(rop->UserData);

								if (m_supportsInstancing)
								{
									CustomEffectParameterType paramType = firstBlob[ep.InstanceBlobIndex].Type;

									int32 legalCont = Math::Min(count, InstancingData::MaxOneTimeInstances);

									bool inValidType = false;
									switch (paramType)
									{
										case CEPT_Float:
										{
											for (int32 i = 0; i < legalCont; i++)
												s_instancingFloatBuffer[i] = (*reinterpret_cast<const InstanceInfoBlob*>(rop[i].UserData))[ep.InstanceBlobIndex].AsSingle();

											int32 alignedCount = (legalCont + 3) & ~0x03;
											ep.SetFloat(s_instancingFloatBuffer, alignedCount);
											break;

										}

										case CEPT_Vector2:
										case CEPT_Ref_Vector2:
										{
											Vector2* dst = reinterpret_cast<Vector2*>(s_instancingVector2Buffer);

											if (paramType == CEPT_Ref_Vector2)
											{
												for (int32 i = 0; i < legalCont; i++)
													dst[i] = *(*reinterpret_cast<const InstanceInfoBlob*>(rop[i].UserData))[ep.InstanceBlobIndex].AsVector2Ref();
											}
											else
											{
												for (int32 i = 0; i < legalCont; i++)
													dst[i] = (*reinterpret_cast<const InstanceInfoBlob*>(rop[i].UserData))[ep.InstanceBlobIndex].AsVector2();
											}

											int32 alignedCount = (legalCont * 2 + 3) & ~0x03;
											ep.SetFloat(s_instancingVector2Buffer, alignedCount);
											break;
										}
										case CEPT_Vector4:
											for (int32 i = 0; i < legalCont; i++)
												s_instancingVector4Buffer[i] = (*reinterpret_cast<const InstanceInfoBlob*>(rop[i].UserData))[ep.InstanceBlobIndex].AsVector4();
											ep.SetVector4(s_instancingVector4Buffer, count);
											break;
										case CEPT_Ref_Vector4:
											for (int32 i = 0; i < legalCont; i++)
												s_instancingVector4Buffer[i] = *(*reinterpret_cast<const InstanceInfoBlob*>(rop[i].UserData))[ep.InstanceBlobIndex].AsVector4Ref();
											ep.SetVector4(s_instancingVector4Buffer, count);
											break;

										case CEPT_Matrix:
											for (int32 i = 0; i < legalCont; i++)
												s_instancingMatrixBuffer[i] = (*reinterpret_cast<const InstanceInfoBlob*>(rop[i].UserData))[ep.InstanceBlobIndex].AsMatrix();
											ep.SetMatrix(s_instancingMatrixBuffer, count);
											break;

										case CEPT_Ref_Vector3:
											for (int32 i = 0; i < legalCont; i++)
												s_instancingVector4Buffer[i] = Vector4(*(*reinterpret_cast<const InstanceInfoBlob*>(rop[i].UserData))[ep.InstanceBlobIndex].AsVector3Ref(), 0);
											ep.SetVector4(s_instancingVector4Buffer, count);
											break;

										case CEPT_Boolean:
										case CEPT_Integer:
										case CEPT_Ref_TextureHandle:
										case CEPT_Ref_Texture:
											inValidType = true;
											break;
									}
									if (inValidType)
									{
										LogManager::getSingleton().Write(LOG_Graphics,
											L"[" + m_name + L"] Instanced InfoBlob parameter " + ep.ReferenceSource->Name + L" type not valid. ", LOGLVL_Warning);
									}
								}
								else
								{
									SetInstanceBlobParameter(ep, firstBlob[ep.InstanceBlobIndex]);
								}
							}
							else
							{
								LogManager::getSingleton().Write(LOG_Graphics, L"[" + m_name + L"] No InfoBlob Obtained.", LOGLVL_Error);
							}
							break;

						case EPUSAGE_CustomMaterialParam:
							if (materialChanged)
							{
								if (mtrl && ep.ReferenceSource->CustomMaterialParamName.size())
								{
									SetMaterialCustomParameter(ep, mtrl);
								}
							}
							break;
					}
				}
			}
			void AutomaticEffect::BeginPass(int32 passId)
			{

			}
			void AutomaticEffect::EndPass()
			{

			}

			void AutomaticEffect::SetParameterValue(int32 index, const Vector2* value, int32 count)
			{
				ResolvedEffectParameter& param = m_parameters[index];
				param.RS_TargetShader->SetVector2(param.RegisterIndex, value, count);
			}
			void AutomaticEffect::SetParameterValue(int32 index, const Vector3* value, int32 count)
			{
				ResolvedEffectParameter& param = m_parameters[index];
				param.RS_TargetShader->SetVector3(param.RegisterIndex, value, count);
			}
			void AutomaticEffect::SetParameterValue(int32 index, const Vector4* value, int32 count)
			{
				ResolvedEffectParameter& param = m_parameters[index];
				param.SetVector4(value, count);
			}
			void AutomaticEffect::SetParameterValue(int32 index, const Matrix* value, int32 count)
			{
				ResolvedEffectParameter& param = m_parameters[index];
				param.SetMatrix(value, count);
			}


			void AutomaticEffect::SetParameterTexture(int32 index, ResourceHandle<Texture>* value)
			{
				ResolvedEffectParameter& param = m_parameters[index];
				Texture* tex = nullptr;

				if (value)
					tex = value->ObtainLoaded();

				if (tex == nullptr)
					tex = m_whiteTexture;

				param.RS_TargetShader->SetTexture(param.SamplerIndex, tex);
			}

			void AutomaticEffect::SetParameterTexture(int32 index, Texture* tex)
			{
				ResolvedEffectParameter& param = m_parameters[index];
				
				param.RS_TargetShader->SetTexture(param.SamplerIndex, tex);
			}


			int32 AutomaticEffect::FindParameterIndex(const String& name)
			{
				for (int32 i = 0; i < m_parametersSrc.getCount(); i++)
				{
					if (m_parametersSrc[i].Name == name)
						return i;
				}
				return -1;
			}

			void AutomaticEffect::SetParameterTextureByName(const String& name, ResourceHandle<Texture>* value)
			{
				int32 idx = FindParameterIndex(name);
				assert(idx != -1);
				SetParameterTexture(idx, value);
			}
			void AutomaticEffect::SetParameterTextureByName(const String& name, Texture* value)
			{
				int32 idx = FindParameterIndex(name);
				assert(idx != -1);
				SetParameterTexture(idx, value);
			}


			int AutomaticEffect::begin()
			{
				if (m_isUnsupported)
					return 0;

				// non render operation specific parameters will be set here
				m_previousMaterialPointer = nullptr;
				
				m_device->BindPixelShader(m_pixelShader);
				m_device->BindVertexShader(m_vertexShader);

				for (int i = 0; i < m_parameters.getCount(); i++)
				{
					ResolvedEffectParameter& ep = m_parameters[i];

					if (!ep.RS_SetupAtBegining)
						continue;

					if (ep.SamplerIndex != -1)
						ep.SetSamplerState();

					switch (ep.Usage)
					{
						case EPUSAGE_LC4_Ambient:  ep.SetColor4(RendererEffectParams::LightAmbient); break;
						case EPUSAGE_LC4_Diffuse:  ep.SetColor4(RendererEffectParams::LightDiffuse); break;
						case EPUSAGE_LC4_Specular: ep.SetColor4(RendererEffectParams::LightSpecular); break;
						case EPUSAGE_LV3_LightDir: ep.SetVector3(RendererEffectParams::LightDirection); break;
						case EPUSAGE_LV3_LightPos: ep.SetVector3(RendererEffectParams::LightDirection); break;
						case EPUSAGE_PV3_ViewPos:
							if (RendererEffectParams::CurrentCamera)
							{
								Vector3 pos = RendererEffectParams::CurrentCamera->getInvViewMatrix().GetTranslation();
								ep.SetVector3(pos);
								ApocLog(LOG_Graphics, L"[" + m_name + L"]: EPUSAGE_PV3_ViewPos/pv3_viewpos is obsolete.", LOGLVL_Warning);
							}
							break;
						case EPUSAGE_SV2_ViewportSize:
						{
							Viewport vp = m_device->getViewport();
							ep.SetVector2({ static_cast<float>(vp.Width), static_cast<float>(vp.Height) });
							break;
						}
						case EPUSAGE_SV2_InvViewportSize:
						{
							Viewport vp = m_device->getViewport();
							ep.SetVector2({ 1.f / vp.Width, 1.f / vp.Height });
							break;
						}
						case EPUSAGE_S_FarPlane:
						{
							const Matrix& proj = RendererEffectParams::CurrentCamera->getProjMatrix();
							float n = proj.ExtractProjectionFarPlane();
							ep.SetFloat(n);
							break;
						}
						case EPUSAGE_S_NearPlane:
						{
							const Matrix& proj = RendererEffectParams::CurrentCamera->getProjMatrix();
							float n = proj.ExtractProjectionNearPlane();
							ep.SetFloat(n);
							break;
						}
						case EPUSAGE_Trans_View:		ep.SetMatrix(RendererEffectParams::CurrentCamera->getViewMatrix()); break;
						case EPUSAGE_Trans_ViewProj:	ep.SetMatrix(RendererEffectParams::CurrentCamera->getViewProjMatrix()); break;
						case EPUSAGE_Trans_Projection:	ep.SetMatrix(RendererEffectParams::CurrentCamera->getProjMatrix()); break;

						case EPUSAGE_Trans_InvView:		ep.SetMatrix(RendererEffectParams::CurrentCamera->getInvViewMatrix()); break;
						case EPUSAGE_Trans_InvViewProj:
						{
							Matrix invViewProj;
							Matrix::Inverse(invViewProj, RendererEffectParams::CurrentCamera->getViewProjMatrix());
							ep.SetMatrix(invViewProj);
							break;
						}
						case EPUSAGE_Trans_InvProj:
						{
							Matrix invProj;
							Matrix::Inverse(invProj, RendererEffectParams::CurrentCamera->getProjMatrix());
							ep.SetMatrix(invProj);
							break;
						}

						case EPUSAGE_V3_CameraX:	ep.SetVector3(RendererEffectParams::CurrentCamera->getInvViewMatrix().GetX()); break;
						case EPUSAGE_V3_CameraY:	ep.SetVector3(RendererEffectParams::CurrentCamera->getInvViewMatrix().GetY()); break;
						case EPUSAGE_V3_CameraZ:	ep.SetVector3(RendererEffectParams::CurrentCamera->getInvViewMatrix().GetZ()); break;
						case EPUSAGE_V3_CameraPos:	ep.SetVector3(RendererEffectParams::CurrentCamera->getInvViewMatrix().GetTranslation()); break;

						case EPUSAGE_DefaultTexture:	SetTexture(ep, ep.DefaultTexture); break;
						case EPUSAGE_S_UnifiedTime:		ep.SetFloat(m_unifiedTime); break;
					}
				}

				return 1;
			}
			void AutomaticEffect::end()
			{

			}


			void AutomaticEffect::SetTexture(const ResolvedEffectParameter& param, ResourceHandle<Texture>* value)
			{
				Texture* tex = nullptr;
				
				if (value)
					tex = value->ObtainLoaded();

				if (tex == nullptr)
					tex = m_whiteTexture;

				param.RS_TargetShader->SetTexture(param.SamplerIndex, tex);
			}
			void AutomaticEffect::SetTexture(const ResolvedEffectParameter& param, Texture* value)
			{
				param.RS_TargetShader->SetTexture(param.SamplerIndex, value == nullptr ? m_whiteTexture : value);
			}
			

			void AutomaticEffect::SetInstanceBlobParameter(const ResolvedEffectParameter& ep, const InstanceInfoBlobValue& v)
			{
				if (CustomEffectParameterType_IsReference(v.Type))
					SetSingleCustomParameter(ep, v.Type, v.RefValue);
				else
					SetSingleCustomParameter(ep, v.Type, v.Value);
			}
			void AutomaticEffect::SetMaterialCustomParameter(const ResolvedEffectParameter& ep, Material* mtrl)
			{
				const MaterialCustomParameter* mcp = mtrl->getCustomParameter(ep.ReferenceSource->CustomMaterialParamName);
				if (mcp)
				{
					if (CustomEffectParameterType_IsReference(mcp->Type))
						SetSingleCustomParameter(ep, mcp->Type, mcp->RefValue);
					else
						SetSingleCustomParameter(ep, mcp->Type, mcp->Value);
				}
			}

			void AutomaticEffect::SetSingleCustomParameter(const ResolvedEffectParameter& ep, CustomEffectParameterType type, const void* data)
			{
				switch (type)
				{
					case CEPT_Float:
						ep.SetFloat(*reinterpret_cast<const float*>(data));
						break;

					case CEPT_Vector2:
					case CEPT_Ref_Vector2:
						ep.SetVector2(*reinterpret_cast<const Vector2*>(data));
						break;

					case CEPT_Ref_Vector3:
						ep.SetVector3(*reinterpret_cast<const Vector3*>(data));
						break;

					case CEPT_Vector4:
					case CEPT_Ref_Vector4:
						ep.SetVector4(*reinterpret_cast<const Vector4*>(data));
						break;

					case CEPT_Matrix:
					case CEPT_Ref_Matrix:
						ep.SetMatrix(*reinterpret_cast<const Matrix*>(data));
						break;

					case CEPT_Boolean:
						ep.SetBool(*reinterpret_cast<const bool*>(data));
						break;
					case CEPT_Integer:
						ep.SetInt(*reinterpret_cast<const int*>(data));
						break;
					case CEPT_Ref_TextureHandle:
						SetTexture(ep, (ResourceHandle<Texture>*)data);
						break;
					case CEPT_Ref_Texture:
						SetTexture(ep, (Texture*)data);
						break;
				}
			}


			void AutomaticEffect::ResolvedEffectParameter::Free() { DELETE_AND_NULL(DefaultTexture); }

			void AutomaticEffect::ResolvedEffectParameter::SetSamplerState() const { RS_TargetShader->SetSamplerState(SamplerIndex, ReferenceSource->SamplerState); }

			void AutomaticEffect::ResolvedEffectParameter::SetVector2(const Vector2& value) const { RS_TargetShader->SetVector2(RegisterIndex, value); }
			void AutomaticEffect::ResolvedEffectParameter::SetVector3(const Vector3& value) const { RS_TargetShader->SetVector3(RegisterIndex, value); }
			void AutomaticEffect::ResolvedEffectParameter::SetVector4(const Vector4& value) const { RS_TargetShader->SetVector4(RegisterIndex, value); }
			void AutomaticEffect::ResolvedEffectParameter::SetVector4(const Vector4* value, int32 count) const { RS_TargetShader->SetVector4(RegisterIndex, value, count); }
			
			void AutomaticEffect::ResolvedEffectParameter::SetColor4(const Color4& value) const { RS_TargetShader->SetValue(RegisterIndex, value); }

			void AutomaticEffect::ResolvedEffectParameter::SetFloat(const float values) const { RS_TargetShader->SetValue(RegisterIndex, values); }
			void AutomaticEffect::ResolvedEffectParameter::SetFloat(const float* values, int32 count) const { RS_TargetShader->SetValue(RegisterIndex, values, count); }

			void AutomaticEffect::ResolvedEffectParameter::SetInt(const int32 values) const { RS_TargetShader->SetValue(RegisterIndex, values); }
			void AutomaticEffect::ResolvedEffectParameter::SetInt(const int32* values, int32 count) const { RS_TargetShader->SetValue(RegisterIndex, values, count); }

			void AutomaticEffect::ResolvedEffectParameter::SetBool(const bool values) const { RS_TargetShader->SetValue(RegisterIndex, values); }
			void AutomaticEffect::ResolvedEffectParameter::SetBool(const bool* values, int32 count) const { RS_TargetShader->SetValue(RegisterIndex, values, count); }


			void AutomaticEffect::ResolvedEffectParameter::Set4X3Matrix(const Matrix* transfroms, int32 count) const { RS_TargetShader->SetMatrix4x3(RegisterIndex, transfroms, count); }
			void AutomaticEffect::ResolvedEffectParameter::SetMatrix(const Matrix* transfroms, int32 count) const { RS_TargetShader->SetValue(RegisterIndex, transfroms, count); }
			void AutomaticEffect::ResolvedEffectParameter::SetMatrix(const Matrix& m) const { RS_TargetShader->SetValue(RegisterIndex, m); }


			AutomaticEffect::ResolvedEffectParameter::ResolvedEffectParameter(RenderDevice* device, const String& effectName,
				const EffectParameter* srcPtr, Shader* targetShader, bool& hasShaderIssues)
				: RS_TargetShader(targetShader), Usage(srcPtr->Usage),
				InstanceBlobIndex(srcPtr->InstanceBlobIndex), RegisterIndex(srcPtr->RegisterIndex),
				SamplerIndex(srcPtr->SamplerIndex), ReferenceSource(srcPtr)
			{
				const EffectParameter& src = *srcPtr;

				switch (src.Usage)
				{
					case EPUSAGE_LC4_Ambient:
					case EPUSAGE_LC4_Diffuse:
					case EPUSAGE_LC4_Specular:
					case EPUSAGE_LV3_LightDir:
					case EPUSAGE_LV3_LightPos:
					case EPUSAGE_PV3_ViewPos:
					case EPUSAGE_SV2_ViewportSize:
					case EPUSAGE_SV2_InvViewportSize:
					case EPUSAGE_S_FarPlane:
					case EPUSAGE_S_NearPlane:
					case EPUSAGE_Trans_View:
					case EPUSAGE_Trans_ViewProj:
					case EPUSAGE_Trans_Projection:
					case EPUSAGE_Trans_InvView:
					case EPUSAGE_Trans_InvProj:
					case EPUSAGE_Trans_InvViewProj:
					case EPUSAGE_V3_CameraX:
					case EPUSAGE_V3_CameraY:
					case EPUSAGE_V3_CameraZ:
					case EPUSAGE_V3_CameraPos:
					case EPUSAGE_S_UnifiedTime:
					case EPUSAGE_DefaultTexture:
						RS_SetupAtBeginingOnly = true;
						RS_SetupAtBegining = true;

						break;
				}

				if (src.DefaultTextureName.size())
				{
					FileLocation fl;
					if (FileSystem::getSingleton().TryLocate(src.DefaultTextureName, FileLocateRule::Textures, fl))
						DefaultTexture = TextureManager::getSingleton().CreateInstance(device, fl);
					else
						ApocLog(LOG_Graphics, L"[AutomaticEffect][" + effectName + L"] Default texture " + src.DefaultTextureName + L" for parameter " + src.Name + L" not found.", LOGLVL_Warning);
				}

				RS_TargetShader->TryGetSamplerIndex(src.Name, SamplerIndex);
				RS_TargetShader->TryGetParamIndex(src.Name, RegisterIndex);

				if (RegisterIndex == -1 && SamplerIndex == -1)
				{
					ApocLog(LOG_Graphics, L"[AutomaticEffect][" + effectName + L"] Effect parameter " + src.Name + L" does not have valid info.", LOGLVL_Warning);
					hasShaderIssues = true;
				}

				if (SamplerIndex != -1)
				{
					RS_SetupAtBegining = true;
				}
			}


			/************************************************************************/
			/*  CustomShaderEffect                                                  */
			/************************************************************************/

			CustomShaderEffect::CustomShaderEffect(RenderDevice* device, const ResourceLocation& rl)
			{
				EffectData data;
				data.Load(rl);
				m_name = data.Name;

				assert(data.IsCFX);

				Capabilities* caps = device->getCapabilities();
				const EffectProfileData* profileSelected = nullptr;

				for (const EffectProfileData& pd : data.Profiles)
				{
					if (!caps->SupportsVertexShader(pd.ImplementationType, pd.MajorVer, pd.MinorVer))
					{
						m_isUnsupported = true;
						return;
					}
					if (!caps->SupportsPixelShader(pd.ImplementationType, pd.MajorVer, pd.MinorVer))
					{
						m_isUnsupported = true;
						return;
					}
					profileSelected = &pd;
				}

				ObjectFactory* objFac = device->getObjectFactory();
				m_vertexShader = objFac->CreateVertexShader(reinterpret_cast<const byte*>(profileSelected->VSCode));
				m_pixelShader = objFac->CreatePixelShader(reinterpret_cast<const byte*>(profileSelected->PSCode));

				m_vertexShader->NotifyLinkage({ m_vertexShader, m_pixelShader });
			}

			CustomShaderEffect::CustomShaderEffect(RenderDevice* device, const String& name, const EffectCode& code)
			{
				m_name = name;

				ObjectFactory* objFac = device->getObjectFactory();
				m_vertexShader = objFac->CreateVertexShader(code.m_vsCode);
				m_pixelShader = objFac->CreatePixelShader(code.m_psCode);

				m_vertexShader->NotifyLinkage({ m_vertexShader, m_pixelShader });
			}

			CustomShaderEffect::~CustomShaderEffect()
			{
				DELETE_AND_NULL(m_vertexShader);
				DELETE_AND_NULL(m_pixelShader);
			}
		};
	}

};