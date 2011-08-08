/*
-----------------------------------------------------------------------------
This source file is part of Apoc3D Engine

Copyright (c) 2009+ Tao Games

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
#include "List.h"
#include "FontManager.h"
#include "StyleSkin.h"
#include "Input/Mouse.h"
#include "Input/InputAPI.h"
#include "Button.h"
#include "Graphics/RenderSystem/Texture.h"
#include "Graphics/RenderSystem/Sprite.h"
#include "Graphics/RenderSystem/RenderDevice.h"
#include "Graphics/RenderSystem/RenderStateManager.h"
#include "Scrollbar.h"
#include "Form.h"

using namespace Apoc3D::Input;

namespace Apoc3D
{
	namespace UI
	{
		ListBox::ListBox(const Point& position, int width, int height, const List<String>& items)
			: Control(position), m_sorted(false), m_isSorted(false), m_visisbleItems(0), m_textOffset(0,0),
			m_selectionRect(0,0,0,0), m_hoverIndex(-1), m_selectedIndex(-1), m_vscrollbar(0), m_hscrollbar(0), m_horizontalScrollbar(false), m_hScrollWidth(0),
			m_mouseOver(false), m_items(items)
		{
			Size = Point(width, height);
		}

		ListBox::~ListBox()
		{
			if(m_hscrollbar)
				delete m_hscrollbar;
			if (m_vscrollbar)
				delete m_vscrollbar;
		}

		void ListBox::Initialize(RenderDevice* device)
		{
			Control::Initialize(device);
			m_textOffset = Point(5, m_skin->TextBox->getHeight()-m_fontRef->getLineHeight()/2);

			m_destRect[0] = Apoc3D::Math::Rectangle(0,0, 
				m_skin->ListBoxSrcRects[0].Width, m_skin->ListBoxSrcRects[0].Height);
			m_destRect[1] = Apoc3D::Math::Rectangle(0,0, 
				Size.X - m_skin->ListBoxSrcRects[0].Width*2, m_skin->ListBoxSrcRects[0].Height);
			m_destRect[2] = Apoc3D::Math::Rectangle(0,0, m_skin->ListBoxSrcRects[0].Width, m_skin->ListBoxSrcRects[0].Height);

			m_destRect[3] = Apoc3D::Math::Rectangle(0,0, 
				m_skin->ListBoxSrcRects[0].Width, Size.Y - m_skin->ListBoxSrcRects[0].Height*2);
			m_destRect[4] = Apoc3D::Math::Rectangle(0,0,
				m_destRect[1].Width, m_destRect[3].Height);
			m_destRect[5] = Apoc3D::Math::Rectangle(0,0,
				m_skin->ListBoxSrcRects[1].Width, m_destRect[3].Height);

			m_destRect[6] = Apoc3D::Math::Rectangle(0,0, m_skin->ListBoxSrcRects[0].Width, m_skin->ListBoxSrcRects[0].Height);
			m_destRect[7] = Apoc3D::Math::Rectangle(0,0, m_destRect[1].Width, m_skin->ListBoxSrcRects[0].Height);
			m_destRect[8] = Apoc3D::Math::Rectangle(0,0, m_skin->ListBoxSrcRects[0].Width, m_skin->ListBoxSrcRects[0].Height);

			m_visisbleItems = (int)ceilf((float)Size.Y / m_fontRef->getLineHeight());
			Size.Y = m_visisbleItems * m_fontRef->getLineHeight();

			UpdateHScrollbar();
			InitScrollbars(device);

			if (m_sorted)
			{
				m_isSorted = true;
			}
		}

		void ListBox::InitScrollbars(RenderDevice* device)
		{
			if (m_vscrollbar)
			{
				delete m_vscrollbar;
				m_vscrollbar = 0;
			}
			if (m_hscrollbar)
			{
				delete m_hscrollbar;
				m_hscrollbar = 0;
			}

			if (getUseHorizontalScrollbar())
			{
				if (m_items.getCount()>m_visisbleItems)
				{
					m_hscrollbar = new ScrollBar(Point(Position.X+1,Position.Y+Size.Y-13),ScrollBar::SCRBAR_Horizontal,Size.X -14);
				}
				else
				{
					m_hscrollbar = new ScrollBar(Point(Position.X+1,Position.Y+Size.Y-13),ScrollBar::SCRBAR_Horizontal,Size.X -2);
				}

				m_hscrollbar->setOwner(getOwner());
				m_hscrollbar->Initialize(device);
			}

			int vScrollbarHeight = 0;
			if (getUseHorizontalScrollbar() && m_hscrollbar->Visible && m_hscrollbar->getMax()>0)
			{
				vScrollbarHeight = Size.Y - 14;
			}
			else
			{
				vScrollbarHeight = Size.Y - 2;
			}

			m_vscrollbar = new ScrollBar(Point(Position.X+Size.X - 13,1),ScrollBar::SCRBAR_Vertical,vScrollbarHeight);
			m_vscrollbar->setOwner(getOwner());
			m_vscrollbar->Initialize(device);
		}
		void ListBox::Update(const GameTime* const time)
		{
			if (m_vscrollbar && m_vscrollbar->Visible)
			{
				m_vscrollbar->Update(time);
			}
			if (m_hscrollbar && m_hscrollbar->Visible)
			{
				m_hscrollbar->Update(time);
			}

			if (!m_isSorted && m_sorted)
			{

			}

			Mouse* mouse = InputAPIManager::getSingleton().getMouse();
			if (UIRoot::getTopMostForm() == getOwner())
			{
				if (m_hoverIndex != -1 && mouse->IsLeftPressed())
				{
					int previousIndex = m_selectedIndex;
					m_selectedIndex = m_hoverIndex;
					if (!m_eSelect.empty())
						m_eSelect(this);
					if (m_selectedIndex != previousIndex && !m_eSelectionChanged.empty())
						m_eSelectionChanged(this);

					if (getAbsoluteArea().Contains(mouse->GetCurrentPosition()))
					{
						if (!m_mouseOver)
						{
							m_mouseOver = true;
							OnMouseOver();
						}

						if (mouse->IsLeftPressed())
							OnPress();
						else if (mouse->IsLeftUp())
							OnRelease();
					}
					else if (m_mouseOver)
					{
						m_mouseOver = false;
						OnMouseOut();
					}
				}
			}
		}

		void ListBox::UpdateHScrollbar()
		{
			int ew = 0;
			for (int i=0;i<m_items.getCount();i++)
			{
				int w;
				if (!m_vscrollbar || m_vscrollbar->getMax()==0)
					w = m_fontRef->MeasureString(m_items[i]).X - Size.X + 12;
				else
					w = m_fontRef->MeasureString(m_items[i]).X - Size.X + 30;

				if (w > ew)
					ew = w;
			}

			m_hScrollWidth = ew;
			if (m_hscrollbar)
				m_hscrollbar->setMax(m_hScrollWidth);
		}

		void ListBox::Draw(Sprite* sprite)
		{
			DrawBackground(sprite);
			
			bool shouldRestoreScissorTest = false;
			Apoc3D::Math::Rectangle oldScissorRect;
			RenderDevice* dev = sprite->getRenderDevice();
			if (dev->getRenderState()->getScissorTestEnabled())
			{
				shouldRestoreScissorTest = true;
				oldScissorRect = dev->getRenderState()->getScissorTestRect();
			}

			m_hoverIndex = -1;
			for (int i=m_vscrollbar->getValue();
				i<min(m_items.getCount(), m_vscrollbar->getValue()+m_visisbleItems);i++)
			{
				if (m_hscrollbar)
					m_textOffset.X = -m_hscrollbar->getValue() + 2;
				else
					m_textOffset.X = 2;

				m_textOffset.Y = (i - m_vscrollbar->getValue()) * m_fontRef->getLineHeight();
				if (UIRoot::getTopMostForm() == getOwner())
					RenderSelectionBox(sprite,i);

				m_fontRef->DrawString(sprite, m_items[i], m_textOffset, m_skin->ForeColor);
			}

			if (shouldRestoreScissorTest)
			{
				dev->getRenderState()->setScissorTest(true,&oldScissorRect);
			}
			else
			{
				dev->getRenderState()->setScissorTest(false,0);
			}

			UpdateHScrollbar();
			DrawScrollbar(sprite);
		}

		
		void ListBox::RenderSelectionBox(Sprite* sprite, int index)
		{
			Mouse* mouse = InputAPIManager::getSingleton().getMouse();

			m_selectionRect.X = 0;
			m_selectionRect.Y = m_textOffset.Y;
			m_selectionRect.Height = m_fontRef->getLineHeight();

			if (m_vscrollbar->getMax()>0)
				m_selectionRect.Width = Size.X - 13;
			else
				m_selectionRect.Width = Size.X - 1;

			if (index == m_selectedIndex)
				sprite->Draw(m_skin->WhitePixelTexture, m_selectionRect, CV_LightGray);
			else if (getOwner()->getArea().Contains(mouse->GetCurrentPosition()) &&
				m_selectionRect.Contains(Point(mouse->GetCurrentPosition().X-getOwner()->Position.X-Position.X, 
				mouse->GetCurrentPosition().Y-getOwner()->Position.Y-Position.Y)))
			{
				m_hoverIndex = index;
				sprite->Draw(m_skin->WhitePixelTexture, m_selectionRect, CV_Silver);
			}
				
		}
		void ListBox::DrawBackground(Sprite* sprite)
		{
			m_destRect[0].X = Position.X;
			m_destRect[0].Y = Position.Y;
			sprite->Draw(m_skin->TextBox, m_destRect[0], &m_skin->ListBoxSrcRects[0], m_skin->BackColor);
			m_destRect[1].X = m_destRect[0].X + m_destRect[0].Width;
			m_destRect[1].Y = m_destRect[0].Y;
			sprite->Draw(m_skin->TextBox, m_destRect[1], &m_skin->ListBoxSrcRects[1], m_skin->BackColor);
			m_destRect[2].X = m_destRect[1].X + m_destRect[1].Width;
			m_destRect[2].Y = m_destRect[0].Y;
			sprite->Draw(m_skin->TextBox, m_destRect[2], &m_skin->ListBoxSrcRects[2], m_skin->BackColor);

			m_destRect[3].X = m_destRect[0].X;
			m_destRect[3].Y = m_destRect[0].Y + m_destRect[0].Height;
			sprite->Draw(m_skin->TextBox, m_destRect[3], &m_skin->ListBoxSrcRects[3], m_skin->BackColor);
			m_destRect[4].X = m_destRect[1].X;
			m_destRect[4].Y = m_destRect[0].Y + m_destRect[0].Height;
			sprite->Draw(m_skin->TextBox, m_destRect[4], &m_skin->ListBoxSrcRects[4], m_skin->BackColor);
			m_destRect[5].X = m_destRect[2].X;
			m_destRect[5].Y = m_destRect[0].Y + m_destRect[0].Height;
			sprite->Draw(m_skin->TextBox, m_destRect[5], &m_skin->ListBoxSrcRects[5], m_skin->BackColor);

			m_destRect[6].X = m_destRect[0].X;
			m_destRect[6].Y = m_destRect[3].Y + m_destRect[3].Height;
			sprite->Draw(m_skin->TextBox, m_destRect[6], &m_skin->ListBoxSrcRects[6], m_skin->BackColor);
			m_destRect[7].X = m_destRect[1].X;
			m_destRect[7].Y = m_destRect[4].Y + m_destRect[4].Height;
			sprite->Draw(m_skin->TextBox, m_destRect[7], &m_skin->ListBoxSrcRects[7], m_skin->BackColor);
			m_destRect[8].X = m_destRect[2].X;
			m_destRect[8].Y = m_destRect[5].Y + m_destRect[5].Height;
			sprite->Draw(m_skin->TextBox, m_destRect[8], &m_skin->ListBoxSrcRects[8], m_skin->BackColor);
		}
		void ListBox::DrawScrollbar(Sprite* sprite)
		{
			m_vscrollbar->setMax(max(0, m_items.getCount()-m_visisbleItems));
			if (m_vscrollbar->getValue()>m_vscrollbar->getMax())
				m_vscrollbar->setValue(m_vscrollbar->getMax());

			if (m_vscrollbar->getMax()>0)
			{
				m_vscrollbar->Draw(sprite);
				if (m_vscrollbar)
				{
					m_vscrollbar->setWidth( Size.X - 14);
					UpdateHScrollbar();
				}
			}
			else if (m_hscrollbar)
			{
				m_hscrollbar->setWidth(Size.X-2);
				UpdateHScrollbar();
			}

			if (getUseHorizontalScrollbar() && m_hscrollbar && m_hscrollbar->getMax()>0)
			{
				m_vscrollbar->setHeight(Size.Y - 12);
				m_hscrollbar->setMax(m_hScrollWidth);
				m_hscrollbar->Draw(sprite);
				m_visisbleItems = (int)ceilf((float)Size.Y / m_fontRef->getLineHeight())-1;
			}
			else if ((!m_hscrollbar || !m_hscrollbar->getMax()) && m_vscrollbar->Size.Y != Size.Y-2)
			{
				m_vscrollbar->setHeight(Size.Y - 2);
				m_visisbleItems++;
			}
		}
	}
}