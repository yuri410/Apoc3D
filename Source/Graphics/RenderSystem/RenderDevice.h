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
#ifndef RENDERDEVICE_H
#define RENDERDEVICE_H

#include "Common.h"
//#include "RenderDeviceCaps.h"

#include "Graphics\GraphicsCommon.h"
#include "Math\Viewport.h"
#include "Graphics\RenderOperation.h"
#include "Graphics\PixelFormat.h"

using namespace Apoc3D::Graphics;
using namespace Apoc3D::Math;

namespace Apoc3D
{
	namespace Graphics
	{
		namespace RenderSystem
		{
			/* Applications use RenderDevice to perform DrawPrimitive level rendering
			*/
			class APAPI RenderDevice
			{
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
			public:
				uint getBatchCount() const { return m_batchCount; }
				uint getPrimitiveCount() const { return m_primitiveCount; }
				uint getVertexCount() const { return m_vertexCount; }

				virtual Capabilities* const getCapabilities() const = 0;
				const String& getRenderDeviceName() const { return m_rdName; }

				virtual PixelFormat GetDefaultRTFormat() = 0;
				virtual DepthFormat GetDefaultDepthStencilFormat() = 0;

				virtual void Initialize() = 0;

				const String& getName() const { return m_rdName; }

				ObjectFactory* getObjectFactory() { return m_objectFactory; }
				RenderStateManager* getRenderState() { return m_renderStates; }

				virtual void BeginFrame()
				{
					m_batchCount = 0;
					m_primitiveCount = 0;
					m_vertexCount = 0;
				}
				virtual void EndFrame() { }

				//virtual RenderTarget* getDefaultRenderTarget() = 0;

				virtual void Clear(ClearFlags flags, uint color, float depth, int stencil) = 0;

				virtual void SetRenderTarget(int index, RenderTarget* rt) = 0;

				virtual RenderTarget* GetRenderTarget(int index) = 0;

				//virtual void SetTexture(int index, Texture* texture) = 0;
				//virtual Texture* GetTexture(int index) = 0;

				virtual void BindVertexShader(VertexShader* shader) = 0;
				virtual void BindPixelShader(PixelShader* shader) = 0;

				virtual void Render(Material* mtrl, const RenderOperation* op, int count, int passSelID);

				virtual Viewport getViewport() = 0;
				virtual void setViewport(const Viewport& vp) = 0;
			};

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