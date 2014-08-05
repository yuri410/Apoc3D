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
#include "Effect.h"

#include "apoc3d/Core/Logging.h"
#include "apoc3d/Core/GameTime.h"
#include "apoc3d/Graphics/Material.h"
#include "apoc3d/Graphics/Camera.h"
#include "apoc3d/Graphics/RenderSystem/Shader.h"
#include "apoc3d/Graphics/RenderSystem/RenderDevice.h"
#include "apoc3d/Graphics/RenderSystem/Texture.h"
#include "apoc3d/Graphics/RenderSystem/InstancingData.h"
#include "apoc3d/IOLib/Streams.h"
#include "apoc3d/IOLib/BinaryReader.h"
#include "apoc3d/IOLib/EffectData.h"
#include "apoc3d/Math/Matrix.h"
#include "apoc3d/Core/Resource.h"
#include "apoc3d/Core/ResourceHandle.h"

using namespace Apoc3D::Math;
using namespace Apoc3D::IO;

namespace Apoc3D
{
	namespace Graphics
	{
		namespace EffectSystem
		{
			Effect::Effect(void)
				: m_begun(false), m_isUnsupported(false)
			{
			}


			Effect::~Effect(void)
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


			Shader* Effect::LoadVertexShader(RenderDevice* rs, const ResourceLocation& vs)
			{
				ObjectFactory* objFac = rs->getObjectFactory();
				return objFac->CreateVertexShader(vs);
			}
			Shader* Effect::LoadPixelShader(RenderDevice* rs, const ResourceLocation& ps)
			{
				ObjectFactory* objFac = rs->getObjectFactory();
				return objFac->CreatePixelShader(ps);
			}

			/************************************************************************/
			/*  AutomaticEffect                                                     */
			/************************************************************************/

			static Matrix s_instancingMatrixBuffer[InstancingData::MaxOneTimeInstances];
			static float s_instancingFloatBuffer[InstancingData::MaxOneTimeInstances];
			static float s_instancingVector2Buffer[InstancingData::MaxOneTimeInstances*2];
			static Vector4 s_instancingVector4Buffer[InstancingData::MaxOneTimeInstances];
			
			
			template void AutomaticEffect::SetParameterValue<bool>(int index, const bool* value, int count);
			template void AutomaticEffect::SetParameterValue<int>(int index, const int* value, int count);
			template void AutomaticEffect::SetParameterValue<float>(int index, const float* value, int count);

			template void AutomaticEffect::SetParameterValueByName<bool>(const String& name, const bool* value, int count);
			template void AutomaticEffect::SetParameterValueByName<int>(const String& name, const int* value, int count);
			template void AutomaticEffect::SetParameterValueByName<float>(const String& name, const float* value, int count);


			AutomaticEffect::AutomaticEffect(RenderDevice* device, const ResourceLocation& rl)
				: m_vertexShader(nullptr), m_pixelShader(nullptr), m_device(device), m_supportsInstancing(false), 
				m_unifiedTime(0), m_lastTime(0),
				m_previousMaterialPointer(nullptr)
			{
				Reload(rl);

				ObjectFactory* objFac = m_device->getObjectFactory();
				m_texture = objFac->CreateTexture(1,1,1, TU_Static, FMT_A8R8G8B8);
				DataRectangle rect = m_texture->Lock(0, LOCK_None);
				*(uint*)rect.getDataPointer() = 0xffffffff;
				m_texture->Unlock(0);
			}

