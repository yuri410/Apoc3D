#pragma once
#ifndef DIP_H
#define DIP_H

/*
-----------------------------------------------------------------------------
This source file is part of Apoc3D Engine

Copyright (c) 2009+ Tao Xin

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

#include "DIPCommon.h"

namespace dip
{
    class DIP1 : public Apoc3DEx::Game
	{
	public:
		DIP1(RenderWindow* wnd);
		~DIP1();

		virtual void Initialize();
		virtual void Finalize();

		virtual void Load();
		virtual void Unload();
		virtual void Update(const GameTime* time);
		virtual void Draw(const GameTime* time);

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
