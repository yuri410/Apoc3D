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
#ifndef CHECKBOX_H
#define CHECKBOX_H

#include "Control.h"
#include "Collections/FastList.h"

using namespace Apoc3D::Collections;
using namespace Apoc3D::Graphics::RenderSystem;

namespace Apoc3D
{
	namespace UI
	{
		class APAPI CheckBox : public Control
		{
		public:

			CheckBox(const Point& position, const String& text, bool checked);
			~CheckBox();

			virtual void Initialize(RenderDevice* device);
			virtual void Update(const GameTime* const time);
			virtual void Draw(Sprite* sprite);

			void Toggle();

			bool getValue() const { return m_check; }
			void setValue(bool v) { m_check = v; }

			bool getCanUncheck() const { return m_canUncheck; }
			void setCanUncheck(bool v) { m_canUncheck = v; }
		private:
			
			bool m_mouseDown;
			bool m_mouseOver;

			bool m_check;
			bool m_canUncheck;
			Point m_textOffset;

			void UpdateEvents();
		};

		class CheckboxGroup : public Control
		{
		public:
			CheckboxGroup(const FastList<CheckBox*>& checkbox);
			~CheckboxGroup();

			virtual void Initialize(RenderDevice* device);
			virtual void Update(const GameTime* const time);
			virtual void Draw(Sprite* sprite);

			int getSelectedIndex() const { return m_selectedIndex; }
			CheckBox* getSubCheckbox(int i) { return m_checkbox[i]; }
			int getSubCheckboxCount() const { return m_checkbox.getCount(); }

			UIEventHandler& eventSelectionChanged() { return m_selectionChanged; }
		private:
			FastList<CheckBox*> m_checkbox;
			UIEventHandler m_eSelectionChanged;
			int m_selectedIndex;

			void Checkbox_Press(Control* ctrl);


		};
	}
}
#endif