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

#ifndef WIN32MOUSE_H
#define WIN32MOUSE_H

#include "WinInputCommon.h"
#include "apoc3d/Input/Mouse.h"

using namespace Apoc3D::Core;

namespace Apoc3D
{
	namespace Input
	{
		namespace Win32
		{
			class OldSchoolMouse : public Mouse
			{
			public:
				OldSchoolMouse(HWND hwnd);
				~OldSchoolMouse();

				void SetPosition(const Point& loc) override;

				void Update(const GameTime* time) override;
			private:
				int32 m_accumlatedMouseWheel;
				bool m_usesMainWindowMouseWheel = false;

				LRESULT WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);
				HWND m_oldWndProc;
				HWND m_hwnd;

				static LRESULT CALLBACK WndProcStatic(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
				{
					if (m_instance)
					{
						return m_instance->WndProc(hWnd, message, wParam, lParam);
					}
					return 0;
				}

				static OldSchoolMouse* m_instance;
			};

			class Win32Mouse : public Mouse, public OIS::MouseListener
			{
			public:
				Win32Mouse(OIS::InputManager* manager);
				~Win32Mouse();

				virtual void SetPosition(const Point& loc) override;

				void Update(const GameTime* time);

				bool mouseMoved( const OIS::MouseEvent &arg );
				bool mousePressed( const OIS::MouseEvent &arg, OIS::MouseButtonID id );
				bool mouseReleased( const OIS::MouseEvent &arg, OIS::MouseButtonID id );

			private:
				OIS::InputManager* m_inpMgr;
				OIS::Mouse* m_mouse;
			};
		}
	}
}

#endif