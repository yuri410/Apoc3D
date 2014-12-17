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
#include "Sprite.h"

#include "apoc3d/Exception.h"
#include "apoc3d/Math/MathCommon.h"

namespace Apoc3D
{
	namespace Graphics
	{
		namespace RenderSystem
		{
			Sprite::Sprite(RenderDevice* rd)
				: m_renderDevice(rd), m_transform(Matrix::Identity), m_stack(10), 
				m_currentSettings((SpriteSettings)(SPR_ChangeState | SPR_AlphaBlended))
			{
				
			}

			Sprite::~Sprite()
			{

			}


			void Sprite::Begin(SpriteSettings settings)
			{
				assert(!m_began);
				m_began = true;
				m_currentSettings = settings;
			}

			void Sprite::End()
			{
				assert(m_began);
				m_began = false;
			}


			void Sprite::Draw(Texture* texture, int x, int y, uint color)
			{
				Draw(texture, PointF(static_cast<float>(x), static_cast<float>(y)), color);
			}

			void Sprite::Draw(Texture* texture, const Point& pos, uint color)
			{
				Draw(texture, PointF(static_cast<float>(pos.X), static_cast<float>(pos.Y)), color);
			}

			void Sprite::Draw(Texture* texture, const Vector2& pos, uint color)
			{
				Draw(texture, PointF(pos.X, pos.Y), color);
			}
			
			void Sprite::Draw(Texture* texture, const Rectangle& rect, uint color)
			{
				Draw(texture, static_cast<RectangleF>(rect), color);
			}

			void Sprite::Draw(Texture* texture, const RectangleF &rect, uint color)
			{
				Draw(texture, rect, nullptr, color);
			}

			void Sprite::Draw(Texture* texture, const Rectangle& dstRect, const Rectangle* srcRect, uint color)
			{
				RectangleF dstRectF = dstRect;
				if (srcRect)
				{
					RectangleF srcRectF = *srcRect;
					Draw(texture, dstRectF, &srcRectF, color);
				}
				else 
					Draw(texture, dstRectF, nullptr, color);
			}

			void Sprite::Draw(Texture* texture, const Rectangle& dstRect, const Rectangle* srcRect,
				uint tlColor, uint trColor, uint blColor, uint brColor)
			{
				RectangleF dstRectF = dstRect;

				if (srcRect)
				{
					RectangleF srcRectF = *srcRect;
					Draw(texture, dstRectF, &srcRectF, tlColor, trColor, blColor, brColor);
				}
				else 
					Draw(texture, dstRectF, nullptr, tlColor, trColor, blColor, brColor);
			}

			void Sprite::Draw(Texture* texture, const Point_A4& corners, const Rectangle* srcRect, const ColorValue_A4& colors)
			{
				PointF c[4] = { corners[0], corners[1], corners[2], corners[3] };

				if (srcRect)
				{
					RectangleF srcRectF = *srcRect;
					Draw(texture, c, &srcRectF, colors);
				}
				else 
					Draw(texture, c, nullptr, colors);
			}

			//////////////////////////////////////////////////////////////////////////

			void Sprite::DrawCircle(Texture* texture, const Rectangle& dstRect, const Rectangle* srcRect, int32 div, uint color)
			{
				DrawCircle(texture, dstRect, srcRect, 0.0f, Math::PI * 2, div, color);
			}
			void Sprite::DrawCircle(Texture* texture, const RectangleF& dstRect, const RectangleF* srcRect, int32 div, uint color)
			{
				DrawCircle(texture, dstRect, srcRect, 0.0f, Math::PI * 2, div, color);
			}

			void Sprite::DrawCircle(Texture* texture, const Rectangle& dstRect, const Rectangle* srcRect,
				float beginAngle, float endAngle, int32 div, uint color)
			{
				RectangleF dstRectF = dstRect;

				if (srcRect)
				{
					RectangleF srcRectF = *srcRect;
					DrawCircle(texture, dstRectF, &srcRectF, beginAngle, endAngle, div, color);
				}
				else 
					DrawCircle(texture, dstRectF, nullptr, beginAngle, endAngle, div, color);
			}

			//////////////////////////////////////////////////////////////////////////

