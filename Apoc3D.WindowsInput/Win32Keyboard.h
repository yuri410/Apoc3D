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

#ifndef WIN32KEYBOARD_H
#define WIN32KEYBOARD_H

#include "WinInputCommon.h"
#include "apoc3d/Input/Keyboard.h"

using namespace Apoc3D::Core;

namespace Apoc3D
{
	namespace Input
	{
		namespace Win32
		{
			class Win32Keyboard : public Keyboard, public OIS::KeyListener
			{
			public:
				Win32Keyboard(OIS::InputManager* mgr);
				~Win32Keyboard();

				void Update(const GameTime* time) override;


				bool keyPressed( const OIS::KeyEvent &arg );
				bool keyReleased( const OIS::KeyEvent &arg );

			private:
				OIS::InputManager* m_inpMgr;
				OIS::Keyboard* m_keyboard;

			};
		}
	}
}

#endif