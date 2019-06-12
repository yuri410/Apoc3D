#pragma once
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

#ifndef D3D9PIXELSHADER_H
#define D3D9PIXELSHADER_H

#include "D3D9Shader.h"

namespace Apoc3D
{
	namespace Graphics
	{
		namespace D3D9RenderSystem
		{
			class D3D9PixelShader : public D3D9Shader<IDirect3DPixelShader9, 
				&IDirect3DDevice9::CreatePixelShader, 
				&IDirect3DDevice9::SetPixelShaderConstantF,
				&IDirect3DDevice9::SetPixelShaderConstantI, 
				&IDirect3DDevice9::SetPixelShaderConstantB>
			{
			public:
				IDirect3DPixelShader9* getD3DPS() const { return m_shader; }

				D3D9PixelShader(D3D9RenderDevice* device, const byte* byteCode)
					: D3D9Shader(device, byteCode) { }
				~D3D9PixelShader() { }

				virtual void SetTexture(int samIndex, Texture* tex);
				virtual void SetSamplerState(int samIndex, const ShaderSamplerState &state);

				virtual ShaderType getType() const { return ShaderType::Pixel; }
			};

			class D3D9VertexShader : public D3D9Shader<IDirect3DVertexShader9, 
				&IDirect3DDevice9::CreateVertexShader, 
				&IDirect3DDevice9::SetVertexShaderConstantF,
				&IDirect3DDevice9::SetVertexShaderConstantI, 
				&IDirect3DDevice9::SetVertexShaderConstantB>
			{
			public:
				IDirect3DVertexShader9* getD3DVS() const { return m_shader; }

				D3D9VertexShader(D3D9RenderDevice* device, const byte* byteCode)
					: D3D9Shader(device, byteCode) { }
				~D3D9VertexShader() { }

				virtual void SetTexture(int samIndex, Texture* tex);
				virtual void SetSamplerState(int samIndex, const ShaderSamplerState &state);

				virtual ShaderType getType() const { return ShaderType::Vertex; }
			};
		}
	}
}

#endif