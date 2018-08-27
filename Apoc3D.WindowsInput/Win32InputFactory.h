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

#ifndef WIN32INPUTFACTORY_H
#define WIN32INPUTFACTORY_H

#include "WinInputCommon.h"
#include "apoc3d/Input/InputAPI.h"
#include "apoc3d/Math/Point.h"

using namespace Apoc3D::Math;
using namespace Apoc3D::Graphics::RenderSystem;

namespace Apoc3D
{
	namespace Input
	{
		namespace Win32
		{
			class Win32InputFactory : public InputAPIFactory
			{
			public:
				Win32InputFactory()
					: InputAPIFactory(GetDescription())
				{
					m_instance = this;
				}
				~Win32InputFactory()
				{
					m_instance = 0;
				}

				virtual void Initialize(RenderWindow* window);
				virtual Mouse* CreateMouse();
				virtual Keyboard* CreateKeyboard();

			private:
				OIS::InputManager* m_InputManager = nullptr;
				HWND m_hwnd = 0;

				String m_tempTitleParam;
				Size m_tempClientSizeParam;

				bool m_relaxedWindowFinding = false;
				int32 m_largestWindowSize = 0;

				BOOL EnumWindowsProc(_In_  HWND hwnd, _In_  LPARAM lParam);


				static Win32InputFactory* m_instance;
				static BOOL CALLBACK EnumWindowsProcStatic(_In_  HWND hwnd, _In_  LPARAM lParam)
				{
					if (m_instance)
					{
						m_instance->EnumWindowsProc(hwnd, lParam);
					}
					return FALSE;
				}

				static APIDescription GetDescription();

			};
		}
	}
}

#endif