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
#include "StyleSkin.h"
#include "apoc3d/Graphics/RenderSystem/Texture.h"
#include "apoc3d/Graphics/RenderSystem/Sprite.h"
#include "FontManager.h"

#include "apoc3d/Input/InputAPI.h"
#include "apoc3d/Input/Mouse.h"
#include "apoc3d/Input/Keyboard.h"

using namespace Apoc3D::Input;

namespace Apoc3D
{
	namespace UI
	{
		void Button::UpdateEvents()
		{
			if (!Visible)
				return;

			Mouse* mouse = InputAPIManager::getSingleton().getMouse();
			//Keyboard* keyb = InputAPIManager::getSingleton().getKeyboard();

			Apoc3D::Math::Rectangle rect = getAbsoluteArea();
			
			Point cursorPos = mouse->GetPosition();

			if (m_owner && rect.Contains(cursorPos))
			{
				if (!m_mouseOver)
				{
					m_mouseOver = true;
					OnMouseOver();
				}
				if (!m_mouseDown && mouse && mouse->IsLeftPressed())
				{
					m_mouseDown = true;
					OnPress();
				}
				else if (m_mouseDown && mouse && mouse->IsLeftUp())
				{
					m_mouseDown = false;
					OnRelease();
				}
			}
			else if (m_mouseOver)
			{
				m_mouseOver = false;
				m_mouseDown = false;
				OnMouseOut();
			}
		}

		void Button::Initialize(RenderDevice* device)
		{
			Control::Initialize(device);

			if (!m_skin)
			{
				assert(NormalTexture.isSet());
				if (NormalTexture.HasSourceRect)
				{
					Size.X = NormalTexture.SourceRect.Width;
					Size.Y = NormalTexture.SourceRect.Height;
				}
				else
				{
					Size.X = NormalTexture.Graphic->getWidth();
					Size.Y = NormalTexture.Graphic->getHeight();
				}
			}
			else
			{
				m_fontRef = m_skin->TitleTextFont;

				m_textSize = m_fontRef->MeasureString(Text);
				//if (Size.X < m_textSize.X + m_skin->BtnVertPadding)
				//if (Size.Y < m_textSize.Y + m_skin->BtnHozPadding)

				int32 hozPadding = m_skin->ButtonPadding.getHorizontalSum();
				int32 vertPadding = m_skin->ButtonPadding.getVerticalSum();

				if (Size.X == 0)
				{
					Size.X = m_textSize.X + hozPadding;

					if (OverlayIcon.isSet())
					{
						int32 cw = OverlayIcon.HasSourceRect ? OverlayIcon.SourceRect.Width : OverlayIcon.Graphic->getWidth();
						cw += hozPadding;

						if (cw > Size.X)
							Size.X = cw;
					}
				}

				if (Size.Y == 0)
				{
					Size.Y = m_textSize.Y + vertPadding;

					if (OverlayIcon.isSet())
					{
						int32 ch = OverlayIcon.HasSourceRect ? OverlayIcon.SourceRect.Height : OverlayIcon.Graphic->getHeight();
						ch += vertPadding;

						if (ch > Size.Y)
							Size.Y = ch;
					}
				}

			}
		}


