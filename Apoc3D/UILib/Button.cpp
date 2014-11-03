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
#include "Button.h"
#include "FontManager.h"
#include "StyleSkin.h"
#include "apoc3d/Graphics/RenderSystem/Texture.h"
#include "apoc3d/Graphics/RenderSystem/Sprite.h"
#include "apoc3d/Math/MathCommon.h"

#include "apoc3d/Input/InputAPI.h"
#include "apoc3d/Input/Mouse.h"
#include "apoc3d/Input/Keyboard.h"

using namespace Apoc3D::Input;

namespace Apoc3D
{
	namespace UI
	{
		/************************************************************************/
		/*  Button                                                              */
		/************************************************************************/

		Button::ButtonEvent Button::eventAnyPress;
		Button::ButtonEvent Button::eventAnyRelease;

		Button::Button(const ButtonVisualSettings& settings, const Point& position, const String& text)
			: Control(nullptr, position), m_text(text), AutoSizedX(true), AutoSizedY(true)
		{
			Initialize(settings);
		}

		Button::Button(const ButtonVisualSettings& settings, const Point& position, int width, const String& text)
			: Control(nullptr, position, Point(width, 0)), m_text(text), AutoSizedY(true)
		{
			Initialize(settings);
		}

		Button::Button(const ButtonVisualSettings& settings, const Point& position, int width, int height, const String& text)
			: Control(nullptr, position, Point(width, height)), m_text(text)
		{
			Initialize(settings);
		}

		Button::Button(const ButtonVisualSettings& settings, const Point& position, const Point& size, const String& text)
			: Control(nullptr, position, size), m_text(text)
		{
			Initialize(settings);
		}

		//////////////////////////////////////////////////////////////////////////

		Button::Button(const StyleSkin* skin, const Point& position, const String& text)
			: Control(skin, position), m_text(text), AutoSizedX(true), AutoSizedY(true)
		{
			Initialize(skin);
		}
		Button::Button(const StyleSkin* skin, const Point& position, int width, const String& text)
			: Control(skin, position, Point(width, 0)), m_text(text), AutoSizedY(true)
		{
			Initialize(skin);
		}
		Button::Button(const StyleSkin* skin, const Point& position, int width, int height, const String& text)
			: Control(skin, position, Point(width, height)), m_text(text)
		{
			Initialize(skin);
		}

		Button::Button(const StyleSkin* skin, const Point& position, const Point& size, const String& text)
			: Control(skin, position, size), m_text(text)
		{
			Initialize(skin);
		}

		Button::~Button() 
		{

		}

		void Button::Initialize(const StyleSkin* skin)
		{
			if (skin)
			{
				SetFont(skin->ButtonFont);
			}
			CopySkinGraphic(skin);

			UpdateSize();

		}
		void Button::CopySkinGraphic(const StyleSkin* skin)
		{
			if (skin)
			{
				NormalGraphic = UIGraphic(skin->SkinTexture, skin->ButtonNormalRegions, skin->ButtonNormalColor);
				MouseDownGraphic = UIGraphic(skin->SkinTexture, skin->ButtonDownRegions, skin->ButtonDownColor);
				MouseHoverGraphic = UIGraphic(skin->SkinTexture, skin->ButtonHoverRegions, skin->ButtonHoverColor);

				DisabledGraphic = UIGraphic(skin->SkinTexture, skin->ButtonDisabledRegions, skin->ButtonDisabledColor);

				TextSettings.TextColor = skin->TextColor;
				TextSettings.TextColorDisabled = skin->TextColorDisabled;

				TextSettings.TextPadding = skin->ButtonPadding;
				Margin = skin->ButtonMargin;
			}
		}

