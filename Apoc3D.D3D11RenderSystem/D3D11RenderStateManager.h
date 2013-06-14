#pragma once

#include "D3D11Common.h"
#include "RenderSystem.h"

namespace rex
{
	class NativeD3DStateManager
	{
	public:
		NativeD3DStateManager(D3D11RenderDevice* device);
		~NativeD3DStateManager();

		void SetAlphaBlend(bool enable, BlendFunction func, Blend srcBlend, Blend dstBlend);
		void SetAlphaBlend(bool enable, BlendFunction func, Blend srcBlend, Blend dstBlend, uint32 factor);
		void SetSeparateAlphaBlend(bool enable, BlendFunction func, Blend srcBlend, Blend dstBlend);
		void SetDepth(bool enable, bool writeEnable);
		void SetDepth(bool enable, bool writeEnable, float bias, float slopebias, CompareFunction compare);
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
		void getColorWriteEnabled0(bool& r, bool& g, bool& b, bool& a);
		void setColorWriteEnabled0(bool r, bool g, bool b, bool a);
		void getColorWriteEnabled1(bool& r, bool& g, bool& b, bool& a);
		void setColorWriteEnabled1(bool r, bool g, bool b, bool a);
		void getColorWriteEnabled2(bool& r, bool& g, bool& b, bool& a);
		void setColorWriteEnabled2(bool r, bool g, bool b, bool a);
		void getColorWriteEnabled3(bool& r, bool& g, bool& b, bool& a);
		void setColorWriteEnabled3(bool r, bool g, bool b, bool a);

		/************************************************************************/
		/* Scissor Test                                                         */
		/************************************************************************/

		bool getScissorTestEnabled();
		rex::Rectangle getScissorTestRect();
		void setScissorTest(bool enable, const rex::Rectangle* rect);

		/************************************************************************/
		/* Samplers                                                             */
		/************************************************************************/
		void SetVertexSampler(int samplerIndex, const ShaderSamplerState& sampler);
		void SetPixelSampler(int samplerIndex, const ShaderSamplerState& sampler);

		const ShaderSamplerState& getPixelSampler(int samplerIndex) const { return m_vertexSamplers[samplerIndex]; }
		const ShaderSamplerState& getVertexSampler(int samplerIndex) const { return m_pixelSamplers[samplerIndex]; }

		void Flush();
	private:
		void InitializeDefaultState();

		D3D11RenderDevice* m_device;

		bool m_needsFlushOverall;
		
		bool m_flushBlendState;

		bool m_cachedAlphaBlendEnable;
		BlendFunction m_cachedAlphaBlendFunction;
		Blend m_cachedAlphaSourceBlend;
		Blend m_cachedAlphaDestBlend;
		uint32 m_cachedAlphaBlendFactor;
		
		bool m_cachedSepAlphaBlendEnable;
		BlendFunction m_cachedSepAlphaBlendFunction;
		Blend m_cachedSepAlphaSourceBlend;
		Blend m_cachedSepAlphaDestBlend;

		bool m_colorWrite0[4];
		bool m_colorWrite1[4];
		bool m_colorWrite2[4];
		bool m_colorWrite3[4];


		bool m_flushDepthStencil;

		CompareFunction m_cachedDepthBufferFunction;
		bool m_cachedDepthBufferWriteEnabled;
		bool m_cachedDepthBufferEnabled;

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


		bool m_flushRasterizer;

		CullMode m_cachedCullMode;
		FillMode m_cachedFillMode;
		float m_cachedDepthBias;
		float m_cachedSlopeScaleDepthBias;

		bool m_scissorEnable;
		rex::Rectangle m_scissorRect;

		
		ShaderSamplerState* m_pixelSamplers;
		ShaderSamplerState* m_vertexSamplers;
	};

	class D3D11RenderStateManager : public RenderStateManager
	{
	public:
		D3D11RenderStateManager(D3D11RenderDevice* device, NativeD3DStateManager* nsmgr);
		~D3D11RenderStateManager();

