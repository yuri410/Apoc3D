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

#include "D3D9RenderStateManager.h"

#include "D3D9RenderDevice.h"
#include "D3D9Utils.h"
#include "D3D9Texture.h"
#include "apoc3d/Math/Math.h"

namespace Apoc3D
{
	namespace Graphics
	{
		namespace D3D9RenderSystem
		{

			NativeD3DStateManager::NativeD3DStateManager(D3D9RenderDevice* device)
				: m_device(device)
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
			void NativeD3DStateManager::SetVertexSampler(int32 samplerIndex, const ShaderSamplerState& state)
			{
				if (m_vertexSamplers)
				{
					assert(samplerIndex >= 0 && samplerIndex < 4);

					D3DDevice* dev = m_device->getDevice();
					ShaderSamplerState& curState = m_vertexSamplers[samplerIndex];

					SetSampler(samplerIndex + D3DVERTEXTEXTURESAMPLER0, curState, state);
				}
			}
			void NativeD3DStateManager::SetPixelSampler(int32 samplerIndex, const ShaderSamplerState& state)
			{
				D3DDevice* dev = m_device->getDevice();
				
				assert(samplerIndex >= 0 && samplerIndex < m_textureSlotCount);

				ShaderSamplerState& curState = m_pixelSamplers[samplerIndex];
				SetSampler(samplerIndex, curState, state);
			}

			void NativeD3DStateManager::SetSampler(DWORD samplerIndex, ShaderSamplerState& curState, const ShaderSamplerState& state)
			{
				D3DDevice* dev = m_device->getDevice();

				if (state.AddressU != curState.AddressU)
				{
					curState.AddressU = state.AddressU;
					dev->SetSamplerState(samplerIndex, D3DSAMP_ADDRESSU, D3D9Utils::ConvertTextureAddress(state.AddressU));
				}

				if (curState.AddressV != state.AddressV)
				{
					curState.AddressV = state.AddressV;
					dev->SetSamplerState(samplerIndex, D3DSAMP_ADDRESSV, D3D9Utils::ConvertTextureAddress(state.AddressV));
				}

				if (curState.AddressW != state.AddressW)
				{
					curState.AddressW = state.AddressW;
					dev->SetSamplerState(samplerIndex, D3DSAMP_ADDRESSW, D3D9Utils::ConvertTextureAddress(state.AddressW));
				}

				if (curState.BorderColor != state.BorderColor)
				{
					curState.BorderColor = state.BorderColor;
					dev->SetSamplerState(samplerIndex, D3DSAMP_BORDERCOLOR, state.BorderColor);
				}

				if (curState.MagFilter != state.MagFilter)
				{
					curState.MagFilter = state.MagFilter;
					dev->SetSamplerState(samplerIndex, D3DSAMP_MAGFILTER, D3D9Utils::ConvertTextureFilter(state.MagFilter));
				}

				if (curState.MinFilter != state.MinFilter)
				{
					curState.MinFilter = state.MinFilter;
					dev->SetSamplerState(samplerIndex, D3DSAMP_MINFILTER, D3D9Utils::ConvertTextureFilter(state.MinFilter));
				}

				if (curState.MipFilter != state.MipFilter)
				{
					curState.MipFilter = state.MipFilter;
					dev->SetSamplerState(samplerIndex, D3DSAMP_MIPFILTER, D3D9Utils::ConvertTextureFilter(state.MipFilter));
				}

				int maxAnis = Math::Max(state.MaxAnisotropy, 1);
				if (curState.MaxAnisotropy != maxAnis)
				{
					curState.MaxAnisotropy = maxAnis;
					dev->SetSamplerState(samplerIndex, D3DSAMP_MAXANISOTROPY, maxAnis);
				}

				if (curState.MaxMipLevel != state.MaxMipLevel)
				{
					curState.MaxMipLevel = state.MaxMipLevel;
					dev->SetSamplerState(samplerIndex, D3DSAMP_MAXMIPLEVEL, state.MaxMipLevel);
				}

				if (curState.MipMapLODBias != state.MipMapLODBias)
				{
					curState.MipMapLODBias = state.MipMapLODBias;
					dev->SetSamplerState(samplerIndex, D3DSAMP_MIPMAPLODBIAS, state.MipMapLODBias);
				}
			}

			const ShaderSamplerState& NativeD3DStateManager::getPixelSampler(int32 samplerIndex) const { return m_pixelSamplers[samplerIndex]; }
			const ShaderSamplerState& NativeD3DStateManager::getVertexSampler(int32 samplerIndex) const { return m_vertexSamplers[samplerIndex]; }

