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

#ifndef D3D9SPRITE_H
#define D3D9SPRITE_H

#include "D3D9Common.h"
#include "Graphics/RenderSystem/Sprite.h"
#include "Graphics/GraphicsCommon.h"
#include "Collections/FastList.h"

using namespace Apoc3D::Collections;
using namespace Apoc3D::Graphics::RenderSystem;
using namespace Apoc3D::Math;

namespace Apoc3D
{
	namespace Graphics
	{
		namespace D3D9RenderSystem
		{
			class D3D9Sprite : public Sprite
			{
			public:
				D3D9Sprite(D3D9RenderDevice* device);
				~D3D9Sprite();

				virtual void Begin(SpriteSettings settings);
				virtual void End();

				virtual void Draw(Texture* texture, const Apoc3D::Math::Rectangle &rect, uint color)
				{
					Draw(texture, rect, 0, color);
				}
				virtual void Draw(Texture* texture, Vector2 pos, uint color);
				virtual void Draw(Texture* texture, const PointF& pos, uint color);
				virtual void Draw(Texture* texture, const Point& pos, uint color);
				virtual void Draw(Texture* texture, int x, int y, uint color);
				virtual void Draw(Texture* texture, const Apoc3D::Math::Rectangle& dstRect, const Apoc3D::Math::Rectangle* srcRect, uint color);
				virtual void Draw(Texture* texture, const Apoc3D::Math::Rectangle& dstRect, float uScale, float vScale, float uBias, float vBias, uint color);
				virtual void Draw(Texture* texture, const PointF& pos, float uScale, float vScale, float uBias, float vBias, uint color);

				virtual void Flush();

				virtual void SetTransform(const Matrix& matrix);
			private:
				void AddNormalDraw(Texture* texture, float x, float y, uint color);
				void AddTransformedDraw(Texture* texture, const Matrix& t, const RECT* srcRect, uint color);

				void SetRenderState();
				void RestoreRenderState();

				struct QuadVertex
				{
					float Position[4];
					uint Diffuse;
					float TexCoord[2];
				};
				struct DrawEntry
				{
					D3D9Texture* Tex;

					QuadVertex TL;
					QuadVertex TR;
					QuadVertex BL;
					QuadVertex BR;

					bool IsUVExtended;
				};
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
				D3D9VertexBuffer* m_quadBuffer;

				D3D9RenderDevice* m_device;
				D3DDevice* m_rawDevice;
				SpriteSettings m_currentSettings;

				FastList<DrawEntry> m_deferredDraws;

			};
		}
	}
}

#endif