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
#include "Console.h"
#include "Form.h"
#include "Label.h"
#include "Button.h"
#include "PictureBox.h"

namespace Apoc3D
{
	namespace UI
	{
		Console::Console(const Point& position, const Point& size)
		{
			m_form = new Form(FBS_Sizable, L"Console");
			m_form->Position = position;
			m_form->Size = size;
			
			m_inputText = new TextBox(Point(5, size.Y - 40), size.X, L"");

			m_submit = new Button(Point(size.X - 100, size.Y - 40), L"Submit");

			m_pictureBox = new PictureBox(Point(5,5), 1);
			m_pictureBox->SetSkin();
		}

		Console::~Console()
		{

		}

		void Console::Update(const GameTime* const time)
		{

		}
	}
}