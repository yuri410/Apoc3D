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
#ifndef D3D9RENDERDEVICE_H
#define D3D9RENDERDEVICE_H

#include "D3D9Common.h"

#include "Graphics/RenderSystem/RenderDevice.h"

using namespace Apoc3D::Graphics;
using namespace Apoc3D::Graphics::RenderSystem;

namespace Apoc3D
{
	namespace Graphics
	{
		namespace D3D9RenderSystem
		{
			class D3D9RenderDevice : public RenderDevice
			{
			private:
				GraphicsDeviceManager* m_devManager;

				D3D9Texture* m_cachedTextures[16];




			public:
				inline D3DDevice* getDevice() const;

				D3D9RenderDevice(GraphicsDeviceManager* devManager);

				
				virtual void Initialize();

				
				virtual void BeginFrame();

				virtual void Clear(ClearFlags flags, uint color, float depth, int stencil);

				virtual void SetRenderTarget(int index, RenderTarget* rt);

				virtual RenderTarget* GetRenderTarget(int index);

				virtual void SetTexture(int index, Texture* texture);
				virtual Texture* GetTexture(int index);

				virtual void BindVertexShader(VertexShader* shader);
				virtual void BindPixelShader(PixelShader* shader);

				virtual void Render(const RenderOperation* op, int count);

				virtual Viewport getViewport();
				virtual void setViewport(const Viewport& vp);
			};
		}
	}
}

#endif