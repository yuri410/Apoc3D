
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

#include "UICommon.h"

#include "Apoc3D/Graphics/RenderSystem/Sprite.h"
#include "apoc3d/Graphics/RenderSystem/Texture.h"
#include "apoc3d/Platform/API.h"
#include "FontManager.h"
#include "SystemUIImpl.h"


namespace Apoc3D
{
	namespace UI
	{
		/************************************************************************/
		/*  UIGraphic                                                           */
		/************************************************************************/

		UIGraphic::UIGraphic(Texture* tex) : UIGraphic(tex, CV_White) { }
		UIGraphic::UIGraphic(Texture* tex, const Apoc3D::Math::Rectangle& srcRect) : UIGraphic(tex, srcRect, CV_White) { }
		UIGraphic::UIGraphic(Texture* tex, const Apoc3D::Math::Rectangle(&srcRect)[3]) : UIGraphic(tex, srcRect, CV_White) { }
		UIGraphic::UIGraphic(Texture* tex, const Apoc3D::Math::Rectangle(&srcRect)[9]) : UIGraphic(tex, srcRect, CV_White) { }

		UIGraphic::UIGraphic(Texture* tex, ColorValue color)
			: Graphic(tex), ModColor(color) { }

		UIGraphic::UIGraphic(Texture* tex, const Apoc3D::Math::Rectangle& srcRect, ColorValue color)
			: Graphic(tex), ModColor(color) 
		{
			SourceRects.Add(srcRect);
		}

		UIGraphic::UIGraphic(Texture* tex, const Apoc3D::Math::Rectangle(&srcRect)[3], ColorValue color)
			: Graphic(tex), ModColor(color)
		{
			SourceRects.AddArray(srcRect);
		}

		UIGraphic::UIGraphic(Texture* tex, const Apoc3D::Math::Rectangle(&srcRect)[9], ColorValue color)
			: Graphic(tex), ModColor(color)
		{
			SourceRects.AddArray(srcRect);
		}

		void UIGraphic::Draw(Sprite* sprite, const Apoc3D::Math::Rectangle& dstRect, bool vertical) const
		{
			if (isSet() && CV_GetColorA(ModColor))
			{
				if (SourceRects.getCount())
				{
					if (is9patch())
					{
						guiDrawRegion9(sprite, dstRect, ModColor, Graphic, 
							reinterpret_cast<const Apoc3D::Math::Rectangle(&)[9]>(*SourceRects.getElements()));
					}
					else if (is3patch())
					{
						if (vertical)
						{
							guiDrawRegion3Vert(sprite, dstRect.getTopLeft(), dstRect.Height, ModColor, Graphic,
								reinterpret_cast<const Apoc3D::Math::Rectangle(&)[3]>(*SourceRects.getElements()));
						}
						else
						{
							guiDrawRegion3(sprite, dstRect.getTopLeft(), dstRect.Width, ModColor, Graphic,
								reinterpret_cast<const Apoc3D::Math::Rectangle(&)[3]>(*SourceRects.getElements()));
						}
					}
					else
					{
						sprite->Draw(Graphic, dstRect, SourceRects.getElements(), ModColor);
					}
				}
				else
				{
					sprite->Draw(Graphic, dstRect, nullptr, ModColor);
				}
			}
		}
		void UIGraphic::DrawVert(Sprite* sprite, const Apoc3D::Math::Rectangle& dstRect) const { Draw(sprite, dstRect, true); }
		void UIGraphic::Draw(Sprite* sprite, const Apoc3D::Math::Rectangle& dstRect) const { Draw(sprite, dstRect, false); }

