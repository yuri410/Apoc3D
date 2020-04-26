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

#include "Sprite.h"

#include "apoc3d/Math/MathCommon.h"
#include "apoc3D/Math/Math.h"
#include "apoc3d/Graphics/EffectSystem/Effect.h"
#include "apoc3d/Graphics/EffectSystem/EffectManager.h"
#include "Apoc3D/Graphics/RenderSystem/Texture.h"

using namespace Apoc3D;

namespace
{
	RectangleF ObtainSrcRect(Texture* texture, const RectangleF* _srcRect)
	{
		RectangleF srcRect;
		if (_srcRect)
			srcRect = *_srcRect;
		else
			srcRect = { 0, 0, static_cast<float>(texture->getWidth()), static_cast<float>(texture->getHeight()) };

		return srcRect;
	}

	struct CircleParameters
	{
		float xRadius;
		float yRadius;
		RectangleF srcRect;

		float srcRectXRatio;
		float srcRectYRatio;

		int32 segmentCount;
		float angleStep;

		CircleParameters(Texture* texture, const RectangleF& dstRect, const RectangleF* _srcRect,
			float beginAngle, float endAngle, int32 div, int32 minDiv = 2)
		{
			yRadius = dstRect.Height / 2;
			xRadius = dstRect.Width / 2;

			srcRect = ObtainSrcRect(texture, _srcRect);

			srcRectXRatio = srcRect.Width / dstRect.Width;
			srcRectYRatio = srcRect.Height / dstRect.Height;

			if (div < minDiv) div = minDiv;
			segmentCount = div;
			angleStep = Math::Max(0.0f, endAngle - beginAngle) / segmentCount;
		}
	};

	struct RoundedRectParameters
	{
		static const int32 CornerCount = 4;

		float minDstEdge;

		RectangleF srcRect;

		float srcRectXRatio;
		float srcRectYRatio;

		int32 segmentCount;
		float angleStep;

		float cornerBaseAngles[CornerCount];
		PointF cornerBases[CornerCount];

		RoundedRectParameters(Texture* texture, const RectangleF& dstRect, const RectangleF* _srcRect,
							  float cornerRadius, int32 div, int32 minDiv = 1)
		{
			minDstEdge = Math::Min(dstRect.Width, dstRect.Height);
			if (cornerRadius > minDstEdge) cornerRadius = minDstEdge;

			srcRect = ObtainSrcRect(texture, _srcRect);

			srcRectXRatio = srcRect.Width / dstRect.Width;
			srcRectYRatio = srcRect.Height / dstRect.Height;

			if (div < minDiv) div = minDiv;
			segmentCount = div;
			angleStep = Math::PI * 0.5f / segmentCount;

			const PointF _cornerBases[CornerCount] =
			{
				{ cornerRadius, cornerRadius },
				{ dstRect.Width - cornerRadius, cornerRadius },
				{ dstRect.Width - cornerRadius, dstRect.Height - cornerRadius },
				{ cornerRadius, dstRect.Height - cornerRadius }
			};
			FillArray(cornerBases, _cornerBases);

			const float _cornerBaseAngles[CornerCount] =
			{
				Math::PI * 0.5f,
				0,
				-Math::PI * 0.5f,
				-Math::PI
			};
			FillArray(cornerBaseAngles, _cornerBaseAngles);

		}
	};

}

namespace Apoc3D
{
	namespace Graphics
	{
		namespace RenderSystem
		{

			//////////////////////////////////////////////////////////////////////////

			Sprite::Sprite(RenderDevice* rd)
				: m_renderDevice(rd)
				, m_transform(Matrix::Identity)
				, m_stack(10)
				, m_currentSettings(SPRMix_ManageStateAlphaBlended)
				, m_flushThreshold(128)
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
				m_batchCount = 0;

				m_drawEntries.Clear();
			}

			void Sprite::End()
			{
				Flush();

				assert(m_began);
				m_began = false;
			}

			void Sprite::Flush()
			{
				if ((m_currentSettings & SPR_RecordBatch) == 0)
				{
					if (m_drawEntries.getCount() > 0)
						Submit(m_drawEntries);

					m_drawEntries.Clear();
				}
			}

			//////////////////////////////////////////////////////////////////////////

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

			void Sprite::Draw(Texture* texture, const PointF& pos, uint color)
			{
				assert(m_began);

				DrawEntry drawE;
				drawE.FillNormalDraw(texture, getTransform(), pos.X, pos.Y, color);
				EnqueueDrawEntry(drawE);
			}
			
