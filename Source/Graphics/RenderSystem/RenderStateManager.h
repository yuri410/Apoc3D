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
#ifndef RENDERSTATEMANAGER_H
#define RENDERSTATEMANAGER_H

#include "Common.h"
#include "Graphics\GraphicsCommon.h"
#include "Math\Plane.h"

using namespace Apoc3D::Math;

namespace Apoc3D
{
	namespace Graphics
	{
		namespace RenderSystem
		{
			/** Represents a user clip plane of a RenderDevice
			*/
			class APAPI ClipPlane
			{
			public:
				virtual bool getEnabled() = 0;
				virtual Plane getPlane() = 0;

				virtual void setEnabled(bool value) = 0;
				virtual void setPlane(const Plane& plane) = 0;
				
			};

			class APAPI RenderStateManager
			{
			private:
				RenderDevice* m_renderDevice;
			protected:
				RenderStateManager(RenderDevice* device)
					: m_renderDevice(device)
				{

				}
			public:
				/************************************************************************/
				/* Alpha Test                                                           */
				/************************************************************************/
				virtual bool getAlphaTestEnable() = 0;
				virtual CompareFunction getAlphaTestFunction() = 0;
				virtual int getAlphaReference() = 0;

				virtual void SetAlphaTestParameters(bool enable, CompareFunction func, int reference) = 0;

				/************************************************************************/
				/* Alpha Blend                                                          */
				/************************************************************************/
				virtual bool getAlphaBlendEnable() = 0;
				virtual BlendFunction getBlendOperation() = 0;
				virtual Blend getSourceBlend() = 0;
				virtual Blend getDestinationBlend() = 0;
				virtual uint getBlendFactor() = 0;

				virtual void SetAlphaBlend(bool enable, BlendFunction func, Blend srcBlend, Blend dstBlend, uint factor) = 0;

				virtual bool getSeparateAlphaBlendEnable() = 0;
				virtual BlendFunction getSeparateBlendOperation() = 0;
				virtual Blend getSeparateSourceBlend() = 0;
				virtual Blend getSeparateDestinationBlend() = 0;
				virtual uint getSeparateBlendFactor() = 0;

				virtual void SetSeparateAlphaBlend(bool enable, BlendFunction func, Blend srcBlend, Blend dstBlend, uint factor) = 0;

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
				virtual void setCullMode(CullMode mode) = 0;
				virtual void setFullMode(FillMode mode) = 0;
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
				virtual int getStencilRefrence() = 0;
				virtual CompareFunction getStencilFunction() = 0;
				virtual int getStencilMask() = 0;				
				virtual int getStencilWriteMask() = 0;

				virtual void SetStencil(bool enabled, StencilOperation fail, StencilOperation depthFail, StencilOperation pass, int ref, CompareFunction func, int mask, int writemask) = 0;
				virtual void SetStencilTwoSide(bool enabled, StencilOperation fail, StencilOperation depthFail, StencilOperation pass, CompareFunction func) = 0;

				virtual bool getTwoSidedStencilMode() = 0;
				
				virtual StencilOperation getCounterClockwiseStencilFail() = 0;
				virtual StencilOperation getCounterClockwiseStencilPass() = 0;
				virtual StencilOperation getCounterClockwiseStencilDepthBufferFail() = 0;

				virtual CompareFunction getCounterClockwiseStencilFunction() = 0;
				
				virtual ClipPlane& getClipPlane(int i) = 0;
				virtual int getClipPlaneCount() = 0;

			};
		}
	}
}
#endif