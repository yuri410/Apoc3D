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

#include "D3D9RenderStateManager.h"

#include "D3D9RenderDevice.h"
#include "D3D9Utils.h"
#include "D3D9Texture.h"

namespace Apoc3D
{
	namespace Graphics
	{
		namespace D3D9RenderSystem
		{

			NativeD3DStateManager::NativeD3DStateManager(D3D9RenderDevice* device)
				: m_device(device), m_vertexSamplers(NULL), m_pixelSamplers(NULL)
			{
				InitializeDefaultState();
			}
			NativeD3DStateManager::~NativeD3DStateManager()
			{
				if (m_vertexSamplers)
					delete[] m_vertexSamplers;

				delete[] m_pixelSamplers;
				delete[] m_textureSlots;
			}

			void NativeD3DStateManager::SetCullMode(CullMode mode)
			{
				m_cachedCullMode = mode;

				D3DCULL cull = D3D9Utils::ConvertCullMode(mode);
				HRESULT hr = m_device->getDevice()->SetRenderState(D3DRS_CULLMODE, cull);
				assert(SUCCEEDED(hr));
			}
			void NativeD3DStateManager::SetFillMode(FillMode mode)
			{
				m_cachedFillMode = mode;

				D3DFILLMODE fill = D3D9Utils::ConvertFillMode(mode);
				HRESULT hr = m_device->getDevice()->SetRenderState(D3DRS_FILLMODE, fill);
				assert(SUCCEEDED(hr));
			}
			void NativeD3DStateManager::SetAlphaTestParameters(bool enable, uint32 reference)
			{
				m_cachedAlphaTestEnable = enable;
				m_cachedAlphaReference = reference;

				D3DDevice* dev = m_device->getDevice();
				HRESULT hr = dev->SetRenderState(D3DRS_ALPHATESTENABLE, enable ? TRUE : FALSE);
				assert(SUCCEEDED(hr));
				hr = dev->SetRenderState(D3DRS_ALPHAREF, reference);
				assert(SUCCEEDED(hr));
			}
			void NativeD3DStateManager::SetAlphaTestParameters(bool enable, CompareFunction func, uint32 reference)
			{
				m_cachedAlphaTestEnable = enable;
				m_cachedAlphaReference = reference;
				m_cachedAlphaTestFunction = func;

				D3DDevice* dev = m_device->getDevice();
				HRESULT hr = dev->SetRenderState(D3DRS_ALPHATESTENABLE, enable ? TRUE : FALSE);
				assert(SUCCEEDED(hr));
				hr = dev->SetRenderState(D3DRS_ALPHAREF, reference);
				assert(SUCCEEDED(hr));
				hr = dev->SetRenderState(D3DRS_ALPHAFUNC, D3D9Utils::ConvertCompare(func));
				assert(SUCCEEDED(hr));

			}
			void NativeD3DStateManager::SetAlphaBlend(bool enable, BlendFunction func, Blend srcBlend, Blend dstBlend)
			{
				m_cachedAlphaBlendEnable = enable;
				m_cachedAlphaBlendFunction = func;
				m_cachedAlphaSourceBlend = srcBlend;
				m_cachedAlphaDestBlend = dstBlend;

				D3DDevice* dev = m_device->getDevice();
				HRESULT hr = dev->SetRenderState(D3DRS_ALPHABLENDENABLE, enable ? TRUE : FALSE);
				assert(SUCCEEDED(hr));
				hr = dev->SetRenderState(D3DRS_BLENDOP, D3D9Utils::ConvertBlendFunction(func));
				assert(SUCCEEDED(hr));
				hr = dev->SetRenderState(D3DRS_SRCBLEND, D3D9Utils::ConvertBlend(srcBlend));
				assert(SUCCEEDED(hr));
				hr = dev->SetRenderState(D3DRS_DESTBLEND, D3D9Utils::ConvertBlend(dstBlend));
				assert(SUCCEEDED(hr));
			}
			void NativeD3DStateManager::SetAlphaBlend(bool enable, BlendFunction func, Blend srcBlend, Blend dstBlend, uint32 factor)
			{
				m_cachedAlphaBlendEnable = enable;
				m_cachedAlphaBlendFunction = func;
				m_cachedAlphaSourceBlend = srcBlend;
				m_cachedAlphaDestBlend = dstBlend;
				m_cachedAlphaBlendFactor = factor;

				D3DDevice* dev = m_device->getDevice();
				HRESULT hr = dev->SetRenderState(D3DRS_ALPHABLENDENABLE, enable ? TRUE : FALSE);
				assert(SUCCEEDED(hr));
				hr = dev->SetRenderState(D3DRS_BLENDOP, D3D9Utils::ConvertBlendFunction(func));
				assert(SUCCEEDED(hr));
				hr = dev->SetRenderState(D3DRS_SRCBLEND, D3D9Utils::ConvertBlend(srcBlend));
				assert(SUCCEEDED(hr));
				hr = dev->SetRenderState(D3DRS_DESTBLEND, D3D9Utils::ConvertBlend(dstBlend));
				assert(SUCCEEDED(hr));
				hr = dev->SetRenderState(D3DRS_BLENDFACTOR, factor);
				assert(SUCCEEDED(hr));
			}