		virtual void SetAlphaBlend(bool enable, BlendFunction func, Blend srcBlend, Blend dstBlend, uint32 factor)
		{
			m_stMgr->SetAlphaBlend(enable, func, srcBlend, dstBlend, factor);
		}
		virtual void SetSeparateAlphaBlend(bool enable, BlendFunction func, Blend srcBlend, Blend dstBlend)
		{
			m_stMgr->SetSeparateAlphaBlend(enable, func, srcBlend, dstBlend);
		}
		virtual void SetDepth(bool enable, bool writeEnable, float bias, float slopebias, CompareFunction compare)
		{
			m_stMgr->SetDepth(enable, writeEnable, bias, slopebias, compare);
		}
		virtual void SetStencil(bool enabled, StencilOperation fail, StencilOperation depthFail, StencilOperation pass, uint32 ref, CompareFunction func, uint32 mask, uint32 writemask)
		{
			m_stMgr->SetStencil(enabled, fail, depthFail, pass, ref, func, mask, writemask);
		}
		virtual void SetStencilTwoSide(bool enabled, StencilOperation fail, StencilOperation depthFail, StencilOperation pass, CompareFunction func)
		{
			m_stMgr->SetStencilTwoSide(enabled, fail, depthFail, pass, func);
		}
		virtual void SetCullMode(CullMode mode)
		{
			m_stMgr->SetCullMode(mode);
		}
		virtual void SetFillMode(FillMode mode)
		{
			m_stMgr->SetFillMode(mode);
		}

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
		virtual float getDepthBias() { return m_stMgr->getDepthBias(); }
		virtual float getSlopeScaleDepthBias() { return m_stMgr->getSlopeScaleDepthBias(); }
		virtual CompareFunction getDepthBufferFunction() { return m_stMgr->getDepthBufferFunction(); }
		virtual bool getDepthBufferWriteEnabled() { return m_stMgr->getDepthBufferWriteEnabled(); }
		virtual bool getDepthBufferEnabled() { return m_stMgr->getDepthBufferEnabled(); }

		/************************************************************************/
		/* Common                                                               */
		/************************************************************************/
		virtual CullMode getCullMode() { return m_stMgr->getCullMode(); }
		virtual FillMode getFillMode() { return m_stMgr->getFillMode(); }

		/************************************************************************/
		/* Stencil                                                              */
		/************************************************************************/
		virtual bool getStencilEnabled() { return m_stMgr->getStencilEnabled(); }
		virtual StencilOperation getStencilFail() { return m_stMgr->getStencilFail(); }
		virtual StencilOperation getStencilPass() { return m_stMgr->getStencilPass(); }
		virtual StencilOperation getStencilDepthFail() { return m_stMgr->getStencilDepthFail(); }
		virtual uint32 getStencilRefrence() { return m_stMgr->getStencilRefrence(); }
		virtual CompareFunction getStencilFunction() { return m_stMgr->getStencilFunction(); }
		virtual uint32 getStencilMask() { return m_stMgr->getStencilMask(); }
		virtual uint32 getStencilWriteMask() { return m_stMgr->getStencilWriteMask(); }

		virtual bool getTwoSidedStencilMode() { return m_stMgr->getTwoSidedStencilMode(); }

		virtual StencilOperation getCounterClockwiseStencilFail() { return m_stMgr->getCounterClockwiseStencilFail(); }
		virtual StencilOperation getCounterClockwiseStencilPass() { return m_stMgr->getCounterClockwiseStencilPass(); }
		virtual StencilOperation getCounterClockwiseStencilDepthBufferFail() { return m_stMgr->getCounterClockwiseStencilDepthBufferFail(); }

		virtual CompareFunction getCounterClockwiseStencilFunction() { return m_stMgr->getCounterClockwiseStencilFunction(); }

		/************************************************************************/
		/* Scissor Test                                                         */
		/************************************************************************/

		virtual bool getScissorTestEnabled();
		virtual rex::Rectangle getScissorTestRect();
		virtual void setScissorTest(bool enable, const rex::Rectangle* rect);

		/************************************************************************/
		/* Color Write                                                          */
		/************************************************************************/
		virtual void getColorWriteEnabled(int rtIndex, bool& r, bool& g, bool& b, bool& a);
		virtual void setColorWriteEnabled(int rtIndex, bool r, bool g, bool b, bool a);

	private:
		D3D11RenderDevice* m_device;

		NativeD3DStateManager* m_stMgr;
	};
}