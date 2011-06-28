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
#ifndef KEYBOARDHELPER_H
#define KEYBOARDHELPER_H

#include "UICommon.h"
#include "Input/Keyboard.h"

using namespace Apoc3D::Core;
using namespace Apoc3D::Input;

namespace Apoc3D
{
	namespace UI
	{
		struct KeyboardEventsArgs
		{
			bool ShiftDown;
			bool ControlDown;
			bool AltDown;
			bool CapsLock;

			KeyboardEventsArgs()
				: ShiftDown(false), ControlDown(false), AltDown(false), CapsLock(false)
			{

			}
		};
		typedef fastdelegate::FastDelegate1<String, void> PasteEventHandler;
		typedef fastdelegate::FastDelegate2<KeyboardKeyCode, KeyboardEventsArgs, void> KeyboardEventHandler;

		

		class KeyboardHelper
		{
		private:
			KeyboardEventHandler m_eKeyPress;
			KeyboardEventHandler m_eKeyRelease;
			PasteEventHandler m_ePaste;

			float m_pressingTime;
			KeyboardKeyCode m_currentKey;
			KeyboardKeyCode m_previousKey;
			bool m_pasting;



		public:
			KeyboardHelper()
				: m_pasting(false), m_pressingTime(0), m_currentKey(KEY_UNASSIGNED), m_previousKey(KEY_UNASSIGNED)
			{

			}
			void Update(const GameTime* const time);
		};
	}
}

#endif