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
#include "KeyboardHelper.h"

#include "apoc3d/Core/GameTime.h"
#include "apoc3d/Input/InputAPI.h"
#include "apoc3d/Utility/StringUtils.h"

using namespace Apoc3D::Collections;
using namespace Apoc3D::Utility;

#if APOC3D_PLATFORM == APOC3D_PLATFORM_WINDOWS
#include <windows.h>
#endif

namespace Apoc3D
{
	namespace UI
	{
		const KeyboardKeyCode InputKeys[] = 
		{
			KEY_0, KEY_1, KEY_2, KEY_3, KEY_4, KEY_5, KEY_6, KEY_7, KEY_8, KEY_9,

			KEY_NUMPAD0, KEY_NUMPAD1, KEY_NUMPAD2, KEY_NUMPAD3, KEY_NUMPAD4, KEY_NUMPAD5, KEY_NUMPAD6, KEY_NUMPAD7, KEY_NUMPAD8, KEY_NUMPAD9,

			KEY_A, KEY_B, KEY_C, KEY_D, KEY_E, KEY_F, KEY_G, 
			KEY_H, KEY_I, KEY_J, KEY_K, KEY_L, KEY_M, KEY_N,
			KEY_O, KEY_P, KEY_Q, KEY_R, KEY_S, KEY_T,
			KEY_U, KEY_V, KEY_W, KEY_X, KEY_Y, KEY_Z,

			KEY_SPACE,
			KEY_HOME,
			KEY_END,
			KEY_LEFT,
			KEY_RIGHT,
			KEY_UP,
			KEY_DOWN,
			KEY_BACK,
			KEY_DELETE,

			KEY_RETURN,
			KEY_NUMPADENTER,

			KEY_ADD,
			KEY_NUMPADEQUALS,
			KEY_DECIMAL,
			KEY_SUBTRACT,
			KEY_MULTIPLY,
			KEY_DIVIDE,
			KEY_COMMA,
			KEY_PERIOD,
			KEY_SLASH,
			KEY_BACKSLASH,
			KEY_MINUS,
			KEY_LBRACKET,
			KEY_RBRACKET,
			KEY_SEMICOLON,
			KEY_GRAVE,

			KEY_TAB

		};