			void NativeD3DStateManager::setAlphaBlendEnable(bool enable)
			{
				m_cachedAlphaBlendEnable = enable;
				HRESULT hr = m_device->getDevice()->SetRenderState(D3DRS_ALPHABLENDENABLE, enable ? TRUE : FALSE);
				assert(SUCCEEDED(hr));
			}
			void NativeD3DStateManager::setAlphaBlendOperation(BlendFunction func)
			{
				m_cachedAlphaBlendFunction = func;
				HRESULT hr = m_device->getDevice()->SetRenderState(D3DRS_BLENDOP, D3D9Utils::ConvertBlendFunction(func));
				assert(SUCCEEDED(hr));
			}
			void NativeD3DStateManager::setAlphaSourceBlend(Blend srcBlend)
			{
				m_cachedAlphaSourceBlend = srcBlend;
				HRESULT hr = m_device->getDevice()->SetRenderState(D3DRS_SRCBLEND, D3D9Utils::ConvertBlend(srcBlend));
				assert(SUCCEEDED(hr));
			}
			void NativeD3DStateManager::setAlphaDestinationBlend(Blend dstBlend)
			{
				m_cachedAlphaDestBlend = dstBlend;
				HRESULT hr = m_device->getDevice()->SetRenderState(D3DRS_DESTBLEND, D3D9Utils::ConvertBlend(dstBlend));
				assert(SUCCEEDED(hr));
			}

			void NativeD3DStateManager::SetSeparateAlphaBlend(bool enable, BlendFunction func, Blend srcBlend, Blend dstBlend)
			{
				m_cachedSepAlphaBlendEnable = enable;
				m_cachedSepAlphaBlendFunction = func;
				m_cachedSepAlphaSourceBlend = srcBlend;
				m_cachedSepAlphaDestBlend = dstBlend;

				D3DDevice* dev = m_device->getDevice();
				HRESULT hr = dev->SetRenderState(D3DRS_SEPARATEALPHABLENDENABLE, enable ? TRUE : FALSE);
				assert(SUCCEEDED(hr));
				hr = dev->SetRenderState(D3DRS_BLENDOPALPHA, D3D9Utils::ConvertBlendFunction(func));
				assert(SUCCEEDED(hr));
				hr = dev->SetRenderState(D3DRS_SRCBLENDALPHA, D3D9Utils::ConvertBlend(srcBlend));
				assert(SUCCEEDED(hr));
				hr = dev->SetRenderState(D3DRS_DESTBLENDALPHA, D3D9Utils::ConvertBlend(dstBlend));
				assert(SUCCEEDED(hr));
				//dev->SetRenderState(D3DRS_BLENDFACTOR, factor);

			}

