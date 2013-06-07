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

#if APOC3D_PLATFORM == APOC3D_PLATFORM_WINDOWS
#include <windows.h>
#endif

namespace Apoc3D
{
	namespace UI
	{
		static const KeyboardKeyCode InputKeys[] = 
		{
			KEY_0,
			KEY_1,
			KEY_2,
			KEY_3,
			KEY_4,
			KEY_5,
			KEY_6,
			KEY_7,
			KEY_8,
			KEY_9,

			KEY_NUMPAD0,
			KEY_NUMPAD1,
			KEY_NUMPAD2,
			KEY_NUMPAD3,
			KEY_NUMPAD4,
			KEY_NUMPAD5,
			KEY_NUMPAD6,
			KEY_NUMPAD7,
			KEY_NUMPAD8,
			KEY_NUMPAD9,


			KEY_A,
			KEY_B,
			KEY_C,
			KEY_D,
			KEY_E,
			KEY_F,
			KEY_G,
			KEY_H,
			KEY_I,
			KEY_J,
			KEY_K,
			KEY_L,
			KEY_M,
			KEY_N,
			KEY_O,
			KEY_P,
			KEY_Q,
			KEY_R,
			KEY_S,
			KEY_T,
			KEY_U,
			KEY_V,
			KEY_W,
			KEY_X,
			KEY_Y,
			KEY_Z,
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


			KEY_TAB,
			

		};

		void KeyboardHelper::Update(const GameTime* const time)
		{
			bool& pasting = m_pasting;
			Keyboard* kb = InputAPIManager::getSingleton().getKeyboard();
			if (kb)
			{
				if (kb->IsPressing(KEY_LCONTROL) || kb->IsPressing(KEY_RCONTROL))
				{
					if (kb->IsKeyDown(KEY_V))
					{
						pasting = true;
						if (m_ePaste.getCount() > 0)
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

													m_ePaste.Invoke(strText);

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
							
							//m_ePaste();
#endif
						}
						return;
					}
					else if (kb->IsKeyUp(KEY_V))
					{
						pasting = false;
					}
				}
				else
				{
					pasting = false;
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
				eventArg.ShiftDown |= kb->IsPressing(KEY_LSHIFT);
				eventArg.ShiftDown |= kb->IsPressing(KEY_RSHIFT);

				for (int i=0;i<sizeof(InputKeys)/sizeof(KeyboardKeyCode);i++)
				{
					if (kb->IsKeyDown(InputKeys[i]))
					{
						if (pasting && InputKeys[i] == KEY_V)
						{
							return;
						}
						
						m_eKeyPress.Invoke(InputKeys[i], eventArg);
						
						m_previousKey = m_currentKey;

						m_currentKey = InputKeys[i];
						if (m_currentKey != m_previousKey)
						{
							m_pressingTime = 0;
							m_timerStarted = true;
						}

					}
					else if (kb->IsKeyUp(InputKeys[i]))
					{
						if (m_currentKey == InputKeys[i])
							m_currentKey = KEY_UNASSIGNED;

						m_timerStarted = false;
						m_pressingTime = 0;

						m_eKeyRelease.Invoke(InputKeys[i], eventArg);
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
							
							m_eKeyPress.Invoke(m_currentKey, eventArg);
						}
					}
					
				}


			}
		}
	}
}