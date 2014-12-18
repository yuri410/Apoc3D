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
#include "Menu.h"
#include "apoc3d/Utility/StringUtils.h"
#include "apoc3d/Input/InputAPI.h"
#include "apoc3d/Input/Mouse.h"
#include "FontManager.h"
#include "apoc3d/Graphics/RenderSystem/Sprite.h"
#include "apoc3d/Graphics/RenderSystem/Texture.h"
#include "StyleSkin.h"
#include "apoc3d/Core/GameTime.h"
#include "Form.h"

using namespace Apoc3D::Utility;
using namespace Apoc3D::Input;

namespace Apoc3D
{
	namespace UI
	{
		/************************************************************************/
		/*  MenuBar                                                             */
		/************************************************************************/

		MenuBar::MenuBar(const StyleSkin* skin)
			: Control(skin), m_skin(skin)
		{
			Initialize(skin);

			m_helper.eventKeyPress.Bind(this, &MenuBar::Keyboard_OnPress);
			m_helper.eventKeyRelease.Bind(this, &MenuBar::Keyboard_OnRelease);
		}

		MenuBar::~MenuBar()
		{

		}

		void MenuBar::Initialize(const StyleSkin* skin)
		{
			m_fontRef = m_skin->TitleTextFont;

			m_size.Y = static_cast<int>(m_fontRef->getLineBackgroundHeight());
		}
		void MenuBar::Add(MenuItem* item, SubMenu* submenu)
		{
			if (submenu)
			{
				submenu->setParentMenu(this);
				//submenu->setOwner(m_owner);
			}
			
			item->setSubMenu(submenu);

			m_items.Add(item);
		}
		void MenuBar::Add(const MenuItemSetupInfo& info, const StyleSkin* skin, ControlContainer* owner)
		{
			MenuItem* menuBtn = new MenuItem(info.Text);

			if (skin == nullptr)
				skin = m_skin;

			
			SubMenu* subMenu = nullptr;
			
			if (info.SubMenuItemsInfo.getCount() > 0 || info.ForceCreateSubMenu)
			{
				subMenu = new SubMenu(skin, owner, info.SubMenuItemsInfo);
			}

			if (info.ResultItem)
				*info.ResultItem = menuBtn;

			if (info.ResultSubMenu)
				*info.ResultSubMenu = subMenu;

			Add(menuBtn, subMenu);
		}

		void MenuBar::CheckSelection()
		{
			Mouse* mouse = InputAPIManager::getSingleton().getMouse();
			if (mouse->IsLeftPressed())
			{
				if (m_hoverIndex != -1)
				{
					MenuItem* hoveringItem = m_items[m_hoverIndex];

					CloseSubMenus();

					SubMenu* sm = hoveringItem->getSubMenu();
					if (sm && sm->getState() != MENU_Closed)
					{
						sm->Close();
						m_state = MENU_Closed;
						return;
					}

					if (sm && sm->getState() == MENU_Closed)
					{
						sm->Open(m_openPos);
						m_state = MENU_Open;
					}
				}
				else
				{
					for (MenuItem* itm : m_items)
					{
						SubMenu* ism = itm->getSubMenu();
						if (ism &&
							ism->getState() == MENU_Open &&
							ism->IsCursorInside())
						{
							return;
						}
					}
					CloseSubMenus();
				}
			}
		}
		void MenuBar::CheckHovering()
		{
			MenuItem* hoveringItem = m_items[m_hoverIndex];
			SubMenu* sm = hoveringItem->getSubMenu();

			if (sm && sm->getState() == MENU_Closed)
			{
				CloseSubMenus();
				sm->Open(m_openPos);
				m_state = MENU_Open;
			}
		}
		void MenuBar::Close()
		{
			CloseSubMenus();
			m_state = MENU_Closed;
		}
		void MenuBar::CloseSubMenus()
		{
			for (MenuItem* itm : m_items)
			{
				SubMenu* ism = itm->getSubMenu();
				if (ism && ism->getState() == MENU_Open)
				{
					ism->Close();
				}
			}
			m_state = MENU_Closed;
		}