			void NativeD3DStateManager::SetDepth(bool enable, bool writeEnable)
			{
				m_cachedDepthBufferEnabled = enable;
				m_cachedDepthBufferWriteEnabled = writeEnable;

				D3DDevice* dev = m_device->getDevice();
				HRESULT hr = dev->SetRenderState(D3DRS_ZENABLE, enable ? TRUE : FALSE);
				assert(SUCCEEDED(hr));
				hr = dev->SetRenderState(D3DRS_ZWRITEENABLE, writeEnable ? TRUE : FALSE);
				assert(SUCCEEDED(hr));
			}
			void NativeD3DStateManager::SetDepth(bool enable, bool writeEnable, float bias, float slopebias, CompareFunction compare)
			{
				m_cachedDepthBufferEnabled = enable;
				m_cachedDepthBufferWriteEnabled = writeEnable;
				m_cachedDepthBias = bias;
				m_cachedSlopeScaleDepthBias = slopebias;
				m_cachedDepthBufferFunction = compare;

				D3DDevice* dev = m_device->getDevice();
				HRESULT hr = dev->SetRenderState(D3DRS_ZENABLE, enable ? TRUE : FALSE);
				assert(SUCCEEDED(hr));
				hr = dev->SetRenderState(D3DRS_ZWRITEENABLE, writeEnable ? TRUE : FALSE);
				assert(SUCCEEDED(hr));
				hr = dev->SetRenderState(D3DRS_DEPTHBIAS, *(DWORD*)&bias);
				assert(SUCCEEDED(hr));
				hr = dev->SetRenderState(D3DRS_SLOPESCALEDEPTHBIAS, *(DWORD*)&slopebias);
				assert(SUCCEEDED(hr));
				hr = dev->SetRenderState(D3DRS_ZFUNC, D3D9Utils::ConvertCompare(compare));
				assert(SUCCEEDED(hr));
			}
			void NativeD3DStateManager::SetPointParameters(float size, float maxSize, float minSize, bool pointSprite)
			{
				m_cachedPointSize = size;
				m_cachedPointSizeMax = maxSize;
				m_cachedPointSizeMin = minSize;
				m_cachedPointSpriteEnabled = pointSprite;

				D3DDevice* dev = m_device->getDevice();
				HRESULT hr = dev->SetRenderState(D3DRS_POINTSIZE, *((DWORD*)&size));
				assert(SUCCEEDED(hr));
				hr = dev->SetRenderState(D3DRS_POINTSIZE_MAX, *((DWORD*)&maxSize));
				assert(SUCCEEDED(hr));
				hr = dev->SetRenderState(D3DRS_POINTSIZE_MIN, *((DWORD*)&minSize));
				assert(SUCCEEDED(hr));
				hr = dev->SetRenderState(D3DRS_POINTSPRITEENABLE, pointSprite ? TRUE : FALSE);
				assert(SUCCEEDED(hr));
			}
			void NativeD3DStateManager::SetStencil(bool enabled, StencilOperation fail, StencilOperation depthFail, StencilOperation pass, 
				uint32 ref, CompareFunction func, uint32 mask, uint32 writemask)
			{
				m_cachedStencilEnabled = enabled;
				m_cachedStencilFail = fail;
				m_cachedStencilDepthFail = depthFail;
				m_cachedStencilPass = pass;
				m_cachedRefrenceStencil = ref;
				m_cachedStencilFunction = func;
				m_cachedStencilMask = mask;
				m_cachedStencilWriteMask = writemask;

				D3DDevice* dev = m_device->getDevice();
				HRESULT hr = dev->SetRenderState(D3DRS_STENCILENABLE, enabled ? TRUE : FALSE);
				assert(SUCCEEDED(hr));
				hr = dev->SetRenderState(D3DRS_STENCILFAIL, D3D9Utils::ConvertStencilOperation(fail));
				assert(SUCCEEDED(hr));
				hr = dev->SetRenderState(D3DRS_STENCILZFAIL, D3D9Utils::ConvertStencilOperation(depthFail));
				assert(SUCCEEDED(hr));
				hr = dev->SetRenderState(D3DRS_STENCILPASS, D3D9Utils::ConvertStencilOperation(pass));
				assert(SUCCEEDED(hr));
				hr = dev->SetRenderState(D3DRS_STENCILREF, ref);
				assert(SUCCEEDED(hr));
				hr = dev->SetRenderState(D3DRS_STENCILFUNC, D3D9Utils::ConvertCompare(func));
				assert(SUCCEEDED(hr));
				hr = dev->SetRenderState(D3DRS_STENCILMASK, mask);
				assert(SUCCEEDED(hr));
				hr = dev->SetRenderState(D3DRS_STENCILWRITEMASK, writemask);
				assert(SUCCEEDED(hr));

			}
			void NativeD3DStateManager::SetStencilTwoSide(bool enabled, StencilOperation fail, StencilOperation depthFail, StencilOperation pass, CompareFunction func)
			{
				m_cachedTwoSidedStencilMode = enabled;
				m_cachedCounterClockwiseStencilFail = fail;
				m_cachedCounterClockwiseStencilDepthBufferFail = depthFail;
				m_cachedCounterClockwiseStencilPass = pass;
				m_cachedCounterClockwiseStencilFunction = func;

				D3DDevice* dev = m_device->getDevice();
				HRESULT hr = dev->SetRenderState(D3DRS_TWOSIDEDSTENCILMODE, enabled ? TRUE : FALSE);
				assert(SUCCEEDED(hr));
				hr = dev->SetRenderState(D3DRS_CCW_STENCILFAIL, D3D9Utils::ConvertStencilOperation(fail));
				assert(SUCCEEDED(hr));
				hr = dev->SetRenderState(D3DRS_CCW_STENCILZFAIL, D3D9Utils::ConvertStencilOperation(depthFail));
				assert(SUCCEEDED(hr));
				hr = dev->SetRenderState(D3DRS_CCW_STENCILPASS, D3D9Utils::ConvertStencilOperation(pass));
				assert(SUCCEEDED(hr));
				hr = dev->SetRenderState(D3DRS_CCW_STENCILFUNC, D3D9Utils::ConvertCompare(func));
				assert(SUCCEEDED(hr));

			}
			void NativeD3DStateManager::SetVertexSampler(int samplerIndex, const ShaderSamplerState& state)
			{
				if (m_vertexSamplers)
				{
					assert(samplerIndex<4);
					D3DDevice* dev = m_device->getDevice();

					if (m_vertexSamplers[samplerIndex].AddressU != state.AddressU)
					{
						dev->SetSamplerState(samplerIndex+D3DVERTEXTEXTURESAMPLER0, D3DSAMP_ADDRESSU, 
							D3D9Utils::ConvertTextureAddress(state.AddressU));
						m_vertexSamplers[samplerIndex].AddressU = state.AddressU;
					}

					if (m_vertexSamplers[samplerIndex].AddressV != state.AddressV)
					{
						dev->SetSamplerState(samplerIndex+D3DVERTEXTEXTURESAMPLER0, D3DSAMP_ADDRESSV, 
							D3D9Utils::ConvertTextureAddress(state.AddressV));
						m_vertexSamplers[samplerIndex].AddressV = state.AddressV;
					}

					if (m_vertexSamplers[samplerIndex].AddressW != state.AddressW)
					{
						dev->SetSamplerState(samplerIndex+D3DVERTEXTEXTURESAMPLER0, D3DSAMP_ADDRESSW, 
							D3D9Utils::ConvertTextureAddress(state.AddressW));
						m_vertexSamplers[samplerIndex].AddressW = state.AddressW;
					}

					if (m_vertexSamplers[samplerIndex].BorderColor != state.BorderColor)
					{
						dev->SetSamplerState(samplerIndex+D3DVERTEXTEXTURESAMPLER0, D3DSAMP_BORDERCOLOR, 
							state.BorderColor);
						m_vertexSamplers[samplerIndex].BorderColor = state.BorderColor;
					}

					if (m_vertexSamplers[samplerIndex].MagFilter != state.MagFilter)
					{
						dev->SetSamplerState(samplerIndex+D3DVERTEXTEXTURESAMPLER0, D3DSAMP_MAGFILTER, 
							D3D9Utils::ConvertTextureFilter(state.MagFilter));
						m_vertexSamplers[samplerIndex].MagFilter = state.MagFilter;
					}

					if (m_vertexSamplers[samplerIndex].MinFilter != state.MinFilter)
					{
						dev->SetSamplerState(samplerIndex+D3DVERTEXTEXTURESAMPLER0, D3DSAMP_MINFILTER, 
							D3D9Utils::ConvertTextureFilter(state.MinFilter));
						m_vertexSamplers[samplerIndex].MinFilter = state.MinFilter;
					}

					if (m_vertexSamplers[samplerIndex].MipFilter != state.MipFilter)
					{
						dev->SetSamplerState(samplerIndex+D3DVERTEXTEXTURESAMPLER0, D3DSAMP_MIPFILTER, 
							D3D9Utils::ConvertTextureFilter(state.MipFilter));
						m_vertexSamplers[samplerIndex].MipFilter = state.MipFilter;
					}

					int maxAnis = std::min<int>(state.MaxAnisotropy,1);
					if (m_vertexSamplers[samplerIndex].MaxAnisotropy != maxAnis)
					{
						dev->SetSamplerState(samplerIndex+D3DVERTEXTEXTURESAMPLER0, D3DSAMP_MAXANISOTROPY, 
							maxAnis);
						m_vertexSamplers[samplerIndex].MaxAnisotropy = maxAnis;
					}

					if (m_vertexSamplers[samplerIndex].MaxMipLevel != state.MaxMipLevel)
					{
						dev->SetSamplerState(samplerIndex+D3DVERTEXTEXTURESAMPLER0, D3DSAMP_MAXMIPLEVEL, 
							state.MaxMipLevel);
						m_vertexSamplers[samplerIndex].MaxMipLevel = state.MaxMipLevel;
					}

					if (m_vertexSamplers[samplerIndex].MipMapLODBias != state.MipMapLODBias)
					{
						dev->SetSamplerState(samplerIndex+D3DVERTEXTEXTURESAMPLER0, D3DSAMP_MIPMAPLODBIAS, 
							state.MipMapLODBias);
						m_vertexSamplers[samplerIndex].MipMapLODBias = state.MipMapLODBias;
					}
				}
			}
			void NativeD3DStateManager::SetPixelSampler(int samplerIndex, const ShaderSamplerState& state)
			{
				D3DDevice* dev = m_device->getDevice();

				if (m_pixelSamplers[samplerIndex].AddressU != state.AddressU)
				{
					dev->SetSamplerState(samplerIndex, D3DSAMP_ADDRESSU, D3D9Utils::ConvertTextureAddress(state.AddressU));
					m_pixelSamplers[samplerIndex].AddressU = state.AddressU;
				}
				
				if (m_pixelSamplers[samplerIndex].AddressV != state.AddressV)
				{
					dev->SetSamplerState(samplerIndex, D3DSAMP_ADDRESSV, D3D9Utils::ConvertTextureAddress(state.AddressV));
					m_pixelSamplers[samplerIndex].AddressV = state.AddressV;
				}

				if (m_pixelSamplers[samplerIndex].AddressW != state.AddressW)
				{
					dev->SetSamplerState(samplerIndex, D3DSAMP_ADDRESSW, D3D9Utils::ConvertTextureAddress(state.AddressW));
					m_pixelSamplers[samplerIndex].AddressW = state.AddressW;
				}

				if (m_pixelSamplers[samplerIndex].BorderColor != state.BorderColor)
				{
					dev->SetSamplerState(samplerIndex, D3DSAMP_BORDERCOLOR, state.BorderColor);
					m_pixelSamplers[samplerIndex].BorderColor = state.BorderColor;
				}
				
				if (m_pixelSamplers[samplerIndex].MagFilter != state.MagFilter)
				{
					dev->SetSamplerState(samplerIndex, D3DSAMP_MAGFILTER, D3D9Utils::ConvertTextureFilter(state.MagFilter));
					m_pixelSamplers[samplerIndex].MagFilter = state.MagFilter;
				}
				
				if (m_pixelSamplers[samplerIndex].MinFilter != state.MinFilter)
				{
					dev->SetSamplerState(samplerIndex, D3DSAMP_MINFILTER, D3D9Utils::ConvertTextureFilter(state.MinFilter));
					m_pixelSamplers[samplerIndex].MinFilter = state.MinFilter;
				}
				
				if (m_pixelSamplers[samplerIndex].MipFilter != state.MipFilter)
				{
					dev->SetSamplerState(samplerIndex, D3DSAMP_MIPFILTER, D3D9Utils::ConvertTextureFilter(state.MipFilter));
					m_pixelSamplers[samplerIndex].MipFilter = state.MipFilter;
				}
				
				int maxAnis = std::max<int>(state.MaxAnisotropy,1);
				if (m_pixelSamplers[samplerIndex].MaxAnisotropy != maxAnis)
				{
					dev->SetSamplerState(samplerIndex, D3DSAMP_MAXANISOTROPY, maxAnis);
					m_pixelSamplers[samplerIndex].MaxAnisotropy = maxAnis;
				}

				if (m_pixelSamplers[samplerIndex].MaxMipLevel != state.MaxMipLevel)
				{
					dev->SetSamplerState(samplerIndex, D3DSAMP_MAXMIPLEVEL, state.MaxMipLevel);
					m_pixelSamplers[samplerIndex].MaxMipLevel = state.MaxMipLevel;
				}

				if (m_pixelSamplers[samplerIndex].MipMapLODBias != state.MipMapLODBias)
				{
					dev->SetSamplerState(samplerIndex, D3DSAMP_MIPMAPLODBIAS, state.MipMapLODBias);
					m_pixelSamplers[samplerIndex].MipMapLODBias = state.MipMapLODBias;
				}

			}
			const ShaderSamplerState& NativeD3DStateManager::getPixelSampler(int samplerIndex) const { return m_pixelSamplers[samplerIndex]; }
			const ShaderSamplerState& NativeD3DStateManager::getVertexSampler(int samplerIndex) const { return m_vertexSamplers[samplerIndex]; }

