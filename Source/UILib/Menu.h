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

#ifndef MENU_H
#define MENU_H

#include "UICommon.h"
#include "Control.h"
#include "Collections/FastList.h"

using namespace Apoc3D::Collections;

namespace Apoc3D
{
	namespace UI
	{
		enum MenuState
		{
			MENU_Open,
			MENU_Closed
		};
		class APAPI Menu : public Control
		{
		private:
			FastList<MenuItem*> m_items;
			MenuState m_state;

			void CheckSelection();
			void CheckHovering();

		public:
			const FastList<MenuItem*>& getItems() const { return m_items; }
			MenuState getState() const { return m_state; }

			Menu();
			~Menu();

			MenuItem* operator [](const String& name) const;
			MenuItem* operator [](int index) const;

			void Add(MenuItem* item, SubMenu* submenu);
			virtual void Initialize(RenderDevice* device);
			virtual void Update(const GameTime* const time);

			virtual void Draw(Sprite* sprite);

			void Close();

		};
	}
}

#endif