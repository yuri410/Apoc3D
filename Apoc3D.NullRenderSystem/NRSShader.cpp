
/* -----------------------------------------------------------------------
 * This source file is part of Apoc3D Framework
 * 
 * Copyright (c) 2011-2018 Tao Xin
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

#include "NRSShader.h"
#include "NRSRenderStateManager.h"
#include "NRSRenderDevice.h"
#include "NRSTexture.h"
#include "apoc3d/Core/Logging.h"

namespace Apoc3D
{
	namespace Graphics
	{
		namespace NullRenderSystem
		{
			NRSShader::NRSShader(NRSRenderDevice* device, const byte* byteCode)
				: Shader(device), m_device(device)
			{
				m_constantTable = new ConstantTable(byteCode);
			}

			NRSShader::~NRSShader()
			{
				delete m_constantTable;
			}

			bool NRSShader::TryGetParamIndex(const String& paramName, int32& result)
			{
				const ShaderConstant* sc = m_constantTable->getConstant(paramName);
				if (sc && !sc->IsSampler)
				{
					result = sc->RegisterIndex;
					return true;
				}
				return false;
			}
			bool NRSShader::TryGetSamplerIndex(const String& paramName, int32& result)
			{
				const ShaderConstant* sc = m_constantTable->getConstant(paramName);
				if (sc && sc->IsSampler)
				{
					result = sc->SamplerIndex;
					return true;
				}
				return false;
			}

			void NRSShader::KeyNotFoundError(const String& name)
			{
				Apoc3D::Core::ApocLog(LOG_Graphics, L"Shader parameter " + name + L" not found.", LOGLVL_Warning);
			}

			//////////////////////////////////////////////////////////////////////////

			void NRSVertexShader::SetTexture(int samIndex, Texture* tex)
			{

			}
			void NRSVertexShader::SetSamplerState(int samIndex, const ShaderSamplerState &state)
			{
				m_device->getNativeStateManager()->SetVertexSampler(samIndex, state);

			}

			//////////////////////////////////////////////////////////////////////////

			void NRSPixelShader::SetTexture(int samIndex, Texture* tex)
			{
				m_device->getNativeStateManager()->SetTexture(samIndex, static_cast<NRSTexture*>(tex));
			}
			void NRSPixelShader::SetSamplerState(int samIndex, const ShaderSamplerState &state)
			{
				m_device->getNativeStateManager()->SetPixelSampler(samIndex, state);
			}

		}
	}
}