		int32 UIGraphic::getWidth() const
		{
			assert(isSet() && (SourceRects.getCount() == 0 || SourceRects.getCount() == 1));
			return isSimple() ? SourceRects[0].Width : Graphic->getWidth();
		}
		int32 UIGraphic::getHeight() const
		{
			assert(isSet() && (SourceRects.getCount() == 0 || SourceRects.getCount() == 1));
			return isSimple() ? SourceRects[0].Height : Graphic->getHeight();
		}
		Point UIGraphic::getSize() const 
		{
			int32 w = getWidth();
			int32 h = getHeight();
			return Point(w, h);
		}

		/************************************************************************/
		/*  UIGraphicSimple                                                     */
		/************************************************************************/

		UIGraphicSimple::UIGraphicSimple(Texture* tex) : UIGraphicSimple(tex, CV_White) { }
		UIGraphicSimple::UIGraphicSimple(Texture* tex, const Apoc3D::Math::Rectangle& srcRect) : UIGraphicSimple(tex, srcRect, CV_White) { }

		UIGraphicSimple::UIGraphicSimple(Texture* tex, ColorValue modColor) 
			: Graphic(tex), ModColor(modColor) { }

		UIGraphicSimple::UIGraphicSimple(Texture* tex, const Apoc3D::Math::Rectangle& srcRect, ColorValue modColor) 
			: Graphic(tex), ModColor(modColor) 
		{
			SourceRects.Add(srcRect);
		}
		
		void UIGraphicSimple::Draw(Sprite* sprite, const Apoc3D::Math::Rectangle& dstRect) const
		{
			if (isSet() && CV_GetColorA(ModColor))
			{
				sprite->Draw(Graphic, dstRect, SourceRects.getCount() > 0 ? SourceRects.getElements() : nullptr, ModColor);
			}
		}

		void UIGraphicSimple::DrawCentered(Sprite* sprite, const Point& pos, const Point& parentSize) const
		{
			if (isSet() && CV_GetColorA(ModColor))
			{
				if (SourceRects.getCount())
				{
					const Apoc3D::Math::Rectangle& sr = SourceRects[0];
					int ix = (int)(parentSize.X - sr.Width) / 2 + pos.X;
					int iy = (int)(parentSize.Y - sr.Height) / 2 + pos.Y;
					Apoc3D::Math::Rectangle icoDR(ix, iy, sr.Width, sr.Height);
					sprite->Draw(Graphic, icoDR, &sr, ModColor);
				}
				else
				{
					int ix = (int)(parentSize.X - Graphic->getWidth()) / 2 + pos.X;
					int iy = (int)(parentSize.Y - Graphic->getHeight()) / 2 + pos.Y;
					Apoc3D::Math::Rectangle icoDR(ix, iy, Graphic->getWidth(), Graphic->getHeight());
					sprite->Draw(Graphic, icoDR, nullptr, ModColor);
				}
			}
		}
		void UIGraphicSimple::DrawCentered(Sprite* sprite, const Apoc3D::Math::Rectangle& dstRect) const
		{
			DrawCentered(sprite, dstRect.getTopLeft(), dstRect.getSize());
		}

		int32 UIGraphicSimple::getWidth() const
		{
			assert(isSet());
			return SourceRects.getCount() > 0 ? SourceRects[0].Width : Graphic->getWidth();
		}
		int32 UIGraphicSimple::getHeight() const
		{
			assert(isSet());
			return SourceRects.getCount() > 0 ? SourceRects[0].Height : Graphic->getHeight();
		}
		Point UIGraphicSimple::getSize() const
		{
			int32 w = getWidth();
			int32 h = getHeight();
			return Point(w, h);
		}


		/************************************************************************/
		/*  ControlBounds                                                       */
		/************************************************************************/

		ControlBounds::ControlBounds(const Apoc3D::Math::Rectangle& graphicalArea, const Apoc3D::Math::Rectangle& hotArea)
		{
			Left = hotArea.X - graphicalArea.X;
			Right = graphicalArea.getRight() - hotArea.getRight();

			Top = hotArea.Y - graphicalArea.Y;
			Bottom = graphicalArea.getBottom() - hotArea.getBottom();
		}

