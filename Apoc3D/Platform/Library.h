#pragma once
#ifndef APOC3D_LIBRARY_H
#define APOC3D_LIBRARY_H

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

#include "apoc3d/ApocCommon.h"

using namespace Apoc3D;

namespace Apoc3D
{
	namespace Platform
	{
		/** 
		 *  Represents a dynamic library.
		 */
		class APAPI Library
		{
		private:
			String m_name;

			byte m_handle[8];
		public:
			Library(String name)
				: m_name(name)
			{}

			const String &getName() const { return m_name; }
			/** 
			 * 
			 */
			void* getSymbolAddress(const String &name) const;

			/**
			 *  Dynamically loads the library into memory.
			 *  Exported functions on the library can be called once it is loaded.
			 */
			void Load();
			void Unload();


		};
	}
}


#endif