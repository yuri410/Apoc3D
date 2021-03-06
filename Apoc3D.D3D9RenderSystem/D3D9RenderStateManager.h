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

#ifndef D3D9RENDERSTATEMANAGER_H
#define D3D9RENDERSTATEMANAGER_H

#include "D3D9Common.h"

#include "apoc3d/Graphics/RenderSystem/RenderStateManager.h"

using namespace Apoc3D::Graphics;
using namespace Apoc3D::Graphics::RenderSystem;

namespace Apoc3D
{
	namespace Graphics
	{
		namespace D3D9RenderSystem
		{
			/** This class stores states for quick state checks without any
			 *  D3D calls. Other classes check state to avoid redundant state changes.
			 */
			class NativeD3DStateManager
			{
			public:
				NativeD3DStateManager(D3D9RenderDevice* device);
				~NativeD3DStateManager();

				void SetAlphaTestParameters(bool enable, uint32 reference);
				void SetAlphaTestParameters(bool enable, CompareFunction func, uint32 reference);
				void SetAlphaBlend(bool enable, BlendFunction func, Blend srcBlend, Blend dstBlend);
				void SetAlphaBlend(bool enable, BlendFunction func, Blend srcBlend, Blend dstBlend, uint32 factor);
				void SetSeparateAlphaBlend(bool enable, BlendFunction func, Blend srcBlend, Blend dstBlend);
				void SetDepth(bool enable, bool writeEnable);
				void SetDepth(bool enable, bool writeEnable, float bias, float slopebias, CompareFunction compare);
				void SetPointParameters(float size, float maxSize, float minSize, bool pointSprite);
				void SetStencil(bool enabled, StencilOperation fail, StencilOperation depthFail, StencilOperation pass, uint32 ref, CompareFunction func, uint32 mask, uint32 writemask);
				void SetStencilTwoSide(bool enabled, StencilOperation fail, StencilOperation depthFail, StencilOperation pass, CompareFunction func);
				void SetCullMode(CullMode mode);
				void SetFillMode(FillMode mode);

				/************************************************************************/
				/* Alpha Test                                                           */
				/************************************************************************/
				bool getAlphaTestEnable() { return m_cachedAlphaTestEnable; }
				CompareFunction getAlphaTestFunction() { return m_cachedAlphaTestFunction; }
				uint32 getAlphaReference() { return m_cachedAlphaReference; }

				/************************************************************************/
				/* Alpha Blend                                                          */
				/************************************************************************/
				bool getAlphaBlendEnable() { return m_cachedAlphaBlendEnable; }
				BlendFunction getAlphaBlendOperation() { return m_cachedAlphaBlendFunction; }
				Blend getAlphaSourceBlend() { return m_cachedAlphaSourceBlend; }
				Blend getAlphaDestinationBlend() { return m_cachedAlphaDestBlend; }
				uint32 getAlphaBlendFactor() { return m_cachedAlphaBlendFactor; }

				void setAlphaBlendEnable(bool val);
				void setAlphaBlendOperation(BlendFunction func);
				void setAlphaSourceBlend(Blend val);
				void setAlphaDestinationBlend(Blend val);

				bool getSeparateAlphaBlendEnable() { return m_cachedSepAlphaBlendEnable; }
				BlendFunction getSeparateAlphaBlendOperation() { return m_cachedSepAlphaBlendFunction; } 
				Blend getSeparateAlphaSourceBlend() { return m_cachedSepAlphaSourceBlend; }
				Blend getSeparateAlphaDestinationBlend() { return m_cachedSepAlphaDestBlend; }
				//virtual uint32 getSeparateAlphaBlendFactor() { return m_cachedSepAlphaBlendFactor; }

				/************************************************************************/
				/* Depth                                                                */
				/************************************************************************/
				float getDepthBias() { return m_cachedDepthBias; }
				float getSlopeScaleDepthBias() { return m_cachedSlopeScaleDepthBias; }
				CompareFunction getDepthBufferFunction() { return m_cachedDepthBufferFunction; }
				bool getDepthBufferWriteEnabled() { return m_cachedDepthBufferWriteEnabled; }
				bool getDepthBufferEnabled() { return m_cachedDepthBufferEnabled; }

				/************************************************************************/
				/* Common                                                               */
				/************************************************************************/
				CullMode getCullMode() { return m_cachedCullMode; }
				FillMode getFillMode() { return m_cachedFillMode; }

