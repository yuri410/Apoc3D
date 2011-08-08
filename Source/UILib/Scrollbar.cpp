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
#include "Scrollbar.h"
#include "FontManager.h"
#include "StyleSkin.h"
#include "Input/Mouse.h"
#include "Input/InputAPI.h"

namespace Apoc3D
{
	namespace UI
	{
		HScrollbar::HScrollbar(const Point& position, int width)
			: Control(position), m_value(0), m_max(0), m_step(1), m_isScrolling(false), m_inverted(false)
		{
			Size.X = width;
			Size.Y = 12;
		}
		HScrollbar::~HScrollbar()
		{

		}

		void HScrollbar::Initialize(RenderDevice* device)
		{

		}

		void HScrollbar::btLeft_OnPress(Control* ctrl)
		{

		}
		void HScrollbar::btRight_OnPress(Control* ctrl)
		{

		}

		void HScrollbar::Update(const GameTime* const time)
		{

		}
		void HScrollbar::UpdateScrolling()
		{

		}
		void HScrollbar::Draw(Sprite* sprite)
		{

		}
		void HScrollbar::DrawBackground(Sprite* sprite)
		{

		}

		void HScrollbar::DrawCursor(Sprite* sprite)
		{

		}
	}
}