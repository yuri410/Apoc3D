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

#include "D3D9TypedShader.h"
#include "D3D9RenderDevice.h"

#include "apoc3d/IOLib/BinaryReader.h"
#include "apoc3d/Vfs/ResourceLocation.h"
#include "apoc3d/IOLib/Streams.h"
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
				const ShaderConstant* sc = m_constantTable->getConstant(paramName);
				if (sc)
					SetTexture(sc->SamplerIndex, tex);
			}
			void D3D9PixelShader::SetSamplerState(const String &paramName, const ShaderSamplerState &state)
			{
				const ShaderConstant* sc = m_constantTable->getConstant(paramName);
				if (sc)
					SetSamplerState(sc->SamplerIndex, state);
			}


			void D3D9VertexShader::SetTexture(int samIndex, Texture* tex) 
			{
				// vertex textures are set to the D3DVERTEXTEXTURESAMPLERn
				D3DBaseTexture* value = 0;
				if (tex)
				{
					if (!tex->isManaged() || tex->getState() == ResourceState::Loaded)
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
				const ShaderConstant* sc = m_constantTable->getConstant(paramName);
				if (sc)
					SetTexture(sc->SamplerIndex, tex);
			}
			void D3D9VertexShader::SetSamplerState(const String &paramName, const ShaderSamplerState &state)
			{
				const ShaderConstant* sc = m_constantTable->getConstant(paramName);
				if (sc)
					SetSamplerState(sc->SamplerIndex, state);
			}
		}
	}
}