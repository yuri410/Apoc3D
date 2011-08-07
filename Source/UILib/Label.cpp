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

#include "Label.h"
#include "Utility/StringUtils.h"
#include "Input/InputAPI.h"
#include "Input/Mouse.h"
#include "FontManager.h"
#include "Graphics/RenderSystem/Sprite.h"
#include "StyleSkin.h"

using namespace Apoc3D::Utility;
using namespace Apoc3D::Input;

namespace Apoc3D
{
	namespace UI
	{
		Label::Label(const Point& position, const String& text, int width, Align alignment)
			: Control(position, text), m_alignment(alignment), m_width(width)
		{
			Size.X = width;
			
		}

		void Label::Initialize(RenderDevice* device)
		{
			Control::Initialize(device);
			UpdateText();
			if (m_lines.getCount())
			{
				Size.Y = m_fontRef->getLineHeight() * m_lines.getCount();
			}
			else
			{
				Size.Y = m_fontRef->getLineHeight();
			}

			m_backgroundRect.Width = Size.X;
			m_backgroundRect.Height = Size.Y;
		}

		void Label::UpdateText()
		{
			std::vector<String> lines = StringUtils::Split(Text, L"\n\r");
			m_lines.Clear();
			for (size_t i=0;i<lines.size();i++)
			{
				m_lines.Add(lines[i]);
			}

			for (int i=0;i<m_lines.getCount();i++)
			{
				if (m_lines[i].length())
				{
					for (size_t c=1;c<m_lines[i].length()+1;c++)
					{
						if (m_fontRef->MeasureString(lines[i].substr(0,c)).X > Size.X)
						{
							String newLine = m_lines[i].substr(c-1, m_lines[i].length() - (c-1));
							m_lines[i] = m_lines[i].substr(0,c-1);
							m_lines.Insert(i+1, newLine);
							break;
						}
					}
				}
			}
		}

		void Label::Update(const GameTime* const time)
		{
			UpdateEvents();
		}

		void Label::UpdateEvents()
		{
			Mouse* mouse = InputAPIManager::getSingleton().getMouse();

			Apoc3D::Math::Rectangle area = getAbsoluteArea();
			if (area.Contains(mouse->GetCurrentPosition()) &&
				getOwner()->getAbsoluteArea().Contains(area))
			{
				if (!m_mouseOver)
				{
					m_mouseOver = true;
					OnMouseOver();
				}

				if (!m_mouseDown && mouse->IsLeftPressed())
				{
					m_mouseDown = true;
					OnPress();
				}
				else if (m_mouseDown && mouse->IsLeftUp())
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

		void Label::Draw(Sprite* sprite)
		{
			Apoc3D::Math::Rectangle area = getArea();

			m_backgroundRect.X = area.X - getOwner()->Position.X;
			m_backgroundRect.Y = area.X - getOwner()->Position.Y;
			sprite->Draw(m_skin->WhitePixelTexture, m_backgroundRect, m_skin->BackColor);


			if (m_lines.getCount()<=1)
			{
				Point txtSize = m_fontRef->MeasureString(Text);
				switch (m_alignment)
				{
				case ALIGN_Right:
					m_textOffset.X = Size.X - txtSize.X;
					break;
				case ALIGN_Center:
					m_textOffset.X = (Size.X - txtSize.X)/2;
					break;
				}

				m_drawPos = Point(Position.X + m_textOffset.X, Position.Y);
				m_fontRef->DrawString(sprite, Text, m_drawPos, m_skin->ForeColor);
			}
			else
			{
				for (int i=0;i<m_lines.getCount();i++)
				{
					Point txtSize = m_fontRef->MeasureString(Text);
					switch (m_alignment)
					{
					case ALIGN_Right:
						m_drawPos.X = Position.X + Size.X - txtSize.X;
						break;
					case ALIGN_Center:
						m_drawPos.X = Position.X + (Size.X - txtSize.X)/2;
						break;
					case ALIGN_Left:
						m_drawPos.X = Position.X;
						break;
					}
					m_drawPos.Y = Position.Y + i * m_fontRef->getLineHeight();
					m_fontRef->DrawString(sprite, m_lines[i], m_drawPos, m_skin->ForeColor);
				}
			}
		}
	}
}