/*
-----------------------------------------------------------------------------
This source file is part of Apoc3D Engine

Copyright (c) 2009+ Tao Xin

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

				SetRenderTarget(idx, rt);
			}
			ScopeRenderTargetChange::ScopeRenderTargetChange(RenderDevice* device, std::initializer_list<std::pair<int32, RenderTarget* >> list)
				: m_device(device)
			{
				ZeroArray(m_oldRenderTargets);
				ZeroArray(m_oldRenderTargetChanged);

				for (const auto& e : list)
				{
					SetRenderTarget(e.first, e.second);
				}
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
			}

			void ScopeRenderTargetChange::SetRenderTarget(int32 idx, RenderTarget* rt)
			{
				RenderTarget* oldRt = m_device->GetRenderTarget(idx);

				if (idx < countof(m_oldRenderTargets))
				{
					m_oldRenderTargets[idx] = oldRt;
					m_oldRenderTargetChanged[idx] = true;
				}
				else
				{
					m_additionalOldRenderTarget.Add(idx, oldRt);
				}
			}

		}
	}
}