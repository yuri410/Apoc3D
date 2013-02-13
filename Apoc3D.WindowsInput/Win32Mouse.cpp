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
#include "Win32Mouse.h"
#include "apoc3d/Core/GameTime.h"

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

				m_oldWndProc = GetWindowLongPtr(hwnd, GWLP_WNDPROC);

				SetWindowLongPtr(hwnd, GWLP_WNDPROC, (LONG)(void*)WndProcStatic);
			}
			OldSchoolMouse::~OldSchoolMouse()
			{
				m_instance = 0;
			}

			void OldSchoolMouse::Update(const GameTime* const time)
			{
				if (m_hwnd)
				{
					POINT pt;
					GetCursorPos(&pt);
					
					ScreenToClient(m_hwnd, &pt);

					m_lastPosition = m_currentPos;
					m_lastZ = m_z;
					m_z = m_accumlatedMouseWheel;
					
					memcpy(m_lastBtnState, m_btnState, sizeof(m_btnState));

					m_currentPos.X = static_cast<int32>(pt.x);
					m_currentPos.Y = static_cast<int32>(pt.y);

					m_btnState[0] = !!GetAsyncKeyState(VK_LBUTTON);
					m_btnState[1] = !!GetAsyncKeyState(VK_MBUTTON);
					m_btnState[2] = !!GetAsyncKeyState(VK_RBUTTON);
				}

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

			void Win32Mouse::Update(const GameTime* const time)
			{
				memcpy(m_lastBtnState, m_btnState, sizeof(m_btnState));
				m_lastPosition = m_currentPos;
				m_lastZ = m_z;
				
				m_mouse->capture();
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