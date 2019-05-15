#include "Text.h"
#include "FontManager.h"

#include "apoc3d/Core/AppTime.h"
#include "apoc3d/Input/InputAPI.h"
#include "apoc3d/Input/Mouse.h"
#include "apoc3d/Graphics/RenderSystem/Sprite.h"
#include "apoc3d/Graphics/RenderSystem/RenderDevice.h"
#include "apoc3d/Graphics/RenderSystem/RenderStateManager.h"
#include "apoc3d/Utility/StringUtils.h"

#include "StyleSkin.h"

#include "Bar.h"

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
			{
				TextSettings.TextColor = skin->TextColor;
				TextSettings.TextColorDisabled = skin->TextColorDisabled;
			}
			
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
			m_hyperLinks.Clear();

			bool hasHyperLink = m_text.find(ControlCodes::Label_HyperLinkStart) != String::npos;

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
				m_size.Y = static_cast<int>(m_fontRef->getTextBackgroundHeight(m_lines.getCount())) + Math::Max(0, (m_lines.getCount() - 1)*AdditionalLineSpacing);
			}


			if (hasHyperLink)
			{
				bool isInLink = false;
				HyperLinkRange hlr;

				for (int32 i = 0; i < m_lines.getCount(); i++)
				{
					String linkTextLine;

					const String& line = m_lines[i];

					const wchar_t* str = line.c_str();
					for (int32 j = 0; j < (int32)line.length(); j++)
					{
						if (str[j] == ControlCodes::Label_HyperLinkStart)
						{
							if (GotoNextCharacter(i,j))
							{
								hlr.ID = ~(uint16)(str[j]);

								if (GotoNextCharacter(i,j))
								{
									isInLink = true;

									linkTextLine.clear();
									linkTextLine.append(1, str[j]);
									hlr.LinkTextLines.Clear();
									hlr.StartLine = i;
									hlr.StartIndex = j;
								}
							}
						}
						else if (str[j] == ControlCodes::Label_HyperLinkEnd)
						{
							isInLink = false;

							// goto previous character
							hlr.EndLine = i;
							hlr.EndIndex = j;

							GotoPreviousCharacter(hlr.EndLine, hlr.EndIndex);

							if (linkTextLine.size())
								hlr.LinkTextLines.Add(linkTextLine);

							m_hyperLinks.Add(hlr);
						}
						else if (isInLink)
						{
							linkTextLine.append(1, str[j]);
						}


						if (i >= m_lines.getCount())
							break;
					}

					if (linkTextLine.size())
						hlr.LinkTextLines.Add(linkTextLine);

				}
			}
		}

		bool Label::GotoNextCharacter(int32& lineIdx, int32& chIdx)
		{
			chIdx++;

			if (chIdx >= (int32)m_lines[lineIdx].length())
			{
				chIdx = 0;
				lineIdx++;

				while (lineIdx < m_lines.getCount() - 1 && m_lines[lineIdx].length() == 0)
				{
					lineIdx++;
				}

				if (lineIdx >= m_lines.getCount() || chIdx >= (int32)m_lines[lineIdx].length())
					return false;
			}
			return true;
		}
		void Label::GotoPreviousCharacter(int32& lineIdx, int32& chIdx)
		{
			chIdx--;

			if (chIdx < 0)
			{
				lineIdx--;

				while (lineIdx >= 0 && m_lines[lineIdx].length() == 0)
				{
					lineIdx--;
				}

				if (lineIdx < 0)
				{
					lineIdx = 0;
					chIdx = 0;
				}
				else
				{
					chIdx = Math::Max((int32)m_lines[lineIdx].length() - 1, 0);
				}
			}
		}

		void Label::Update(const AppTime* time)
		{
			UpdateEvents_StandardButton(m_mouseHover, m_mouseDown, getAbsoluteArea(),
				&Label::OnMouseHover, &Label::OnMouseOut, &Label::OnPress, &Label::OnRelease);

			Mouse* mouse = InputAPIManager::getSingleton().getMouse();
			Point cursorPos = mouse->GetPosition();
			Point drawPos = GetAbsolutePosition();

			m_mouseHoverLinks = false;

			for (auto& e : m_hyperLinks)
			{
				bool isInteracting = Visible && Enabled && IsInteractive;
				
				if (isInteracting)
				{
					bool isInteractingArea = false;

					bool firstArea = true;
					Apoc3D::Math::Rectangle totalArea;
					for (int32 i = e.StartLine; i <= e.EndLine;i++)
					{
						Point lineSize;
						Point lineOff;
						e.GetLineMetrics(i, m_lines[i], m_fontRef, getLineHeight(), TextSettings, lineSize, lineOff);

						Apoc3D::Math::Rectangle area(drawPos + lineOff, lineSize);

						if (area.Contains(cursorPos))
						{
							isInteractingArea = true;

							if (firstArea)
								totalArea = area;
							else
								totalArea = Apoc3D::Math::Rectangle::Union(totalArea, area);

							m_mouseHoverLinks = true;
							break;
						}
					}

					if (isInteractingArea)
					{
						m_mouseHoverLinks = true;
						m_mouseHoverLinkArea = totalArea;
					}

					isInteracting &= isInteractingArea;
				}
				if (isInteracting)
				{
					if (!e.MouseHover)
					{
						e.MouseHover = true;
						eventHyperlinkMouseHover.Invoke(this, e.ID);
					}
					if (!e.MouseDown && mouse && mouse->IsLeftPressed())
					{
						e.MouseDown = true;
						eventHyperlinkPress.Invoke(this, e.ID);
					}
					else if (e.MouseDown && mouse && mouse->IsLeftUp())
					{
						e.MouseDown = false;
						eventHyperlinkRelease.Invoke(this, e.ID);
					}
				}
				else
				{
					if (e.MouseHover)
					{
						eventHyperlinkMouseOut.Invoke(this, e.ID);
					}
					e.MouseDown = false;
					e.MouseHover = false;
				}
			}
			

		}
		
		void Label::Draw(Sprite* sprite)
		{
			Point drawPos = GetAbsolutePosition();
			if (m_lines.getCount() <= 1)
			{
				TextSettings.Draw(sprite, m_fontRef, m_text, drawPos, m_size, Enabled);
			}
			else
			{
				for (const String& line : m_lines)
				{
					TextSettings.Draw(sprite, m_fontRef, line, drawPos, m_size, Enabled);

					drawPos.Y += getLineHeight();
				}
			}

			drawPos = GetAbsolutePosition();
			for (const auto& e : m_hyperLinks)
			{
				for (int32 i = 0; i < e.LinkTextLines.getCount(); i++)
				{
					int32 lineIdx = i + e.StartLine;
					if (!m_lines.isIndexInRange(lineIdx))
						break;

					Point lineSize;
					Point lineOff;
					e.GetLineMetrics(lineIdx, m_lines[lineIdx], m_fontRef, getLineHeight(), TextSettings, lineSize, lineOff);

					TextSettings.DrawHyperLinkText(sprite, m_fontRef, e.LinkTextLines[i], lineOff + drawPos, Enabled, e.MouseHover, 1);
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

		void Label::setSize(const Point& v)
		{
			m_size.Y = v.Y;
			setWidth(v.X);
		}
		void Label::setWidth(int v) 
		{
			if (m_size.X != v)
			{
				m_size.X = v;
				UpdateText();
			}
		}

		int32 Label::getLineHeight()
		{
			return AdditionalLineSpacing + m_fontRef->getLineHeightInt();
		}

		void Label::HyperLinkRange::GetLineMetrics(int32 i, const String& line, Font* fnt, int32 lineHeight, TextRenderSettings& ts, Point& size, Point& off) const
		{
			Point lineSize = fnt->MeasureString(line);
			Point tofs = ts.GetTextOffset(lineSize, lineSize);

			if (i == StartLine)
			{
				off.X = fnt->MeasureString(line, 0, StartIndex - 2).X;
			}
			else
			{
				off.X = 0;
			}
			off.Y = lineHeight * i;

			if (i == StartLine && i == EndLine)
			{
				size = fnt->MeasureString(line, StartIndex, EndIndex);
			}
			else if (i == EndLine)
			{
				size = fnt->MeasureString(line, 0, EndIndex);
			}
			else if (i == StartLine)
			{
				size = fnt->MeasureString(line, StartIndex, (int32)line.size());
			}
			else
			{
				size = fnt->MeasureString(line);
			}
		}
		
		/************************************************************************/
		/*   TextBox                                                            */
		/************************************************************************/
		TextBox::TextBoxEvent TextBox::eventAnyContentChanged;

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
			TextSettings.TextColorDisabled = skin->TextColorDisabled;

			TextSettings.TextSelectionColor = skin->TextSelectionColor;
			TextSettings.TextSelectionColorDisabled = skin->TextSelectionColorDisabled;

			BackgroundTextSettings.TextColor = TextSettings.TextColor;
			BackgroundTextSettings.TextColorDisabled = TextSettings.TextColorDisabled;

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

			if (settings.Margin.isSet())
				Margin = settings.Margin;

			if (settings.DisabledGraphic.isSet())
				DisabledGraphic = settings.DisabledGraphic;
			
			if (settings.AlwaysShowHS.isSet())
				m_alwaysShowVS = settings.AlwaysShowVS;
			if (settings.AlwaysShowVS.isSet())
				m_alwaysShowVS = settings.AlwaysShowVS;

			SetFont(settings.FontRef);

			TextSettings.TextColor = settings.TextColor;
			TextSettings.TextColorDisabled = settings.TextColorDisabled.isSet() ? settings.TextColorDisabled : settings.TextColor;

			TextSettings.TextSelectionColor = settings.TextSelectionColor;
			TextSettings.TextSelectionColorDisabled = settings.TextSelectionColorDisabled.isSet() ? settings.TextSelectionColorDisabled : settings.TextSelectionColor;

			BackgroundTextSettings.TextColor = TextSettings.TextColor;
			BackgroundTextSettings.TextColorDisabled = TextSettings.TextColorDisabled;

			if (settings.ContentPadding.isSet())
				ContentPadding = settings.ContentPadding;

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

			BackgroundTextSettings.HorizontalAlignment = TextSettings.HorizontalAlignment;
			BackgroundTextSettings.VerticalAlignment = TextSettings.VerticalAlignment;

			int32 tcAlpha = CV_GetColorA(BackgroundTextSettings.TextColor) / 3;
			BackgroundTextSettings.TextColor = CV_RepackAlpha(BackgroundTextSettings.TextColor, tcAlpha);

			tcAlpha = CV_GetColorA(BackgroundTextSettings.TextColorDisabled) / 3;
			BackgroundTextSettings.TextColorDisabled = CV_RepackAlpha(BackgroundTextSettings.TextColorDisabled, tcAlpha);


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

		void TextBox::Update(const AppTime* time)
		{
			Apoc3D::Math::Rectangle textArea = GetTextArea();
			m_visibleLines = (int)ceilf((float)textArea.Height / m_fontRef->getLineHeightInt());

			if (!Enabled)
				return;

			CheckFocus();
			Point cursorPos = m_textEdit.getCursorPosition();

			if (HasFocus && !ReadOnly)
			{
				m_textEdit.Update(time);
			}
			
			Mouse* mouse = InputAPIManager::getSingleton().getMouse();
			m_mouseHover = IsInteractive && getAbsoluteArea().Contains(mouse->GetPosition());

			if (HasFocus && !ReadOnly && textArea.Contains(mouse->GetPosition()))
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
				m_timer -= time->ElapsedTime;
				if (m_timer < 0)
				{
					m_timer = 0.5f;
					m_cursorVisible = !m_cursorVisible;
				}
			}

			m_clickChecker.Update(time, mouse);
		}
		void TextBox::UpdateScrollbars(const AppTime* time)
		{
			Mouse* mouse = InputAPIManager::getSingleton().getMouse();

			if (m_vscrollbar)
			{
				m_vscrollbar->VisibleRange = m_visibleLines;
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
			if (m_textEdit.isExternalSelecting())
			{
				// allow dragging out of area
				HasFocus = true;
				return;
			}

			Mouse* mouse = InputAPIManager::getSingleton().getMouse();
			if (IsInteractive)
			{
				if (mouse->IsLeftPressed())
				{
					Apoc3D::Math::Rectangle area = getAbsoluteArea();

					HasFocus = area.Contains(mouse->GetPosition());
				}
			}
			else
			{
				HasFocus = false;
			}
		}

		void TextBox::Draw(Sprite* sprite)
		{
			const UIGraphic& g = Enabled ? NormalGraphic : DisabledGraphic;

			Apoc3D::Math::Rectangle dstRect = getAbsoluteArea();
			g.Draw(sprite, Margin.InflateRect(dstRect));

			{
				ScissorTestScope sts(dstRect, sprite);

				if (!sts.isEmpty())
					_DrawText(sprite);
			}
			
			DrawScrollBars(sprite);
		}

		void TextBox::_DrawText(Sprite* sprite)
		{
			//bool canShowCursor = HasFocus && !ReadOnly && m_cursorVisible && ParentFocused;
			bool canShowSelection = !ReadOnly;

			Apoc3D::Math::Rectangle contentArea = GetTextArea();

			if (!m_multiline)
			{
				const String& text = m_textEdit.getText();
				
				int32 lss, lse;
				if (canShowSelection && m_textEdit.GetLineSelectionRegion(0, lss, lse))
				{
					TextSettings.DrawBG(sprite, m_fontRef, text, lss, lse, contentArea.getTopLeft() - m_scrollOffset, contentArea.getSize(), CV_LightBlue);
				}
				
				TextSettings.Draw(sprite, m_fontRef, text, contentArea.getTopLeft() - m_scrollOffset, contentArea.getSize(), Enabled);
			}
			else
			{
				const List<String>& lines = m_textEdit.getLines();

				Point lineOffset = Point(0, 0);
				for (int32 i = 0; i < lines.getCount(); i++)
				{
					const String& line = lines[i];

					int32 lss, lse;
					if (canShowSelection && m_textEdit.GetLineSelectionRegion(i, lss, lse))
					{
						TextSettings.DrawBG(sprite, m_fontRef, line, lss, lse, lineOffset - m_scrollOffset + contentArea.getTopLeft(), contentArea.getSize(), CV_LightBlue);
					}

					TextSettings.Draw(sprite, m_fontRef, line, lineOffset - m_scrollOffset + contentArea.getTopLeft(), contentArea.getSize(), Enabled);

					lineOffset.Y += m_fontRef->getLineHeightInt();
				}
			}

			if (!HasFocus && BackgroundText.size() && m_textEdit.getText().empty())
			{
				BackgroundTextSettings.Draw(sprite, m_fontRef, BackgroundText, contentArea.getTopLeft() - m_scrollOffset, contentArea.getSize(), Enabled);
			}

			DrawCursor(sprite);
		}
		void TextBox::DrawCursor(Sprite* sprite)
		{
			Apoc3D::Math::Rectangle contentArea = GetTextArea();

			bool canShowCursor = HasFocus && !ReadOnly && m_cursorVisible && IsInteractive;

			Point cursorPos = m_textEdit.getCursorPosition();

			Apoc3D::Math::Rectangle cursorDstRect;
			cursorDstRect.Width = 1;
			cursorDstRect.Height = Math::Min(m_fontRef->getLineHeightInt(), contentArea.Height);
			
			if (!m_multiline)
			{
				const String& text = m_textEdit.getText();
				if (cursorPos.X > 0)
				{
					m_cursorOffset.X = m_fontRef->MeasureString(text.substr(0, cursorPos.X)).X;
				}
				else
				{
					m_cursorOffset.X = 0;
				}

				m_cursorOffset.Y = 0;

				if (canShowCursor)
				{
					//TextSettings.Draw(sprite, m_fontRef, L"|", contentArea.getTopLeft() + m_cursorOffset - m_scrollOffset, contentArea.getSize(), Enabled);

					cursorDstRect.setPosition(contentArea.getTopLeft() + m_cursorOffset - m_scrollOffset);

					// vertical center 
					cursorDstRect.Y += (contentArea.Height - cursorDstRect.Height) / 2;
				}

			}
			else
			{
				const List<String>& lines = m_textEdit.getLines();

				m_cursorOffset.X = m_fontRef->MeasureString(
					lines[cursorPos.Y].substr(0, cursorPos.X)).X;
				m_cursorOffset.Y = m_fontRef->getLineHeightInt() * cursorPos.Y + 1;

				if (canShowCursor)
				{
					cursorDstRect.setPosition(m_cursorOffset - m_scrollOffset + contentArea.getTopLeft());
					//TextSettings.Draw(sprite, m_fontRef, L"|", m_cursorOffset - m_scrollOffset + contentArea.getTopLeft(), contentArea.getSize(), Enabled);
				}
			}

			if (canShowCursor)
			{
				sprite->Draw(SystemUI::GetWhitePixel(), cursorDstRect, nullptr, TextSettings.TextColor);
			}
		}

		Apoc3D::Math::Rectangle TextBox::GetTextArea() const
		{
			return ContentPadding.ShrinkRect(GetContentArea());
		}

		void TextBox::TextEditState_UpPressedSingleline() { eventUpPressedSingleline.Invoke(this); }
		void TextBox::TextEditState_DownPressedSingleline() { eventDownPressedSingleline.Invoke(this); }
		void TextBox::TextEditState_EnterPressed() { eventEnterPressed.Invoke(this); }
		void TextBox::TextEditState_ContentChanged()
		{
			eventContentChanged.Invoke(this);
			if (RaiseGlobalEvent)
				eventAnyContentChanged.Invoke(this);
		}

		void TextBox::SetText(const String& text)
		{
			SetText(text, false);
		}
		void TextBox::SetText(const String& text, bool keepCursorAndScroll)
		{
			m_textEdit.SetText(text, keepCursorAndScroll);

			if (!keepCursorAndScroll)
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