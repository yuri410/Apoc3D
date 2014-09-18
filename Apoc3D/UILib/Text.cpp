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
		/*  Label                                                               */
		/************************************************************************/

		Label::Label(Font* font,const Point& position, const String& text)
			: Control(nullptr, position), m_text(text), AutosizeX(true), AutosizeY(true)
		{
			Initialize(font);
		}

		Label::Label(Font* font, const Point& position, const String& text, int width, TextHAlign alignment)
			: Control(nullptr, position, width), m_text(text), AutosizeY(true)
		{
			Initialize(font);
			TextSettings.HorizontalAlignment = alignment;
		}

		Label::Label(Font* font, const Point& position, const String& text, const Point& size, TextHAlign alignment)
			: Control(nullptr, position, size), m_text(text)
		{
			Initialize(font);
			TextSettings.HorizontalAlignment = alignment;
		}

		Label::Label(Font* font, const Point& position, const String& text, int width, int height, TextHAlign alignment)
			: Label(font, position, text, Point(width, height), alignment) { }

		///////////////////////////////////////////////////////////////////////////////////////////////////////////////

		Label::Label(const StyleSkin* skin, const Point& position, const String& text)
			: Control(skin, position), m_text(text), AutosizeX(true), AutosizeY(true)
		{
			Initialize(skin);
		}

		Label::Label(const StyleSkin* skin, const Point& position, const String& text, int width, TextHAlign alignment)
			: Control(skin, position, width), m_text(text), AutosizeY(true)
		{
			Initialize(skin);
			TextSettings.HorizontalAlignment = alignment;
		}

		Label::Label(const StyleSkin* skin, const Point& position, const String& text, const Point& size, TextHAlign alignment)
			: Control(skin, position, size), m_text(text)
		{
			Initialize(skin);
			TextSettings.HorizontalAlignment = alignment;
		}

		Label::Label(const StyleSkin* skin, const Point& position, const String& text, int width, int height, TextHAlign alignment)
			: Label(skin, position, text, Point(width, height), alignment) { }

		///////////////////////////////////////////////////////////////////////////////////////////////////////////////

		Label::~Label()
		{

		}

		void Label::Initialize(const StyleSkin* skin)
		{
			if (skin)
				TextSettings.TextColor = skin->TextColor;
			
			TextSettings.HorizontalAlignment = TextHAlign::Left;
			TextSettings.VerticalAlignment = TextVAlign::Top;

			UpdateText();
		}
		void Label::Initialize(Font* font)
		{
			m_fontRef = font;
			TextSettings.HorizontalAlignment = TextHAlign::Left;
			TextSettings.VerticalAlignment = TextVAlign::Top;
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

		TextBox::TextBox(const TextBoxVisualSettings& settings, const Point& position, int width)
			: ScrollableControl(nullptr, position, width), m_textEdit(false)
		{
			Initialize(settings);
		}
		TextBox::TextBox(const TextBoxVisualSettings& settings, const Point& position, int width, const String& text)
			: ScrollableControl(nullptr, position, width), m_textEdit(false)
		{
			InitText(text);
			m_textEdit.MoveCursorToEnd();
			Initialize(settings);
		}
		TextBox::TextBox(const TextBoxVisualSettings& settings, const Point& position, const Point& size, const String& text)
			: ScrollableControl(nullptr, position, size), m_multiline(true), m_textEdit(true)
		{
			InitText(text);
			m_textEdit.MoveCursorToEnd();
			Initialize(settings);
		}
		TextBox::TextBox(const TextBoxVisualSettings& settings, const Point& position, int width, int height, const String& text)
			: TextBox(settings, position, Point(width, height), text) { }


		TextBox::TextBox(const StyleSkin* skin, const Point& position, int width)
			: ScrollableControl(skin, position, width), m_textEdit(false)
		{
			Initialize(skin);
		}
		TextBox::TextBox(const StyleSkin* skin, const Point& position, int width, const String& text)
			: ScrollableControl(skin, position, width), m_textEdit(false)
		{
			InitText(text);
			m_textEdit.MoveCursorToEnd();
			Initialize(skin);
		}
		TextBox::TextBox(const StyleSkin* skin, const Point& position, const Point& size, const String& text)
			: ScrollableControl(skin, position, size), m_multiline(true), m_textEdit(true)
		{
			InitText(text);
			m_textEdit.MoveCursorToEnd();
			Initialize(skin);
		}
		TextBox::TextBox(const StyleSkin* skin, const Point& position, int width, int height, const String& text)
			: TextBox(skin, position, Point(width, height), text) { }


		TextBox::~TextBox()
		{ }


		void TextBox::Initialize(const StyleSkin* skin)
		{
			m_alwaysShowVS = m_multiline;

			Margin = m_multiline ? skin->TextBoxExMargin : skin->TextBoxMargin;

			if (!m_multiline)
			{
				NormalGraphic = UIGraphic(skin->SkinTexture, skin->TextBoxRegions, skin->TextBoxColor);
			}
			else
			{
				NormalGraphic = UIGraphic(skin->SkinTexture, skin->TextBoxExRegions, skin->TextBoxExColor);
			}

			DisabledGraphic = NormalGraphic;

			SetFont(skin->TextBoxFont);

			TextSettings.TextColor = skin->TextColor;

			ContentPadding = skin->TextBoxPadding;

			PostInitialize();

			if (m_multiline)
			{
				InitScrollbars(skin);
			}
		}

		void TextBox::Initialize(const TextBoxVisualSettings& settings)
		{
			NormalGraphic = settings.BackgroundGraphic;

			if (settings.Magin.isSet())
				Margin = settings.Magin;

			if (settings.DisabledGraphic.isSet())
				DisabledGraphic = settings.DisabledGraphic;

			if (settings.AlwaysShowHS.isSet())
				m_alwaysShowVS = settings.AlwaysShowVS;

			if (settings.AlwaysShowVS.isSet())
				m_alwaysShowVS = settings.AlwaysShowVS;

			if (settings.ContentPadding.isSet())
				ContentPadding = settings.ContentPadding;

			SetFont(settings.FontRef);

			TextSettings.TextColor = settings.TextColor;

			PostInitialize();

			if (m_multiline)
			{
				assert(settings.HScrollBar.isSet() && settings.VScrollBar.isSet());
				InitScrollbars(settings.HScrollBar, settings.VScrollBar);
			}
		}

		void TextBox::PostInitialize()
		{
			TextSettings.HorizontalAlignment = TextHAlign::Left;
			TextSettings.VerticalAlignment = m_multiline ? TextVAlign::Top : TextVAlign::Middle;

			if (!m_multiline)
			{
				m_size.Y = NormalGraphic.SourceRects[0].Height - Margin.getVerticalSum();
			}
			else
			{
				m_visibleLines = (int)ceilf((float)m_size.Y / m_fontRef->getLineHeightInt());

				// fix up height
				m_size.Y = m_visibleLines * m_fontRef->getLineHeightInt() - Margin.getVerticalSum();
			}

			m_textEdit.eventKeyPress().Bind(this, &TextBox::Keyboard_OnPress);
			m_textEdit.eventKeyPaste().Bind(this, &TextBox::Keyboard_OnPaste);
			m_textEdit.eventEnterPressed.Bind(this, &TextBox::TextEditState_EnterPressed);
			m_textEdit.eventContentChanged.Bind(this, &TextBox::TextEditState_ContentChanged);
			m_textEdit.eventUpPressedSingleline.Bind(this, &TextBox::TextEditState_UpPressedSingleline);
			m_textEdit.eventDownPressedSingleline.Bind(this, &TextBox::TextEditState_DownPressedSingleline);

			m_timerStarted = true;
		}

		void TextBox::InitText(const String& text)
		{
			m_textEdit.Add(text);

			CursorScrolling();
		}

		void TextBox::CursorScrolling()
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
				if (m_hscrollbar)
					m_hscrollbar->SetValue(m_scrollOffset.X);

				if (m_fontRef)
				{
					int offsetY = m_scrollOffset.Y / m_fontRef->getLineHeightInt();
					if (m_hscrollbar && m_hscrollbar->Maximum > 0)
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

					if (m_vscrollbar)
						m_vscrollbar->SetValue(m_scrollOffset.Y / m_fontRef->getLineHeightInt());
				}

			}
		}

		void TextBox::Update(const GameTime* time)
		{
			Apoc3D::Math::Rectangle txtArea = GetTextArea();
			m_visibleLines = (int)ceilf((float)txtArea.Height / m_fontRef->getLineHeightInt());

			CheckFocus();
			Point cursorPos = m_textEdit.getCursorPosition();

			if (HasFocus && !ReadOnly)
			{
				m_textEdit.Update(time);
			}
			
			Mouse* mouse = InputAPIManager::getSingleton().getMouse();
			Apoc3D::Math::Rectangle textArea = GetTextArea();
			if (!ReadOnly && textArea.Contains(mouse->GetPosition()))
			{
				if (mouse->IsLeftPressedState())
				{
					Point mp = mouse->GetPosition() + m_scrollOffset;

					int32 lineIndex = (mp.Y - textArea.Y) / m_fontRef->getLineHeightInt();
					int32 dx = mp.X - textArea.X;
					m_textEdit.MoveCursorTo(Point(0, lineIndex));

					int32 xpos = m_fontRef->FitSingleLineString(m_textEdit.getCurrentLine(), Math::Max(0, dx));
					m_textEdit.MoveCursorTo(Point(xpos, lineIndex));

					CursorScrolling();

					m_cursorVisible = true;
					m_timer = 0.5f;

					if (m_clickChecker.Check(mouse))
					{
						if (m_textEdit.isExternalSelecting())
							m_textEdit.EndExternalSelection();

						// double click : select word around current position
						m_textEdit.SetSelectionFromCursorWord();
					}
					else
					{
						if (mouse->IsLeftPressed())
						{
							m_textEdit.StartExternalSelection();
						}
						else if (m_textEdit.isExternalSelecting())
						{
							m_textEdit.SetSelectionEndFromCursor();
						}
					}
				}
				else if (m_textEdit.isExternalSelecting() && mouse->IsLeftUp())
				{
					m_textEdit.EndExternalSelection();
				}
			}

			if (m_multiline)
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

			m_clickChecker.Update(time, mouse);
		}
		void TextBox::UpdateScrollbars(const GameTime* time)
		{
			Mouse* mouse = InputAPIManager::getSingleton().getMouse();

			if (m_vscrollbar)
			{
				m_vscrollbar->Maximum = Math::Max(0, m_textEdit.getLineCount() - m_visibleLines);
				m_vscrollbar->Step = Math::Max(1, m_hscrollbar->Maximum / 15);

				if (m_vscrollbar->Maximum > 0 && mouse->getDZ() && getAbsoluteArea().Contains(mouse->GetPosition()))
				{
					m_vscrollbar->SetValue(m_vscrollbar->getValue() + mouse->getDZ() / 60);
				}

				m_scrollOffset.Y = m_vscrollbar->getValue() * m_fontRef->getLineHeightInt();
			}

			int32 maxLineWidth = 0;
			const List<String>& lines = m_textEdit.getLines();

			for (const String& line : lines)
			{
				Point lineSize = m_fontRef->MeasureString(line);
				if (lineSize.X > maxLineWidth)
					maxLineWidth = lineSize.X;
			}

			if (m_hscrollbar)
			{
				Apoc3D::Math::Rectangle	area = GetContentArea();

				m_hscrollbar->Maximum = Math::Max(0, maxLineWidth - area.Width);
				m_hscrollbar->Step = Math::Max(1, m_hscrollbar->Maximum / 15);
				m_scrollOffset.X = m_hscrollbar->getValue();
			}

			UpdateScrollBarsGeneric(getArea(), time);
		}
		void TextBox::CheckFocus()
		{
			Apoc3D::Math::Rectangle area = getAbsoluteArea();

			Mouse* mouse = InputAPIManager::getSingleton().getMouse();
			if (mouse->IsLeftPressed())
			{
				if (area.Contains(mouse->GetPosition()))
				{
					HasFocus = true;
				}
				else
				{
					HasFocus = false;
				}
			}

		}
		void TextBox::Draw(Sprite* sprite)
		{
			const UIGraphic& g = Enabled ? NormalGraphic : DisabledGraphic;

			g.Draw(sprite, Margin.InflateRect(getAbsoluteArea()));

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

			DrawScrollBars(sprite);
		}

		void TextBox::_DrawText(Sprite* sprite)
		{
			bool canShowCursor = HasFocus && !ReadOnly && m_cursorVisible && ParentFocused;
			bool canShowSelection = !ReadOnly;

			Apoc3D::Math::Rectangle contentArea = GetTextArea();

			Point cursorPos = m_textEdit.getCursorPosition();
			int32 cursorLeft = - m_fontRef->MeasureString(L"|").X / 4;
			
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

				int32 lss, lse;
				if (canShowSelection && m_textEdit.GetLineSelectionRegion(0, lss, lse))
				{
					TextSettings.DrawBG(sprite, m_fontRef, text, lss, lse, contentArea.getTopLeft() - m_scrollOffset, contentArea.getSize(), CV_LightBlue);
				}
				
				TextSettings.Draw(sprite, m_fontRef, text, contentArea.getTopLeft() - m_scrollOffset, contentArea.getSize(), 0xff);

				if (canShowCursor)
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
				for (int32 i = 0; i < lines.getCount(); i++)
				{
					const String& line = lines[i];

					int32 lss, lse;
					if (canShowSelection && m_textEdit.GetLineSelectionRegion(i, lss, lse))
					{
						TextSettings.DrawBG(sprite, m_fontRef, line, lss, lse, lineOffset - m_scrollOffset + contentArea.getTopLeft(), contentArea.getSize(), CV_LightBlue);
					}

					TextSettings.Draw(sprite, m_fontRef, line, lineOffset - m_scrollOffset + contentArea.getTopLeft(), contentArea.getSize(), 0xff);
					//m_fontRef->DrawString(sprite, line, m_textOffset + m_lineOffset - m_scrollOffset + baseOffset, m_skin->TextColor);

					lineOffset.Y += m_fontRef->getLineHeightInt();
				}

				if (canShowCursor)
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
			return ContentPadding.ShrinkRect(GetContentArea());
		}

		void TextBox::TextEditState_EnterPressed() { eventEnterPressed.Invoke(this); }
		void TextBox::TextEditState_ContentChanged() { eventContentChanged.Invoke(this); }
		void TextBox::TextEditState_UpPressedSingleline() { eventUpPressedSingleline.Invoke(this); }
		void TextBox::TextEditState_DownPressedSingleline() { eventDownPressedSingleline.Invoke(this); }


		void TextBox::SetText(const String& text)
		{
			m_textEdit.SetText(text);
			m_scrollOffset = Point(0, 0);
		}

		void TextBox::Keyboard_OnPress(KeyboardKeyCode code, KeyboardEventsArgs e)
		{
			CursorScrolling();
			m_cursorVisible = true;
			m_timerStarted = true;
		}
		void TextBox::Keyboard_OnPaste(String value)
		{
			if (value.size())
			{
				Point textSize = m_fontRef->MeasureString(m_textEdit.getText());
				m_cursorOffset.Y += (int)(textSize.Y / m_fontRef->getLineHeightInt()) - 1;

				CursorScrolling();
			}
		}

		void TextBox::vScrollbar_OnChangeValue(ScrollBar* ctrl)
		{
			HasFocus = true;
		}
		void TextBox::hScrollbar_OnChangeValue(ScrollBar* ctrl)
		{
			HasFocus = true;
		}

	}
}