		Apoc3D::Math::Rectangle ControlBounds::InflateRect(const Apoc3D::Math::Rectangle& rect) const
		{
			Apoc3D::Math::Rectangle result = rect;
			result.X -= Left;
			result.Y -= Top;
			result.Width += getHorizontalSum();
			result.Height += getVerticalSum();
			return result;
		}
		Apoc3D::Math::Rectangle ControlBounds::ShrinkRect(const Apoc3D::Math::Rectangle& rect) const
		{
			Apoc3D::Math::Rectangle result = rect;
			result.X += Left;
			result.Y += Top;
			result.Width -= getHorizontalSum();
			result.Height -= getVerticalSum();
			return result;
		}

		int32 ControlBounds::operator[](SideIndex idx) const
		{
			switch (idx)
			{
			case Apoc3D::UI::ControlBounds::SI_Left:
				return Left;
			case Apoc3D::UI::ControlBounds::SI_Top:
				return Top;
			case Apoc3D::UI::ControlBounds::SI_Right:
				return Right;
			case Apoc3D::UI::ControlBounds::SI_Bottom:
				return Bottom;
			default:
				return 0;
			}
		}

		void ControlBounds::SetFromLeftTopRightBottom(int32 padding[4])
		{
			Left = padding[ControlBounds::SI_Left];
			Right = padding[ControlBounds::SI_Right];
			Top = padding[ControlBounds::SI_Top];
			Bottom = padding[ControlBounds::SI_Bottom];
		}
		void ControlBounds::SetZero()
		{
			Left = Right = Top = Bottom = 0;
		}


		/************************************************************************/
		/* TextRenderSettings                                                   */
		/************************************************************************/
		
		void TextRenderSettings::Draw(Sprite* sprite, Font* font, const String& text, const Apoc3D::Math::Rectangle& area, bool enabled) const
		{
			ColorValue textColor = enabled ? TextColor : TextColorDisabled;
			ColorValue textShadowColor = enabled ? TextShadowColor : TextShadowColorDisabled;

			if (CV_GetColorA(textColor) == 0 && (!HasTextShadow || CV_GetColorA(textShadowColor) == 0))
				return;

			Apoc3D::Math::Rectangle modArea = TextPadding.ShrinkRect(area);

			Point pos = modArea.getTopLeft();
			Point size = modArea.getSize();

			// x dir alignment
			Point textPos = pos + GetTextOffset(font, text, size);

			if (HasTextShadow)
			{
				Point shdTextPos = textPos + TextShadowOffset;
				font->DrawString(sprite, text, shdTextPos, textShadowColor);
			}

			font->DrawString(sprite, text, textPos, textColor);
		}

		void TextRenderSettings::Draw(Sprite* sprite, Font* font, const String& text, const Point& pos, const Point& size, bool enabled) const
		{
			Draw(sprite, font, text, Apoc3D::Math::Rectangle(pos, size), enabled);
		}

		void TextRenderSettings::DrawBG(Sprite* sprite, Font* font, const String& text, int32 selStart, int32 selEnd, const Apoc3D::Math::Rectangle& area, bool enabled) const
		{
			Apoc3D::Math::Rectangle modArea = TextPadding.ShrinkRect(area);

			Point pos = modArea.getTopLeft(); 
			Point size = modArea.getSize();

			Point textSize = font->MeasureString(text);
			Point textPos = pos + GetTextOffset(textSize, size);

			int32 preX = selStart > 0 ? font->MeasureString(text.substr(0, selStart)).X : 0;
			int32 postX = selEnd < (int32)text.size() ? font->MeasureString(text.substr(0, selEnd)).X : textSize.X;

			Point bgSize;
			bgSize.X = postX - preX;
			bgSize.Y = textSize.Y;

			textPos.X += preX;

			sprite->Draw(SystemUI::GetWhitePixel(), Apoc3D::Math::Rectangle(textPos, bgSize), enabled ? TextSelectionColor : TextSelectionColorDisabled);
		}
		void TextRenderSettings::DrawBG(Sprite* sprite, Font* font, const String& text, int32 selStart, int32 selEnd, const Point& pos, const Point& size, bool enabled) const
		{
			DrawBG(sprite, font, text, selStart, selEnd, Apoc3D::Math::Rectangle(pos, size), enabled);
		}

