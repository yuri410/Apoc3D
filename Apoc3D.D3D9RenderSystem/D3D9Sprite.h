#pragma once
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

				virtual void Draw(Texture* texture, const PointF& pos, uint color) override;
				virtual void Draw(Texture* texture, const Apoc3D::Math::RectangleF& dstRect, const Apoc3D::Math::RectangleF* srcRect, uint color) override;
				virtual void Draw(Texture* texture, const Apoc3D::Math::RectangleF& dstRect, const Apoc3D::Math::RectangleF* srcRect,
					uint tlColor, uint trColor, uint blColor, uint brColor) override;

				virtual void Draw(Texture* texture, const PointF(&corners)[4], const Apoc3D::Math::RectangleF* srcRect, const uint(&colors)[4]) override;
				virtual void Draw(Texture* texture, const PointF(&corners)[4], const PointF(&texCoords)[4], const uint(&colors)[4]) override;

				virtual void DrawTiled(Texture* texture, const Apoc3D::Math::RectangleF& dstRect, float uScale, float vScale, float uBias, float vBias, uint color) override;
				virtual void DrawTiled(Texture* texture, const PointF& pos, float uScale, float vScale, float uBias, float vBias, uint color) override;

				virtual void DrawRoundedRect(Texture* texture, const Apoc3D::Math::RectangleF& dstRect, const Apoc3D::Math::RectangleF* srcRect,
					float cornerRadius, int32 div, uint color) override;
				virtual void DrawCircle(Texture* texture, const Apoc3D::Math::RectangleF& dstRect, const Apoc3D::Math::RectangleF* srcRect,
					float beginAngle, float endAngle, int32 div, uint color) override;

				virtual void DrawCircleArc(Texture* texture, const Apoc3D::Math::RectangleF& dstRect, const Apoc3D::Math::RectangleF* srcRect, float width,
					float beginAngle, float endAngle, int32 div, uint color) override;


				virtual void Flush();

				virtual void SetTransform(const Matrix& matrix);
			private:
				struct QuadVertex
				{
					float Position[4];
					uint Diffuse;
					float TexCoord[2];
				};
				struct DrawEntry
				{
					D3D9Texture* Tex = nullptr;

					QuadVertex TL;
					QuadVertex TR;
					QuadVertex BL;
					QuadVertex BR;

					bool IsUVExtended = false;

					void FillNormalDraw(Texture* texture, const Matrix& baseTrans,
						const PointF& tl_dp, const PointF& tr_dp, const PointF& bl_dp, const PointF& br_dp,
						const PointF& tl_sp, const PointF& tr_sp, const PointF& bl_sp, const PointF& br_sp,
						uint color);

					void FillNormalDraw(Texture* texture, const Matrix& baseTrans, float x, float y, uint color);
					void FillNormalDraw(Texture* texture, const Matrix& baseTrans, const Apoc3D::Math::RectangleF& dstRect, const Apoc3D::Math::RectangleF* srcRect, uint color);

					void FillTransformedDraw(Texture* texture, const Matrix& t, const Apoc3D::Math::RectangleF* srcRect, uint color);

					void SetTexture(Texture* tex);

					void SetPositions(const Matrix& baseTrans, const PointF& tl_dp, const PointF& tr_dp, const PointF& bl_dp, const PointF& br_dp);

					void SetSrcRect(Texture* texture, const Apoc3D::Math::RectangleF* srcRect);
					void SetColors(uint color);
					void SetColors(uint tlColor, uint trColor, uint blColor, uint brColor);

					void SetTextureCoords(const PointF& tl_sp, const PointF& tr_sp, const PointF& bl_sp, const PointF& br_sp);

					void ChangeUV(float uScale, float vScale, float uBias, float vBias);
				};

				void EnqueueDrawEntry(const DrawEntry& drawE);
				
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

				List<DrawEntry> m_deferredDraws;

			};
		}
	}
}

#endif