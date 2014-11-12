#pragma once
#ifndef APOC3D_SPRITE_H
#define APOC3D_SPRITE_H

/**
 * -----------------------------------------------------------------------------
 * This source file is part of Apoc3D Engine
 * 
 * Copyright (c) 2009+ Tao Xin
 * 
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 * 
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 * 
 * You should have received a copy of the GNU General Public License
 * along with this program.  if not, write to the Free Software Foundation, 
 * Inc., 59 Temple Place - Suite 330, Boston, MA 02111-1307, USA, or go to
 * http://www.gnu.org/copyleft/gpl.txt.
 * 
 * -----------------------------------------------------------------------------
 */

#include "apoc3d/Math/Matrix.h"
#include "apoc3d/Math/Rectangle.h"

using namespace Apoc3D::Math;

namespace Apoc3D
{
	namespace Graphics
	{
		namespace RenderSystem
		{
			/**
			 *  Sprite is a utility used to draw textured rectangles in viewport. 
			 *
			 *  Sprite can work with a built-in matrix stack. When using matrix 
			 *  stack, please notice that you should keep the stack balanced. 
			 *  A call to SetTransform will push a matrix into the stack.
			 */
			class APAPI Sprite
			{
			public:
				enum SpriteSettings
				{
					/** Modify render states, but not restore them  */
					SPR_ChangeState = 1,  // 01
					/** Keep render states unchanged */
					SPR_KeepState = 2,	// 10
					/** Modify render states when Begin() and restore when calling End() */
					SPR_RestoreState = 3,	// 11


					SPR_AlphaBlended = 4,
					SPR_UsePostTransformStack = 8,

					SPR_AllowShading = 16,

					SPRMix_RestoreStateAlphaBlended = SPR_AlphaBlended | SPR_RestoreState
				};


				virtual ~Sprite();

				virtual void Begin(SpriteSettings settings);
				virtual void End();

				void ResetBatchCount() { m_batchCount = 0; }

				void Draw(Texture* texture, const Apoc3D::Math::Rectangle& rect, uint color);
				void Draw(Texture* texture, const Point& pos, uint color);
				void Draw(Texture* texture, int x, int y, uint color);
				void Draw(Texture* texture, const Apoc3D::Math::RectangleF &rect, uint color);
				void Draw(Texture* texture, const Vector2& pos, uint color);
				void Draw(Texture* texture, const Apoc3D::Math::Rectangle& dstRect, const Apoc3D::Math::Rectangle* srcRect, uint color);

				void DrawCircle(Texture* texture, const Apoc3D::Math::Rectangle& dstRect, const Apoc3D::Math::Rectangle* srcRect, int32 div, uint color);
				void DrawCircle(Texture* texture, const Apoc3D::Math::RectangleF& dstRect, const Apoc3D::Math::RectangleF* srcRect, int32 div, uint color);
				void DrawCircle(Texture* texture, const Apoc3D::Math::Rectangle& dstRect, const Apoc3D::Math::Rectangle* srcRect,
					float beginAngle, float endAngle, int32 div, uint color);

				void DrawCircleArc(Texture* texture, const Apoc3D::Math::Rectangle& dstRect, const Apoc3D::Math::Rectangle* srcRect, float lineWidth, int32 div, uint color);
				void DrawCircleArc(Texture* texture, const Apoc3D::Math::RectangleF& dstRect, const Apoc3D::Math::RectangleF* srcRect, float lineWidth, int32 div, uint color);
				void DrawCircleArc(Texture* texture, const Apoc3D::Math::Rectangle& dstRect, const Apoc3D::Math::Rectangle* srcRect, float lineWidth,
					float beginAngle, float endAngle, int32 div, uint color);
				
				void DrawRoundedRect(Texture* texture, const Apoc3D::Math::Rectangle& dstRect, const Apoc3D::Math::Rectangle* srcRect,
					float cornerRadius, int32 div, uint color);


				virtual void DrawCircle(Texture* texture, const Apoc3D::Math::RectangleF& dstRect, const Apoc3D::Math::RectangleF* srcRect,
					float beginAngle, float endAngle, int32 div, uint color) = 0;
				virtual void DrawCircleArc(Texture* texture, const Apoc3D::Math::RectangleF& dstRect, const Apoc3D::Math::RectangleF* srcRect, float lineWidth,
					float beginAngle, float endAngle, int32 div, uint color) = 0;

				virtual void DrawRoundedRect(Texture* texture, const Apoc3D::Math::RectangleF& dstRect, const Apoc3D::Math::RectangleF* srcRect,
					float cornerRadius, int32 div, uint color) = 0;
				
				virtual void Draw(Texture* texture, const PointF& pos, uint color) = 0;
				virtual void Draw(Texture* texture, const Apoc3D::Math::RectangleF& dstRect, const Apoc3D::Math::RectangleF* srcRect, uint color) = 0;
				virtual void Draw(Texture* texture, const Apoc3D::Math::RectangleF& dstRect, const Apoc3D::Math::RectangleF* srcRect, 
					uint tlColor, uint trColor, uint blColor, uint brColor) = 0;

				virtual void DrawTiled(Texture* texture, const PointF& pos, float uScale, float vScale, float uBias, float vBias, uint color) = 0;
				virtual void DrawTiled(Texture* texture, const Apoc3D::Math::RectangleF& dstRect, float uScale, float vScale, float uBias, float vBias, uint color) = 0;

				virtual void Flush() = 0;

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

				/**
				 *  Set current transform. If using matrix stack, pushes the matrix onto the stack as well.
				 */
				virtual void SetTransform(const Matrix& matrix);

				RenderDevice* getRenderDevice() const { return m_renderDevice; }
				bool isUsingStack() const { return !!(m_currentSettings & SPR_UsePostTransformStack); }

				int32 getBatchCount() const { return m_batchCount; }
			protected:
				Sprite(RenderDevice* rd);

				SpriteSettings getSettings() const { return m_currentSettings; }

				bool m_began = false;
				int32 m_batchCount = 0;
			private:
				RenderDevice* m_renderDevice;
				Matrix m_transform;
				MatrixStack m_stack;

				SpriteSettings m_currentSettings;

			};
		}
	}
}
#endif