			/************************************************************************/
			/* Color Write                                                          */
			/************************************************************************/

			ColorWriteMasks NativeD3DStateManager::GetColorWriteMasks(int32 rtIndex)
			{
				if (rtIndex >= 0 && rtIndex < countof(m_colorWriteMasks))
				{
					return m_colorWriteMasks[rtIndex];
				}
				return ColorWrite_All;
			}

			void NativeD3DStateManager::SetColorWriteMasks(int32 rtIndex, ColorWriteMasks masks)
			{
				if (rtIndex >= 0 && rtIndex < countof(m_colorWriteMasks))
				{
					m_colorWriteMasks[rtIndex] = masks;

					D3DRENDERSTATETYPE stateSlot = D3DRS_COLORWRITEENABLE;

					if (rtIndex == 1)
						stateSlot = D3DRS_COLORWRITEENABLE1;
					else if (rtIndex == 2)
						stateSlot = D3DRS_COLORWRITEENABLE2;
					else if (rtIndex == 3)
						stateSlot = D3DRS_COLORWRITEENABLE3;

					DWORD writeFlags = 0;

					if (masks & ColorWrite_Red)
						writeFlags |= D3DCOLORWRITEENABLE_RED;
					if (masks & ColorWrite_Green)
						writeFlags |= D3DCOLORWRITEENABLE_GREEN;
					if (masks & ColorWrite_Blue)
						writeFlags |= D3DCOLORWRITEENABLE_BLUE;
					if (masks & ColorWrite_Alpha)
						writeFlags |= D3DCOLORWRITEENABLE_ALPHA;

					D3DDevice* dev = m_device->getDevice();
					HRESULT hr = dev->SetRenderState(D3DRS_COLORWRITEENABLE, writeFlags);
					assert(SUCCEEDED(hr));
				}
			}

