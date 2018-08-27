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

				virtual void Draw(Texture* texture, const PointF& pos, uint color) override;
				virtual void Draw(Texture* texture, const RectangleF& dstRect, const RectangleF* srcRect, uint color) override;
				virtual void Draw(Texture* texture, const RectangleF& dstRect, const RectangleF* srcRect,
					uint tlColor, uint trColor, uint blColor, uint brColor) override;

				virtual void Draw(Texture* texture, const PointF_A4& corners, const RectangleF* srcRect, const ColorValue_A4& colors) override;
				virtual void Draw(Texture* texture, const PointF_A4& corners, const PointF_A4& texCoords, const ColorValue_A4& colors) override;

				virtual void DrawTiled(Texture* texture, const PointF& pos, const PointF& uvScale, const PointF& uvShift, uint color) override;
				virtual void DrawTiled(Texture* texture, const RectangleF& dstRect, const PointF& uvScale, const PointF& uvShift, uint color) override;

				virtual void DrawCircle(Texture* texture, const RectangleF& dstRect, const RectangleF* srcRect,
					float beginAngle, float endAngle, int32 div, uint color) override;

				virtual void DrawCircleArc(Texture* texture, const RectangleF& dstRect, const RectangleF* srcRect, float width,
					float beginAngle, float endAngle, int32 div, uint color) override;

				virtual void DrawRoundedRect(Texture* texture, const RectangleF& dstRect, const RectangleF* srcRect,
					float cornerRadius, int32 div, uint color) override;

				virtual void DrawRoundedRectBorder(Texture* texture, const RectangleF& dstRect, const RectangleF* srcRect, float width,
					float cornerRadius, int32 div, uint color) override;

				virtual void DrawLine(Texture* texture, const PointF& start, const PointF& end, uint color, 
					float width, LineCapOptions caps, const PointF& uvScale, const PointF& uvShift) override;

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


					void SetTexture(Texture* tex);

					void SetPositions(const Matrix& baseTrans, const PointF& tl_dp, const PointF& tr_dp, const PointF& bl_dp, const PointF& br_dp);

					void SetSrcRect(Texture* texture, const Apoc3D::Math::RectangleF* srcRect);
					void SetColors(uint color);
					void SetColors(uint tlColor, uint trColor, uint blColor, uint brColor);

					void SetTextureCoords(const PointF& tl_sp, const PointF& tr_sp, const PointF& bl_sp, const PointF& br_sp);

					void ChangeUV(float uScale, float vScale, float uBias, float vBias);
					void ChangeUV(const PointF& uvScale, const PointF& uvShift);

				};

				void DrawCircleGeneric(Texture* texture, const RectangleF& dstRect, const RectangleF* srcRect, float width,
					float beginAngle, float endAngle, int32 div, uint color, bool uvExt = false);

				void DrawRoundedRectGeneric(Texture* texture, const RectangleF& dstRect, const RectangleF* srcRect, float width,
					float cornerRadius, int32 div, uint color);


				void EnqueueDrawEntry(const DrawEntry& drawE);
				
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

				List<DrawEntry> m_deferredDraws;

			};
		}
	}
}

#endif