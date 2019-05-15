#pragma once
#ifndef DIP_H
#define DIP_H

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

#include "DIPCommon.h"

namespace dip
{
    class DIP1 : public Application
	{
	public:
		DIP1(RenderWindow* wnd);
		~DIP1();

		virtual void Initialize();
		virtual void Finalize();

		virtual void Load();
		virtual void Unload();
		virtual void Update(const AppTime* time);
		virtual void Draw(const AppTime* time);

		virtual void OnFrameStart();
		virtual void OnFrameEnd();
		
	private:
		void MenuItem_OpenDemo(MenuItem* mi);

		void ShowAbout(MenuItem* ctrl);



		List<SubDemo*> m_subdemos;
		

		Form* m_aboutDlg;

		MenuBar* m_mainMenu;

		Sprite* m_sprite;
    };
}
#endif
