#pragma once
#ifndef APOC3D_INPUTAPI_H
#define APOC3D_INPUTAPI_H

/**
 * -----------------------------------------------------------------------------
 * This source file is part of Apoc3D Engine
 * 
 * Copyright (c) 2009+ Tao Xin
 * 
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 * 
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 * 
 * You should have received a copy of the GNU General Public License
 * along with this program.  if not, write to the Free Software Foundation, 
 * Inc., 59 Temple Place - Suite 330, Boston, MA 02111-1307, USA, or go to
 * http://www.gnu.org/copyleft/gpl.txt.
 * 
 * -----------------------------------------------------------------------------
 */

#include "apoc3d/Core/Singleton.h"
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
			bool UseMouse;
			bool UseKeyboard;
			int PreferredGamepadCount;
		};
		class APAPI InputAPIFactory
		{
		private:
			APIDescription m_description;

		protected:
			InputAPIFactory(const APIDescription& desc)
				: m_description(desc)
			{ }

		public:
			const APIDescription& getDescription() const { return m_description; }

			const String& getName() { return m_description.Name; }

			virtual void Initialize(RenderWindow* window) = 0;
			virtual Mouse* CreateMouse() = 0;
			virtual Keyboard* CreateKeyboard() = 0;
			//virtual DeviceContent* CreateDeviceContent() = 0;
		};
		class APAPI InputAPIManager : public Singleton<InputAPIManager>
		{
		private:
			struct Entry
			{
				InputAPIFactory* Factory;
				int PlatformMark;

			};
			typedef List<Entry> APIList;
			typedef HashMap<String, APIList*> PlatformTable;
			PlatformTable m_factories;

			InputAPIFactory* m_selectedAPI;

			static bool Comparison(const Entry& a, const Entry& b);

			Mouse* m_mouse;
			Keyboard* m_keyboard;

			virtual Mouse* CreateMouse();
			virtual Keyboard* CreateKeyboard();
		public:
			Mouse* getMouse() const { return m_mouse; }
			Keyboard* getKeyboard() const { return m_keyboard; }

			InputAPIManager()
				: m_mouse(0), m_keyboard(0)
			{
			}

			virtual ~InputAPIManager();
		
			void RegisterInputAPI(InputAPIFactory* fac);
			void UnregisterInputAPI(const String& name);
			void UnregisterInputAPI(InputAPIFactory* fac);

			void InitializeInput(RenderWindow* window, const InputCreationParameters& params);
			void FinalizeInput();
			void Update(const GameTime* const time);
			SINGLETON_DECL_HEARDER(InputAPIManager);
		};

	}
}
#endif