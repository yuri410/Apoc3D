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

#include "Label.h"
#include "StyleSkin.h"
#include "Scrollbar.h"
#include "Form.h"
#include "FontManager.h"

#include "apoc3d/Utility/StringUtils.h"
#include "apoc3d/Input/InputAPI.h"
#include "apoc3d/Input/Mouse.h"
#include "apoc3d/Core/GameTime.h"
#include "apoc3d/Graphics/RenderSystem/Sprite.h"
#include "apoc3d/Graphics/RenderSystem/Texture.h"
#include "apoc3d/Graphics/RenderSystem/RenderDevice.h"
#include "apoc3d/Graphics/RenderSystem/RenderStateManager.h"

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
		Label::~Label()
		{

		}
		void Label::Initialize(RenderDevice* device)
		{
			Control::Initialize(device);
			UpdateText();

			Size.Y = static_cast<int>( m_fontRef->getTextBackgroundHeight(m_lines.getCount()) );

			//if (m_lines.getCount())
			//{
			//	Size.Y = m_fontRef->getLineHeightInt() * m_lines.getCount();
			//}
			//else
			//{
			//	Size.Y = m_fontRef->getLineHeightInt();
			//}

			m_backgroundRect.Width = Size.X;
			m_backgroundRect.Height = Size.Y;
		}

		void Label::UpdateText()
		{
			List<String> lines;
			StringUtils::Split(Text, lines, L"\n\r");
			m_lines.Clear();
			for (int32 i=0;i<lines.getCount();i++)
			{
				m_lines.Add(lines[i]);
			}

			for (int i=0;i<m_lines.getCount();i++)
			{
				if (m_lines[i].length())
				{
					for (size_t c=1;c<m_lines[i].length()+1;c++)
					{
						if (m_fontRef->MeasureString(m_lines[i].substr(0,c)).X > Size.X)
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
			//Apoc3D::Math::Rectangle area = getAbsoluteArea();

			//m_backgroundRect.X = area.X - getOwner()->Position.X;
			//m_backgroundRect.Y = area.X - getOwner()->Position.Y;
			//sprite->Draw(m_skin->WhitePixelTexture, area, m_skin->BackColor);


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
					m_drawPos.Y = Position.Y + i * m_fontRef->getLineHeightInt();
					m_fontRef->DrawString(sprite, m_lines[i], m_drawPos, m_skin->ForeColor);
				}
			}
		}

		/************************************************************************/
		/*                                                                      */
		/************************************************************************/

		TextBox::TextBox(const Point& position, int width)
			: Control(position), m_curorLocation(0,0), m_previousLocation(0,0), m_cursorOffset(0,0), m_scrollOffset(0,0), m_hasFocus(false),
			m_multiline(false), m_lineOffset(0,0), m_visibleLines(0), m_locked(false), m_scrollBar(SBT_None), m_vscrollBar(0), m_hscrollBar(0),
			m_cursorVisible(false), m_timer(0.5f), m_timerStarted(false)
		{
			Size.X = width;
		}
		TextBox::TextBox(const Point& position, int width, const String& text)
			: Control(position), m_curorLocation(0,0), m_previousLocation(0,0), m_cursorOffset(0,0), m_scrollOffset(0,0), m_hasFocus(false),
			m_multiline(false), m_lineOffset(0,0), m_visibleLines(0), m_locked(false), m_scrollBar(SBT_None), m_vscrollBar(0), m_hscrollBar(0),
			m_cursorVisible(false), m_timer(0.5f), m_timerStarted(false)
		{
			Size.X = width;
			Add(text);
			m_curorLocation.X = Text.length();
		}
		TextBox::TextBox(const Point& position, int width, int height, const String& text)
			: Control(position, L"", Point(width,height)), m_curorLocation(0,0), m_previousLocation(0,0), m_cursorOffset(0,0), m_scrollOffset(0,0), m_hasFocus(false),
			m_multiline(true), m_lineOffset(0,0), m_visibleLines(0), m_locked(false), m_scrollBar(SBT_None), m_vscrollBar(0), m_hscrollBar(0),
			m_cursorVisible(false), m_timer(0.5f), m_timerStarted(false)
		{
			Size.X = width;
			Add(text);
			m_curorLocation.Y = m_lines.getCount()-1;
			m_curorLocation.X = (int)m_lines[m_lines.getCount()-1].size();
		}
		TextBox::~TextBox()
		{
			if (m_vscrollBar)
				delete m_vscrollBar;
			if (m_hscrollBar)
				delete m_hscrollBar;
		}
		void TextBox::Add(const String& text)
		{
			if (!m_multiline)
			{
				if (m_curorLocation.X > static_cast<int>(Text.size()))
					m_curorLocation.X = static_cast<int>(Text.size());

				Text = Text.insert(m_curorLocation.X, text);
			}
			else
			{
				if (text.find_first_of('\n',0)!=String::npos)
				{
					List<String> lines;
					StringUtils::Split(text, lines, L"\n");
					for (int32 i=0;i<lines.getCount();i++)
					{
						for (size_t j=0;j<lines[i].size();j++)
						{
							if (lines[i][j] == '\t')
							{
								lines[i][j] = ' ';
								lines[i].insert(j, L"   ");
							}
						}
						//wchar_t tab = '\t';
						//lines[i] = m_lines[i].replace()
						if (i==0)
						{
							if (m_lines.getCount() == 0)
								m_lines.Add(L"");

							if (m_curorLocation.X > (int)m_lines[m_curorLocation.Y].size())
								m_curorLocation.X = (int)m_lines[m_curorLocation.Y].size();

							m_lines[m_curorLocation.Y] = m_lines[m_curorLocation.Y].insert(m_curorLocation.X, text);
						}
						else
						{
							m_lines.Insert(m_curorLocation.Y + i, lines[i]);
						}
					}
				}
				else
				{
					if (m_lines.getCount()==0)
						m_lines.Add(L"");

					if (m_curorLocation.X > (int)m_lines[m_curorLocation.Y].size())
						m_curorLocation.X = (int)m_lines[m_curorLocation.Y].size();

					m_lines[m_curorLocation.Y] = m_lines[m_curorLocation.Y].insert(m_curorLocation.X, text);
				}

				for (int i=0;i<m_lines.getCount();i++)
					Text.append(m_lines[i]);
			}
			UpdateScrolling();
		}
		
		void TextBox::UpdateScrolling()
		{
			if (m_cursorOffset.X > m_scrollOffset.X + m_sRect.Width - 20)
				m_scrollOffset.X = m_cursorOffset.X - (m_sRect.Width - 20);
			else if (m_cursorOffset.X - 20 < m_scrollOffset.X)
				m_scrollOffset.X = std::max(0, m_cursorOffset.X - 20);

			if (m_scrollOffset.X < 0)
				m_scrollOffset.X = 0;
			if (m_curorLocation.X == 0)
				m_scrollOffset.X = 0;

			if (m_multiline)
			{
				if (m_hscrollBar)
					m_hscrollBar->setValue(m_scrollOffset.X);

				if (m_fontRef)
				{
					int offsetY = m_scrollOffset.Y / m_fontRef->getLineHeightInt();
					if (m_hscrollBar && m_hscrollBar->getMax()>0)
					{
						if (m_curorLocation.Y > offsetY + m_visibleLines -2)
							m_scrollOffset.Y = (m_curorLocation.Y - (m_visibleLines - 2)) * m_fontRef->getLineHeightInt();
						else if (m_curorLocation.Y < offsetY)
							m_scrollOffset.Y = m_curorLocation.Y * m_fontRef->getLineHeightInt();
					}
					else
					{
						if (m_curorLocation.Y > offsetY + m_visibleLines -1)
							m_scrollOffset.Y = (m_curorLocation.Y - (m_visibleLines - 1)) * m_fontRef->getLineHeightInt();
						else if (m_curorLocation.Y < offsetY)
							m_scrollOffset.Y = m_curorLocation.Y * m_fontRef->getLineHeightInt();
					}

					if (m_vscrollBar)
						m_vscrollBar->setValue(m_scrollOffset.Y / m_fontRef->getLineHeightInt());
				}
				
			}
		}

		void TextBox::Initialize(RenderDevice* device)
		{
			Control::Initialize(device);
			Texture* texture = m_skin->TextBox;

			m_textOffset = Point(5, static_cast<int>((texture->getHeight() - m_fontRef->getLineBackgroundHeight()) /2 ));

			InitDstRect();

			m_dRect = Apoc3D::Math::Rectangle(0,0,Size.X,Size.Y);

			m_keyboard.eventKeyPress().Bind(this, &TextBox::Keyboard_OnPress);
			m_keyboard.eventKeyPaste().Bind(this, &TextBox::Keyboard_OnPaste);

			if (m_multiline && m_scrollBar != SBT_None)
			{
				InitScrollbars(device);
			}
			m_timerStarted = true;
		}

		void TextBox::InitDstRect()
		{
			Texture* texture = m_skin->TextBox;
			if (!m_multiline)
			{
				m_destRect[0] = Apoc3D::Math::Rectangle(0,0,m_skin->TextBoxSrcRectsSingle[0].Width, m_skin->TextBoxSrcRectsSingle[0].Height);
				m_destRect[1] = Apoc3D::Math::Rectangle(0,0,Size.X - m_skin->TextBoxSrcRectsSingle[0].Width * 2, m_skin->TextBoxSrcRectsSingle[1].Height);
				m_destRect[2] = m_destRect[0];
				Size.Y = texture->getHeight();
			}
			else
			{
				//if (Size.Y == 0)
				{
					m_visibleLines = (int)ceilf((float)Size.Y / m_fontRef->getLineHeightInt());

					Size.Y = m_visibleLines * m_fontRef->getLineHeightInt() + 2;
				}
				//else
				//{
				//	m_visibleLines = static_cast<int32>(0.5f + Size.Y / m_fontRef->getLineHeight());

				//	if (m_visibleLines == 0)
				//	{
				//		m_visibleLines = 1;
				//	}
				//}
				
				m_destRect[0] = Apoc3D::Math::Rectangle(0,0, 
					m_skin->TextBoxSrcRects[0].Width, m_skin->TextBoxSrcRects[0].Height);
				m_destRect[1] = Apoc3D::Math::Rectangle(0,0, 
					Size.X - m_skin->TextBoxSrcRects[0].Width*2, m_skin->TextBoxSrcRects[0].Height);
				m_destRect[2] = Apoc3D::Math::Rectangle(0,0, m_skin->TextBoxSrcRects[0].Width, m_skin->TextBoxSrcRects[0].Height);

				m_destRect[3] = Apoc3D::Math::Rectangle(0,0, 
					m_skin->TextBoxSrcRects[0].Width, Size.Y - m_skin->TextBoxSrcRects[0].Height*2);
				m_destRect[4] = Apoc3D::Math::Rectangle(0,0,
					m_destRect[1].Width, m_destRect[3].Height);
				m_destRect[5] = Apoc3D::Math::Rectangle(0,0,
					m_skin->TextBoxSrcRects[0].Width, m_destRect[3].Height);

				m_destRect[6] = Apoc3D::Math::Rectangle(0,0, m_skin->TextBoxSrcRects[0].Width, m_skin->TextBoxSrcRects[0].Height);
				m_destRect[7] = Apoc3D::Math::Rectangle(0,0, m_destRect[1].Width, m_skin->TextBoxSrcRects[0].Height);
				m_destRect[8] = Apoc3D::Math::Rectangle(0,0, m_skin->TextBoxSrcRects[0].Width, m_skin->TextBoxSrcRects[0].Height);

			}
		}
		void TextBox::InitScrollbars(RenderDevice* device)
		{
			if (m_scrollBar == SBT_Vertical)
			{
				Point pos = Position;
				pos.X += Size.X - 2;
				pos.Y++;
				m_vscrollBar = new ScrollBar(pos, ScrollBar::SCRBAR_Vertical, Size.Y - 2);
			}
			else if (m_scrollBar == SBT_Horizontal)
			{
				Point pos = Position;
				pos.X++;
				pos.Y += Size.Y - 2;
				m_hscrollBar = new ScrollBar(pos, ScrollBar::SCRBAR_Horizontal, Size.X - 2);
			}
			else if (m_scrollBar == SBT_Both)
			{
				Point pos = Position;
				pos.X += Size.X - 13;
				pos.Y++;

				m_vscrollBar = new ScrollBar(pos, ScrollBar::SCRBAR_Vertical, Size.Y - 14);

				pos = Position;
				pos.X++;
				pos.Y += Size.Y - 13;
				m_hscrollBar = new ScrollBar(pos, ScrollBar::SCRBAR_Horizontal, Size.X - 14);
			}

			if (m_vscrollBar)
			{
				m_vscrollBar->SetSkin(m_skin);
				m_vscrollBar->setOwner(getOwner());
				m_vscrollBar->eventValueChanged().Bind(this, &TextBox::vScrollbar_OnChangeValue);
				m_vscrollBar->Initialize(device);
			}
			if (m_hscrollBar)
			{
				m_hscrollBar->SetSkin(m_skin);
				m_hscrollBar->setOwner(getOwner());
				m_hscrollBar->eventValueChanged().Bind(this, &TextBox::hScrollbar_OnChangeValue);
				m_hscrollBar->Initialize(device);
			}
		}
		void TextBox::Update(const GameTime* const time)
		{
			Control::Update(time);

			CheckFocus();

			if (m_hasFocus && !m_locked)
			{
				m_keyboard.Update(time);

				if (m_multiline && m_scrollBar == SBT_Vertical &&
					m_curorLocation.X > (int)m_lines[m_curorLocation.Y].size())
					m_curorLocation.X = (int)m_lines[m_curorLocation.Y].size();
			}

			if (m_multiline && m_scrollBar != SBT_None)
			{
				UpdateScrollbars(time);
			}

			if (m_timerStarted)
			{
				m_timer-= time->getElapsedTime();
				if (m_timer<0)
				{
					m_timer = 0.5f;
					m_cursorVisible = !m_cursorVisible;
				}
			}
		}
		void TextBox::UpdateScrollbars(const GameTime* const time)
		{
 			Mouse* mouse = InputAPIManager::getSingleton().getMouse();
			if (m_vscrollBar)
			{
				if (m_hscrollBar && m_hscrollBar->getMax()>0)
					m_vscrollBar->setMax(std::max(0, m_lines.getCount() - (m_visibleLines-1)));
				else
					m_vscrollBar->setMax(std::max(0, m_lines.getCount() - m_visibleLines));
			}

			if (m_vscrollBar && m_vscrollBar->getMax()>0)
			{
				if (m_hscrollBar && m_hscrollBar->getMax()>0)
					m_vscrollBar->setHeight(Size.Y - 13);
				else
					m_vscrollBar->setHeight(Size.Y - 2);

				m_vscrollBar->Update(time);

				if (mouse->getDZ() && m_dRect.Contains(mouse->GetCurrentPosition()))
				{
					m_vscrollBar->setValue(Math::Clamp(m_vscrollBar->getValue() + mouse->getDZ() / 60, 0, m_vscrollBar->getMax()));
				}
			}

			if (m_hscrollBar && m_hscrollBar->getMax()>0)
			{
				if (m_vscrollBar && m_vscrollBar->getMax()>0)
					m_hscrollBar->setWidth(Size.X - 13);
				else
					m_hscrollBar->setWidth(Size.X - 2);

				m_hscrollBar->Update(time);
			}
		}
		void TextBox::CheckFocus()
		{
			m_sRect = getArea();

			Mouse* mouse = InputAPIManager::getSingleton().getMouse();
			if (mouse->IsLeftPressed())
			{
				if (m_dRect.Contains(mouse->GetCurrentPosition()))
				{
					m_hasFocus = true;
				}
				else
				{
					m_hasFocus = false;
				}
			}

		}
		void TextBox::Draw(Sprite* sprite)
		{
			if (!m_multiline)
			{
				DrawMonoline(sprite);
			}
			else
			{
				DrawMultiline(sprite);
			}

			if (m_vscrollBar && m_vscrollBar->getMax()>0)
				m_sRect.Width = Size.X - 14;
			else
				m_sRect.Width = Size.X - 2;

			if (m_hscrollBar && m_hscrollBar->getMax()>0)
				m_sRect.Height = Size.Y - 14;
			else
				m_sRect.Height = Size.Y - 2;

			m_dRect = getAbsoluteArea();

			RenderStateManager* stMgr = sprite->getRenderDevice()->getRenderState();
			bool oldScissorTest = stMgr->getScissorTestEnabled();
			Apoc3D::Math::Rectangle oldScissorRect;
			if (oldScissorTest)
			{
				oldScissorRect = stMgr->getScissorTestRect();
			}

			sprite->Flush();
			stMgr->setScissorTest(true, &m_dRect);
			_DrawText(sprite);
			sprite->Flush();
			stMgr->setScissorTest(oldScissorTest, &oldScissorRect);
			
			if (m_vscrollBar && m_vscrollBar->getMax()>0)
			{
				m_vscrollBar->Draw(sprite);
			}
			if (m_hscrollBar && m_hscrollBar->getMax()>0)
			{
				m_hscrollBar->Draw(sprite);
			}
		}
		void TextBox::DrawMonoline(Sprite* sprite)
		{
			Texture* texture = m_skin->TextBox;
			m_destRect[0].X = (int)Position.X;
			m_destRect[0].Y = (int)Position.Y;
			sprite->Draw(texture, m_destRect[0], &m_skin->TextBoxSrcRectsSingle[0], CV_White);

			m_destRect[1].X = m_destRect[0].X + m_destRect[0].Width;
			m_destRect[1].Y = m_destRect[0].Y;
			sprite->Draw(texture, m_destRect[1], &m_skin->TextBoxSrcRectsSingle[1], CV_White);

			m_destRect[2].X = m_destRect[1].X + m_destRect[1].Width;
			m_destRect[2].Y = m_destRect[0].Y;
			sprite->Draw(texture, m_destRect[2], &m_skin->TextBoxSrcRectsSingle[2], CV_White);
		}
		void TextBox::DrawMultiline(Sprite* sprite)
		{
			Texture* texture = m_skin->TextBox;
			m_destRect[0].X = (int)Position.X;
			m_destRect[0].Y = (int)Position.Y;
			sprite->Draw(texture, m_destRect[0], &m_skin->TextBoxSrcRects[0], CV_White);
			m_destRect[1].X = m_destRect[0].X + m_destRect[0].Width;
			m_destRect[1].Y = m_destRect[0].Y;
			sprite->Draw(texture, m_destRect[1], &m_skin->TextBoxSrcRects[1], CV_White);
			m_destRect[2].X = m_destRect[1].X + m_destRect[1].Width;
			m_destRect[2].Y = m_destRect[0].Y;
			sprite->Draw(texture, m_destRect[2], &m_skin->TextBoxSrcRects[2], CV_White);

			m_destRect[3].X = m_destRect[0].X;
			m_destRect[3].Y = m_destRect[0].Y + m_destRect[0].Height;
			sprite->Draw(texture, m_destRect[3], &m_skin->TextBoxSrcRects[3], CV_White);
			m_destRect[4].X = m_destRect[1].X;
			m_destRect[4].Y = m_destRect[0].Y + m_destRect[0].Height;
			sprite->Draw(texture, m_destRect[4], &m_skin->TextBoxSrcRects[4], CV_White);
			m_destRect[5].X = m_destRect[2].X;
			m_destRect[5].Y = m_destRect[0].Y + m_destRect[0].Height;
			sprite->Draw(texture, m_destRect[5], &m_skin->TextBoxSrcRects[5], CV_White);

			m_destRect[6].X = m_destRect[0].X;
			m_destRect[6].Y = m_destRect[3].Y + m_destRect[3].Height;
			sprite->Draw(texture, m_destRect[6], &m_skin->TextBoxSrcRects[6], CV_White);
			m_destRect[7].X = m_destRect[1].X;
			m_destRect[7].Y = m_destRect[4].Y + m_destRect[4].Height;
			sprite->Draw(texture, m_destRect[7], &m_skin->TextBoxSrcRects[7], CV_White);
			m_destRect[8].X = m_destRect[2].X;
			m_destRect[8].Y = m_destRect[5].Y + m_destRect[5].Height;
			sprite->Draw(texture, m_destRect[8], &m_skin->TextBoxSrcRects[8], CV_White);
		}
		void TextBox::_DrawText(Sprite* sprite)
		{
			Point baseOffset = Point(m_sRect.X, m_sRect.Y);
			
			int32 cursorLeft = m_fontRef->MeasureString(L"|").X/2;
			//if (m_curorLocation.X==0)
			//	cursorLeft=1;//-cursorLeft;

			//baseOffset.X += m_sRect.X; baseOffset.X += m_sRect.Y;
			if (!m_multiline)
			{
				if (m_curorLocation.X>0)
				{
					m_cursorOffset.X = m_fontRef->MeasureString(
						Text.substr(0, m_curorLocation.X)).X+cursorLeft;
				}
				else
				{
					m_cursorOffset.X = cursorLeft;
				}
				
				m_cursorOffset.Y = m_textOffset.Y;

				//Point pos(m_textOffset.X - m_scrollOffset.X, m_textOffset.Y - m_scrollOffset.Y);
				m_fontRef->DrawString(sprite, Text, m_textOffset-m_scrollOffset + baseOffset, CV_Black);
				if (m_hasFocus && !m_locked && m_cursorVisible && getOwner()==UIRoot::getTopMostForm())
				{
					m_fontRef->DrawString(sprite, L"|", m_cursorOffset - m_scrollOffset + baseOffset, CV_Black);
				}
			}
			else
			{
				int maxWidth = 0;
				for (int i=0;i<m_lines.getCount();i++)
				{
					Point lineSize = m_fontRef->MeasureString(m_lines[i]);

					m_lineOffset.Y = i*m_fontRef->getLineHeightInt();

					m_fontRef->DrawString(sprite, m_lines[i], m_textOffset+m_lineOffset-m_scrollOffset + baseOffset, CV_Black);

					if (lineSize.X -Size.X > maxWidth)
					{
						if (m_vscrollBar && m_vscrollBar->getMax()>0)
						{
							maxWidth = lineSize.X - Size.X;
						}
						else
						{
							maxWidth = lineSize.X - Size.X+12;
						}
					}
				}

				if (m_hscrollBar)
				{
					m_hscrollBar->setMax(maxWidth);

					if (m_vscrollBar && m_vscrollBar->getMax()>0 && m_hscrollBar->getMax()>0)
						m_hscrollBar->setMax(m_hscrollBar->getMax()+20);
					m_hscrollBar->setStep(std::max(1, m_hscrollBar->getMax()/15));
				}
				if (m_hasFocus && !m_locked && m_cursorVisible && getOwner() == UIRoot::getTopMostForm())
				{
					if (m_curorLocation.X > (int)m_lines[m_curorLocation.Y].size())
						m_curorLocation.X = (int)m_lines[m_curorLocation.Y].size();

					m_cursorOffset.X = m_fontRef->MeasureString(
						m_lines[m_curorLocation.Y].substr(0, m_curorLocation.X>0 ? m_curorLocation.X-1 : 0)).X+cursorLeft;
					m_cursorOffset.Y = m_fontRef->getLineHeightInt() * m_curorLocation.Y + 1;

					m_fontRef->DrawString(sprite, L"|", m_cursorOffset - m_scrollOffset + baseOffset, CV_Black);
				}
			}
		}
		void TextBox::setText(const String& text)
		{
			Text = L"";
			m_lines.Clear();
			m_curorLocation.X = 0;
			m_curorLocation.Y = 0;
			Add(text);
		}

		void TextBox::Keyboard_OnPress(KeyboardKeyCode code, KeyboardEventsArgs e)
		{
			bool changed = false;
			switch(code)
			{
			case KEY_LEFT:
				{
					if (e.ControlDown)
					{
						bool foundSpace = false;
						if (m_lines.getCount() > 0)
						{
							for (int i = m_curorLocation.X - 2; i > 0; i--)
								if (m_lines[m_curorLocation.Y].substr(i, 1) == L" ")
								{
									m_curorLocation.X = i + 1;
									foundSpace = true;
									break;
								}
						}

						if (!foundSpace)
							if (m_curorLocation.X != 0)
								m_curorLocation.X = 0;
							else if (m_curorLocation.Y > 0)
							{
								m_curorLocation.Y -= 1;
								m_curorLocation.X = (int)m_lines[m_curorLocation.Y].size();
							}
					}
					else
					{
						if (!m_multiline && m_curorLocation.X > 0)
							m_curorLocation.X -= 1;
						else if (m_multiline)
						{
							if (m_curorLocation.X > 0)
								m_curorLocation.X -= 1;
							else if (m_curorLocation.Y > 0)
							{
								m_curorLocation.X = (int)m_lines[m_curorLocation.Y - 1].size();
								m_curorLocation.Y -= 1;
							}
						}
					}
				}
				break;
			case KEY_RIGHT:
				{
					if (e.ControlDown)
					{
						bool foundSpace = false;
						if (m_lines.getCount() > 0)
						{
							for (int i = m_curorLocation.X; i < (int)m_lines[m_curorLocation.Y].size(); i++)
								if (m_lines[m_curorLocation.Y].substr(i, 1) == L" ")
								{
									m_curorLocation.X = i + 1;
									foundSpace = true;
									break;
								}

								if (!foundSpace)
									if (m_curorLocation.X != (int)m_lines[m_curorLocation.Y].size())
										m_curorLocation.X = (int)m_lines[m_curorLocation.Y].size();
									else if (m_curorLocation.Y < m_lines.getCount() - 1)
									{
										m_curorLocation.X = 0;
										m_curorLocation.Y += 1;
									}
						}
					}
					else
					{
						if (!m_multiline && m_curorLocation.X < (int)Text.size())
							m_curorLocation.X += 1;
						else if (m_multiline)
						{
							if (m_curorLocation.X < (int)m_lines[m_curorLocation.Y].size())
								m_curorLocation.X += 1;
							else if (m_curorLocation.Y < m_lines.getCount() - 1)
							{
								m_curorLocation.X = 0;
								m_curorLocation.Y += 1;
							}
						}
					}
				}
				break;
			case KEY_UP:
				if (!m_multiline)
				{
					eventUpPressedSingleline.Invoke(this);
				}
				if (m_curorLocation.Y > 0)
					m_curorLocation.Y--;
				break;
			case KEY_DOWN:
				if (!m_multiline)
				{
					eventDownPressedSingleline.Invoke(this);
				}
				if (m_curorLocation.Y < m_lines.getCount()-1)
					m_curorLocation.Y++;
				break;
			case KEY_BACK:
				{
					if (!m_multiline && m_curorLocation.X > 0)
					{
						if (m_curorLocation.X > 1 && Text.substr(m_curorLocation.X - 2, 2) == L"\n")
						{
							Text = Text.erase(m_curorLocation.X - 2, 2);
							m_curorLocation.X -= 2;
						}
						else
						{
							Text = Text.erase(m_curorLocation.X - 1, 1);
							m_curorLocation.X -= 1;
						}
						changed = true;
					}
					else if (m_multiline)
					{
						if (m_curorLocation.X > 0)
						{
							m_lines[m_curorLocation.Y] = m_lines[m_curorLocation.Y].erase(m_curorLocation.X - 1, 1);
							m_curorLocation.X -= 1;

							changed = true;
						}
						else
						{
							if (m_curorLocation.Y > 0)
							{
								m_curorLocation.X = (int)m_lines[m_curorLocation.Y - 1].size();
								m_lines[m_curorLocation.Y - 1] += m_lines[m_curorLocation.Y];
								m_lines.RemoveAt(m_curorLocation.Y);
								m_curorLocation.Y -= 1;

								changed = true;
							}
						}
					}
				}
				break;
			case KEY_DELETE:
				{
					if (!m_multiline && m_curorLocation.X < (int)Text.size())
					{
						if (m_curorLocation.X < (int)Text.size() - 1 && Text.substr(m_curorLocation.X, 2) == L"\n")
							Text = Text.erase(m_curorLocation.X, 2);
						else
							Text = Text.erase(m_curorLocation.X, 1);

						changed = true;
					}
					else if (m_multiline)
					{
						if (m_curorLocation.X < (int)m_lines[m_curorLocation.Y].size())
						{
							m_lines[m_curorLocation.Y] = m_lines[m_curorLocation.Y].erase(m_curorLocation.X, 1);

							changed = true;
						}
						else if (m_curorLocation.X == (int)m_lines[m_curorLocation.Y].size())
							if (m_curorLocation.Y < m_lines.getCount() - 1)
							{
								if (m_lines[m_curorLocation.Y + 1].size() > 0)
									m_lines[m_curorLocation.Y] += m_lines[m_curorLocation.Y + 1];
								m_lines.RemoveAt(m_curorLocation.Y + 1);

								changed = true;
							}
					}
				}
				break;
			case KEY_HOME:
				m_curorLocation.X = 0;
				if (m_multiline && e.ControlDown)
				{
					m_curorLocation.Y = 0;
					m_scrollOffset.X = 0;
				}
				break;
			case KEY_END:
				if (!m_multiline)
					m_curorLocation.X = (int)Text.size();
				else
				{
					if (e.ControlDown)
					{
						m_curorLocation.Y = m_lines.getCount() - 1;
						m_curorLocation.X = (int)m_lines[m_curorLocation.Y].size();                            
					}
					else
						m_curorLocation.X = (int)m_lines[m_curorLocation.Y].size();
				}
				break;
			case KEY_NUMPADENTER:
			case KEY_RETURN:
				{
					if (m_multiline)
					{
						String lineEnd = L"";


						if ((int)m_lines[m_curorLocation.Y].size() > m_curorLocation.X)
						{
							lineEnd = m_lines[m_curorLocation.Y].substr(m_curorLocation.X, m_lines[m_curorLocation.Y].size() - m_curorLocation.X);
							m_lines[m_curorLocation.Y] = m_lines[m_curorLocation.Y].substr(0, m_lines[m_curorLocation.Y].size() - lineEnd.size());
						}

						m_lines.Insert(m_curorLocation.Y + 1, lineEnd);
						m_curorLocation.X = 0;
						m_curorLocation.Y += 1;

						changed = true;
					}
					else
					{
						eventEnterPressed.Invoke(this);
					}
				}
				break;
			case KEY_SPACE:
				Add(L" ");
				m_curorLocation.X++;
				changed = true;
				break;
			case KEY_DECIMAL:
				Add(L".");
				m_curorLocation.X++;
				changed = true;
				break;
			case KEY_DIVIDE:
				Add(L"/");
				m_curorLocation.X++;
				changed = true;
				break;
			case KEY_BACKSLASH:
				if (e.ShiftDown) Add(L"|"); else Add(L"\\");
				m_curorLocation.X++;
				changed = true;
				break;
				// " ~
			case KEY_COMMA:
				if (e.ShiftDown) Add(L"<"); else Add(L",");
				m_curorLocation.X++;
				changed = true;
				break;
			case KEY_MINUS:
				if (e.ShiftDown) Add(L"_"); else Add(L"-");
				m_curorLocation.X++;
				changed = true;
				break;
			case KEY_LBRACKET:
				if (e.ShiftDown) Add(L"{"); else Add(L"[");
				m_curorLocation.X++;
				changed = true;
				break;
			case KEY_RBRACKET:
				if (e.ShiftDown) Add(L"}"); else Add(L"]");
				m_curorLocation.X++;
				changed = true;
				break;
			case KEY_PERIOD:
				if (e.ShiftDown) Add(L">"); else Add(L".");
				m_curorLocation.X++;
				changed = true;
				break;
			case KEY_ADD:
				if (e.ShiftDown) Add(L"+"); else Add(L"=");
				m_curorLocation.X++;
				changed = true;
				break;
			case KEY_SLASH:
				if (e.ShiftDown) Add(L"?"); else Add(L"/");
				m_curorLocation.X++;
				changed = true;
				break;
			case KEY_SEMICOLON:
				if (e.ShiftDown) Add(L":"); else Add(L"'");
				m_curorLocation.X++;
				changed = true;
				break;
			case KEY_SUBTRACT:
				Add(L"-");
				m_curorLocation.X++;
				changed = true;
				break;
			case KEY_MULTIPLY:
				Add(L"*");
				m_curorLocation.X++;
				changed = true;
				break;
			case KEY_TAB:
				Add(L"      ");
				m_curorLocation.X+=6;
				changed = true;
				break;
			case KEY_NUMPAD0:
			case KEY_0:
				if (e.ShiftDown) Add(L")"); else Add(L"0");
				m_curorLocation.X++;
				changed = true;
				break;
			case KEY_NUMPAD1:
			case KEY_1:
				if (e.ShiftDown) Add(L"!"); else Add(L"1");
				m_curorLocation.X++;
				changed = true;
				break;
			case KEY_NUMPAD2:
			case KEY_2:
				if (e.ShiftDown) Add(L"@"); else Add(L"2");
				m_curorLocation.X++;
				changed = true;
				break;
			case KEY_NUMPAD3:
			case KEY_3:
				if (e.ShiftDown) Add(L"#"); else Add(L"3");
				m_curorLocation.X++;
				changed = true;
				break;
			case KEY_NUMPAD4:
			case KEY_4:
				if (e.ShiftDown) Add(L"$"); else Add(L"4");
				m_curorLocation.X++;
				changed = true;
				break;
			case KEY_NUMPAD5:
			case KEY_5:
				if (e.ShiftDown) Add(L"%"); else Add(L"5");
				m_curorLocation.X++;
				changed = true;
				break;
			case KEY_NUMPAD6:
			case KEY_6:
				if (e.ShiftDown) Add(L"^"); else Add(L"6");
				m_curorLocation.X++;
				changed = true;
				break;
			case KEY_NUMPAD7:
			case KEY_7:
				if (e.ShiftDown) Add(L"&"); else Add(L"7");
				m_curorLocation.X++;
				changed = true;
				break;
			case KEY_NUMPAD8:
			case KEY_8:
				if (e.ShiftDown) Add(L"*"); else Add(L"8");
				m_curorLocation.X++;
				changed = true;
				break;
			case KEY_NUMPAD9:
			case KEY_9:
				if (e.ShiftDown) Add(L"("); else Add(L"9");
				m_curorLocation.X++;
				changed = true;
				break;

			case KEY_A:
				if (e.ShiftDown || e.CapsLock) Add(L"A"); else Add(L"a");
				m_curorLocation.X++;
				changed = true;
				break;
			case KEY_B:
				if (e.ShiftDown || e.CapsLock) Add(L"B"); else Add(L"b");
				m_curorLocation.X++;
				changed = true;
				break;
			case KEY_C:
				if (e.ShiftDown || e.CapsLock) Add(L"C"); else Add(L"c");
				m_curorLocation.X++;
				changed = true;
				break;
			case KEY_D:
				if (e.ShiftDown || e.CapsLock) Add(L"D"); else Add(L"d");
				m_curorLocation.X++;
				changed = true;
				break;
			case KEY_E:
				if (e.ShiftDown || e.CapsLock) Add(L"E"); else Add(L"e");
				m_curorLocation.X++;
				changed = true;
				break;
			case KEY_F:
				if (e.ShiftDown || e.CapsLock) Add(L"F"); else Add(L"f");
				m_curorLocation.X++;
				changed = true;
				break;
			case KEY_G:
				if (e.ShiftDown || e.CapsLock) Add(L"G"); else Add(L"g");
				m_curorLocation.X++;
				changed = true;
				break;

			case KEY_H:
				if (e.ShiftDown || e.CapsLock) Add(L"H"); else Add(L"h");
				m_curorLocation.X++;
				changed = true;
				break;
			case KEY_I:
				if (e.ShiftDown || e.CapsLock) Add(L"I"); else Add(L"i");
				m_curorLocation.X++;
				changed = true;
				break;
			case KEY_J:
				if (e.ShiftDown || e.CapsLock) Add(L"J"); else Add(L"j");
				m_curorLocation.X++;
				changed = true;
				break;
			case KEY_K:
				if (e.ShiftDown || e.CapsLock) Add(L"K"); else Add(L"k");
				m_curorLocation.X++;
				changed = true;
				break;
			case KEY_L:
				if (e.ShiftDown || e.CapsLock) Add(L"L"); else Add(L"l");
				m_curorLocation.X++;
				changed = true;
				break;
			case KEY_M:
				if (e.ShiftDown || e.CapsLock) Add(L"M"); else Add(L"m");
				m_curorLocation.X++;
				changed = true;
				break;
			case KEY_N:
				if (e.ShiftDown || e.CapsLock) Add(L"N"); else Add(L"n");
				m_curorLocation.X++;
				changed = true;
				break;

			case KEY_O:
				if (e.ShiftDown || e.CapsLock) Add(L"O"); else Add(L"o");
				m_curorLocation.X++;
				changed = true;
				break;
			case KEY_P:
				if (e.ShiftDown || e.CapsLock) Add(L"P"); else Add(L"p");
				m_curorLocation.X++;
				changed = true;
				break;
			case KEY_Q:
				if (e.ShiftDown || e.CapsLock) Add(L"Q"); else Add(L"q");
				m_curorLocation.X++;
				changed = true;
				break;
			case KEY_R:
				if (e.ShiftDown || e.CapsLock) Add(L"R"); else Add(L"r");
				m_curorLocation.X++;
				changed = true;
				break;
			case KEY_S:
				if (e.ShiftDown || e.CapsLock) Add(L"S"); else Add(L"s");
				m_curorLocation.X++;
				changed = true;
				break;
			case KEY_T:
				if (e.ShiftDown || e.CapsLock) Add(L"T"); else Add(L"t");
				m_curorLocation.X++;
				changed = true;
				break;

			case KEY_U:
				if (e.ShiftDown || e.CapsLock) Add(L"U"); else Add(L"u");
				m_curorLocation.X++;
				changed = true;
				break;
			case KEY_V:
				if (e.ShiftDown || e.CapsLock) Add(L"V"); else Add(L"v");
				m_curorLocation.X++;
				changed = true;
				break;
			case KEY_W:
				if (e.ShiftDown || e.CapsLock) Add(L"W"); else Add(L"w");
				m_curorLocation.X++;
				changed = true;
				break;
			case KEY_X:
				if (e.ShiftDown || e.CapsLock) Add(L"X"); else Add(L"x");
				m_curorLocation.X++;
				changed = true;
				break;
			case KEY_Y:
				if (e.ShiftDown || e.CapsLock) Add(L"Y"); else Add(L"y");
				m_curorLocation.X++;
				changed = true;
				break;
			case KEY_Z:
				if (e.ShiftDown || e.CapsLock) Add(L"Z"); else Add(L"z");
				m_curorLocation.X++;
				changed = true;
				break;

			default:
				break;
			}

			UpdateScrolling();
			m_cursorVisible = true;
			m_timerStarted = true;

			if (changed)
			{
				eventContentChanged.Invoke(this);
			}
		}
		void TextBox::Keyboard_OnPaste(String value)
		{
			if (value.size())
			{
				String newText(value);
				
				for (size_t i=0;i<newText.size();i++)
				{
					if (newText[i] == '\r')
						newText[i] = ' ';

					if (m_multiline && newText[i]=='\t')
					{
						newText[i] = ' ';
						newText.insert(i, L"   ");
					}
				}

				Add(newText);

				Point textSize = m_fontRef->MeasureString(newText);
				m_cursorOffset.Y += (int)(textSize.Y / m_fontRef->getLineHeightInt()) - 1;
				
				List<String> lines;
				StringUtils::Split(newText, lines, L"\n");
				int len = (int)lines.getCount();
				m_curorLocation.X += (int)lines[len-1].size();

				UpdateScrolling();
				
				eventContentChanged.Invoke(this);
			}
		}

		void TextBox::vScrollbar_OnChangeValue(Control* ctrl)
		{
			m_scrollOffset.Y = m_vscrollBar->getValue() * m_fontRef->getLineHeightInt();
			m_hasFocus = true;
		}
		void TextBox::hScrollbar_OnChangeValue(Control* ctrl)
		{
			if (m_multiline)
			{
				m_scrollOffset.X = m_hscrollBar->getValue();
				m_hasFocus = true;
			}
		}


	}
}