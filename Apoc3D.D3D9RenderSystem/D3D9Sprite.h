#pragma once
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

#ifndef D3D9SPRITE_H
#define D3D9SPRITE_H

#include "D3D9Common.h"

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
		namespace D3D9RenderSystem
		{
			class D3D9Sprite final : public Sprite
			{
			public:
				D3D9Sprite(D3D9RenderDevice* device);
				~D3D9Sprite();

				virtual void Begin(SpriteSettings settings) override;
				virtual void End() override;

				virtual void Flush();

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

				D3D9VertexDeclaration* m_vtxDecl;
				D3D9VertexDeclaration* m_vtxDeclShadable;
				D3D9VertexBuffer* m_quadBuffer;
				D3D9IndexBuffer* m_quadIndices;

				D3D9RenderDevice* m_device;
				D3DDevice* m_rawDevice;

			};
		}
	}
}

#endif