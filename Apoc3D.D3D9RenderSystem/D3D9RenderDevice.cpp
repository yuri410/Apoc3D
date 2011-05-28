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

#include "D3D9RenderDevice.h"
#include "GraphicsDeviceManager.h"
#include "D3D9Utils.h"

#include "D3D9PixelShader.h"
#include "D3D9VertexShader.h"
#include "D3D9RenderTarget.h"
#include "D3D9Texture.h"

#include "D3D9ObjectFactory.h"

namespace Apoc3D
{
	namespace Graphics
	{
		namespace D3D9RenderSystem
		{
			D3D9RenderDevice::D3D9RenderDevice(GraphicsDeviceManager* devManager)
				: RenderDevice(L"Direct3D9 RenderSystem"), m_devManager(devManager)
			{

			}

			D3DDevice* D3D9RenderDevice::getDevice() const { return m_devManager->getDevice(); } 

			void D3D9RenderDevice::Initialize()
			{
				
			}
			
			void D3D9RenderDevice::BeginFrame()
			{
				RenderDevice::BeginFrame();
			}

			void D3D9RenderDevice::Clear(ClearFlags flags, uint color, float depth, int stencil)
			{
				assert(getDevice());
				getDevice()->Clear(0, NULL, D3D9Utils::ConvertClearFlags(flags), color, depth, stencil);
			}

			void D3D9RenderDevice::SetRenderTarget(int index, RenderTarget* rt){}

			RenderTarget* D3D9RenderDevice::GetRenderTarget(int index){}

			void D3D9RenderDevice::SetTexture(int index, Texture* texture)
			{
			}
			Texture* D3D9RenderDevice::GetTexture(int index)
			{
				return m_cachedTextures[index];
			}

			void D3D9RenderDevice::BindVertexShader(VertexShader* shader)
			{
				if (shader)
				{
					VertexShader* vs = static_cast<VertexShader*>(shader);
					getDevice()->SetVertexShader(vs->getD3DVS());
				}
				else
				{
					getDevice()->SetVertexShader(0);
				}
			}
			void D3D9RenderDevice::BindPixelShader(PixelShader* shader)
			{
				if (shader)
				{

					getDevice()->SetVertexShader();
				}
				else
				{
					getDevice()->SetPixelShader(0);
				}
			}

			void D3D9RenderDevice::Render(const RenderOperation* op, int count)
			{

			}

			Viewport D3D9RenderDevice::getViewport()
			{
				D3DVIEWPORT9 dvp;
				getDevice()->GetViewport(&dvp);

				Viewport vp(dvp.X, dvp.Y, dvp.Width, dvp.Height, dvp.MinZ, dvp.MaxZ);
				return vp;
			}
			void D3D9RenderDevice::setViewport(const Viewport& vp)
			{
				D3DVIEWPORT9 dvp;
				dvp.X = vp.X;
				dvp.Y = vp.Y;
				dvp.Width = vp.Width;
				dvp.Height = vp.Height;
				dvp.MinZ = vp.MinZ;
				dvp.MaxZ = vp.MaxZ;

				getDevice()->SetViewport(&dvp);
			}
		}
	}
}