			void NativeD3DStateManager::SetTexture(int32 i, D3D9Texture* tex)
			{
				assert(i >= 0 && i < m_textureSlotCount);
				if (m_textureSlots[i] != tex)
				{
					D3DBaseTexture* value = NULL;
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

				SetAlphaTestParameters(false, CompareFunction::Always, 0);
				SetAlphaBlend(false, BlendFunction::Add, Blend::One, Blend::Zero, 0xffffffff);
				SetSeparateAlphaBlend(false, BlendFunction::Add, Blend::One, Blend::Zero);
				SetDepth(zEnabled == TRUE, zEnabled == TRUE, 0, 0, CompareFunction::LessEqual);

				float psize;
				dev->GetRenderState(D3DRS_POINTSIZE, reinterpret_cast<DWORD*>(&psize));
				SetPointParameters(psize, 256, 1, false);

				SetStencil(false, StencilOperation::Keep, StencilOperation::Keep, StencilOperation::Keep, 0, CompareFunction::Always, 0xFFFFFFFF, 0xFFFFFFFF);
				SetStencilTwoSide(false, StencilOperation::Keep, StencilOperation::Keep, StencilOperation::Keep, CompareFunction::Always);

				SetFillMode(FillMode::Solid);
				SetCullMode(CullMode::None);

				D3DCAPS9 caps;
				dev->GetDeviceCaps(&caps);
				
				SetColorWriteMasks(0, ColorWrite_All);
				SetColorWriteMasks(1, ColorWrite_All);
				SetColorWriteMasks(2, ColorWrite_All);
				SetColorWriteMasks(3, ColorWrite_All);
				
				if (caps.VertexShaderVersion >= D3DVS_VERSION((uint)3, (uint)0))
				{
					if (m_vertexSamplers)
						delete[] m_vertexSamplers;

					m_vertexSamplers = new ShaderSamplerState[4];
					for (int i = 0; i < 4; i++)
					{
						m_vertexSamplers[i].AddressU = TextureAddressMode::Wrap;
						m_vertexSamplers[i].AddressV = TextureAddressMode::Wrap;
						m_vertexSamplers[i].AddressW = TextureAddressMode::Wrap;
						m_vertexSamplers[i].BorderColor = 0x00000000;
						m_vertexSamplers[i].MagFilter = TextureFilter::Point;
						m_vertexSamplers[i].MinFilter = TextureFilter::Point;
						m_vertexSamplers[i].MipFilter = TextureFilter::None;
						m_vertexSamplers[i].MipMapLODBias = 0;
						m_vertexSamplers[i].MaxMipLevel = 0;
						m_vertexSamplers[i].MaxAnisotropy = 1;

						dev->SetSamplerState(i + D3DVERTEXTEXTURESAMPLER0, D3DSAMP_ADDRESSU, D3DTADDRESS_WRAP);
						dev->SetSamplerState(i + D3DVERTEXTEXTURESAMPLER0, D3DSAMP_ADDRESSV, D3DTADDRESS_WRAP);
						dev->SetSamplerState(i + D3DVERTEXTEXTURESAMPLER0, D3DSAMP_ADDRESSW, D3DTADDRESS_WRAP);
						dev->SetSamplerState(i + D3DVERTEXTEXTURESAMPLER0, D3DSAMP_BORDERCOLOR, 0x00000000);
						dev->SetSamplerState(i + D3DVERTEXTEXTURESAMPLER0, D3DSAMP_MAGFILTER, D3DTEXF_POINT);
						dev->SetSamplerState(i + D3DVERTEXTEXTURESAMPLER0, D3DSAMP_MINFILTER, D3DTEXF_POINT);
						dev->SetSamplerState(i + D3DVERTEXTEXTURESAMPLER0, D3DSAMP_MIPFILTER, D3DTEXF_NONE);
						dev->SetSamplerState(i + D3DVERTEXTEXTURESAMPLER0, D3DSAMP_MIPMAPLODBIAS, 0);
						dev->SetSamplerState(i + D3DVERTEXTEXTURESAMPLER0, D3DSAMP_MAXMIPLEVEL, 0);
						dev->SetSamplerState(i + D3DVERTEXTEXTURESAMPLER0, D3DSAMP_MAXANISOTROPY, 1);
					}
				}
				else
				{
					m_vertexSamplers = NULL;
				}
				
				if (m_pixelSamplers)
					delete[] m_pixelSamplers;

				m_pixelSamplers = new ShaderSamplerState[caps.MaxSimultaneousTextures];
				for (DWORD i = 0; i < caps.MaxSimultaneousTextures; i++)
				{
					m_pixelSamplers[i].AddressU = TextureAddressMode::Wrap;
					m_pixelSamplers[i].AddressV = TextureAddressMode::Wrap;
					m_pixelSamplers[i].AddressW = TextureAddressMode::Wrap;
					m_pixelSamplers[i].BorderColor = 0x00000000;
					m_pixelSamplers[i].MagFilter = TextureFilter::Point;
					m_pixelSamplers[i].MinFilter = TextureFilter::Point;
					m_pixelSamplers[i].MipFilter = TextureFilter::None;
					m_pixelSamplers[i].MipMapLODBias = 0;
					m_pixelSamplers[i].MaxMipLevel = 0;
					m_pixelSamplers[i].MaxAnisotropy = 1;

					dev->SetSamplerState(i, D3DSAMP_ADDRESSU, D3DTADDRESS_WRAP);
					dev->SetSamplerState(i, D3DSAMP_ADDRESSV, D3DTADDRESS_WRAP);
					dev->SetSamplerState(i, D3DSAMP_ADDRESSW, D3DTADDRESS_WRAP);
					dev->SetSamplerState(i, D3DSAMP_BORDERCOLOR, 0x00000000);
					dev->SetSamplerState(i, D3DSAMP_MAGFILTER, D3DTEXF_POINT);
					dev->SetSamplerState(i, D3DSAMP_MINFILTER, D3DTEXF_POINT);
					dev->SetSamplerState(i, D3DSAMP_MIPFILTER, D3DTEXF_NONE);
					dev->SetSamplerState(i, D3DSAMP_MIPMAPLODBIAS, 0);
					dev->SetSamplerState(i, D3DSAMP_MAXMIPLEVEL, 0);
					dev->SetSamplerState(i, D3DSAMP_MAXANISOTROPY, 1);
				}

				if (m_textureSlots)
					delete[] m_textureSlots;

				m_textureSlots = new D3D9Texture*[caps.MaxSimultaneousTextures]();
				m_textureSlotCount = (int32)caps.MaxSimultaneousTextures;
			}



			D3D9RenderStateManager::D3D9RenderStateManager(D3D9RenderDevice* device, NativeD3DStateManager* nsmgr)
				: RenderStateManager(device), m_device(device),  m_stMgr(nsmgr)
			{
				
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
			ColorWriteMasks D3D9RenderStateManager::getColorWriteMasks(uint32 rtIndex)
			{
				return m_stMgr->GetColorWriteMasks(rtIndex);
			}

			void D3D9RenderStateManager::setColorWriteMasks(uint32 rtIndex, ColorWriteMasks masks)
			{
				m_stMgr->SetColorWriteMasks(rtIndex, masks);
			}
		}
	}
}