				/************************************************************************/
				/* Point                                                                */
				/************************************************************************/
				float getPointSize() { return m_cachedPointSize; }
				float getPointSizeMax() { return m_cachedPointSizeMax; }
				float getPointSizeMin() { return m_cachedPointSizeMin; }
				bool getPointSpriteEnabled() { return m_cachedPointSpriteEnabled; }

				/************************************************************************/
				/* Stencil                                                              */
				/************************************************************************/
				bool getStencilEnabled() { return m_cachedStencilEnabled; }
				StencilOperation getStencilFail() { return m_cachedStencilFail; }
				StencilOperation getStencilPass() { return m_cachedStencilPass; }
				StencilOperation getStencilDepthFail() { return m_cachedStencilDepthFail; }
				uint32 getStencilRefrence() { return m_cachedRefrenceStencil; }
				CompareFunction getStencilFunction() { return m_cachedStencilFunction; }
				uint32 getStencilMask() { return m_cachedStencilMask; }
				uint32 getStencilWriteMask() { return m_cachedStencilWriteMask; }

				bool getTwoSidedStencilMode() { return m_cachedTwoSidedStencilMode; }

				StencilOperation getCounterClockwiseStencilFail() { return m_cachedCounterClockwiseStencilFail; }
				StencilOperation getCounterClockwiseStencilPass() { return m_cachedCounterClockwiseStencilPass; }
				StencilOperation getCounterClockwiseStencilDepthBufferFail() { return m_cachedCounterClockwiseStencilDepthBufferFail; }

				CompareFunction getCounterClockwiseStencilFunction() { return m_cachedCounterClockwiseStencilFunction; }


				/************************************************************************/
				/* Color Write                                                          */
				/************************************************************************/
				ColorWriteMasks GetColorWriteMasks(int32 rtIndex);
				void SetColorWriteMasks(int32 rtIndex, ColorWriteMasks masks);

				/************************************************************************/
				/* Samplers                                                             */
				/************************************************************************/
				void SetVertexSampler(int32 samplerIndex, const ShaderSamplerState& sampler);
				void SetPixelSampler(int32 samplerIndex, const ShaderSamplerState& sampler);

				const ShaderSamplerState& getPixelSampler(int32 samplerIndex) const;
				const ShaderSamplerState& getVertexSampler(int32 samplerIndex) const;

				int32 getTextureSlotCount() const { return m_textureSlotCount; }
				D3D9Texture* getTexture(int32 i) const { assert(i >= 0 && i < m_textureSlotCount); return m_textureSlots[i]; }
				void SetTexture(int32 i, D3D9Texture* tex);

				void Reset() { InitializeDefaultState(); }
			private:
				void InitializeDefaultState();
				void SetSampler(DWORD samplerIndex, ShaderSamplerState& curState, const ShaderSamplerState& state);

				D3D9RenderDevice* m_device;

				bool m_cachedAlphaTestEnable;
				CompareFunction m_cachedAlphaTestFunction;
				uint32 m_cachedAlphaReference;

				bool m_cachedAlphaBlendEnable;
				BlendFunction m_cachedAlphaBlendFunction;
				Blend m_cachedAlphaSourceBlend;
				Blend m_cachedAlphaDestBlend;
				uint32 m_cachedAlphaBlendFactor;

				bool m_cachedSepAlphaBlendEnable;
				BlendFunction m_cachedSepAlphaBlendFunction;
				Blend m_cachedSepAlphaSourceBlend;
				Blend m_cachedSepAlphaDestBlend;
				//uint32 m_cachedSepAlphaBlendFactor;

				float m_cachedDepthBias;
				float m_cachedSlopeScaleDepthBias;
				CompareFunction m_cachedDepthBufferFunction;
				bool m_cachedDepthBufferWriteEnabled;
				bool m_cachedDepthBufferEnabled;

				CullMode m_cachedCullMode;
				FillMode m_cachedFillMode;

				float m_cachedPointSize;
				float m_cachedPointSizeMax;
				float m_cachedPointSizeMin;
				bool m_cachedPointSpriteEnabled;

