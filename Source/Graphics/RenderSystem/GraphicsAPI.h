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
#ifndef GRAPHICSAPI_H
#define GRAPHICSAPI_H

#include "Core/Singleton.h"
#include "Platform/API.h"

using namespace Apoc3D::Core;
using namespace Apoc3D::Platform;
using namespace std;

namespace Apoc3D
{
	namespace Graphics
	{
		namespace RenderSystem
		{
			/* Manages all registered graphics APIs. 
			* Creates device content best suited for the platform.
			*/
			class APAPI GraphicsAPIManager : public Singleton<GraphicsAPIManager>
			{
			private:
				struct Entry
				{
					GraphicsAPIFactory* Factory;
					int PlatformMark;

				};
				typedef vector<const Entry> APIList;
				typedef unordered_map<String, APIList*> PlatformTable;
				PlatformTable m_factories;
				
				static bool Comparison(const Entry& a, const Entry& b);

			public:
				GraphicsAPIManager() { }
			
				virtual ~GraphicsAPIManager();
			public:
				void RegisterGraphicsAPI(GraphicsAPIFactory* fac);
				void UnregisterGraphicsAPI(const String& name);
				void UnregisterGraphicsAPI(GraphicsAPIFactory* fac);
				
				DeviceContent* CreateDeviceContent();
			
				SINGLETON_DECL_HEARDER(GraphicsAPIManager);
			};


			/* Factory that create device content of particular Graphics API.
			*/
			class APAPI GraphicsAPIFactory
			{
			private:
				APIDescription m_description;

			protected:
				GraphicsAPIFactory(const APIDescription& desc)
					: m_description(desc)
				{ }

			public:
				const APIDescription& getDescription() const { return m_description; }

				const String& getName() { return m_description.Name; }

				/** Create the device content of the Graphics API.
				*/
				virtual DeviceContent* CreateDeviceContent() = 0;
			};
		}
	}
}

#endif