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

#include "D3D9TypedShader.h"
#include "D3D9RenderDevice.h"

#include "apoc3d/IOLib/BinaryReader.h"
#include "apoc3d/Vfs/ResourceLocation.h"
#include "apoc3d/IOLib/Streams.h"
#include "apoc3d/Exception.h"
#include "apoc3d/Utility/StringUtils.h"

#include "ConstantTable.h"
#include "D3D9Texture.h"
#include "D3D9Utils.h"
#include "D3D9RenderStateManager.h"


using namespace Apoc3D::Utility;
using namespace Apoc3D::IO;

namespace Apoc3D
{
	namespace Graphics
	{
		namespace D3D9RenderSystem
		{
			void D3D9PixelShader::SetTexture(int samIndex, Texture* tex) 
			{
				m_device->getNativeStateManager()->SetTexture(samIndex, static_cast<D3D9Texture*>(tex));
			}
			void D3D9PixelShader::SetSamplerState(int samIndex, const ShaderSamplerState &state) 
			{
				m_device->getNativeStateManager()->SetPixelSampler(samIndex, state);
			}

			void D3D9PixelShader::SetTexture(const String &paramName, Texture* tex)
			{
				const ShaderConstant& cons = m_constantTable->getConstant(paramName);
				SetTexture(cons.SamplerIndex, tex);
			}
			void D3D9PixelShader::SetSamplerState(const String &paramName, const ShaderSamplerState &state)
			{
				const ShaderConstant& cons = m_constantTable->getConstant(paramName);
				SetSamplerState(cons.SamplerIndex, state);
			}


			void D3D9VertexShader::SetTexture(int samIndex, Texture* tex) 
			{
				// vertex textures are set to the D3DVERTEXTEXTURESAMPLERn
				D3DBaseTexture* value = 0;
				if (tex)
				{
					if (!tex->isManaged() || tex->getState() == RS_Loaded)
					{
						D3D9Texture* d3dTex = static_cast<D3D9Texture*>(tex);
						if (d3dTex->getInternal2D())
						{
							value = d3dTex->getInternal2D();
						}
						else if (d3dTex->getInternalCube())
						{
							value = d3dTex->getInternalCube();
						}
						else if (d3dTex->getInternal3D())
						{
							value = d3dTex->getInternalCube();
						}
					}
				}
				m_device->getDevice()->SetTexture(samIndex + D3DVERTEXTEXTURESAMPLER0, value);
			}
			void D3D9VertexShader::SetSamplerState(int samIndex, const ShaderSamplerState &state) 
			{
				m_device->getNativeStateManager()->SetVertexSampler(samIndex, state);

			}

			void D3D9VertexShader::SetTexture(const String &paramName, Texture* tex)
			{
				const ShaderConstant& cons = m_constantTable->getConstant(paramName);
				SetTexture(cons.SamplerIndex, tex);
			}
			void D3D9VertexShader::SetSamplerState(const String &paramName, const ShaderSamplerState &state)
			{
				const ShaderConstant& cons = m_constantTable->getConstant(paramName);
				SetSamplerState(cons.SamplerIndex, state);
			}
		}
	}
}