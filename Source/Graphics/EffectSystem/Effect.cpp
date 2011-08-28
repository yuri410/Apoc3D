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

			AutomaticEffect::AutomaticEffect(RenderDevice* device, const ResourceLocation* rl)
				: m_vertexShader(0), m_pixelShader(0), m_device(device)
			{
				EffectData data;
				data.Load(rl);
				m_name = data.Name;

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
				
			}

			AutomaticEffect::~AutomaticEffect()
			{
				if (m_vertexShader)
					delete m_vertexShader;
				if (m_pixelShader)
					delete m_pixelShader;
			}

			void AutomaticEffect::Setup(Material* mtrl, const RenderOperation& rop)
			{
				for (int i=0;i<m_parameters.getCount();i++)
				{
					switch (m_parameters[i].TypicalUsage)
					{
						case EPUSAGE_MtrlC4_Ambient:
							SetValue(m_parameters[i], mtrl->Ambient);
							break;
						case EPUSAGE_MtrlC4_Diffuse:
							SetValue(m_parameters[i], mtrl->Diffuse);
							break;
						case EPUSAGE_MtrlC4_Emissive:
							SetValue(m_parameters[i], mtrl->Emissive);
							break;
						case EPUSAGE_MtrlC4_Specular:
							SetValue(m_parameters[i], mtrl->Specular);
							break;
						case EPUSAGE_MtrlC_Power:
							SetValue(m_parameters[i], mtrl->Power);
							break;
						case EPUSAGE_Tex0:
							SetTexture(m_parameters[i], mtrl->getTexture(0));
							break;
						case EPUSAGE_Tex1:
							SetTexture(m_parameters[i], mtrl->getTexture(1));
							break;
						case EPUSAGE_Tex2:
							SetTexture(m_parameters[i], mtrl->getTexture(2));
							break;
						case EPUSAGE_Tex3:
							SetTexture(m_parameters[i], mtrl->getTexture(3));
							break;
						case EPUSAGE_Tex4:
							SetTexture(m_parameters[i], mtrl->getTexture(4));
							break;
						case EPUSAGE_Tex5:
							SetTexture(m_parameters[i], mtrl->getTexture(5));
							break;
						case EPUSAGE_Tex6:
							SetTexture(m_parameters[i], mtrl->getTexture(6));
							break;
						case EPUSAGE_Tex7:
							SetTexture(m_parameters[i], mtrl->getTexture(7));
							break;
						case EPUSAGE_Tex8:
							SetTexture(m_parameters[i], mtrl->getTexture(8));
							break;
						case EPUSAGE_Tex9:
							SetTexture(m_parameters[i], mtrl->getTexture(9));
							break;
						case EPUSAGE_Tex10:
							SetTexture(m_parameters[i], mtrl->getTexture(10));
							break;
						case EPUSAGE_Tex11:
							SetTexture(m_parameters[i], mtrl->getTexture(11));
							break;
						case EPUSAGE_Tex12:
							SetTexture(m_parameters[i], mtrl->getTexture(12));
							break;
						case EPUSAGE_Tex13:
							SetTexture(m_parameters[i], mtrl->getTexture(13));
							break;
						case EPUSAGE_Tex14:
							SetTexture(m_parameters[i], mtrl->getTexture(14));
							break;
						case EPUSAGE_Tex15:
							SetTexture(m_parameters[i], mtrl->getTexture(15));
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

			int AutomaticEffect::begin()
			{
				m_device->BindPixelShader(m_pixelShader);
				m_device->BindVertexShader(m_vertexShader);

				for (int i=0;i<m_parameters.getCount();i++)
				{
					switch (m_parameters[i].TypicalUsage)
					{
					case EPUSAGE_LC3_Ambient:
						SetVector3(m_parameters[i], 
							Vector3Utils::LDVector(RendererEffectParams::LightAmbient.Red,
							RendererEffectParams::LightAmbient.Green,
							RendererEffectParams::LightAmbient.Blue
							));
						break;
					case EPUSAGE_LC3_Diffuse:
						SetVector3(m_parameters[i], 
							Vector3Utils::LDVector(RendererEffectParams::LightDiffuse.Red,
							RendererEffectParams::LightDiffuse.Green,
							RendererEffectParams::LightDiffuse.Blue
							));
						break;
					case EPUSAGE_LC3_Specular:
						SetVector3(m_parameters[i], 
							Vector3Utils::LDVector(RendererEffectParams::LightSpecular.Red,
							RendererEffectParams::LightSpecular.Green,
							RendererEffectParams::LightSpecular.Blue
							));
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
						SetSamplerState(m_parameters[i]);
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
				Texture* tex = value->operator->();
				if (tex->getState() != RS_Loaded)
				{
					tex = 0;
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
		};
	}

};