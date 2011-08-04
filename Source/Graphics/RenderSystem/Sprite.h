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
			class APAPI Sprite
			{
			private:
				RenderDevice* m_renderDevice;
				Matrix m_transform;
				MatrixStack m_stack;

				bool m_useStack;

			protected:
				Sprite(RenderDevice* rd);
				
			public:
				RenderDevice* getRenderDevice() const { return m_renderDevice; }
				bool isUsingStack() const { return m_useStack; }


				virtual ~Sprite();

				virtual void Begin(bool alphabled, bool useStack)
				{
					m_useStack = useStack;
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
					if (m_useStack)
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
					if (m_useStack)
					{
						m_stack.PushMatrix(matrix);
					}
					else
					{
						m_transform = matrix;
					}
				}
			};
		}
	}
}
#endif