		void KeyboardHelper::Update(const GameTime* time)
		{
			Keyboard* kb = InputAPIManager::getSingleton().getKeyboard();
			if (kb)
			{
				bool isShiftDown = kb->IsPressing(KEY_LSHIFT) || kb->IsPressing(KEY_RSHIFT);
				if (!m_shiftDown && isShiftDown)
				{
					eventKeyboardSelectionStart.Invoke();
				}
				else if (m_shiftDown && !isShiftDown)
				{
					eventKeyboardSelectionEnd.Invoke();
				}
				m_shiftDown = isShiftDown;

				if (kb->IsPressing(KEY_LCONTROL) || kb->IsPressing(KEY_RCONTROL))
				{
					if (kb->IsKeyDown(KEY_V))
					{
						m_pasting = true;
						if (eventKeyPaste.getCount() > 0)
						{
#if APOC3D_PLATFORM == APOC3D_PLATFORM_WINDOWS
							IDataObject* dataObj;
							HRESULT hr = OleGetClipboard(&dataObj);
							if (SUCCEEDED(hr))
							{
								IEnumFORMATETC* ienum;
								hr = dataObj->EnumFormatEtc(DATADIR_GET, &ienum);
								if (SUCCEEDED(hr))
								{
									FORMATETC fmts[16];
									ULONG amount;
									ienum->Next(16, fmts, &amount);

									
									for (ULONG i=0;i<amount;i++)
									{
										FORMATETC& fmt = fmts[i];
										if (fmt.cfFormat == CF_UNICODETEXT)
										{
											STGMEDIUM medium;
											hr = dataObj->GetData(&fmt, &medium);
											if (SUCCEEDED(hr))
											{
												if (medium.tymed == TYMED_HGLOBAL)
												{
													void* src = GlobalLock(medium.hGlobal);

													String strText = reinterpret_cast<const wchar_t*>(src);

													eventKeyPaste.Invoke(strText);

													GlobalFree(medium.hGlobal);
												}
												
												//medium.
												if (medium.pUnkForRelease)
												{
													medium.pUnkForRelease->Release();
												}
												else
												{
													ReleaseStgMedium(&medium);
												}
											}

											break;
										}
										else if (fmt.cfFormat == CF_TEXT)
										{
											
											break;
										}
									}
									ienum->Release();
								}
								dataObj->Release();
							}
							
#endif
						}
						return;
					}
					else if (kb->IsKeyUp(KEY_V))
					{
						m_pasting = false;
					}
				}
				else
				{
					m_pasting = false;
				}

				KeyboardEventsArgs eventArg;

				if (kb->IsPressing(KEY_LMENU) || kb->IsPressing(KEY_RMENU))
				{
					eventArg.AltDown = true;
				}
				if (kb->IsPressing(KEY_LCONTROL) || kb->IsPressing(KEY_RCONTROL))
				{
					eventArg.ControlDown = true;
				}
#if APOC3D_PLATFORM == APOC3D_PLATFORM_WINDOWS
				eventArg.CapsLock =  (GetKeyState(VK_CAPITAL) & 0x8001) != 0;
#endif
				eventArg.ShiftDown |= m_shiftDown;

				for (KeyboardKeyCode testKey : InputKeys)
				{
					if (kb->IsKeyDown(testKey))
					{
						if (m_pasting && testKey == KEY_V)
						{
							return;
						}
						
						eventKeyPress.Invoke(testKey, eventArg);
						
						m_previousKey = m_currentKey;

						m_currentKey = testKey;
						if (m_currentKey != m_previousKey)
						{
							m_pressingTime = 0;
							m_timerStarted = true;
						}

					}
					else if (kb->IsKeyUp(testKey))
					{
						if (m_currentKey == testKey)
							m_currentKey = KEY_UNASSIGNED;

						m_timerStarted = false;
						m_pressingTime = 0;

						eventKeyRelease.Invoke(testKey, eventArg);
					}
				}
				//if (m_currentKey == m_previousKey && m_currentKey != KEY_UNASSIGNED)
				{
					if (m_timerStarted)
					{
						m_pressingTime += time->getElapsedTime();
						if (m_pressingTime > 15 * 25 * 0.001f)
						{
							m_pressingTime -= 0.1f;
							
							eventKeyPress.Invoke(m_currentKey, eventArg);
						}
					}
					
				}


			}
			else
			{
				m_shiftDown = false;
			}
		}

		/************************************************************************/
		/* TextEditState                                                        */
		/************************************************************************/
		
		TextEditState::TextEditState(bool multiline, const Point& cursorPos)
			: m_cursorLocation(cursorPos), m_multiline(multiline)
		{
			m_keyboard.eventKeyPress.Bind(this, &TextEditState::Keyboard_OnPress);
			m_keyboard.eventKeyPaste.Bind(this, &TextEditState::Keyboard_OnPaste);

			m_keyboard.eventKeyboardSelectionStart.Bind(this, &TextEditState::StartExternalSelection);
			m_keyboard.eventKeyboardSelectionEnd.Bind(this, &TextEditState::EndExternalSelection);

			m_lines.Add(L"");
		}

		TextEditState::~TextEditState()
		{
			m_keyboard.eventKeyboardSelectionStart.Unbind(this, &TextEditState::StartExternalSelection);
			m_keyboard.eventKeyboardSelectionEnd.Unbind(this, &TextEditState::EndExternalSelection);

			m_keyboard.eventKeyPress.Unbind(this, &TextEditState::Keyboard_OnPress);
			m_keyboard.eventKeyPaste.Unbind(this, &TextEditState::Keyboard_OnPaste);
		}

		void TextEditState::Update(const GameTime* time)
		{
			m_keyboard.Update(time);
		}

		void ReplaceTabs(const List<String>& lines)
		{
			for (String& line : lines)
			{
				for (size_t j = 0; j < line.size(); j++)
				{
					if (line[j] == '\t')
					{
						line[j] = ' ';
						line.insert(j, L"   ");
					}
				}
			}
		}

		void TextEditState::SetText(const String& text)
		{
			if (!m_multiline)
			{
				m_text = text;
			}
			else
			{
				m_lines.Clear();
				StringUtils::Split(text, m_lines, L"\n\r");
				ReplaceTabs(m_lines);
			}

			Sync();

			m_cursorLocation.X = 0;
			m_cursorLocation.Y = 0;
			
			ClearSelectionRegion();
		}