		void Button::DrawDefaultButton(Sprite* sprite)
		{
			Apoc3D::Math::Rectangle graphicalArea(Position.X, Position.Y, Size.X, Size.Y);
			graphicalArea = m_skin->ButtonMargin.InflateRect(graphicalArea);

			const int GraphicalPaddingWidth = m_skin->ButtonRegionsNormal[0].Width + m_skin->ButtonRegionsNormal[2].Width;
			const int GraphicalPaddingHeight = m_skin->ButtonRegionsNormal[0].Height + m_skin->ButtonRegionsNormal[6].Height;


			Apoc3D::Math::Rectangle destRect[9];
			for (int i=0;i<9;i++)
			{
				destRect[i] = m_skin->ButtonRegionsNormal[i];

				destRect[i].X = graphicalArea.X + (destRect[i].X - m_skin->ButtonRegionsNormal[0].X);
				destRect[i].Y = graphicalArea.Y + (destRect[i].Y - m_skin->ButtonRegionsNormal[0].Y);
			}

			int eWidth = graphicalArea.Width - GraphicalPaddingWidth;
			int eHeight = graphicalArea.Height - GraphicalPaddingHeight;

			destRect[1].Width = eWidth; // top
			destRect[4].Width = eWidth; // mid
			destRect[7].Width = eWidth; // bottom

			destRect[3].Height = eHeight; // left
			destRect[4].Height = eHeight; // mid
			destRect[5].Height = eHeight; // right

			destRect[2].X = destRect[5].X = destRect[8].X = destRect[1].getRight();
			destRect[6].Y = destRect[7].Y = destRect[8].Y = destRect[3].getBottom();

			
			if (Text.size())
			{
				m_textSize = m_fontRef->MeasureString(Text);

				m_textPos.X = Position.X + (Size.X - m_textSize.X) / 2;
				m_textPos.Y = Position.Y + (Size.Y - m_textSize.Y) / 2;
			}


			
			if (Enabled)
			{
				const Apoc3D::Math::Rectangle* srcRect;
				if (m_mouseDown)
				{
					srcRect = m_skin->ButtonRegionsDown;
				}
				else if (m_mouseOver)
				{
					srcRect = m_skin->ButtonRegionsHover;
				}
				else
				{
					srcRect = m_skin->ButtonRegionsNormal;
				}

				for (int i=0;i<9;i++)
				{
					if (destRect[i].Width > 0 && destRect[i].Height)
						sprite->Draw(m_skin->SkinTexture, destRect[i], &srcRect[i], CV_White);
				}
			}
			else
			{
				for (int i=0;i<9;i++)
				{
					if (destRect[i].Width > 0 && destRect[i].Height)
						sprite->Draw(m_skin->SkinTexture, destRect[i], &m_skin->ButtonRegionsNormal[i], m_skin->ButtonDisabledColorMod);
				}
			}


			if (Text.size())
			{
				m_fontRef->DrawString(sprite, Text, m_textPos, m_hasTextColorValue ? m_textColorOverride : m_skin->TextColor);
			}
			if (OverlayIcon.isSet())
			{
				if (OverlayIcon.HasSourceRect)
				{
					int ix = (int)(Size.X - OverlayIcon.SourceRect.Width) / 2 + Position.X;
					int iy = (int)(Size.Y - OverlayIcon.SourceRect.Height) / 2 + Position.Y;
					Apoc3D::Math::Rectangle icoDR(ix, iy, OverlayIcon.SourceRect.Width, OverlayIcon.SourceRect.Height);
					sprite->Draw(OverlayIcon.Graphic, icoDR, &OverlayIcon.SourceRect, CV_White);
				}
				else
				{
					int ix = (int)(Size.X - OverlayIcon.Graphic->getWidth()) / 2 + Position.X;
					int iy = (int)(Size.Y - OverlayIcon.Graphic->getHeight()) / 2 + Position.Y;
					Apoc3D::Math::Rectangle icoDR(ix, iy, OverlayIcon.Graphic->getWidth(), OverlayIcon.Graphic->getHeight());
					sprite->Draw(OverlayIcon.Graphic, icoDR, nullptr, CV_White);
				}
			}
		}

