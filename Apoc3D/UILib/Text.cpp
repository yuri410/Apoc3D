#include "Text.h"
#include "FontManager.h"

#include "apoc3d/Core/GameTime.h"
#include "apoc3d/Input/InputAPI.h"
#include "apoc3d/Input/Mouse.h"
#include "apoc3d/Graphics/RenderSystem/Sprite.h"
#include "apoc3d/Graphics/RenderSystem/RenderDevice.h"
#include "apoc3d/Graphics/RenderSystem/RenderStateManager.h"
#include "apoc3d/Utility/StringUtils.h"
#include "StyleSkin.h"

#include "Scrollbar.h"

using namespace Apoc3D::Input;
using namespace Apoc3D::Graphics::RenderSystem;
using namespace Apoc3D::Utility;

namespace Apoc3D
{
	namespace UI
	{
		/************************************************************************/
		/* Label                                                                */
		/************************************************************************/

		Label::Label(Font* font,const Point& position, const String& text)
			: Label((const StyleSkin*)nullptr, position, text) { }

		Label::Label(Font* font,const Point& position, const String& text, int width, TextHAlign alignment )
			: Label((const StyleSkin*)nullptr, position, text, width, alignment) { }

		Label::Label(Font* font, const Point& position, const String& text, int width, int height, TextHAlign alignment)
			: Label((const StyleSkin*)nullptr, position, text, width, height, alignment) { }


		Label::Label(const StyleSkin* skin, const Point& position, const String& text)
			: Control(skin, position), m_text(text), AutosizeX(true), AutosizeY(true)
		{
			Initialize(skin);
		}

		Label::Label(const StyleSkin* skin, const Point& position, const String& text, int width, TextHAlign alignment)
			: Control(skin, position), m_text(text), AutosizeY(true)
		{
			TextSettings.HorizontalAlignment = alignment;

			m_size.X = width;

			Initialize(skin);
		}

		Label::Label(const StyleSkin* skin, const Point& position, const String& text, int width, int height, TextHAlign alignment)
			: Control(skin, position), m_text(text)
		{
			TextSettings.HorizontalAlignment = alignment;

			m_size.X = width;
			m_size.Y = height;

			Initialize(skin);
		}

		Label::~Label()
		{

		}

		void Label::Initialize(const StyleSkin* skin)
		{
			if (skin)
				TextSettings.TextColor = skin->TextColor;

			UpdateText();
		}

		void Label::UpdateText()
		{
			if (AutosizeX)
			{
				m_lines.Clear();
				StringUtils::Split(m_text, m_lines, L"\n\r");
			}
			else
			{
				List<String> lines;
				StringUtils::Split(m_text, lines, L"\n\r");

				List<String> newLines(lines.getCount());
				for (String& line : lines)
				{
					if (line.length())
					{
						int32 newlineCount;
						String newLine = m_fontRef->LineBreakString(line, m_size.X, true, newlineCount);

						if (newlineCount == 1)
						{
							newLines.Add(newLine);
						}
						else
						{
							List<String> temp = StringUtils::Split(newLine, L"\n\r");
							newLines.AddList(temp);
						}
					}
				}
				m_lines = newLines;
			}
			

			if (AutosizeX)
			{
				int32 maxWidth = 0;
				for (const String& line : m_lines)
				{
					Point sz = m_fontRef->MeasureString(line);
					if (sz.X > maxWidth)
						maxWidth = sz.X;
				}
				m_size.X = maxWidth;
			}
			if (AutosizeY)
			{
				m_size.Y = static_cast<int>(m_fontRef->getTextBackgroundHeight(m_lines.getCount()));
			}
		}

		void Label::Update(const GameTime* time)
		{
			UpdateEvents();
		}

		void Label::UpdateEvents()
		{
			if (!Visible)
				return;

			UpdateEvents_StandardButton(m_mouseHover, m_mouseDown, getAbsoluteArea(),
				&Label::OnMouseHover, &Label::OnMouseOut, &Label::OnPress, &Label::OnRelease);
		}

		void Label::Draw(Sprite* sprite)
		{
			Point drawPos = GetAbsolutePosition();
			if (m_lines.getCount() <= 1)
			{
				TextSettings.Draw(sprite, m_fontRef, m_text, drawPos, m_size, 0xff);
			}
			else
			{
				for (const String& line : m_lines)
				{
					TextSettings.Draw(sprite, m_fontRef, line, drawPos, m_size, 0xff);

					drawPos.Y += m_fontRef->getLineHeightInt();
				}
			}
		}

