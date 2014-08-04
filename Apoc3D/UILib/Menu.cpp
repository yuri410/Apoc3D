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
		Menu::Menu()
			: m_drawPos(0,0), m_itemPos(0,0), m_hoverIndex(-1), m_openPos(0,0), m_itemArea(0,0,0,0),
			m_state(MENU_Closed), m_altDown(false), m_openedMenu(false), m_indexToOpen(-1)
		{
			m_helper.eventKeyPress().Bind(this, &Menu::Keyboard_OnPress);
			m_helper.eventKeyRelease().Bind(this, &Menu::Keyboard_OnRelease);
			
		}

		Menu::~Menu()
		{

		}

		void Menu::Initialize(RenderDevice* device)
		{
			Control::Initialize(device);

			m_fontRef = m_skin->TitleTextFont;

			for (int i=0;i<m_items.getCount();i++)
			{
				if (m_items[i]->getSubMenu())
				{
					m_items[i]->getSubMenu()->Initialize(device);
				}
			}
			Size.Y = static_cast<int>(m_fontRef->getLineBackgroundHeight());
		}
		void Menu::Add(MenuItem* item, SubMenu* submenu)
		{
			if (submenu)
			{
				submenu->setParent( this);
				submenu->setOwner(m_owner);
			}
			
			item->setSubMenu(submenu);

			m_items.Add(item);
		}

		void Menu::CheckSelection()
		{
			Mouse* mouse = InputAPIManager::getSingleton().getMouse();
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
						m_items[m_hoverIndex]->getSubMenu()->getState()==MENU_Closed)
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
							return;
						}
					}
					CloseSubMenus();
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
			{
				if (m_items[i]->getSubMenu() && m_items[i]->getSubMenu()->getState() != MENU_Closed)
				{
					m_items[i]->getSubMenu()->Update(time);
				}
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
			Mouse* mouse = InputAPIManager::getSingleton().getMouse();

			if (getOwner())
			{
				Size.X = getOwner()->Size.X - 2;
			}
			else
			{
				Size.X = UIRoot::GetUIArea(sprite->getRenderDevice()).Width;
				Size.Y = static_cast<int>(m_fontRef->getLineBackgroundHeight());
			}
			{
				Apoc3D::Math::Rectangle area(Position.X+1,Position.Y, Size.X,Size.Y - m_skin->HShade.Height);

				sprite->Draw(m_skin->WhitePixelTexture, area, m_skin->ControlFaceColor);

				Apoc3D::Math::Rectangle shadeArea(area.X, area.getBottom(), Size.X, m_skin->HShade.Height);
				sprite->Draw(m_skin->SkinTexture, shadeArea, &m_skin->HShade, CV_White);
			}
			

			m_itemPos.X = Position.X + 5;
			m_itemPos.Y = Position.Y;

			m_hoverIndex = -1;

			for (int i=0;i<m_items.getCount();i++)
			{
				Point cleanTextSize = m_fontRef->MeasureString(m_items[i]->getCleanText());
				m_itemArea.X = m_itemPos.X - 4;
				m_itemArea.Y = m_itemPos.Y;
				m_itemArea.Width = cleanTextSize.X + 10;
				m_itemArea.Height = static_cast<int>(m_fontRef->getLineBackgroundHeight());

				if (!getOwner())
				{
					m_itemArea.X--;
				}

				Point cursorLoc;
				if (!getOwner())
				{
					cursorLoc = mouse->GetCurrentPosition();
				}
				else
				{
					cursorLoc = mouse->GetCurrentPosition();
					cursorLoc.X -= getOwner()->Position.X;
					cursorLoc.Y -= getOwner()->Position.Y;
				}

				// selected item
				if (m_items[i]->getSubMenu() &&
					m_items[i]->getSubMenu()->getState() != MENU_Closed)
				{
					sprite->Draw(m_skin->WhitePixelTexture, m_itemArea, CV_LightGray);
					m_fontRef->DrawString(sprite, m_items[i]->getCleanText(), m_itemPos, m_skin->TextColor);
				}
				// hover item
				else if (m_itemArea.Contains(cursorLoc))
				{
					m_hoverIndex = i;
					sprite->Draw(m_skin->WhitePixelTexture, m_itemArea, CV_Silver);
					m_fontRef->DrawString(sprite, m_items[i]->getCleanText(), m_itemPos, m_skin->TextColor);

					m_openPos.X = m_itemArea.X + 1;
					m_openPos.Y = m_itemArea.Y + m_itemArea.Height + 2;
				}
				// normal item
				else
				{
					m_fontRef->DrawString(sprite, m_items[i]->getCleanText(), m_itemPos, m_skin->TextColor);
				}

				if (m_indexToOpen == i)
				{
					m_items[i]->event.Invoke(m_items[i]);
					
					if (m_items[i]->getSubMenu() &&
						m_items[i]->getSubMenu()->getState() == MENU_Closed)
					{
						m_openPos.X = m_itemArea.X + 1;
						m_openPos.Y = m_itemArea.Y + m_itemArea.Height + 2;
						CloseSubMenus();
						m_items[i]->getSubMenu()->Open(m_openPos);
						m_state = MENU_Open;
						m_openedMenu = true;
					}
					else
						Close();
					m_indexToOpen=-1;
				}

				if (m_items[i]->getKeyCode() != KEY_UNASSIGNED)
				{
					Point underscorePos(
						m_itemPos.X + m_fontRef->MeasureString(m_items[i]->getCleanText().substr(0,m_items[i]->getKeyIndex())).X,
						m_itemPos.Y);
					m_fontRef->DrawString(sprite, L"_", underscorePos, m_skin->TextColor);
				}
				m_itemPos.X += cleanTextSize.X + 10;
			}

			for (int i=0;i<m_items.getCount();i++)
			{
				MenuItem* itm = m_items[i];
				if (itm->getSubMenu() && 
					itm->getSubMenu()->getState() != MENU_Closed)
				{
					itm->getSubMenu()->Draw(sprite);
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
				StringUtils::Split(m_text, split, L"&");

				m_keyIndex = (int)split[0].length();
				m_key = GetKey(split[1][0]);
				m_cleanText.reserve(m_text.size());
				for (int32 i=0;i<split.getCount();i++)
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
			m_hoverIndex(-1), m_openPos(0,0), m_shadowColor(CV_PackColor(0,0,0,127)),m_state(MENU_Closed),
			m_indexToOpen(-1),
			m_timerStarted(0), m_timerCount(0.5f)
		{
			setOwner(owner);
			m_helper.eventKeyPress().Bind(this, &SubMenu::Keyboard_OnPress);
			m_helper.eventKeyRelease().Bind(this, &SubMenu::Keyboard_OnRelease);

		}

		SubMenu::~SubMenu()
		{
			CloseSubMenus();
		}

		void SubMenu::Add(MenuItem* item, SubMenu* submenu)
		{
			if (submenu)
			{
				submenu->setParent(this);
			}
			item->setSubMenu(submenu);
			m_items.Add(item);
		}

		void SubMenu::Clear()
		{
			for (int i=0;i<m_items.getCount();i++)
				delete m_items[i];
			m_items.Clear();
		}

		void SubMenu::Initialize(RenderDevice* device)
		{
			Control::Initialize(device);
			for (int i=0;i<m_items.getCount();i++)
			{
				if (m_items[i]->getSubMenu())
					m_items[i]->getSubMenu()->Initialize(device);
			}
		}

		void SubMenu::CalcualteSize()
		{
			Size.Y = 0;
			for (int i=0;i<m_items.getCount();i++)
			{
				if (m_items[i]->getText()==L"_")
				{
					Size.Y += 11;
				}
				else
				{
					Size.Y += static_cast<int>(m_fontRef->getLineBackgroundHeight());
				}

				Point txtSize = m_fontRef->MeasureString(m_items[i]->getText());
				if (Size.X <txtSize.X + 30)
					Size.X = txtSize.X + 30;
			}
		}

		void SubMenu::Open(const Point& position)
		{
			if (Size.X==0 && Size.Y ==0)
				CalcualteSize();

			Position = position;

			m_state = MENU_Open;
		}
		void SubMenu::Close()
		{
			m_hoverIndex = -1;
			m_indexToOpen = -1;

			m_state = MENU_Closed;

			SubMenu* parentMenu = up_cast<SubMenu*>(m_parent);
			if (parentMenu)
			{
				parentMenu->Close();
			}
			else
			{
				Menu* parentMenu2 = up_cast<Menu*>(m_parent);
				if (parentMenu2) parentMenu2->Close();
			}

			CloseSubMenus();
		}
		void SubMenu::CloseSubMenus()
		{
			for (int i=0;i<m_items.getCount();i++)
			{
				if (m_items[i]->getSubMenu() &&
					m_items[i]->getSubMenu()->getState() != MENU_Closed)
				{
					m_items[i]->getSubMenu()->m_state = MENU_Closed;
				}
			}
		}

		bool SubMenu::IsCursorInside()
		{
			Mouse* mouse = InputAPIManager::getSingleton().getMouse();
			Point cursorLoc = mouse->GetCurrentPosition();
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

			for (int i=0;i<m_items.getCount();i++)
			{
				if (m_items[i]->getSubMenu() &&
					m_items[i]->getSubMenu()->getState() == MENU_Open &&
					m_items[i]->getSubMenu()->IsCursorInside())
				{
					return true;
				}
			}

			return false;
		}

		void SubMenu::Keyboard_OnPress(KeyboardKeyCode key, KeyboardEventsArgs e)
		{
			for (int i=0;i<m_items.getCount();i++)
			{
				if (m_items[i]->getSubMenu() &&
					m_items[i]->getSubMenu()->getState() == MENU_Open)
				{
					return;
				}
			}

			for (int i=0;i<m_items.getCount();i++)
			{
				if (key == m_items[i]->getKeyCode() && m_items[i]->Enabled)
					m_indexToOpen = i;
			}
		}
		void SubMenu::Keyboard_OnRelease(KeyboardKeyCode key, KeyboardEventsArgs e)
		{

		}

		void SubMenu::Update(const GameTime* const time)
		{
			if (m_hoverIndex != -1)
				CheckSelection();

			m_helper.Update(time);

			for (int i=0;i<m_items.getCount();i++)
			{
				if (m_items[i]->getSubMenu() &&
					m_items[i]->getSubMenu()->getState() != MENU_Closed)
				{
					m_items[i]->getSubMenu()->Update(time);
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
			Mouse* mouse = InputAPIManager::getSingleton().getMouse();
			Apoc3D::Math::Rectangle area = getArea();
			area.Width = Size.X;
			area.Height = Size.Y;

			area.X = Position.X + 6;
			area.Y = Position.Y + 4;

			sprite->Draw(m_skin->WhitePixelTexture, area, m_shadowColor);

			area = getArea();
			m_borderArea.X = area.X - 1;
			m_borderArea.Y = area.Y - 1;
			m_borderArea.Width = area.Width + 2;
			m_borderArea.Height = area.Height + 2;
			sprite->Draw(m_skin->WhitePixelTexture, m_borderArea, CV_Black);
			sprite->Draw(m_skin->WhitePixelTexture, area, CV_White);

			m_hoverIndex = -1;

			m_itemPos = Position;
			for (int i=0;i<m_items.getCount();i++)
			{
				m_itemArea.X = m_itemPos.X;
				m_itemArea.Y = m_itemPos.Y;

				if (!m_itemArea.Width)
					m_itemArea.Width = Size.X;

				if (m_items[i]->getText().length() && m_items[i]->getText() != L"-")
				{
					if (!m_itemArea.Height)
						m_itemArea.Height = static_cast<int>(m_fontRef->getLineBackgroundHeight());

					Point cursorLoc;
					if (!getOwner())
					{
						cursorLoc = mouse->GetCurrentPosition();
					}
					else
					{
						cursorLoc = mouse->GetCurrentPosition();
						cursorLoc.X -= getOwner()->Position.X;
						cursorLoc.Y -= getOwner()->Position.Y;
					}

					if (m_items[i]->getSubMenu() &&
						m_items[i]->getSubMenu()->getState() != MENU_Closed)
					{
						sprite->Draw(m_skin->WhitePixelTexture, m_itemArea, CV_LightGray);
					}
					else if (m_items[i]->Enabled && m_itemArea.Contains(cursorLoc))
					{
						m_hoverIndex = i;
						m_openPos.X = m_itemArea.X + m_itemArea.Width -3;
						m_openPos.Y = m_itemArea.Y;
						// timer start
						m_timerStarted = true;
						m_timerCount = 0.5f;

						sprite->Draw(m_skin->WhitePixelTexture, m_itemArea, CV_Silver);
					}

					if (m_indexToOpen == i)
					{
						if (m_items[i]->getSubMenu() &&
							m_items[i]->getSubMenu()->getState() == MENU_Closed)
						{
							m_openPos.X = m_itemArea.X + m_itemArea.Width - 3;
							m_openPos.Y = m_itemArea.Y;
							m_items[i]->getSubMenu()->Open(m_openPos);
							CloseSubMenus();
						}
						else
						{
							m_items[i]->event.Invoke(m_items[i]);
						}

					}

					m_textPos = m_itemPos;
					m_textPos.X += 2;
					m_fontRef->DrawString(sprite, m_items[i]->getCleanText(), m_textPos, m_items[i]->Enabled ? CV_Black : CV_Gray);
					m_itemPos.Y += static_cast<int>(m_fontRef->getLineBackgroundHeight());

					if (m_items[i]->getKeyCode() != KEY_UNASSIGNED)
					{
						Point underscorePos(
							m_itemPos.X + m_fontRef->MeasureString(m_items[i]->getCleanText().substr(0,m_items[i]->getKeyIndex())).X,
							m_itemPos.Y);
						m_fontRef->DrawString(sprite, L"_", underscorePos, m_items[i]->Enabled ? CV_Black : CV_Gray);
					}

					if (m_items[i]->getSubMenu())
					{
						m_arrowPos.X = m_itemArea.X + m_itemArea.Width - 15;
						m_arrowPos.Y = m_itemArea.Y + (m_itemArea.Height - m_skin->SubMenuArrow.Height)/2;

						Apoc3D::Math::Rectangle dstRect = m_skin->SubMenuArrow;
						dstRect.X = m_arrowPos.X;
						dstRect.Y = m_arrowPos.Y;
						sprite->Draw(m_skin->SkinTexture, dstRect, &m_skin->SubMenuArrow, CV_Black);
					}
				}
				else if (m_items[i]->getText() == L"-")
				{
					Apoc3D::Math::Rectangle separator = Apoc3D::Math::Rectangle(m_itemPos.X, m_itemPos.Y+6, Size.X,1);
					sprite->Draw(m_skin->WhitePixelTexture, separator, CV_Silver);
					m_itemPos.Y += 11;
				}
			}
			
			for (int i=0;i<m_items.getCount();i++)
			{
				if (m_items[i]->getSubMenu() && 
					m_items[i]->getSubMenu()->getState() != MENU_Closed)
				{
					m_items[i]->getSubMenu()->Draw(sprite);
				}
			}

			if (m_hoverIndex == -1 && m_timerStarted)
			{
				m_timerStarted = false;
			}
		}
	}
}