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

#ifndef APOC3D_MENU_H
#define APOC3D_MENU_H

#include "UICommon.h"
#include "Control.h"
#include "Collections/FastList.h"
#include "KeyboardHelper.h"

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
		public:
			const FastList<MenuItem*>& getItems() const { return m_items; }
			MenuState getState() const { return m_state; }

			Menu();
			~Menu();

			MenuItem* operator [](int index) const;

			void Add(MenuItem* item, SubMenu* submenu);
			virtual void Initialize(RenderDevice* device);
			virtual void Update(const GameTime* const time);

			virtual void Draw(Sprite* sprite);

			void Close();

		private:
			Point m_drawPos;

			Point m_itemPos;
			FastList<MenuItem*> m_items;
			MenuState m_state;

			int m_hoverIndex;
			Point m_openPos;
			Apoc3D::Math::Rectangle m_itemArea;


			KeyboardHelper m_helper;
			bool m_altDown;
			bool m_openedMenu;
			int m_indexToOpen;

			void CheckSelection();
			void CheckHovering();
			void Keyboard_OnPress(KeyboardKeyCode key, KeyboardEventsArgs e);
			void Keyboard_OnRelease(KeyboardKeyCode key, KeyboardEventsArgs e);
			void CloseSubMenus();
		};

		class APAPI MenuItem
		{
		public:
			void* UserPointer;
			bool Enabled;

			const String& getText() const { return m_text; }
			void setText(const String& txt);
			const String& getCleanText() const { return m_cleanText; }

			KeyboardKeyCode getKeyCode() const { return m_key; }
			int getKeyIndex() const { return m_keyIndex; }

			UIEventHandler& event() { return m_event; }

			SubMenu* getSubMenu() const { return m_submenu; }
			void setSubMenu(SubMenu* sm) { m_submenu = sm; }

			MenuItem(const String& text)
				: m_submenu(0), UserPointer(0), m_key(KEY_UNASSIGNED), m_keyIndex(-1),
				Enabled(true)
			{
				setText(text);
			}

		private:
			String m_text;
			String m_cleanText;

			int m_keyIndex;
			KeyboardKeyCode m_key;

			UIEventHandler m_event;
			SubMenu* m_submenu;
		};

		class APAPI SubMenu : public Control
		{
		public:
			MenuState getState() const { return m_state; }
			Control* getParent() const { return m_parent; }
			void setParent(Control* ctrl) { m_parent = ctrl; }

			SubMenu(ControlContainer* owner);
			~SubMenu();

			void Add(MenuItem* item, SubMenu* submenu);

			virtual void Initialize(RenderDevice* device);
			virtual void Update(const GameTime* const time);
			virtual void Draw(Sprite* sprite);

			void Open(const Point& position);
			void Close();

			bool IsCursorInside();

			int getHoverIndex() const { return m_hoverIndex; }
			const FastList<MenuItem*>& getItems() const { return m_items; }
		private:
			void Keyboard_OnPress(KeyboardKeyCode key, KeyboardEventsArgs e);
			void Keyboard_OnRelease(KeyboardKeyCode key, KeyboardEventsArgs e);

			void CalcualteSize();
			void CloseSubMenus();
			void CheckSelection();

			Control* m_parent;
			FastList<MenuItem*> m_items;
			Point m_itemPos;
			Point m_arrowPos;
			Point m_textPos;
			Apoc3D::Math::Rectangle m_itemArea;
			Apoc3D::Math::Rectangle m_borderArea;
			int m_hoverIndex;
			Point m_openPos;
			ColorValue m_shadowColor;

			MenuState m_state;

			KeyboardHelper m_helper;
			int m_indexToOpen;

			float m_timerCount;
			bool m_timerStarted;

		};
	}
}

#endif