			void Sprite::DrawCircleArc(Texture* texture, const Rectangle& dstRect, const Rectangle* srcRect, float lineWidth, int32 div, uint color)
			{
				DrawCircleArc(texture, dstRect, srcRect, lineWidth, 0.0f, Math::PI * 2, div, color);
			}
			void Sprite::DrawCircleArc(Texture* texture, const RectangleF& dstRect, const RectangleF* srcRect, float lineWidth, int32 div, uint color)
			{
				DrawCircleArc(texture, dstRect, srcRect, lineWidth, 0.0f, Math::PI * 2, div, color);
			}
			void Sprite::DrawCircleArc(Texture* texture, const Rectangle& dstRect, const Rectangle* srcRect, float lineWidth,
				float beginAngle, float endAngle, int32 div, uint color)
			{
				RectangleF dstRectF = dstRect;

				if (srcRect)
				{
					RectangleF srcRectF = *srcRect;
					DrawCircleArc(texture, dstRectF, &srcRectF, lineWidth, beginAngle, endAngle, div, color);
				}
				else
					DrawCircleArc(texture, dstRectF, nullptr, lineWidth, beginAngle, endAngle, div, color);
			}
			
			//////////////////////////////////////////////////////////////////////////

			void Sprite::DrawRoundedRect(Texture* texture, const Rectangle& dstRect, const Rectangle* srcRect,
				float cornerRadius, int32 div, uint color)
			{
				RectangleF dstRectF = dstRect;

				if (srcRect)
				{
					RectangleF srcRectF = *srcRect;
					DrawRoundedRect(texture, dstRectF, &srcRectF, cornerRadius, div, color);
				}
				else
					DrawRoundedRect(texture, dstRectF, nullptr, cornerRadius, div, color);
			}

			void Sprite::DrawRoundedRectBorder(Texture* texture, const Rectangle& dstRect, const Rectangle* srcRect,
				float cornerRadius, float width, int32 div, uint color)
			{
				RectangleF dstRectF = dstRect;

				if (srcRect)
				{
					RectangleF srcRectF = *srcRect;
					DrawRoundedRectBorder(texture, dstRectF, &srcRectF, cornerRadius, width, div, color);
				}
				else
					DrawRoundedRectBorder(texture, dstRectF, nullptr, cornerRadius, width, div, color);
			}
			void Sprite::DrawLine(Texture* texture, const Point& start, const Point& end, uint color, 
				float width, LineCapOptions caps, const PointF& uvScale, const PointF& uvShift)
			{
				DrawLine(texture, PointF(start), PointF(end), color, width, caps, uvScale, uvShift);
			}

			//////////////////////////////////////////////////////////////////////////

			const Matrix& Sprite::getTransform() const
			{
				if (m_currentSettings & SPR_UsePostTransformStack)
				{
					if (m_stack.getCount())
						return m_stack.Peek();
					return Matrix::Identity;
				}
				return m_transform;
			}

			void Sprite::PopTransform()
			{
				if (m_currentSettings & SPR_UsePostTransformStack)
				{
					m_stack.PopMatrix();
				}
				else
				{
					throw AP_EXCEPTION(ExceptID::InvalidOperation, L"The sprite is not begun with SPR_UsePostTransformStack.");
				}
			}

			void Sprite::MultiplyTransform(const Matrix& matrix)
			{
				Matrix result;
				Matrix::Multiply(result, getTransform(), matrix);
				SetTransform(result);
			}
			void Sprite::PreMultiplyTransform(const Matrix& matrix)
			{
				Matrix result;
				Matrix::Multiply(result, matrix, getTransform());
				SetTransform(result);
			}

			void Sprite::SetTransform(const Matrix& matrix)
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


			//////////////////////////////////////////////////////////////////////////

			SpriteTransformScope::SpriteTransformScope(Sprite* spr, const Matrix& transform)
				: m_sprite(spr)
			{
				if (!m_sprite->isUsingStack())
				{
					m_oldTransform = m_sprite->getTransform();
				}

				m_sprite->SetTransform(transform);
			}

			SpriteTransformScope::~SpriteTransformScope()
			{
				if (m_sprite->isUsingStack())
					m_sprite->PopTransform();
				else
				{
					m_sprite->SetTransform(m_oldTransform);
				}
			}


		}
	}
}