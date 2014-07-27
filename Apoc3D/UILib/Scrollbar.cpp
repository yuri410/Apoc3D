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

#include "apoc3d/Math/MathCommon.h"
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
			m_btRight->Position.X = Position.X + w - m_skin->HScrollBarBG.Height;
			m_btRight->Position.Y = Position.Y;
		}
		void HScrollbar::Initialize(RenderDevice* device)
		{
			Control::Initialize(device);

			Size.Y = m_skin->HScrollBarBG.Height;

			m_btLeft = new Button(Position,1, L"");
			m_btLeft->setOwner(getOwner());
			m_btLeft->NormalTexture = UIGraphic(m_skin->SkinTexture, m_skin->HScrollBarLeft);
			m_btLeft->eventPress.Bind(this, &HScrollbar::btLeft_OnPress);
			m_btLeft->Initialize(device);

			m_btRight = new Button(Point(Position.X + Size.X - 12, Position.Y), 1, L"");
			m_btRight->setOwner(getOwner());
			//m_btRight->setNormalTexture(m_skin->HScrollBar_Button);
			m_btRight->NormalTexture = UIGraphic(m_skin->SkinTexture, m_skin->HScrollBarRight);
			m_btRight->eventPress.Bind(this, &HScrollbar::btRight_OnPress);
			m_btRight->Initialize(device);
		}

		void HScrollbar::btLeft_OnPress(Control* ctrl)
		{
			if (!m_inverted)
			{
				if (m_value>0)
				{
					m_value -= m_step;
					
					eventValueChanged.Invoke(this);
				}
			}
			else if (m_value<m_max)
			{
				m_value += m_step;
				
				eventValueChanged.Invoke(this);
			}
		}
		void HScrollbar::btRight_OnPress(Control* ctrl)
		{
			if (!m_inverted)
			{
				if (m_value<m_max)
				{
					m_value += m_step;
					
					eventValueChanged.Invoke(this);
				}
			}
			else if (m_value>0)
			{
				m_value -= m_step;
				
				eventValueChanged.Invoke(this);
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

		
		void HScrollbar::UpdateScrolling()
		{
			Mouse* mouse = InputAPIManager::getSingleton().getMouse();
			
			m_cursorPos.X = mouse->GetCurrentPosition().X - m_cursorOffset.X - getOwner()->Position.X;

			if (m_cursorPos.X < Position.X + m_skin->HScrollBarLeft.Width)
			{
				m_cursorPos.X = Position.X + m_skin->HScrollBarLeft.Width;
			}
			else if (m_cursorPos.X > Position.X + Size.X - m_cursorArea.Width + m_skin->HScrollBarLeft.Width)
			{
				m_cursorPos.X = Position.X + Size.X - m_cursorArea.Width + m_skin->HScrollBarLeft.Width;
			}

			float x = (float)(m_cursorPos.X - m_backArea.X);

			int value;

			if (!m_inverted)
			{
				value = Math::Round(x/(m_backArea.Width-m_cursorArea.Width)*m_max);
			}
			else
			{
				value = m_max - Math::Round(x/(m_backArea.Width - m_cursorArea.Width)*m_max);
			}

			if (value <0)
				value = 0;
			else if (value>m_max)
				value = m_max;

			if (m_value != value)
			{
				m_value = value;
				
				eventValueChanged.Invoke(this);
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
				Position.X + m_skin->HScrollBarLeft.Width, Position.Y, Size.X - m_skin->HScrollBarLeft.Width - m_skin->HScrollBarRight.Width, m_skin->HScrollBarBG.Height);

			sprite->Draw(m_skin->SkinTexture, m_backArea, &m_skin->HScrollBarBG, CV_White);
		}

		void HScrollbar::DrawCursor(Sprite* sprite)
		{
			m_cursorArea.Width = Math::Max(20, m_backArea.Width - Math::Max(20, m_max / 4));
			m_cursorArea.Height = m_skin->HScrollBarCursor->Height;
			m_cursorPos.Y = Position.Y;
			if (!m_isScrolling)
			{
				if (!m_inverted)
				{
					m_cursorPos.X = (int)( m_backArea.X + (Size.X - m_cursorArea.Width) * ((float)m_value/(float)m_max));
				}
				else
				{
					m_cursorPos.X = (int)( m_backArea.X + (Size.X - m_cursorArea.Width) * ((float)(m_max-m_value)/(float)m_max));
				}
			}
			m_cursorArea.X = m_cursorPos.X + getOwner()->Position.X;
			m_cursorArea.Y = m_cursorPos.Y + getOwner()->Position.Y;

			Apoc3D::Math::Rectangle dstRect(m_cursorPos.X, m_cursorPos.Y, m_skin->HScrollBarCursor[0].Width, m_skin->HScrollBarCursor[0].Height);
			sprite->Draw(m_skin->SkinTexture, dstRect, &m_skin->HScrollBarCursor[0], CV_White);

			dstRect = Apoc3D::Math::Rectangle(
				m_cursorPos.X + m_skin->HScrollBarCursor[0].Width, 
				m_cursorPos.Y, 
				m_cursorArea.Width - m_skin->HScrollBarCursor[0].Width - m_skin->HScrollBarCursor[2].Width, m_skin->HScrollBarCursor[1].Height);
			sprite->Draw(m_skin->SkinTexture, dstRect, &m_skin->HScrollBarCursor[1], CV_White);

			int32 midWidth = dstRect.Width;
			dstRect = Apoc3D::Math::Rectangle(
				m_cursorPos.X + midWidth, m_cursorPos.Y, m_skin->HScrollBarCursor[2].Width, m_skin->HScrollBarCursor[2].Height);
			sprite->Draw(m_skin->SkinTexture, dstRect, &m_skin->HScrollBarCursor[2], CV_White);

			
		}

		/************************************************************************/
		/* VScrollBar                                                           */
		/************************************************************************/

		VScrollBar::VScrollBar(const Point& position, int height)
			: Control(position), m_value(0), m_max(0), m_step(1), m_isScrolling(false), m_inverted(false)
		{
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
			m_btDown->Position.Y = Position.Y + w-m_skin->VScrollBarBG.Width;
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

			Size.X = m_skin->VScrollBarBG.Width;


			m_btUp = new Button(Position, 1, L"");
			m_btUp->setOwner(getOwner());
			m_btUp->NormalTexture = UIGraphic(m_skin->SkinTexture, m_skin->VScrollBarUp);// (m_skin->VScrollBar_Button);
			m_btUp->eventPress.Bind(this, &VScrollBar::btUp_OnPress);
			m_btUp->Initialize(device);

			m_btDown = new Button(Point(Position.X, Position.Y + Size.Y -12),1,L"");
			m_btDown->setOwner(getOwner());
			m_btDown->NormalTexture = UIGraphic(m_skin->SkinTexture, m_skin->VScrollBarDown);
			m_btDown->eventPress.Bind(this, &VScrollBar::btDown_OnPress);
			m_btDown->Initialize(device);

		}

		void VScrollBar::btUp_OnPress(Control* ctrl)
		{
			if (!m_inverted)
			{
				if (m_value>0)
				{
					m_value -= m_step;
					
					eventValueChanged.Invoke(this);
				}
			}
			else if (m_value<m_max)
			{
				m_value += m_step;
				
				eventValueChanged.Invoke(this);
			}
		}
		void VScrollBar::btDown_OnPress(Control* ctrl)
		{
			if (!m_inverted)
			{
				if (m_value<m_max)
				{
					m_value += m_step;
					
					eventValueChanged.Invoke(this);
				}
			}
			else if (m_value>0)
			{
				m_value -= m_step;
				
				eventValueChanged.Invoke(this);
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

			if (m_cursorPos.Y < Position.Y + m_skin->VScrollBarUp.Height)
			{
				m_cursorPos.Y = Position.Y + m_skin->VScrollBarUp.Height;
			}
			else if (m_cursorPos.Y > Position.Y + Size.Y - m_cursorArea.Height + m_skin->VScrollBarUp.Height)
			{
				m_cursorPos.Y = Position.Y + Size.Y - m_cursorArea.Height + m_skin->VScrollBarUp.Height;
			}

			float y = (float)(m_cursorPos.Y - m_backArea.Y);

			int value;

			if (!m_inverted)
			{
				value = Math::Round(y/(m_backArea.Height-m_cursorArea.Height)*m_max);
			}
			else
			{
				value = m_max - Math::Round(y/(m_backArea.Height - m_cursorArea.Height)*m_max);
			}

			if (value <0)
				value = 0;
			else if (value>m_max)
				value = m_max;

			if (m_value != value)
			{
				m_value = value;
				
				eventValueChanged.Invoke(this);
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
				Position.X, Position.Y + m_skin->VScrollBarUp.Height, m_skin->VScrollBarBG.Width, Size.Y - m_skin->VScrollBarUp.Height - m_skin->VScrollBarDown.Height);

			sprite->Draw(m_skin->SkinTexture, m_backArea, &m_skin->VScrollBarBG, CV_White);
		}

		void VScrollBar::DrawCursor(Sprite* sprite)
		{
			m_cursorArea.Height = Math::Max(20, m_backArea.Height - Math::Max(20, m_max / 4));
			m_cursorArea.Width = m_skin->VScrollBarCursor->Width;
			m_cursorPos.X = Position.X;

			int32 btnHeight = m_skin->VScrollBarDown.Height;

			if (!m_isScrolling)
			{
				if (!m_inverted)
				{
					m_cursorPos.Y = (int)( m_backArea.Y + (Size.Y - m_cursorArea.Height) * ((float)m_value/(float)m_max));
				}
				else
				{
					m_cursorPos.Y = (int)( m_backArea.Y + (Size.Y - m_cursorArea.Height) * ((float)(m_max-m_value)/(float)m_max));
				}
			}
			m_cursorArea.X = m_cursorPos.X + getOwner()->Position.X;
			m_cursorArea.Y = m_cursorPos.Y + getOwner()->Position.Y;

			Apoc3D::Math::Rectangle dstRect(m_cursorPos.X, m_cursorPos.Y, m_skin->VScrollBarCursor[0].Width, m_skin->VScrollBarCursor[0].Height);
			sprite->Draw(m_skin->SkinTexture, dstRect, &m_skin->VScrollBarCursor[0], CV_White);

			dstRect = Apoc3D::Math::Rectangle(
				m_cursorPos.X, 
				m_cursorPos.Y + m_skin->VScrollBarCursor[0].Height, 
				m_skin->VScrollBarCursor[1].Width,
				m_cursorArea.Height - m_skin->HScrollBarCursor[0].Height - m_skin->HScrollBarCursor[2].Height);
			sprite->Draw(m_skin->SkinTexture, dstRect, &m_skin->VScrollBarCursor[1], CV_White);


			int32 midHeight = dstRect.Height;

			dstRect = Apoc3D::Math::Rectangle(
				m_cursorPos.X, m_cursorPos.Y+midHeight, m_skin->VScrollBarCursor[2].Width, m_skin->VScrollBarCursor[2].Height);
			sprite->Draw(m_skin->SkinTexture, dstRect, &m_skin->VScrollBarCursor[2], CV_White);
		}

		/************************************************************************/
		/* ScrollBar                                                            */
		/************************************************************************/

		ScrollBar::ScrollBar(const Point& position, ScrollBarType type, int size)
			: m_hsbar(0), m_vsbar(0), m_type(type)
		{
			if (m_type == SCRBAR_Horizontal)
			{
				m_hsbar = new HScrollbar(position, size);
			}
			else
			{
				m_vsbar = new VScrollBar(position, size);
			}
		}
		ScrollBar::~ScrollBar()
		{
			if (m_hsbar)
				delete m_hsbar;
			if (m_vsbar)
				delete m_vsbar;
		}

		void ScrollBar::Initialize(RenderDevice* device)
		{
			if (m_hsbar)
			{
				m_hsbar->SetSkin(m_skin);
				m_hsbar->setOwner(getOwner());
				m_hsbar->Initialize(device);
			}
			if (m_vsbar)
			{
				m_vsbar->SetSkin(m_skin);
				m_vsbar->setOwner(getOwner());
				m_vsbar->Initialize(device);
			}

		}
		void ScrollBar::Update(const GameTime* const time)
		{
			if (Visible)
			{
				if (m_hsbar)
				{
					m_hsbar->setOwner(getOwner());
					m_hsbar->Update(time);
				}
				if (m_vsbar)
				{
					m_vsbar->setOwner(getOwner());
					m_vsbar->Update(time);
				}
			}
		}
		void ScrollBar::Draw(Sprite* sprite)
		{
			if (Visible)
			{
				if (m_hsbar)
					m_hsbar->Draw(sprite);
				if (m_vsbar)
					m_vsbar->Draw(sprite);
			}
		}

		void ScrollBar::setPosition(const Point& pos)
		{
			if (m_type == SCRBAR_Horizontal)
			{
				m_hsbar->Position = pos;
				m_hsbar->setPosition(pos);
			}
			else
			{
				m_vsbar->Position = pos;
				m_vsbar->setPosition(pos);
			}
		}
		const Point& ScrollBar::getPosition() const 
		{
			if (m_type == SCRBAR_Horizontal)
				return m_hsbar->Position;
			return m_vsbar->Position;
		}



	}
}