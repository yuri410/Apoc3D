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
		Label::Label(const Point& position, const String& text, int width, TextAlignment alignment)
			: Control(position, text), m_alignment(alignment), m_width(width), m_hasColorValue(false)
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
			if (m_lines.getCount()<=1)
			{
				Point txtSize = m_fontRef->MeasureString(Text);
				switch (m_alignment)
				{
				case TextAlignment::Right:
					m_textOffset.X = Size.X - txtSize.X;
					break;
				case TextAlignment::Center:
					m_textOffset.X = (Size.X - txtSize.X)/2;
					break;
				}

				m_drawPos = Point(Position.X + m_textOffset.X, Position.Y);
				m_fontRef->DrawString(sprite, Text, m_drawPos, m_hasColorValue ? m_colorOverride : m_skin->TextColor);
			}
			else
			{
				for (int i=0;i<m_lines.getCount();i++)
				{
					Point txtSize = m_fontRef->MeasureString(Text);
					switch (m_alignment)
					{
					case TextAlignment::Right:
						m_drawPos.X = Position.X + Size.X - txtSize.X;
						break;
					case TextAlignment::Center:
						m_drawPos.X = Position.X + (Size.X - txtSize.X)/2;
						break;
					case TextAlignment::Left:
						m_drawPos.X = Position.X;
						break;
					}
					m_drawPos.Y = Position.Y + i * m_fontRef->getLineHeightInt();
					m_fontRef->DrawString(sprite, m_lines[i], m_drawPos, m_hasColorValue ? m_colorOverride : m_skin->TextColor);
				}
			}
		}

		void Label::SetTextColorOverride(ColorValue cv)
		{
			m_colorOverride = cv;
			m_hasColorValue = true;
		}

		/************************************************************************/
		/*   TextBox                                                            */
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
				m_scrollOffset.X = Math::Max(0, m_cursorOffset.X - 20);

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

			m_textOffset = Point(5, static_cast<int>((m_skin->TextBox[0].Height - m_fontRef->getLineBackgroundHeight()) /2 ));
			
			if (!m_multiline)
			{
				int32 yMargin = m_skin->TextBoxMargin.getVerticalSum();
				Size.Y = m_skin->TextBox[0].Height - yMargin;
			}
			else
			{
				m_visibleLines = (int)ceilf((float)Size.Y / m_fontRef->getLineHeightInt());

				Size.Y = m_visibleLines * m_fontRef->getLineHeightInt() + 2;
			}

			m_dRect = Apoc3D::Math::Rectangle(Position.X,Position.Y,Size.X,Size.Y);

			m_keyboard.eventKeyPress().Bind(this, &TextBox::Keyboard_OnPress);
			m_keyboard.eventKeyPaste().Bind(this, &TextBox::Keyboard_OnPaste);

			if (m_multiline && m_scrollBar != SBT_None)
			{
				InitScrollbars(device);
			}
			m_timerStarted = true;
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
					m_vscrollBar->setMax(Math::Max(0, m_lines.getCount() - (m_visibleLines - 1)));
				else
					m_vscrollBar->setMax(Math::Max(0, m_lines.getCount() - m_visibleLines));
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

		const ColorValue DisabledBG = 0xffe1e1e1;

		void TextBox::DrawMonoline(Sprite* sprite)
		{
			int32 xMargin = -m_skin->TextBoxMargin.getHorizontalSum();//m_skin->TextBoxMargin[StyleSkin::SI_Left] + m_skin->TextBoxMargin[StyleSkin::SI_Right]);
			int32 yMargin = -m_skin->TextBoxMargin.getVerticalSum();//m_skin->TextBoxMargin[StyleSkin::SI_Top] + m_skin->TextBoxMargin[StyleSkin::SI_Bottom]);

			Apoc3D::Math::Rectangle dstRect[3];
			dstRect[0] = Apoc3D::Math::Rectangle(m_skin->TextBoxMargin.Left, m_skin->TextBoxMargin.Top, m_skin->TextBox[0].Width , m_skin->TextBox[0].Height + yMargin);
			dstRect[1] = Apoc3D::Math::Rectangle(dstRect[0].Width, m_skin->TextBoxMargin.Top,
				Size.X - m_skin->TextBox[0].Width - m_skin->TextBox[2].Width + xMargin, m_skin->TextBox[1].Height + yMargin);
			dstRect[2] = Apoc3D::Math::Rectangle(dstRect[1].getRight(), m_skin->TextBoxMargin.Top, m_skin->TextBox[2].Width, m_skin->TextBox[2].Height + yMargin);
			
			for (int32 i=0;i<3;i++)
			{
				dstRect[i].X += Position.X;
				dstRect[i].Y += Position.Y;
			}

			ColorValue modColor = Enabled ? CV_White : DisabledBG;

			sprite->Draw(m_skin->SkinTexture, dstRect[0], &m_skin->TextBox[0], modColor);
			sprite->Draw(m_skin->SkinTexture, dstRect[1], &m_skin->TextBox[1], modColor);
			sprite->Draw(m_skin->SkinTexture, dstRect[2], &m_skin->TextBox[2], modColor);
		}
		void TextBox::DrawMultiline(Sprite* sprite)
		{
			Apoc3D::Math::Rectangle graphicalArea(Position.X, Position.Y, Size.X, Size.Y);
			graphicalArea = m_skin->TextBoxExMargin.InflateRect(graphicalArea);

			const int GraphicalPaddingWidth = m_skin->TextBoxEx[0].Width + m_skin->TextBoxEx[2].Width;
			const int GraphicalPaddingHeight = m_skin->TextBoxEx[0].Height + m_skin->TextBoxEx[6].Height;

			Apoc3D::Math::Rectangle dstRect[9];
			for (int i=0;i<9;i++)
			{
				dstRect[i] = m_skin->TextBoxEx[i];

				dstRect[i].X += graphicalArea.X - m_skin->TextBoxEx[0].X;
				dstRect[i].Y += graphicalArea.Y - m_skin->TextBoxEx[0].Y;
			}

			int eWidth = graphicalArea.Width - GraphicalPaddingWidth;
			int eHeight = graphicalArea.Height - GraphicalPaddingHeight;

			dstRect[1].Width = eWidth; // top
			dstRect[4].Width = eWidth; // mid
			dstRect[7].Width = eWidth; // bottom

			dstRect[3].Height = eHeight; // left
			dstRect[4].Height = eHeight; // mid
			dstRect[5].Height = eHeight; // right

			dstRect[2].X = dstRect[5].X = dstRect[8].X = dstRect[1].getRight();
			dstRect[6].Y = dstRect[7].Y = dstRect[8].Y = dstRect[3].getBottom();


			ColorValue modColor = Enabled ? CV_White : DisabledBG;

			for (int i=0;i<9;i++)
			{
				sprite->Draw(m_skin->SkinTexture, dstRect[i], &m_skin->TextBoxEx[i], modColor);
			}
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
				m_fontRef->DrawString(sprite, Text, m_textOffset-m_scrollOffset + baseOffset, m_skin->TextColor);
				if (m_hasFocus && !m_locked && m_cursorVisible && getOwner()==UIRoot::getTopMostForm())
				{
					m_fontRef->DrawString(sprite, L"|", m_cursorOffset - m_scrollOffset + baseOffset, m_skin->TextColor);
				}
			}
			else
			{
				int maxWidth = 0;
				for (int i=0;i<m_lines.getCount();i++)
				{
					Point lineSize = m_fontRef->MeasureString(m_lines[i]);

					m_lineOffset.Y = i*m_fontRef->getLineHeightInt();

					m_fontRef->DrawString(sprite, m_lines[i], m_textOffset+m_lineOffset-m_scrollOffset + baseOffset, m_skin->TextColor);

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
					m_hscrollBar->setStep(Math::Max(1, m_hscrollBar->getMax() / 15));
				}
				if (m_hasFocus && !m_locked && m_cursorVisible && getOwner() == UIRoot::getTopMostForm())
				{
					if (m_curorLocation.X > (int)m_lines[m_curorLocation.Y].size())
						m_curorLocation.X = (int)m_lines[m_curorLocation.Y].size();

					m_cursorOffset.X = m_fontRef->MeasureString(
						m_lines[m_curorLocation.Y].substr(0, m_curorLocation.X)).X+cursorLeft;
					m_cursorOffset.Y = m_fontRef->getLineHeightInt() * m_curorLocation.Y + 1;

					m_fontRef->DrawString(sprite, L"|", m_cursorOffset - m_scrollOffset + baseOffset, m_skin->TextColor);
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
					if (!m_multiline && m_curorLocation.X > 0 && m_curorLocation.X < (int)Text.size() + 1)
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