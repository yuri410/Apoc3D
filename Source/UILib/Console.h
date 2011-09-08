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
#ifndef CONSOLE_H
#define CONSOLE_H

#include "UICommon.h"
#include "Core/Logging.h"

using namespace Apoc3D::Collections;
using namespace Apoc3D::Core;
using namespace Apoc3D::Math;
using namespace Apoc3D::Graphics;

namespace Apoc3D
{
	namespace UI
	{
		typedef fastdelegate::FastDelegate2<String, List<String>*> ConsoleCommandHandler;

		class APAPI Console
		{
		public:
			Console(RenderDevice* device, StyleSkin* skin, const Point& position, const Point& size);
			~Console();

			ConsoleCommandHandler& eventCommandSubmited() { return m_eCommandSubmited; }
			void setPosition(const Point& pt);
			const Point& getSize() const;

			void Minimize();
			void Restore();
			void Maximize();
			void Update(const GameTime* const time);
		private:
			void TextBox_ReturnPressed(Control* ctrl);
			void Submit_Pressed(Control* ctrl);

			void PictureBox_Draw(Sprite* sprite, Apoc3D::Math::Rectangle* dstRect);
			void Log_New(LogEntry e);

			Form* m_form;
			TextBox* m_inputText;
			Button* m_submit;
			PictureBox* m_pictureBox;
			StyleSkin* m_skin;
			ScrollBar* m_scrollBar;
			std::list<LogEntry> m_logs;

			ConsoleCommandHandler m_eCommandSubmited;


		};
	}
}

#endif