		Point TextRenderSettings::GetTextOffset(Font* font, const String& text, const Point& areaSize) const
		{
			Point textSize = font->MeasureString(text);
			return GetTextOffset(textSize, areaSize);
		}
		Point TextRenderSettings::GetTextOffset(const Point& textSize, const Point& areaSize) const
		{
			Point textOffset;
			if (HorizontalAlignment == TextHAlign::Left)
			{
				textOffset.X = 0;
			}
			else if (HorizontalAlignment == TextHAlign::Right)
			{
				textOffset.X = areaSize.X - textSize.X;
			}
			else
			{
				textOffset.X = (areaSize.X - textSize.X) / 2;
			}

			if (VerticalAlignment == TextVAlign::Top)
			{
				textOffset.Y = 0;
			}
			else if (VerticalAlignment == TextVAlign::Bottom)
			{
				textOffset.Y = areaSize.Y - textSize.Y;
			}
			else
			{
				textOffset.Y = (areaSize.Y - textSize.Y) / 2;
			}
			return textOffset;
		}

		/************************************************************************/
		/*  SystemUI                                                            */
		/************************************************************************/

		static SystemUIImpl sysUI;

		RectangleF& SystemUI::UIArea = sysUI.UIArea;
		RectangleF& SystemUI::MaximizedArea = sysUI.MaximizedArea;

		Form*& SystemUI::ActiveForm = sysUI.m_activeForm;
		MenuBar*& SystemUI::MainMenu = sysUI.m_mainMenu;
		Form*& SystemUI::TopMostForm = sysUI.m_topMostForm;
		Form*& SystemUI::ModalForm = sysUI.m_modalForm;

		float SystemUI::DoubleClickInterval = Platform::GetSystemDoubleClickIntervalInMS() / 1000.0f;

		void SystemUI::Initialize(RenderDevice* device) { sysUI.Initialize(device); }
		void SystemUI::Finalize() { sysUI.Finalize(); }

		void SystemUI::Add(ControlContainer* cc) { sysUI.Add(cc); }
		void SystemUI::Remove(ControlContainer* cc) { sysUI.Remove(cc); }
		void SystemUI::RemoveForm(const String& name) { sysUI.RemoveForm(name); }
		void SystemUI::RemoveContainer(const String& name) { sysUI.RemoveContainer(name); }
		void SystemUI::BringToFirst(ControlContainer* cc) { sysUI.BringToFirst(cc); }

		void SystemUI::Draw() { sysUI.Draw(); }
		void SystemUI::Update(const GameTime* time) { sysUI.Update(time); }

		Point SystemUI::ClampFormMovementOffset(Form* frm, const Point& vec) { return sysUI.ClampFormMovementOffset(frm, vec); }
		Texture* SystemUI::GetWhitePixel() { return sysUI.getWhitePixel(); }

		Apoc3D::Math::Rectangle SystemUI::GetUIArea(RenderDevice* device) { return sysUI.GetUIArea(device); }

		bool SystemUI::GetMinimizedPosition(RenderDevice* dev, Form* form, Point& pos) { return sysUI.GetMinimizedPosition(dev, form, pos); }
		Point SystemUI::GetMaximizedSize(RenderDevice* dev, Form* form) { return sysUI.GetMaximizedSize(dev, form); }
		Apoc3D::Math::Rectangle SystemUI::GetMaximizedRect(RenderDevice* dev, Form* form) { return sysUI.GetMaximizedRect(dev, form); }
		
