#pragma once
#ifndef APOC3D_GRAPHICSAPI_H
#define APOC3D_GRAPHICSAPI_H

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

#include "apoc3d/Collections/HashMap.h"
#include "apoc3d/Platform/API.h"

using namespace Apoc3D::Core;
using namespace Apoc3D::Collections;
using namespace Apoc3D::Platform;

namespace Apoc3D
{
	namespace Graphics
	{
		namespace RenderSystem
		{
			/**
			 *  Manages all registered graphics APIs. 
			 *  Creates device content best suited for the platform.
			 */
			class APAPI GraphicsAPIManager
			{
				SINGLETON_DECL(GraphicsAPIManager);

			public:
				GraphicsAPIManager() { }
			
				virtual ~GraphicsAPIManager();

				bool RegisterGraphicsAPI(GraphicsAPIFactory* fac);
				bool UnregisterGraphicsAPI(const String& name);
				bool UnregisterGraphicsAPI(GraphicsAPIFactory* fac);
				
				DeviceContext* CreateDeviceContext();

			private:
				/**
				 *  Obtains the information for each graphics API.h
				 *  Platform mark is the API's score indicating how the API fits the current platform running on, scaled to 0-100.
				 */
				struct Entry
				{
					GraphicsAPIFactory* Factory;
					int PlatformMark;

				};
				typedef List<Entry> APIList;
				typedef HashMap<String, APIList*> PlatformTable;
				PlatformTable m_factories;
				
			};


			/** Factory that create device context of particular Graphics API. */
			class APAPI GraphicsAPIFactory
			{
			public:
				const APIDescription& getDescription() const { return m_description; }

				/** Gets the name of the graphics API. */
				const String& getName() { return m_description.Name; }

				/** Create the device content of the Graphics API. */
				virtual DeviceContext* CreateDeviceContext() = 0;


			protected:
				GraphicsAPIFactory(const APIDescription& desc)
					: m_description(desc)
				{ }
			private:
				APIDescription m_description;


			};
		}
	}
}

#endif