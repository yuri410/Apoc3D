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
#ifndef WIN32MOUSE_H
#define WIN32MOUSE_H

#include "WinInputCommon.h"
#include "Input/Mouse.h"

using namespace Apoc3D::Core;

namespace Apoc3D
{
	namespace Input
	{
		namespace Win32
		{
			class Win32Mouse : public Mouse, public OIS::MouseListener
			{
			private:
				OIS::InputManager* m_inpMgr;
				OIS::Mouse* m_mouse;

			public:
				Win32Mouse(OIS::InputManager* mgr);
				~Win32Mouse();

				void Update(const GameTime* const time);

				bool mouseMoved( const OIS::MouseEvent &arg );
				bool mousePressed( const OIS::MouseEvent &arg, OIS::MouseButtonID id );
				bool mouseReleased( const OIS::MouseEvent &arg, OIS::MouseButtonID id );
			};
		}
	}
}

#endif