			AutomaticEffect::~AutomaticEffect()
			{
				DELETE_AND_NULL(m_vertexShader);
				DELETE_AND_NULL(m_pixelShader);

				delete m_texture;
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
				for (int i=data.ProfileCount-1; i>=0; i--)
				{
					const EffectProfileData& pd = data.Profiles[i];
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

				ObjectFactory* objFac = m_device->getObjectFactory();
				m_vertexShader = objFac->CreateVertexShader(reinterpret_cast<const byte*>(profileSelected->VSCode));
				m_pixelShader = objFac->CreatePixelShader(reinterpret_cast<const byte*>(profileSelected->PSCode));
				
				bool hasShaderIssues = false;

				m_parameters.ResizeDiscard(profileSelected->Parameters.getCount());
				for (int32 i=0;i<profileSelected->Parameters.getCount();i++)
				{
					ResolvedEffectParameter rep(profileSelected->Parameters[i]);

					Shader* shader = nullptr;
					if (rep.ProgramType == SHDT_Vertex)
					{
						shader = m_vertexShader;
					}
					else if (rep.ProgramType == SHDT_Pixel)
					{
						shader = m_pixelShader;
					}
					assert(shader);
					rep.RS_TargetShader = shader;

					switch (rep.Usage)
					{
					case EPUSAGE_LC4_Ambient:
					case EPUSAGE_LC4_Diffuse:
					case EPUSAGE_LC4_Specular:
					case EPUSAGE_LV3_LightDir:
					case EPUSAGE_PV3_ViewPos:
					case EPUSAGE_SV2_ViewportSize:
					case EPUSAGE_SV2_InvViewportSize:
					case EPUSAGE_S_FarPlane:
					case EPUSAGE_S_NearPlane:
					case EPUSAGE_Trans_View:
					case EPUSAGE_Trans_InvView:
					case EPUSAGE_Trans_ViewProj:
					case EPUSAGE_Trans_Projection:
					case EPUSAGE_Trans_InvProj:
					case EPUSAGE_V3_CameraX:
					case EPUSAGE_V3_CameraY:
					case EPUSAGE_V3_CameraZ:
					case EPUSAGE_S_UnifiedTime:
						rep.RS_SetupAtBeginingOnly = true;

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

						rep.RS_SetupAtBegining = true;
						break;
					}

					shader->TryGetSamplerIndex(rep.Name, rep.SamplerIndex);
					shader->TryGetParamIndex(rep.Name, rep.RegisterIndex);

					if (!(rep.RegisterIndex != -1 || rep.SamplerIndex != -1))
					{
						ApocLog(LOG_Graphics, L"[AutomaticEffect][" + m_name + L"] Effect parameter " + rep.Name +  L" does not have valid info.", LOGLVL_Warning);
						hasShaderIssues = true;
					}

					m_parameters.Add(rep);
				}
				
				// instancing check
				for (int i=0;i<m_parameters.getCount();i++)
				{
					if (m_parameters[i].Usage == EPUSAGE_Trans_InstanceWorlds)
					{
						m_supportsInstancing = true;
						break;
					}
				}

				if (hasShaderIssues)
				{
					ApocLog(LOG_Graphics, L"[AutomaticEffect][" + m_name + L"] Effect disable due to invalid data.", LOGLVL_Warning);
					m_isUnsupported = true;
				}
			}


			void AutomaticEffect::Update(const GameTime* const time)
			{
				float t = m_lastTime + time->getElapsedTime();//(float)( clock() / CLOCKS_PER_SEC);
				m_unifiedTime += t - m_lastTime;
				m_lastTime = t;

				while (m_unifiedTime > Math::PI * 10)
				{
					m_unifiedTime -= Math::PI * 10;
				}
			}

			void AutomaticEffect::Setup(Material* mtrl, const RenderOperation* rop, int count)
			{
				// set the param's value one by one. 

				// the material for all the render operations in the list is the same.
				// it is better to be set only once. by checking m_previousMaterialPointer

				for (int i=0;i<m_parameters.getCount();i++)
				{
					ResolvedEffectParameter& ep = m_parameters[i];

					if (ep.RS_SetupAtBegining && ep.RS_SetupAtBeginingOnly)
						continue;

					switch (ep.Usage)
					{
						case EPUSAGE_MtrlC4_Ambient:
							if (m_previousMaterialPointer != mtrl)
								ep.SetColor4(mtrl->Ambient);
							break;
						case EPUSAGE_MtrlC4_Diffuse:
							if (m_previousMaterialPointer != mtrl)
								ep.SetColor4(mtrl->Diffuse);
							break;
						case EPUSAGE_MtrlC4_Emissive:
							if (m_previousMaterialPointer != mtrl)
								ep.SetColor4(mtrl->Emissive);
							break;
						case EPUSAGE_MtrlC4_Specular:
							if (m_previousMaterialPointer != mtrl)
								ep.SetColor4(mtrl->Specular);
							break;
						case EPUSAGE_MtrlC_Power:
							if (m_previousMaterialPointer != mtrl)
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
							if (m_previousMaterialPointer != mtrl)
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
								for (int i=0;i<count && i<InstancingData::MaxOneTimeInstances;i++)
								{
									s_instancingMatrixBuffer[i] = rop[i].RootTransform;
								}

								ep.SetMatrix(s_instancingMatrixBuffer, count);
							}
							break;
						
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

									bool inValidType = false;
									switch (paramType)
									{
									case CEPT_Float:
										{
											for (int i=0;i<count && i<InstancingData::MaxOneTimeInstances;i++)
												s_instancingFloatBuffer[i] = (*reinterpret_cast<const InstanceInfoBlob*>(rop[i].UserData))[ep.InstanceBlobIndex].AsSingle();

											int32 alignedCount = (count + 3) & ~0x03;
											ep.SetFloat(s_instancingFloatBuffer, alignedCount);
										}
										break;

									case CEPT_Vector2:
									case CEPT_Ref_Vector2:
										{
											Vector2* dst = reinterpret_cast<Vector2*>(s_instancingVector2Buffer);

											if (paramType == CEPT_Ref_Vector2)
											{
												for (int i=0;i<count && i<InstancingData::MaxOneTimeInstances;i++)
													dst[i] = (*reinterpret_cast<const InstanceInfoBlob*>(rop[i].UserData))[ep.InstanceBlobIndex].AsVector2Ref();
											}
											else
											{
												for (int i=0;i<count && i<InstancingData::MaxOneTimeInstances;i++)
													dst[i] = (*reinterpret_cast<const InstanceInfoBlob*>(rop[i].UserData))[ep.InstanceBlobIndex].AsVector2();
											}
												
											int32 alignedCount = (count*2 + 3) & ~0x03;
											ep.SetFloat(s_instancingVector2Buffer, alignedCount);
										}
										break;
											
									case CEPT_Vector4:
										for (int i=0;i<count && i<InstancingData::MaxOneTimeInstances;i++)
											s_instancingVector4Buffer[i] = (*reinterpret_cast<const InstanceInfoBlob*>(rop[i].UserData))[ep.InstanceBlobIndex].AsVector4();
										ep.SetVector4(s_instancingVector4Buffer, count);
										break;
									case CEPT_Ref_Vector4:
										for (int i=0;i<count && i<InstancingData::MaxOneTimeInstances;i++)
											s_instancingVector4Buffer[i] = (*reinterpret_cast<const InstanceInfoBlob*>(rop[i].UserData))[ep.InstanceBlobIndex].AsVector4Ref();
										ep.SetVector4(s_instancingVector4Buffer, count);
										break;

									case CEPT_Ref_Vector3:
										for (int i=0;i<count && i<InstancingData::MaxOneTimeInstances;i++)
											(Vector3&)s_instancingVector4Buffer[i] = (*reinterpret_cast<const InstanceInfoBlob*>(rop[i].UserData))[ep.InstanceBlobIndex].AsVector3Ref();
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
											L"[" + m_name + L"] Instanced InfoBlob parameter " + ep.Name + L" type not valid. ", LOGLVL_Warning);
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
							if (m_previousMaterialPointer != mtrl)
							{
								if (mtrl && ep.CustomMaterialParamName.size())
								{
									SetMaterialCustomParameter(ep, mtrl);
								}
							}
							break;
					}
				}

				m_previousMaterialPointer = mtrl;
			}
			void AutomaticEffect::BeginPass(int passId)
			{

			}
			void AutomaticEffect::EndPass()
			{

			}