		void Button::Initialize(const ButtonVisualSettings& settings)
		{
			NormalGraphic = settings.NormalGraphic;
			
			if (settings.FontRef)
				SetFont(settings.FontRef);

			if (settings.TextColor.isSet())
				TextSettings.TextColor = settings.TextColor;

			if (settings.TextColorDisabled.isSet())
				TextSettings.TextColorDisabled = settings.TextColorDisabled;


			if (settings.ContentPadding.isSet())
				TextSettings.TextPadding = settings.ContentPadding;

			if (settings.Margin.isSet())
				Margin = settings.Margin;

			if (settings.DisabledGraphic.isSet())
				DisabledGraphic = settings.DisabledGraphic;

			if (settings.MouseHoverGraphic.isSet())
				MouseHoverGraphic = settings.MouseHoverGraphic;

			if (settings.MouseDownGraphic.isSet())
				MouseDownGraphic = settings.MouseDownGraphic;

			if (settings.OverlayIcon.isSet())
				OverlayIcon = settings.OverlayIcon;

			if (settings.DisabledOverlayIcon.isSet())
				DisabledOverlayIcon = settings.DisabledOverlayIcon;

			UpdateSize();
		}

		void Button::Resetup(const ButtonVisualSettings& settings)
		{
			Initialize(settings);
		}

		void Button::Draw(Sprite* sprite)
		{
			UIGraphicSimple& iconOverlay = Enabled ? OverlayIcon : (DisabledOverlayIcon.isSet() ? DisabledOverlayIcon : OverlayIcon);

			Apoc3D::Math::Rectangle graphicalArea = Margin.InflateRect(getAbsoluteArea());

			if (fabs(Rotation) > 0.01f)
			{
				Matrix oldTransform;
				if (!sprite->isUsingStack())
				{
					oldTransform = sprite->getTransform();
				}

				Apoc3D::Math::Rectangle destRect = graphicalArea;
				Matrix rot; Matrix::CreateRotationZ(rot, Rotation);
				Matrix preT; Matrix::CreateTranslation(preT, -0.5f * destRect.Width, -0.5f * destRect.Height, 0);

				Matrix trans;
				Matrix::Multiply(trans, preT, rot);
				trans.SetTranslation(destRect.X + trans.M41 + 0.5f * destRect.Width, destRect.Y + trans.M42 + 0.5f * destRect.Height, trans.M43);
				sprite->PreMultiplyTransform(trans);

				Apoc3D::Math::Rectangle newDestRect(0, 0, destRect.Width, destRect.Height);

				DrawButtonCore(sprite, newDestRect);

				iconOverlay.DrawCentered(sprite, Point(0, 0), m_size);

				if (sprite->isUsingStack())
				{
					sprite->PopTransform();
				}
				else
				{
					sprite->SetTransform(oldTransform);
				}
			}
			else
			{
				DrawButtonCore(sprite, graphicalArea);

				iconOverlay.DrawCentered(sprite, graphicalArea);
			}
		}

		void Button::DrawButtonCore(Sprite* sprite, const Apoc3D::Math::Rectangle& dstRect)
		{
			if (Enabled)
			{
				UIGraphic* selectedGraphic;

				if (m_mouseDown || ForceMouseDownVisual || (IsSwitchMode && IsSwitchedOn))
				{
					selectedGraphic = MouseDownGraphic.isSet() ? &MouseDownGraphic : &NormalGraphic;
				}
				else if (m_mouseOver)
				{
					selectedGraphic = MouseHoverGraphic.isSet() ? &MouseHoverGraphic : &NormalGraphic;
				}
				else
				{
					selectedGraphic = &NormalGraphic;
				}

				selectedGraphic->Draw(sprite, dstRect);
			}
			else
			{
				DisabledGraphic.Draw(sprite, dstRect);
			}

			if (m_text.size())
			{
				if (OmitsText && !AutoSizedX)
				{
					String txt = m_text;
					int space = Math::Max(0, dstRect.Width - TextSettings.TextPadding.getHorizontalSum());
					guiOmitLineText(m_fontRef, space, txt);
					TextSettings.Draw(sprite, m_fontRef, txt, dstRect, Enabled);
				}
				else
				{
					TextSettings.Draw(sprite, m_fontRef, m_text, dstRect, Enabled);
				}
			}
		}


