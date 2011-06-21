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
#ifndef INPUTAPI_H
#define INPUTAPI_H

#include "Common.h"

#include "Core/Singleton.h"
#include "Platform/API.h"

using namespace Apoc3D::Platform;
using namespace Apoc3D::Core;
using namespace std;

namespace Apoc3D
{
	namespace Input
	{
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
			typedef vector<const Entry> APIList;
			typedef unordered_map<String, APIList*> PlatformTable;
			PlatformTable m_factories;

			static bool Comparison(const Entry& a, const Entry& b);

		public:
			InputAPIManager() { }

			virtual ~InputAPIManager();
		public:
			void RegisterInputAPI(InputAPIFactory* fac);
			void UnregisterInputAPI(const String& name);
			void UnregisterInputAPI(InputAPIFactory* fac);

			virtual Mouse* CreateMouse();
			virtual Keyboard* CreateKeyboard();
		public:
			SINGLETON_DECL_HEARDER(InputAPIManager);
		};

	}
}
#endif