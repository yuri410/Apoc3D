#pragma once

/* -----------------------------------------------------------------------
 * This source file is part of Apoc3D Framework
 *
 * Copyright (c) 2011-2019 Tao Xin
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

#ifndef GL3RENDERSTATEMANAGER_H
#define GL3RENDERSTATEMANAGER_H

#include "GL3Common.h"
#include "apoc3d/Graphics/RenderSystem/RenderStateManager.h"

using namespace Apoc3D::Graphics;
using namespace Apoc3D::Graphics::RenderSystem;

namespace Apoc3D
{
	namespace Graphics
	{
		namespace GL3RenderSystem
		{
			class NativeGL3StateManager
			{
			public:
				NativeGL3StateManager(GL3RenderDevice* device);
				~NativeGL3StateManager();

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
				const ShaderSamplerState& getSampler(int32 slotIdx) const;
				void SetSampler(int32 slotIdx, const ShaderSamplerState& sampler);

				int32 getTextureSlotCount() const { return m_textureSlotCount; }
				GL3Texture* getTexture(int32 slotIdx) const { assert(slotIdx >= 0 && slotIdx < m_textureSlotCount); return m_textureSlots[slotIdx]; }
				void SetTexture(int32 slotIdx, GL3Texture* tex);

				void Reset() { InitializeDefaultState(); }

			private:
				void InitializeDefaultState();
				void SetSampler(int32 slotIdx, ShaderSamplerState& curState, const ShaderSamplerState& state);

				GL3RenderDevice* m_device;

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

				ColorWriteMasks m_colorWriteMasks[4];

				int32 m_textureSlotCount;
				GL3Texture** m_textureSlots = nullptr;
				GLSampler* m_textureSlotSamplers = nullptr;
				ShaderSamplerState* m_cachedSamplerStates = nullptr;
			};


			class GL3RenderStateManager final : public RenderStateManager
			{
			public:
				GL3RenderStateManager(GL3RenderDevice* device, NativeGL3StateManager* nsmgr);
				~GL3RenderStateManager();

				void SetAlphaTestParameters(bool enable, CompareFunction func, uint32 reference) override
				{
					//m_stMgr->SetAlphaTestParameters(enable, func, reference);
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
				bool getAlphaTestEnable() override { return false; }
				CompareFunction getAlphaTestFunction() override { return CompareFunction::Always; }
				uint32 getAlphaReference() override { return 0; }

				/************************************************************************/
				/* Alpha Blend                                                          */
				/************************************************************************/
				bool getAlphaBlendEnable() override { return m_stMgr->getAlphaBlendEnable(); }
				BlendFunction getAlphaBlendOperation() override { return m_stMgr->getAlphaBlendOperation(); }
				Blend getAlphaSourceBlend() override { return m_stMgr->getAlphaSourceBlend(); }
				Blend getAlphaDestinationBlend() override { return m_stMgr->getAlphaDestinationBlend(); }
				uint32 getAlphaBlendFactor() override { return m_stMgr->getAlphaBlendFactor(); }

				bool getSeparateAlphaBlendEnable() override { return m_stMgr->getSeparateAlphaBlendEnable(); }
				BlendFunction getSeparateAlphaBlendOperation() override { return m_stMgr->getSeparateAlphaBlendOperation(); }
				Blend getSeparateAlphaSourceBlend() override { return m_stMgr->getSeparateAlphaSourceBlend(); }
				Blend getSeparateAlphaDestinationBlend() override { return m_stMgr->getSeparateAlphaDestinationBlend(); }
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
				StencilOperation getStencilDepthFail() override { return m_stMgr->getStencilDepthFail(); }
				uint32 getStencilRefrence() override { return m_stMgr->getStencilRefrence(); }
				CompareFunction getStencilFunction() override { return m_stMgr->getStencilFunction(); }
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

				void SetVertexSampler(int32 samplerIndex, const ShaderSamplerState& sampler) override { m_stMgr->SetSampler(samplerIndex, sampler); }
				void SetPixelSampler(int32 samplerIndex, const ShaderSamplerState& sampler) override { m_stMgr->SetSampler(samplerIndex, sampler); }

				const ShaderSamplerState& getPixelSampler(int32 samplerIndex) const override { return m_stMgr->getSampler(samplerIndex); }
				const ShaderSamplerState& getVertexSampler(int32 samplerIndex) const override { return m_stMgr->getSampler(samplerIndex); }

			private:
				GL3RenderDevice* m_device;

				NativeGL3StateManager* m_stMgr;
			};
		}
	}
}

#endif