		bool MenuBar::HasItemOpen() const
		{
			for (MenuItem* m : m_items)
			{
				if (m->getSubMenu() && m->getSubMenu()->getState() == MENU_Open)
				{
					return true;
				}
			}
			return false;
		}

		void MenuBar::Update(const GameTime* time)
		{
			CheckSelection();
			m_helper.Update(time);

			if (m_state == MENU_Open && m_hoverIndex != -1)
				CheckHovering();

			for (MenuItem* itm : m_items)
			{
				SubMenu* ism = itm->getSubMenu();

				if (ism && ism->getState() != MENU_Closed)
				{
					ism->Update(time);
				}
			}
		}
		void MenuBar::Keyboard_OnPress(KeyboardKeyCode key, KeyboardEventsArgs e)
		{
			switch (key)
			{
				case KEY_LMENU: //  alt
				case KEY_RMENU:
					m_altDown = true;
					break;
				default:
					m_indexToOpen = -1;
					for (int i = 0; i < m_items.getCount(); i++)
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
		void MenuBar::Keyboard_OnRelease(KeyboardKeyCode key, KeyboardEventsArgs e)
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

		void MenuBar::Draw(Sprite* sprite)
		{
			Mouse* mouse = InputAPIManager::getSingleton().getMouse();

			Point drawPos = GetAbsolutePosition();

			if (Owner)
			{
				m_size.X = Owner->getSize().X - 2;
			}
			else
			{
				m_size.X = SystemUI::GetUIArea(sprite->getRenderDevice()).Width;
			}

			{
				Apoc3D::Math::Rectangle area(drawPos.X + 1, drawPos.Y, m_size.X, m_size.Y - m_skin->HShade.Height);

				sprite->Draw(SystemUI::GetWhitePixel(), area, m_skin->ControlFaceColor);

				Apoc3D::Math::Rectangle shadeArea(area.X, area.getBottom(), m_size.X, m_skin->HShade.Height);
				sprite->Draw(m_skin->SkinTexture, shadeArea, &m_skin->HShade, CV_White);
			}
			

			m_itemPos.X = drawPos.X + 5;
			m_itemPos.Y = drawPos.Y;

			m_hoverIndex = -1;

			for (int i=0;i<m_items.getCount();i++)
			{
				MenuItem* itm = m_items[i]; 
				SubMenu* ism = itm->getSubMenu();

				Point cleanTextSize = m_fontRef->MeasureString(itm->getCleanText());
				m_itemArea.X = m_itemPos.X - 4;
				m_itemArea.Y = m_itemPos.Y;
				m_itemArea.Width = cleanTextSize.X + 10;
				m_itemArea.Height = static_cast<int>(m_fontRef->getLineBackgroundHeight());

				if (Owner == nullptr)
				{
					m_itemArea.X--;
				}

				Point cursorLoc = mouse->GetPosition();

				// selected item
				if (ism && ism->getState() != MENU_Closed)
				{
					sprite->Draw(SystemUI::GetWhitePixel(), m_itemArea, CV_LightGray);
					m_fontRef->DrawString(sprite, itm->getCleanText(), m_itemPos, m_skin->TextColor);
				}
				// hover item
				else if (m_itemArea.Contains(cursorLoc))
				{
					m_hoverIndex = i;
					sprite->Draw(SystemUI::GetWhitePixel(), m_itemArea, CV_Silver);
					m_fontRef->DrawString(sprite, itm->getCleanText(), m_itemPos, m_skin->TextColor);

					m_openPos.X = m_itemArea.X + 1;
					m_openPos.Y = m_itemArea.Y + m_itemArea.Height + 2;
				}
				// normal item
				else
				{
					m_fontRef->DrawString(sprite, itm->getCleanText(), m_itemPos, m_skin->TextColor);
				}

				if (m_indexToOpen == i)
				{
					itm->event.Invoke(itm);
					
					if (ism && ism->getState() == MENU_Closed)
					{
						m_openPos.X = m_itemArea.X + 1;
						m_openPos.Y = m_itemArea.Y + m_itemArea.Height + 2;
						CloseSubMenus();
						ism->Open(m_openPos);
						m_state = MENU_Open;
						m_openedMenu = true;
					}
					else
						Close();
					m_indexToOpen = -1;
				}

				if (itm->getKeyCode() != KEY_UNASSIGNED)
				{
					Point underscorePos = m_itemPos;
					underscorePos.X += m_fontRef->MeasureString(itm->getCleanText().substr(0, itm->getKeyIndex())).X;

					m_fontRef->DrawString(sprite, L"_", underscorePos, m_skin->TextColor);
				}
				m_itemPos.X += cleanTextSize.X + 10;
			}

			for (MenuItem* mi : m_items)
			{
				SubMenu* ism = mi->getSubMenu();

				if (ism && ism->getState() != MENU_Closed)
				{
					ism->Draw(sprite);
				}
			}
		}

		KeyboardKeyCode GetKey(wchar_t ch)
		{
			ch = static_cast<wchar_t>(tolower(ch));
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
				List<String> split;
				StringUtils::Split(m_text, split, '&');

				m_keyIndex = (int)split[0].length();
				m_key = GetKey(split[1][0]);
				m_cleanText.reserve(m_text.size());
				for (int32 i = 0; i < split.getCount(); i++)
				{
					m_cleanText += split[i];
				}
			}
			else
			{
				m_cleanText = txt;
			}
		}

		/************************************************************************/
		/*   MenuItem                                                           */
		/************************************************************************/

		MenuItem::MenuItem(const String& text)
		{
			setText(text);
		}


		/************************************************************************/
		/*  SubMenu                                                             */
		/************************************************************************/

		SubMenu::SubMenu(const StyleSkin* skin, ControlContainer* owner)
			: Control(skin), m_skin(skin)
		{
			m_helper.eventKeyPress.Bind(this, &SubMenu::Keyboard_OnPress);
			m_helper.eventKeyRelease.Bind(this, &SubMenu::Keyboard_OnRelease);
		}
		SubMenu::SubMenu(const StyleSkin* skin, ControlContainer* owner, const List<MenuItemSetupInfo>& itemsInfo)
			: SubMenu(skin, owner)
		{
			for (const MenuItemSetupInfo& msi : itemsInfo)
			{
				MenuItem* mi = new MenuItem(msi.Text);
				mi->event = msi.EventHandler;

				SubMenu* subMenu = nullptr;
				if (msi.SubMenuItemsInfo.getCount() > 0 || msi.ForceCreateSubMenu)
				{
					subMenu = new SubMenu(skin, owner, msi.SubMenuItemsInfo);
				}

				if (msi.ResultItem)
					*msi.ResultItem = mi;
				if (msi.ResultSubMenu)
					*msi.ResultSubMenu = subMenu;

				Add(mi, subMenu);
			}
		}

		SubMenu::~SubMenu()
		{
			CloseSubMenus();
		}

		void SubMenu::Add(MenuItem* item, SubMenu* submenu)
		{
			if (submenu)
			{
				submenu->setParentMenu(this);
			}
			item->setSubMenu(submenu);
			m_items.Add(item);
		}

		void SubMenu::Clear()
		{
			m_items.DeleteAndClear();
		}



		void SubMenu::CalcualteSize()
		{
			m_size.Y = 0;

			for (MenuItem* mi : m_items)
			{
				if (mi->getText() == L"_")
				{
					m_size.Y += 11;
				}
				else
				{
					m_size.Y += static_cast<int>(m_fontRef->getLineBackgroundHeight());
				}

				Point txtSize = m_fontRef->MeasureString(mi->getText());
				if (m_size.X < txtSize.X + 30)
					m_size.X = txtSize.X + 30;
			}
		}

		void SubMenu::Open(const Point& position)
		{
			if (m_size.X == 0 && m_size.Y == 0)
				CalcualteSize();

			Position = position - BaseOffset;

			m_state = MENU_Open;
		}

		void SubMenu::Close()
		{
			m_hoverIndex = -1;
			m_indexToOpen = -1;

			m_state = MENU_Closed;

			SubMenu* parentMenu = up_cast<SubMenu*>(m_parentMenu);
			if (parentMenu)
			{
				parentMenu->Close();
			}
			else
			{
				MenuBar* parentMenu2 = up_cast<MenuBar*>(m_parentMenu);
				if (parentMenu2) parentMenu2->Close();
			}

			CloseSubMenus();
		}
		void SubMenu::CloseSubMenus()
		{
			for (MenuItem* mi : m_items)
			{
				SubMenu* ism = mi->getSubMenu();

				if (ism && ism->getState() != MENU_Closed)
				{
					ism->m_state = MENU_Closed;
				}
			}
		}

		bool SubMenu::IsCursorInside()
		{
			Mouse* mouse = InputAPIManager::getSingleton().getMouse();
			Point cursorLoc = mouse->GetPosition();
			//if (!getOwner())
			//{
			//	cursorLoc 
			//}
			//else
			//{
			//	cursorLoc = mouse->GetCurrentPosition();
			//	cursorLoc.X -= getOwner()->Position.X;
			//	cursorLoc.Y -= getOwner()->Position.Y;
			//}

			if (getAbsoluteArea().Contains(cursorLoc))
				return true;

			for (MenuItem* mi : m_items)
			{
				SubMenu* ism = mi->getSubMenu();

				if (ism &&
					ism->getState() == MENU_Open &&
					ism->IsCursorInside())
				{
					return true;
				}
			}

			return false;
		}

		void SubMenu::Keyboard_OnPress(KeyboardKeyCode key, KeyboardEventsArgs e)
		{
			for (MenuItem* mi : m_items)
			{
				SubMenu* ism = mi->getSubMenu();

				if (ism && ism->getState() == MENU_Open)
				{
					return;
				}
			}

			for (int i = 0; i < m_items.getCount(); i++)
			{
				MenuItem* mi = m_items[i];

				if (key == mi->getKeyCode() && mi->Enabled)
					m_indexToOpen = i;
			}
		}
		void SubMenu::Keyboard_OnRelease(KeyboardKeyCode key, KeyboardEventsArgs e)
		{

		}

		void SubMenu::Update(const GameTime* time)
		{
			if (m_hoverIndex != -1)
				CheckSelection();

			m_helper.Update(time);

			for (MenuItem* mi : m_items)
			{
				SubMenu* ism = mi->getSubMenu();

				if (ism && ism->getState() != MENU_Closed)
				{
					ism->Update(time);
				}
			}

			if (m_timerStarted)
			{
				m_timerCount -= time->getElapsedTime();
				if (m_timerCount<0)
				{
					if (m_hoverIndex != -1 && m_items[m_hoverIndex]->getSubMenu() &&
						m_items[m_hoverIndex]->getSubMenu()->getState() != MENU_Open)
					{
						m_timerStarted = false;
						CloseSubMenus();
						m_items[m_hoverIndex]->getSubMenu()->Open(m_openPos);
					}
				}
			}
		}

		void SubMenu::CheckSelection()
		{
			Mouse* mouse = InputAPIManager::getSingleton().getMouse();

			if (mouse->IsLeftPressed())
			{
				int index = m_hoverIndex;
				if (!m_items[index]->Enabled)
					return;

				m_items[index]->event.Invoke(m_items[index]);

				if (m_items[index]->getSubMenu())
				{
					CloseSubMenus();
					if (m_items[index]->getSubMenu()->getState() == MENU_Closed)
						m_items[index]->getSubMenu()->Open(m_openPos);
				}
				else
					Close();
			}
		}

		void SubMenu::Draw(Sprite* sprite)
		{
			Point drawPos = GetAbsolutePosition();

			Mouse* mouse = InputAPIManager::getSingleton().getMouse();
			Apoc3D::Math::Rectangle area = getAbsoluteArea();
			area.Width = m_size.X;
			area.Height = m_size.Y;

			area.X = drawPos.X + 6;
			area.Y = drawPos.Y + 4;

			sprite->Draw(SystemUI::GetWhitePixel(), area, m_shadowColor);

			area = getAbsoluteArea();
			m_borderArea.X = area.X - 1;
			m_borderArea.Y = area.Y - 1;
			m_borderArea.Width = area.Width + 2;
			m_borderArea.Height = area.Height + 2;
			sprite->Draw(SystemUI::GetWhitePixel(), m_borderArea, CV_Black);
			sprite->Draw(SystemUI::GetWhitePixel(), area, CV_White);

			m_hoverIndex = -1;

			m_itemPos = drawPos;
			for (int i=0;i<m_items.getCount();i++)
			{
				MenuItem* mi = m_items[i];
				SubMenu* ism = mi->getSubMenu();

				m_itemArea.X = m_itemPos.X;
				m_itemArea.Y = m_itemPos.Y;

				if (m_itemArea.Width == 0)
					m_itemArea.Width = m_size.X;

				if (mi->getText().length() && mi->getText() != L"-")
				{
					if (!m_itemArea.Height)
						m_itemArea.Height = static_cast<int>(m_fontRef->getLineBackgroundHeight());

					Point cursorLoc = mouse->GetPosition();
					
					if (ism && ism->getState() != MENU_Closed)
					{
						sprite->Draw(SystemUI::GetWhitePixel(), m_itemArea, CV_LightGray);
					}
					else if (mi->Enabled && m_itemArea.Contains(cursorLoc))
					{
						m_hoverIndex = i;
						m_openPos.X = m_itemArea.X + m_itemArea.Width -3;
						m_openPos.Y = m_itemArea.Y;
						// timer start
						m_timerStarted = true;
						m_timerCount = 0.5f;

						sprite->Draw(SystemUI::GetWhitePixel(), m_itemArea, CV_Silver);
					}

					if (m_indexToOpen == i)
					{
						if (ism && ism->getState() == MENU_Closed)
						{
							m_openPos.X = m_itemArea.X + m_itemArea.Width - 3;
							m_openPos.Y = m_itemArea.Y;
							ism->Open(m_openPos);
							CloseSubMenus();
						}
						else
						{
							mi->event.Invoke(mi);
						}

					}

					m_textPos = m_itemPos;
					m_textPos.X += 2;
					m_fontRef->DrawString(sprite, mi->getCleanText(), m_textPos, mi->Enabled ? CV_Black : CV_Gray);
					m_itemPos.Y += static_cast<int>(m_fontRef->getLineBackgroundHeight());

					if (mi->getKeyCode() != KEY_UNASSIGNED)
					{
						Point underscorePos(
							m_itemPos.X + m_fontRef->MeasureString(mi->getCleanText().substr(0,mi->getKeyIndex())).X,
							m_itemPos.Y);
						m_fontRef->DrawString(sprite, L"_", underscorePos, mi->Enabled ? CV_Black : CV_Gray);
					}

					if (ism)
					{
						m_arrowPos.X = m_itemArea.X + m_itemArea.Width - 15;
						m_arrowPos.Y = m_itemArea.Y + (m_itemArea.Height - m_skin->SubMenuArrow.Height)/2;

						Apoc3D::Math::Rectangle dstRect = m_skin->SubMenuArrow;
						dstRect.X = m_arrowPos.X;
						dstRect.Y = m_arrowPos.Y;
						sprite->Draw(m_skin->SkinTexture, dstRect, &m_skin->SubMenuArrow, CV_Black);
					}
				}
				else if (mi->getText() == L"-")
				{
					Apoc3D::Math::Rectangle separator = Apoc3D::Math::Rectangle(m_itemPos.X, m_itemPos.Y + 6, m_size.X, 1);
					sprite->Draw(SystemUI::GetWhitePixel(), separator, CV_Silver);
					m_itemPos.Y += 11;
				}
			}
			
			for (MenuItem* mi : m_items)
			{
				SubMenu* ism = mi->getSubMenu();

				if (ism && ism->getState() != MENU_Closed)
				{
					ism->Draw(sprite);
				}
			}

			if (m_hoverIndex == -1 && m_timerStarted)
			{
				m_timerStarted = false;
			}
		}
	}
}