			//////////////////////////////////////////////////////////////////////////

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
			void Sprite::Draw(Texture* texture, const RectangleF& dstRect, const RectangleF* srcRect, uint color)
			{
				assert(m_began);

				DrawEntry drawE;
				drawE.FillNormalDraw(texture, getTransform(), dstRect, srcRect, color);
				EnqueueDrawEntry(drawE);
			}
			
			//////////////////////////////////////////////////////////////////////////

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
			void Sprite::Draw(Texture* texture, const RectangleF& dstRect, const RectangleF* srcRect,
				uint tlColor, uint trColor, uint blColor, uint brColor)
			{
				assert(m_began);

				DrawEntry drawE;
				drawE.FillNormalDraw(texture, getTransform(), dstRect, srcRect, CV_White);

				drawE.SetColors(tlColor, trColor, blColor, brColor);
				EnqueueDrawEntry(drawE);
			}

			//////////////////////////////////////////////////////////////////////////

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
			void Sprite::Draw(Texture* texture, const PointF_A4& corners, const RectangleF* srcRect, const ColorValue_A4& colors)
			{
				assert(m_began);

				DrawEntry drawE;
				drawE.SetTexture(texture);
				drawE.SetPositions(getTransform(), corners[0], corners[1], corners[2], corners[3]);
				drawE.SetSrcRect(texture, srcRect);
				drawE.SetColors(colors[0], colors[1], colors[2], colors[3]);
				EnqueueDrawEntry(drawE);
			}
			void Sprite::Draw(Texture* texture, const PointF_A4& corners, const PointF_A4& texCoords, const ColorValue_A4& colors)
			{
				assert(m_began);

				DrawEntry drawE;
				drawE.SetTexture(texture);
				drawE.SetPositions(getTransform(), corners[0], corners[1], corners[2], corners[3]);
				drawE.SetTextureCoords(texCoords[0], texCoords[1], texCoords[2], texCoords[3]);
				drawE.SetColors(colors[0], colors[1], colors[2], colors[3]);
				EnqueueDrawEntry(drawE);
			}

			//////////////////////////////////////////////////////////////////////////

			void Sprite::DrawTiled(Texture* texture, const PointF& pos, const PointF& uvScale, const PointF& uvShift, uint color)
			{
				assert(m_began);

				DrawEntry drawE;
				drawE.FillNormalDraw(texture, getTransform(), pos.X, pos.Y, color);

				drawE.ChangeUV(uvScale, uvShift);

				EnqueueDrawEntry(drawE);
			}