		void Label::OnMouseHover() { eventMouseHover.Invoke(this); }
		void Label::OnMouseOut() { eventMouseOut.Invoke(this); }
		void Label::OnPress() { eventPress.Invoke(this); }
		void Label::OnRelease() { eventRelease.Invoke(this); }

		void Label::SetText(const String& txt)
		{
			if (m_text != txt)
			{
				m_text = txt;
				UpdateText();
			}
		}

		/************************************************************************/
		/*   TextBox                                                            */
		/************************************************************************/

		TextBox::TextBox(const StyleSkin* skin, const Point& position, int width)
			: Control(skin, position), m_textEdit(Point(0,0), false)
		{
			m_size.X = width;
			Initialize(skin);
		}
		TextBox::TextBox(const StyleSkin* skin, const Point& position, int width, const String& text)
			: Control(skin, position), m_textEdit(Point(0,0), false)
		{
			m_size.X = width;
			Add(text);
			m_textEdit.MoveCursorToEnd();
			Initialize(skin);
		}
		TextBox::TextBox(const StyleSkin* skin, const Point& position, int width, int height, const String& text)
			: Control(skin, position, Point(width, height)), m_multiline(true), m_textEdit(Point(0,0), true)
		{
			m_size.X = width;
			Add(text);
			m_textEdit.MoveCursorToEnd();
			Initialize(skin);
		}
		TextBox::~TextBox()
		{
			DELETE_AND_NULL(m_vscrollBar);
			DELETE_AND_NULL(m_hscrollBar);
		}

		void TextBox::Add(const String& text)
		{
			m_textEdit.Add(text);

			UpdateScrolling();
		}

		void TextBox::UpdateScrolling()
		{
			Point cursorPos = m_textEdit.getCursorPosition();

			if (m_cursorOffset.X > m_scrollOffset.X + m_size.X - 20)
				m_scrollOffset.X = m_cursorOffset.X - (m_size.X - 20);
			else if (m_cursorOffset.X - 20 < m_scrollOffset.X)
				m_scrollOffset.X = Math::Max(0, m_cursorOffset.X - 20);

			if (m_scrollOffset.X < 0)
				m_scrollOffset.X = 0;
			if (cursorPos.X == 0)
				m_scrollOffset.X = 0;

			if (m_multiline)
			{
				if (m_hscrollBar)
					m_hscrollBar->SetValue(m_scrollOffset.X);

				if (m_fontRef)
				{
					int offsetY = m_scrollOffset.Y / m_fontRef->getLineHeightInt();
					if (m_hscrollBar && m_hscrollBar->Maximum > 0)
					{
						if (cursorPos.Y > offsetY + m_visibleLines - 2)
							m_scrollOffset.Y = (cursorPos.Y - (m_visibleLines - 2)) * m_fontRef->getLineHeightInt();
						else if (cursorPos.Y < offsetY)
							m_scrollOffset.Y = cursorPos.Y * m_fontRef->getLineHeightInt();
					}
					else
					{
						if (cursorPos.Y > offsetY + m_visibleLines - 1)
							m_scrollOffset.Y = (cursorPos.Y - (m_visibleLines - 1)) * m_fontRef->getLineHeightInt();
						else if (cursorPos.Y < offsetY)
							m_scrollOffset.Y = cursorPos.Y * m_fontRef->getLineHeightInt();
					}

					if (m_vscrollBar)
						m_vscrollBar->SetValue(m_scrollOffset.Y / m_fontRef->getLineHeightInt());
				}

			}
		}

		void TextBox::Initialize(const StyleSkin* skin)
		{
			Magin = m_multiline ? skin->TextBoxExMargin : skin->TextBoxMargin;

			if (!m_multiline)
			{
				NormalGraphic = UIGraphic(skin->SkinTexture, skin->TextBox);

				m_size.Y = NormalGraphic.SourceRects[0].Height - Magin.getVerticalSum();
			}
			else
			{
				NormalGraphic = UIGraphic(skin->SkinTexture, skin->TextBoxEx);

				m_visibleLines = (int)ceilf((float)m_size.Y / m_fontRef->getLineHeightInt());

				// fix up height
				m_size.Y = m_visibleLines * m_fontRef->getLineHeightInt() - Magin.getVerticalSum();
			}

			DisabledGraphic = NormalGraphic;

			SetFont(skin->TextBoxFont);

			TextSettings.HorizontalAlignment = TextHAlign::Left;
			TextSettings.TextColor = skin->TextColor;

			ContentPadding = skin->TextBoxPadding;
			//m_dstRect = Apoc3D::Math::Rectangle(Position, m_size);

			//m_textOffset = Point(5, static_cast<int>((skin->TextBox[0].Height - m_fontRef->getLineBackgroundHeight()) / 2));

			m_textEdit.eventKeyPress().Bind(this, &TextBox::Keyboard_OnPress);
			m_textEdit.eventKeyPaste().Bind(this, &TextBox::Keyboard_OnPaste);
			m_textEdit.eventEnterPressed.Bind(this, &TextBox::TextEditState_EnterPressed);
			m_textEdit.eventContentChanged.Bind(this, &TextBox::TextEditState_ContentChanged);
			m_textEdit.eventUpPressedSingleline.Bind(this, &TextBox::TextEditState_UpPressedSingleline);
			m_textEdit.eventDownPressedSingleline.Bind(this, &TextBox::TextEditState_DownPressedSingleline);

			if (m_multiline && m_scrollBar != SBT_None)
			{
				InitScrollbars(skin);
			}
			m_timerStarted = true;
		}