		void Button::Update(const GameTime* time)
		{
			Control::Update(time);

			UpdateEvents();
		}
		
		void Button::UpdateEvents()
		{
			UpdateEvents_StandardButton(m_mouseOver, m_mouseDown, getAbsoluteArea(),
				&Button::OnMouseHover, &Button::OnMouseOut, &Button::OnPress, &Button::OnRelease);
		}

		void Button::UpdateSize()
		{
			if (m_fontRef)
			{
				Point textSize;
				
				if (AutoSizedX || AutoSizedY)
					textSize = m_fontRef->MeasureString(m_text);

				if (AutoSizedX)
				{
					int32 hozPad = TextSettings.TextPadding.getHorizontalSum();

					m_size.X = textSize.X + hozPad;
					if (OverlayIcon.isSet())
					{
						int32 cw = OverlayIcon.getWidth() + hozPad;
						
						if (cw > m_size.X)
							m_size.X = cw;
					}
				}

				if (AutoSizedY)
				{
					int32 vertPad = TextSettings.TextPadding.getVerticalSum();

					m_size.Y = textSize.Y + vertPad;

					if (OverlayIcon.isSet())
					{
						int32 ch = OverlayIcon.getHeight() + vertPad;
						
						if (ch > m_size.Y)
							m_size.Y = ch;
					}
				}
			}
			else if (NormalGraphic.isSet() && NormalGraphic.isSimple())
			{
				m_size.X = NormalGraphic.getWidth() - Margin.getHorizontalSum();
				m_size.Y = NormalGraphic.getHeight() - Margin.getVerticalSum();
			}
		}

		int32 Button::EvaluateAutoWidth() const
		{
			if (m_fontRef)
			{
				int32 hozPad = TextSettings.TextPadding.getHorizontalSum();

				Point textSize = m_fontRef->MeasureString(m_text);
				int32 w = textSize.X + hozPad;
				if (OverlayIcon.isSet())
				{
					int32 cw = OverlayIcon.getWidth() + hozPad;

					if (cw > m_size.X)
						w = cw;
				}
				return w;
			}
			else if (NormalGraphic.isSet() && NormalGraphic.isSimple())
			{
				return NormalGraphic.getWidth() - Margin.getHorizontalSum();
			}
			return 0;
		}
		

		void Button::SetFont(Font* fontRef)
		{
			if (fontRef != m_fontRef)
			{
				m_fontRef = fontRef;
				UpdateSize();
			}
		}
		void Button::SetText(const String& text)
		{
			if (m_text != text)
			{
				m_text = text;
				UpdateSize();
			}
		}

		void Button::OnMouseHover() { eventMouseHover.Invoke(this);  }
		void Button::OnMouseOut() { eventMouseOut.Invoke(this); }

		void Button::OnPress() 
		{
			eventPress.Invoke(this);

			if (RaiseGlobalEvent)
				eventAnyPress.Invoke(this);
		}
		void Button::OnRelease() 
		{
			if (IsSwitchMode)
			{
				IsSwitchedOn = !IsSwitchedOn;
			}

			eventRelease.Invoke(this);

			if (RaiseGlobalEvent)
				eventAnyRelease.Invoke(this);
		}

		/************************************************************************/
		/*  ButtonRow                                                           */
		/************************************************************************/

		ButtonRow::ButtonEvent ButtonRow::eventAnyPress;
		ButtonRow::ButtonEvent ButtonRow::eventAnyRelease;

		ButtonRow::ButtonRow(const StyleSkin* skin, const Point& position, int32 width, const List<String>& titles)
			: Control(skin, position), m_countPerRow(titles.getCount()), m_count(titles.getCount()), m_numRows(1)
		{ 
			Setup(skin);
			Init(width, titles);
		}
		ButtonRow::ButtonRow(const StyleSkin* skin, const Point& position, int32 width, int32 colCount, const List<String>& titles)
			: Control(skin, position), m_countPerRow(colCount), m_count(titles.getCount())
		{
			m_numRows = (m_count + m_countPerRow - 1) / m_countPerRow;

			Setup(skin);
			Init(width, titles);
		}