		void TextEditState::Add(const String& newText)
		{
			ClampCursorPos(m_cursorLocation);
			ClampCursorPos(m_selectionStart);
			ClampCursorPos(m_selectionEnd);

			if (hasSelection())
				EraseSelectedText();

			if (!m_multiline)
			{
				m_text = m_text.insert(m_cursorLocation.X, newText);
				m_cursorLocation.X += newText.size();
			}
			else
			{
				String& currentLine = m_lines[m_cursorLocation.Y];

				if (newText.find_first_of('\n', 0) != String::npos)
				{
					String linePre = currentLine.substr(0, m_cursorLocation.X);
					String linePost = m_cursorLocation.X < (int32)currentLine.size() ? currentLine.substr(m_cursorLocation.X) : L"";

					// new line
					List<String> lines;
					StringUtils::Split(newText, lines, L"\n\r");

					ReplaceTabs(lines);

					lines[0] = linePre + lines[0];
					lines[lines.getCount() - 1].append(linePost);

					m_lines[m_cursorLocation.Y] = lines[0];

					if (lines.getCount()>1)
						m_lines.InsertArray(m_cursorLocation.Y + 1, lines.getElements()+1, lines.getCount() - 1);

					m_cursorLocation.Y += lines.getCount() - 1;
					m_cursorLocation.X = (int32)lines[lines.getCount()-1].size() - (int32)linePost.size();
				}
				else
				{
					m_lines[m_cursorLocation.Y] = m_lines[m_cursorLocation.Y].insert(m_cursorLocation.X, newText);
					m_cursorLocation.X += newText.size();
				}
			}

			Sync();

			ClampCursorPos(m_cursorLocation);
			ClampCursorPos(m_selectionStart);
			ClampCursorPos(m_selectionEnd);
		}


