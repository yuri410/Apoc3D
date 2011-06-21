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
#include "InputAPI.h"

#include "Apoc3DException.h"
#include "Utility/StringUtils.h"

using namespace Apoc3D::Utility;

SINGLETON_DECL(Apoc3D::Input::InputAPIManager);

namespace Apoc3D
{
	namespace Input
	{
		InputAPIManager::~InputAPIManager()
		{
			for (PlatformTable::iterator iter = m_factories.begin();
				iter != m_factories.end(); iter++)
			{
				APIList* list = iter->second;
				delete list;
			}
		}

		void InputAPIManager::RegisterInputAPI(InputAPIFactory* fac)
		{
			if (m_factories.find(fac->getName()) != m_factories.end())
			{
				Apoc3DException::createException(EX_InvalidOperation, L"Input API Already registered");
			}


			const vector<PlatformAPISupport>& plats = fac->getDescription().SupportedPlatforms;

			for (size_t i = 0; i<plats.size();i++)
			{
				APIList* facList;

				String pName = plats[i].PlatformName;
				StringUtils::ToLowerCase(pName);

				PlatformTable::iterator iter = m_factories.find(pName);
				if (iter == m_factories.end())
				{
					facList = new APIList();
					m_factories.insert(pair<String, APIList*>(pName, facList));
				}
				else
				{
					facList = iter->second;
				}

				const Entry ent ={ fac, plats[i].Score };
				facList->push_back(ent);
			}
		}
		void InputAPIManager::UnregisterInputAPI(const String& name)
		{
			if (m_factories.find(name) == m_factories.end())
			{
				Apoc3DException::createException(EX_InvalidOperation, L"Graphics API not registered");
			}

			for (PlatformTable::iterator iter = m_factories.begin();
				iter != m_factories.end(); iter++)
			{
				APIList* list = static_cast<APIList*>(iter->second);

				for (int i=static_cast<int>(list->size())-1; i>=0;i--)
				{
					if (list->operator[](i).Factory->getName() == name)
					{
						APIList::iterator i2 = list->begin();
						i2+=i;
						list->erase(i2);
					}
				}
			}
		}
		void InputAPIManager::UnregisterInputAPI(InputAPIFactory* fac)
		{
			if (m_factories.find(fac->getName()) == m_factories.end())
			{
				Apoc3DException::createException(EX_InvalidOperation, L"Graphics API not registered");
			}


			const vector<PlatformAPISupport>& plats = fac->getDescription().SupportedPlatforms;
			for (size_t i = 0; i < plats.size(); i++)
			{

				String pName = plats[i].PlatformName;
				StringUtils::ToLowerCase(pName);

				PlatformTable::iterator iter = m_factories.find(pName);

				if (iter != m_factories.end())
				{
					APIList* facList = iter->second;
					for (int j = facList->size(); j >0; j--)
					{
						if (facList->operator[](j).Factory == fac)
						{
							APIList::iterator i2 = facList->begin();
							i2+=i;
							facList->erase(i2);

							break;
						}
					}
				}
			}
		}

		bool InputAPIManager::Comparison(const Entry& a, const Entry& b)
		{
			return a.PlatformMark < b.PlatformMark;
		}


		Mouse* InputAPIManager::CreateMouse()
		{

		}
		Keyboard* InputAPIManager::CreateKeyboard()
		{

		}
	}
}