		void Button::DrawCustomButton(Sprite* spriteBatch)
		{
			if (fabs(m_rotation) > 0.01f)
			{
				Matrix oldTransform;
				if (!spriteBatch->isUsingStack())
				{
					oldTransform = spriteBatch->getTransform();
				}

				Apoc3D::Math::Rectangle destRect = getArea();
				Matrix rot; Matrix::CreateRotationZ(rot, m_rotation);
				Matrix preT; Matrix::CreateTranslation(preT, -0.5f * destRect.Width, -0.5f * destRect.Height, 0);
				
				Matrix trans;
				Matrix::Multiply(trans, preT, rot);
				trans.SetTranslation(destRect.X + trans.M41 + 0.5f * destRect.Width, destRect.Y + trans.M42 + 0.5f * destRect.Height, trans.M43);
				spriteBatch->PreMultiplyTransform(trans);

				Apoc3D::Math::Rectangle newDestRect(0,0, destRect.Width, destRect.Height);
				
				if (Enabled)
				{
					if (m_mouseDown)
					{
						if (MouseDownTexture.isSet())
							spriteBatch->Draw(MouseDownTexture.Graphic, newDestRect, MouseDownTexture.HasSourceRect ? &MouseDownTexture.SourceRect : nullptr, m_modMouseDownColor);
						else
							spriteBatch->Draw(NormalTexture.Graphic, newDestRect, NormalTexture.HasSourceRect ? &NormalTexture.SourceRect : nullptr, m_modMouseDownColor);
					}
					else if (m_mouseOver)
					{
						if (MouseOverTexture.isSet())
							spriteBatch->Draw(MouseOverTexture.Graphic, newDestRect, MouseOverTexture.HasSourceRect ? &MouseOverTexture.SourceRect : nullptr, m_modMouseOverColor);
						else
							spriteBatch->Draw(NormalTexture.Graphic, newDestRect, NormalTexture.HasSourceRect ? &NormalTexture.SourceRect : nullptr, m_modMouseOverColor);
					}
					else
					{
						spriteBatch->Draw(NormalTexture.Graphic, newDestRect, NormalTexture.HasSourceRect ? &NormalTexture.SourceRect : nullptr, m_modColor);
					}
				}
				else
				{
					if (DisabledTexture.isSet())
						spriteBatch->Draw(DisabledTexture.Graphic, newDestRect, DisabledTexture.HasSourceRect ? &DisabledTexture.SourceRect : nullptr, m_modDisabledColor);
					else
						spriteBatch->Draw(NormalTexture.Graphic, newDestRect, NormalTexture.HasSourceRect ? &NormalTexture.SourceRect : nullptr, m_modDisabledColor);
				}

				if (OverlayIcon.isSet())
				{
					if (OverlayIcon.HasSourceRect)
					{
						int ix = (int)(Size.X - OverlayIcon.SourceRect.Width) / 2;
						int iy = (int)(Size.Y - OverlayIcon.SourceRect.Height) / 2;
						Apoc3D::Math::Rectangle icoDR(ix, iy, OverlayIcon.SourceRect.Width, OverlayIcon.SourceRect.Height);
						spriteBatch->Draw(OverlayIcon.Graphic, icoDR, &OverlayIcon.SourceRect, CV_White);
					}
					else
					{
						int ix = (int)(Size.X - OverlayIcon.Graphic->getWidth()) / 2;
						int iy = (int)(Size.Y - OverlayIcon.Graphic->getHeight()) / 2;
						Apoc3D::Math::Rectangle icoDR(ix, iy, OverlayIcon.Graphic->getWidth(), OverlayIcon.Graphic->getHeight());
						spriteBatch->Draw(OverlayIcon.Graphic, icoDR, nullptr, CV_White);
					}
				}


				if (spriteBatch->isUsingStack())
				{
					spriteBatch->PopTransform();
				}
				else
				{
					spriteBatch->SetTransform(oldTransform);
				}
			}
			else
			{
				if (Enabled)
				{
					if (m_mouseDown)
					{
						if (MouseDownTexture.isSet())
							spriteBatch->Draw(MouseDownTexture.Graphic, getArea(), MouseDownTexture.HasSourceRect ? &MouseDownTexture.SourceRect : nullptr, m_modMouseDownColor);
						else
							spriteBatch->Draw(NormalTexture.Graphic, getArea(), NormalTexture.HasSourceRect ? &NormalTexture.SourceRect : nullptr, m_modMouseDownColor);
					}
					else if (m_mouseOver)
					{
						if (MouseOverTexture.isSet())
							spriteBatch->Draw(MouseOverTexture.Graphic, getArea(), MouseOverTexture.HasSourceRect ? &MouseOverTexture.SourceRect : nullptr, m_modMouseOverColor);
						else
							spriteBatch->Draw(NormalTexture.Graphic, getArea(), NormalTexture.HasSourceRect ? &NormalTexture.SourceRect : nullptr, m_modMouseOverColor);
					}
					else
					{
						spriteBatch->Draw(NormalTexture.Graphic, getArea(), NormalTexture.HasSourceRect ? &NormalTexture.SourceRect : nullptr, m_modColor);
					}
				}
				else
				{
					if (DisabledTexture.isSet())
						spriteBatch->Draw(DisabledTexture.Graphic, getArea(), DisabledTexture.HasSourceRect ? &DisabledTexture.SourceRect : nullptr, m_modDisabledColor);
					else
						spriteBatch->Draw(NormalTexture.Graphic, getArea(), NormalTexture.HasSourceRect ? &NormalTexture.SourceRect : nullptr, m_modDisabledColor);
				}

				if (OverlayIcon.isSet())
				{
					if (OverlayIcon.HasSourceRect)
					{
						int ix = (int)(Size.X - OverlayIcon.SourceRect.Width) / 2 + Position.X;
						int iy = (int)(Size.Y - OverlayIcon.SourceRect.Height) / 2 + Position.Y;
						Apoc3D::Math::Rectangle icoDR(ix, iy, OverlayIcon.SourceRect.Width, OverlayIcon.SourceRect.Height);
						spriteBatch->Draw(OverlayIcon.Graphic, icoDR, &OverlayIcon.SourceRect, CV_White);
					}
					else
					{
						int ix = (int)(Size.X - OverlayIcon.Graphic->getWidth()) / 2 + Position.X;
						int iy = (int)(Size.Y - OverlayIcon.Graphic->getHeight()) / 2 + Position.Y;
						Apoc3D::Math::Rectangle icoDR(ix, iy, OverlayIcon.Graphic->getWidth(), OverlayIcon.Graphic->getHeight());
						spriteBatch->Draw(OverlayIcon.Graphic, icoDR, nullptr, CV_White);
					}
				}
			}
		}