		void TextEditState::Keyboard_OnPaste(String value)
		{
			if (value.size())
			{
				Add(value);

				eventContentChanged.Invoke();
			}
		}
		void TextEditState::Keyboard_OnPress(KeyboardKeyCode code, KeyboardEventsArgs e)
		{
			bool changed = false;
			bool checkKeyboardSelection = false;

			
			switch (code)
			{
				case KEY_LEFT:
				{
					checkKeyboardSelection = true;

					String* previousLine, *nextLine;
					String& currentLine = GetLines(previousLine, nextLine);

					if (e.ControlDown)
					{
						size_t spacePos = String::npos;
						if (m_cursorLocation.X > 1)
							spacePos = currentLine.find_last_of(' ', m_cursorLocation.X - 2);

						if (spacePos == String::npos)
						{
							if (m_cursorLocation.X != 0)
								m_cursorLocation.X = 0;
							else if (previousLine)
							{
								m_cursorLocation.Y--;
								m_cursorLocation.X = (int)previousLine->size();
							}
						}
						else
							m_cursorLocation.X = spacePos+1;
					}
					else
					{
						if (!m_multiline)
							m_cursorLocation.X--;
						else
						{
							if (m_cursorLocation.X > 0)
								m_cursorLocation.X--;
							else if (previousLine)
							{
								m_cursorLocation.X = (int)previousLine->size();
								m_cursorLocation.Y--;
							}
						}
					}

					break;
				}
				case KEY_RIGHT:
				{
					checkKeyboardSelection = true;

					String* previousLine, *nextLine;
					String& currentLine = GetLines(previousLine, nextLine);

					if (e.ControlDown)
					{
						size_t spacePos = currentLine.find_first_of(' ', m_cursorLocation.X + 1);

						if (spacePos == String::npos)
						{
							if (m_cursorLocation.X != (int)currentLine.size())
								m_cursorLocation.X = (int)currentLine.size();
							else if (nextLine)
							{
								m_cursorLocation.X = 0;
								m_cursorLocation.Y++;
							}
						}
						else
							m_cursorLocation.X = spacePos+1;
					}
					else
					{
						if (!m_multiline)
							m_cursorLocation.X++;
						else
						{
							if (m_cursorLocation.X < (int)currentLine.size())
								m_cursorLocation.X++;
							else if (nextLine)
							{
								m_cursorLocation.X = 0;
								m_cursorLocation.Y++;
							}
						}
					}

					break;
				}
				case KEY_UP:
				{
					checkKeyboardSelection = true;

					if (!m_multiline)
						eventUpPressedSingleline.Invoke();

					m_cursorLocation.Y--;

					break;
				}
				case KEY_DOWN:
				{
					checkKeyboardSelection = true;

					if (!m_multiline)
						eventDownPressedSingleline.Invoke();

					m_cursorLocation.Y++;

					break;
				}
				case KEY_BACK:
				{
					if (hasSelection())
					{
						EraseSelectedText();
						changed = true;
					}
					else
					{
						if (!m_multiline)
						{
							if (m_cursorLocation.X > 0 && m_cursorLocation.X <= (int32)m_text.size())
							{
								if (m_cursorLocation.X > 1 && m_text.substr(m_cursorLocation.X - 2, 2) == L"\r\n")
								{
									m_text = m_text.erase(m_cursorLocation.X - 2, 2);
									m_cursorLocation.X -= 2;
								}
								else
								{
									m_text = m_text.erase(m_cursorLocation.X - 1, 1);
									m_cursorLocation.X--;
								}
								changed = true;
							}
						}
						else
						{
							String* previousLine, *nextLine;
							String& currentLine = GetLines(previousLine, nextLine);

							if (m_cursorLocation.X > 0)
							{
								currentLine = currentLine.erase(m_cursorLocation.X - 1, 1);
								m_cursorLocation.X -= 1;

								changed = true;
							}
							else
							{
								if (previousLine)
								{
									m_cursorLocation.X = (int)previousLine->size();
									*previousLine += currentLine;
									m_lines.RemoveAt(m_cursorLocation.Y);
									m_cursorLocation.Y -= 1;

									changed = true;
								}
							}
						}
					}

					if (changed) Sync();
					break;
				}
				case KEY_DELETE:
				{
					if (hasSelection())
					{
						EraseSelectedText();
						changed = true;
					}
					else
					{
						if (!m_multiline)
						{
							if (m_cursorLocation.X < (int)m_text.size())
							{
								if (m_cursorLocation.X < (int)m_text.size() - 1 && m_text.substr(m_cursorLocation.X, 2) == L"\r\n")
									m_text = m_text.erase(m_cursorLocation.X, 2);
								else
									m_text = m_text.erase(m_cursorLocation.X, 1);

								changed = true;
							}
						}
						else
						{
							String* previousLine, *nextLine;
							String& currentLine = GetLines(previousLine, nextLine);

							if (m_cursorLocation.X < (int)currentLine.size())
							{
								currentLine = currentLine.erase(m_cursorLocation.X, 1);

								changed = true;
							}
							else if (m_cursorLocation.X == (int)currentLine.size())
							{
								if (nextLine)
								{
									currentLine += *nextLine;
									m_lines.RemoveAt(m_cursorLocation.Y + 1);

									changed = true;
								}
							}
						}
					}
					
					if (changed) Sync();
					break;
				}
				case KEY_HOME:
				{
					checkKeyboardSelection = true;

					m_cursorLocation.X = 0;
					if (m_multiline && e.ControlDown)
					{
						m_cursorLocation.Y = 0;
					}
					break;
				}
				case KEY_END:
				{
					checkKeyboardSelection = true;
					
					if (!m_multiline)
						m_cursorLocation.X = (int)m_text.size();
					else
					{
						String* previousLine, *nextLine;
						String& currentLine = GetLines(previousLine, nextLine);

						if (e.ControlDown)
						{
							m_cursorLocation.Y = m_lines.getCount() - 1;
							m_cursorLocation.X = (int)currentLine.size();
						}
						else
							m_cursorLocation.X = (int)currentLine.size();
					}
					break;
				}

				case KEY_NUMPADENTER:
				case KEY_RETURN:
				{
					if (hasSelection())
					{
						EraseSelectedText();
						changed = true;
					}

					if (m_multiline)
					{
						String* previousLine, *nextLine;
						String& currentLine = GetLines(previousLine, nextLine);

						String lineEnd;
						if ((int)currentLine.size() > m_cursorLocation.X)
						{
							lineEnd = currentLine.substr(m_cursorLocation.X, currentLine.size() - m_cursorLocation.X);
							currentLine = currentLine.substr(0, currentLine.size() - lineEnd.size());
						}

						m_lines.Insert(m_cursorLocation.Y + 1, lineEnd);
						m_cursorLocation.X = 0;
						m_cursorLocation.Y++;

						changed = true;
					}
					else
					{
						eventEnterPressed.Invoke();
					}

					if (changed) Sync();
					break;
				}
				case KEY_SPACE:
					Add(L" ");
					changed = true;
					break;
				case KEY_DECIMAL:
					Add(L".");
					changed = true;
					break;
				case KEY_DIVIDE:
					Add(L"/");
					changed = true;
					break;
				case KEY_BACKSLASH:
					Add(e.ShiftDown ? L"|" : L"\\");
					changed = true;
					break;
					// " ~
				case KEY_COMMA:
					Add(e.ShiftDown ? L"<" : L",");
					changed = true;
					break;
				case KEY_MINUS:
					Add(e.ShiftDown ? L"_" : L"-");
					changed = true;
					break;
				case KEY_LBRACKET:
					Add(e.ShiftDown ? L"{" : L"[");
					changed = true;
					break;
				case KEY_RBRACKET:
					Add(e.ShiftDown ? L"}" : L"]");
					changed = true;
					break;
				case KEY_PERIOD:
					Add(e.ShiftDown ? L">" : L".");
					changed = true;
					break;
				case KEY_ADD:
					Add(e.ShiftDown ? L"+" : L"=");
					changed = true;
					break;
				case KEY_SLASH:
					Add(e.ShiftDown ? L"?" : L"/");
					changed = true;
					break;
				case KEY_SEMICOLON:
					Add(e.ShiftDown ? L":" : L";");
					changed = true;
					break;
				case KEY_SUBTRACT:
					Add(L"-");
					changed = true;
					break;
				case KEY_MULTIPLY:
					Add(L"*");
					changed = true;
					break;
				case KEY_TAB:
					Add(L"      ");
					changed = true;
					break;
				case KEY_NUMPAD0:
				case KEY_0:
					Add(e.ShiftDown ? L")" : L"0");
					changed = true;
					break;
				case KEY_NUMPAD1:
				case KEY_1:
					Add(e.ShiftDown ? L"!" : L"1");
					changed = true;
					break;
				case KEY_NUMPAD2:
				case KEY_2:
					Add(e.ShiftDown ? L"@" : L"2");
					changed = true;
					break;
				case KEY_NUMPAD3:
				case KEY_3:
					Add(e.ShiftDown ? L"#" : L"3");
					changed = true;
					break;
				case KEY_NUMPAD4:
				case KEY_4:
					Add(e.ShiftDown ? L"$" : L"4");
					changed = true;
					break;
				case KEY_NUMPAD5:
				case KEY_5:
					Add(e.ShiftDown ? L"%" : L"5");
					changed = true;
					break;
				case KEY_NUMPAD6:
				case KEY_6:
					Add(e.ShiftDown ? L"^" : L"6");
					changed = true;
					break;
				case KEY_NUMPAD7:
				case KEY_7:
					Add(e.ShiftDown ? L"&" : L"7");
					changed = true;
					break;
				case KEY_NUMPAD8:
				case KEY_8:
					Add(e.ShiftDown ? L"*" : L"8");
					changed = true;
					break;
				case KEY_NUMPAD9:
				case KEY_9:
					Add(e.ShiftDown ? L"(" : L"9");
					changed = true;
					break;

				case KEY_A:
					Add(e.ShiftDown || e.CapsLock ? L"A" : L"a");
					changed = true;
					break;
				case KEY_B:
					Add(e.ShiftDown || e.CapsLock ? L"B" : L"b");
					changed = true;
					break;
				case KEY_C:
					Add(e.ShiftDown || e.CapsLock ? L"C" : L"c");
					changed = true;
					break;
				case KEY_D:
					Add(e.ShiftDown || e.CapsLock ? L"D" : L"d");
					changed = true;
					break;
				case KEY_E:
					Add(e.ShiftDown || e.CapsLock ? L"E" : L"e");
					changed = true;
					break;
				case KEY_F:
					Add(e.ShiftDown || e.CapsLock ? L"F" : L"f");
					changed = true;
					break;
				case KEY_G:
					Add(e.ShiftDown || e.CapsLock ? L"G" : L"g");
					changed = true;
					break;

				case KEY_H:
					Add(e.ShiftDown || e.CapsLock ? L"H" : L"h");
					changed = true;
					break;
				case KEY_I:
					Add(e.ShiftDown || e.CapsLock ? L"I" : L"i");
					changed = true;
					break;
				case KEY_J:
					Add(e.ShiftDown || e.CapsLock ? L"J" : L"j");
					changed = true;
					break;
				case KEY_K:
					Add(e.ShiftDown || e.CapsLock ? L"K" : L"k");
					changed = true;
					break;
				case KEY_L:
					Add(e.ShiftDown || e.CapsLock ? L"L" : L"l");
					changed = true;
					break;
				case KEY_M:
					Add(e.ShiftDown || e.CapsLock ? L"M" : L"m");
					changed = true;
					break;
				case KEY_N:
					Add(e.ShiftDown || e.CapsLock ? L"N" : L"n");
					changed = true;
					break;

				case KEY_O:
					Add(e.ShiftDown || e.CapsLock ? L"O" : L"o");
					changed = true;
					break;
				case KEY_P:
					Add(e.ShiftDown || e.CapsLock ? L"P" : L"p");
					changed = true;
					break;
				case KEY_Q:
					Add(e.ShiftDown || e.CapsLock ? L"Q" : L"q");
					changed = true;
					break;
				case KEY_R:
					Add(e.ShiftDown || e.CapsLock ? L"R" : L"r");
					changed = true;
					break;
				case KEY_S:
					Add(e.ShiftDown || e.CapsLock ? L"S" : L"s");
					changed = true;
					break;
				case KEY_T:
					Add(e.ShiftDown || e.CapsLock ? L"T" : L"t");
					changed = true;
					break;

				case KEY_U:
					Add(e.ShiftDown || e.CapsLock ? L"U" : L"u");
					changed = true;
					break;
				case KEY_V:
					Add(e.ShiftDown || e.CapsLock ? L"V" : L"v");
					changed = true;
					break;
				case KEY_W:
					Add(e.ShiftDown || e.CapsLock ? L"W" : L"w");
					changed = true;
					break;
				case KEY_X:
					Add(e.ShiftDown || e.CapsLock ? L"X" : L"x");
					changed = true;
					break;
				case KEY_Y:
					Add(e.ShiftDown || e.CapsLock ? L"Y" : L"y");
					changed = true;
					break;
				case KEY_Z:
					Add(e.ShiftDown || e.CapsLock ? L"Z" : L"z");
					changed = true;
					break;

				default:
					break;
			}

			ClampCursorPos(m_cursorLocation);
			ClampCursorPos(m_selectionStart);
			ClampCursorPos(m_selectionEnd);

			if (checkKeyboardSelection)
			{
				if (!isKeyboardSelecting())
				{
					if (hasSelection())
						ClearSelectionRegion();
				}
				else
				{
					SetSelectionEndFromCursor();
				}
			}
			

			if (changed)
			{
				eventContentChanged.Invoke();
			}
		}

