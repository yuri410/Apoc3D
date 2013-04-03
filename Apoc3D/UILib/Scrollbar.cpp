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
#include "Scrollbar.h"
#include "FontManager.h"
#include "StyleSkin.h"
#include "Button.h"

#include "apoc3d/Input/Mouse.h"
#include "apoc3d/Input/InputAPI.h"
#include "apoc3d/Graphics/RenderSystem/Texture.h"
#include "apoc3d/Graphics/RenderSystem/Sprite.h"

using namespace Apoc3D::Input;

namespace Apoc3D
{
	namespace UI
	{
		HScrollbar::HScrollbar(const Point& position, int width)
			: Control(position), m_value(0), m_max(0), m_step(1), m_isScrolling(false), m_inverted(false)
		{
			Size.X = width;
			Size.Y = 12;
		}
		HScrollbar::~HScrollbar()
		{
			delete m_btLeft;
			delete m_btRight;
		}
		void HScrollbar::setPosition(const Point& pos)
		{
			m_btLeft->Position = pos;
			Position = pos;
			setWidth(Size.X);
		}
		void HScrollbar::setWidth(int w)
		{
			Size.X = w;
			m_btRight->Position.X = Position.X + w - 12;
			m_btRight->Position.Y = Position.Y;
		}
		void HScrollbar::Initialize(RenderDevice* device)
		{
			Control::Initialize(device);
			m_btLeft = new Button(Position,1, L"");
			m_btLeft->setOwner(getOwner());
			m_btLeft->setNormalTexture(m_skin->HScrollBar_Button);
			m_btLeft->eventPress().bind(this, &HScrollbar::btLeft_OnPress);
			m_btLeft->Initialize(device);

			m_cursorMidDest = Apoc3D::Math::Rectangle(0,0,1, m_skin->HScrollBar_Cursor->getHeight());

			m_btRight = new Button(Point(Position.X + Size.X - 12, Position.Y), 1, L"");
			m_btRight->setOwner(getOwner());
			m_btRight->setNormalTexture(m_skin->HScrollBar_Button);
			m_btRight->eventPress().bind(this, &HScrollbar::btRight_OnPress);
			m_btRight->setRotation(ToRadian(180));
			m_btRight->Initialize(device);
		}

		void HScrollbar::btLeft_OnPress(Control* ctrl)
		{
			if (!m_inverted)
			{
				if (m_value>0)
				{
					m_value -= m_step;
					if (!m_eChangeValue.empty())
						m_eChangeValue(this);
				}
			}
			else if (m_value<m_max)
			{
				m_value += m_step;
				if (!m_eChangeValue.empty())
					m_eChangeValue(this);
			}
		}
		void HScrollbar::btRight_OnPress(Control* ctrl)
		{
			if (!m_inverted)
			{
				if (m_value<m_max)
				{
					m_value += m_step;
					if (!m_eChangeValue.empty())
						m_eChangeValue(this);
				}
			}
			else if (m_value>0)
			{
				m_value -= m_step;
				if (!m_eChangeValue.empty())
					m_eChangeValue(this);
			}
		}

		void HScrollbar::Update(const GameTime* const time)
		{
			Mouse* mouse = InputAPIManager::getSingleton().getMouse();

			if (getOwner()->getArea().Contains(m_cursorArea) && m_cursorArea.Contains(mouse->GetCurrentPosition()))
			{
				if (mouse->IsLeftPressed())
				{
					m_isScrolling = true;
					m_cursorOffset = mouse->GetCurrentPosition();
					m_cursorOffset.X -= m_cursorArea.X;
					m_cursorOffset.Y -= m_cursorArea.Y;
				}
			}

			if (m_isScrolling)
			{
				if (mouse->IsLeftPressedState())
					UpdateScrolling();
				else if (mouse->IsLeftReleasedState())
					m_isScrolling = false;
			}

			if (m_max>0)
			{
				m_btLeft->Update(time);
				m_btRight->Update(time);
			}
		}

		float round(float r) {
			return (r > 0.0f) ? floor(r + 0.5f) : ceil(r - 0.5f);
		}
		void HScrollbar::UpdateScrolling()
		{
			Mouse* mouse = InputAPIManager::getSingleton().getMouse();
			
			m_cursorPos.X = mouse->GetCurrentPosition().X - m_cursorOffset.X - getOwner()->Position.X;

			if (m_cursorPos.X < Position.X + 12)
			{
				m_cursorPos.X = Position.X + 12;
			}
			else if (m_cursorPos.X > Position.X + Size.X - m_cursorArea.Width - 9)
			{
				m_cursorPos.X = Position.X + Size.X - m_cursorArea.Width - 9;
			}

			float x = (float)(m_cursorPos.X - m_backArea.X);

			int value;

			if (!m_inverted)
			{
				value = (int)round(x/(m_backArea.Width-m_cursorArea.Width)*m_max);
			}
			else
			{
				value = m_max - (int)round(x/(m_backArea.Width - m_cursorArea.Width)*m_max);
			}

			if (value <0)
				value = 0;
			else if (value>m_max)
				value = m_max;

			if (m_value != value)
			{
				m_value = value;
				if (!m_eChangeValue.empty())
				{
					m_eChangeValue(this);
				}
			}
		}
		void HScrollbar::Draw(Sprite* sprite)
		{
			DrawBackground(sprite);
			if (m_max>0)
				DrawCursor(sprite);
			m_btLeft->Draw(sprite);
			m_btRight->Draw(sprite);
		}
		void HScrollbar::DrawBackground(Sprite* sprite)
		{

			m_backArea = Apoc3D::Math::Rectangle(
				Position.X + 12, Position.Y, Size.X - 24,12);

			sprite->Draw(m_skin->HScrollBar_Back, m_backArea, CV_White);
		}

