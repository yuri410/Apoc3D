#pragma once
#ifndef APOC3D_RENDERSTATEMANAGER_H
#define APOC3D_RENDERSTATEMANAGER_H

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

#include "apoc3d/Collections/HashMap.h"
#include "apoc3d/Graphics/GraphicsCommon.h"
#include "apoc3d/Math/Plane.h"
#include "apoc3d/Math/Rectangle.h"

using namespace Apoc3D::Collections;
using namespace Apoc3D::Math;

namespace Apoc3D
{
	namespace Graphics
	{
		namespace RenderSystem
		{
			class APAPI RenderStateManager
			{
			public:
				/************************************************************************/
				/* Alpha Test                                                           */
				/************************************************************************/
				virtual bool getAlphaTestEnable() = 0;
				virtual CompareFunction getAlphaTestFunction() = 0;
				virtual uint32 getAlphaReference() = 0;

				virtual void SetAlphaTestParameters(bool enable, CompareFunction func, uint32 reference) = 0;

				/************************************************************************/
				/* Alpha Blend                                                          */
				/************************************************************************/
				virtual bool getAlphaBlendEnable() = 0;
				virtual BlendFunction getAlphaBlendOperation() = 0;
				virtual Blend getAlphaSourceBlend() = 0;
				virtual Blend getAlphaDestinationBlend() = 0;
				virtual uint32 getAlphaBlendFactor() = 0;

				virtual void SetAlphaBlend(bool enable, BlendFunction func, Blend srcBlend, Blend dstBlend, uint32 factor) = 0;

				virtual bool getSeparateAlphaBlendEnable() = 0;
				virtual BlendFunction getSeparateAlphaBlendOperation() = 0;
				virtual Blend getSeparateAlphaSourceBlend() = 0;
				virtual Blend getSeparateAlphaDestinationBlend() = 0;
				//virtual uint32 getSeparateAlphaBlendFactor() = 0;

				virtual void SetSeparateAlphaBlend(bool enable, BlendFunction func, Blend srcBlend, Blend dstBlend) = 0;

				/************************************************************************/
				/* Depth                                                                */
				/************************************************************************/
				virtual float getDepthBias() = 0;
				virtual float getSlopeScaleDepthBias() = 0;
				virtual CompareFunction getDepthBufferFunction() = 0;
				virtual bool getDepthBufferWriteEnabled() = 0;
				virtual bool getDepthBufferEnabled() = 0;

				virtual void SetDepth(bool enable, bool writeEnable, float bias, float slopebias, CompareFunction compare) = 0;

				/************************************************************************/
				/* Common                                                               */
				/************************************************************************/
				virtual CullMode getCullMode() = 0;
				virtual FillMode getFillMode() = 0;
				virtual void SetCullMode(CullMode mode) = 0;
				virtual void SetFillMode(FillMode mode) = 0;
				/************************************************************************/
				/* Point                                                                */
				/************************************************************************/
				virtual float getPointSize() = 0;
				virtual float getPointSizeMax() = 0;
				virtual float getPointSizeMin() = 0;
				virtual bool getPointSpriteEnabled() = 0;

				virtual void SetPointParameters(float size, float maxSize, float minSize, bool pointSprite) = 0;
				/************************************************************************/
				/* Stencil                                                              */
				/************************************************************************/
				virtual bool getStencilEnabled() = 0;
				virtual StencilOperation getStencilFail() = 0;
				virtual StencilOperation getStencilPass() = 0;
				virtual StencilOperation getStencilDepthFail() = 0;
				virtual uint32 getStencilRefrence() = 0;
				virtual CompareFunction getStencilFunction() = 0;
				virtual uint32 getStencilMask() = 0;				
				virtual uint32 getStencilWriteMask() = 0;

				virtual void SetStencil(bool enabled, StencilOperation fail, StencilOperation depthFail, StencilOperation pass, uint32 ref, CompareFunction func, uint32 mask, uint32 writemask) = 0;
				virtual void SetStencilTwoSide(bool enabled, StencilOperation fail, StencilOperation depthFail, StencilOperation pass, CompareFunction func) = 0;

				virtual bool getTwoSidedStencilMode() = 0;
				
				virtual StencilOperation getCounterClockwiseStencilFail() = 0;
				virtual StencilOperation getCounterClockwiseStencilPass() = 0;
				virtual StencilOperation getCounterClockwiseStencilDepthBufferFail() = 0;

				virtual CompareFunction getCounterClockwiseStencilFunction() = 0;
				
				/************************************************************************/
				/* Scissor Test                                                         */
				/************************************************************************/
				virtual bool getScissorTestEnabled() = 0;
				virtual Apoc3D::Math::Rectangle getScissorTestRect() = 0;
				virtual void setScissorTest(bool enable, const Apoc3D::Math::Rectangle* rect) = 0;
				
				/************************************************************************/
				/* ColorWrite                                                           */
				/************************************************************************/
				virtual ColorWriteMasks getColorWriteMasks(uint32 rtIndex ) = 0;
				virtual void setColorWriteMasks(uint32 rtIndex, ColorWriteMasks masks ) = 0;
				
				/************************************************************************/
				/* Samplers                                                             */
				/************************************************************************/
				virtual void SetVertexSampler(int32 samplerIndex, const ShaderSamplerState& sampler) = 0;
				virtual void SetPixelSampler(int32 samplerIndex, const ShaderSamplerState& sampler) = 0;

				virtual const ShaderSamplerState& getPixelSampler(int32 samplerIndex) const = 0;
				virtual const ShaderSamplerState& getVertexSampler(int32 samplerIndex) const = 0;

			protected:
				RenderStateManager(RenderDevice* device);
			private:
				RenderDevice* m_renderDevice;
			};

			class APAPI ScopeRenderTargetChange
			{
			public:
				ScopeRenderTargetChange(RenderDevice* device, int32 idx, RenderTarget* rt);
				ScopeRenderTargetChange(RenderDevice* device, std::initializer_list<std::pair<int32, RenderTarget* >> list);

				ScopeRenderTargetChange(RenderDevice* device, int32 idx, RenderTarget* rt, DepthStencilBuffer* dsb);
				ScopeRenderTargetChange(RenderDevice* device, std::initializer_list<std::pair<int32, RenderTarget* >> list, DepthStencilBuffer* dsb);
				~ScopeRenderTargetChange();

				template <typename Func>
				void Run(Func f) { f(); }
			private:
				void ChangeRenderTarget(int32 idx, RenderTarget* rt);
				void ChangeDepthStencilBuffer(DepthStencilBuffer* dsb);

				RenderDevice* m_device;

				RenderTarget* m_oldRenderTargets[4];
				bool m_oldRenderTargetChanged[4];
				
				DepthStencilBuffer* m_oldDSB = nullptr;
				bool m_oldDSBChanged = false;

				// no allocation unless involved
				HashMap<int32, RenderTarget*> m_additionalOldRenderTarget;
			};
		}
	}
}
#endif