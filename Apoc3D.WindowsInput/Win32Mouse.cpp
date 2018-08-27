/* -----------------------------------------------------------------------
 * This source file is part of Apoc3D Framework
 * 
 * Copyright (c) 2009-2018 Tao Xin
 * 
 * This content of this file is subject to the terms of the Mozilla Public 
 * License v2.0. If a copy of the MPL was not distributed with this file, 
 * you can obtain one at http://mozilla.org/MPL/2.0/.
 * 
 * This program is distributed in the hope that it will be useful, 
 * WITHOUT WARRANTY OF ANY KIND; either express or implied. See the 
 * Mozilla Public License for more details.
 * 
 * ------------------------------------------------------------------------
 */

#include "Win32Mouse.h"
#include "apoc3d/Core/GameTime.h"
#include "apoc3d.Win32/Win32Common.h"

namespace Apoc3D
{
	namespace Input
	{
		namespace Win32
		{
			OldSchoolMouse* OldSchoolMouse::m_instance = 0;

			OldSchoolMouse::OldSchoolMouse(HWND hwnd)
				: m_hwnd(hwnd), m_accumlatedMouseWheel(0)
			{
				m_instance = this;

				if (Apoc3D::Win32::MainWindowHandle == 0)
				{
					m_oldWndProc = (HWND)GetWindowLongPtr(hwnd, GWLP_WNDPROC);

					SetWindowLongPtr(hwnd, GWLP_WNDPROC, (LONG_PTR)(void*)WndProcStatic);
				}
				else
				{
					m_usesMainWindowMouseWheel = true;
				}
			}
			OldSchoolMouse::~OldSchoolMouse()
			{
				m_instance = 0;
			}

			void OldSchoolMouse::Update(const GameTime* time)
			{
				POINT pt = { 0,0 };
				GetCursorPos(&pt);
				if (m_hwnd)
				{
					ScreenToClient(m_hwnd, &pt);
				}

				m_lastPosition = m_currentPos;
				m_lastZ = m_z;

				if (!m_usesMainWindowMouseWheel)
				{
					m_z = m_accumlatedMouseWheel;
				}
				else
				{
					m_z = Apoc3D::Win32::MainWindowMouseWheel;
				}

				memcpy(m_lastBtnState, m_btnState, sizeof(m_btnState));

				m_currentPos.X = static_cast<int32>(pt.x);
				m_currentPos.Y = static_cast<int32>(pt.y);

				m_btnState[0] = !!GetAsyncKeyState(VK_LBUTTON);
				m_btnState[1] = !!GetAsyncKeyState(VK_MBUTTON);
				m_btnState[2] = !!GetAsyncKeyState(VK_RBUTTON);

				if (m_buttonSwapped)
				{
					std::swap(m_btnState[0], m_btnState[2]);
				}
			}
			void OldSchoolMouse::SetPosition(const Point& loc)
			{
				Mouse::SetPosition(loc);

				POINT pt;
				pt.x = loc.X;
				pt.y = loc.Y;
				
				if (m_hwnd)
				{
					ClientToScreen(m_hwnd, &pt);
				}

				SetCursorPos(pt.x, pt.y);
			}

			LRESULT OldSchoolMouse::WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
			{
				switch (message)
				{
				case WM_MOUSEWHEEL:
					m_accumlatedMouseWheel += (int16) (wParam >> 0x10);
				}
				return CallWindowProc((WNDPROC)m_oldWndProc, hWnd, message, wParam, lParam);
			}

			//////////////////////////////////////////////////////////////////////////


			Win32Mouse::Win32Mouse(OIS::InputManager* manager)
				: m_inpMgr(manager)
			{
				m_mouse = static_cast<OIS::Mouse*>(manager->createInputObject(OIS::OISMouse, true));
				m_mouse->setEventCallback(this);
			}
			Win32Mouse::~Win32Mouse()
			{
				m_inpMgr->destroyInputObject(m_mouse);
			}

			void Win32Mouse::Update(const GameTime* time)
			{
				memcpy(m_lastBtnState, m_btnState, sizeof(m_btnState));
				m_lastPosition = m_currentPos;
				m_lastZ = m_z;
				
				m_mouse->capture();
			}
			void Win32Mouse::SetPosition(const Point& loc)
			{
				Mouse::SetPosition(loc);

				OIS::MouseState& mutableMouseState = const_cast<OIS::MouseState &>(m_mouse->getMouseState());
				mutableMouseState.X.abs = loc.X;
				mutableMouseState.Y.abs = loc.Y;
			}
			bool Win32Mouse::mouseMoved( const OIS::MouseEvent &arg )
			{
				const OIS::MouseState& s = arg.state;
				m_currentPos.X = s.X.abs;
				m_currentPos.Y = s.Y.abs;
				m_z = s.Z.abs;

				return true;
			}
			bool Win32Mouse::mousePressed( const OIS::MouseEvent &arg, OIS::MouseButtonID id ) 
			{
				//const OIS::MouseState& s = arg.state;
				m_btnState[(int)id] = true;

				//std::cout << "\nMouse button #" << id << " pressed. Abs("
					//<< s.X.abs << ", " << s.Y.abs << ", " << s.Z.abs << ") Rel("
					//<< s.X.rel << ", " << s.Y.rel << ", " << s.Z.rel << ")";
				return true;
			}
			bool Win32Mouse::mouseReleased( const OIS::MouseEvent &arg, OIS::MouseButtonID id )
			{
				//const OIS::MouseState& s = arg.state;

				m_btnState[(int)id] = false;

				//std::cout << "\nMouse button #" << id << " released. Abs("
					//<< s.X.abs << ", " << s.Y.abs << ", " << s.Z.abs << ") Rel("
					//<< s.X.rel << ", " << s.Y.rel << ", " << s.Z.rel << ")";
				return true;
			}
		}
	}
}