			/************************************************************************/
			/* Color Write                                                          */
			/************************************************************************/

			void NativeD3DStateManager::getColorWriteEnabled0(bool& r, bool& g, bool& b, bool& a)
			{
				r = m_colorWrite0[0]; b = m_colorWrite0[1]; g = m_colorWrite0[2]; a = m_colorWrite0[3];
			}
			void NativeD3DStateManager::setColorWriteEnabled0(bool r, bool g, bool b, bool a)
			{
				DWORD writeFlags = 0;

				if (r)
					writeFlags |= D3DCOLORWRITEENABLE_RED;
				if (g)
					writeFlags |= D3DCOLORWRITEENABLE_GREEN;
				if (b)
					writeFlags |= D3DCOLORWRITEENABLE_BLUE;
				if (a)
					writeFlags |= D3DCOLORWRITEENABLE_ALPHA;


				D3DDevice* dev = m_device->getDevice();
				HRESULT hr = dev->SetRenderState(D3DRS_COLORWRITEENABLE, writeFlags);
				assert(SUCCEEDED(hr));
			}

			void NativeD3DStateManager::getColorWriteEnabled1(bool& r, bool& g, bool& b, bool& a)
			{
				r = m_colorWrite1[0]; b = m_colorWrite1[1]; g = m_colorWrite1[2]; a = m_colorWrite1[3];
			}
			void NativeD3DStateManager::setColorWriteEnabled1(bool r, bool g, bool b, bool a)
			{
				DWORD writeFlags = 0;

				if (r)
					writeFlags |= D3DCOLORWRITEENABLE_RED;
				if (g)
					writeFlags |= D3DCOLORWRITEENABLE_GREEN;
				if (b)
					writeFlags |= D3DCOLORWRITEENABLE_BLUE;
				if (a)
					writeFlags |= D3DCOLORWRITEENABLE_ALPHA;


				D3DDevice* dev = m_device->getDevice();
				HRESULT hr = dev->SetRenderState(D3DRS_COLORWRITEENABLE1, writeFlags);
				assert(SUCCEEDED(hr));
			}

