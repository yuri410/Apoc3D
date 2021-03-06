
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

#include "UICommon.h"

#include "apoc3d/Graphics/RenderSystem/Sprite.h"
#include "apoc3d/Graphics/RenderSystem/Texture.h"
#include "apoc3d/Graphics/RenderSystem/RenderDevice.h"
#include "apoc3d/Graphics/RenderSystem/RenderStateManager.h"
#include "apoc3d/Platform/API.h"

#include "apoc3d/Math/Color.h"

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
		UIGraphic::UIGraphic(Texture* tex, std::initializer_list<Apoc3D::Math::Rectangle> srcRect) : UIGraphic(tex, srcRect, CV_White) { }

		UIGraphic::UIGraphic(Texture* tex, ColorValue color)
			: Graphic(tex), ModColor(color) { }

		UIGraphic::UIGraphic(Texture* tex, std::initializer_list<Apoc3D::Math::Rectangle> srcRect, ColorValue color)
			: Graphic(tex), ModColor(color)
		{
			assert(srcRect.size() == 0 || srcRect.size() == 1 || srcRect.size() == 3 || srcRect.size() == 9);
			SourceRects.AddList(srcRect);
		}


		UIGraphic::UIGraphic(Texture* tex, const Apoc3D::Math::Rectangle& srcRect, int32 left, int32 right, int32 top, int32 bottom)
			: UIGraphic(tex, srcRect, left, right, top, bottom, CV_White) { }

		UIGraphic::UIGraphic(Texture* tex, const Apoc3D::Math::Rectangle& srcRect, int32 left, int32 right, int32 top, int32 bottom, ColorValue color)
			: Graphic(tex), ModColor(color)
		{
			Apoc3D::Math::Rectangle regions[9];
			srcRect.DivideTo9Regions(left, right, top, bottom, regions);
			SourceRects.AddArray(regions);
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
			assert(isSet() && (SourceRects.getCount() == 0 || SourceRects.getCount() == 1 || SourceRects.getCount() == 3));
			return (isSimple() || is3patch()) ? SourceRects[0].Width : Graphic->getWidth();
		}
		int32 UIGraphic::getHeight() const
		{
			assert(isSet() && (SourceRects.getCount() == 0 || SourceRects.getCount() == 1 || SourceRects.getCount() == 3));
			return (isSimple() || is3patch()) ? SourceRects[0].Height : Graphic->getHeight();
		}
		Point UIGraphic::getSize() const 
		{
			assert(!is3patch() && !is9patch());
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
				case SI_Left: return Left;
				case SI_Top: return Top;
				case SI_Right: return Right;
				case SI_Bottom: return Bottom;
			}
			return 0;
		}

		void ControlBounds::SetFromLeftTopRightBottom(int32 padding[4])
		{
			Left = padding[SI_Left];
			Right = padding[SI_Right];
			Top = padding[SI_Top];
			Bottom = padding[SI_Bottom];
		}
		void ControlBounds::SetZero()
		{
			Left = Right = Top = Bottom = 0;
		}
		void ControlBounds::SetLeftRight(int32 l, int32 r)
		{
			Left = l; Right = r;
		}
		void ControlBounds::SetTopBottom(int32 t, int32 b)
		{
			Top = t; Bottom = b;
		}

		/************************************************************************/
		/* TextRenderSettings                                                   */
		/************************************************************************/
		
		void TextRenderSettings::Draw(Sprite* sprite, Font* font, const String& text, const Apoc3D::Math::Rectangle& area, bool enabled) const
		{
			Apoc3D::Math::Rectangle modArea = TextPadding.ShrinkRect(area);

			Point pos = modArea.getTopLeft();
			Point size = modArea.getSize();

			Point textPos = pos + GetTextOffset(font, text, size);

			DrawNoAlignment(sprite, font, text, textPos, enabled);
		}

		void TextRenderSettings::Draw(Sprite* sprite, Font* font, const String& text, const Point& pos, const Point& size, bool enabled) const
		{
			Draw(sprite, font, text, Apoc3D::Math::Rectangle(pos, size), enabled);
		}

		void TextRenderSettings::DrawNoAlignment(Sprite* sprite, Font* font, const String& text, const Point& textPos, bool enabled) const
		{
			ColorValue textColor = enabled ? TextColor : TextColorDisabled;
			ColorValue textShadowColor = enabled ? TextShadowColor : TextShadowColorDisabled;

			if (CV_GetColorA(textColor) == 0 && (!HasTextShadow || CV_GetColorA(textShadowColor) == 0))
				return;

			if (HasTextShadow)
			{
				Point shdTextPos = textPos + TextShadowOffset;
				font->DrawString(sprite, text, shdTextPos, textShadowColor);
			}

			font->DrawString(sprite, text, textPos, textColor);
		}
		void TextRenderSettings::DrawHyperLinkText(Sprite* sprite, Font* font, const String& text, const Point& pos, bool enabled, bool selected, float underlineWidth) const
		{
			ColorValue textColor;
			if (selected)
			{
				textColor = enabled ? TextSelectionColor : TextSelectionColorDisabled;
			}
			else
			{
				textColor = enabled ? TextColor : TextColorDisabled;
			}

			ColorValue shadowColor;
			Point shadowOffset;
			if (!HasTextShadow)
			{
				Color4 c(textColor);
				Color4::Negate(c, c);
				c.Alpha = 1;

				shadowColor = c.ToArgb();
				shadowOffset = Point(1, 1);
			}
			else
			{
				shadowColor = TextShadowColor;
				shadowOffset = TextShadowOffset;
			}

			ColorValue textShadowColor = 0;
			if (selected && enabled)
			{
				textShadowColor = shadowColor;
			}

			float ascender, descender;
			font->getStandardMetrics(ascender, descender);

			Point textSize = font->MeasureString(text);
			Point underlineStart = pos +Point(0, textSize.Y);
			Point underlineEnd = pos + textSize;
			underlineStart.Y -= (int32)descender;
			underlineEnd.Y -= (int32)descender;
			if (textShadowColor)
			{
				sprite->DrawLine(SystemUI::GetWhitePixel(), underlineStart + shadowOffset,
					underlineEnd + shadowOffset, textShadowColor, underlineWidth, LineCapOptions::Butt);

				font->DrawString(sprite, text, pos + shadowOffset, textShadowColor);
			}

			sprite->DrawLine(SystemUI::GetWhitePixel(), underlineStart,
				underlineEnd, textColor, underlineWidth, LineCapOptions::Butt);

			font->DrawString(sprite, text, pos, textColor);
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

		void TextRenderSettings::DrawTextLine(Sprite* sprite, const Point& start, const Point& end, float width, bool enabled, bool selected) const
		{
			if (HasTextShadow)
			{
				sprite->DrawLine(SystemUI::GetWhitePixel(), start + TextShadowOffset, end + TextShadowOffset,
					enabled ? TextShadowColor : TextShadowColorDisabled, 1, LineCapOptions::Butt);
			}

			ColorValue lineColor;
			if (selected)
			{
				lineColor = enabled ? TextSelectionColor : TextSelectionColorDisabled;
			}
			else
			{
				lineColor = enabled ? TextColor : TextColorDisabled;
			}

			sprite->DrawLine(SystemUI::GetWhitePixel(), start, end, lineColor, width, LineCapOptions::Butt);
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
		/*  ButtonVisualSettings                                                */
		/************************************************************************/

		void ButtonVisualSettings::Setup(Texture* tex, const ARect& normal, const ARect& hover, const ARect& down)
		{
			NormalGraphic = UIGraphic(tex, normal);
			MouseHoverGraphic = UIGraphic(tex, hover);
			MouseDownGraphic = UIGraphic(tex, down);
			DisabledGraphic = NormalGraphic;
			DisabledGraphic.getContent().ModColor = CV_PackLA(0xff, 0x7f);
		}

		void ButtonVisualSettings::Setup(Texture* tex, const ARect& normal, const ARect& hover, const ARect& down, const ControlBounds& margin)
		{
			Setup(tex, normal, down, hover);
			Margin = margin;
		}

		void ButtonVisualSettings::Setup(Texture* tex, const ARect& normal, const ARect& hover, const ARect& down, const ARect& disabled, const ControlBounds& margin)
		{
			NormalGraphic = UIGraphic(tex, normal);
			MouseHoverGraphic = UIGraphic(tex, hover);
			MouseDownGraphic = UIGraphic(tex, down);
			DisabledGraphic = UIGraphic(tex, disabled);
			Margin = margin;
		}

		/************************************************************************/
		/*  ScissorTestState                                                    */
		/************************************************************************/

		ScissorTestScope::ScissorTestScope(const Apoc3D::Math::Rectangle& region, Sprite* sprite)
			: m_sprite(sprite)
		{
			Apoc3D::Math::Rectangle dstRect = region;

			RenderStateManager* stMgr = sprite->getRenderDevice()->getRenderState();
			m_oldScissorTest = stMgr->getScissorTestEnabled();

			if (m_oldScissorTest)
			{
				m_oldScissorRect = stMgr->getScissorTestRect();
				dstRect = Apoc3D::Math::Rectangle::Intersect(dstRect, m_oldScissorRect);
			}

			m_isEmpty = dstRect.Width == 0 || dstRect.Height == 0;

			if (!m_isEmpty)
			{
				sprite->Flush();
				stMgr->setScissorTest(true, &dstRect);
			}
		}
		ScissorTestScope::~ScissorTestScope()
		{
			if (!m_isEmpty)
			{
				RenderStateManager* stMgr = m_sprite->getRenderDevice()->getRenderState();

				m_sprite->Flush();
				stMgr->setScissorTest(m_oldScissorTest, m_oldScissorTest ? &m_oldScissorRect : nullptr);
			}
		}


		/************************************************************************/
		/*  SystemUI                                                            */
		/************************************************************************/

		static SystemUIImpl sysUI;

		RectangleF& SystemUI::UIArea = sysUI.UIArea;
		RectangleF& SystemUI::MaximizedArea = sysUI.MaximizedArea;

		Form*& SystemUI::InteractingForm = sysUI.InteractingForm;
		Form*& SystemUI::TopMostForm = sysUI.TopMostForm;
		Form*& SystemUI::ModalForm = sysUI.m_modalForm;

		MenuBar*& SystemUI::MainMenu = sysUI.m_mainMenu;
		

		float SystemUI::DoubleClickInterval = Platform::GetSystemDoubleClickIntervalInMS() / 1000.0f;

		void SystemUI::Initialize(RenderDevice* device) { sysUI.Initialize(device); }
		void SystemUI::Finalize() { sysUI.Finalize(); }

		void SystemUI::Add(ControlContainer* cc) { sysUI.Add(cc); }
		void SystemUI::Remove(ControlContainer* cc) { sysUI.Remove(cc); }
		void SystemUI::RemoveForm(const String& name) { sysUI.RemoveForm(name); }
		void SystemUI::RemoveContainer(const String& name) { sysUI.RemoveContainer(name); }
		void SystemUI::ScheduleBringFront(ControlContainer* cc) { sysUI.ScheduleBringFront(cc); }

		void SystemUI::Draw() { sysUI.Draw(); }
		void SystemUI::Update(const AppTime* time) { sysUI.Update(time); }

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

		void guiDrawProgressBar(Sprite* sprite, const Point& position, int32 width, float value,
			Texture* texture, const Apoc3D::Math::Rectangle(&bgSrcRects)[3], const Apoc3D::Math::Rectangle(&barSrcRects)[3],
			const ControlBounds& margin, int32 barStartPad, int32 barEndPad)
		{
			int32 graphicalWidth = width + margin.getHorizontalSum();

			int32 prgStartCap = margin.Left + barStartPad;
			int32 prgEndCap = margin.Right + barEndPad;

			int32 filledPartWidth = Math::Round((graphicalWidth - prgStartCap - prgEndCap) * value) + prgStartCap;

			Point pos = position;
			pos.X -= margin.Left;
			pos.Y -= margin.Top;

			guiDrawRegion3(sprite, pos, graphicalWidth, CV_White, texture, bgSrcRects);

			if (filledPartWidth>prgStartCap)
			{
				guiDrawRegion3Clipped(sprite, pos, graphicalWidth, filledPartWidth, CV_White, texture, barSrcRects);
			}

		}


		/************************************************************************/
		/* Control Codes                                                        */
		/************************************************************************/

		String ControlCodes::MakeColorControl(uint32 cv)
		{
			if (cv == 0)
			{
				// avoid being treated as null terminator
				// transparent white
				cv = CV_PackLA(255, 0);
			}

			String r;
			r.append(1, Font_Color);
			char16_t ar = (char16_t)((cv >> 16) & 0xffff);
			char16_t gb = (char16_t)(cv & 0xffff);
			r.append(1, ar);
			r.append(1, gb);
			return r;
		}
		String ControlCodes::MakeMoveControl(const Point& position, bool passConditionCheck, bool relative)
		{
			String r;
			r.append(1, Font_Move);

			uint32 flags = passConditionCheck ? 1 : 0;
			flags |= (relative ? 1 : 0) << 1;

			uint16 x = ~(uint16)position.X; // avoid being treated as null terminator
			uint16 y = ~(uint16)position.Y;

			// 15 bit
			x &= 0x7FFF;
			y &= 0x7FFF;

			uint32 data = (flags << 30) | (x << 15) | y;

			char16_t ch1 = (char16_t)((data >> 16) & 0xffff);
			char16_t ch2 = (char16_t)(data & 0xffff);
			r.append(1, ch1);
			r.append(1, ch2);
			return r;
		}

		String ControlCodes::MakeHyperLink(const String& linkText, uint16 linkID)
		{
			String r;
			r.append(1, Label_HyperLinkStart);
			r.append(1, ~linkID); // avoid being treated as null terminator
			r += linkText;
			r.append(1, Label_HyperLinkEnd);

			return r;
		}
	}
}