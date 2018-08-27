#pragma once

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

#ifndef NRSRENDERSTATEMANAGER_H
#define NRSRENDERSTATEMANAGER_H

#include "NRSCommon.h"

#include "apoc3d/Graphics/RenderSystem/RenderStateManager.h"

using namespace Apoc3D::Graphics;
using namespace Apoc3D::Graphics::RenderSystem;

namespace Apoc3D
{
	namespace Graphics
	{
		namespace NullRenderSystem
		{
			class NativeStateManager
			{
			public:
				NativeStateManager(NRSRenderDevice* device);
				~NativeStateManager();

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
				bool getAlphaTestEnable() { return m_alphaTestEnable; }
				CompareFunction getAlphaTestFunction() { return m_alphaTestFunction; }
				uint32 getAlphaReference() { return m_alphaReference; }

				/************************************************************************/
				/* Alpha Blend                                                          */
				/************************************************************************/
				bool getAlphaBlendEnable() { return m_alphaBlendEnable; }
				BlendFunction getAlphaBlendOperation() { return m_alphaBlendFunction; }
				Blend getAlphaSourceBlend() { return m_alphaSourceBlend; }
				Blend getAlphaDestinationBlend() { return m_alphaDestBlend; }
				uint32 getAlphaBlendFactor() { return m_alphaBlendFactor; }

				void setAlphaBlendEnable(bool val);
				void setAlphaBlendOperation(BlendFunction func);
				void setAlphaSourceBlend(Blend val);
				void setAlphaDestinationBlend(Blend val);

				bool getSeparateAlphaBlendEnable() { return m_sepAlphaBlendEnable; }
				BlendFunction getSeparateAlphaBlendOperation() { return m_sepAlphaBlendFunction; } 
				Blend getSeparateAlphaSourceBlend() { return m_sepAlphaSourceBlend; }
				Blend getSeparateAlphaDestinationBlend() { return m_sepAlphaDestBlend; }
				//virtual uint32 getSeparateAlphaBlendFactor() { return m_cachedSepAlphaBlendFactor; }

				/************************************************************************/
				/* Depth                                                                */
				/************************************************************************/
				float getDepthBias() { return m_depthBias; }
				float getSlopeScaleDepthBias() { return m_slopeScaleDepthBias; }
				CompareFunction getDepthBufferFunction() { return m_depthBufferFunction; }
				bool getDepthBufferWriteEnabled() { return m_depthBufferWriteEnabled; }
				bool getDepthBufferEnabled() { return m_depthBufferEnabled; }

				/************************************************************************/
				/* Common                                                               */
				/************************************************************************/
				CullMode getCullMode() { return m_cullMode; }
				FillMode getFillMode() { return m_fillMode; }

				/************************************************************************/
				/* Point                                                                */
				/************************************************************************/
				float getPointSize() { return m_pointSize; }
				float getPointSizeMax() { return m_pointSizeMax; }
				float getPointSizeMin() { return m_pointSizeMin; }
				bool getPointSpriteEnabled() { return m_pointSpriteEnabled; }

				/************************************************************************/
				/* Stencil                                                              */
				/************************************************************************/
				bool getStencilEnabled() { return m_stencilEnabled; }
				StencilOperation getStencilFail() { return m_stencilFail; }
				StencilOperation getStencilPass() { return m_stencilPass; }
				StencilOperation getStencilDepthFail() { return m_stencilDepthFail; }
				uint32 getStencilRefrence() { return m_refrenceStencil; }
				CompareFunction getStencilFunction() { return m_stencilFunction; }
				uint32 getStencilMask() { return m_stencilMask; }
				uint32 getStencilWriteMask() { return m_stencilWriteMask; }

				bool getTwoSidedStencilMode() { return m_twoSidedStencilMode; }

				StencilOperation getCounterClockwiseStencilFail() { return m_counterClockwiseStencilFail; }
				StencilOperation getCounterClockwiseStencilPass() { return m_counterClockwiseStencilPass; }
				StencilOperation getCounterClockwiseStencilDepthBufferFail() { return m_counterClockwiseStencilDepthBufferFail; }

				CompareFunction getCounterClockwiseStencilFunction() { return m_counterClockwiseStencilFunction; }


				/************************************************************************/
				/* Color Write                                                          */
				/************************************************************************/
				ColorWriteMasks GetColorWriteMasks(int32 rtIndex);
				void SetColorWriteMasks(int32 rtIndex, ColorWriteMasks masks);

				/************************************************************************/
				/* Scissor Test                                                         */
				/************************************************************************/
				bool getScissorTestEnabled() const { return m_scissorTestEnabled; }
				Apoc3D::Math::Rectangle getScissorTestRect() { return m_scissorRect; }
				void setScissorTest(bool enable, const Apoc3D::Math::Rectangle* rect) { m_scissorTestEnabled = enable; if (rect) m_scissorRect = *rect; }

				/************************************************************************/
				/* Samplers                                                             */
				/************************************************************************/
				void SetVertexSampler(int32 samplerIndex, const ShaderSamplerState& sampler);
				void SetPixelSampler(int32 samplerIndex, const ShaderSamplerState& sampler);

				const ShaderSamplerState& getPixelSampler(int32 samplerIndex) const;
				const ShaderSamplerState& getVertexSampler(int32 samplerIndex) const;

				int32 getTextureSlotCount() const { return m_textureSlotCount; }
				NRSTexture* getTexture(int32 i) const { assert(i >= 0 && i < m_textureSlotCount); return m_textureSlots[i]; }
				void SetTexture(int32 i, NRSTexture* tex);

