#pragma once
#ifndef APOC3D_NULLINPUT_H
#define APOC3D_NULLINPUT_H

#include "Mouse.h"
#include "Keyboard.h"
#include "InputAPI.h"

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

namespace Apoc3D
{
	namespace Input
	{
		class NullInputFactory : public InputAPIFactory
		{
		public:
			NullInputFactory() : InputAPIFactory(GetDescription()) { }
			~NullInputFactory() { }

			void Initialize(RenderWindow* window) override { }
			Mouse* CreateMouse() override;
			Keyboard* CreateKeyboard() override;

		private:
			static APIDescription GetDescription();
		};

		class NullMouse : public Mouse
		{
		public:
			NullMouse() { }
			~NullMouse() { }

			void Update(const GameTime* time) override { }
		};

		class NullKeyboard : public Keyboard
		{
		public:
			NullKeyboard() { }
			~NullKeyboard() { }

			void Update(const GameTime* time) override { }

		};


	}
}
#endif