			void Sprite::DrawTiled(Texture* texture, const RectangleF& dstRect, const PointF& uvScale, const PointF& uvShift, uint color)
			{
				assert(m_began);

				DrawEntry drawE;
				drawE.FillNormalDraw(texture, getTransform(), dstRect, NULL, color);

				drawE.ChangeUV(uvScale, uvShift);

				EnqueueDrawEntry(drawE);
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
			void Sprite::DrawCircle(Texture* texture, const RectangleF& dstRect, const RectangleF* srcRect,
				float beginAngle, float endAngle, int32 div, uint color)
			{
				assert(m_began);

				DrawCircleGeneric(texture, dstRect, srcRect, 0, beginAngle, endAngle, div, color);
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
			void Sprite::DrawCircleArc(Texture* texture, const RectangleF& dstRect, const RectangleF* srcRect, float lineWidth,
				float beginAngle, float endAngle, int32 div, uint color)
			{
				assert(m_began);

				if (lineWidth <= EPSILON) return;

				DrawCircleGeneric(texture, dstRect, srcRect, lineWidth, beginAngle, endAngle, div, color);
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

			void Sprite::DrawRoundedRect(Texture* texture, const RectangleF& dstRect, const RectangleF* _srcRect, float cornerRadius, int32 div, uint color)
			{
				assert(m_began);

				DrawRoundedRectGeneric(texture, dstRect, _srcRect, 0, cornerRadius, div, color);
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

			void Sprite::DrawRoundedRectBorder(Texture* texture, const RectangleF& dstRect, const RectangleF* _srcRect, float width, float cornerRadius, int32 div, uint color)
			{
				assert(m_began);
				if (width <= EPSILON) return;

				DrawRoundedRectGeneric(texture, dstRect, _srcRect, width, cornerRadius, div, color);
			}

			void Sprite::DrawLine(Texture* texture, const Point& start, const Point& end, uint color, 
				float width, LineCapOptions caps, const PointF& uvScale, const PointF& uvShift)
			{
				DrawLine(texture, PointF(start), PointF(end), color, width, caps, uvScale, uvShift);
			}

			void Sprite::DrawLine(Texture* texture, const PointF& _start, const PointF& _end, uint color,
				float width, LineCapOptions caps, const PointF& uvScale, const PointF& uvShift)
			{
				assert(m_began);

				PointF start = _start;
				PointF end = _end;

				Vector2 d = end - start;
				float length = d.Length();
				d.NormalizeInPlace();

				Vector2 normal = { -d.Y, d.X };

				if (caps == LineCapOptions::Square)
				{
					float r = width * 0.5f;
					start -= d * r;
					end += d * r;
				}

				if (width == 0)
				{
					width = static_cast<float>(texture->getHeight());
				}

				float scaleOnLengthDir = length / texture->getWidth();

				PointF sideExt = normal * (width * 0.5f);

				PointF corners[4] =
				{
					start - sideExt, end - sideExt,
					start + sideExt, end + sideExt
				};
				PointF texCoords[4] =
				{
					{ 0, 0 }, { scaleOnLengthDir, 0 },
					{ 0, 1 }, { scaleOnLengthDir, 1 },
				};

				if (caps == LineCapOptions::Round)
				{
					float xTexCoordPad = width * 0.5f / texture->getWidth();
					texCoords[0].X += xTexCoordPad;
					texCoords[1].X -= xTexCoordPad;
					texCoords[2].X += xTexCoordPad;
					texCoords[3].X -= xTexCoordPad;
				}

				DrawEntry drawE;
				drawE.SetTexture(texture);
				drawE.SetPositions(getTransform(), corners[0], corners[1], corners[2], corners[3]);
				drawE.SetTextureCoords(texCoords[0], texCoords[1], texCoords[2], texCoords[3]);
				drawE.ChangeUV(uvScale, uvShift);
				drawE.SetColors(color);
				EnqueueDrawEntry(drawE);

				if (caps == LineCapOptions::Round)
				{
					PointF offset = uvShift;
					offset.X *= texture->getWidth();
					offset.Y *= texture->getHeight();

					float r = width * 0.5f;

					float angle = atan2(normal.Y, normal.X);

					RectangleF leftRoundDst = { start - PointF(r, r), { width, width } };
					RectangleF leftRoundSrc = { 0, 0, width, (float)texture->getHeight() };
					leftRoundSrc.Offset(offset);
					leftRoundSrc.Width *= uvScale.X; leftRoundSrc.Height *= uvScale.Y;
					DrawCircle(texture, leftRoundDst, &leftRoundSrc, angle, angle + Math::PI, 2, color);

					RectangleF rightRoundDst = { end - PointF(r, r), { width, width } };
					RectangleF rightRoundSrc = { length - width, 0, width, (float)texture->getHeight() };
					rightRoundSrc.Offset(offset);
					rightRoundSrc.Width *= uvScale.X; rightRoundSrc.Height *= uvScale.Y;
					DrawCircle(texture, rightRoundDst, &rightRoundSrc, angle - Math::PI, angle, 2, color);
				}
			}

			//////////////////////////////////////////////////////////////////////////

			void Sprite::DrawCircleGeneric(Texture* texture, const RectangleF& dstRect, const RectangleF* _srcRect, float lineWidth, float beginAngle, float endAngle, int32 div, uint color, bool uvExt)
			{
				assert(m_began);

				bool isArc = lineWidth != 0;

				const CircleParameters cp(texture, dstRect, _srcRect, beginAngle, endAngle,
					div * (isArc ? 1 : 2), (isArc ? 2 : 4));

				if (isArc)
				{
					float minR = Math::Min(cp.xRadius, cp.yRadius);
					if (lineWidth > minR) lineWidth = minR;
				}

				const PointF circleBaseDstPos = dstRect.getSize() * 0.5f;

				for (int32 i = 0; i < cp.segmentCount; i++)
				{
					float startAngle = cp.angleStep * i + beginAngle;
					float endAngle = cp.angleStep * (i + 1) + beginAngle;

					PointF startDir = { cosf(startAngle), -sinf(startAngle) };
					PointF endDir = { cosf(endAngle), -sinf(endAngle) };

					DrawEntry de;

					if (!isArc)
					{
						float centerAngle = (startAngle + endAngle) * 0.5f;

						PointF leftDir = startDir * PointF(cp.xRadius, cp.yRadius);
						PointF rightDir = endDir * PointF(cp.xRadius, cp.yRadius);

						PointF centerDir = { cosf(centerAngle) * cp.xRadius, -sinf(centerAngle) * cp.yRadius };

						PointF leftDstPos = circleBaseDstPos + leftDir;
						PointF rightDstPos = circleBaseDstPos + rightDir;
						PointF centerDstPos = circleBaseDstPos + centerDir;
						PointF baseDstPos = circleBaseDstPos;

						PointF leftSrcPos = leftDstPos;
						PointF rightSrcPos = rightDstPos;
						PointF centerSrcPos = centerDstPos;

						PointF * srcPosList[] = { &leftSrcPos, &rightSrcPos, &centerSrcPos };
						for (PointF* sp : srcPosList)
						{
							sp->X *= cp.srcRectXRatio;
							sp->Y *= cp.srcRectYRatio;
							*sp += cp.srcRect.getPosition();
						}

						PointF* dstPosList[] = { &leftDstPos, &rightDstPos, &centerDstPos, &baseDstPos };
						for (PointF* dp : dstPosList)
						{
							*dp += dstRect.getPosition();
						}

						const PointF circleBaseSrcPos = cp.srcRect.getCenter();

						de.FillNormalDraw(texture, getTransform(), leftDstPos, centerDstPos, baseDstPos, rightDstPos,
							leftSrcPos, centerSrcPos, circleBaseSrcPos, rightSrcPos, color);
					}
					else
					{
						PointF leftInnerD = startDir * PointF(cp.xRadius - lineWidth, cp.yRadius - lineWidth);
						PointF rightInnerD = endDir * PointF(cp.xRadius - lineWidth, cp.yRadius - lineWidth);
						PointF leftOutterD = startDir * PointF(cp.xRadius, cp.yRadius);
						PointF rightOutterD = endDir * PointF(cp.xRadius, cp.yRadius);

						PointF leftDstInnerPos = circleBaseDstPos + leftInnerD;
						PointF rightDstInnerPos = circleBaseDstPos + rightInnerD;
						PointF leftDstOutterPos = circleBaseDstPos + leftOutterD;
						PointF rightDstOutterPos = circleBaseDstPos + rightOutterD;

						PointF leftSrcInnerPos = leftDstInnerPos;
						PointF rightSrcInnerPos = rightDstInnerPos;
						PointF leftSrcOutterPos = leftDstOutterPos;
						PointF rightSrcOutterPos = rightDstOutterPos;

						PointF * srcPosList[] = { &leftSrcInnerPos, &rightSrcInnerPos, &leftSrcOutterPos, &rightSrcOutterPos };
						for (PointF* sp : srcPosList)
						{
							sp->X *= cp.srcRectXRatio;
							sp->Y *= cp.srcRectYRatio;
							*sp += cp.srcRect.getPosition();
						}

						PointF* dstPosList[] = { &leftDstInnerPos, &rightDstInnerPos, &leftDstOutterPos, &rightDstOutterPos };
						for (PointF* dp : dstPosList)
						{
							*dp += dstRect.getPosition();
						}

						de.FillNormalDraw(texture, getTransform(), leftDstOutterPos, rightDstOutterPos, leftDstInnerPos, rightDstInnerPos,
							leftSrcOutterPos, rightSrcOutterPos, leftSrcInnerPos, rightSrcInnerPos, color);
					}
					de.IsUVExtended = uvExt;
					EnqueueDrawEntry(de);
				}
			}

			void Sprite::DrawRoundedRectGeneric(Texture* texture, const RectangleF& dstRect, const RectangleF* _srcRect, float width, float cornerRadius, int32 div, uint color)
			{
				assert(m_began);

				bool isArc = width != 0;

				RoundedRectParameters rp(texture, dstRect, _srcRect, cornerRadius, div);

				if (isArc)
				{
					float minR = cornerRadius;
					if (width > minR) width = minR;
				}

				if (!isArc)
				{
					// draw rect body

					// draw center vertical
					RectangleF centerDstRect = dstRect;
					centerDstRect.Inflate(-cornerRadius, 0);

					RectangleF centerSrcRect = rp.srcRect;
					centerSrcRect.Inflate(-cornerRadius * rp.srcRectXRatio, 0);

					Draw(texture, centerDstRect, &centerSrcRect, color);

					// draw left & right cap

					RectangleF leftDstRect = centerDstRect;
					leftDstRect.Inflate(0, -cornerRadius);
					leftDstRect.X = dstRect.X;
					leftDstRect.Width = cornerRadius;

					RectangleF leftSrcRect = centerSrcRect;
					leftSrcRect.Inflate(0, -cornerRadius * rp.srcRectYRatio);
					leftSrcRect.X = rp.srcRect.X;
					leftSrcRect.Width = cornerRadius * rp.srcRectXRatio;

					Draw(texture, leftDstRect, &leftSrcRect, color);

					RectangleF rightDstRect = leftDstRect;
					rightDstRect.X += centerDstRect.Width + leftDstRect.Width;

					RectangleF rightSrcRect = leftSrcRect;
					rightSrcRect.X += centerSrcRect.Width + leftSrcRect.Width;

					Draw(texture, rightDstRect, &rightSrcRect, color);
				}
				else
				{
					// draw direct borders

					RectangleF partDstRect = dstRect;
					partDstRect.Inflate(-cornerRadius, 0);
					partDstRect.Height = width;

					RectangleF partSrcRect = rp.srcRect;
					partSrcRect.Inflate(-cornerRadius * rp.srcRectXRatio, 0);
					partSrcRect.Height = width * rp.srcRectYRatio;

					Draw(texture, partDstRect, &partSrcRect, color);

					partDstRect.Y += dstRect.Height - partDstRect.Height;
					partSrcRect.Y += rp.srcRect.Height - partSrcRect.Height;

					Draw(texture, partDstRect, &partSrcRect, color);

					partDstRect = dstRect;
					partDstRect.Inflate(0, -cornerRadius);
					partDstRect.Width = width;

					partSrcRect = rp.srcRect;
					partSrcRect.Inflate(0, -cornerRadius * rp.srcRectYRatio);
					partSrcRect.Width = width * rp.srcRectXRatio;

					Draw(texture, partDstRect, &partSrcRect, color);

					partDstRect.X += dstRect.Width - partDstRect.Width;
					partSrcRect.X += rp.srcRect.Width - partSrcRect.Width;

					Draw(texture, partDstRect, &partSrcRect, color);

				}

				for (int32 j = 0; j < RoundedRectParameters::CornerCount; j++)
				{
					float baseAngle = rp.cornerBaseAngles[j];

					PointF cornerBaseDstPos = rp.cornerBases[j];
					PointF cornerBaseSrcPos = rp.cornerBases[j];
					cornerBaseSrcPos.X *= rp.srcRectXRatio;
					cornerBaseSrcPos.Y *= rp.srcRectYRatio;
					cornerBaseSrcPos += rp.srcRect.getPosition();

					for (int32 i = 0; i < rp.segmentCount; i++)
					{
						float startAngle = rp.angleStep * i + baseAngle;
						float endAngle = rp.angleStep * (i + 1) + baseAngle;

						PointF startDir = { cosf(startAngle), -sinf(startAngle) };
						PointF endDir = { cosf(endAngle), -sinf(endAngle) };

						DrawEntry de;

						if (!isArc)
						{
							float centerAngle = (startAngle + endAngle)*0.5f;
							PointF centerDir = PointF(cosf(centerAngle), -sinf(centerAngle)) * cornerRadius;

							PointF leftDir = startDir * cornerRadius;
							PointF rightDir = endDir * cornerRadius;

							PointF leftDstPos = cornerBaseDstPos + leftDir;
							PointF rightDstPos = cornerBaseDstPos + rightDir;
							PointF centerDstPos = cornerBaseDstPos + centerDir;
							PointF baseDstPos = cornerBaseDstPos;

							PointF leftSrcPos = leftDstPos;
							PointF rightSrcPos = rightDstPos;
							PointF centerSrcPos = centerDstPos;

							PointF* srcPosList[] = { &leftSrcPos, &rightSrcPos, &centerSrcPos };
							for (PointF* sp : srcPosList)
							{
								sp->X *= rp.srcRectXRatio;
								sp->Y *= rp.srcRectYRatio;
								*sp += rp.srcRect.getPosition();
							}

							PointF* dstPosList[] = { &leftDstPos, &rightDstPos, &centerDstPos, &baseDstPos };
							for (PointF* dp : dstPosList)
							{
								*dp += dstRect.getPosition();
							}

							de.FillNormalDraw(texture, getTransform(), leftDstPos, centerDstPos, baseDstPos, rightDstPos,
								leftSrcPos, centerSrcPos, cornerBaseSrcPos, rightSrcPos, color);
						}
						else
						{
							PointF leftInnerD = startDir * (cornerRadius - width);
							PointF rightInnerD = endDir * (cornerRadius - width);
							PointF leftOutterD = startDir * cornerRadius;
							PointF rightOutterD = endDir * cornerRadius;

							PointF leftDstInnerPos = cornerBaseDstPos + leftInnerD;
							PointF rightDstInnerPos = cornerBaseDstPos + rightInnerD;
							PointF leftDstOutterPos = cornerBaseDstPos + leftOutterD;
							PointF rightDstOutterPos = cornerBaseDstPos + rightOutterD;

							PointF leftSrcInnerPos = leftDstInnerPos;
							PointF rightSrcInnerPos = rightDstInnerPos;
							PointF leftSrcOutterPos = leftDstOutterPos;
							PointF rightSrcOutterPos = rightDstOutterPos;

							PointF* srcPosList[] = { &leftSrcInnerPos, &rightSrcInnerPos, &leftSrcOutterPos, &rightSrcOutterPos };
							for (PointF* sp : srcPosList)
							{
								sp->X *= rp.srcRectXRatio;
								sp->Y *= rp.srcRectYRatio;
								*sp += rp.srcRect.getPosition();
							}

							PointF* dstPosList[] = { &leftDstInnerPos, &rightDstInnerPos, &leftDstOutterPos, &rightDstOutterPos };
							for (PointF* dp : dstPosList)
							{
								*dp += dstRect.getPosition();
							}

							de.FillNormalDraw(texture, getTransform(), leftDstOutterPos, rightDstOutterPos, leftDstInnerPos, rightDstInnerPos,
								leftSrcOutterPos, rightSrcOutterPos, leftSrcInnerPos, rightSrcInnerPos, color);
						}

						EnqueueDrawEntry(de);
					}
				}

			}

			//////////////////////////////////////////////////////////////////////////

			void Sprite::DrawBatch(const SpriteDrawEntries& batch)
			{
				if (batch.getCount() > 0)
				{
					Flush();

					Submit(batch);
				}
			}

			//////////////////////////////////////////////////////////////////////////

			const Matrix& Sprite::getTransform() const
			{
				if (m_currentSettings & SPR_UseTransformStack)
				{
					if (m_stack.getCount())
						return m_stack.Peek();
					return Matrix::Identity;
				}
				return m_transform;
			}

			void Sprite::PopTransform()
			{
				if (m_currentSettings & SPR_UseTransformStack)
				{
					m_stack.PopMatrix();
				}
				else
				{
					AP_EXCEPTION(ErrorID::InvalidOperation, L"The sprite is not begun with SPR_UseTransformStack.");
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
				if (m_currentSettings & SPR_UseTransformStack)
				{
					m_stack.PushMatrix(matrix);
				}
				else
				{
					m_transform = matrix;
				}
			}

			void Sprite::EnqueueDrawEntry(const DrawEntry& drawE)
			{
				m_drawEntries.Add(drawE);

				if (m_drawEntries.getCount() > m_flushThreshold)
				{
					Flush();
				}
			}

			//////////////////////////////////////////////////////////////////////////

			void SpriteDrawEntries::DrawEntry::FillNormalDraw(Texture* texture, const Matrix& baseTrans,
				const PointF& tl_dp, const PointF& tr_dp, const PointF& bl_dp, const PointF& br_dp,
				const PointF& tl_sp, const PointF& tr_sp, const PointF& bl_sp, const PointF& br_sp, uint color)
			{
				SetTexture(texture);

				SetPositions(baseTrans, tl_dp, tr_dp, bl_dp, br_dp);
				SetColors(color);

				float invWidth = 1.0f / texture->getWidth();
				float invHeight = 1.0f / texture->getHeight();

				TL.TexCoord[0] = tl_sp.X * invWidth; TL.TexCoord[1] = tl_sp.Y * invHeight;
				TR.TexCoord[0] = tr_sp.X * invWidth; TR.TexCoord[1] = tr_sp.Y * invHeight;
				BL.TexCoord[0] = bl_sp.X * invWidth; BL.TexCoord[1] = bl_sp.Y * invHeight;
				BR.TexCoord[0] = br_sp.X * invWidth; BR.TexCoord[1] = br_sp.Y * invHeight;
			}

			void SpriteDrawEntries::DrawEntry::FillNormalDraw(Texture* texture, const Matrix& baseTrans, float x, float y, uint color)
			{
				SetTexture(texture);

				float width = (float)texture->getWidth();
				float height = (float)texture->getHeight();

				SetPositions(baseTrans, { x, y }, { width + x, y }, { x, height + y }, { width + x, height + y });

				SetColors(color);
				SetSrcRect(nullptr, nullptr);
			}

			// Auto resizing to fit the target rectangle is implemented in this method.
			void SpriteDrawEntries::DrawEntry::FillNormalDraw(Texture * texture, const Matrix & baseTrans, const RectangleF & dstRect, const RectangleF * srcRect, uint color)
			{
				SetTexture(texture);

				float width = (float)dstRect.Width;
				float height = (float)dstRect.Height;

				float x = dstRect.X;
				float y = dstRect.Y;

				SetPositions(baseTrans, { x, y }, { x + width, y }, { x, y + height }, { x + width, y + height });

				SetColors(color);
				SetSrcRect(texture, srcRect);
			}

			void SpriteDrawEntries::DrawEntry::SetTexture(Texture * tex)
			{
				Tex = tex;
			}
			void SpriteDrawEntries::DrawEntry::SetPositions(const Matrix & baseTrans, const PointF & tl_dp, const PointF & tr_dp, const PointF & bl_dp, const PointF & br_dp)
			{
				const Matrix& trans = baseTrans;

				Vector3 tl = { tl_dp.X, tl_dp.Y, 0 };
				Vector3 tr = { tr_dp.X, tr_dp.Y, 0 };
				Vector3 bl = { bl_dp.X, bl_dp.Y, 0 };
				Vector3 br = { br_dp.X, br_dp.Y, 0 };

				tl = Vector3::TransformCoordinate(tl, trans);
				tr = Vector3::TransformCoordinate(tr, trans);
				bl = Vector3::TransformCoordinate(bl, trans);
				br = Vector3::TransformCoordinate(br, trans);


				TL.Position[0] = tl.X - 0.5f;
				TL.Position[1] = tl.Y - 0.5f;
				TL.Position[2] = tl.Z;
				TL.Position[3] = 1;

				TR.Position[0] = tr.X - 0.5f;
				TR.Position[1] = tr.Y - 0.5f;
				TR.Position[2] = tr.Z;
				TR.Position[3] = 1;

				BL.Position[0] = bl.X - 0.5f;
				BL.Position[1] = bl.Y - 0.5f;
				BL.Position[2] = bl.Z;
				BL.Position[3] = 1;

				BR.Position[0] = br.X - 0.5f;
				BR.Position[1] = br.Y - 0.5f;
				BR.Position[2] = br.Z;
				BR.Position[3] = 1;
			}

			void SpriteDrawEntries::DrawEntry::SetSrcRect(Texture * texture, const RectangleF * srcRect)
			{
				if (srcRect)
				{
					float invWidth = 1.0f / (float)texture->getWidth();
					float invHeight = 1.0f / (float)texture->getHeight();

					TL.TexCoord[0] = srcRect->getLeft() * invWidth; TL.TexCoord[1] = srcRect->getTop() * invHeight;
					TR.TexCoord[0] = srcRect->getRight() * invWidth; TR.TexCoord[1] = srcRect->getTop() * invHeight;
					BL.TexCoord[0] = srcRect->getLeft() * invWidth; BL.TexCoord[1] = srcRect->getBottom() * invHeight;
					BR.TexCoord[0] = srcRect->getRight() * invWidth; BR.TexCoord[1] = srcRect->getBottom() * invHeight;
				}
				else
				{
					TL.TexCoord[0] = 0; TL.TexCoord[1] = 0;
					TR.TexCoord[0] = 1; TR.TexCoord[1] = 0;
					BL.TexCoord[0] = 0; BL.TexCoord[1] = 1;
					BR.TexCoord[0] = 1; BR.TexCoord[1] = 1;
				}
			}

			void SpriteDrawEntries::DrawEntry::SetColors(uint color)
			{
				TL.Diffuse = TR.Diffuse = BL.Diffuse = BR.Diffuse = color;
			}

			void SpriteDrawEntries::DrawEntry::SetColors(uint tlColor, uint trColor, uint blColor, uint brColor)
			{
				TL.Diffuse = tlColor;
				TR.Diffuse = trColor;
				BL.Diffuse = blColor;
				BR.Diffuse = brColor;
			}

			void SpriteDrawEntries::DrawEntry::SetTextureCoords(const PointF & tl_sp, const PointF & tr_sp, const PointF & bl_sp, const PointF & br_sp)
			{
				TL.TexCoord[0] = tl_sp.X; TL.TexCoord[1] = tl_sp.Y;
				TR.TexCoord[0] = tr_sp.X; TR.TexCoord[1] = tr_sp.Y;
				BL.TexCoord[0] = bl_sp.X; BL.TexCoord[1] = bl_sp.Y;
				BR.TexCoord[0] = br_sp.X; BR.TexCoord[1] = br_sp.Y;
			}

			void SpriteDrawEntries::DrawEntry::ChangeUV(float uScale, float vScale, float uBias, float vBias)
			{
				BL.TexCoord[0] *= uScale;
				BR.TexCoord[0] *= uScale;
				TL.TexCoord[0] *= uScale;
				TR.TexCoord[0] *= uScale;

				BL.TexCoord[1] *= vScale;
				BR.TexCoord[1] *= vScale;
				TL.TexCoord[1] *= vScale;
				TR.TexCoord[1] *= vScale;

				BL.TexCoord[0] += uBias;
				BR.TexCoord[0] += uBias;
				TL.TexCoord[0] += uBias;
				TR.TexCoord[0] += uBias;

				BL.TexCoord[1] += vBias;
				BR.TexCoord[1] += vBias;
				TL.TexCoord[1] += vBias;
				TR.TexCoord[1] += vBias;

				IsUVExtended = true;
			}
			void SpriteDrawEntries::DrawEntry::ChangeUV(const PointF & uvScale, const PointF & uvShift)
			{
				ChangeUV(uvScale.X, uvScale.Y, uvShift.X, uvShift.Y);
			}

			//////////////////////////////////////////////////////////////////////////

			SpriteTransformScope::SpriteTransformScope(Sprite* spr, const Matrix& transform, bool mult)
				: m_sprite(spr)
			{
				if (!m_sprite->isUsingStack())
				{
					m_oldTransform = m_sprite->getTransform();
				}

				if (mult)
					m_sprite->MultiplyTransform(transform);
				else 
					m_sprite->SetTransform(transform);
			}

			SpriteTransformScope::~SpriteTransformScope()
			{
				if (m_sprite->isUsingStack())
					m_sprite->PopTransform();
				else
					m_sprite->SetTransform(m_oldTransform);
			}

			//////////////////////////////////////////////////////////////////////////

			SpriteBeginEndScope::SpriteBeginEndScope(Sprite* spr, Sprite::SpriteSettings settings)
				: m_sprite(spr)
				, m_oldBegan(spr->m_began)
				, m_oldSettings(Sprite::SPRMix_ManageStateAlphaBlended)
			{
				if (spr->m_began)
				{
					m_oldSettings = m_sprite->getSettings();
					m_sprite->End();
				}
				m_sprite->Begin(settings);
			}
			SpriteBeginEndScope::~SpriteBeginEndScope()
			{
				m_sprite->End();
				if (m_oldBegan)
				{
					m_sprite->Begin(m_oldSettings);
				}
			}

			//////////////////////////////////////////////////////////////////////////

			SpriteShadedScope::SpriteShadedScope(Sprite* spr, Sprite::SpriteSettings baseSettings, const String& effectName, Material* mtrl)
				: SpriteBeginEndScope(spr, (Sprite::SpriteSettings)(baseSettings | Sprite::SPR_AllowShading))
			{
				m_effect = up_cast<EffectSystem::AutomaticEffect*>(EffectSystem::EffectManager::getSingleton().getEffect(effectName));
				assert(m_effect);
				m_effect->Begin();
				m_effect->BeginPass(0);

				m_effect->Setup(mtrl, nullptr, 0);
			}
			SpriteShadedScope::~SpriteShadedScope()
			{
				m_sprite->Flush();

				m_effect->EndPass();
				m_effect->End();
			}

		}
	}
}