				bool m_cachedStencilEnabled;
				StencilOperation m_cachedStencilFail;
				StencilOperation m_cachedStencilPass;
				StencilOperation m_cachedStencilDepthFail;
				uint32 m_cachedRefrenceStencil;
				CompareFunction m_cachedStencilFunction;
				uint32 m_cachedStencilMask;
				uint32 m_cachedStencilWriteMask;

				bool m_cachedTwoSidedStencilMode;

				StencilOperation m_cachedCounterClockwiseStencilFail;
				StencilOperation m_cachedCounterClockwiseStencilPass;
				StencilOperation m_cachedCounterClockwiseStencilDepthBufferFail;

				CompareFunction m_cachedCounterClockwiseStencilFunction;

				ShaderSamplerState* m_pixelSamplers = nullptr;
				ShaderSamplerState* m_vertexSamplers = nullptr;

				ColorWriteMasks m_colorWriteMasks[4];

				int32 m_textureSlotCount;
				D3D9Texture** m_textureSlots = nullptr;
			};

			class D3D9RenderStateManager final : public RenderStateManager
			{
			public:
				D3D9RenderStateManager(D3D9RenderDevice* device, NativeD3DStateManager* nsmgr);
				~D3D9RenderStateManager();

				void SetAlphaTestParameters(bool enable, CompareFunction func, uint32 reference) override
				{
					m_stMgr->SetAlphaTestParameters(enable, func, reference);
				}
				void SetAlphaBlend(bool enable, BlendFunction func, Blend srcBlend, Blend dstBlend, uint32 factor) override
				{
					m_stMgr->SetAlphaBlend(enable, func, srcBlend, dstBlend, factor);
				}
				void SetSeparateAlphaBlend(bool enable, BlendFunction func, Blend srcBlend, Blend dstBlend) override
				{
					m_stMgr->SetSeparateAlphaBlend(enable, func, srcBlend, dstBlend);
				}
				void SetDepth(bool enable, bool writeEnable, float bias, float slopebias, CompareFunction compare) override
				{
					m_stMgr->SetDepth(enable, writeEnable, bias, slopebias, compare);
				}
				void SetPointParameters(float size, float maxSize, float minSize, bool pointSprite) override
				{
					m_stMgr->SetPointParameters(size, maxSize, minSize, pointSprite);
				}
				void SetStencil(bool enabled, StencilOperation fail, StencilOperation depthFail, StencilOperation pass, uint32 ref, CompareFunction func, uint32 mask, uint32 writemask) override
				{
					m_stMgr->SetStencil(enabled, fail, depthFail, pass, ref, func, mask, writemask);
				}
				void SetStencilTwoSide(bool enabled, StencilOperation fail, StencilOperation depthFail, StencilOperation pass, CompareFunction func) override
				{
					m_stMgr->SetStencilTwoSide(enabled, fail, depthFail, pass, func);
				}
				void SetCullMode(CullMode mode) override
				{
					m_stMgr->SetCullMode(mode);
				}
				void SetFillMode(FillMode mode) override
				{
					m_stMgr->SetFillMode(mode);
				}

				/************************************************************************/
				/* Alpha Test                                                           */
				/************************************************************************/
				bool getAlphaTestEnable() override { return m_stMgr->getAlphaTestEnable(); }
				CompareFunction getAlphaTestFunction() override { return m_stMgr->getAlphaTestFunction(); }
				uint32 getAlphaReference() override { return m_stMgr->getAlphaReference(); }

				/************************************************************************/
				/* Alpha Blend                                                          */
				/************************************************************************/
				virtual bool getAlphaBlendEnable() { return m_stMgr->getAlphaBlendEnable(); }
				virtual BlendFunction getAlphaBlendOperation() { return m_stMgr->getAlphaBlendOperation(); }
				virtual Blend getAlphaSourceBlend() { return m_stMgr->getAlphaSourceBlend(); }
				virtual Blend getAlphaDestinationBlend() { return m_stMgr->getAlphaDestinationBlend(); }
				virtual uint32 getAlphaBlendFactor() { return m_stMgr->getAlphaBlendFactor(); }

				virtual bool getSeparateAlphaBlendEnable() { return m_stMgr->getSeparateAlphaBlendEnable(); }
				virtual BlendFunction getSeparateAlphaBlendOperation() { return m_stMgr->getSeparateAlphaBlendOperation(); } 
				virtual Blend getSeparateAlphaSourceBlend() { return m_stMgr->getSeparateAlphaSourceBlend(); }
				virtual Blend getSeparateAlphaDestinationBlend() { return m_stMgr->getSeparateAlphaDestinationBlend(); }
				//virtual uint32 getSeparateAlphaBlendFactor() { return m_cachedSepAlphaBlendFactor; }

