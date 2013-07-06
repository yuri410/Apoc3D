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
				assert(m_NormalTexture);
				if (m_NormalTexture)
				{
					Size.X = m_NormalTexture->getWidth();
					Size.Y = m_NormalTexture->getHeight();
				}
			}
			else
			{
				m_textSize = m_fontRef->MeasureString(Text);
				//if (Size.X < m_textSize.X + m_skin->BtnVertPadding)
				//if (Size.Y < m_textSize.Y + m_skin->BtnHozPadding)

				if (Size.X == 0)
					Size.X = m_textSize.X + m_skin->BtnVertPadding;
				
				if (Size.Y == 0)
					Size.Y = m_textSize.Y + m_skin->BtnHozPadding;

			}
		}


		void Button::DrawDefaultButton(Sprite* sprite)
		{
			m_btnDestRect[0].X = (int)(Position.X);
			m_btnDestRect[0].Y = (int)(Position.Y);
			m_btnDestRect[0].Width = m_skin->ButtonTexture->getWidth() - 1;
			m_btnDestRect[0].Height = (int)Size.Y;

			m_btnDestRect[1].X = m_btnDestRect[0].X + m_btnDestRect[0].Width;
			m_btnDestRect[1].Y = m_btnDestRect[0].Y;
			m_btnDestRect[1].Width = (int)Size.X - m_btnDestRect[0].Width * 2;
			m_btnDestRect[1].Height = m_btnDestRect[0].Height;

			m_btnDestRect[2].X = m_btnDestRect[1].X + m_btnDestRect[1].Width;
			m_btnDestRect[2].Y = m_btnDestRect[0].Y;
			m_btnDestRect[2].Width = m_btnDestRect[0].Width;
			m_btnDestRect[2].Height = m_btnDestRect[0].Height;

			if (Text.size())
			{
				m_textSize = m_fontRef->MeasureString(Text);

				m_textPos.X = (int)(m_btnDestRect[0].X + (Size.X - m_textSize.X) / 2.0f);
				m_textPos.Y = (int)(m_btnDestRect[0].Y + (m_btnDestRect[0].Height - m_textSize.Y) / 2.0f);
			}

			//if (Size.X < m_textSize.X+20)
			//{
			//	Size.X = m_textSize.X + 20;
			//}
			//if (Size.Y < m_skin->ButtonTexture->getHeight())
			//{
			//	Size.Y = m_skin->ButtonTexture->getHeight();
			//}
			//if (Size.Y < m_textSize.Y + 5)
			//{
			//	Size.Y = m_textSize.Y + 5;
			//}
			//

			if (m_mouseOver)
			{
				sprite->Draw(m_skin->ButtonTexture, m_btnDestRect[0], &m_skin->BtnSrcRect[0], m_skin->BtnDimColor);
				sprite->Draw(m_skin->ButtonTexture, m_btnDestRect[1], &m_skin->BtnSrcRect[1], m_skin->BtnDimColor);
				sprite->Draw(m_skin->ButtonTexture, m_btnDestRect[2], &m_skin->BtnSrcRect[2], m_skin->BtnDimColor);
				if (Text.size())
				{
					m_fontRef->DrawString(sprite, Text, m_textPos, m_skin->BtnTextDimColor);
				}
			}
			else
			{
				sprite->Draw(m_skin->ButtonTexture, m_btnDestRect[0], &m_skin->BtnSrcRect[0], m_skin->BtnHighLightColor);
				sprite->Draw(m_skin->ButtonTexture, m_btnDestRect[1], &m_skin->BtnSrcRect[1], m_skin->BtnHighLightColor);
				sprite->Draw(m_skin->ButtonTexture, m_btnDestRect[2], &m_skin->BtnSrcRect[2], m_skin->BtnHighLightColor);
				if (Text.size())
				{
					m_fontRef->DrawString(sprite, Text, m_textPos, m_skin->BtnTextDimColor);
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
						if (m_MouseDownTexture)
							spriteBatch->Draw(m_MouseDownTexture, newDestRect, nullptr, m_modMouseDownColor);
						else
							spriteBatch->Draw(m_NormalTexture, newDestRect, nullptr, m_modMouseDownColor);
					}
					else if (m_mouseOver)
					{
						if (m_MouseOverTexture)
							spriteBatch->Draw(m_MouseOverTexture, newDestRect, nullptr, m_modMouseOverColor);
						else
							spriteBatch->Draw(m_NormalTexture, newDestRect, nullptr, m_modMouseOverColor);
					}
					else
					{
						spriteBatch->Draw(m_NormalTexture, newDestRect, nullptr, m_modColor);
					}
				}
				else
				{
					if (m_DisabledTexture)
						spriteBatch->Draw(m_DisabledTexture, newDestRect, nullptr, m_modDisabledColor);
					else
						spriteBatch->Draw(m_NormalTexture, newDestRect, nullptr, m_modDisabledColor);
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
						if (m_MouseDownTexture)
							spriteBatch->Draw(m_MouseDownTexture, getArea(), nullptr, m_modMouseDownColor);
						else
							spriteBatch->Draw(m_NormalTexture, getArea(), nullptr, m_modMouseDownColor);
					}
					else if (m_mouseOver)
					{
						if (m_MouseOverTexture)
							spriteBatch->Draw(m_MouseOverTexture, getArea(), nullptr, m_modMouseOverColor);
						else
							spriteBatch->Draw(m_NormalTexture, getArea(), nullptr, m_modMouseOverColor);
					}
					else
					{
						spriteBatch->Draw(m_NormalTexture, getArea(), nullptr, m_modColor);
					}
				}
				else
				{
					if (m_DisabledTexture)
						spriteBatch->Draw(m_DisabledTexture, getArea(), nullptr, m_modDisabledColor);
					else
						spriteBatch->Draw(m_NormalTexture, getArea(), nullptr, m_modDisabledColor);
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
			
			m_texPos = new Point[m_count];
			m_titles = new String[m_count];
			m_btRect = new Apoc3D::Math::Rectangle[m_count];
			m_rect = new Apoc3D::Math::Rectangle[m_count];

			for (int i=0;i<titles.getCount();i++)
			{
				m_titles[i] = titles[i];
			}
		}
		ButtonRow::~ButtonRow()
		{
			delete[] m_texPos;
			delete[] m_titles;
			delete[] m_btRect;
			delete[] m_rect;
		}
		void ButtonRow::Initialize(RenderDevice* device)
		{
			Size.Y = m_skin->ButtonTexture->getHeight();

			Control::Initialize(device);
			
			float cellWidth = (float)Size.X / m_count;
			Apoc3D::Math::Rectangle area = getArea();

			for (int i=0;i<m_count;i++)
			{
				Point s = m_fontRef->MeasureString(m_titles[i]);
				m_btRect[i] = Apoc3D::Math::Rectangle(
					area.X + (int)(cellWidth*i), area.Y, (int)cellWidth, m_skin->ButtonTexture->getHeight());
				m_texPos[i] = Point((int)(m_btRect[i].X + (m_btRect[i].Width - s.X) * 0.5f),
					(int)(m_btRect[i].Y + (m_btRect[i].Height - s.Y) * 0.5f));

				if (i==0)
				{
					m_rect[i] = Apoc3D::Math::Rectangle(
						Position.X+m_skin->ButtonTexture->getWidth() + (int)(cellWidth*i),
						Position.Y, 
						(int)cellWidth-m_skin->ButtonTexture->getWidth(), 
						m_skin->ButtonTexture->getHeight());
				}
				else if (i == m_count-1)
				{
					m_rect[i] = Apoc3D::Math::Rectangle(
						Position.X + (int)(cellWidth*i), 
						Position.Y, 
						(int)cellWidth - m_skin->ButtonTexture->getWidth(), 
						m_skin->ButtonTexture->getHeight());
				}
				else
				{
					m_rect[i] = Apoc3D::Math::Rectangle(Position.X + (int)(cellWidth*i),
						Position.Y,
						(int)cellWidth,
						m_skin->ButtonTexture->getHeight());
				}
			}
			m_tailPos = Point(m_rect[m_count-1].getRight(), Position.Y);
			Text = m_titles[0];
			Control::Initialize(device);
		}
		void ButtonRow::Draw(Sprite* sprite)
		{
			if (m_selectedIndex == 0)
				sprite->Draw(m_skin->ButtonTexture, Position, m_skin->BackColor);
			else
				sprite->Draw(m_skin->ButtonTexture, Position, m_skin->BtnDimColor);

			for (int i=0;i<m_count;i++)
			{
				if (i == m_selectedIndex)
					sprite->Draw(m_skin->ButtonTexture, m_rect[i], &m_skin->BtnSrcRect[1], m_skin->BackColor);
				else if (i == m_hoverIndex)
					sprite->Draw(m_skin->ButtonTexture, m_rect[i], &m_skin->BtnSrcRect[1], m_skin->BtnHighLightColor);
				else
					sprite->Draw(m_skin->ButtonTexture, m_rect[i], &m_skin->BtnSrcRect[1], m_skin->BtnDimColor);

				if (i>0)
					sprite->Draw(m_skin->ButtonTexture, 
					Apoc3D::Math::Rectangle(m_rect[i].X-1,m_rect[i].Y,1,m_rect[i].Height),
					&m_skin->BtnSrcRect[1], CV_Black);

				m_fontRef->DrawString(sprite, m_titles[i], m_texPos[i], m_skin->ForeColor);
			}

			//m_btnDestRect[0].X = (int)(Position.X);
			//m_btnDestRect[0].Y = (int)(Position.Y);
			//m_btnDestRect[0].Width = m_skin->ButtonTexture->getWidth() - 1;
			//m_btnDestRect[0].Height = (int)Size.Y;

			//m_btnDestRect[1].X = m_btnDestRect[0].X + m_btnDestRect[0].Width;
			//m_btnDestRect[1].Y = m_btnDestRect[0].Y;
			//m_btnDestRect[1].Width = (int)Size.X - m_btnDestRect[0].Width * 2;
			//m_btnDestRect[1].Height = m_btnDestRect[0].Height;

			//m_btnDestRect[2].X = m_btnDestRect[1].X + m_btnDestRect[1].Width;
			//m_btnDestRect[2].Y = m_btnDestRect[0].Y;
			//m_btnDestRect[2].Width = m_btnDestRect[0].Width;
			//m_btnDestRect[2].Height = m_btnDestRect[0].Height;

			Apoc3D::Math::Rectangle rect(
				m_tailPos.X,
				m_tailPos.Y,
				m_skin->BtnSrcRect[0].Width, 
				m_skin->BtnSrcRect[0].Height);
			if (m_selectedIndex == m_count-1)
				sprite->Draw(m_skin->ButtonTexture, rect, &m_skin->BtnSrcRect[2], m_skin->BackColor);
			else
				sprite->Draw(m_skin->ButtonTexture, rect, &m_skin->BtnSrcRect[2], m_skin->BtnDimColor);
					//(m_hoverIndex == m_count-1)? m_skin->BtnHighLightColor : m_skin->BtnDimColor);

		}
		void ButtonRow::Update(const GameTime* const time)
		{
			if (!Visible)
			{
				m_hoverIndex = -1;
				m_mouseDown = false;
				return;
			}

			Mouse* mouse = InputAPIManager::getSingleton().getMouse();

			Point rectPos;
			Apoc3D::Math::Rectangle rect;// = Apoc3D::Math::Rectangle::Empty;
			rectPos.X = Position.X + m_owner->Position.X;
			rectPos.Y = Position.Y + m_owner->Position.Y;

			m_hoverIndex = -1;

			for (int i=0;i<m_count;i++)
			{
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

