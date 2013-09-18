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
			
			Point cursorPos = mouse->GetCurrentPosition();

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

				int32 hozPadding = m_skin->ButtonPadding[StyleSkin::SI_Left] + m_skin->ButtonPadding[StyleSkin::SI_Right];
				int32 vertPadding = m_skin->ButtonPadding[StyleSkin::SI_Top] + m_skin->ButtonPadding[StyleSkin::SI_Bottom];

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
			graphicalArea.X -= m_skin->ButtonMargin[StyleSkin::SI_Left];
			graphicalArea.Y -= m_skin->ButtonMargin[StyleSkin::SI_Top];
			graphicalArea.Width += m_skin->ButtonMargin[StyleSkin::SI_Left] + m_skin->ButtonMargin[StyleSkin::SI_Right];
			graphicalArea.Height += m_skin->ButtonMargin[StyleSkin::SI_Top] + m_skin->ButtonMargin[StyleSkin::SI_Bottom];

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


		ButtonGroup::ButtonGroup(const FastList<Button*> buttons)
			: m_selectedIndex(0)
		{
			for (int i=0;i<buttons.getCount();i++)
			{
				m_button.Add(buttons[i]);
			}
		}

		ButtonGroup::ButtonGroup(const FastList<Button*> buttons, int selected)
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
				m_button[i]->eventRelease().Bind(this, &ButtonGroup::Button_OnRelease);
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
						m_eChangeSelection.Invoke(this);
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
		/*                                                                      */
		/************************************************************************/

		ButtonRow::ButtonRow(const Point& position, float width, const List<String>& titles)
			: Control(position), m_count(titles.getCount()), m_hoverIndex(-1), m_selectedIndex(0)
		{ 
			Size.X = (int)width;
			
			//m_texPos = new Point[m_count];
			m_titles = new String[m_count];
			m_btRect = new Apoc3D::Math::Rectangle[m_count];
			//m_rect = new Apoc3D::Math::Rectangle[m_count];

			for (int i=0;i<titles.getCount();i++)
			{
				m_titles[i] = titles[i];
			}
		}
		ButtonRow::~ButtonRow()
		{
			//delete[] m_texPos;
			delete[] m_titles;
			delete[] m_btRect;
			//delete[] m_rect;
		}
		void ButtonRow::Initialize(RenderDevice* device)
		{
			Control::Initialize(device);

			int32 vertPadding = m_skin->ButtonMargin[StyleSkin::SI_Top] + m_skin->ButtonMargin[StyleSkin::SI_Bottom];
			Size.Y = m_skin->TitleTextFont->getLineHeightInt() + vertPadding;
			
			float cellWidth = (float)Size.X / m_count;
			Apoc3D::Math::Rectangle area = getArea();

			for (int i=0;i<m_count;i++)
			{
				Point s = m_fontRef->MeasureString(m_titles[i]);
				m_btRect[i] = Apoc3D::Math::Rectangle(
					area.X + (int)(cellWidth*i), area.Y, (int)cellWidth, Size.Y);
				m_texPos[i] = Point((int)(m_btRect[i].X + (m_btRect[i].Width - s.X) * 0.5f),
					(int)(m_btRect[i].Y + (m_btRect[i].Height - s.Y) * 0.5f));
			}

			/*float cellWidth = (float)Size.X / m_count;
			Apoc3D::Math::Rectangle area = getArea();

			for (int i=0;i<m_count;i++)
			{
			Point s = m_fontRef->MeasureString(m_titles[i]);
			m_btRect[i] = Apoc3D::Math::Rectangle(
			area.X + (int)(cellWidth*i), area.Y, (int)cellWidth, Size.Y);
			m_texPos[i] = Point((int)(m_btRect[i].X + (m_btRect[i].Width - s.X) * 0.5f),
			(int)(m_btRect[i].Y + (m_btRect[i].Height - s.Y) * 0.5f));

			if (i==0)
			{
			m_rect[i] = Apoc3D::Math::Rectangle(
			Position.X+m_skin->ButtonTexture->getWidth() + (int)(cellWidth*i),
			Position.Y, 
			(int)cellWidth-m_skin->ButtonTexture->getWidth(), 
			Size.Y);
			}
			else if (i == m_count-1)
			{
			m_rect[i] = Apoc3D::Math::Rectangle(
			Position.X + (int)(cellWidth*i), 
			Position.Y, 
			(int)cellWidth - m_skin->ButtonTexture->getWidth(), 
			Size.Y);
			}
			else
			{
			m_rect[i] = Apoc3D::Math::Rectangle(Position.X + (int)(cellWidth*i),
			Position.Y,
			(int)cellWidth,
			Size.Y);
			}
			}
			m_tailPos = Point(m_rect[m_count-1].getRight(), Position.Y);*/
			Text = m_titles[0];
			Control::Initialize(device);
		}
		void ButtonRow::Draw(Sprite* sprite)
		{
			for (int i=0;i<m_count;i++)
			{
				const Apoc3D::Math::Rectangle* btnSrc = m_skin->ButtonRegionsNormal;
				if (i == m_hoverIndex)
				{
					btnSrc = m_skin->ButtonRegionsHover;
				}
				else if (i == m_selectedIndex && m_mouseDown)
				{
					btnSrc = m_skin->ButtonRegionsDown;
				}

				Apoc3D::Math::Rectangle currentRegions[6];
				if (i == 0)
				{
					Apoc3D::Math::Rectangle graphicalArea = m_btRect[i];
					graphicalArea.X -= m_skin->ButtonMargin[StyleSkin::SI_Left];
					graphicalArea.Y -= m_skin->ButtonMargin[StyleSkin::SI_Top];
					graphicalArea.Width += m_skin->ButtonMargin[StyleSkin::SI_Left];
					graphicalArea.Height += m_skin->ButtonMargin[StyleSkin::SI_Top] + m_skin->ButtonMargin[StyleSkin::SI_Bottom];

					const int GraphicalPaddingWidth = btnSrc[0].Width;// + btnSrc[2].Width;
					const int GraphicalPaddingHeight = btnSrc[0].Height + btnSrc[6].Height;

					currentRegions[0] = btnSrc[0];
					currentRegions[1] = btnSrc[1];
					currentRegions[2] = btnSrc[3];
					currentRegions[3] = btnSrc[4];
					currentRegions[4] = btnSrc[6];
					currentRegions[5] = btnSrc[7];

					for (int i=0;i<6;i++)
					{
						currentRegions[i].X += graphicalArea.X - btnSrc[0].X;
						currentRegions[i].Y += graphicalArea.Y - btnSrc[0].Y;
					}

					int eWidth = graphicalArea.Width - GraphicalPaddingWidth;
					int eHeight = graphicalArea.Height - GraphicalPaddingHeight;

					currentRegions[1].Width = eWidth; // top
					currentRegions[3].Width = eWidth; // mid
					currentRegions[5].Width = eWidth; // bottom

					currentRegions[2].Height = eHeight; // left
					currentRegions[3].Height = eHeight; // mid

					currentRegions[4].Y = currentRegions[5].Y = currentRegions[3].getBottom();

					sprite->Draw(m_skin->SkinTexture, currentRegions[0], &btnSrc[0], CV_White);
					sprite->Draw(m_skin->SkinTexture, currentRegions[1], &btnSrc[1], CV_White);
					sprite->Draw(m_skin->SkinTexture, currentRegions[2], &btnSrc[3], CV_White);
					sprite->Draw(m_skin->SkinTexture, currentRegions[3], &btnSrc[4], CV_White);
					sprite->Draw(m_skin->SkinTexture, currentRegions[4], &btnSrc[6], CV_White);
					sprite->Draw(m_skin->SkinTexture, currentRegions[5], &btnSrc[7], CV_White);
				}
				else if (i == m_count - 1)
				{
					Apoc3D::Math::Rectangle graphicalArea = m_btRect[i];
					graphicalArea.Y -= m_skin->ButtonMargin[StyleSkin::SI_Top];
					graphicalArea.Width += m_skin->ButtonMargin[StyleSkin::SI_Right];
					graphicalArea.Height += m_skin->ButtonMargin[StyleSkin::SI_Top] + m_skin->ButtonMargin[StyleSkin::SI_Bottom];

					const int GraphicalPaddingWidth = btnSrc[2].Width;
					const int GraphicalPaddingHeight = btnSrc[0].Height + btnSrc[6].Height;

					currentRegions[0] = btnSrc[1];
					currentRegions[1] = btnSrc[2];
					currentRegions[2] = btnSrc[4];
					currentRegions[3] = btnSrc[5];
					currentRegions[4] = btnSrc[7];
					currentRegions[5] = btnSrc[8];

					for (int i=0;i<6;i++)
					{
						currentRegions[i].X += graphicalArea.X - btnSrc[0].X;
						currentRegions[i].Y += graphicalArea.Y - btnSrc[0].Y;
					}

					int eWidth = graphicalArea.Width - GraphicalPaddingWidth;
					int eHeight = graphicalArea.Height - GraphicalPaddingHeight;

					currentRegions[0].Width = eWidth; // top
					currentRegions[2].Width = eWidth; // mid
					currentRegions[4].Width = eWidth; // bottom

					currentRegions[2].Height = eHeight; // mid
					currentRegions[3].Height = eHeight; // right

					currentRegions[4].Y = currentRegions[5].Y = currentRegions[3].getBottom();

					sprite->Draw(m_skin->SkinTexture, currentRegions[0], &btnSrc[1], CV_White);
					sprite->Draw(m_skin->SkinTexture, currentRegions[1], &btnSrc[2], CV_White);
					sprite->Draw(m_skin->SkinTexture, currentRegions[2], &btnSrc[4], CV_White);
					sprite->Draw(m_skin->SkinTexture, currentRegions[3], &btnSrc[5], CV_White);
					sprite->Draw(m_skin->SkinTexture, currentRegions[4], &btnSrc[7], CV_White);
					sprite->Draw(m_skin->SkinTexture, currentRegions[5], &btnSrc[8], CV_White);
				}
				else
				{
					Apoc3D::Math::Rectangle graphicalArea = m_btRect[i];
					graphicalArea.Height += m_skin->ButtonMargin[StyleSkin::SI_Top] + m_skin->ButtonMargin[StyleSkin::SI_Bottom];

					const int GraphicalPaddingWidth = 0;
					const int GraphicalPaddingHeight = btnSrc[0].Height + btnSrc[6].Height;

					currentRegions[0] = btnSrc[1];
					currentRegions[1] = btnSrc[4];
					currentRegions[2] = btnSrc[7];

					for (int i=0;i<6;i++)
					{
						currentRegions[i].X += graphicalArea.X - btnSrc[0].X;
						currentRegions[i].Y += graphicalArea.Y - btnSrc[0].Y;
					}

					int eWidth = graphicalArea.Width - GraphicalPaddingWidth;
					int eHeight = graphicalArea.Height - GraphicalPaddingHeight;

					currentRegions[0].Width = eWidth; // top
					currentRegions[1].Width = eWidth; // mid
					currentRegions[2].Width = eWidth; // bottom

					currentRegions[2].Y = currentRegions[1].getBottom();

					sprite->Draw(m_skin->SkinTexture, currentRegions[0], &btnSrc[1], CV_White);
					sprite->Draw(m_skin->SkinTexture, currentRegions[1], &btnSrc[4], CV_White);
					sprite->Draw(m_skin->SkinTexture, currentRegions[2], &btnSrc[7], CV_White);
				}

				if (i>0)
					sprite->Draw(m_skin->WhitePixelTexture, 
						Apoc3D::Math::Rectangle(m_btRect[i].X-1,m_btRect[i].Y,1,m_btRect[i].Height),
						nullptr, m_skin->MIDBackgroundColor);

				m_fontRef->DrawString(sprite, m_titles[i], m_texPos[i], m_skin->TextColor);
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

			float cellWidth = (float)Size.X / m_count;
			Apoc3D::Math::Rectangle area = getArea();

			Mouse* mouse = InputAPIManager::getSingleton().getMouse();

			Point rectPos;
			Apoc3D::Math::Rectangle rect;// = Apoc3D::Math::Rectangle::Empty;
			rectPos.X = Position.X + m_owner->Position.X;
			rectPos.Y = Position.Y + m_owner->Position.Y;

			m_hoverIndex = -1;

			for (int i=0;i<m_count;i++)
			{
				Point s = m_fontRef->MeasureString(m_titles[i]);
				m_btRect[i] = Apoc3D::Math::Rectangle(
					area.X + (int)(cellWidth*i), area.Y, (int)cellWidth, Size.Y);
				m_texPos[i] = Point((int)(m_btRect[i].X + (m_btRect[i].Width - s.X) * 0.5f),
					(int)(m_btRect[i].Y + (m_btRect[i].Height - s.Y) * 0.5f));

				rect.X = rectPos.X;
				rect.Y = rectPos.Y;
				rect.Width = m_btRect[i].Width;
				rect.Height = m_btRect[i].Height;

				if (rect.Contains(mouse->GetCurrentPosition().X, mouse->GetCurrentPosition().Y))
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

				rectPos.X += m_btRect[i].Width;
			}

			if (m_hoverIndex == -1 && m_mouseDown)
			{
				m_mouseDown = false;
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
	}
}