		ButtonRow::~ButtonRow()
		{
			//delete[] m_textPos;
			//delete[] m_textSize;

			delete[] m_titles;
			delete[] m_buttonDstRect;
		}

		void ButtonRow::Setup(const StyleSkin* skin)
		{
			m_fontRef = skin->TitleTextFont;

			ButtonMargin = skin->ButtonMargin;
			NormalGraphic = UIGraphic(skin->SkinTexture, skin->ButtonNormalRegions, skin->ButtonNormalColor);

			MouseHoverGraphic = UIGraphic(skin->SkinTexture, skin->ButtonHoverRegions, skin->ButtonHoverColor);
			MouseDownGraphic = UIGraphic(skin->SkinTexture, skin->ButtonDownRegions, skin->ButtonDownColor);
			DisabledGraphic = UIGraphic(skin->SkinTexture, skin->ButtonDisabledRegions, skin->ButtonDisabledColor);

			TextSettings.TextColor = skin->TextColor;
			TextSettings.TextColorDisabled = skin->TextColorDisabled;

			SeparationLineColor = skin->MIDBackgroundColor;
		}

		void ButtonRow::Init(int32 width, const List<String>& titles)
		{
			m_size.X = width;
			m_rowHeight = 0;

			m_titles = titles.AllocateArrayCopy();
			m_buttonDstRect = new Apoc3D::Math::Rectangle[m_count];

			int32 vertPadding = ButtonMargin.getVerticalSum();
			m_rowHeight = m_fontRef->getLineHeightInt() + vertPadding;

			m_size.Y = m_rowHeight * m_numRows;

			UpdatePositions();
		}

		void ButtonRow::Draw(Sprite* sprite)
		{
			for (int i = 0; i < m_count; i++)
			{
				Texture* tex = NormalGraphic.Graphic;
				const Apoc3D::Math::Rectangle* btnSrcRect = NormalGraphic.SourceRects.getElements();

				if (i == m_selectedIndex)
				{
					btnSrcRect = MouseDownGraphic.SourceRects.getElements();
					tex = MouseDownGraphic.Graphic;
				}
				else if (i == m_hoverIndex)
				{
					btnSrcRect = MouseHoverGraphic.SourceRects.getElements();
					tex = MouseHoverGraphic.Graphic;
				}

				int32 colStyle = 0;
				VerticalBorderStyle vertStyle = VBS_Both;
				int32 idxInRow = i % m_countPerRow;
				int32 rowIndex = i / m_countPerRow;

				if (idxInRow == 0)
					colStyle = -1;
				else if (idxInRow == m_countPerRow - 1 || i == m_count - 1)
					colStyle = 1;


				if (m_numRows >= 1)
				{
					if (rowIndex == 0)
						vertStyle = VBS_Top;
					else if (rowIndex == m_numRows - 1)
						vertStyle = VBS_Bottom;
					else
						vertStyle = VBS_None;
				}

				DrawButton(sprite, tex, i, colStyle, vertStyle, btnSrcRect);

				const Apoc3D::Math::Rectangle& dstRect = m_buttonDstRect[i];

				if (idxInRow > 0 && idxInRow < m_countPerRow)
				{
					// vertical separation line
					sprite->Draw(SystemUI::GetWhitePixel(),
						Apoc3D::Math::Rectangle(dstRect.X - 1, dstRect.Y, 1, dstRect.Height),
						nullptr, SeparationLineColor);
				}

				if (rowIndex < m_numRows - 1)
				{
					// horizontal separation line
					sprite->Draw(SystemUI::GetWhitePixel(),
						Apoc3D::Math::Rectangle(dstRect.X, dstRect.getBottom(), dstRect.Width, 1),
						nullptr, SeparationLineColor);
				}

				TextSettings.Draw(sprite, m_fontRef, m_titles[i], m_buttonDstRect[i], Enabled);
				//m_fontRef->DrawString(sprite, m_titles[i], m_textPos[i], m_skin->TextColor);
			}
		}

