#pragma once
#ifndef APOC3D_COMBOBOX_H
#define APOC3D_COMBOBOX_H

/**
 * -----------------------------------------------------------------------------
 * This source file is part of Apoc3D Engine
 * 
 * Copyright (c) 2009+ Tao Xin
 * 
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 * 
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 * 
 * You should have received a copy of the GNU General Public License
 * along with this program.  if not, write to the Free Software Foundation, 
 * Inc., 59 Temple Place - Suite 330, Boston, MA 02111-1307, USA, or go to
 * http://www.gnu.org/copyleft/gpl.txt.
 * 
 * -----------------------------------------------------------------------------
 */


#include "Control.h"

using namespace Apoc3D::Graphics::RenderSystem;

namespace Apoc3D
{
	namespace UI
	{
		class APAPI ComboBox : public Control
		{
			RTTI_DERIVED(ComboBox, Control);
		public:
			ComboBox(const Point& position, int width, const List<String>& items);
			virtual ~ComboBox();

			virtual void Initialize(RenderDevice* device);
			virtual void Update(const GameTime* time);
			virtual void Draw(Sprite* sprite);
			virtual void DrawOverlay(Sprite* sprite);
			virtual bool IsOverriding();

			List<String>& getItems() const;

			bool getOpened() const;
			bool getLocked() const;
			void setLocked(bool value) const;
			const String& getText() const;
			void setText(const String& value);
			int getSelectedIndex() const;
			void setSelectedIndex(int v) const;
			void SetSelectedByName(const String& name);

			UIEventHandler eventSelectionChanged;
		private:
			TextBox* m_textbox;
			Button* m_button;
			ListBox* m_listBox;
			List<String> m_items;

			bool m_justOpened;

			void ListBox_SelectionChanged(Control* ctrl);
			void ListBox_OnPress(Control* ctrl);
			void Button_OnPress(Control* ctrl);
			void Open();
			void Close();


		};
	}
}
#endif