			void NativeD3DStateManager::getColorWriteEnabled2(bool& r, bool& g, bool& b, bool& a)
			{
				r = m_colorWrite2[0]; b = m_colorWrite2[1]; g = m_colorWrite2[2]; a = m_colorWrite2[3];
			}
			void NativeD3DStateManager::setColorWriteEnabled2(bool r, bool g, bool b, bool a)
			{
				DWORD writeFlags = 0;

				if (r)
					writeFlags |= D3DCOLORWRITEENABLE_RED;
				if (g)
					writeFlags |= D3DCOLORWRITEENABLE_GREEN;
				if (b)
					writeFlags |= D3DCOLORWRITEENABLE_BLUE;
				if (a)
					writeFlags |= D3DCOLORWRITEENABLE_ALPHA;


				D3DDevice* dev = m_device->getDevice();
				HRESULT hr = dev->SetRenderState(D3DRS_COLORWRITEENABLE2, writeFlags);
				assert(SUCCEEDED(hr));
			}

			void NativeD3DStateManager::getColorWriteEnabled3(bool& r, bool& g, bool& b, bool& a)
			{
				r = m_colorWrite3[0]; b = m_colorWrite3[1]; g = m_colorWrite3[2]; a = m_colorWrite3[3];
			}
			void NativeD3DStateManager::setColorWriteEnabled3(bool r, bool g, bool b, bool a)
			{
				DWORD writeFlags = 0;

				if (r)
					writeFlags |= D3DCOLORWRITEENABLE_RED;
				if (g)
					writeFlags |= D3DCOLORWRITEENABLE_GREEN;
				if (b)
					writeFlags |= D3DCOLORWRITEENABLE_BLUE;
				if (a)
					writeFlags |= D3DCOLORWRITEENABLE_ALPHA;


				D3DDevice* dev = m_device->getDevice();
				HRESULT hr = dev->SetRenderState(D3DRS_COLORWRITEENABLE3, writeFlags);
				assert(SUCCEEDED(hr));
			}