			template<typename T>
			void AutomaticEffect::SetParameterValue(int index, const T* value, int count)
			{
				ResolvedEffectParameter& param = m_parameters[index];
				
				param.RS_TargetShader->SetValue(param.RegisterIndex, value, count);
			}
			void AutomaticEffect::SetParameterTexture(int index, ResourceHandle<Texture>* value)
			{
				ResolvedEffectParameter& param = m_parameters[index];
				Texture* tex = 0;

				if (value)
				{
					tex = value->operator->();
					if (!value->shouldNotTouchResource() && tex->getState() != ResourceState::Loaded)
					{
						tex = 0;
					}
				}
				else
				{
					tex = m_texture;
				}

				param.RS_TargetShader->SetSamplerState(param.SamplerIndex, param.SamplerState);
				param.RS_TargetShader->SetTexture(param.SamplerIndex, tex);
			}

			void AutomaticEffect::SetParameterTexture(int index, Texture* tex)
			{
				ResolvedEffectParameter& param = m_parameters[index];
				
				param.RS_TargetShader->SetSamplerState(param.SamplerIndex, param.SamplerState);
				param.RS_TargetShader->SetTexture(param.SamplerIndex, tex);
			}

			int AutomaticEffect::FindParameterIndex(const String& name)
			{
				for (int i=0;i<m_parameters.getCount();i++)
				{
					if (m_parameters[i].Name == name)
					{
						return i;
					}
				}
				return -1;
			}