		void HScrollbar::DrawCursor(Sprite* sprite)
		{
			m_cursorArea.Width = max(20, m_backArea.Width - max(20, m_max/4));
			m_cursorArea.Height = 12;
			m_cursorPos.Y = Position.Y;
			if (!m_isScrolling)
			{
				if (!m_inverted)
				{
					m_cursorPos.X = (int)( m_backArea.X + (Size.X - 21 - m_cursorArea.Width) * ((float)m_value/(float)m_max));
				}
				else
				{
					m_cursorPos.X = (int)( m_backArea.X + (Size.X - 21 - m_cursorArea.Width) * ((float)(m_max-m_value)/(float)m_max));
				}
			}
			m_cursorArea.X = m_cursorPos.X + getOwner()->Position.X;
			m_cursorArea.Y = m_cursorPos.Y + getOwner()->Position.Y;

			Apoc3D::Math::Rectangle dstRect(m_cursorPos.X, m_cursorPos.Y, m_skin->HSCursorLeft.Width, m_skin->HSCursorLeft.Height);
			sprite->Draw(m_skin->HScrollBar_Cursor, dstRect, &m_skin->HSCursorLeft, CV_White);

			m_cursorMidDest.X = m_cursorPos.X + 3;
			m_cursorMidDest.Y = m_cursorPos.Y;
			m_cursorMidDest.Width = m_cursorArea.Width - 6;
			sprite->Draw(m_skin->HScrollBar_Cursor, m_cursorMidDest, &m_skin->HSCursorMiddle, CV_White);

			dstRect = Apoc3D::Math::Rectangle(
				m_cursorPos.X + m_cursorMidDest.Width, m_cursorPos.Y, m_skin->HSCursorRight.Width, m_skin->HSCursorRight.Height);
			sprite->Draw(m_skin->HScrollBar_Cursor, dstRect, &m_skin->HSCursorRight, CV_White);

			
		}

		/************************************************************************/
		/*                                                                      */
		/************************************************************************/
		VScrollBar::VScrollBar(const Point& position, int height)
			: Control(position), m_value(0), m_max(0), m_step(1), m_isScrolling(false), m_inverted(false)
		{
			Size.X = 12;
			Size.Y = height;
		}
		VScrollBar::~VScrollBar()
		{
			delete m_btDown;
			delete m_btUp;
		}
		void VScrollBar::setHeight(int w)
		{
			Size.Y= w;
			m_btDown->Position.Y = Position.Y + w-12;
			m_btDown->Position.X = Position.X;
		}
		void VScrollBar::setPosition(const Point& pos)
		{
			m_btUp->Position = pos;
			Position = pos;
			setHeight(Size.Y);
		}
		void VScrollBar::Initialize(RenderDevice* device)
		{
			Control::Initialize(device);
			m_btUp = new Button(Position, 1, L"");
			m_btUp->setOwner(getOwner());
			m_btUp->setNormalTexture(m_skin->VScrollBar_Button);
			m_btUp->eventPress().bind(this, &VScrollBar::btUp_OnPress);
			m_btUp->Initialize(device);

			m_btDown = new Button(Point(Position.X, Position.Y + Size.Y -12),1,L"");
			m_btDown->setOwner(getOwner());
			m_btDown->setNormalTexture(m_skin->VScrollBar_Button);
			m_btDown->eventPress().bind(this, &VScrollBar::btDown_OnPress);
			m_btDown->setRotation(ToRadian(180));
			m_btDown->Initialize(device);


			m_cursorMidDest = Apoc3D::Math::Rectangle(0,0,m_skin->VScrollBar_Cursor->getWidth(),1);

		}

