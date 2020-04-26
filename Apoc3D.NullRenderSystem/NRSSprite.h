#pragma once

/* -----------------------------------------------------------------------
 * This source file is part of Apoc3D Framework
 * 
 * Copyright (c) 2011-2018 Tao Xin
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

#ifndef NRSSPRITE_H
#define NRSSPRITE_H

#include "NRSCommon.h"

#include "apoc3d/Collections/List.h"
#include "apoc3d/Graphics/RenderSystem/Sprite.h"
#include "apoc3d/Graphics/GraphicsCommon.h"

using namespace Apoc3D::Collections;
using namespace Apoc3D::Graphics::RenderSystem;
using namespace Apoc3D::Math;

namespace Apoc3D
{
	namespace Graphics
	{
		namespace NullRenderSystem
		{
			class NRSSprite final : public Sprite
			{
			public:
				NRSSprite(NRSRenderDevice* device);
				~NRSSprite();

				virtual void Begin(SpriteSettings settings) override;
				virtual void End() override;

				virtual void Submit(const SpriteDrawEntries& batch) override;

			private:

				void SetUVExtendedState(bool isExtended);

				void SetRenderState();
				void RestoreRenderState();

				struct  
				{
					bool oldAlphaBlendEnable;
					BlendFunction oldBlendFunc;
					Blend oldSrcBlend;
					Blend oldDstBlend;
					uint oldBlendFactor;
					bool oldDepthEnabled;
					CullMode oldCull;
				} m_storedState;

				NRSVertexDeclaration* m_vtxDecl;
				NRSVertexDeclaration* m_vtxDeclShadable;
				NRSVertexBuffer* m_quadBuffer;
				NRSIndexBuffer* m_quadIndices;

				NRSRenderDevice* m_device;

				List<DrawEntry> m_deferredDraws;

			};
		}
	}
}

#endif