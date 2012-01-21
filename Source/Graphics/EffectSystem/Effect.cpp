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
#include "Effect.h"

#include "Graphics/Material.h"
#include "Graphics/Camera.h"
#include "Graphics/RenderSystem/Shader.h"
#include "Graphics/RenderSystem/RenderDevice.h"
#include "Graphics/RenderSystem/ObjectFactory.h"
#include "Graphics/RenderSystem/Texture.h"
#include "Graphics/RenderSystem/InstancingData.h"
#include "IOLib/Streams.h"
#include "IOLib/BinaryReader.h"
#include "IOLib/EffectData.h"
#include "Math/Matrix.h"
#include "Core/Resource.h"
#include "Core/ResourceHandle.h"

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


			VertexShader* Effect::LoadVertexShader(RenderDevice* rs, const ResourceLocation* vs)
			{
				ObjectFactory* objFac = rs->getObjectFactory();
				return objFac->CreateVertexShader(vs);
			}
			PixelShader* Effect::LoadPixelShader(RenderDevice* rs, const ResourceLocation* ps)
			{
				ObjectFactory* objFac = rs->getObjectFactory();
				return objFac->CreatePixelShader(ps);
			}

			/************************************************************************/
			/*                                                                      */
			/************************************************************************/

			template void AutomaticEffect::SetParameterValue<bool>(int index, bool* value, int count);
			template void AutomaticEffect::SetParameterValue<int>(int index, int* value, int count);
			template void AutomaticEffect::SetParameterValue<float>(int index, float* value, int count);

			template void AutomaticEffect::SetParameterValue<const bool>(int index, const bool* value, int count);
			template void AutomaticEffect::SetParameterValue<const int>(int index, const int* value, int count);
			template void AutomaticEffect::SetParameterValue<const float>(int index, const float* value, int count);




			AutomaticEffect::AutomaticEffect(RenderDevice* device, const ResourceLocation* rl)
				: m_vertexShader(0), m_pixelShader(0), m_device(device), m_supportsInstancing(false), 
				m_unifiedTime(0), m_lastTime(0)
			{
				EffectData data;
				data.Load(rl);
				m_name = data.Name;
				assert(!data.IsCustom);

				Capabilities* caps = device->getCapabilities();
				if (!caps->SupportsVertexShader(data.MajorVer, data.MinorVer))
				{
					m_isUnsupported = true;
					return;
				}
				if (!caps->SupportsVertexShader(data.MajorVer, data.MinorVer))
				{
					m_isUnsupported = true;
					return;
				}

				ObjectFactory* objFac = device->getObjectFactory();
				m_vertexShader = objFac->CreateVertexShader(reinterpret_cast<const byte*>(data.VSCode));
				m_pixelShader = objFac->CreatePixelShader(reinterpret_cast<const byte*>(data.PSCode));

				m_parameters =  data.Parameters;
				
				m_texture = objFac->CreateTexture(1,1,1, TU_Static, FMT_A8R8G8B8);
				DataRectangle rect = m_texture->Lock(0, LOCK_None);
				*(uint*)rect.getDataPointer() = 0xffffffff;
				m_texture->Unlock(0);

				// instancing check
				{
					for (int i=0;i<m_parameters.getCount();i++)
					{
						if (m_parameters[i].TypicalUsage == EPUSAGE_Trans_InstanceWorlds)
						{
							m_supportsInstancing = true;
							break;
						}
					}
				}
			}

			AutomaticEffect::~AutomaticEffect()
			{
				if (m_vertexShader)
					delete m_vertexShader;
				if (m_pixelShader)
					delete m_pixelShader;
				delete m_texture;
			}

			bool AutomaticEffect::SupportsInstancing()
			{
				return m_supportsInstancing;
			}
			void AutomaticEffect::Setup(Material* mtrl, const RenderOperation* rop, int count)
			{
				// set the param's value one by one. 

				// TODO: some optimization can be done to reduce the parameters here.
				// as the material for all the render operations in the list is the same.
				// material parameters had better to be set only once.

				for (int i=0;i<m_parameters.getCount();i++)
				{
					EffectParameter& ep = m_parameters[i];
					switch (ep.TypicalUsage)
					{
						case EPUSAGE_MtrlC4_Ambient:
							SetValue(ep, mtrl->Ambient);
							break;
						case EPUSAGE_MtrlC4_Diffuse:
							SetValue(ep, mtrl->Diffuse);
							break;
						case EPUSAGE_MtrlC4_Emissive:
							SetValue(ep, mtrl->Emissive);
							break;
						case EPUSAGE_MtrlC4_Specular:
							SetValue(ep, mtrl->Specular);
							break;
						case EPUSAGE_MtrlC_Power:
							SetValue(ep, mtrl->Power);
							break;
						case EPUSAGE_Trans_WorldViewProj:
							if (RendererEffectParams::CurrentCamera)
							{
								Matrix temp;
								Matrix::Multiply(temp, rop->RootTransform, RendererEffectParams::CurrentCamera->getViewMatrix());

								Matrix mvp;
								Matrix::Multiply(mvp, temp, RendererEffectParams::CurrentCamera->getProjMatrix());

								SetValue(ep, mvp);
							}
							break;
						case EPUSAGE_Trans_WorldView:
							if (RendererEffectParams::CurrentCamera)
							{
								Matrix mv;
								Matrix::Multiply(mv, rop->RootTransform, RendererEffectParams::CurrentCamera->getViewMatrix());

								SetValue(ep, mv);
							}
							break;
						case EPUSAGE_Trans_World:
							SetValue(ep, rop->RootTransform);
							break;
						case EPUSAGE_Trans_WorldViewOriProj:
							if (RendererEffectParams::CurrentCamera)
							{
								Matrix view = RendererEffectParams::CurrentCamera->getViewMatrix();
								view.SetTranslation(Vector3Utils::Zero);

								Matrix temp;
								Matrix::Multiply(temp, rop->RootTransform, view);

								Matrix mvp;
								Matrix::Multiply(mvp, temp, RendererEffectParams::CurrentCamera->getProjMatrix());

								SetValue(ep, mvp);
							}
							break;
						case EPUSAGE_Trans_InstanceWorlds:
							{
								Shader* shader = 0;
								if (ep.ProgramType == SHDT_Vertex)
								{
									shader = m_vertexShader;
								}
								else if (ep.ProgramType == SHDT_Pixel)
								{
									shader = m_pixelShader;
								}

								if (ep.RegisterIndex == -1)
								{
									ep.RegisterIndex = shader->GetParamIndex(ep.Name);
								}

								for (int i=0;i<count && i<InstancingData::MaxOneTimeInstances;i++)
								{
									shader->SetValue(ep.RegisterIndex + i * 4, rop[i].RootTransform);
								}
							}
							break;
						case EPUSAGE_Trans_View:
							{
								SetValue(ep, RendererEffectParams::CurrentCamera->getViewMatrix());
							}
							break;
						case EPUSAGE_Trans_Projection:
							{
								SetValue(ep, RendererEffectParams::CurrentCamera->getProjMatrix());
							}
							break;
						case EPUSAGE_M4X3_BoneTrans:
							Set4X3Matrix(ep, rop->BoneTransform.Transfroms, rop->BoneTransform.Count);
							break;
						case EPUSAGE_M4X4_BoneTrans:
							SetMatrix(ep, rop->BoneTransform.Transfroms, rop->BoneTransform.Count);
							break;
						case EPUSAGE_Tex0:
							SetSamplerState(ep);
							SetTexture(ep, mtrl->getTexture(0));
							break;
						case EPUSAGE_Tex1:
							SetSamplerState(ep);
							SetTexture(ep, mtrl->getTexture(1));
							break;
						case EPUSAGE_Tex2:
							SetSamplerState(ep);
							SetTexture(ep, mtrl->getTexture(2));
							break;
						case EPUSAGE_Tex3:
							SetSamplerState(ep);
							SetTexture(ep, mtrl->getTexture(3));
							break;
						case EPUSAGE_Tex4:
							SetSamplerState(ep);
							SetTexture(ep, mtrl->getTexture(4));
							break;
						case EPUSAGE_Tex5:
							SetSamplerState(ep);
							SetTexture(ep, mtrl->getTexture(5));
							break;
						case EPUSAGE_Tex6:
							SetSamplerState(ep);
							SetTexture(ep, mtrl->getTexture(6));
							break;
						case EPUSAGE_Tex7:
							SetSamplerState(ep);
							SetTexture(ep, mtrl->getTexture(7));
							break;
						case EPUSAGE_Tex8:
							SetSamplerState(ep);
							SetTexture(ep, mtrl->getTexture(8));
							break;
						case EPUSAGE_Tex9:
							SetSamplerState(ep);
							SetTexture(ep, mtrl->getTexture(9));
							break;
						case EPUSAGE_Tex10:
							SetSamplerState(ep);
							SetTexture(ep, mtrl->getTexture(10));
							break;
						case EPUSAGE_Tex11:
							SetSamplerState(ep);
							SetTexture(ep, mtrl->getTexture(11));
							break;
						case EPUSAGE_Tex12:
							SetSamplerState(ep);
							SetTexture(ep, mtrl->getTexture(12));
							break;
						case EPUSAGE_Tex13:
							SetSamplerState(ep);
							SetTexture(ep, mtrl->getTexture(13));
							break;
						case EPUSAGE_Tex14:
							SetSamplerState(ep);
							SetTexture(ep, mtrl->getTexture(14));
							break;
						case EPUSAGE_Tex15:
							SetSamplerState(ep);
							SetTexture(ep, mtrl->getTexture(15));
							break;

						case EPUSAGE_Unknown:
							if (m_parameters[i].IsCustomUsage && mtrl)
							{
								const MaterialCustomParameter* mcp = mtrl->getCustomParameter(m_parameters[i].CustomUsage);
								if (mcp)
								{
									switch (mcp->Type)
									{
									case MTRLPT_Float:
										if (mcp->IsReference())
										{
											SetValue(ep, *reinterpret_cast<const float*>(mcp->RefValue));
										}
										else
										{
											SetValue(ep, *reinterpret_cast<const float*>(mcp->Value));
										}
										break;
									case MTRLPT_Vector2:
									case MTRLPT_Ref_Vector2:
										if (mcp->IsReference())
										{
											SetVector2(ep, *reinterpret_cast<const Vector2*>(mcp->RefValue));
										}
										else
										{
											SetVector2(ep, *reinterpret_cast<const Vector2*>(mcp->Value));
										}
										break;
									case MTRLPT_Ref_Vector3:
										if (mcp->IsReference())
										{
											SetVector3(ep, *reinterpret_cast<const Vector3*>(mcp->RefValue));
										}
										else
										{
											SetVector3(ep, *reinterpret_cast<const Vector3*>(mcp->Value));
										}
										break;

									case MTRLPT_Ref_Vector4:
									case MTRLPT_Vector4:
										if (mcp->IsReference())
										{
											SetVector4(ep, *reinterpret_cast<const Vector4*>(mcp->RefValue));
										}
										else
										{
											SetVector4(ep, *reinterpret_cast<const Vector4*>(mcp->Value));
										}
										break;
									case MTRLPT_Boolean:
										if (mcp->IsReference())
										{
											SetValue(ep, *reinterpret_cast<const bool*>(mcp->RefValue));
										}
										else
										{
											SetValue(ep, *reinterpret_cast<const bool*>(mcp->Value));
										}
										break;
									case MTRLPT_Integer:
										if (mcp->IsReference())
										{
											SetValue(ep, *reinterpret_cast<const int*>(mcp->RefValue));
										}
										else
										{
											SetValue(ep, *reinterpret_cast<const int*>(mcp->Value));
										}
										break;
									case MTRLPT_Ref_TextureHandle:
										if (mcp->IsReference())
										{
											SetTexture(ep, reinterpret_cast<ResourceHandle<Texture>*>(mcp->RefValue));
										}
										break;
									case MTRLPT_Ref_Texture:
										if (mcp->IsReference())
										{
											SetTexture(ep, reinterpret_cast<Texture*>(mcp->RefValue));
										}
										break;
									}
								}
							}
							
							break;
					}

				}
			}
			void AutomaticEffect::BeginPass(int passId)
			{

			}
			void AutomaticEffect::EndPass()
			{

			}

			template<typename T>
			void AutomaticEffect::SetParameterValue(int index, T* value, int count)
			{
				EffectParameter& param = m_parameters[index];
				Shader* shader = 0;
				if (param.ProgramType == SHDT_Vertex)
				{
					shader = m_vertexShader;
				}
				else if (param.ProgramType == SHDT_Pixel)
				{
					shader = m_pixelShader;
				}

				if (param.RegisterIndex == -1)
				{
					param.RegisterIndex = shader->GetParamIndex(param.Name);
				}
				shader->SetValue(param.RegisterIndex, value, count);
			}
			void AutomaticEffect::SetParameterTexture(int index, ResourceHandle<Texture>* value)
			{
				EffectParameter& param = m_parameters[index];
				Texture* tex = 0;

				if (value)
				{
					tex = value->operator->();
					if (tex->getState() != RS_Loaded)
					{
						tex = 0;
					}
				}
				else
				{
					tex = m_texture;
				}

				Shader* shader = 0;
				if (param.ProgramType == SHDT_Vertex)
				{
					shader = m_vertexShader;
				}
				else if (param.ProgramType == SHDT_Pixel)
				{
					shader = m_pixelShader;
				}

				if (param.SamplerIndex == -1)
				{
					param.SamplerIndex = shader->GetSamplerIndex(param.Name);
				}
				shader->SetSamplerState(param.SamplerIndex, param.SamplerState);
				shader->SetTexture(param.SamplerIndex, tex);
			}

			void AutomaticEffect::SetParameterTexture(int index, Texture* tex)
			{
				EffectParameter& param = m_parameters[index];
				

				Shader* shader = 0;
				if (param.ProgramType == SHDT_Vertex)
				{
					shader = m_vertexShader;
				}
				else if (param.ProgramType == SHDT_Pixel)
				{
					shader = m_pixelShader;
				}

				if (param.SamplerIndex == -1)
				{
					param.SamplerIndex = shader->GetSamplerIndex(param.Name);
				}
				shader->SetSamplerState(param.SamplerIndex, param.SamplerState);
				shader->SetTexture(param.SamplerIndex, tex);
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

			int AutomaticEffect::begin()
			{
				// as you can see, some parameters are set up here.
				// The reason here is for performance reasons, the begin method
				// is called per material and geometry shape, not per render operation.   
				// The amount is much smaller than render operations.
				// As a result, data sending to the GPU will be fewer.

				float t = m_lastTime + 0.016f;//(float)( clock() / CLOCKS_PER_SEC);
				m_unifiedTime += t - m_lastTime;
				m_lastTime = t;

				while (m_unifiedTime > Math::PI * 10)
				{
					m_unifiedTime -= Math::PI * 10;
				}

				m_device->BindPixelShader(m_pixelShader);
				m_device->BindVertexShader(m_vertexShader);

				for (int i=0;i<m_parameters.getCount();i++)
				{
					switch (m_parameters[i].TypicalUsage)
					{
					case EPUSAGE_LC4_Ambient:
						
						SetValue(m_parameters[i], RendererEffectParams::LightAmbient);
						break;
					case EPUSAGE_LC4_Diffuse:
						SetValue(m_parameters[i], RendererEffectParams::LightDiffuse);
						break;
					case EPUSAGE_LC4_Specular:
						SetValue(m_parameters[i], RendererEffectParams::LightSpecular);
						break;
					case EPUSAGE_LV3_LightDir:
						SetVector3(m_parameters[i], RendererEffectParams::LightDirection);
						break;
					case EPUSAGE_PV3_ViewPos:
						if (RendererEffectParams::CurrentCamera)
						{
							Vector3 pos = RendererEffectParams::CurrentCamera->getInvViewMatrix().GetTranslation();
							SetVector3(m_parameters[i], pos);
						}
						break;

					case EPUSAGE_SV2_ViewportSize:
						{
							Viewport vp = m_device->getViewport();
							Vector2 size = Vector2Utils::LDVector((float)vp.Width, (float)vp.Height);
							SetVector2(m_parameters[i], size);
						}
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
						//SetSamplerState(m_parameters[i]);
						break;
					case EPUSAGE_S_UnifiedTime:
						SetValue(m_parameters[i], m_unifiedTime);
						break;
					}
				}

				return 1;
			}
			void AutomaticEffect::end()
			{

			}
			template<typename T>
			void AutomaticEffect::SetValue(EffectParameter& param, const T& value)
			{
				Shader* shader = 0;
				if (param.ProgramType == SHDT_Vertex)
				{
					shader = m_vertexShader;
				}
				else if (param.ProgramType == SHDT_Pixel)
				{
					shader = m_pixelShader;
				}

				if (param.RegisterIndex == -1)
				{
					param.RegisterIndex = shader->GetParamIndex(param.Name);
				}
				shader->SetValue(param.RegisterIndex, value);
			}
			void AutomaticEffect::SetVector2(EffectParameter& param, Vector2 value)
			{
				Shader* shader = 0;
				if (param.ProgramType == SHDT_Vertex)
				{
					shader = m_vertexShader;
				}
				else if (param.ProgramType == SHDT_Pixel)
				{
					shader = m_pixelShader;
				}

				if (param.RegisterIndex == -1)
				{
					param.RegisterIndex = shader->GetParamIndex(param.Name);
				}
				
				shader->SetVector2(param.RegisterIndex, value);
			}
			void AutomaticEffect::SetVector3(EffectParameter& param, Vector3 value)
			{
				Shader* shader = 0;
				if (param.ProgramType == SHDT_Vertex)
				{
					shader = m_vertexShader;
				}
				else if (param.ProgramType == SHDT_Pixel)
				{
					shader = m_pixelShader;
				}
				
				if (param.RegisterIndex == -1)
				{
					param.RegisterIndex = shader->GetParamIndex(param.Name);
				}

				shader->SetVector3(param.RegisterIndex, value);
			}
			void AutomaticEffect::SetVector4(EffectParameter& param, Vector4 value)
			{
				Shader* shader = 0;
				if (param.ProgramType == SHDT_Vertex)
				{
					shader = m_vertexShader;
				}
				else if (param.ProgramType == SHDT_Pixel)
				{
					shader = m_pixelShader;
				}

				if (param.RegisterIndex == -1)
				{
					param.RegisterIndex = shader->GetParamIndex(param.Name);
				}

				shader->SetVector4(param.RegisterIndex, value);
			}
			void AutomaticEffect::SetSamplerState(EffectParameter& param)
			{
				Shader* shader = 0;
				if (param.ProgramType == SHDT_Vertex)
				{
					shader = m_vertexShader;
				}
				else if (param.ProgramType == SHDT_Pixel)
				{
					shader = m_pixelShader;
				}

				if (param.SamplerIndex == -1)
				{
					param.SamplerIndex = shader->GetSamplerIndex(param.Name);
				}
				shader->SetSamplerState(param.SamplerIndex, param.SamplerState);
			}
			void AutomaticEffect::SetTexture(EffectParameter& param, ResourceHandle<Texture>* value)
			{
				Texture* tex = 0;
				
				if (value)
				{
					tex = value->operator->();
					if (tex->getState() != RS_Loaded)
					{
						tex = 0;
					}
				}
				else
				{
					tex = m_texture;
				}

				Shader* shader = 0;
				if (param.ProgramType == SHDT_Vertex)
				{
					shader = m_vertexShader;
				}
				else if (param.ProgramType == SHDT_Pixel)
				{
					shader = m_pixelShader;
				}

				if (param.SamplerIndex == -1)
				{
					param.SamplerIndex = shader->GetSamplerIndex(param.Name);
				}
				shader->SetTexture(param.SamplerIndex, tex);
			}
			void AutomaticEffect::SetTexture(EffectParameter& param, Texture* value)
			{
				Shader* shader = 0;
				if (param.ProgramType == SHDT_Vertex)
				{
					shader = m_vertexShader;
				}
				else if (param.ProgramType == SHDT_Pixel)
				{
					shader = m_pixelShader;
				}

				if (param.SamplerIndex == -1)
				{
					param.SamplerIndex = shader->GetSamplerIndex(param.Name);
				}
				shader->SetTexture(param.SamplerIndex, value == 0 ? m_texture : value);
			}
			void AutomaticEffect::Set4X3Matrix(EffectParameter& param, const Matrix* transfroms, int count)
			{
				Shader* shader = 0;
				if (param.ProgramType == SHDT_Vertex)
				{
					shader = m_vertexShader;
				}
				else if (param.ProgramType == SHDT_Pixel)
				{
					shader = m_pixelShader;
				}

				if (param.RegisterIndex == -1)
				{
					param.RegisterIndex = shader->GetParamIndex(param.Name);
				}
				shader->SetMatrix4x3(param.RegisterIndex, transfroms, count);
			}
			void AutomaticEffect::SetMatrix(EffectParameter& param, const Matrix* transfroms, int count)
			{
				Shader* shader = 0;
				if (param.ProgramType == SHDT_Vertex)
				{
					shader = m_vertexShader;
				}
				else if (param.ProgramType == SHDT_Pixel)
				{
					shader = m_pixelShader;
				}

				if (param.RegisterIndex == -1)
				{
					param.RegisterIndex = shader->GetParamIndex(param.Name);
				}
				shader->SetValue(param.RegisterIndex, transfroms, count);
			}

			/************************************************************************/
			/*                                                                      */
			/************************************************************************/

			CustomShaderEffect::CustomShaderEffect(RenderDevice* device, const ResourceLocation* rl)
			{
				EffectData data;
				data.Load(rl);
				m_name = data.Name;

				assert(data.IsCustom);

				Capabilities* caps = device->getCapabilities();
				if (!caps->SupportsVertexShader(data.MajorVer, data.MinorVer))
				{
					m_isUnsupported = true;
					return;
				}
				if (!caps->SupportsVertexShader(data.MajorVer, data.MinorVer))
				{
					m_isUnsupported = true;
					return;
				}

				ObjectFactory* objFac = device->getObjectFactory();
				m_vertexShader = objFac->CreateVertexShader(reinterpret_cast<const byte*>(data.VSCode));
				m_pixelShader = objFac->CreatePixelShader(reinterpret_cast<const byte*>(data.PSCode));

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