		void Button::Draw(Sprite* sprite)
		{
			if (m_skin)
			{
				DrawDefaultButton(sprite);
			}
			else
			{
				DrawCustomButton(sprite);
			}
		}
		void Button::Update(const GameTime* const time)
		{
			Control::Update(time);

			UpdateEvents();
		}


		void Button::SetTextColorOverride(ColorValue cv)
		{
			m_textColorOverride = cv;
			m_hasTextColorValue = true;
		}
		/************************************************************************/
		/*                                                                      */
		/************************************************************************/


		ButtonGroup::ButtonGroup(const List<Button*> buttons)
			: m_selectedIndex(0)
		{
			for (int i=0;i<buttons.getCount();i++)
			{
				m_button.Add(buttons[i]);
			}
		}

		ButtonGroup::ButtonGroup(const List<Button*> buttons, int selected)
			: m_selectedIndex(selected)
		{
			for (int i=0;i<buttons.getCount();i++)
			{
				m_button.Add(buttons[i]);
			}
		}

		const String& ButtonGroup::getSelectedText() const
		{
			return m_button[m_selectedIndex]->Text;
		}

		void ButtonGroup::setSelectedText(const String& text)
		{
			for (int i=0;i<m_button.getCount();i++)
			{
				if (m_button[i]->Text == text)
				{
					m_selectedIndex = i;
					break;
				}
			}
		}

		void ButtonGroup::Initialize(RenderDevice* device)
		{
			Control::Initialize(device);
			for (int i=0;i<m_button.getCount();i++)
			{
				m_button[i]->Initialize(device);
				m_button[i]->eventRelease.Bind(this, &ButtonGroup::Button_OnRelease);
				m_button[i]->setOwner(m_owner);
			}
		}