				/************************************************************************/
				/* Depth                                                                */
				/************************************************************************/
				float getDepthBias() override { return m_stMgr->getDepthBias(); }
				float getSlopeScaleDepthBias() override { return m_stMgr->getSlopeScaleDepthBias(); }
				CompareFunction getDepthBufferFunction() override { return m_stMgr->getDepthBufferFunction(); }
				bool getDepthBufferWriteEnabled() override { return m_stMgr->getDepthBufferWriteEnabled(); }
				bool getDepthBufferEnabled() override { return m_stMgr->getDepthBufferEnabled(); }

				/************************************************************************/
				/* Common                                                               */
				/************************************************************************/
				CullMode getCullMode() override { return m_stMgr->getCullMode(); }
				FillMode getFillMode() override { return m_stMgr->getFillMode(); }

				/************************************************************************/
				/* Point                                                                */
				/************************************************************************/
				float getPointSize() override { return m_stMgr->getPointSize(); }
				float getPointSizeMax() override { return m_stMgr->getPointSizeMax(); }
				float getPointSizeMin() override { return m_stMgr->getPointSizeMin(); }
				bool getPointSpriteEnabled() override { return m_stMgr->getPointSpriteEnabled(); }

				/************************************************************************/
				/* Stencil                                                              */
				/************************************************************************/
				bool getStencilEnabled() override { return m_stMgr->getStencilEnabled(); }
				StencilOperation getStencilFail() override { return m_stMgr->getStencilFail(); }
				StencilOperation getStencilPass() override { return m_stMgr->getStencilPass(); }
				StencilOperation getStencilDepthFail()override { return m_stMgr->getStencilDepthFail(); }
				uint32 getStencilRefrence() override { return m_stMgr->getStencilRefrence(); }
				CompareFunction getStencilFunction()override { return m_stMgr->getStencilFunction(); }
				uint32 getStencilMask() override { return m_stMgr->getStencilMask(); }
				uint32 getStencilWriteMask() override { return m_stMgr->getStencilWriteMask(); }

				bool getTwoSidedStencilMode() override { return m_stMgr->getTwoSidedStencilMode(); }
				
				StencilOperation getCounterClockwiseStencilFail() override { return m_stMgr->getCounterClockwiseStencilFail(); }
				StencilOperation getCounterClockwiseStencilPass() override { return m_stMgr->getCounterClockwiseStencilPass(); }
				StencilOperation getCounterClockwiseStencilDepthBufferFail() override { return m_stMgr->getCounterClockwiseStencilDepthBufferFail(); }

				CompareFunction getCounterClockwiseStencilFunction() override { return m_stMgr->getCounterClockwiseStencilFunction(); }
				

				/************************************************************************/
				/* Scissor Test                                                         */
				/************************************************************************/

				bool getScissorTestEnabled() override;
				Apoc3D::Math::Rectangle getScissorTestRect() override;
				void setScissorTest(bool enable, const Apoc3D::Math::Rectangle* rect) override;

				/************************************************************************/
				/* Color Write                                                          */
				/************************************************************************/
				ColorWriteMasks getColorWriteMasks(uint32 rtIndex) override;
				void setColorWriteMasks(uint32 rtIndex, ColorWriteMasks masks) override;

				/************************************************************************/
				/* Samplers                                                             */
				/************************************************************************/

				void SetVertexSampler(int32 samplerIndex, const ShaderSamplerState& sampler) override { m_stMgr->SetVertexSampler(samplerIndex, sampler); }
				void SetPixelSampler(int32 samplerIndex, const ShaderSamplerState& sampler) override { m_stMgr->SetPixelSampler(samplerIndex, sampler); }

				const ShaderSamplerState& getPixelSampler(int32 samplerIndex) const override { return m_stMgr->getPixelSampler(samplerIndex); }
				const ShaderSamplerState& getVertexSampler(int32 samplerIndex) const override { return m_stMgr->getVertexSampler(samplerIndex); }

			private:
				D3D9RenderDevice* m_device;

				NativeD3DStateManager* m_stMgr;

			};
		}
	}
}

#endif