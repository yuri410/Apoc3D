#pragma once
#ifndef APOC3D_MENU_H
#define APOC3D_MENU_H

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

#include "Control.h"
#include "KeyboardHelper.h"
#include "apoc3d/Math/ColorValue.h"

namespace Apoc3D
{
	namespace UI
	{
		enum MenuState
		{
			MENU_Open,
			MENU_Closed
		};

		struct MenuItemSetupInfo
		{
			String Text;
			List<MenuItemSetupInfo> SubMenuItemsInfo;
			MenuItemEventHandler EventHandler;

			bool ForceCreateSubMenu = false;

			MenuItem** ResultItem = nullptr;
			SubMenu** ResultSubMenu = nullptr;

			
			MenuItemSetupInfo() { }

			MenuItemSetupInfo(const String& text, 
				MenuItem** resultItem = nullptr, SubMenu** resultSubMenu = nullptr, bool forceSubMenu = false)
				: Text(text), 
				ForceCreateSubMenu(forceSubMenu), ResultItem(resultItem), ResultSubMenu(resultSubMenu) 
			{ }

			MenuItemSetupInfo(const String& text, std::initializer_list<MenuItemSetupInfo> subItems, 
				MenuItem** resultItem = nullptr, SubMenu** resultSubMenu = nullptr, bool forceSubMenu = false)
				: Text(text), SubMenuItemsInfo(subItems),
				ForceCreateSubMenu(forceSubMenu), ResultItem(resultItem), ResultSubMenu(resultSubMenu)
			{ }

			MenuItemSetupInfo(const String& text, fastdelegate::FastDelegate<void(MenuItem*)> handler, 
				MenuItem** resultItem = nullptr, SubMenu** resultSubMenu = nullptr, bool forceSubMenu = false)
				: Text(text), 
				ForceCreateSubMenu(forceSubMenu), ResultItem(resultItem), ResultSubMenu(resultSubMenu)
			{
				EventHandler.Bind(handler);
			}

			MenuItemSetupInfo(const String& text, fastdelegate::FastDelegate<void(MenuItem*)> handler, std::initializer_list<MenuItemSetupInfo> subItems, 
				MenuItem** resultItem = nullptr, SubMenu** resultSubMenu = nullptr, bool forceSubMenu = false)
				: Text(text), SubMenuItemsInfo(subItems), 
				ForceCreateSubMenu(forceSubMenu), ResultItem(resultItem), ResultSubMenu(resultSubMenu)
			{
				EventHandler.Bind(handler);
			}
		};

		class APAPI MenuBar : public Control
		{
			RTTI_DERIVED(MenuBar, Control);
		public:
			MenuBar(const StyleSkin* skin);
			virtual ~MenuBar();

			void Add(MenuItem* item, SubMenu* submenu);
			void Add(const MenuItemSetupInfo& info, const StyleSkin* skin, ControlContainer* owner);

			virtual void Update(const AppTime* time) override;
			virtual void Draw(Sprite* sprite) override;

			void Close();

			bool HasItemOpen() const;

			MenuItem* operator [](int index) const;

			const List<MenuItem*>& getItems() const { return m_items; }
			MenuState getState() const { return m_state; }

			ControlContainer* Owner = nullptr;
		private:
			void Initialize(const StyleSkin* skin);

			void CheckSelection();
			void CheckHovering();
			void Keyboard_OnPress(KeyboardKeyCode key, KeyboardEventsArgs e);
			void Keyboard_OnRelease(KeyboardKeyCode key, KeyboardEventsArgs e);
			void CloseSubMenus();

			const StyleSkin* m_skin = nullptr;

			Point m_drawPos;

			Point m_itemPos;
			List<MenuItem*> m_items;
			MenuState m_state = MENU_Closed;

			int m_hoverIndex = -1;
			Point m_openPos;
			Apoc3D::Math::Rectangle m_itemArea;


			KeyboardHelper m_helper;
			bool m_altDown = false;
			bool m_openedMenu = false;
			int m_indexToOpen = -1;

		};

		class APAPI MenuItem
		{
		public:
			explicit MenuItem(const String& text);

			const String& getText() const { return m_text; }
			void setText(const String& txt);
			const String& getCleanText() const { return m_cleanText; }

			KeyboardKeyCode getKeyCode() const { return m_key; }
			int getKeyIndex() const { return m_keyIndex; }

			SubMenu* getSubMenu() const { return m_submenu; }
			void setSubMenu(SubMenu* sm) { m_submenu = sm; }

			void* UserPointer = nullptr;
			bool Enabled = true;

			MenuItemEventHandler event;

		private:
			String m_text;
			String m_cleanText;

			int m_keyIndex = -1;
			KeyboardKeyCode m_key = KEY_UNASSIGNED;

			SubMenu* m_submenu = nullptr;
		};


		class APAPI SubMenu : public Control
		{
			RTTI_DERIVED(SubMenu, Control);
		public:
			SubMenu(const StyleSkin* skin, ControlContainer* owner);
			SubMenu(const StyleSkin* skin, ControlContainer* owner, const List<MenuItemSetupInfo>& itemsInfo);

			virtual ~SubMenu();

			void Add(MenuItem* item, SubMenu* submenu);

			void Clear();

			virtual void Update(const AppTime* time) override;
			virtual void Draw(Sprite* sprite) override;

			void Open(const Point& position);
			void Close();

			bool IsCursorInside();

			int getHoverIndex() const { return m_hoverIndex; }
			const List<MenuItem*>& getItems() const { return m_items; }

			MenuState getState() const { return m_state; }
			Control* getParentMenu() const { return m_parentMenu; }
			void setParentMenu(Control* ctrl) { m_parentMenu = ctrl; }

		private:
			void Keyboard_OnPress(KeyboardKeyCode key, KeyboardEventsArgs e);
			void Keyboard_OnRelease(KeyboardKeyCode key, KeyboardEventsArgs e);

			void CalcualteSize();
			void CloseSubMenus();
			void CheckSelection();

			Control* m_parentMenu = nullptr;
			const StyleSkin* m_skin = nullptr;

			List<MenuItem*> m_items;
			Point m_itemPos;
			Point m_arrowPos;
			Point m_textPos;
			Point m_openPos;

			Apoc3D::Math::Rectangle m_itemArea;
			Apoc3D::Math::Rectangle m_borderArea;
			int m_hoverIndex = -1;
			ColorValue m_shadowColor = CV_PackColor(0, 0, 0, 127);

			MenuState m_state = MENU_Closed;

			KeyboardHelper m_helper;
			int m_indexToOpen = -1;

			float m_timerCount = 0.5f;
			bool m_timerStarted = false;

			
		};
	}
}

#endif