		void TextBox::InitScrollbars(const StyleSkin* skin)
		{
			//Apoc3D::Math::Rectangle area = getAbsoluteArea();

			if (m_scrollBar == SBT_Vertical)
			{
				Point pos = Position;
				pos.X += m_size.X;
				//pos.Y++;
				m_vscrollBar = new ScrollBar(skin, pos, ScrollBar::SCRBAR_Vertical, m_size.Y);
			}
			else if (m_scrollBar == SBT_Horizontal)
			{
				Point pos = Position;
				//pos.X++;
				pos.Y += m_size.Y;// -2;
				m_hscrollBar = new ScrollBar(skin, pos, ScrollBar::SCRBAR_Horizontal, m_size.X);
			}
			else if (m_scrollBar == SBT_Both)
			{
				Point pos = Position;
				pos.X += m_size.X - skin->HScrollBarBG.Height;
				//pos.Y++;

				m_vscrollBar = new ScrollBar(skin, pos, ScrollBar::SCRBAR_Vertical, m_size.Y - skin->HScrollBarBG.Height);

				pos = Position;
				//pos.X++;
				pos.Y += m_size.Y - skin->VScrollBarBG.Width;
				m_hscrollBar = new ScrollBar(skin, pos, ScrollBar::SCRBAR_Horizontal, m_size.X - skin->VScrollBarBG.Width);
			}

			if (m_vscrollBar)
			{
				//m_vscrollBar->SetSkin(m_skin);
				//m_vscrollBar->setOwner(getOwner());
				m_vscrollBar->eventValueChanged.Bind(this, &TextBox::vScrollbar_OnChangeValue);
				//m_vscrollBar->Initialize(device);
			}
			if (m_hscrollBar)
			{
				//m_hscrollBar->SetSkin(m_skin);
				//m_hscrollBar->setOwner(getOwner());
				m_hscrollBar->eventValueChanged.Bind(this, &TextBox::hScrollbar_OnChangeValue);
				//m_hscrollBar->Initialize(device);
			}
		}
		void TextBox::Update(const GameTime* time)
		{
			Control::Update(time);

			CheckFocus();
			Point cursorPos = m_textEdit.getCursorPosition();

			if (m_hasFocus && !m_readOnly)
			{
				m_textEdit.Update(time);
			}

			Mouse* mouse = InputAPIManager::getSingleton().getMouse();
			Apoc3D::Math::Rectangle textArea = GetTextArea();
			if (textArea.Contains(mouse->GetPosition()))
			{
				if (mouse->IsLeftPressed())
				{
					m_isDraggingSelecting = true;

					int32 lineIndex = (mouse->getY() - textArea.Y) / m_fontRef->getLineHeightInt();
					int32 dx = mouse->getX() - textArea.X;
					m_textEdit.MoveCursorTo(Point(0, lineIndex));

					int32 xpos = m_fontRef->FitSingleLineString(m_textEdit.getCurrentLine(), Math::Max(0, dx));
					m_textEdit.MoveCursorTo(Point(xpos, lineIndex));

					m_textSelectionStart = m_textEdit.getCursorPosition();
				}
				else if (m_isDraggingSelecting && mouse->IsLeftUp())
				{
					m_isDraggingSelecting = false;
				}
			}

			if (m_multiline && m_scrollBar != SBT_None)
			{
				UpdateScrollbars(time);
			}

			if (m_timerStarted)
			{
				m_timer -= time->getElapsedTime();
				if (m_timer < 0)
				{
					m_timer = 0.5f;
					m_cursorVisible = !m_cursorVisible;
				}
			}
		}
		void TextBox::UpdateScrollbars(const GameTime* time)
		{
			Apoc3D::Math::Rectangle area = getAbsoluteArea();

			Mouse* mouse = InputAPIManager::getSingleton().getMouse();
			if (m_vscrollBar)
			{
				if (m_hscrollBar && m_hscrollBar->Maximum>0)
					m_vscrollBar->Maximum = Math::Max(0, m_textEdit.getLineCount() - (m_visibleLines - 1));
				else
					m_vscrollBar->Maximum = Math::Max(0, m_textEdit.getLineCount() - m_visibleLines);
			}

			if (m_vscrollBar && m_vscrollBar->Maximum > 0)
			{
				if (m_hscrollBar && m_hscrollBar->Maximum > 0)
					m_vscrollBar->SetLength(m_size.Y - m_hscrollBar->getHeight());
				else
					m_vscrollBar->SetLength(m_size.Y);

				m_vscrollBar->Update(time);

				if (mouse->getDZ() && area.Contains(mouse->GetPosition()))
				{
					m_vscrollBar->SetValue(m_vscrollBar->getValue() + mouse->getDZ() / 60);
				}
			}

			if (m_hscrollBar && m_hscrollBar->Maximum > 0)
			{
				if (m_vscrollBar && m_vscrollBar->Maximum > 0)
					m_hscrollBar->SetLength(m_size.X - m_vscrollBar->getWidth());
				else
					m_hscrollBar->SetLength(m_size.X);

				m_hscrollBar->Update(time);
			}
		}
		void TextBox::CheckFocus()
		{
			//m_sRect = getArea();
			Apoc3D::Math::Rectangle area = getAbsoluteArea();

			Mouse* mouse = InputAPIManager::getSingleton().getMouse();
			if (mouse->IsLeftPressed())
			{
				if (area.Contains(mouse->GetPosition()))
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
			const UIGraphic& g = Enabled ? NormalGraphic : DisabledGraphic;

			g.Draw(sprite, Magin.InflateRect(getAbsoluteArea()));


			Apoc3D::Math::Rectangle dstRect = getAbsoluteArea();

			RenderStateManager* stMgr = sprite->getRenderDevice()->getRenderState();
			bool oldScissorTest = stMgr->getScissorTestEnabled();
			Apoc3D::Math::Rectangle oldScissorRect;
			if (oldScissorTest)
			{
				oldScissorRect = stMgr->getScissorTestRect();
			}

			sprite->Flush();
			stMgr->setScissorTest(true, &dstRect);
			_DrawText(sprite);
			sprite->Flush();
			stMgr->setScissorTest(oldScissorTest, &oldScissorRect);

			if (m_vscrollBar && m_vscrollBar->Maximum > 0)
			{
				m_vscrollBar->Draw(sprite);
			}
			if (m_hscrollBar && m_hscrollBar->Maximum > 0)
			{
				m_hscrollBar->Draw(sprite);
			}
		}

		//const ColorValue DisabledBG = 0xffe1e1e1;

		void TextBox::_DrawText(Sprite* sprite)
		{
			Apoc3D::Math::Rectangle contentArea = GetTextArea();

			Point cursorPos = m_textEdit.getCursorPosition();

			int32 cursorLeft = 0;// m_fontRef->MeasureString(L"|").X / 2;
			
			if (!m_multiline)
			{
				const String& text = m_textEdit.getText();
				if (cursorPos.X > 0)
				{
					m_cursorOffset.X = m_fontRef->MeasureString(
						text.substr(0, cursorPos.X)).X + cursorLeft;
				}
				else
				{
					m_cursorOffset.X = cursorLeft;
				}

				m_cursorOffset.Y = 0;// m_textOffset.Y;

				//Point pos(m_textOffset.X - m_scrollOffset.X, m_textOffset.Y - m_scrollOffset.Y);
				//m_fontRef->DrawString(sprite, text, m_textOffset - m_scrollOffset + baseOffset, m_skin->TextColor);

				TextSettings.Draw(sprite, m_fontRef, text, contentArea.getTopLeft() - m_scrollOffset, contentArea.getSize(), 0xff);

				if (m_hasFocus && !m_readOnly && m_cursorVisible && ParentFocused)//getOwner() == UIRoot::getTopMostForm())
				{
					//m_fontRef->DrawString(sprite, L"|", m_cursorOffset - m_scrollOffset + baseOffset, m_skin->TextColor);
					TextSettings.Draw(sprite, m_fontRef, L"|", contentArea.getTopLeft() + m_cursorOffset - m_scrollOffset, contentArea.getSize(), 0xff);
				}
			}
			else
			{
				const List<String>& lines = m_textEdit.getLines();

				Point lineOffset = Point(0, 0);
				int maxWidth = 0;
				for (const String& line : lines)
				{
					Point lineSize = m_fontRef->MeasureString(line);

					TextSettings.Draw(sprite, m_fontRef, line, lineOffset - m_scrollOffset + contentArea.getTopLeft(), contentArea.getSize(), 0xff);
					//m_fontRef->DrawString(sprite, line, m_textOffset + m_lineOffset - m_scrollOffset + baseOffset, m_skin->TextColor);
					
					if (lineSize.X - m_size.X > maxWidth)
					{
						if (m_vscrollBar && m_vscrollBar->Maximum > 0)
						{
							maxWidth = lineSize.X - m_size.X;
						}
						else
						{
							maxWidth = lineSize.X - m_size.X + 12;
						}
					}
					
					lineOffset.Y += m_fontRef->getLineHeightInt();
				}

				if (m_hscrollBar)
				{
					m_hscrollBar->Maximum = maxWidth;

					if (m_vscrollBar && m_vscrollBar->Maximum > 0 && m_hscrollBar->Maximum > 0)
						m_hscrollBar->Maximum = m_hscrollBar->Maximum + 20;

					m_hscrollBar->Step = Math::Max(1, m_hscrollBar->Maximum / 15);
				}

				if (m_hasFocus && !m_readOnly && m_cursorVisible && ParentFocused)// getOwner() == UIRoot::getTopMostForm())
				{
					if (cursorPos.X > (int)lines[cursorPos.Y].size())
						cursorPos.X = (int)lines[cursorPos.Y].size();

					m_cursorOffset.X = m_fontRef->MeasureString(
						lines[cursorPos.Y].substr(0, cursorPos.X)).X + cursorLeft;
					m_cursorOffset.Y = m_fontRef->getLineHeightInt() * cursorPos.Y + 1;

					//m_fontRef->DrawString(sprite, L"|", m_cursorOffset - m_scrollOffset + baseOffset, m_skin->TextColor);
					TextSettings.Draw(sprite, m_fontRef, L"|", m_cursorOffset - m_scrollOffset + contentArea.getTopLeft(), contentArea.getSize(), 0xff);
				}
			}
		}

		Apoc3D::Math::Rectangle TextBox::GetTextArea() const
		{
			Point pos = GetAbsolutePosition();

			Apoc3D::Math::Rectangle rect;
			rect.X = pos.X;
			rect.Y = pos.Y;

			if (m_vscrollBar && m_vscrollBar->Maximum > 0)
				rect.Width = m_size.X - m_vscrollBar->getWidth();
			else
				rect.Width = m_size.X;

			if (m_hscrollBar && m_hscrollBar->Maximum > 0)
				rect.Height = m_size.Y - m_hscrollBar->getHeight();
			else
				rect.Height = m_size.Y;

			return ContentPadding.ShrinkRect(rect);
		}

		void TextBox::TextEditState_EnterPressed() { eventEnterPressed.Invoke(this); }
		void TextBox::TextEditState_ContentChanged() { eventContentChanged.Invoke(this); }
		void TextBox::TextEditState_UpPressedSingleline() { eventUpPressedSingleline.Invoke(this); }
		void TextBox::TextEditState_DownPressedSingleline() { eventDownPressedSingleline.Invoke(this); }


		void TextBox::SetText(const String& text)
		{
			m_textEdit.SetText(text);
		}

		void TextBox::Keyboard_OnPress(KeyboardKeyCode code, KeyboardEventsArgs e)
		{
			UpdateScrolling();
			m_cursorVisible = true;
			m_timerStarted = true;
		}
		void TextBox::Keyboard_OnPaste(String value)
		{
			if (value.size())
			{
				Point textSize = m_fontRef->MeasureString(m_textEdit.getText());
				m_cursorOffset.Y += (int)(textSize.Y / m_fontRef->getLineHeightInt()) - 1;

				UpdateScrolling();
			}
		}

		void TextBox::vScrollbar_OnChangeValue(ScrollBar* ctrl)
		{
			m_scrollOffset.Y = m_vscrollBar->getValue() * m_fontRef->getLineHeightInt();
			m_hasFocus = true;
		}
		void TextBox::hScrollbar_OnChangeValue(ScrollBar* ctrl)
		{
			if (m_multiline)
			{
				m_scrollOffset.X = m_hscrollBar->getValue();
				m_hasFocus = true;
			}
		}

	}
}