		const List<Form*>& SystemUI::getForms() { return sysUI.getForms(); }

		/************************************************************************/
		/* GUIUtils                                                             */
		/************************************************************************/

		void GUIUtils::CalculateScrollBarPositions(const Apoc3D::Math::Rectangle& area, ScrollBarPositioning* vs, ScrollBarPositioning* hs)
		{
			Point size = area.getSize();
			if (vs)
			{
				size.X -= vs->Width;
			}
			
			if (hs)
			{
				size.Y -= hs->Width;
			}
			
			if (vs)
			{
				vs->Position = area.getTopRight();
				vs->Position.X -= vs->Width;
				vs->Length = size.Y;
			}

			if (hs)
			{
				hs->Position = area.getBottomLeft();
				hs->Position.Y -= hs->Width;
				hs->Length = size.X;
			}
		}

		/************************************************************************/
		/*    GUI sprite rendering                                              */
		/************************************************************************/

		// layout 3 one-dimensional segments with in the given fullLength
		// calculates the result segment lengths that fits
		void guiDirectional3SegmentLayout(int32 beginLength, int32 endLength, int32 fullLength, 
			int32& resultBeginLength, int32& resultMiddleLength, int32& resultEndLength, int32* resultStartShift, int32* resultEndShift)
		{
			const int32 MarginWidth = beginLength + endLength;

			resultMiddleLength = fullLength - MarginWidth;

			if (resultMiddleLength < 0)
			{
				int32 overflow = -resultMiddleLength;
				int32 halfOverflow1 = overflow/2;
				int32 halfOverflow2 = overflow - halfOverflow1;

				resultBeginLength = beginLength - halfOverflow1;
				resultEndLength = endLength - halfOverflow2;

				if (resultBeginLength<0) resultBeginLength = 0;
				if (resultEndLength<0) resultEndLength = 0;

				if (resultStartShift)
					*resultStartShift = halfOverflow1;
				if (resultEndShift)
					*resultEndShift = halfOverflow2;

				resultMiddleLength = 0;
			}
			else
			{
				if (resultStartShift)
					*resultStartShift = 0;
				if (resultEndShift)
					*resultEndShift = 0;

				resultBeginLength = beginLength;
				resultEndLength = endLength;
			}
		}

		void guiGenerateRegion9Rects(const Apoc3D::Math::Rectangle& dstRect, 
			Apoc3D::Math::Rectangle (&srcRects)[9], Apoc3D::Math::Rectangle (&destRects)[9])
		{
			const int32 SrcLeftWidth = srcRects[3].Width;
			const int32 SrcRightWidth = srcRects[5].Width;

			int32 leftWidth;
			int32 middleWidth;
			int32 rightWidth;
			int32 rightShift;

			guiDirectional3SegmentLayout(SrcLeftWidth, SrcRightWidth, dstRect.Width, leftWidth, middleWidth, rightWidth, nullptr, &rightShift);


			srcRects[0].Width = srcRects[3].Width = srcRects[6].Width = leftWidth;
			srcRects[2].Width = srcRects[5].Width = srcRects[8].Width = rightWidth;

			srcRects[2].X += rightShift;
			srcRects[5].X += rightShift;
			srcRects[8].X += rightShift;



			const int32 SrcTopHeight = srcRects[1].Height;
			const int32 SrcBottomHeight = srcRects[7].Height;

			int32 topHeight;
			int32 middleHeight;
			int32 bottomHeight;
			int32 bottomShift;
			guiDirectional3SegmentLayout(SrcTopHeight, SrcBottomHeight, dstRect.Height, topHeight, middleHeight, bottomHeight, nullptr, &bottomShift);

			srcRects[0].Height = srcRects[1].Height = srcRects[2].Height = topHeight;
			srcRects[6].Height = srcRects[7].Height = srcRects[8].Height = bottomHeight;

			srcRects[6].Y += bottomShift;
			srcRects[7].Y += bottomShift;
			srcRects[8].Y += bottomShift;



			// top-mid-bottom:
			//  left column
			destRects[0].Width = destRects[3].Width = destRects[6].Width = leftWidth;

			//  center column
			destRects[1].Width = destRects[4].Width = destRects[7].Width = middleWidth;

			//  right column
			destRects[2].Width = destRects[5].Width = destRects[8].Width = rightWidth;


			// left-center-right:
			//  top
			destRects[0].Height = destRects[1].Height = destRects[2].Height = topHeight;

			//  middle
			destRects[3].Height = destRects[4].Height = destRects[5].Height = middleHeight; 

			//  bottom
			destRects[6].Height = destRects[7].Height = destRects[8].Height = bottomHeight;

			// positioning
			destRects[0].X = destRects[3].X = destRects[6].X = dstRect.X;
			destRects[1].X = destRects[4].X = destRects[7].X = destRects[0].getRight();
			destRects[2].X = destRects[5].X = destRects[8].X = destRects[1].getRight();

			destRects[0].Y = destRects[1].Y = destRects[2].Y = dstRect.Y;
			destRects[3].Y = destRects[4].Y = destRects[5].Y = destRects[0].getBottom();
			destRects[6].Y = destRects[7].Y = destRects[8].Y = destRects[3].getBottom();
		}