		void TextEditState::MoveCursorToEnd()
		{
			if (!m_multiline)
			{
				m_cursorLocation.X = m_text.length();
			}
			else
			{
				m_cursorLocation.Y = m_lines.getCount() - 1;
				m_cursorLocation.X = (int)m_lines[m_lines.getCount() - 1].size();
			}
		}
		void TextEditState::MoveCursorTo(const Point& cp)
		{
			m_cursorLocation = cp;
			ClampCursorPos(m_cursorLocation);
		}

		void TextEditState::ClampCursorPos(Point& cp)
		{
			if (!m_multiline)
			{
				cp.Y = 0;

				if (cp.X > (int32)m_text.size())
					cp.X = (int32)m_text.size();
			}
			else
			{
				cp.Y = m_lines.ClampIndexInRange(cp.Y);

				const String& line = m_lines[cp.Y];
				if (cp.X > (int32)line.size())
					cp.X = (int32)line.size();
			}

			if (cp.X < 0)
				cp.X = 0;
		}

		void TextEditState::StartExternalSelection()
		{
			ClearSelectionRegion();
			m_isExternalSelecting = true;
		}
		void TextEditState::EndExternalSelection()
		{
			m_isExternalSelecting = false;
		}
		void TextEditState::SetSelectionEndFromCursor()
		{
			m_selectionEnd = m_cursorLocation;
		}
		void TextEditState::SetSelectionFromCursorWord()
		{
			const String& line = getCurrentLine();

			size_t spos = line.find_last_of(' ', m_cursorLocation.X);
			size_t epos = line.find_first_of(' ', m_cursorLocation.X + 1);

			m_selectionStart.Y = m_selectionEnd.Y = m_cursorLocation.Y;

			if (spos == String::npos)
				spos = 0;
			else
				spos++;

			if (epos == String::npos)
				epos = line.size();

			if (spos >= epos)
			{
				// in the middle of a blank
				spos = line.find_last_not_of(' ', m_cursorLocation.X);
				epos = line.find_first_not_of(' ', m_cursorLocation.X + 1);

				if (spos == String::npos)
					spos = 0;
				else
					spos++;

				if (epos == String::npos)
					epos = line.size();
			}

			m_selectionStart.X = spos;
			m_selectionEnd.X = epos;
		}

