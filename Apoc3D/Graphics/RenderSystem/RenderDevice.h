#pragma once
#ifndef APOC3D_RENDERDEVICE_H
#define APOC3D_RENDERDEVICE_H

/**
 * -----------------------------------------------------------------------------
 * This source file is part of Apoc3D Engine
 * 
 * Copyright (c) 2009+ Tao Xin
 * 
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 * 
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 * 
 * You should have received a copy of the GNU General Public License
 * along with this program.  if not, write to the Free Software Foundation, 
 * Inc., 59 Temple Place - Suite 330, Boston, MA 02111-1307, USA, or go to
 * http://www.gnu.org/copyleft/gpl.txt.
 * 
 * -----------------------------------------------------------------------------
 */

#include "apoc3d/Graphics/GraphicsCommon.h"
#include "apoc3d/Graphics/RenderOperation.h"
#include "apoc3d/Graphics/PixelFormat.h"
#include "apoc3d/Math/Viewport.h"

using namespace Apoc3D::Graphics;
using namespace Apoc3D::Math;

namespace Apoc3D
{
	namespace Graphics
	{
		namespace RenderSystem
		{
			/**
			 *  Applications use RenderDevice to perform DrawPrimitive level rendering and 
			 *  obtain related objects such as RenderStateManager and RenderTargets.
			 */
			class APAPI RenderDevice
			{
			public:
				virtual Capabilities* const getCapabilities() const = 0;
				
				/**
				 *  Gets the color pixel format of default render target.
				 */
				virtual PixelFormat GetDefaultRTFormat() = 0;
				/**
				 *  Gets the depth pixel format of default render target.
				 */
				virtual DepthFormat GetDefaultDepthStencilFormat() = 0;

				/**
				 *  Initialize. 
				 *  This is called when the graphics device, window,  have been created and called just before 
				 *  when the Game::Initialize method is called.
				 */
				virtual void Initialize() = 0;

				/**
				 *  Gets the device's name.
				 */
				const String& getName() const { return m_rdName; }

				/**
				 *  Gets the device's ObjectFactory object.
				 */
				ObjectFactory* getObjectFactory() { return m_objectFactory; }

				/**
				 *  Gets the device's RenderStateManager object.
				 */
				RenderStateManager* getRenderState() { return m_renderStates; }

				/**
				 *  Notify the RenderDevice a new frame is began to draw.
				 */
				virtual void BeginFrame()
				{
					m_batchCount = 0;
					m_primitiveCount = 0;
					m_vertexCount = 0;
				}
				/**
				 *  Notify the RenderDevice the current frame rendering is over.
				 */
				virtual void EndFrame() { }

				//virtual RenderTarget* getDefaultRenderTarget() = 0;

				/**
				 *  Clears all the currently binded render targets.
				 */
				virtual void Clear(ClearFlags flags, uint color, float depth, int stencil) = 0;

				/**
				 *  Sets the current render target at given index. A value of 0 will reset the rendertarget to default.
				 */
				virtual void SetRenderTarget(int index, RenderTarget* rt) = 0;

				virtual RenderTarget* GetRenderTarget(int index) = 0;

				//virtual void SetTexture(int index, Texture* texture) = 0;
				//virtual Texture* GetTexture(int index) = 0;

				virtual void BindVertexShader(VertexShader* shader) = 0;
				virtual void BindPixelShader(PixelShader* shader) = 0;

				/**
				 *  Draws a list of RenderOperations.
				 *  @param mtrl All RenderOperations are expected to have the same material as this param.
				 *  @param passSelID An index used to tell which shader effect inside materials to be used. 
				 *   A number of -1 mean the first available shader effect will be used. 
				 */
				virtual void Render(Material* mtrl, const RenderOperation* op, int count, int passSelID);

				virtual Viewport getViewport() = 0;
				virtual void setViewport(const Viewport& vp) = 0;

				/**
				 *  Gets the number of draw calls on the last frame if the frame is done, or the current draw call count if not.
				 */
				uint getBatchCount() const { return m_batchCount; }
				/**
				 *  Gets the number of primitives drawn on the last frame if the frame is done, or the current value if not.
				 */
				uint getPrimitiveCount() const { return m_primitiveCount; }
				/**
				 *  Gets the number of vertices drawn on the last frame if the frame is done, or the current value if not.
				 */
				uint getVertexCount() const { return m_vertexCount; }

			private:
				String m_rdName;


			protected:
				//Capabilities m_caps;
				uint m_batchCount;
				uint m_primitiveCount;
				uint m_vertexCount;

				ObjectFactory* m_objectFactory;
				RenderStateManager* m_renderStates;

				RenderDevice(const String &renderSysName)
					: m_rdName(renderSysName), 
					m_batchCount(0), m_primitiveCount(0), m_vertexCount(0), 
					m_objectFactory(0), m_renderStates(0)
				{

				}
			};

			/**
			 *  An interface for check if some feature are supported.
			 */
			class APAPI Capabilities
			{
			public:
				virtual bool SupportsRenderTarget(uint multisampleCount, PixelFormat pixFormat, DepthFormat depthFormat) = 0;
				virtual bool SupportsPixelShader(int majorVer, int minorVer) = 0;
				virtual bool SupportsVertexShader(int majorVer, int minorVer) = 0;

				virtual int GetMRTCount() = 0;
			};
		}
	}
}
#endif