			template<typename T>
			void AutomaticEffect::SetParameterValueByName(const String& name, const T* value, int count)
			{
				int32 idx = FindParameterIndex(name);
				assert(idx != -1);
				SetParameterValue(idx, value, count);
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

				for (int i=0;i<m_parameters.getCount();i++)
				{
					ResolvedEffectParameter& ep = m_parameters[i];

					if (!ep.RS_SetupAtBegining)
						continue;

					switch (ep.Usage)
					{
					case EPUSAGE_LC4_Ambient:
						ep.SetColor4(RendererEffectParams::LightAmbient);
						break;
					case EPUSAGE_LC4_Diffuse:
						ep.SetColor4(RendererEffectParams::LightDiffuse);
						break;
					case EPUSAGE_LC4_Specular:
						ep.SetColor4(RendererEffectParams::LightSpecular);
						break;
					case EPUSAGE_LV3_LightDir:
						ep.SetVector3(RendererEffectParams::LightDirection);
						break;
					case EPUSAGE_PV3_ViewPos:
						if (RendererEffectParams::CurrentCamera)
						{
							Vector3 pos = RendererEffectParams::CurrentCamera->getInvViewMatrix().GetTranslation();
							ep.SetVector3(pos);
						}
						break;
					case EPUSAGE_SV2_ViewportSize:
						{
							Viewport vp = m_device->getViewport();
							Vector2 size = Vector2((float)vp.Width, (float)vp.Height);
							ep.SetVector2(size);
							break;
						}
					case EPUSAGE_SV2_InvViewportSize:
						{
							Viewport vp = m_device->getViewport();
							Vector2 size = Vector2(1.f/(float)vp.Width, 1.f/(float)vp.Height);
							ep.SetVector2(size);
							break;
						}
					case EPUSAGE_S_FarPlane:
						{
							//SetValue(m_parameters[i], RendererEffectParams::CurrentCamera->);
							const Matrix& view = RendererEffectParams::CurrentCamera->getProjMatrix();
							float n = -view.M34 * view.M43 / view.M33;
							if (view.M34<0)
							{
								// RH
								ep.SetFloat((float)(view.M33 * n /(view.M33*n+1)));
							}
							else
							{
								ep.SetFloat((float)(view.M33 * n /(view.M33*n-1)));
							}
							break;
						}
					case EPUSAGE_S_NearPlane:
						{
							const Matrix& view = RendererEffectParams::CurrentCamera->getProjMatrix();
							float n = - view.M34 * view.M43 / view.M33;
							ep.SetFloat(n);
							break;
						}
					case EPUSAGE_Trans_View:
						ep.SetMatrix(RendererEffectParams::CurrentCamera->getViewMatrix());
						break;
					case EPUSAGE_Trans_InvView:
						ep.SetMatrix(RendererEffectParams::CurrentCamera->getInvViewMatrix());
						break;
					case EPUSAGE_Trans_ViewProj:
						ep.SetMatrix(RendererEffectParams::CurrentCamera->getViewProjMatrix());
						break;
					case EPUSAGE_Trans_Projection:
						ep.SetMatrix(RendererEffectParams::CurrentCamera->getProjMatrix());
						break;
					case EPUSAGE_Trans_InvProj:
						{
							Matrix invProj;
							Matrix::Inverse(invProj, RendererEffectParams::CurrentCamera->getProjMatrix());
							ep.SetMatrix(invProj);
							break;
						}
					case EPUSAGE_V3_CameraX:
						ep.SetVector3(RendererEffectParams::CurrentCamera->getInvViewMatrix().GetX());
						break;
					case EPUSAGE_V3_CameraY:
						ep.SetVector3(RendererEffectParams::CurrentCamera->getInvViewMatrix().GetY());
						break;
					case EPUSAGE_V3_CameraZ:
						ep.SetVector3(RendererEffectParams::CurrentCamera->getInvViewMatrix().GetZ());
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
						ep.SetSamplerState();
						break;

					case EPUSAGE_S_UnifiedTime:
						ep.SetFloat(m_unifiedTime);
						break;
					}
				}

				return 1;
			}
			void AutomaticEffect::end()
			{

			}