		void ButtonRow::DrawButton(Sprite* sprite, Texture* tex, int32 i, int32 colType, VerticalBorderStyle rowType, const Apoc3D::Math::Rectangle* btnSrcRect)
		{
			uint32 subBox = R9_MiddleCenter;

			ControlBounds padding;

			switch (rowType)
			{
			case Apoc3D::UI::ButtonRow::VBS_Both:
				subBox |= R9_TopCenter;
				subBox |= R9_BottomCenter;

				padding.Top = ButtonMargin.Top;
				padding.Bottom = ButtonMargin.Bottom;

				break;
			case Apoc3D::UI::ButtonRow::VBS_Top:
				subBox |= R9_TopCenter;
				padding.Top = ButtonMargin.Top;
				break;
			case Apoc3D::UI::ButtonRow::VBS_Bottom:
				subBox |= R9_BottomCenter;
				padding.Bottom = ButtonMargin.Bottom;
				break;
			}

			if (colType == -1)
			{
				subBox |= R9_MiddleLeft;
				padding.Left = ButtonMargin.Left;

				if (subBox & R9_TopCenter)
					subBox |= R9_TopLeft;
				if (subBox & R9_BottomCenter)
					subBox |= R9_BottomLeft;
			}
			else if (colType == 1)
			{
				subBox |= R9_MiddleRight;
				padding.Right = ButtonMargin.Right;

				if (subBox & R9_TopCenter)
					subBox |= R9_TopRight;
				if (subBox & R9_BottomCenter)
					subBox |= R9_BottomRight;
			}

			Apoc3D::Math::Rectangle graphicalArea = padding.InflateRect(m_buttonDstRect[i]);
		
			DrawRegion9Subbox(sprite, graphicalArea, CV_White, tex, btnSrcRect, subBox);
			
		}
		
		void ButtonRow::DrawRegion9Subbox(Sprite* sprite, const Apoc3D::Math::Rectangle& dstRect, ColorValue cv, 
			Texture* texture, const Apoc3D::Math::Rectangle* srcRects, uint32 subRegionFlags)
		{
			Apoc3D::Math::Rectangle srcRectsTable[9];
			Apoc3D::Math::Rectangle destRects[9];

			const uint flags[9] = 
			{
				R9_TopLeft,
				R9_TopCenter,
				R9_TopRight,
				R9_MiddleLeft,
				R9_MiddleCenter,
				R9_MiddleRight,
				R9_BottomLeft,
				R9_BottomCenter,
				R9_BottomRight
			};

			for (int32 i=0;i<9;i++)
			{
				if (subRegionFlags & flags[i])
				{
					srcRectsTable[i] = srcRects[i];
				}
			}
			
			guiGenerateRegion9Rects(dstRect, srcRectsTable, destRects);

			for (int i=0;i<9;i++)
			{
				if (destRects[i].Width > 0 && destRects[i].Height > 0)
					sprite->Draw(texture, destRects[i], &srcRectsTable[i], cv);
			}

		}

		void ButtonRow::Update(const GameTime* time)
		{
			m_hoverIndex = -1;

			if (!Visible || !Enabled)
			{
				m_mouseDown = false;
				return;
			}

			UpdatePositions();

			Mouse* mouse = InputAPIManager::getSingleton().getMouse();

			if (IsInteractive)
			{
				for (int i = 0; i < m_count; i++)
				{
					Apoc3D::Math::Rectangle rect = m_buttonDstRect[i];

					if (rect.Contains(mouse->GetPosition().X, mouse->GetPosition().Y))
					{
						m_hoverIndex = i;
						if (mouse->IsLeftPressed())
						{
							m_mouseDown = true;
							OnPress();
						}
						else if (m_mouseDown && mouse->IsLeftUp())
						{
							m_mouseDown = false;
							eventSelectedChanging.Invoke(i);
							m_selectedIndex = i;
							//Text = m_titles[i];
							OnRelease();
						}
					}
				}
			}
			else
			{
				m_mouseDown = false;
			}

			if (m_hoverIndex == -1 && m_mouseDown)
			{
				m_mouseDown = false;
			}
		}