		void guiGenerateRegion3Rects(const Point& pos, int32 width, 
			Apoc3D::Math::Rectangle (&srcRects)[3], Apoc3D::Math::Rectangle (&destRects)[3])
		{
			const int32 SrcLeftWidth = srcRects[0].Width;
			const int32 SrcRightWidth = srcRects[2].Width;

			int32 leftWidth;
			int32 middleWidth;
			int32 rightWidth;
			int32 rightShift;

			guiDirectional3SegmentLayout(SrcLeftWidth, SrcRightWidth, width, leftWidth, middleWidth, rightWidth, nullptr, &rightShift);

			srcRects[0].Width = leftWidth;
			srcRects[2].Width = rightWidth;
			srcRects[2].X += rightShift;

			destRects[0] = srcRects[0];
			destRects[1] = srcRects[1];
			destRects[2] = srcRects[2];

			destRects[0].Y = destRects[1].Y = destRects[2].Y = pos.Y;

			destRects[0].X = pos.X;
			destRects[1].X = destRects[0].getRight();
			destRects[1].Width = middleWidth;
			destRects[2].X = destRects[1].getRight();

		}
		
		void guiGenerateRegion3VertRects(const Point& pos, int32 height, 
			Apoc3D::Math::Rectangle (&srcRects)[3], Apoc3D::Math::Rectangle (&destRects)[3])
		{
			const int32 SrcTopHeight = srcRects[0].Height;
			const int32 SrcBottomHeight = srcRects[2].Height;

			int32 topHeight;
			int32 middleHeight;
			int32 bottomHeight;
			int32 bottomShift;

			guiDirectional3SegmentLayout(SrcTopHeight, SrcBottomHeight, height, topHeight, middleHeight, bottomHeight, nullptr, &bottomShift);

			srcRects[0].Height = topHeight;
			srcRects[2].Height = bottomHeight;
			srcRects[2].Y += bottomShift;

			destRects[0] = srcRects[0];
			destRects[1] = srcRects[1];
			destRects[2] = srcRects[2];

			destRects[0].X = destRects[1].X = destRects[2].X = pos.X;

			destRects[0].Y = pos.Y;
			destRects[1].Y = destRects[0].getBottom();
			destRects[1].Height = middleHeight;
			destRects[2].Y = destRects[1].getBottom();
		}

