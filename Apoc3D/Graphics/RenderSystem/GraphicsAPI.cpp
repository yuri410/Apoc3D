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

#include "GraphicsAPI.h"

#include "apoc3d/Utility/StringUtils.h"

using namespace Apoc3D;
using namespace Apoc3D::Utility;

namespace Apoc3D
{
	namespace Graphics
	{
		namespace RenderSystem
		{
			SINGLETON_IMPL(GraphicsAPIManager);

			GraphicsAPIManager::~GraphicsAPIManager()
			{
				m_factories.DeleteValuesAndClear();
			}

			bool GraphicsAPIManager::RegisterGraphicsAPI(GraphicsAPIFactory* fac)
			{
				if (m_factories.Contains(fac->getName()))
				{
					AP_EXCEPTION(ErrorID::InvalidOperation, L"The Graphics API " + fac->getName() + L" is already registered");
					return false;
				}

				for (const PlatformAPISupport& p : fac->getDescription().SupportedPlatforms)
				{
					APIList* facList;

					String pName = p.PlatformName;
					StringUtils::ToLowerCase(pName);

					if (!m_factories.TryGetValue(pName, facList))
					{
						facList = new APIList();
						m_factories.Add(pName, facList);
					}

					const Entry ent = { fac, p.Score };
					facList->Add(ent);
				}
				return true;
			}
			bool GraphicsAPIManager::UnregisterGraphicsAPI(const String& name)
			{
				bool passed = false;

				for (APIList* list : m_factories.getValueAccessor())
				{
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
					AP_EXCEPTION(ErrorID::InvalidOperation, L"The graphics API " + name + L" is not registered");
				}
				return passed;
			}
			bool GraphicsAPIManager::UnregisterGraphicsAPI(GraphicsAPIFactory* fac)
			{
				bool passed = false;

				for (const PlatformAPISupport& p : fac->getDescription().SupportedPlatforms)
				{
					String pName = p.PlatformName;
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
					AP_EXCEPTION(ErrorID::InvalidOperation, L"Graphics API not registered");
				}
				return passed;
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
				}
				AP_EXCEPTION(ErrorID::NotSupported, L"Platform not supported");
				return nullptr;
			}
		}
	}
}