		void ButtonRow::UpdatePositions()
		{
			int32 cellWidth = m_size.X / m_countPerRow;
			Apoc3D::Math::Rectangle area = getAbsoluteArea();

			for (int i = 0; i < m_count; i++)
			{
				int32 rowIndex = i / m_countPerRow;
				int32 idxInRow = i % m_countPerRow;

				m_buttonDstRect[i] =
				{
					area.X + cellWidth*idxInRow, 
					area.Y + rowIndex*(m_rowHeight + 1),
					cellWidth, m_rowHeight
				};
			}
		}

		void ButtonRow::OnPress()
		{
			eventPress.Invoke(this);
			eventAnyPress.Invoke(this);
		}
		void ButtonRow::OnRelease()
		{
			eventRelease.Invoke(this);
			eventAnyRelease.Invoke(this);
		}

		/************************************************************************/
		/*  ButtonRowTextured                                                   */
		/************************************************************************/

		ButtonGroupTextured::ButtonEvent ButtonGroupTextured::eventAnyPress;
		ButtonGroupTextured::ButtonEvent ButtonGroupTextured::eventAnyRelease;

		ButtonGroupTextured::ButtonGroupTextured(const Point& position, Texture* tex, const Apoc3D::Math::Rectangle& normalRegion,
			const List<Apoc3D::Math::Rectangle>& hoverRegs, const List<Apoc3D::Math::Rectangle>& downRegs, const List<ControlBounds>& paddings)
			: Position(position), m_graphic(tex)
		{
			m_graphicsSrcRect = normalRegion;

			m_buttonCount = hoverRegs.getCount();

			assert(m_buttonCount == downRegs.getCount());
			assert(m_buttonCount == paddings.getCount());

			m_isMouseHover = new bool[m_buttonCount];
			m_isMouseDown = new bool[m_buttonCount];
			memset(m_isMouseHover, 0, sizeof(bool) * m_buttonCount);
			memset(m_isMouseDown, 0, sizeof(bool) * m_buttonCount);

			m_graphicsSrcRectHover = new Apoc3D::Math::Rectangle[m_buttonCount];
			m_graphicsSrcRectDown = new Apoc3D::Math::Rectangle[m_buttonCount];
			m_hotAreaPaddings = new ControlBounds[m_buttonCount];
			for (int32 i = 0; i < m_buttonCount; i++)
			{
				m_hotAreaPaddings[i] = paddings[i];
				m_graphicsSrcRectDown[i] = downRegs[i];
				m_graphicsSrcRectHover[i] = hoverRegs[i];
			}
		}
		ButtonGroupTextured::~ButtonGroupTextured()
		{
			delete[] m_hotAreaPaddings;
			m_hotAreaPaddings = nullptr;

			delete[] m_graphicsSrcRectHover;
			m_graphicsSrcRectHover = nullptr;
			delete[] m_graphicsSrcRectDown;
			m_graphicsSrcRectDown = nullptr;

			delete[] m_isMouseHover;
			m_isMouseHover = nullptr;
			delete[] m_isMouseDown;
			m_isMouseDown = nullptr;
		}

		void ButtonGroupTextured::Draw(Sprite* sprite)
		{
			const Apoc3D::Math::Rectangle* srcRect = &m_graphicsSrcRect;

			for (int32 i = 0; i < m_buttonCount; i++)
			{
				if (m_isMouseDown[i] || ForceMouseDownLookIndex == i)
				{
					srcRect = &m_graphicsSrcRectDown[i];
				}
				else if (m_isMouseHover[i])
				{
					srcRect = &m_graphicsSrcRectHover[i];
				}
			}

			Apoc3D::Math::Rectangle dstRect = *srcRect;
			dstRect.X = Position.X - m_hotAreaPaddings[0].Left;
			dstRect.Y = Position.Y - m_hotAreaPaddings[0].Top;

			sprite->Draw(m_graphic, dstRect, srcRect, CV_White);
		}