		void guiDrawRegion9(Sprite* sprite, const Apoc3D::Math::Rectangle& dstRect, ColorValue cv, 
			Texture* texture, const Apoc3D::Math::Rectangle (&srcRects)[9], Apoc3D::Math::Rectangle* centerRegion)
		{
			Apoc3D::Math::Rectangle srcRectCopy[9];
			Apoc3D::Math::Rectangle destRects[9];
			memcpy(srcRectCopy, srcRects, sizeof(srcRectCopy));
			guiGenerateRegion9Rects(dstRect, srcRectCopy, destRects);

			for (int i=0;i<9;i++)
			{
				if (destRects[i].Width > 0 && destRects[i].Height > 0)
					sprite->Draw(texture, destRects[i], &srcRectCopy[i], cv);
			}

			if (centerRegion)
				*centerRegion = destRects[4];
		}

		void guiDrawRegion3(Sprite* sprite, const Point& pos, int width, ColorValue cv, 
			Texture* texture, const Apoc3D::Math::Rectangle (&srcRects)[3], Apoc3D::Math::Rectangle* centerRegion)
		{
			Apoc3D::Math::Rectangle srcRectsCopy[3];
			Apoc3D::Math::Rectangle dstRects[3];
			memcpy(srcRectsCopy, srcRects, sizeof(srcRectsCopy));
			guiGenerateRegion3Rects(pos, width, srcRectsCopy, dstRects);

			for (int i=0;i<3;i++)
			{
				sprite->Draw(texture, dstRects[i], &srcRectsCopy[i], cv);
			}
			
			if (centerRegion)
				*centerRegion = dstRects[1];
		}

		void guiDrawRegion3Vert(Sprite* sprite, const Point& pos, int height, ColorValue cv, 
			Texture* texture, const Apoc3D::Math::Rectangle (&srcRects)[3], Apoc3D::Math::Rectangle* centerRegion)
		{
			Apoc3D::Math::Rectangle srcRectsCopy[3];
			Apoc3D::Math::Rectangle dstRects[3];
			memcpy(srcRectsCopy, srcRects, sizeof(srcRectsCopy));
			guiGenerateRegion3VertRects(pos, height, srcRectsCopy, dstRects);

			for (int i=0;i<3;i++)
			{
				sprite->Draw(texture, dstRects[i], &srcRectsCopy[i], cv);
			}

			if (centerRegion)
				*centerRegion = dstRects[1];
		}

		void guiDrawRegion3Clipped(Sprite* sprite, const Point& pos, int32 maxWidth, int32 capWidth,
			ColorValue cv, Texture* texture, const Apoc3D::Math::Rectangle (&srcRects)[3])
		{
			int32 cx = pos.X;

			for (int32 i = 0; i < countof(srcRects); i++)
			{
				Apoc3D::Math::Rectangle srcRect = srcRects[i];
				Apoc3D::Math::Rectangle dstRect = srcRect;

				dstRect.X = cx;
				dstRect.Y = pos.Y;

				int32 partWidth;

				if (i == 1)
				{
					partWidth = Math::Min(maxWidth - srcRects[0].Width - srcRects[2].Width, capWidth);
				}
				else
				{
					partWidth = Math::Min(srcRect.Width, capWidth);
					srcRect.Width = partWidth;
				}

				dstRect.Width = partWidth;
				sprite->Draw(texture, dstRect, &srcRect, cv);

				cx += partWidth;
				capWidth -= partWidth;

				if (capWidth <= 0)
					break;
			}
		}



