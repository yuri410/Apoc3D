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
#include "GraphicsAPI.h"

#include "apoc3d/Apoc3DException.h"
#include "apoc3d/Utility/StringUtils.h"

using namespace Apoc3D;
using namespace Apoc3D::Utility;

SINGLETON_DECL(Apoc3D::Graphics::RenderSystem::GraphicsAPIManager);

namespace Apoc3D
{
	namespace Graphics
	{
		namespace RenderSystem
		{
			GraphicsAPIManager::~GraphicsAPIManager()
			{
				for (PlatformTable::iterator iter = m_factories.begin();
					iter != m_factories.end(); iter++)
				{
					APIList* list = iter->second;
					delete list;
				}
			}

			void GraphicsAPIManager::RegisterGraphicsAPI(GraphicsAPIFactory* fac)
			{
				if (m_factories.find(fac->getName()) != m_factories.end())
				{
					Apoc3DException::createException(EX_InvalidOperation, L"Graphics API Already registered");
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
			void GraphicsAPIManager::UnregisterGraphicsAPI(const String& name)
			{
				//if (m_factories.find(name) == m_factories.end())
				//{
				//	Apoc3DException::createException(EX_InvalidOperation, L"Graphics API not registered");
				//}
				bool passed = false;

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
							passed = true;
						}
					}
				}
				if (!passed)
				{
					Apoc3DException::createException(EX_InvalidOperation, L"Graphics API not registered");
				}
			}
			void GraphicsAPIManager::UnregisterGraphicsAPI(GraphicsAPIFactory* fac)
			{
				//if (m_factories.find(fac->getName()) == m_factories.end())
				//{
				//	Apoc3DException::createException(EX_InvalidOperation, L"Graphics API not registered");
				//}
				bool passed = false;

				const vector<PlatformAPISupport>& plats = fac->getDescription().SupportedPlatforms;
				for (size_t i = 0; i < plats.size(); i++)
				{

					String pName = plats[i].PlatformName;
					StringUtils::ToLowerCase(pName);

					PlatformTable::iterator iter = m_factories.find(pName);

					if (iter != m_factories.end())
					{
						APIList* facList = iter->second;
						for (int j = facList->size()-1; j >=0; j--)
						{
							if (facList->operator[](j).Factory == fac)
							{
								APIList::iterator i2 = facList->begin();
								i2+=i;
								facList->erase(i2);
								passed = true;
								break;
							}
						}
					}
				}
				if (!passed)
				{
					Apoc3DException::createException(EX_InvalidOperation, L"Graphics API not registered");
				}
			}

			bool GraphicsAPIManager::Comparison(const Entry& a, const Entry& b)
			{
				return a.PlatformMark < b.PlatformMark;
				//if (a.PlatformMark == b.PlatformMark)
					//return 0;
				//return a.PlatformMark < b.PlatformMark ? -1 : 1;
			}

			/** Find a best suited a Graphics API for current platform and create a device context.
			*/
			DeviceContext* GraphicsAPIManager::CreateDeviceContext()
			{
				const String OSName = APOC3D_PLATFORM_NAME;
				
				PlatformTable::iterator iter = m_factories.find(OSName);
				if (iter != m_factories.end())
				{
					APIList* list = iter->second;
					if (!list->empty())
					{
						std::sort(list->begin(), list->end(), GraphicsAPIManager::Comparison);

						int idx = list->size();
						idx--;
						return list->operator[](idx).Factory->CreateDeviceContext();
					}
					throw Apoc3DException::createException(EX_NotSupported, L"Platform not supported");
				}
				throw Apoc3DException::createException(EX_NotSupported, L"Platform not supported");
			}
		}
	}
}