			void NativeD3DStateManager::SetTexture(int i, D3D9Texture* tex)
			{
				assert(i<m_textureSlotCount);
				if (m_textureSlots[i] != tex)
				{
					D3DBaseTexture* value = NULL;
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
								value = d3dTex->getInternal3D();
							}
						}
					}
					m_device->getDevice()->SetTexture(i, value);
					m_textureSlots[i] = tex;
				}
			}

			void NativeD3DStateManager::InitializeDefaultState()
			{
				D3DDevice* dev = m_device->getDevice();

				DWORD zEnabled;
				dev->GetRenderState(D3DRS_ZENABLE, &zEnabled);

				SetAlphaTestParameters(false, COMFUN_Always, 0);
				SetAlphaBlend(false, BLFUN_Add, BLEND_One, BLEND_Zero, 0xffffffff);
				SetSeparateAlphaBlend(false, BLFUN_Add, BLEND_One, BLEND_Zero);
				SetDepth(zEnabled == TRUE, zEnabled == TRUE, 0, 0, COMFUN_LessEqual);

				float psize;
				dev->GetRenderState(D3DRS_POINTSIZE, reinterpret_cast<DWORD*>(&psize));
				SetPointParameters(psize, 256, 1, false);

				SetStencil(false, STOP_Keep, STOP_Keep, STOP_Keep, 0, COMFUN_Always, 0xFFFFFFFF, 0xFFFFFFFF);
				SetStencilTwoSide(false, STOP_Keep, STOP_Keep, STOP_Keep, COMFUN_Always);

				SetFillMode(FILL_Solid);
				SetCullMode(CULL_None);

				D3DCAPS9 caps;
				dev->GetDeviceCaps(&caps);

				m_colorWrite0[0] = m_colorWrite0[1] = m_colorWrite0[2] = m_colorWrite0[3] = true;
				m_colorWrite1[0] = m_colorWrite1[1] = m_colorWrite1[2] = m_colorWrite1[3] = true;
				m_colorWrite2[0] = m_colorWrite2[1] = m_colorWrite2[2] = m_colorWrite2[3] = true;
				m_colorWrite3[0] = m_colorWrite3[1] = m_colorWrite3[2] = m_colorWrite3[3] = true;

				if (caps.VertexShaderVersion >= D3DVS_VERSION((uint)3, (uint)0))
				{
					m_vertexSamplers = new ShaderSamplerState[4];
					for (int i=0;i<4;i++)
					{
						m_vertexSamplers[i].AddressU = TA_Wrap;
						m_vertexSamplers[i].AddressV = TA_Wrap;
						m_vertexSamplers[i].AddressW = TA_Wrap;
						m_vertexSamplers[i].BorderColor = 0x00000000;
						m_vertexSamplers[i].MagFilter = TFLT_Point;
						m_vertexSamplers[i].MinFilter = TFLT_Point;
						m_vertexSamplers[i].MipFilter = TFLT_None;
						m_vertexSamplers[i].MipMapLODBias = 0;
						m_vertexSamplers[i].MaxMipLevel = 0;
						m_vertexSamplers[i].MaxAnisotropy = 1;
					}
				}
				else
				{
					m_vertexSamplers = NULL;
				}
				
				m_pixelSamplers = new ShaderSamplerState[caps.MaxSimultaneousTextures];
				for (DWORD i=0;i<caps.MaxSimultaneousTextures;i++)
				{
					m_pixelSamplers[i].AddressU = TA_Wrap;
					m_pixelSamplers[i].AddressV = TA_Wrap;
					m_pixelSamplers[i].AddressW = TA_Wrap;
					m_pixelSamplers[i].BorderColor = 0x00000000;
					m_pixelSamplers[i].MagFilter = TFLT_Point;
					m_pixelSamplers[i].MinFilter = TFLT_Point;
					m_pixelSamplers[i].MipFilter = TFLT_None;
					m_pixelSamplers[i].MipMapLODBias = 0;
					m_pixelSamplers[i].MaxMipLevel = 0;
					m_pixelSamplers[i].MaxAnisotropy = 1;
				}

				m_textureSlots = new D3D9Texture*[caps.MaxSimultaneousTextures];
				for (DWORD i=0;i<caps.MaxSimultaneousTextures;i++)
					m_textureSlots[i] = nullptr;
				m_textureSlotCount = (int)caps.MaxSimultaneousTextures;
			}


			D3D9ClipPlane::D3D9ClipPlane(D3D9RenderDevice* device, D3D9RenderStateManager* mgr, int index)
				: m_manager(mgr), m_device(device), m_index(index)
			{

			}

			bool D3D9ClipPlane::getEnabled()
			{
				uint32 mask = 1<<m_index;
				return !!(m_manager->clipPlaneEnable & mask);
			}
			Plane D3D9ClipPlane::getPlane()
			{
				return m_cachedPlane;
			}

			void D3D9ClipPlane::setEnabled(bool value)
			{
				if (value)
				{
					uint32 mask = 1<<m_index;
					m_manager->clipPlaneEnable |= mask;
				}
				else
				{
					uint32 mask = 1<<m_index;
					m_manager->clipPlaneEnable ^= mask;
				}
				HRESULT hr = m_device->getDevice()->SetRenderState(D3DRS_CLIPPLANEENABLE, m_manager->clipPlaneEnable);
				assert(SUCCEEDED(hr));
			}
			void D3D9ClipPlane::setPlane(const Plane& plane)
			{
				m_cachedPlane = plane;

				HRESULT hr = m_device->getDevice()->SetClipPlane(m_index, &plane.X);
				assert(SUCCEEDED(hr));
			}

			
			D3D9RenderStateManager::D3D9RenderStateManager(D3D9RenderDevice* device, NativeD3DStateManager* nsmgr)
				: RenderStateManager(device), m_device(device), clipPlaneEnable(0), m_stMgr(nsmgr)
			{
				for (int i=0;i<32;i++)
				{
					m_clipPlanes[i] = D3D9ClipPlane(device, this, i);
				}
			}
			D3D9RenderStateManager::~D3D9RenderStateManager()
			{

			}

			/************************************************************************/
			/* Scissor Test                                                         */
			/************************************************************************/

			bool D3D9RenderStateManager::getScissorTestEnabled()
			{
				DWORD result;
				HRESULT hr = m_device->getDevice()->GetRenderState(D3DRS_SCISSORTESTENABLE, &result);
				assert(SUCCEEDED(hr));
				return !!result;
			}
			Apoc3D::Math::Rectangle D3D9RenderStateManager::getScissorTestRect()
			{
				RECT rect;
				HRESULT hr = m_device->getDevice()->GetScissorRect(&rect);
				assert(SUCCEEDED(hr));
				return Apoc3D::Math::Rectangle((int)rect.left, (int)rect.top, (int)(rect.right-rect.left), (int)(rect.bottom-rect.top));
			}
			void D3D9RenderStateManager::setScissorTest(bool enable, const Apoc3D::Math::Rectangle* r)
			{
				if (enable)
				{
					RECT rect = { 
						(LONG)r->X, 
						(LONG)r->Y,
						(LONG)r->getRight(),
						(LONG)r->getBottom()
					};

					// standardize rect ( make sure top left corner is at the top left, i.e. no negative w/h)
					if (rect.left>rect.right)
					{
						std::swap(rect.left, rect.right);
					}
					if (rect.top >rect.bottom)
					{
						std::swap(rect.top, rect.bottom);
					}

					// crop it
					D3DVIEWPORT9 vp;
					if (SUCCEEDED(m_device->getDevice()->GetViewport(&vp)))
					{
						if (rect.right > (LONG)(vp.X + vp.Width))
						{
							rect.right = vp.X + vp.Width;
						}
						if (rect.bottom > (LONG)(vp.Y + vp.Height))
						{
							rect.bottom = vp.Y + vp.Height;
						}
						if (rect.right < (LONG)vp.X)
						{
							rect.right = vp.X;
						}
						if (rect.bottom < (LONG)vp.Y)
						{
							rect.bottom = vp.Y;
						}


						if (rect.left > (LONG)(vp.X + vp.Width))
						{
							rect.left = vp.X + vp.Width;
						}
						if (rect.top > (LONG)(vp.Y + vp.Height))
						{
							rect.top = vp.Y + vp.Height;
						}
						if (rect.left < (LONG)vp.X)
						{
							rect.left = vp.X;
						}
						if (rect.top < (LONG)vp.Y)
						{
							rect.top = vp.Y;
						}

					}

					
					HRESULT hr = m_device->getDevice()->SetScissorRect(&rect);
					assert(SUCCEEDED(hr));
					hr = m_device->getDevice()->SetRenderState(D3DRS_SCISSORTESTENABLE, TRUE);
					assert(SUCCEEDED(hr));
				}
				else
				{
					HRESULT hr = m_device->getDevice()->SetRenderState(D3DRS_SCISSORTESTENABLE, FALSE);
					assert(SUCCEEDED(hr));
				}
			}

			/************************************************************************/
			/* Color Write                                                          */
			/************************************************************************/
			void D3D9RenderStateManager::getColorWriteEnabled(int rtIndex, bool& r, bool& g, bool& b, bool& a)
			{
				if (rtIndex == 0)
					m_stMgr->getColorWriteEnabled0(r,g,b,a);
				else if (rtIndex == 1)
					m_stMgr->getColorWriteEnabled1(r,g,b,a);
				else if (rtIndex == 2)
					m_stMgr->getColorWriteEnabled2(r,g,b,a);
				else if (rtIndex == 3)
					m_stMgr->getColorWriteEnabled3(r,g,b,a);
			}
			void D3D9RenderStateManager::setColorWriteEnabled(int rtIndex, bool r, bool g, bool b, bool a)
			{
				if (rtIndex == 0)
					m_stMgr->setColorWriteEnabled0(r,g,b,a);
				else if (rtIndex == 1)
					m_stMgr->setColorWriteEnabled1(r,g,b,a);
				else if (rtIndex == 2)
					m_stMgr->setColorWriteEnabled2(r,g,b,a);
				else if (rtIndex == 3)
					m_stMgr->setColorWriteEnabled3(r,g,b,a);
			}
		}
	}
}