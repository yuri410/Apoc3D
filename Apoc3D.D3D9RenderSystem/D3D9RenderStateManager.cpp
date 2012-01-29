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

			NativeD3DStateManager::NativeD3DStateManager(D3D9RenderDevice* device)
				: m_device(device)
			{
				InitializeDefaultState();
			}
			NativeD3DStateManager::~NativeD3DStateManager()
			{

			}

			void NativeD3DStateManager::SetCullMode(CullMode mode)
			{
				m_cachedCullMode = mode;

				D3DCULL cull = D3D9Utils::ConvertCullMode(mode);
				HRESULT hr = m_device->getDevice()->SetRenderState(D3DRS_CULLMODE, cull);
				assert(SUCCEEDED(hr));
			}
			void NativeD3DStateManager::SetFillMode(FillMode mode)
			{
				m_cachedFillMode = mode;

				D3DFILLMODE fill = D3D9Utils::ConvertFillMode(mode);
				HRESULT hr = m_device->getDevice()->SetRenderState(D3DRS_FILLMODE, fill);
				assert(SUCCEEDED(hr));
			}
			void NativeD3DStateManager::SetAlphaTestParameters(bool enable, uint32 reference)
			{
				m_cachedAlphaTestEnable = enable;
				m_cachedAlphaReference = reference;

				D3DDevice* dev = m_device->getDevice();
				HRESULT hr = dev->SetRenderState(D3DRS_ALPHATESTENABLE, enable ? TRUE : FALSE);
				assert(SUCCEEDED(hr));
				hr = dev->SetRenderState(D3DRS_ALPHAREF, reference);
				assert(SUCCEEDED(hr));
			}
			void NativeD3DStateManager::SetAlphaTestParameters(bool enable, CompareFunction func, uint32 reference)
			{
				m_cachedAlphaTestEnable = enable;
				m_cachedAlphaReference = reference;
				m_cachedAlphaTestFunction = func;

				D3DDevice* dev = m_device->getDevice();
				HRESULT hr = dev->SetRenderState(D3DRS_ALPHATESTENABLE, enable ? TRUE : FALSE);
				assert(SUCCEEDED(hr));
				hr = dev->SetRenderState(D3DRS_ALPHAREF, reference);
				assert(SUCCEEDED(hr));
				hr = dev->SetRenderState(D3DRS_ALPHAFUNC, D3D9Utils::ConvertCompare(func));
				assert(SUCCEEDED(hr));

			}
			void NativeD3DStateManager::SetAlphaBlend(bool enable, BlendFunction func, Blend srcBlend, Blend dstBlend)
			{
				m_cachedAlphaBlendEnable = enable;
				m_cachedAlphaBlendFunction = func;
				m_cachedAlphaSourceBlend = srcBlend;
				m_cachedAlphaDestBlend = dstBlend;

				D3DDevice* dev = m_device->getDevice();
				HRESULT hr = dev->SetRenderState(D3DRS_ALPHABLENDENABLE, enable ? TRUE : FALSE);
				assert(SUCCEEDED(hr));
				hr = dev->SetRenderState(D3DRS_BLENDOP, D3D9Utils::ConvertBlendFunction(func));
				assert(SUCCEEDED(hr));
				hr = dev->SetRenderState(D3DRS_SRCBLEND, D3D9Utils::ConvertBlend(srcBlend));
				assert(SUCCEEDED(hr));
				hr = dev->SetRenderState(D3DRS_DESTBLEND, D3D9Utils::ConvertBlend(dstBlend));
				assert(SUCCEEDED(hr));
			}
			void NativeD3DStateManager::SetAlphaBlend(bool enable, BlendFunction func, Blend srcBlend, Blend dstBlend, uint32 factor)
			{
				m_cachedAlphaBlendEnable = enable;
				m_cachedAlphaBlendFunction = func;
				m_cachedAlphaSourceBlend = srcBlend;
				m_cachedAlphaDestBlend = dstBlend;
				m_cachedAlphaBlendFactor = factor;

				D3DDevice* dev = m_device->getDevice();
				HRESULT hr = dev->SetRenderState(D3DRS_ALPHABLENDENABLE, enable ? TRUE : FALSE);
				assert(SUCCEEDED(hr));
				hr = dev->SetRenderState(D3DRS_BLENDOP, D3D9Utils::ConvertBlendFunction(func));
				assert(SUCCEEDED(hr));
				hr = dev->SetRenderState(D3DRS_SRCBLEND, D3D9Utils::ConvertBlend(srcBlend));
				assert(SUCCEEDED(hr));
				hr = dev->SetRenderState(D3DRS_DESTBLEND, D3D9Utils::ConvertBlend(dstBlend));
				assert(SUCCEEDED(hr));
				hr = dev->SetRenderState(D3DRS_BLENDFACTOR, factor);
				assert(SUCCEEDED(hr));
			}

			void NativeD3DStateManager::setAlphaBlendEnable(bool enable)
			{
				m_cachedAlphaBlendEnable = enable;
				HRESULT hr = m_device->getDevice()->SetRenderState(D3DRS_ALPHABLENDENABLE, enable ? TRUE : FALSE);
				assert(SUCCEEDED(hr));
			}
			void NativeD3DStateManager::setAlphaBlendOperation(BlendFunction func)
			{
				m_cachedAlphaBlendFunction = func;
				HRESULT hr = m_device->getDevice()->SetRenderState(D3DRS_BLENDOP, D3D9Utils::ConvertBlendFunction(func));
				assert(SUCCEEDED(hr));
			}
			void NativeD3DStateManager::setAlphaSourceBlend(Blend srcBlend)
			{
				m_cachedAlphaSourceBlend = srcBlend;
				HRESULT hr = m_device->getDevice()->SetRenderState(D3DRS_SRCBLEND, D3D9Utils::ConvertBlend(srcBlend));
				assert(SUCCEEDED(hr));
			}
			void NativeD3DStateManager::setAlphaDestinationBlend(Blend dstBlend)
			{
				m_cachedAlphaDestBlend = dstBlend;
				HRESULT hr = m_device->getDevice()->SetRenderState(D3DRS_DESTBLEND, D3D9Utils::ConvertBlend(dstBlend));
				assert(SUCCEEDED(hr));
			}

			void NativeD3DStateManager::SetSeparateAlphaBlend(bool enable, BlendFunction func, Blend srcBlend, Blend dstBlend)
			{
				m_cachedSepAlphaBlendEnable = enable;
				m_cachedSepAlphaBlendFunction = func;
				m_cachedSepAlphaSourceBlend = srcBlend;
				m_cachedSepAlphaDestBlend = dstBlend;

				D3DDevice* dev = m_device->getDevice();
				HRESULT hr = dev->SetRenderState(D3DRS_SEPARATEALPHABLENDENABLE, enable ? TRUE : FALSE);
				assert(SUCCEEDED(hr));
				hr = dev->SetRenderState(D3DRS_BLENDOPALPHA, D3D9Utils::ConvertBlendFunction(func));
				assert(SUCCEEDED(hr));
				hr = dev->SetRenderState(D3DRS_SRCBLENDALPHA, D3D9Utils::ConvertBlend(srcBlend));
				assert(SUCCEEDED(hr));
				hr = dev->SetRenderState(D3DRS_DESTBLENDALPHA, D3D9Utils::ConvertBlend(dstBlend));
				assert(SUCCEEDED(hr));
				//dev->SetRenderState(D3DRS_BLENDFACTOR, factor);

			}

			void NativeD3DStateManager::SetDepth(bool enable, bool writeEnable)
			{
				m_cachedDepthBufferEnabled = enable;
				m_cachedDepthBufferWriteEnabled = writeEnable;

				D3DDevice* dev = m_device->getDevice();
				HRESULT hr = dev->SetRenderState(D3DRS_ZENABLE, enable ? TRUE : FALSE);
				assert(SUCCEEDED(hr));
				hr = dev->SetRenderState(D3DRS_ZWRITEENABLE, writeEnable ? TRUE : FALSE);
				assert(SUCCEEDED(hr));
			}
			void NativeD3DStateManager::SetDepth(bool enable, bool writeEnable, float bias, float slopebias, CompareFunction compare)
			{
				m_cachedDepthBufferEnabled = enable;
				m_cachedDepthBufferWriteEnabled = writeEnable;
				m_cachedDepthBias = bias;
				m_cachedSlopeScaleDepthBias = slopebias;
				m_cachedDepthBufferFunction = compare;

				D3DDevice* dev = m_device->getDevice();
				HRESULT hr = dev->SetRenderState(D3DRS_ZENABLE, enable ? TRUE : FALSE);
				assert(SUCCEEDED(hr));
				hr = dev->SetRenderState(D3DRS_ZWRITEENABLE, writeEnable ? TRUE : FALSE);
				assert(SUCCEEDED(hr));
				hr = dev->SetRenderState(D3DRS_DEPTHBIAS, *(DWORD*)&bias);
				assert(SUCCEEDED(hr));
				hr = dev->SetRenderState(D3DRS_SLOPESCALEDEPTHBIAS, *(DWORD*)&slopebias);
				assert(SUCCEEDED(hr));
				hr = dev->SetRenderState(D3DRS_ZFUNC, D3D9Utils::ConvertCompare(compare));
				assert(SUCCEEDED(hr));
			}
			void NativeD3DStateManager::SetPointParameters(float size, float maxSize, float minSize, bool pointSprite)
			{
				m_cachedPointSize = size;
				m_cachedPointSizeMax = maxSize;
				m_cachedPointSizeMin = minSize;
				m_cachedPointSpriteEnabled = pointSprite;

				D3DDevice* dev = m_device->getDevice();
				HRESULT hr = dev->SetRenderState(D3DRS_POINTSIZE, *((DWORD*)&size));
				assert(SUCCEEDED(hr));
				hr = dev->SetRenderState(D3DRS_POINTSIZE_MAX, *((DWORD*)&maxSize));
				assert(SUCCEEDED(hr));
				hr = dev->SetRenderState(D3DRS_POINTSIZE_MIN, *((DWORD*)&minSize));
				assert(SUCCEEDED(hr));
				hr = dev->SetRenderState(D3DRS_POINTSPRITEENABLE, pointSprite ? TRUE : FALSE);
				assert(SUCCEEDED(hr));
			}
			void NativeD3DStateManager::SetStencil(bool enabled, StencilOperation fail, StencilOperation depthFail, StencilOperation pass, 
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
				HRESULT hr = dev->SetRenderState(D3DRS_STENCILENABLE, enabled ? TRUE : FALSE);
				assert(SUCCEEDED(hr));
				hr = dev->SetRenderState(D3DRS_STENCILFAIL, D3D9Utils::ConvertStencilOperation(fail));
				assert(SUCCEEDED(hr));
				hr = dev->SetRenderState(D3DRS_STENCILZFAIL, D3D9Utils::ConvertStencilOperation(depthFail));
				assert(SUCCEEDED(hr));
				hr = dev->SetRenderState(D3DRS_STENCILPASS, D3D9Utils::ConvertStencilOperation(pass));
				assert(SUCCEEDED(hr));
				hr = dev->SetRenderState(D3DRS_STENCILREF, ref);
				assert(SUCCEEDED(hr));
				hr = dev->SetRenderState(D3DRS_STENCILFUNC, D3D9Utils::ConvertCompare(func));
				assert(SUCCEEDED(hr));
				hr = dev->SetRenderState(D3DRS_STENCILMASK, mask);
				assert(SUCCEEDED(hr));
				hr = dev->SetRenderState(D3DRS_STENCILWRITEMASK, writemask);
				assert(SUCCEEDED(hr));

			}
			void NativeD3DStateManager::SetStencilTwoSide(bool enabled, StencilOperation fail, StencilOperation depthFail, StencilOperation pass, CompareFunction func)
			{
				m_cachedTwoSidedStencilMode = enabled;
				m_cachedCounterClockwiseStencilFail = fail;
				m_cachedCounterClockwiseStencilDepthBufferFail = depthFail;
				m_cachedCounterClockwiseStencilPass = pass;
				m_cachedCounterClockwiseStencilFunction = func;

				D3DDevice* dev = m_device->getDevice();
				HRESULT hr = dev->SetRenderState(D3DRS_TWOSIDEDSTENCILMODE, enabled ? TRUE : FALSE);
				assert(SUCCEEDED(hr));
				hr = dev->SetRenderState(D3DRS_CCW_STENCILFAIL, D3D9Utils::ConvertStencilOperation(fail));
				assert(SUCCEEDED(hr));
				hr = dev->SetRenderState(D3DRS_CCW_STENCILZFAIL, D3D9Utils::ConvertStencilOperation(depthFail));
				assert(SUCCEEDED(hr));
				hr = dev->SetRenderState(D3DRS_CCW_STENCILPASS, D3D9Utils::ConvertStencilOperation(pass));
				assert(SUCCEEDED(hr));
				hr = dev->SetRenderState(D3DRS_CCW_STENCILFUNC, D3D9Utils::ConvertCompare(func));
				assert(SUCCEEDED(hr));

			}


			void NativeD3DStateManager::InitializeDefaultState()
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

			
			D3D9RenderStateManager::D3D9RenderStateManager(D3D9RenderDevice* device, NativeD3DStateManager* nsmgr)
				: RenderStateManager(device), m_device(device), clipPlaneEnable(0), m_stMgr(nsmgr)
			{
				for (int i=0;i<32;i++)
				{
					m_clipPlanes[i] = D3D9ClipPlane(device, this, i);
				}
			}
			D3D9RenderStateManager::~D3D9RenderStateManager()
			{

			}

			bool D3D9RenderStateManager::getScissorTestEnabled()
			{
				DWORD result;
				HRESULT hr = m_device->getDevice()->GetRenderState(D3DRS_SCISSORTESTENABLE, &result);
				assert(SUCCEEDED(hr));
				return !!result;
			}
			Apoc3D::Math::Rectangle D3D9RenderStateManager::getScissorTestRect()
			{
				RECT rect;
				HRESULT hr = m_device->getDevice()->GetScissorRect(&rect);
				return Apoc3D::Math::Rectangle((int)rect.left, (int)rect.top, (int)(rect.right-rect.left), (int)(rect.bottom-rect.top));
			}
			void D3D9RenderStateManager::setScissorTest(bool enable, const Apoc3D::Math::Rectangle* r)
			{
				if (enable)
				{
					RECT rect = { 
						(LONG)r->X, 
						(LONG)r->Y,
						(LONG)r->getRight(),
						(LONG)r->getBottom()
					};

					D3DVIEWPORT9 vp;
					if (SUCCEEDED(m_device->getDevice()->GetViewport(&vp)))
					{
						if ((DWORD)rect.bottom > (vp.Y + vp.Height))
						{
							rect.bottom = vp.Y + vp.Height;
						}
						if ((DWORD)rect.right > (vp.X + vp.Width))
						{
							rect.right = vp.X + vp.Width;
						}

					}

					// In some cases, the X,Y of the rect is not always the top-left corner,
					// when the Width or Height is negative. Standardize it.
					if (rect.left>rect.right)
					{
						swap(rect.left, rect.right);
					}
					if (rect.top > rect.bottom)
					{
						swap(rect.top, rect.bottom);
					}

					if (rect.left<0)
						rect.left = 0;
					if (rect.top<0)
						rect.top = 0;
					
					HRESULT hr = m_device->getDevice()->SetScissorRect(&rect);
					assert(SUCCEEDED(hr));
					hr = m_device->getDevice()->SetRenderState(D3DRS_SCISSORTESTENABLE, TRUE);
					assert(SUCCEEDED(hr));
				}
				else
				{
					HRESULT hr = m_device->getDevice()->SetRenderState(D3DRS_SCISSORTESTENABLE, FALSE);
					assert(SUCCEEDED(hr));
				}
			}

		}
	}
}