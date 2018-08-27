/* -----------------------------------------------------------------------
 * This source file is part of Apoc3D Framework
 * 
 * Copyright (c) 2011-2018 Tao Xin
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

#include "NullInput.h"

namespace Apoc3D
{
	namespace Input
	{
		Mouse* NullInputFactory::CreateMouse()
		{
			return new NullMouse();
		}
		Keyboard* NullInputFactory::CreateKeyboard()
		{
			return new NullKeyboard();
		}

		APIDescription NullInputFactory::GetDescription()
		{
			APIDescription desc;
			desc.Name = L"Null Input";
			desc.SupportedPlatforms.Add(PlatformAPISupport{ 100, L"WINDOWS" });
			desc.SupportedPlatforms.Add(PlatformAPISupport{ 100, L"LINUX" });
			return desc;
		}
	}
}