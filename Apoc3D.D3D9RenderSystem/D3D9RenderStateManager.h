/*
-----------------------------------------------------------------------------
This source file is part of Apoc3D Engine

Copyright (c) 2009+ Tao Games

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
#ifndef D3D9RENDERSTATEMANAGER_H
#define D3D9RENDERSTATEMANAGER_H

#include "D3D9Common.h"
#include "Graphics/RenderSystem/RenderStateManager.h"

using namespace Apoc3D::Graphics;
using namespace Apoc3D::Graphics::RenderSystem;

namespace Apoc3D
{
	namespace Graphics
	{
		namespace D3D9RenderSystem
		{
			class D3D9ClipPlane : public ClipPlane
			{
			private:
				D3D9RenderStateManager* m_manager;
				D3D9RenderDevice* m_device;
				int32 m_index;
				Plane m_cachedPlane;

			public:
				D3D9ClipPlane() {}
				D3D9ClipPlane(D3D9RenderDevice* device, D3D9RenderStateManager* mgr, int index);
				virtual bool getEnabled();
				virtual Plane getPlane();

				virtual void setEnabled(bool value);
				virtual void setPlane(const Plane& plane);
			};

			class D3D9RenderStateManager : public RenderStateManager
			{
			private:
				D3D9RenderDevice* m_device;
				D3D9ClipPlane m_clipPlanes[32];

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


				void InitializeDefaultState();

			public:
				DWORD clipPlaneEnable;

				D3D9RenderStateManager(D3D9RenderDevice* device);
				~D3D9RenderStateManager();

				virtual void SetAlphaTestParameters(bool enable, CompareFunction func, uint32 reference);
				virtual void SetAlphaBlend(bool enable, BlendFunction func, Blend srcBlend, Blend dstBlend, uint32 factor);
				virtual void SetSeparateAlphaBlend(bool enable, BlendFunction func, Blend srcBlend, Blend dstBlend);
				virtual void SetDepth(bool enable, bool writeEnable, float bias, float slopebias, CompareFunction compare);
				virtual void SetPointParameters(float size, float maxSize, float minSize, bool pointSprite);
				virtual void SetStencil(bool enabled, StencilOperation fail, StencilOperation depthFail, StencilOperation pass, uint32 ref, CompareFunction func, uint32 mask, uint32 writemask);
				virtual void SetStencilTwoSide(bool enabled, StencilOperation fail, StencilOperation depthFail, StencilOperation pass, CompareFunction func);
				virtual void SetCullMode(CullMode mode);
				virtual void SetFillMode(FillMode mode);

				/************************************************************************/
				/* Alpha Test                                                           */
				/************************************************************************/
				virtual bool getAlphaTestEnable() { return m_cachedAlphaTestEnable; }
				virtual CompareFunction getAlphaTestFunction() { return m_cachedAlphaTestFunction; }
				virtual uint32 getAlphaReference() { return m_cachedAlphaReference; }

				/************************************************************************/
				/* Alpha Blend                                                          */
				/************************************************************************/
				virtual bool getAlphaBlendEnable() { return m_cachedAlphaBlendEnable; }
				virtual BlendFunction getAlphaBlendOperation() { return m_cachedAlphaBlendFunction; }
				virtual Blend getAlphaSourceBlend() { return m_cachedAlphaSourceBlend; }
				virtual Blend getAlphaDestinationBlend() { return m_cachedAlphaDestBlend; }
				virtual uint32 getAlphaBlendFactor() { return m_cachedAlphaBlendFactor; }

				virtual bool getSeparateAlphaBlendEnable() { return m_cachedSepAlphaBlendEnable; }
				virtual BlendFunction getSeparateAlphaBlendOperation() { return m_cachedSepAlphaBlendFunction; } 
				virtual Blend getSeparateAlphaSourceBlend() { return m_cachedSepAlphaSourceBlend; }
				virtual Blend getSeparateAlphaDestinationBlend() { return m_cachedSepAlphaDestBlend; }
				//virtual uint32 getSeparateAlphaBlendFactor() { return m_cachedSepAlphaBlendFactor; }

				/************************************************************************/
				/* Depth                                                                */
				/************************************************************************/
				virtual float getDepthBias() { return m_cachedDepthBias; }
				virtual float getSlopeScaleDepthBias() { return m_cachedSlopeScaleDepthBias; }
				virtual CompareFunction getDepthBufferFunction() { return m_cachedDepthBufferFunction; }
				virtual bool getDepthBufferWriteEnabled() { return m_cachedDepthBufferWriteEnabled; }
				virtual bool getDepthBufferEnabled() { return m_cachedDepthBufferEnabled; }

				/************************************************************************/
				/* Common                                                               */
				/************************************************************************/
				virtual CullMode getCullMode() { return m_cachedCullMode; }
				virtual FillMode getFillMode() { return m_cachedFillMode; }

				/************************************************************************/
				/* Point                                                                */
				/************************************************************************/
				virtual float getPointSize() { return m_cachedPointSize; }
				virtual float getPointSizeMax() { return m_cachedPointSizeMax; }
				virtual float getPointSizeMin() { return m_cachedPointSizeMin; }
				virtual bool getPointSpriteEnabled() { return m_cachedPointSpriteEnabled; }

				/************************************************************************/
				/* Stencil                                                              */
				/************************************************************************/
				virtual bool getStencilEnabled() { return m_cachedStencilEnabled; }
				virtual StencilOperation getStencilFail() { return m_cachedStencilFail; }
				virtual StencilOperation getStencilPass() { return m_cachedStencilPass; }
				virtual StencilOperation getStencilDepthFail() { return m_cachedStencilDepthFail; }
				virtual uint32 getStencilRefrence() { return m_cachedRefrenceStencil; }
				virtual CompareFunction getStencilFunction() { return m_cachedStencilFunction; }
				virtual uint32 getStencilMask() { return m_cachedStencilMask; }
				virtual uint32 getStencilWriteMask() { return m_cachedStencilWriteMask; }

				virtual bool getTwoSidedStencilMode() { return m_cachedTwoSidedStencilMode; }
				
				virtual StencilOperation getCounterClockwiseStencilFail() { return m_cachedCounterClockwiseStencilFail; }
				virtual StencilOperation getCounterClockwiseStencilPass() { return m_cachedCounterClockwiseStencilPass; }
				virtual StencilOperation getCounterClockwiseStencilDepthBufferFail() { return m_cachedCounterClockwiseStencilDepthBufferFail; }

				virtual CompareFunction getCounterClockwiseStencilFunction() { return m_cachedCounterClockwiseStencilFunction; }
				
				virtual ClipPlane& getClipPlane(int i) { return m_clipPlanes[i]; }
				virtual int getClipPlaneCount() { return 32; }
			};
		}
	}
}

#endif