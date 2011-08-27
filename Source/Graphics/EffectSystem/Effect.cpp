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

#include "Graphics/RenderSystem/RenderDevice.h"
#include "Graphics/RenderSystem/ObjectFactory.h"
#include "IOLib/Streams.h"
#include "IOLib/BinaryReader.h"
#include "IOLib/EffectData.h"

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
				: m_vertexShader(0), m_pixelShader(0)
			{
				EffectData data;
				data.Load(rl);
				
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

			}
			void AutomaticEffect::BeginPass(int passId)
			{

			}
			void AutomaticEffect::EndPass()
			{

			}

			int AutomaticEffect::begin()
			{
				return 1;
			}
			void AutomaticEffect::end()
			{

			}

		};
	}

};