		void ButtonGroup::Button_OnRelease(Control* sender)
		{
			Button* button = static_cast<Button*>(sender);

			for (int i=0;i<m_button.getCount();i++)
			{
				if (m_button[i] == button)
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
			for (int i=0;i<m_button.getCount();i++)
			{
				//if (m_selectedIndex == i)
				{
					m_button[i]->Draw(sprite);
				}
			}
		}

		void ButtonGroup::Update(const GameTime* const time)
		{
			Control::Update(time);

			for (int i=0;i<m_button.getCount();i++)
			{
				m_button[i]->Update(time);
			}
		}

		/************************************************************************/
		/*  ButtonRow                                                           */
		/************************************************************************/

		ButtonRow::ButtonRow(const Point& position, int32 width, const List<String>& titles)
			: Control(position), m_countPerRow(titles.getCount()), m_count(titles.getCount()), m_numRows(1), m_hoverIndex(-1), m_selectedIndex(0)
		{ 
			Init(width, titles);
		}
		ButtonRow::ButtonRow(const Point& position, int32 width, int32 colCount, const List<String>& titles)
			: Control(position), m_countPerRow(colCount), m_count(titles.getCount()), m_hoverIndex(-1), m_selectedIndex(0)
		{
			m_numRows = (m_count + m_countPerRow - 1) / m_countPerRow;

			Init(width, titles);
		}

		void ButtonRow::Init(int32 width, const List<String>& titles)
		{
			Size.X = width;
			m_rowHeight = 0;

			m_textPos = new Point[m_count];
			m_textSize = new Point[m_count];
			m_titles = new String[m_count];
			m_buttonDstRect = new Apoc3D::Math::Rectangle[m_count];

			for (int i=0;i<titles.getCount();i++)
			{
				m_titles[i] = titles[i];
			}
		}

		ButtonRow::~ButtonRow()
		{
			delete[] m_textPos;
			delete[] m_textSize;

			delete[] m_titles;
			delete[] m_buttonDstRect;
		}


		void ButtonRow::Initialize(RenderDevice* device)
		{
			Control::Initialize(device);

			int32 vertPadding = m_skin->ButtonMargin.getVerticalSum();
			m_rowHeight = m_skin->TitleTextFont->getLineHeightInt() + vertPadding;

			Size.Y = m_rowHeight * m_numRows;

			for (int i=0;i<m_count;i++)
			{
				m_textSize[i] = m_fontRef->MeasureString(m_titles[i]);
			}

			UpdatePositions();

			Text = m_titles[0];
			Control::Initialize(device);
		}
		void ButtonRow::Draw(Sprite* sprite)
		{
			for (int i=0;i<m_count;i++)
			{
				const Apoc3D::Math::Rectangle* btnSrcRect = m_skin->ButtonRegionsNormal;
				
				if (i == m_selectedIndex)
				{
					btnSrcRect = m_skin->ButtonRegionsDown;
				}
				else if (i == m_hoverIndex)
				{
					btnSrcRect = m_skin->ButtonRegionsHover;
				}

				int32 colStyle = 0;
				VerticalBorderStyle vertStyle = VBS_Both;
				int32 idxInRow = i % m_countPerRow;
				int32 rowIndex = i / m_countPerRow;

				if (idxInRow == 0)
					colStyle = -1;
				else if (idxInRow == m_countPerRow - 1 || i == m_count-1)
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

				DrawButton(sprite, i, colStyle, vertStyle, btnSrcRect);
				
				const Apoc3D::Math::Rectangle& dstRect = m_buttonDstRect[i];

				if (idxInRow > 0 && idxInRow < m_countPerRow)
				{
					// vertical separation line
					sprite->Draw(m_skin->WhitePixelTexture, 
						Apoc3D::Math::Rectangle(dstRect.X-1,dstRect.Y,1,dstRect.Height),
						nullptr, m_skin->MIDBackgroundColor);
				}

				if (rowIndex<m_numRows-1)
				{
					// horizontal separation line
					sprite->Draw(m_skin->WhitePixelTexture, 
						Apoc3D::Math::Rectangle(dstRect.X,dstRect.getBottom(),dstRect.Width,1),
						nullptr, m_skin->MIDBackgroundColor);
				}

				m_fontRef->DrawString(sprite, m_titles[i], m_textPos[i], m_skin->TextColor);
			}
		}

		
		void ButtonRow::DrawButton(Sprite* sprite, int32 i, int32 colType, VerticalBorderStyle rowType, const Apoc3D::Math::Rectangle* btnSrcRect)
		{
			uint32 subBox = R9_MiddleCenter;

			ControlBounds padding;

			switch (rowType)
			{
			case Apoc3D::UI::ButtonRow::VBS_Both:
				subBox |= R9_TopCenter;
				subBox |= R9_BottomCenter;

				padding.Top = m_skin->ButtonMargin.Top;
				padding.Bottom = m_skin->ButtonMargin.Bottom;

				break;
			case Apoc3D::UI::ButtonRow::VBS_Top:
				subBox |= R9_TopCenter;
				padding.Top = m_skin->ButtonMargin.Top;
				break;
			case Apoc3D::UI::ButtonRow::VBS_Bottom:
				subBox |= R9_BottomCenter;
				padding.Bottom = m_skin->ButtonMargin.Bottom;
				break;
			}

			if (colType == -1)
			{
				subBox |= R9_MiddleLeft;
				padding.Left = m_skin->ButtonMargin.Left;

				if (subBox & R9_TopCenter)
					subBox |= R9_TopLeft;
				if (subBox & R9_BottomCenter)
					subBox |= R9_BottomLeft;
			}
			else if (colType == 1)
			{
				subBox |= R9_MiddleRight;
				padding.Right = m_skin->ButtonMargin.Right;

				if (subBox & R9_TopCenter)
					subBox |= R9_TopRight;
				if (subBox & R9_BottomCenter)
					subBox |= R9_BottomRight;
			}

			Apoc3D::Math::Rectangle graphicalArea = padding.InflateRect(m_buttonDstRect[i]);
		
			DrawRegion9Subbox(sprite, graphicalArea, CV_White, m_skin->SkinTexture, btnSrcRect, subBox);
			
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

		void ButtonRow::Update(const GameTime* const time)
		{
			if (!Visible)
			{
				m_hoverIndex = -1;
				m_mouseDown = false;
				return;
			}

			UpdatePositions();

			Mouse* mouse = InputAPIManager::getSingleton().getMouse();

			Point basePosition(0,0);
			if (m_owner)
			{
				basePosition = m_owner->GetAbsolutePosition();
			}
			m_hoverIndex = -1;

			for (int i=0;i<m_count;i++)
			{
				Apoc3D::Math::Rectangle rect = m_buttonDstRect[i];
				rect.X += basePosition.X;
				rect.Y += basePosition.Y;

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
						Text = m_titles[i];
						OnRelease();
					}
				}
			}

			if (m_hoverIndex == -1 && m_mouseDown)
			{
				m_mouseDown = false;
			}
		}

