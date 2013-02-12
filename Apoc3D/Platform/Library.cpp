/*
-----------------------------------------------------------------------------
This source file is part of Apoc3D

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
#include "Library.h"
#if APOC3D_PLATFORM == APOC3D_PLATFORM_WINDOWS
#	 include <windows.h>
#	 define LIB_HANDLE HMODULE
#    define LIB_LOAD( a ) LoadLibraryEx( a, NULL, 0 )
#    define LIB_GETSYM( a, b ) GetProcAddress( a, b )
#    define LIB_UNLOAD( a ) !FreeLibrary( a )
#elif APOC3D_PLATFORM == APOC3D_PLATFORM_LINUX
#	 include <CoreFoundation/CoreFoundation.h>
#    define LIB_HANDLE void*
#    define LIB_LOAD( a ) dlopen( a, RTLD_LAZY | RTLD_GLOBAL)
#    define LIB_GETSYM( a, b ) dlsym( a, b )
#    define LIB_UNLOAD( a ) dlclose( a )

#elif APOC3D_PLATFORM == APOC3D_PLATFORM_MAC
#    define LIB_HANDLE CFBundleRef
#    define LIB_LOAD( a ) mac_loadExeBundle( a )
#    define LIB_GETSYM( a, b ) mac_getBundleSym( a, b )
#    define LIB_UNLOAD( a ) mac_unloadExeBundle( a )
#endif

#include "apoc3d/Utility/StringUtils.h"

using namespace Apoc3D::Utility;

namespace Apoc3D
{
	namespace Platform
	{


		void Library::Load()
		{
			String name = m_name;
#if APOC3D_PLATFORM == APOC3D_PLATFORM_WINDOWS
			if (!StringUtils::EndsWidth(name, L".dll", true))
				name.append(L".dll");
#elif APOC3D_PLATFORM == APOC3D_PLATFORM_LINUX
			if (!StringUtils::EndsWidth(name, L".so", true))
				name.append(L".so");
#elif APOC3D_PLATFORM == APOC3D_PLATFORM_MAC
#endif
			LIB_HANDLE handle = LIB_LOAD(name.c_str());
			memcpy(&m_handle, &handle, sizeof(LIB_HANDLE));
		}

		void Library::Unload()
		{
			LIB_HANDLE handle;
			memcpy(&handle, &m_handle, sizeof(LIB_HANDLE));

			LIB_UNLOAD(handle);
		}

		void* Library::getSymbolAddress(const String &name) const
		{
			LIB_HANDLE handle;
			memcpy(&handle, &m_handle, sizeof(LIB_HANDLE));
			
			//char sname[64];
			//wcstombs(sname, name.c_str(), 64);
			std::string sname = StringUtils::toString(name);

			return (void*)LIB_GETSYM( handle, sname.c_str() );
		}
	}
}