		void VScrollBar::btUp_OnPress(Control* ctrl)
		{
			if (!m_inverted)
			{
				if (m_value>0)
				{
					m_value -= m_step;
					if (!m_eChangeValue.empty())
						m_eChangeValue(this);
				}
			}
			else if (m_value<m_max)
			{
				m_value += m_step;
				if (!m_eChangeValue.empty())
					m_eChangeValue(this);
			}
		}
		void VScrollBar::btDown_OnPress(Control* ctrl)
		{
			if (!m_inverted)
			{
				if (m_value<m_max)
				{
					m_value += m_step;
					if (!m_eChangeValue.empty())
						m_eChangeValue(this);
				}
			}
			else if (m_value>0)
			{
				m_value -= m_step;
				if (!m_eChangeValue.empty())
					m_eChangeValue(this);
			}
		}

		void VScrollBar::Update(const GameTime* const time)
		{
			Mouse* mouse = InputAPIManager::getSingleton().getMouse();

			if (m_cursorArea.Contains(mouse->GetCurrentPosition()))
			{
				if (mouse->IsLeftPressed())
				{
					m_isScrolling = true;
					m_cursorOffset = mouse->GetCurrentPosition();
					m_cursorOffset.X -= m_cursorArea.X;
					m_cursorOffset.Y -= m_cursorArea.Y;
				}
			}

			if (m_isScrolling)
			{
				if (mouse->IsLeftPressedState())
					UpdateScrolling();
				else if (mouse->IsLeftReleasedState())
					m_isScrolling = false;
			}

			if (m_max>0)
			{
				m_btUp->Update(time);
				m_btDown->Update(time);
			}
		}


		void VScrollBar::UpdateScrolling()
		{
			Mouse* mouse = InputAPIManager::getSingleton().getMouse();

			m_cursorPos.Y = mouse->GetCurrentPosition().Y - m_cursorOffset.Y - getOwner()->Position.Y;

			if (m_cursorPos.Y < Position.Y + 12)
			{
				m_cursorPos.Y = Position.Y + 12;
			}
			else if (m_cursorPos.Y > Position.Y + Size.Y - m_cursorArea.Height - 9)
			{
				m_cursorPos.Y = Position.Y + Size.Y - m_cursorArea.Height - 9;
			}

			float y = (float)(m_cursorPos.Y - m_backArea.Y);

			int value;

			if (!m_inverted)
			{
				value = (int)round(y/(m_backArea.Height-m_cursorArea.Height)*m_max);
			}
			else
			{
				value = m_max - (int)round(y/(m_backArea.Height - m_cursorArea.Height)*m_max);
			}

			if (value <0)
				value = 0;
			else if (value>m_max)
				value = m_max;

			if (m_value != value)
			{
				m_value = value;
				if (!m_eChangeValue.empty())
				{
					m_eChangeValue(this);
				}
			}
		}
		void VScrollBar::Draw(Sprite* sprite)
		{
			DrawBackground(sprite);
			DrawCursor(sprite);
			m_btUp->Draw(sprite);
			m_btDown->Draw(sprite);
		}
		void VScrollBar::DrawBackground(Sprite* sprite)
		{
			m_backArea = Apoc3D::Math::Rectangle(
				Position.X, Position.Y+12, 12, Size.Y - 24);

			sprite->Draw(m_skin->VScrollBar_Back, m_backArea, CV_White);
		}

		void VScrollBar::DrawCursor(Sprite* sprite)
		{
			m_cursorArea.Height = max(20, m_backArea.Height - max(20, m_max/4));
			m_cursorArea.Width = 12;
			m_cursorPos.X = Position.X;
			if (!m_isScrolling)
			{
				if (!m_inverted)
				{
					m_cursorPos.Y = (int)( m_backArea.Y + (Size.Y - 21 - m_cursorArea.Height) * ((float)m_value/(float)m_max));
				}
				else
				{
					m_cursorPos.Y = (int)( m_backArea.Y + (Size.Y - 21 - m_cursorArea.Height) * ((float)(m_max-m_value)/(float)m_max));
				}
			}
			m_cursorArea.X = m_cursorPos.X + getOwner()->Position.X;
			m_cursorArea.Y = m_cursorPos.Y + getOwner()->Position.Y;

			Apoc3D::Math::Rectangle dstRect(m_cursorPos.X, m_cursorPos.Y, abs(m_skin->VSCursorTop.Width), abs(m_skin->VSCursorTop.Height));
			sprite->Draw(m_skin->VScrollBar_Cursor, dstRect, &m_skin->VSCursorTop, CV_White);

			m_cursorMidDest.X = m_cursorPos.X;
			m_cursorMidDest.Y = m_cursorPos.Y + 3;
			m_cursorMidDest.Height = m_cursorArea.Height - 6;
			sprite->Draw(m_skin->VScrollBar_Cursor, m_cursorMidDest, &m_skin->VSCursorMiddle, CV_White);

			dstRect = Apoc3D::Math::Rectangle(
				m_cursorPos.X, m_cursorPos.Y+m_cursorMidDest.Height, abs(m_skin->VSCursorBottom.Width), abs(m_skin->VSCursorBottom.Height));
			sprite->Draw(m_skin->VScrollBar_Cursor, dstRect, &m_skin->VSCursorBottom, CV_White);

			
		}


	}
}