				void Reset() { InitializeDefaultState(); }
			private:
				void InitializeDefaultState();

				NRSRenderDevice* m_device;

				bool m_alphaTestEnable;
				CompareFunction m_alphaTestFunction;
				uint32 m_alphaReference;

				bool m_alphaBlendEnable;
				BlendFunction m_alphaBlendFunction;
				Blend m_alphaSourceBlend;
				Blend m_alphaDestBlend;
				uint32 m_alphaBlendFactor;

				bool m_sepAlphaBlendEnable;
				BlendFunction m_sepAlphaBlendFunction;
				Blend m_sepAlphaSourceBlend;
				Blend m_sepAlphaDestBlend;
				//uint32 m_cachedSepAlphaBlendFactor;

				float m_depthBias;
				float m_slopeScaleDepthBias;
				CompareFunction m_depthBufferFunction;
				bool m_depthBufferWriteEnabled;
				bool m_depthBufferEnabled;

				CullMode m_cullMode;
				FillMode m_fillMode;

				float m_pointSize;
				float m_pointSizeMax;
				float m_pointSizeMin;
				bool m_pointSpriteEnabled;

				bool m_stencilEnabled;
				StencilOperation m_stencilFail;
				StencilOperation m_stencilPass;
				StencilOperation m_stencilDepthFail;
				uint32 m_refrenceStencil;
				CompareFunction m_stencilFunction;
				uint32 m_stencilMask;
				uint32 m_stencilWriteMask;

				bool m_twoSidedStencilMode;

				StencilOperation m_counterClockwiseStencilFail;
				StencilOperation m_counterClockwiseStencilPass;
				StencilOperation m_counterClockwiseStencilDepthBufferFail;

				CompareFunction m_counterClockwiseStencilFunction;

				ColorWriteMasks m_colorWriteMasks[4];

				bool m_scissorTestEnabled;
				Apoc3D::Math::Rectangle m_scissorRect;

				ShaderSamplerState* m_pixelSamplers = nullptr;
				ShaderSamplerState* m_vertexSamplers = nullptr;

				int32 m_textureSlotCount;
				NRSTexture** m_textureSlots = nullptr;
			};

			class NRSRenderStateManager final : public RenderStateManager
			{
			public:
				NRSRenderStateManager(NRSRenderDevice* device, NativeStateManager* nsmgr);
				~NRSRenderStateManager();

				void SetAlphaTestParameters(bool enable, CompareFunction func, uint32 reference) override { m_stMgr->SetAlphaTestParameters(enable, func, reference); }
				void SetAlphaBlend(bool enable, BlendFunction func, Blend srcBlend, Blend dstBlend, uint32 factor) override { m_stMgr->SetAlphaBlend(enable, func, srcBlend, dstBlend, factor); }
				void SetSeparateAlphaBlend(bool enable, BlendFunction func, Blend srcBlend, Blend dstBlend) override { m_stMgr->SetSeparateAlphaBlend(enable, func, srcBlend, dstBlend); }
				void SetDepth(bool enable, bool writeEnable, float bias, float slopebias, CompareFunction compare) override { m_stMgr->SetDepth(enable, writeEnable, bias, slopebias, compare); }
				void SetPointParameters(float size, float maxSize, float minSize, bool pointSprite) override { m_stMgr->SetPointParameters(size, maxSize, minSize, pointSprite); }
				void SetStencil(bool enabled, StencilOperation fail, StencilOperation depthFail, StencilOperation pass, uint32 ref, CompareFunction func, uint32 mask, uint32 writemask) override { m_stMgr->SetStencil(enabled, fail, depthFail, pass, ref, func, mask, writemask); }
				void SetStencilTwoSide(bool enabled, StencilOperation fail, StencilOperation depthFail, StencilOperation pass, CompareFunction func) override { m_stMgr->SetStencilTwoSide(enabled, fail, depthFail, pass, func); }
				void SetCullMode(CullMode mode) override { m_stMgr->SetCullMode(mode); }
				void SetFillMode(FillMode mode) override { m_stMgr->SetFillMode(mode); }

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

				bool getScissorTestEnabled() override { return m_stMgr->getScissorTestEnabled(); }
				Apoc3D::Math::Rectangle getScissorTestRect() override { return m_stMgr->getScissorTestRect(); }
				void setScissorTest(bool enable, const Apoc3D::Math::Rectangle* rect) override;

				/************************************************************************/
				/* Color Write                                                          */
				/************************************************************************/
				ColorWriteMasks getColorWriteMasks(uint32 rtIndex) override { return m_stMgr->GetColorWriteMasks(rtIndex); }
				void setColorWriteMasks(uint32 rtIndex, ColorWriteMasks masks) override { m_stMgr->SetColorWriteMasks(rtIndex, masks); }

				/************************************************************************/
				/* Samplers                                                             */
				/************************************************************************/

				void SetVertexSampler(int32 samplerIndex, const ShaderSamplerState& sampler) override { m_stMgr->SetVertexSampler(samplerIndex, sampler); }
				void SetPixelSampler(int32 samplerIndex, const ShaderSamplerState& sampler) override { m_stMgr->SetPixelSampler(samplerIndex, sampler); }

				const ShaderSamplerState& getPixelSampler(int32 samplerIndex) const override { return m_stMgr->getPixelSampler(samplerIndex); }
				const ShaderSamplerState& getVertexSampler(int32 samplerIndex) const override { return m_stMgr->getVertexSampler(samplerIndex); }

			private:
				NRSRenderDevice* m_device;

				NativeStateManager* m_stMgr;

			};
		}
	}
}

#endif