			void AutomaticEffect::SetTexture(ResolvedEffectParameter& param, ResourceHandle<Texture>* value)
			{
				Texture* tex = 0;
				
				if (value)
				{
					tex = value->operator->();
					if (!value->shouldNotTouchResource() && tex->getState() != ResourceState::Loaded)
					{
						tex = nullptr;
					}
				}
				else
				{
					tex = m_texture;
				}

				param.RS_TargetShader->SetTexture(param.SamplerIndex, tex);
			}
			void AutomaticEffect::SetTexture(ResolvedEffectParameter& param, Texture* value)
			{
				param.RS_TargetShader->SetTexture(param.SamplerIndex, value == nullptr ? m_texture : value);
			}
			

			void AutomaticEffect::SetInstanceBlobParameter(ResolvedEffectParameter& ep, const InstanceInfoBlobValue& v)
			{
				if (EffectParameter::IsReference(v.Type))
					SetSingleCustomParameter(ep, v.Type, v.RefValue);
				else
					SetSingleCustomParameter(ep, v.Type, v.Value);
			}
			void AutomaticEffect::SetMaterialCustomParameter(ResolvedEffectParameter& ep, Material* mtrl)
			{
				const MaterialCustomParameter* mcp = mtrl->getCustomParameter(ep.CustomMaterialParamName);
				if (mcp)
				{
					if (EffectParameter::IsReference(mcp->Type))
						SetSingleCustomParameter(ep, mcp->Type, mcp->RefValue);
					else
						SetSingleCustomParameter(ep, mcp->Type, mcp->Value);
				}
			}

			void AutomaticEffect::SetSingleCustomParameter(ResolvedEffectParameter& ep, CustomEffectParameterType type, const void* data)
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
				case CEPT_Ref_Vector4:
				case CEPT_Vector4:
					ep.SetVector4(*reinterpret_cast<const Vector4*>(data));
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

			void AutomaticEffect::ResolvedEffectParameter::SetSamplerState() const { RS_TargetShader->SetSamplerState(SamplerIndex, SamplerState); }

			void AutomaticEffect::ResolvedEffectParameter::SetVector2(const Vector2& value) const { RS_TargetShader->SetVector2(RegisterIndex, value); }
			void AutomaticEffect::ResolvedEffectParameter::SetVector3(const Vector3& value) const { RS_TargetShader->SetVector3(RegisterIndex, value); }
			void AutomaticEffect::ResolvedEffectParameter::SetVector4(const Vector4& value) const { RS_TargetShader->SetVector4(RegisterIndex, value); }
			void AutomaticEffect::ResolvedEffectParameter::SetVector4(const Vector4* value, int count) const { RS_TargetShader->SetVector4(RegisterIndex, value, count); }
			
			void AutomaticEffect::ResolvedEffectParameter::SetColor4(const Color4& value) const { RS_TargetShader->SetValue(RegisterIndex, value); }

			void AutomaticEffect::ResolvedEffectParameter::SetFloat(const float values) const { RS_TargetShader->SetValue(RegisterIndex, values); }
			void AutomaticEffect::ResolvedEffectParameter::SetFloat(const float* values, int count) const { RS_TargetShader->SetValue(RegisterIndex, values, count); }

			void AutomaticEffect::ResolvedEffectParameter::SetInt(const int values) const { RS_TargetShader->SetValue(RegisterIndex, values); }
			void AutomaticEffect::ResolvedEffectParameter::SetInt(const int* values, int count) const { RS_TargetShader->SetValue(RegisterIndex, values, count); }

			void AutomaticEffect::ResolvedEffectParameter::SetBool(const bool values) const { RS_TargetShader->SetValue(RegisterIndex, values); }
			void AutomaticEffect::ResolvedEffectParameter::SetBool(const bool* values, int count) const { RS_TargetShader->SetValue(RegisterIndex, values, count); }


			void AutomaticEffect::ResolvedEffectParameter::Set4X3Matrix(const Matrix* transfroms, int count) const { RS_TargetShader->SetMatrix4x3(RegisterIndex, transfroms, count); }
			void AutomaticEffect::ResolvedEffectParameter::SetMatrix(const Matrix* transfroms, int count) const { RS_TargetShader->SetValue(RegisterIndex, transfroms, count); }
			void AutomaticEffect::ResolvedEffectParameter::SetMatrix(const Matrix& m) const { RS_TargetShader->SetValue(RegisterIndex, m); }

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

				for (int i=data.ProfileCount-1; i>=0; i--)
				{
					const EffectProfileData& pd = data.Profiles[i];
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
			}

			CustomShaderEffect::~CustomShaderEffect()
			{
				if (m_vertexShader)
					delete m_vertexShader;
				if (m_pixelShader)
					delete m_pixelShader;
			}
		};
	}

};