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
#include "GraphicsAPI.h"

#include "apoc3d/ApocException.h"
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
				m_factories.DeleteValuesAndClear();
			}

			void GraphicsAPIManager::RegisterGraphicsAPI(GraphicsAPIFactory* fac)
			{
				if (m_factories.Contains(fac->getName()))
				{
					throw AP_EXCEPTION(ApocExceptionType::InvalidOperation, L"The Graphics API " + fac->getName() + L" is already registered");
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
			void GraphicsAPIManager::UnregisterGraphicsAPI(const String& name)
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
					throw AP_EXCEPTION(ApocExceptionType::InvalidOperation, L"The graphics API " + name + L" is not registered");
				}
			}
			void GraphicsAPIManager::UnregisterGraphicsAPI(GraphicsAPIFactory* fac)
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
					throw AP_EXCEPTION(ApocExceptionType::InvalidOperation, L"Graphics API not registered");
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
						return list->operator[](bestIdx).Factory->CreateDeviceContext();
					}
					throw AP_EXCEPTION(ApocExceptionType::NotSupported, L"Platform not supported");
				}
				throw AP_EXCEPTION(ApocExceptionType::NotSupported, L"Platform not supported");
			}
		}
	}
}