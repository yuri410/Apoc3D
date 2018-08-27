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

#include "Win32Keyboard.h"
#include "apoc3d/Core/GameTime.h"
#include "apoc3d/Core/Logging.h"
#include "apoc3d/Utility/StringUtils.h"

using namespace Apoc3D::Utility;

namespace Apoc3D
{
	namespace Input
	{
		namespace Win32
		{
			Win32Keyboard::Win32Keyboard(OIS::InputManager* mgr)
				: m_inpMgr(mgr)
			{
				m_keyboard = static_cast<OIS::Keyboard*>(mgr->createInputObject(OIS::OISKeyboard, true));
				m_keyboard->setEventCallback(this);
			}
			Win32Keyboard::~Win32Keyboard()
			{
				m_inpMgr->destroyInputObject(m_keyboard);
			}

			void Win32Keyboard::Update(const GameTime* time)
			{
				m_lastKeyState = m_keyState;

				m_keyboard->capture();
			}

			bool Win32Keyboard::keyPressed( const OIS::KeyEvent &arg )
			{
				m_keyState[arg.key] = true;
				
				return true;
			}
			bool Win32Keyboard::keyReleased( const OIS::KeyEvent &arg )
			{
				m_keyState[arg.key] = false;

				return true;
			}
		}
	}
}