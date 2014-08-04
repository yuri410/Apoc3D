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
#include "InputAPI.h"

#include "apoc3d/Exception.h"
#include "apoc3d/Utility/StringUtils.h"
#include "Mouse.h"
#include "Keyboard.h"

using namespace Apoc3D::Utility;
using namespace Apoc3D::Graphics;

SINGLETON_DECL(Apoc3D::Input::InputAPIManager);

namespace Apoc3D
{
	namespace Input
	{
		InputAPIManager::~InputAPIManager()
		{
			m_factories.DeleteValuesAndClear();
		}

		void InputAPIManager::RegisterInputAPI(InputAPIFactory* fac)
		{
			if (m_factories.Contains(fac->getName()))
			{
				throw AP_EXCEPTION(ExceptID::InvalidOperation, L"Input API Already registered");
			}
			const List<PlatformAPISupport>& plats = fac->getDescription().SupportedPlatforms;

			for (int32 i = 0; i<plats.getCount();i++)
			{
				APIList* facList;

				String pName = plats[i].PlatformName;
				StringUtils::ToLowerCase(pName);

				if (!m_factories.TryGetValue(pName, facList))
				{
					facList = new APIList();
					m_factories.Add(pName, facList);
				}

				const Entry ent ={ fac, plats[i].Score };
				facList->Add(ent);
			}
		}
		void InputAPIManager::UnregisterInputAPI(const String& name)
		{
			bool passed = false;

			for (PlatformTable::Enumerator e = m_factories.GetEnumerator();
				e.MoveNext(); )
			{
				APIList* list = e.getCurrentValue();

				for (int32 i=list->getCount()-1; i>=0;i--)
				{
					if (list->operator[](i).Factory->getName() == name)
					{
						list->RemoveAt(i);
						passed = true;
					}
				}
			}

			if (!passed)
			{
				throw AP_EXCEPTION(ExceptID::InvalidOperation, L"Input API not registered");
			}
		}
		void InputAPIManager::UnregisterInputAPI(InputAPIFactory* fac)
		{
			bool passed = false;

			const List<PlatformAPISupport>& plats = fac->getDescription().SupportedPlatforms;
			for (int32 i = 0; i < plats.getCount(); i++)
			{

				String pName = plats[i].PlatformName;
				StringUtils::ToLowerCase(pName);

				APIList* facList;
				if (m_factories.TryGetValue(pName, facList))
				{
					for (int j = facList->getCount()-1; j >=0; j--)
					{
						if (facList->operator[](j).Factory == fac)
						{
							facList->RemoveAt(j);
							passed = true;
							break;
						}
					}
				}
			}
			if (!passed)
			{
				throw AP_EXCEPTION(ExceptID::InvalidOperation, L"Input API not registered");
			}
		}

		bool InputAPIManager::Comparison(const Entry& a, const Entry& b)
		{
			return a.PlatformMark < b.PlatformMark;
		}

		void InputAPIManager::InitializeInput(RenderWindow* window, const InputCreationParameters& params)
		{
			const String OSName = APOC3D_PLATFORM_NAME;

			APIList* list;
			if (m_factories.TryGetValue(OSName, list))
			{
				if (list->getCount()>0)
				{
					int32 bestIdx = -1;
					int32 bestMark = 0;
					for (int32 i=0;i<list->getCount();i++)
					{
						int32 m = list->operator[](i).PlatformMark;
						if (m>bestMark)
						{
							bestMark = m;
							bestIdx = i;
						}
					}

					m_selectedAPI = list->operator[](bestIdx).Factory;
					m_selectedAPI->Initialize(window);

					if (params.UseMouse)
					{
						m_mouse = CreateMouse();
					}
					if (params.UseKeyboard)
					{
						m_keyboard = CreateKeyboard();
					}
				}
				else throw AP_EXCEPTION(ExceptID::NotSupported, L"Platform not supported");
			}
			else throw AP_EXCEPTION(ExceptID::NotSupported, L"Platform not supported");
		}
		void InputAPIManager::FinalizeInput()
		{
			if (m_mouse)
				delete m_mouse;
			if (m_keyboard)
				delete m_keyboard;

		}
		Mouse* InputAPIManager::CreateMouse()
		{
			return m_selectedAPI->CreateMouse();
		}
		Keyboard* InputAPIManager::CreateKeyboard()
		{
			return m_selectedAPI->CreateKeyboard();
		}

		void InputAPIManager::Update(const GameTime* const time)
		{
			if (m_mouse)
				m_mouse->Update(time);
			if (m_keyboard)
				m_keyboard->Update(time);
		}
	}
}