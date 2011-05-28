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

#include "D3D9RenderStateManager.h"

#include "D3D9RenderDevice.h"

namespace Apoc3D
{
	namespace Graphics
	{
		namespace D3D9RenderSystem
		{
			D3D9ClipPlane::D3D9ClipPlane(D3D9RenderDevice* device, D3D9RenderStateManager* mgr, int index)
				: m_manager(mgr), m_device(device), m_index(index)
			{

			}

			bool D3D9ClipPlane::getEnabled()
			{
				uint32 mask = 1<<m_index;
				return !!(m_manager->clipPlaneEnable & mask);
			}
			Plane D3D9ClipPlane::getPlane()
			{
				return m_cachedPlane;
			}

			void D3D9ClipPlane::setEnabled(bool value)
			{
				if (value)
				{
					uint32 mask = 1<<m_index;
					m_manager->clipPlaneEnable |= mask;
				}
				else
				{
					uint32 mask = 1<<m_index;
					m_manager->clipPlaneEnable ^= mask;
				}
				HRESULT hr = m_device->getDevice()->SetRenderState(D3DRS_CLIPPLANEENABLE, m_manager->clipPlaneEnable);
				assert(SUCCEEDED(hr));
			}
			void D3D9ClipPlane::setPlane(const Plane& plane)
			{
				m_cachedPlane = plane;

				HRESULT hr = m_device->getDevice()->SetClipPlane(m_index, &plane.X);
				assert(SUCCEEDED(hr));
			}

			
			D3D9RenderStateManager::D3D9RenderStateManager(D3D9RenderDevice* device)
				: RenderStateManager(device), m_device(device), clipPlaneEnable(0)
			{
				for (int i=0;i<32;i++)
				{
					m_clipPlanes[i] = D3D9ClipPlane(device, this, i);
				}
				InitializeDefaultState();
			}
			D3D9RenderStateManager::~D3D9RenderStateManager()
			{

			}

			void D3D9RenderStateManager::InitializeDefaultState()
			{
				D3DDevice* dev = m_device->getDevice();

				
			}


			void D3D9RenderStateManager::SetAlphaTestParameters(bool enable, CompareFunction func, int reference)
			{

			}
			void D3D9RenderStateManager::SetAlphaBlend(bool enable, BlendFunction func, Blend srcBlend, Blend dstBlend, uint factor)
			{

			}
			void D3D9RenderStateManager::SetSeparateAlphaBlend(bool enable, BlendFunction func, Blend srcBlend, Blend dstBlend, uint factor)
			{

			}
			void D3D9RenderStateManager::SetDepth(bool enable, bool writeEnable, float bias, float slopebias, CompareFunction compare)
			{

			}
			void D3D9RenderStateManager::SetPointParameters(float size, float maxSize, float minSize, bool pointSprite)
			{

			}
			void D3D9RenderStateManager::SetStencil(bool enabled, StencilOperation fail, StencilOperation depthFail, StencilOperation pass, int ref, CompareFunction func, int mask, int writemask)
			{

			}
			void D3D9RenderStateManager::SetStencilTwoSide(bool enabled, StencilOperation fail, StencilOperation depthFail, StencilOperation pass, CompareFunction func)
			{

			}

				

		}
	}
}