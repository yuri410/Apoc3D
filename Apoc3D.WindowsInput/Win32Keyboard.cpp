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