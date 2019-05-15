#pragma once
#ifndef APOC3D_INPUTAPI_H
#define APOC3D_INPUTAPI_H

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

#include "apoc3d/Collections/List.h"
#include "apoc3d/Collections/HashMap.h"
#include "apoc3d/Platform/API.h"

using namespace Apoc3D::Core;
using namespace Apoc3D::Collections;
using namespace Apoc3D::Graphics::RenderSystem;
using namespace Apoc3D::Platform;

namespace Apoc3D
{
	namespace Input
	{
		struct InputCreationParameters
		{
			bool UseMouse = false;
			bool UseKeyboard = false;
			int PreferredGamepadCount = 0;
		};

		class APAPI InputAPIFactory
		{
		public:
			const APIDescription& getDescription() const { return m_description; }

			const String& getName() { return m_description.Name; }

			virtual void Initialize(RenderWindow* window) = 0;
			virtual Mouse* CreateMouse() = 0;
			virtual Keyboard* CreateKeyboard() = 0;

		protected:
			InputAPIFactory(const APIDescription& desc)
				: m_description(desc)
			{ }

		private:
			APIDescription m_description;

		};

		class APAPI InputAPIManager
		{
			SINGLETON_DECL(InputAPIManager);
		public:
			InputAPIManager();
			virtual ~InputAPIManager();
		
			bool RegisterInputAPI(InputAPIFactory* fac);
			bool UnregisterInputAPI(const String& name);
			bool UnregisterInputAPI(InputAPIFactory* fac);

			void InitializeInput(RenderWindow* window, const InputCreationParameters& params);
			void FinalizeInput();

			void Update(const AppTime* time);

			Mouse* getMouse() const { return m_mouse; }
			Keyboard* getKeyboard() const { return m_keyboard; }

		private:
			Mouse* CreateMouse();
			Keyboard* CreateKeyboard();

			struct Entry
			{
				InputAPIFactory* Factory;
				int PlatformMark;
			};

			typedef List<Entry> APIList;
			typedef HashMap<String, APIList*> PlatformTable;
			PlatformTable m_factories;

			InputAPIFactory* m_selectedAPI = nullptr;

			Mouse* m_mouse = nullptr;
			Keyboard* m_keyboard = nullptr;
		};

	}
}
#endif