		bool guiDrawRegion9Noclip(Sprite* sprite, const Apoc3D::Math::Rectangle& rect, ColorValue cv, 
			Texture* texture, const Apoc3D::Math::Rectangle (&srcRect)[9], Apoc3D::Math::Rectangle* centerRegion)
		{
			const int MarginWidth = srcRect[0].Width + srcRect[2].Width;
			const int MarginHeight = srcRect[0].Height + srcRect[6].Height;

			// size limit
			if (rect.Width >= MarginWidth && rect.Height >= MarginHeight)
			{
				Apoc3D::Math::Rectangle destRect[9];
				for (int i=0;i<9;i++)
				{
					destRect[i] = srcRect[i];

					destRect[i].X += rect.X - srcRect[0].X;
					destRect[i].Y += rect.Y - srcRect[0].Y;
				}

				int eWidth = rect.Width - MarginWidth;
				int eHeight = rect.Height - MarginHeight;

				destRect[1].Width = eWidth; // top
				destRect[4].Width = eWidth; // mid
				destRect[7].Width = eWidth; // bottom

				destRect[3].Height = eHeight; // left
				destRect[4].Height = eHeight; // mid
				destRect[5].Height = eHeight; // right

				destRect[2].X = destRect[5].X = destRect[8].X = destRect[1].getRight();
				destRect[6].Y = destRect[7].Y = destRect[8].Y = destRect[3].getBottom();

				for (int i=0;i<9;i++)
				{
					if (destRect[i].Width > 0 && destRect[i].Height > 0)
						sprite->Draw(texture, destRect[i], &srcRect[i], cv);
				}

				if (centerRegion)
					*centerRegion = destRect[4];
				return true;
			}
			return false;
		}

		bool guiDrawRegion3Noclip(Sprite* sprite, const Point& pt, int w, ColorValue cv, 
			Texture* texture, const Apoc3D::Math::Rectangle (&srcRect)[3], Apoc3D::Math::Rectangle* centerRegion)
		{
			int headAndTailWidth = srcRect[0].Width + srcRect[2].Width;

			if (w >= headAndTailWidth)
			{
				int x = pt.X;
				int y = pt.Y;

				Apoc3D::Math::Rectangle dstRects[3] = 
				{
					srcRect[0],
					srcRect[1],
					srcRect[2]
				};

				dstRects[0].Y = dstRects[1].Y = dstRects[2].Y = y;

				dstRects[0].X = x;
				//dstRects[1].X -= srcRect[0].X;
				dstRects[1].X = dstRects[0].getRight();
				dstRects[1].Width = w - headAndTailWidth;
				dstRects[2].X = dstRects[1].getRight();

				for (int i=0;i<3;i++)
				{
					sprite->Draw(texture, dstRects[i], &srcRect[i], cv);
				}

				if (centerRegion)
					*centerRegion = dstRects[1];
				return true;
			}
			return false;
		}

		bool guiDrawRegion3VertNoclip(Sprite* sprite, const Point& pt, int h, ColorValue cv, 
			Texture* texture, const Apoc3D::Math::Rectangle (&srcRect)[3], Apoc3D::Math::Rectangle* centerRegion)
		{
			int headAndTailHeight = srcRect[0].Height + srcRect[2].Height;

			if (h >= headAndTailHeight)
			{
				int x = pt.X;
				int y = pt.Y;

				Apoc3D::Math::Rectangle dstRects[3] = 
				{
					srcRect[0],
					srcRect[1],
					srcRect[2]
				};

				dstRects[0].X = dstRects[1].X = dstRects[2].X = x;

				dstRects[0].Y = y;
				//dstRects[1].Y -= srcRect[0].Y;
				dstRects[1].Y = dstRects[0].getBottom();
				dstRects[1].Height = h - headAndTailHeight;
				dstRects[2].Y = dstRects[1].getBottom();

				for (int i=0;i<3;i++)
				{
					sprite->Draw(texture, dstRects[i], &srcRect[i], cv);
				}

				if (centerRegion)
					*centerRegion = dstRects[1];
				return true;
			}
			return false;
		}

		void guiOmitLineText(Font* fnt, int32 widthlimit, String& line)
		{
			int chCount = fnt->FitSingleLineString(line, widthlimit);

			if (chCount < (int32)line.length() - 2)
			{
				String textToDraw = line.substr(0, chCount);
				textToDraw.append(L"..");
				line = textToDraw;
			}
		}
	}
}