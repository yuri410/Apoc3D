#pragma once
#ifndef APOC3D_SPRITE_H
#define APOC3D_SPRITE_H

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

#include "apoc3d/Math/Rectangle.h"
#include "apoc3d/Math/MatrixStack.h"
#include "Apoc3D/Collections/List.h"

using namespace Apoc3D::Math;
using namespace Apoc3D::Collections;

namespace Apoc3D
{
	namespace Graphics
	{
		namespace RenderSystem
		{
			enum struct LineCapOptions
			{
				Butt,
				Round,
				Square
			};

			enum struct LineCornerOptions
			{
				Miter,
				Round,
				Bevel
			};

			class APAPI SpriteDrawEntries
			{
			public:

				struct QuadVertex
				{
					float Position[4];
					uint  Diffuse;
					float TexCoord[2];
				};

				struct DrawEntry
				{
					Texture* Tex = nullptr;

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

				void Add(const DrawEntry& e) { m_drawsEntires.Add(e); }

				void Clear() { m_drawsEntires.Clear(); }

				int32 getCount() const { return m_drawsEntires.getCount(); }

				template <int GroupSize>
				GroupAccessor<DrawEntry, GroupSize> getGroupAccessor() const { return GroupAccessor<DrawEntry, GroupSize>(m_drawsEntires); }

			private:
				List<DrawEntry> m_drawsEntires;
			};

			/**
			*  Sprite is a utility used to draw textured rectangles in viewport.
			*
			*  Sprite can work with a built-in matrix stack. When using matrix
			*  stack, please notice that you should keep the stack balanced.
			*  A call to SetTransform will push a matrix into the stack.
			*/
			class APAPI Sprite
			{
				friend class SpriteBeginEndScope;
			public:
				typedef Point Point_A4[4];
				typedef PointF PointF_A4[4];

				typedef uint ColorValue_A4[4];

				typedef Apoc3D::Math::Rectangle Rectangle;

				enum SpriteSettings
				{
					/** Modify render states, but not restore them  */
					SPR_ChangeState = 1 << 0,
					/** Store render states */
					SPR_RestoreState = 1 << 1,

					SPR_AlphaBlended = 1 << 2,
					SPR_UseTransformStack = 1 << 3,

					SPR_AllowShading = 1 << 4,

					SPR_RecordBatch = 1 << 5,

					/** Modify render states when Begin() and restore when calling End() */
					SPRMix_ManageState = SPR_RestoreState | SPR_ChangeState,
					SPRMix_ManageStateAlphaBlended = SPR_AlphaBlended | SPRMix_ManageState
				};

				virtual ~Sprite();

				virtual void Begin(SpriteSettings settings);
				virtual void End();

				virtual void Submit(const SpriteDrawEntries& batch) = 0;

				void Flush();

				//////////////////////////////////////////////////////////////////////////

				void Draw(Texture* texture, int x, int y, uint color);
				void Draw(Texture* texture, const Point& pos, uint color);
				void Draw(Texture* texture, const Vector2& pos, uint color);
				void Draw(Texture* texture, const PointF& pos, uint color);

				void Draw(Texture* texture, const Rectangle& rect, uint color);
				void Draw(Texture* texture, const RectangleF &rect, uint color);
				void Draw(Texture* texture, const Rectangle& dstRect, const Rectangle* srcRect, uint color);
				void Draw(Texture* texture, const RectangleF& dstRect, const RectangleF* srcRect, uint color);

				void Draw(Texture* texture, const Rectangle& dstRect, const Rectangle* srcRect,
					uint tlColor, uint trColor, uint blColor, uint brColor);
				void Draw(Texture* texture, const RectangleF& dstRect, const RectangleF* srcRect,
					uint tlColor, uint trColor, uint blColor, uint brColor);

				void Draw(Texture* texture, const Point_A4& corners, const Rectangle* srcRect, const ColorValue_A4& colors);
				void Draw(Texture* texture, const PointF_A4& corners, const RectangleF* srcRect, const ColorValue_A4& colors);
				void Draw(Texture* texture, const PointF_A4& corners, const PointF_A4& texCoords, const ColorValue_A4& colors);

				void DrawTiled(Texture* texture, const PointF& pos, const PointF& uvScale, const PointF& uvShift, uint color);
				void DrawTiled(Texture* texture, const RectangleF& dstRect, const PointF& uvScale, const PointF& uvShift, uint color);

				void DrawCircle(Texture* texture, const Rectangle& dstRect, const Rectangle* srcRect, int32 div, uint color);
				void DrawCircle(Texture* texture, const RectangleF& dstRect, const RectangleF* srcRect, int32 div, uint color);
				void DrawCircle(Texture* texture, const Rectangle& dstRect, const Rectangle* srcRect,
					float beginAngle, float endAngle, int32 div, uint color);
				void DrawCircle(Texture* texture, const RectangleF& dstRect, const RectangleF* srcRect,
					float beginAngle, float endAngle, int32 div, uint color);

