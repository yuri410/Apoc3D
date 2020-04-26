#pragma once

/* -----------------------------------------------------------------------
 * This source file is part of Apoc3D Framework
 *
 * Copyright (c) 2011-2019 Tao Xin
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

#ifndef GL3SPRITE_H
#define GL3SPRITE_H

#include "GL3Common.h"
#include "apoc3d/Graphics/RenderSystem/Sprite.h"
#include "apoc3d/Graphics/GraphicsCommon.h"

using namespace Apoc3D::Graphics::RenderSystem;
using namespace Apoc3D::Math;

namespace Apoc3D
{
	namespace Graphics
	{
		namespace GL3RenderSystem
		{
			class GL3Sprite : public Sprite
			{
			public:
				GL3Sprite(GL3RenderDevice* device);
				~GL3Sprite();

				virtual void Begin(SpriteSettings settings);
				virtual void End();

				virtual void Submit(const SpriteDrawEntries& batch);

			private:
				GL3RenderDevice* m_gldevice;

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

				GL3VertexDeclaration* m_vtxDecl;
				GL3VertexBuffer* m_quadBuffer;
				GL3IndexBuffer* m_quadIndices;

				GL3RenderDevice* m_device;

			};
		}
	}
}

#endif