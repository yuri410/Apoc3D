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
#ifndef SPRITE_H
#define SPRITE_H

#include "Common.h"
#include "Math/Vector.h"
#include "Math/Rectangle.h"
#include "Math/Matrix.h"

using namespace Apoc3D::Math;

namespace Apoc3D
{
	namespace Graphics
	{
		namespace RenderSystem
		{
			/** Sprite is a utility used to draw textured rectangles in viewport. 
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
					/** Modify render states, but not restore them
					*/
					SPR_ChangeState=1,  // 01
					/** Keep render states unchanged
					*/
					SPR_KeepState=2,	// 10
					/** Modify render states when Begin() and restore when calling End()
					*/
					SPR_RestoreState=3,	// 11
					
					


					SPR_AlphaBlended =4,
					SPR_UsePostTransformStack=8
				};


				RenderDevice* getRenderDevice() const { return m_renderDevice; }
				bool isUsingStack() const { return !!(m_currentSettings & SPR_UsePostTransformStack); }


				virtual ~Sprite();

				virtual void Begin(SpriteSettings settings)
				{
					m_currentSettings = settings;
				}

				virtual void End() = 0;
				//virtual void DrawQuad(const GeometryData* quad, PostEffect* effect) = 0;
				virtual void Draw(Texture* texture, const Apoc3D::Math::Rectangle &rect, uint color) = 0;
				virtual void Draw(Texture* texture, Vector2 pos, uint color) = 0;
				virtual void Draw(Texture* texture, const PointF& pos, uint color) = 0;
				virtual void Draw(Texture* texture, const Point& pos, uint color) = 0;
				virtual void Draw(Texture* texture, int x, int y, uint color) = 0;
				virtual void Draw(Texture* texture, const Apoc3D::Math::Rectangle& dstRect, const Apoc3D::Math::Rectangle* srcRect, uint color) = 0;
				
				virtual void Flush() = 0;

				const Matrix& getTransform() const 
				{
					if (m_currentSettings & SPR_UsePostTransformStack)
					{
						if (m_stack.getCount())
							return m_stack.Peek();
						return Matrix::Identity;
					}
					return m_transform; 
				}

				/** When using matrix stack, pop the current matrix and restore to a previous transform
					state. Throws exception if not using matrix stack.
				*/
				void PopTransform();

				/** Multiply the current transform matrix by a given matrix. If using matrix stack, push
					the result onto the stack as well.
				*/
				void MultiplyTransform(const Matrix& matrix)
				{
					Matrix result;
					Matrix::Multiply(result, getTransform(), matrix);
					SetTransform(result);
				}

				/** Set current transform. If using matrix stack, pushes the matrix onto the stack as well.
				*/
				virtual void SetTransform(const Matrix& matrix)
				{
					if (m_currentSettings & SPR_UsePostTransformStack)
					{
						m_stack.PushMatrix(matrix);
					}
					else
					{
						m_transform = matrix;
					}
				}

			private:
				RenderDevice* m_renderDevice;
				Matrix m_transform;
				MatrixStack m_stack;

				SpriteSettings m_currentSettings;

			protected:
				Sprite(RenderDevice* rd);

				SpriteSettings getSettings() const { return m_currentSettings; }
			};
		}
	}
}
#endif