		void ButtonRow::UpdatePositions()
		{
			float cellWidth = (float)Size.X / m_countPerRow;
			Apoc3D::Math::Rectangle area = getArea();

			for (int i=0;i<m_count;i++)
			{
				int32 rowIndex = i / m_countPerRow;
				int32 idxInRow = i % m_countPerRow;

				const Point& textSize = m_textSize[i];
				Apoc3D::Math::Rectangle& dstRect = m_buttonDstRect[i];

				dstRect = Apoc3D::Math::Rectangle(
					area.X + (int)(cellWidth*idxInRow), area.Y + rowIndex*(m_rowHeight+1), 
					(int)cellWidth, m_rowHeight);

				m_textPos[i] = Point((int)(dstRect.X + (dstRect.Width - textSize.X) * 0.5f),
					(int)(dstRect.Y + (dstRect.Height - textSize.Y) * 0.5f));
			}
		}

		/************************************************************************/
		/*                                                                      */
		/************************************************************************/
		RadioButton::RadioButton(const Point& position, const String& text, bool checked)
			: Control(position, text), m_checked(checked), m_canUncheck(true), m_mouseOver(false), m_mouseDown(false), m_textOffset(0,0)
		{
			
		}
		void RadioButton::Initialize(RenderDevice* device)
		{
			Control::Initialize(device);
		}
		void RadioButton::Draw(Sprite* sprite)
		{

		}
		void RadioButton::Update(const GameTime* const time)
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

