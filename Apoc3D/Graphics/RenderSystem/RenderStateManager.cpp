/* -----------------------------------------------------------------------
 * This source file is part of Apoc3D Framework
 * 
 * Copyright (c) 2009-2018 Tao Xin
 * 
 * This content of this file is subject to the terms of the Mozilla Public 
 * License v2.0. If a copy of the MPL was not distributed with this file, 
 * you can obtain one at http://mozilla.org/MPL/2.0/.
 * 
 * This program is distributed in the hope that it will be useful, 
 * WITHOUT WARRANTY OF ANY KIND; either express or implied. See the 
 * Mozilla Public License for more details.
 * 
 * ------------------------------------------------------------------------
 */

#include "RenderStateManager.h"
#include "RenderDevice.h"

namespace Apoc3D
{
	namespace Graphics
	{
		namespace RenderSystem
		{
			RenderStateManager::RenderStateManager(RenderDevice* device)
				: m_renderDevice(device)
			{

			}

			//////////////////////////////////////////////////////////////////////////
			ScopeRenderTargetChange::ScopeRenderTargetChange(RenderDevice* device, int32 idx, RenderTarget* rt)
				: m_device(device)
			{
				ZeroArray(m_oldRenderTargets);
				ZeroArray(m_oldRenderTargetChanged);

				ChangeRenderTarget(idx, rt);
			}
			ScopeRenderTargetChange::ScopeRenderTargetChange(RenderDevice* device, std::initializer_list<std::pair<int32, RenderTarget* >> list)
				: m_device(device)
			{
				ZeroArray(m_oldRenderTargets);
				ZeroArray(m_oldRenderTargetChanged);

				for (const auto& e : list)
				{
					ChangeRenderTarget(e.first, e.second);
				}
			}

			ScopeRenderTargetChange::ScopeRenderTargetChange(RenderDevice* device, int32 idx, RenderTarget* rt, DepthStencilBuffer* dsb)
				: ScopeRenderTargetChange(device, idx, rt)
			{
				ChangeDepthStencilBuffer(dsb);
			}
			ScopeRenderTargetChange::ScopeRenderTargetChange(RenderDevice* device, std::initializer_list<std::pair<int32, RenderTarget* >> list, DepthStencilBuffer* dsb)
				: ScopeRenderTargetChange(device, list)
			{	
				ChangeDepthStencilBuffer(dsb);
			}

			ScopeRenderTargetChange::~ScopeRenderTargetChange()
			{
				for (int32 i = 0; i < countof(m_oldRenderTargets);i++)
				{
					if (m_oldRenderTargetChanged[i])
					{
						m_device->SetRenderTarget(i, m_oldRenderTargets[i]);
					}
				}
				for (auto e : m_additionalOldRenderTarget)
				{
					m_device->SetRenderTarget(e.Key, e.Value);
				}

				if (m_oldDSBChanged)
				{
					m_device->SetDepthStencilBuffer(m_oldDSB);
				}
			}

			void ScopeRenderTargetChange::ChangeRenderTarget(int32 idx, RenderTarget* rt)
			{
				RenderTarget* oldRt = m_device->GetRenderTarget(idx);

				if (oldRt != rt)
				{
					if (idx < countof(m_oldRenderTargets))
					{
						m_oldRenderTargets[idx] = oldRt;
						m_oldRenderTargetChanged[idx] = true;
					}
					else
					{
						m_additionalOldRenderTarget.Add(idx, oldRt);
					}

					m_device->SetRenderTarget(idx, rt);
				}
			}
			void ScopeRenderTargetChange::ChangeDepthStencilBuffer(DepthStencilBuffer* dsb)
			{
				DepthStencilBuffer* oldDSB = m_device->GetDepthStencilBuffer();

				if (oldDSB != m_oldDSB)
				{
					m_oldDSB = oldDSB;
					m_oldDSBChanged = true;

					m_device->SetDepthStencilBuffer(oldDSB);
				}
			}

		}
	}
}