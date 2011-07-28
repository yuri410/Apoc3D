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
#include "Menu.h"
#include "Utility/StringUtils.h"
#include "Input/InputAPI.h"
#include "Input/Mouse.h"

using namespace Apoc3D::Utility;
using namespace Apoc3D::Input;

namespace Apoc3D
{
	namespace UI
	{
		Menu::Menu()
			: m_drawPos(0,0), m_itemPos(0,0), m_hoverIndex(-1), m_openPos(0,0), m_itemArea(0,0,0,0),
			m_state(MENU_Closed), m_altDown(false), m_openedMenu(false), m_indexToOpen(-1)
		{
			m_helper.eventKeyPress().bind(this, &Menu::Keyboard_OnPress);
			m_helper.eventKeyRelease().bind(this, &Menu::Keyboard_OnRelease);

		}

		void Menu::Add(MenuItem* item, SubMenu* submenu)
		{
			if (submenu)
			{
				submenu->setParent( this);
			}
			if (m_owner)
				submenu->setOwner(m_owner);

			item->setSubMenu(submenu);

			m_items.Add(item);
		}
		void Menu::CheckSelection()
		{
			Mouse* mouse = InputAPIManager::getSingleton.getMouse();
			if (mouse->IsLeftPressed())
			{
				if (m_hoverIndex != -1)
				{
					CloseSubMenus();

					if (m_items[m_hoverIndex]->getSubMenu() && 
						m_items[m_hoverIndex]->getSubMenu()->getState()!=MENU_Closed)
					{
						m_items[m_hoverIndex]->getSubMenu()->Close();
						m_state = MENU_Closed;
						return;
					}

					if (m_items[m_hoverIndex]->getSubMenu() &&
						m_items[m_hoverIndex]->getSubMenu()->getState()!=MENU_Closed)
					{
						m_items[m_hoverIndex]->getSubMenu()->Open(m_openPos);
						m_state = MENU_Open;
					}
				}
				else
				{
					for (int i=0;i<m_items.getCount();i++)
					{
						if (m_items[i]->getSubMenu() && 
							m_items[i]->getSubMenu()->getState()==MENU_Open &&
							m_items[i]->getSubMenu()->IsCursorInside())
						{
							CloseSubMenus();
						}
					}
				}
			}
		}
		void Menu::CheckHovering()
		{
			if (m_items[m_hoverIndex]->getSubMenu() &&
				m_items[m_hoverIndex]->getSubMenu()->getState() == MENU_Closed)
			{
				CloseSubMenus();
				m_items[m_hoverIndex]->getSubMenu()->Open(m_openPos);
				m_state = MENU_Open;
			}
		}
		void Menu::Close()
		{
			CloseSubMenus();
			m_state = MENU_Closed;
		}
		void Menu::CloseSubMenus()
		{
			for (int i=0;i<m_items.getCount();i++)
			{
				if (m_items[i]->getSubMenu() &&
					m_items[i]->getSubMenu()->getState() == MENU_Open)
				{
					m_items[i]->getSubMenu()->Close();
				}
			}
			m_state = MENU_Closed;
		}
		void Menu::Update(const GameTime* const time)
		{
			CheckSelection();
			m_helper.Update(time);

			if (m_state == MENU_Open && m_hoverIndex != -1)
				CheckHovering();

			for (int i=0;i<m_items.getCount();i++)
				if (m_items[i]->getSubMenu() && m_items[i]->getSubMenu()->getState() != MENU_Closed)
				{
					m_items[i]->getSubMenu()->Update(time);
				}

		}
		void Menu::Keyboard_OnPress(KeyboardKeyCode key, KeyboardEventsArgs e)
		{
			switch (key)
			{
			case KEY_LMENU: //  alt
			case KEY_RMENU:
				m_altDown = true;
				break;
			default:
				m_indexToOpen = -1;
				for (int i=0;i<m_items.getCount();i++)
				{
					if (m_altDown && key == m_items[i]->getKeyCode())
					{
						m_indexToOpen = i;
						break;
					}
				}
				break;
			}
		}
		void Menu::Keyboard_OnRelease(KeyboardKeyCode key, KeyboardEventsArgs e)
		{
			switch (key)
			{
			case KEY_LMENU: //  alt
				m_altDown = false;
				if (!m_openedMenu)
					CloseSubMenus();
				m_openedMenu = false;
				break;
			case KEY_RMENU:
				if (!m_openedMenu)
					CloseSubMenus();
				m_altDown = false;
				m_openedMenu = false;
				break;
			}
		}

		void Menu::Draw(Sprite* sprite)
		{

		}

		KeyboardKeyCode GetKey(wchar_t ch)
		{
			ch = tolower(ch);
			switch (ch)
			{
			case 'a':
				return KEY_A;
			case 'b': return KEY_B;
			case 'c': return KEY_C;
			case 'd': return KEY_D;
			case 'e': return KEY_E;
			case 'f': return KEY_F;
			case 'g': return KEY_G;
			case 'h': return KEY_H;
			case 'i': return KEY_I;
			case 'j': return KEY_J;
			case 'k': return KEY_K;
			case 'l': return KEY_L;
			case 'm': return KEY_M;
			case 'n': return KEY_N;
			case 'o': return KEY_O;
			case 'p': return KEY_P;
			case 'q': return KEY_Q;
			case 'r': return KEY_R;
			case 's': return KEY_S;
			case 't': return KEY_T;
			case 'u': return KEY_U;
			case 'v': return KEY_V;
			case 'w': return KEY_W;
			case 'x': return KEY_X;
			case 'y': return KEY_Y;
			case 'z': return KEY_Z;
			}
			return KEY_UNASSIGNED;
		}

		void MenuItem::setText(const String& txt)
		{
			m_text = txt;


			if (m_text.find('&') != String::npos)
			{
				std::vector<String> split = StringUtils::Split(m_text, L"&");

				m_keyIndex = (int)split[0].length();
				m_key = GetKey(split[1][0]);
				m_cleanText.reserve(m_text.size());
				for (size_t i=0;i<split.size();i++)
				{
					m_cleanText+=split[i];
				}
			}
			else
			{
				m_cleanText = txt;
			}
		}

		/************************************************************************/
		/*                                                                      */
		/************************************************************************/

		SubMenu::SubMenu(ControlContainer* owner)
			: m_itemPos(0,0), m_arrowPos(0,0), m_textPos(0,0), m_itemArea(0,0,0,0), m_borderArea(0,0,0,0),
			m_hoverIndex(-1), m_openPos(0,0), m_shadowColor(PACK_COLOR(0,0,0,127)),m_state(MENU_Closed),
			m_indexToOpen(-1)
		{
			setOwner(owner);
			m_helper.eventKeyPress().bind(this, &SubMenu::Keyboard_OnPress);
			m_helper.eventKeyRelease().bind(this, &SubMenu::Keyboard_OnRelease);

		}
	}
}