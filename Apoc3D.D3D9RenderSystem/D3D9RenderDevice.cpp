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
#include "D3D9RenderStateManager.h"
#include "D3D9ObjectFactory.h"
#include "Buffer/D3D9DepthBuffer.h"
#include "Buffer/D3D9IndexBuffer.h"
#include "Buffer/D3D9VertexBuffer.h"
#include "D3D9VertexDeclaration.h"
#include "VolatileResource.h"
#include "Apoc3DException.h"

#include "Graphics/Material.h"
#include "Graphics/GeometryData.h"
#include "Graphics/EffectSystem/Effect.h"

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
			D3D9RenderDevice::~D3D9RenderDevice()
			{
				m_defaultRT->Release();
				m_defaultRT = 0;

				m_defaultDS->Release();
				m_defaultDS = 0;

				delete[] m_cachedRenderTarget;
				delete m_stateManager;
				delete m_objectFactory;
			}

			D3DDevice* D3D9RenderDevice::getDevice() const { return m_devManager->getDevice(); } 

			
			void D3D9RenderDevice::OnDeviceLost()
			{
				for (size_t i=0;i<m_volatileResources.size();i++)
				{
					m_volatileResources[i]->ReleaseVolatileResource();
				}
			}
			void D3D9RenderDevice::OnDeviceReset()
			{
				for (size_t i=0;i<m_volatileResources.size();i++)
				{
					m_volatileResources[i]->ReloadVolatileResource();
				}
			}

			void D3D9RenderDevice::Initialize()
			{
				m_nativeState = new NativeD3DStateManager(this);
				m_renderStates = new D3D9RenderStateManager(this, m_nativeState);
				m_objectFactory = new D3D9ObjectFactory(this);

				D3DDevice* dev = getDevice();

				D3DCAPS9 caps;
				dev->GetDeviceCaps(&caps);

				m_cachedRenderTarget = new D3D9RenderTarget*[caps.NumSimultaneousRTs];
			}
			
			void D3D9RenderDevice::BeginFrame()
			{
				RenderDevice::BeginFrame();
			}
			void D3D9RenderDevice::EndFrame()
			{

			}

			void D3D9RenderDevice::Clear(ClearFlags flags, uint color, float depth, int stencil)
			{
				assert(getDevice());
				getDevice()->Clear(0, NULL, D3D9Utils::ConvertClearFlags(flags), color, depth, stencil);
			}

			void D3D9RenderDevice::SetRenderTarget(int index, RenderTarget* rt)
			{
				D3DDevice* dev = getDevice();
				if (rt)
				{
					D3D9RenderTarget* oldRt = m_cachedRenderTarget[index];

					D3D9RenderTarget* drt = static_cast<D3D9RenderTarget*>(rt);
					
					dev->SetRenderTarget(index, drt->getColorSurface());
					if (drt->getDepthSurface())
					{
						if (index)
						{
							throw Apoc3DException::createException(EX_InvalidOperation, L"Render targets with a depth buffer can only be set at index 0.");
						}
						dev->SetDepthStencilSurface(drt->getDepthSurface());
					}

					m_cachedRenderTarget[index] = drt;

					if (oldRt && oldRt != drt)
					{
						m_cachedRenderTarget[index]->Resolve();
					}
				}
				else 
				{
					if (index == 0)
					{
						dev->SetRenderTarget(0, m_defaultRT);
						dev->SetDepthStencilSurface(m_defaultDS);

						m_cachedRenderTarget[0] = 0;
					}
					else
					{
						dev->SetRenderTarget(index, 0);
						m_cachedRenderTarget[index] = 0;
					}
				}
			}

			RenderTarget* D3D9RenderDevice::GetRenderTarget(int index)
			{
				return m_cachedRenderTarget[index];
			}

			void D3D9RenderDevice::BindVertexShader(VertexShader* shader)
			{
				if (shader)
				{
					D3D9VertexShader* vs = static_cast<D3D9VertexShader*>(shader);
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
					D3D9PixelShader* ps = static_cast<D3D9PixelShader*>(shader);
					getDevice()->SetPixelShader(ps->getD3DPS());
				}
				else
				{
					getDevice()->SetPixelShader(0);
				}
			}

			void D3D9RenderDevice::Render(Material* mtrl, const RenderOperation* op, int count, int passSelID)
			{
				if (!op)
					return;

				getDevice()->SetPixelShader(NULL);
				getDevice()->SetVertexShader(NULL);

				Effect* fx = mtrl->getPassEffect(passSelID);
				if (!fx)
				{
					return;
				}

				if (m_nativeState->getAlphaBlendEnable() != mtrl->IsBlendTransparent)
				{
					m_nativeState->setAlphaBlendEnable(mtrl->IsBlendTransparent);
				}
				if (m_nativeState->getAlphaSourceBlend() != mtrl->SourceBlend)
				{
					m_nativeState->setAlphaSourceBlend(mtrl->SourceBlend);
				}
				if (m_nativeState->getAlphaDestinationBlend() != mtrl->DestinationBlend)
				{
					m_nativeState->setAlphaDestinationBlend(mtrl->DestinationBlend);
				}
				if (m_nativeState->getAlphaBlendOperation() != mtrl->BlendFunction)
				{
					m_nativeState->setAlphaBlendOperation(mtrl->BlendFunction);
				}

				if (m_nativeState->getCullMode() != mtrl->Cull)
				{
					m_nativeState->SetCullMode(mtrl->Cull);
				}



				if (m_nativeState->getAlphaTestEnable() != mtrl->AlphaTestEnabled ||
					m_nativeState->getAlphaReference() != mtrl->AlphaReference)
				{
					m_nativeState->SetAlphaTestParameters(mtrl->AlphaTestEnabled, 
						mtrl->AlphaReference);
				}

				if (m_nativeState->getDepthBufferEnabled() != mtrl->DepthTestEnabled ||
					m_nativeState->getDepthBufferWriteEnabled() != mtrl->DepthWriteEnabled)
				{
					m_nativeState->SetDepth(mtrl->DepthTestEnabled,
						mtrl->DepthWriteEnabled);
				}

				for (int j=0;j<count;j++)
				{
					const RenderOperation& rop = op[j];
					const GeometryData* gm = rop.GeometryData;
					if (!gm->VertexCount || !gm->PrimitiveCount)
					{
						continue;
					}

					m_batchCount++;
					m_primitiveCount += gm->PrimitiveCount;
					m_vertexCount += gm->VertexCount;

					// setup effect

					D3D9VertexBuffer* dvb = static_cast<D3D9VertexBuffer*>(gm->VertexBuffer);
					getDevice()->SetStreamSource(0, dvb->getD3DBuffer(), 0, gm->VertexSize);

					getDevice()->SetVertexDeclaration(static_cast<D3D9VertexDeclaration*>(gm->VertexDecl)->getD3DDecl());

					if (gm->usesIndex())
					{
						D3D9IndexBuffer* dib = static_cast<D3D9IndexBuffer*>(gm->IndexBuffer);
						getDevice()->SetIndices(dib->getD3DBuffer());

						getDevice()->DrawIndexedPrimitive(D3D9Utils::ConvertPrimitiveType(gm->PrimitiveType), 
							gm->BaseVertex, 0,
							gm->VertexCount, 0, 
							gm->PrimitiveCount);
					}
					else
					{
						getDevice()->DrawPrimitive(D3D9Utils::ConvertPrimitiveType(gm->PrimitiveType),
							0, gm->PrimitiveCount);
					}
				}
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