		void TextEditState::EraseSelectedText()
		{
			Point fixedStart, fixedEnd;
			GetFixedSelectionRegion(fixedStart, fixedEnd);
			ClearSelectionRegion();

			if (m_multiline)
			{
				if (fixedStart.Y == fixedEnd.Y)
				{
					String& line = m_lines[fixedStart.Y];
					line = line.erase(fixedStart.X, fixedEnd.X - fixedStart.X);
				}
				else
				{
					String& firstLine = m_lines[fixedStart.Y];
					String& lastLine = m_lines[fixedEnd.Y];

					if (fixedStart.X < (int32)firstLine.size())
						firstLine = firstLine.erase(fixedStart.X, firstLine.size() - fixedStart.X);

					lastLine = lastLine.erase(0, fixedEnd.X);

					firstLine.append(lastLine);

					if (fixedEnd.Y - fixedStart.Y > 0)
						m_lines.RemoveRange(fixedStart.Y + 1, fixedEnd.Y - fixedStart.Y);
				}
			}
			else
			{
				m_text = m_text.erase(fixedStart.X, fixedEnd.X - fixedStart.X);
			}

			m_cursorLocation = fixedStart;
		}
		void TextEditState::ClearSelectionRegion()
		{
			m_selectionStart = m_selectionEnd = m_cursorLocation;
		}
		