		void ButtonGroupTextured::Update(const GameTime* time)
		{
			Mouse* mouse = InputAPIManager::getSingleton().getMouse();

			m_hasMouseHoverArea = false;

			for (int32 i = 0; i < m_buttonCount; i++)
			{
				Apoc3D::Math::Rectangle rect;

				rect = m_graphicsSrcRect;

				rect.X = Position.X + m_hotAreaPaddings[i].Left - m_hotAreaPaddings[0].Left;
				rect.Y = Position.Y + m_hotAreaPaddings[i].Top - m_hotAreaPaddings[0].Top;
				rect.Width -= m_hotAreaPaddings[i].getHorizontalSum();
				rect.Height -= m_hotAreaPaddings[i].getVerticalSum();

				if (rect.Contains(mouse->getX(), mouse->getY()))
				{
					if (m_isMouseHover[i])
					{
						m_isMouseHover[i] = true;
					}

					if (!m_isMouseDown[i] && mouse->IsLeftPressed())
					{
						m_isMouseDown[i] = true;
						eventPress.Invoke(this, i);
						eventAnyPress.Invoke(this, i);
					}
					else if (m_isMouseDown[i] && mouse->IsLeftUp())
					{
						m_isMouseDown[i] = false;

						eventRelease.Invoke(this, i);
						eventAnyRelease.Invoke(this, i);
					}

					m_hasMouseHoverArea = true;
					m_mouseHoverArea = rect;
					break;
				}
				else
				{
					m_isMouseHover[i] = false;
					m_isMouseDown[i] = false;
				}
			}
		}

		/************************************************************************/
		/*  ButtonGroup                                                         */
		/************************************************************************/

		ButtonGroup::ButtonGroup(const StyleSkin* skin, const List<Button*>& buttons)
			: Control(skin), m_button(buttons), m_selectedIndex(0)
		{
			Initialize();
		}

		ButtonGroup::ButtonGroup(const StyleSkin* skin, const List<Button*>& buttons, int selected)
			: Control(skin), m_button(buttons), m_selectedIndex(selected)
		{
			Initialize();
		}
		ButtonGroup::~ButtonGroup()
		{
			m_button.DeleteAndClear();
		}

		void ButtonGroup::Initialize()
		{
			for (Button* btn : m_button)
			{
				btn->eventRelease.Bind(this, &ButtonGroup::Button_OnRelease);
			}
		}

		void ButtonGroup::Button_OnRelease(Button* button)
		{
			for (int i = 0; i < m_button.getCount(); i++)
			{
				Button* btn = m_button[i];

				if (btn == button)
				{
					if (m_selectedIndex != i)
					{
						m_selectedIndex = i;
						eventSelectionChanged.Invoke(this);
					}
				}
			}
		}

		void ButtonGroup::Draw(Sprite* sprite)
		{
			for (Button* btn : m_button)
				btn->Draw(sprite);
		}

		void ButtonGroup::Update(const GameTime* time)
		{
			Control::Update(time);

			for (Button* btn : m_button)
				btn->Update(time);
		}

		/************************************************************************/
		/*  RadioButton                                                         */
		/************************************************************************/

		RadioButton::RadioButton(const StyleSkin* skin, const Point& position, const String& text, bool checked)
			: Control(skin, position), Text(text), m_checked(checked), m_textOffset(0,0)
		{
			
		}

		void RadioButton::Draw(Sprite* sprite)
		{

		}
		void RadioButton::Update(const GameTime* time)
		{

		}
		void RadioButton::UpdateEvents()
		{

		}
		void RadioButton::Toggle()
		{
			if (m_checked && m_canUncheck)
			{
				m_checked = false;
			}
			else if (!m_checked)
			{
				m_checked = true;
			}
		}
	}
}

