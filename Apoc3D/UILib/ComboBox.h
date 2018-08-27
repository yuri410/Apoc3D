#pragma once
#ifndef APOC3D_COMBOBOX_H
#define APOC3D_COMBOBOX_H

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

#include "Control.h"

using namespace Apoc3D::Graphics::RenderSystem;

namespace Apoc3D
{
	namespace UI
	{
		struct ComboBoxVisualSettings
		{
			TextBoxVisualSettings ContentTextBox;
			ButtonVisualSettings DropdownButton;
			ListBoxVisualSettings DropdownList;

			OptionalSetting<Point> DropdownButtonOffset;
		};

		class APAPI ComboBox : public Control
		{
			RTTI_DERIVED(ComboBox, Control);
		public:
			typedef EventDelegate<ComboBox*> ComboBoxEvent;

			ComboBox(const ComboBoxVisualSettings& settings, const Point& position, int width, const List<String>& items, int32 listItemCount = 8);
			ComboBox(const StyleSkin* skin, const Point& position, int width, const List<String>& items, int32 listItemCount = 8);
			virtual ~ComboBox();

			virtual void Update(const GameTime* time) override;
			virtual void Draw(Sprite* sprite) override;
			virtual void DrawOverlay(Sprite* sprite) override;
			virtual bool IsOverriding() override;
			virtual Apoc3D::Math::Rectangle ComboBox::GetOverridingAbsoluteArea() override;

			bool isMouseHover() const;
			Apoc3D::Math::Rectangle getMouseHoverArea() const;

			List<String>& getItems() const;

			bool isOpened() const;
			
			bool getLocked() const;
			void setLocked(bool value) const;
			//const String& getText() const;
			//void setText(const String& value);
			int getSelectedIndex() const;
			void setSelectedIndex(int v) const;

			void SetSelectionByName(const String& n);

			String* getSelectedItem() const;

			void setWidth(int32 w);
			

			ComboBoxEvent eventSelectionChanged;

			Point DropdownButtonOffset;
		private:
			void Initialize(const StyleSkin* skin, int32 listItemCount);
			void Initialize(const ComboBoxVisualSettings& settings, int32 listItemCount);
			void PostInit();
			
			Point CalculateDropButtonPos(TextBox* ctb, Button* btn, int32 btnWidth);

			void Open();
			void Close();

			void ListBox_SelectionChanged(ListBox* ctrl);
			void ListBox_Select(ListBox* ctrl);
			void Button_OnPress(Button* ctrl);
			
			TextBox* m_textbox = nullptr;
			Button* m_button = nullptr;
			ListBox* m_listBox = nullptr;
			List<String> m_items;

			
			bool m_justOpened = false;

		};
	}
}
#endif