		void TextEditState::GetFixedSelectionRegion(Point& fixedStart, Point& fixedEnd)
		{
			if (m_selectionStart.Y == m_selectionEnd.Y)
			{
				fixedStart.Y = fixedEnd.Y = m_selectionStart.Y;

				fixedStart.X = Math::Min(m_selectionStart.X, m_selectionEnd.X);
				fixedEnd.X = Math::Max(m_selectionStart.X, m_selectionEnd.X);
			}
			else if (m_selectionStart.Y < m_selectionEnd.Y)
			{
				fixedStart = m_selectionStart;
				fixedEnd = m_selectionEnd;
			}
			else
			{
				fixedStart = m_selectionEnd;
				fixedEnd = m_selectionStart;
			}
		}

		bool TextEditState::GetLineSelectionRegion(int32 line, int32& start, int32& end)
		{
			if (!hasSelection())
				return false;

			Point fixedStart, fixedEnd;
			GetFixedSelectionRegion(fixedStart, fixedEnd);

			if (fixedStart.Y == fixedEnd.Y && line == fixedStart.Y)
			{
				start = Math::Min(fixedStart.X, fixedEnd.X);
				end = Math::Max(fixedStart.X, fixedEnd.X);
			}
			else if (line > fixedStart.Y && line < fixedEnd.Y)
			{
				start = 0;
				end = m_lines[line].size();
			}
			else if (line == fixedStart.Y)
			{
				start = fixedStart.X;
				end = m_lines[line].size();
			}
			else if (line == fixedEnd.Y)
			{
				start = 0;
				end = fixedEnd.X;
			}
			else
			{
				return false;
			}
			return true;
		}

		String& TextEditState::GetLines(String*& previousLine, String*& nextLine)
		{
			previousLine = m_cursorLocation.Y > 0 ? &m_lines[m_cursorLocation.Y - 1] : nullptr;
			nextLine = m_cursorLocation.Y < m_lines.getCount() - 1 ? &m_lines[m_cursorLocation.Y + 1] : nullptr;

			return m_lines[m_cursorLocation.Y];
		}

		void TextEditState::Sync()
		{
			if (m_multiline)
			{
				m_text.clear();
				for (const String& l : m_lines)
					m_text.append(l);
			}
			else
			{
				m_lines.Clear();
				m_lines.Add(m_text);
			}
		}
	}
}