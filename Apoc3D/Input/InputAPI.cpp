/* -----------------------------------------------------------------------
 * This source file is part of Apoc3D Framework
 * 
 * Copyright (c) 2011-2017 Tao Xin
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

#include "InputAPI.h"

#include "apoc3d/Utility/StringUtils.h"
#include "Mouse.h"
#include "Keyboard.h"

using namespace Apoc3D::Utility;
using namespace Apoc3D::Graphics;

namespace Apoc3D
{
	namespace Input
	{
		SINGLETON_IMPL(InputAPIManager);
		
		InputAPIManager::InputAPIManager()
		{

		}
		InputAPIManager::~InputAPIManager()
		{
			m_factories.DeleteValuesAndClear();
		}

		bool InputAPIManager::RegisterInputAPI(InputAPIFactory* fac)
		{
			if (m_factories.Contains(fac->getName()))
			{
				AP_EXCEPTION(ErrorID::InvalidOperation, L"Input API Already registered");
				return false;
			}

			const List<PlatformAPISupport>& plats = fac->getDescription().SupportedPlatforms;

			for (int32 i = 0; i < plats.getCount(); i++)
			{
				APIList* facList;

				String pName = plats[i].PlatformName;
				StringUtils::ToLowerCase(pName);

				if (!m_factories.TryGetValue(pName, facList))
				{
					facList = new APIList();
					m_factories.Add(pName, facList);
				}

				const Entry ent = { fac, plats[i].Score };
				facList->Add(ent);
			}
			return true;
		}
		bool InputAPIManager::UnregisterInputAPI(const String& name)
		{
			bool passed = false;

			for (APIList* list : m_factories.getValueAccessor())
			{
				for (int32 i = list->getCount() - 1; i >= 0; i--)
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
				AP_EXCEPTION(ErrorID::InvalidOperation, L"Input API not registered");
			}
			return passed;
		}
		bool InputAPIManager::UnregisterInputAPI(InputAPIFactory* fac)
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
				AP_EXCEPTION(ErrorID::InvalidOperation, L"Input API not registered");
			}
			return passed;
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
				else AP_EXCEPTION(ErrorID::NotSupported, L"Platform not supported");
			}
			else AP_EXCEPTION(ErrorID::NotSupported, L"Platform not supported");
		}
		void InputAPIManager::FinalizeInput()
		{
			DELETE_AND_NULL(m_mouse);
			DELETE_AND_NULL(m_keyboard);
		}
		Mouse* InputAPIManager::CreateMouse()
		{
			return m_selectedAPI->CreateMouse();
		}
		Keyboard* InputAPIManager::CreateKeyboard()
		{
			return m_selectedAPI->CreateKeyboard();
		}

		void InputAPIManager::Update(const GameTime* time)
		{
			if (m_mouse)
				m_mouse->Update(time);
			if (m_keyboard)
				m_keyboard->Update(time);
		}
	}
}