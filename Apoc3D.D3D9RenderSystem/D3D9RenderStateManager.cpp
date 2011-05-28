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
#include "D3D9Utils.h"

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
				
				DWORD zEnabled;
				dev->GetRenderState(D3DRS_ZENABLE, &zEnabled);

				SetAlphaTestParameters(false, COMFUN_Always, 0);
				SetAlphaBlend(false, BLFUN_Add, BLEND_One, BLEND_Zero, 0xffffffff);
				SetSeparateAlphaBlend(false, BLFUN_Add, BLEND_One, BLEND_Zero);
				SetDepth(zEnabled == TRUE, zEnabled == TRUE, 0, 0, COMFUN_LessEqual);

				float psize;
				dev->GetRenderState(D3DRS_POINTSIZE, reinterpret_cast<DWORD*>(&psize));
				SetPointParameters(psize, 1, 64, false);

				SetStencil(false, STOP_Keep, STOP_Keep, STOP_Keep, 0, COMFUN_Always, 0xFFFFFFFF, 0xFFFFFFFF);
				SetStencilTwoSide(false, STOP_Keep, STOP_Keep, STOP_Keep, COMFUN_Always);
			}

			void D3D9RenderStateManager::SetCullMode(CullMode mode)
			{
				m_cachedCullMode = mode;

				D3DCULL cull = D3D9Utils::ConvertCullMode(mode);
				m_device->getDevice()->SetRenderState(D3DRS_CULLMODE, cull);
			}
			void D3D9RenderStateManager::SetFillMode(FillMode mode)
			{
				m_cachedFillMode = mode;

				D3DFILLMODE fill = D3D9Utils::ConvertFillMode(mode);
				m_device->getDevice()->SetRenderState(D3DRS_FILLMODE, fill);
			}
			void D3D9RenderStateManager::SetAlphaTestParameters(bool enable, CompareFunction func, uint32 reference)
			{
				m_cachedAlphaTestEnable = enable;
				m_cachedAlphaReference = reference;
				m_cachedAlphaTestFunction = func;

				D3DDevice* dev = m_device->getDevice();
				dev->SetRenderState(D3DRS_ALPHATESTENABLE, enable ? TRUE : FALSE);
				dev->SetRenderState(D3DRS_ALPHAREF, reference);
				dev->SetRenderState(D3DRS_ALPHAFUNC, D3D9Utils::ConvertCompare(func));

			}
			void D3D9RenderStateManager::SetAlphaBlend(bool enable, BlendFunction func, Blend srcBlend, Blend dstBlend, uint32 factor)
			{
				m_cachedAlphaBlendEnable = enable;
				m_cachedAlphaBlendFunction = func;
				m_cachedAlphaSourceBlend = srcBlend;
				m_cachedAlphaDestBlend = dstBlend;
				m_cachedAlphaBlendFactor = factor;

				D3DDevice* dev = m_device->getDevice();
				dev->SetRenderState(D3DRS_ALPHABLENDENABLE, enable ? TRUE : FALSE);
				dev->SetRenderState(D3DRS_BLENDOP, D3D9Utils::ConvertBlendFunction(func));
				dev->SetRenderState(D3DRS_SRCBLEND, D3D9Utils::ConvertBlend(srcBlend));
				dev->SetRenderState(D3DRS_SRCBLEND, D3D9Utils::ConvertBlend(dstBlend));
				dev->SetRenderState(D3DRS_BLENDFACTOR, factor);
			}
			void D3D9RenderStateManager::SetSeparateAlphaBlend(bool enable, BlendFunction func, Blend srcBlend, Blend dstBlend)
			{
				m_cachedSepAlphaBlendEnable = enable;
				m_cachedSepAlphaBlendFunction = func;
				m_cachedSepAlphaSourceBlend = srcBlend;
				m_cachedSepAlphaDestBlend = dstBlend;

				D3DDevice* dev = m_device->getDevice();
				dev->SetRenderState(D3DRS_SEPARATEALPHABLENDENABLE, enable ? TRUE : FALSE);
				dev->SetRenderState(D3DRS_BLENDOPALPHA, D3D9Utils::ConvertBlendFunction(func));
				dev->SetRenderState(D3DRS_SRCBLENDALPHA, D3D9Utils::ConvertBlend(srcBlend));
				dev->SetRenderState(D3DRS_SRCBLENDALPHA, D3D9Utils::ConvertBlend(dstBlend));
				//dev->SetRenderState(D3DRS_BLENDFACTOR, factor);
				
			}
			void D3D9RenderStateManager::SetDepth(bool enable, bool writeEnable, float bias, float slopebias, CompareFunction compare)
			{
				m_cachedDepthBufferEnabled = enable;
				m_cachedDepthBufferWriteEnabled = writeEnable;
				m_cachedDepthBias = bias;
				m_cachedSlopeScaleDepthBias = slopebias;
				m_cachedDepthBufferFunction = compare;

				D3DDevice* dev = m_device->getDevice();
				dev->SetRenderState(D3DRS_ZENABLE, enable ? TRUE : FALSE);
				dev->SetRenderState(D3DRS_ZWRITEENABLE, writeEnable ? TRUE : FALSE);
				dev->SetRenderState(D3DRS_DEPTHBIAS, *(DWORD*)&bias);
				dev->SetRenderState(D3DRS_SLOPESCALEDEPTHBIAS, *(DWORD*)&slopebias);
				dev->SetRenderState(D3DRS_ZFUNC, D3D9Utils::ConvertCompare(compare));
			}
			void D3D9RenderStateManager::SetPointParameters(float size, float maxSize, float minSize, bool pointSprite)
			{
				m_cachedPointSize = size;
				m_cachedPointSizeMax = maxSize;
				m_cachedPointSizeMin = minSize;
				m_cachedPointSpriteEnabled = pointSprite;

				D3DDevice* dev = m_device->getDevice();
				dev->SetRenderState(D3DRS_POINTSIZE, *((DWORD*)&size));
				dev->SetRenderState(D3DRS_POINTSIZE_MAX, *((DWORD*)&maxSize));
				dev->SetRenderState(D3DRS_POINTSIZE_MIN, *((DWORD*)&minSize));
				dev->SetRenderState(D3DRS_POINTSPRITEENABLE, pointSprite ? TRUE : FALSE);

			}
			void D3D9RenderStateManager::SetStencil(bool enabled, StencilOperation fail, StencilOperation depthFail, StencilOperation pass, 
				uint32 ref, CompareFunction func, uint32 mask, uint32 writemask)
			{
				m_cachedStencilEnabled = enabled;
				m_cachedStencilFail = fail;
				m_cachedStencilDepthFail = depthFail;
				m_cachedStencilPass = pass;
				m_cachedRefrenceStencil = ref;
				m_cachedStencilFunction = func;
				m_cachedStencilMask = mask;
				m_cachedStencilWriteMask = writemask;

				D3DDevice* dev = m_device->getDevice();
				dev->SetRenderState(D3DRS_STENCILENABLE, enabled ? TRUE : FALSE);
				dev->SetRenderState(D3DRS_STENCILFAIL, D3D9Utils::ConvertStencilOperation(fail));
				dev->SetRenderState(D3DRS_STENCILZFAIL, D3D9Utils::ConvertStencilOperation(depthFail));
				dev->SetRenderState(D3DRS_STENCILPASS, D3D9Utils::ConvertStencilOperation(pass));
				dev->SetRenderState(D3DRS_STENCILREF, ref);
				dev->SetRenderState(D3DRS_STENCILFUNC, D3D9Utils::ConvertCompare(func));
				dev->SetRenderState(D3DRS_STENCILMASK, mask);
				dev->SetRenderState(D3DRS_STENCILWRITEMASK, writemask);

			}
			void D3D9RenderStateManager::SetStencilTwoSide(bool enabled, StencilOperation fail, StencilOperation depthFail, StencilOperation pass, CompareFunction func)
			{
				m_cachedTwoSidedStencilMode = enabled;
				m_cachedCounterClockwiseStencilFail = fail;
				m_cachedCounterClockwiseStencilDepthBufferFail = depthFail;
				m_cachedCounterClockwiseStencilPass = pass;
				m_cachedCounterClockwiseStencilFunction = func;

				D3DDevice* dev = m_device->getDevice();
				dev->SetRenderState(D3DRS_TWOSIDEDSTENCILMODE, enabled ? TRUE : FALSE);
				dev->SetRenderState(D3DRS_CCW_STENCILFAIL, D3D9Utils::ConvertStencilOperation(fail));
				dev->SetRenderState(D3DRS_CCW_STENCILZFAIL, D3D9Utils::ConvertStencilOperation(depthFail));
				dev->SetRenderState(D3DRS_CCW_STENCILPASS, D3D9Utils::ConvertStencilOperation(pass));
				dev->SetRenderState(D3DRS_CCW_STENCILFUNC, D3D9Utils::ConvertCompare(func));
			}

				

		}
	}
}