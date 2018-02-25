
/* -----------------------------------------------------------------------
* This source file is part of Apoc3D Framework
*
* Copyright (c) 2009+ Tao Xin
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

#include "NRSRenderStateManager.h"

#include "NRSRenderDevice.h"
#include "NRSTexture.h"
#include "apoc3d/Math/Math.h"

namespace Apoc3D
{
	namespace Graphics
	{
		namespace NullRenderSystem
		{
			NativeStateManager::NativeStateManager(NRSRenderDevice* device)
				: m_device(device)
			{
				InitializeDefaultState();
			}
			NativeStateManager::~NativeStateManager()
			{
				if (m_vertexSamplers)
					delete[] m_vertexSamplers;

				delete[] m_pixelSamplers;
				delete[] m_textureSlots;
			}

			void NativeStateManager::SetCullMode(CullMode mode)
			{
				m_cullMode = mode;
			}
			void NativeStateManager::SetFillMode(FillMode mode)
			{
				m_fillMode = mode;
			}
			void NativeStateManager::SetAlphaTestParameters(bool enable, uint32 reference)
			{
				m_alphaTestEnable = enable;
				m_alphaReference = reference;
			}
			void NativeStateManager::SetAlphaTestParameters(bool enable, CompareFunction func, uint32 reference)
			{
				m_alphaTestEnable = enable;
				m_alphaReference = reference;
				m_alphaTestFunction = func;

			}
			void NativeStateManager::SetAlphaBlend(bool enable, BlendFunction func, Blend srcBlend, Blend dstBlend)
			{
				m_alphaBlendEnable = enable;
				m_alphaBlendFunction = func;
				m_alphaSourceBlend = srcBlend;
				m_alphaDestBlend = dstBlend;
			}
			void NativeStateManager::SetAlphaBlend(bool enable, BlendFunction func, Blend srcBlend, Blend dstBlend, uint32 factor)
			{
				m_alphaBlendEnable = enable;
				m_alphaBlendFunction = func;
				m_alphaSourceBlend = srcBlend;
				m_alphaDestBlend = dstBlend;
				m_alphaBlendFactor = factor;
			}

			void NativeStateManager::setAlphaBlendEnable(bool enable)
			{
				m_alphaBlendEnable = enable;
			}
			void NativeStateManager::setAlphaBlendOperation(BlendFunction func)
			{
				m_alphaBlendFunction = func;
			}
			void NativeStateManager::setAlphaSourceBlend(Blend srcBlend)
			{
				m_alphaSourceBlend = srcBlend;
			}
			void NativeStateManager::setAlphaDestinationBlend(Blend dstBlend)
			{
				m_alphaDestBlend = dstBlend;
			}

			void NativeStateManager::SetSeparateAlphaBlend(bool enable, BlendFunction func, Blend srcBlend, Blend dstBlend)
			{
				m_sepAlphaBlendEnable = enable;
				m_sepAlphaBlendFunction = func;
				m_sepAlphaSourceBlend = srcBlend;
				m_sepAlphaDestBlend = dstBlend;
			}

			void NativeStateManager::SetDepth(bool enable, bool writeEnable)
			{
				m_depthBufferEnabled = enable;
				m_depthBufferWriteEnabled = writeEnable;
			}
			void NativeStateManager::SetDepth(bool enable, bool writeEnable, float bias, float slopebias, CompareFunction compare)
			{
				m_depthBufferEnabled = enable;
				m_depthBufferWriteEnabled = writeEnable;
				m_depthBias = bias;
				m_slopeScaleDepthBias = slopebias;
				m_depthBufferFunction = compare;
			}
			void NativeStateManager::SetPointParameters(float size, float maxSize, float minSize, bool pointSprite)
			{
				m_pointSize = size;
				m_pointSizeMax = maxSize;
				m_pointSizeMin = minSize;
				m_pointSpriteEnabled = pointSprite;
			}
			void NativeStateManager::SetStencil(bool enabled, StencilOperation fail, StencilOperation depthFail, StencilOperation pass, 
				uint32 ref, CompareFunction func, uint32 mask, uint32 writemask)
			{
				m_stencilEnabled = enabled;
				m_stencilFail = fail;
				m_stencilDepthFail = depthFail;
				m_stencilPass = pass;
				m_refrenceStencil = ref;
				m_stencilFunction = func;
				m_stencilMask = mask;
				m_stencilWriteMask = writemask;
			}
			void NativeStateManager::SetStencilTwoSide(bool enabled, StencilOperation fail, StencilOperation depthFail, StencilOperation pass, CompareFunction func)
			{
				m_twoSidedStencilMode = enabled;
				m_counterClockwiseStencilFail = fail;
				m_counterClockwiseStencilDepthBufferFail = depthFail;
				m_counterClockwiseStencilPass = pass;
				m_counterClockwiseStencilFunction = func;
			}
			void NativeStateManager::SetVertexSampler(int32 samplerIndex, const ShaderSamplerState& state)
			{
				if (m_vertexSamplers)
				{
					assert(samplerIndex >= 0 && samplerIndex < 4);

					m_vertexSamplers[samplerIndex] = state;
				}
			}
			void NativeStateManager::SetPixelSampler(int32 samplerIndex, const ShaderSamplerState& state)
			{
				assert(samplerIndex >= 0 && samplerIndex < m_textureSlotCount);

				m_pixelSamplers[samplerIndex] = state;
			}

			const ShaderSamplerState& NativeStateManager::getPixelSampler(int32 samplerIndex) const { return m_pixelSamplers[samplerIndex]; }
			const ShaderSamplerState& NativeStateManager::getVertexSampler(int32 samplerIndex) const { return m_vertexSamplers[samplerIndex]; }

			/************************************************************************/
			/* Color Write                                                          */
			/************************************************************************/

			ColorWriteMasks NativeStateManager::GetColorWriteMasks(int32 rtIndex)
			{
				if (rtIndex >= 0 && rtIndex < countof(m_colorWriteMasks))
				{
					return m_colorWriteMasks[rtIndex];
				}
				return ColorWrite_All;
			}

			void NativeStateManager::SetColorWriteMasks(int32 rtIndex, ColorWriteMasks masks)
			{
				if (rtIndex >= 0 && rtIndex < countof(m_colorWriteMasks))
				{
					m_colorWriteMasks[rtIndex] = masks;
				}
			}

			void NativeStateManager::SetTexture(int32 i, NRSTexture* tex)
			{
				assert(i >= 0 && i < m_textureSlotCount);
				if (m_textureSlots[i] != tex)
				{
					m_textureSlots[i] = tex;
				}
			}

			void NativeStateManager::InitializeDefaultState()
			{
				SetAlphaTestParameters(false, CompareFunction::Always, 0);
				SetAlphaBlend(false, BlendFunction::Add, Blend::One, Blend::Zero, 0xffffffff);
				SetSeparateAlphaBlend(false, BlendFunction::Add, Blend::One, Blend::Zero);
				SetDepth(false, false, 0, 0, CompareFunction::LessEqual);

				SetPointParameters(1.0f, 256, 1, false);

				SetStencil(false, StencilOperation::Keep, StencilOperation::Keep, StencilOperation::Keep, 0, CompareFunction::Always, 0xFFFFFFFF, 0xFFFFFFFF);
				SetStencilTwoSide(false, StencilOperation::Keep, StencilOperation::Keep, StencilOperation::Keep, CompareFunction::Always);

				SetFillMode(FillMode::Solid);
				SetCullMode(CullMode::None);

				
				SetColorWriteMasks(0, ColorWrite_All);
				SetColorWriteMasks(1, ColorWrite_All);
				SetColorWriteMasks(2, ColorWrite_All);
				SetColorWriteMasks(3, ColorWrite_All);
				
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
					}
				}
				
				if (m_pixelSamplers)
					delete[] m_pixelSamplers;

				m_textureSlotCount = 16;
				m_pixelSamplers = new ShaderSamplerState[m_textureSlotCount];
				for (int32 i = 0; i < m_textureSlotCount; i++)
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
				}

				if (m_textureSlots)
					delete[] m_textureSlots;

				m_textureSlots = new NRSTexture*[m_textureSlotCount]();
			}



			NRSRenderStateManager::NRSRenderStateManager(NRSRenderDevice* device, NativeStateManager* nsmgr)
				: RenderStateManager(device), m_device(device),  m_stMgr(nsmgr)
			{
				
			}
			NRSRenderStateManager::~NRSRenderStateManager()
			{

			}

			/************************************************************************/
			/* Scissor Test                                                         */
			/************************************************************************/

			
			void NRSRenderStateManager::setScissorTest(bool enable, const Apoc3D::Math::Rectangle* r)
			{
				if (enable)
				{
					int32 rectLeft = r->getLeft();
					int32 rectRight = r->getRight();
					int32 rectTop = r->getTop();
					int32 rectBottom = r->getBottom();

					// standardize rect ( make sure top left corner is at the top left, i.e. no negative w/h)
					if (rectLeft > rectRight)
					{
						std::swap(rectLeft, rectRight);
					}
					if (rectTop > rectBottom)
					{
						std::swap(rectTop, rectBottom);
					}

					// crop it
					Viewport vp = m_device->getViewport();
					{
						if (rectRight > (vp.X + vp.Width))
						{
							rectRight = vp.X + vp.Width;
						}
						if (rectBottom > (vp.Y + vp.Height))
						{
							rectBottom = vp.Y + vp.Height;
						}
						if (rectRight < vp.X)
						{
							rectRight = vp.X;
						}
						if (rectBottom < vp.Y)
						{
							rectBottom = vp.Y;
						}

						if (rectLeft > (vp.X + vp.Width))
						{
							rectLeft = vp.X + vp.Width;
						}
						if (rectTop > (vp.Y + vp.Height))
						{
							rectTop = vp.Y + vp.Height;
						}
						if (rectLeft < vp.X)
						{
							rectLeft = vp.X;
						}
						if (rectTop < vp.Y)
						{
							rectTop = vp.Y;
						}
					}

					Apoc3D::Math::Rectangle rect = { rectLeft, rectTop, rectRight - rectLeft, rectBottom - rectTop };
					m_stMgr->setScissorTest(true, &rect);
				}
				else
				{
					m_stMgr->setScissorTest(false, nullptr);
				}
			}


		}
	}
}