				void DrawCircleArc(Texture* texture, const Rectangle& dstRect, const Rectangle* srcRect, float lineWidth, int32 div, uint color);
				void DrawCircleArc(Texture* texture, const RectangleF& dstRect, const RectangleF* srcRect, float lineWidth, int32 div, uint color);
				void DrawCircleArc(Texture* texture, const Rectangle& dstRect, const Rectangle* srcRect, float lineWidth,
					float beginAngle, float endAngle, int32 div, uint color);
				void DrawCircleArc(Texture* texture, const RectangleF& dstRect, const RectangleF* srcRect, float lineWidth,
					float beginAngle, float endAngle, int32 div, uint color);

				void DrawRoundedRect(Texture* texture, const Rectangle& dstRect, const Rectangle* srcRect,
					float cornerRadius, int32 div, uint color);
				void DrawRoundedRect(Texture* texture, const RectangleF& dstRect, const RectangleF* srcRect,
					float cornerRadius, int32 div, uint color);

				void DrawRoundedRectBorder(Texture* texture, const Rectangle& dstRect, const Rectangle* srcRect, float width,
					float cornerRadius, int32 div, uint color);
				void DrawRoundedRectBorder(Texture* texture, const RectangleF& dstRect, const RectangleF* srcRect, float width,
					float cornerRadius, int32 div, uint color);

				void DrawLine(Texture* texture, const Point& start, const Point& end, uint color,
					float width, LineCapOptions caps, const PointF& uvScale = PointF(1, 1), const PointF& uvShift = PointF(0, 0));
				void DrawLine(Texture* texture, const PointF& start, const PointF& end, uint color,
					float width, LineCapOptions caps, const PointF& uvScale = PointF(1, 1), const PointF& uvShift = PointF(0, 0));

				void DrawBatch(const SpriteDrawEntries& batch);

				//////////////////////////////////////////////////////////////////////////

				const Matrix& getTransform() const;

				/**
				 *  When using matrix stack, pop the current matrix and restore to a previous transform
				 *  state. Throws exception if not using matrix stack.
				 */
				void PopTransform();

				/**
				 *  Multiply the current transform matrix by a given matrix. If using matrix stack, push
				 *  the result onto the stack as well.
				 */
				void MultiplyTransform(const Matrix& matrix);
				void PreMultiplyTransform(const Matrix& matrix);

				/** Set current transform. If using matrix stack, pushes the matrix onto the stack as well. */
				void SetTransform(const Matrix& matrix);

				const SpriteDrawEntries& getDrawEntries() { return m_drawEntries; }

				RenderDevice* getRenderDevice() const { return m_renderDevice; }
				bool isUsingStack() const { return !!(m_currentSettings & SPR_UseTransformStack); }

				int32 getBatchCount() const { return m_batchCount; }

			protected:
				static const int MaxDeferredDraws = 144;

				typedef SpriteDrawEntries::QuadVertex QuadVertex;
				typedef SpriteDrawEntries::DrawEntry DrawEntry;

				Sprite(RenderDevice* rd);

				void EnqueueDrawEntry(const DrawEntry& drawE);

				void DrawCircleGeneric(Texture* texture, const RectangleF& dstRect, const RectangleF* srcRect, float width,
					float beginAngle, float endAngle, int32 div, uint color, bool uvExt = false);

				void DrawRoundedRectGeneric(Texture* texture, const RectangleF& dstRect, const RectangleF* srcRect, float width,
					float cornerRadius, int32 div, uint color);

				SpriteSettings getSettings() const { return m_currentSettings; }


				bool m_began = false;
				int32 m_batchCount = 0;

			private:
				RenderDevice* m_renderDevice;
				Matrix m_transform;
				MatrixStack m_stack;

				SpriteSettings m_currentSettings;
				int32 m_flushThreshold;
				SpriteDrawEntries m_drawEntries;
			};

			class APAPI SpriteTransformScope
			{
			public:
				SpriteTransformScope(Sprite* spr, const Matrix& transform, bool mult);
				~SpriteTransformScope();

			private:
				Sprite* m_sprite;
				Matrix m_oldTransform;
			};

			class APAPI SpriteBeginEndScope
			{
			public:
				SpriteBeginEndScope(Sprite* spr, Sprite::SpriteSettings settings);
				virtual ~SpriteBeginEndScope();

			protected:
				Sprite* m_sprite;

			private:
				bool m_oldBegan;
				Sprite::SpriteSettings m_oldSettings;
			};

			class APAPI SpriteShadedScope : SpriteBeginEndScope
			{
			public:
				SpriteShadedScope(Sprite* spr, Sprite::SpriteSettings baseSettings, const String& effectName, Material* mtrl = nullptr);
				virtual ~SpriteShadedScope();

				EffectSystem::AutomaticEffect* getEffect() const { return m_effect; }
			private:
				EffectSystem::AutomaticEffect* m_effect;
			};
		}
	}
}
#endif