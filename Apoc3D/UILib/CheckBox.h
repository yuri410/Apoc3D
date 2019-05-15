#pragma once
#ifndef APOC3D_CHECKBOX_H
#define APOC3D_CHECKBOX_H

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
		struct CheckboxVisualSettings
		{
			Font* FontRef = nullptr;

			ColorValue TextColor = CV_Black;
			OptionalSetting<ColorValue> TextColorDisabled;

			UIGraphicSimple NormalGraphic;
			OptionalSetting<UIGraphicSimple> HoverGraphic;
			OptionalSetting<UIGraphicSimple> DownGraphic;
			OptionalSetting<UIGraphicSimple> DisabledGraphic;

			UIGraphicSimple TickGraphic;
			OptionalSetting<UIGraphicSimple> DisabledTickGraphic;
			
			OptionalSetting<ControlBounds> Margin;
			OptionalSetting<int> TextSpacing;
		};

		class APAPI CheckBox : public Control
		{
			RTTI_DERIVED(CheckBox, Control);
		public:
			typedef EventDelegate<CheckBox*> CheckBoxEvent;

			CheckBox(const CheckboxVisualSettings& settings, const Point& position, const String& text, bool checked);
			CheckBox(const CheckboxVisualSettings& settings, const Point& position, const String& text, const Point& sz, bool checked);

			CheckBox(const StyleSkin* skin, const Point& position, const String& text, bool checked);
			CheckBox(const StyleSkin* skin, const Point& position, const String& text, const Point& sz, bool checked);
			virtual ~CheckBox();

			virtual void Update(const AppTime* time) override;
			virtual void Draw(Sprite* sprite) override;

			void Toggle();

			void SetFont(Font* fontRef);
			void SetText(const String& text);

			void SetSize(int32 w, int32 h) { SetSize(Point(w, h)); }
			void SetSize(const Point& sz);
			void SetSizeX(int32 v);
			void SetSizeY(int32 v);

			bool isMouseHover() const { return m_mouseHover; }

			TextRenderSettings TextSettings;

			bool Checked = false;
			bool CanUncheck = true;
			bool AutosizeX = false;
			bool AutosizeY = false;

			UIGraphicSimple NormalGraphic;
			UIGraphicSimple HoverGraphic;
			UIGraphicSimple DownGraphic;
			UIGraphicSimple DisabledGraphic;

			UIGraphicSimple TickGraphic;
			UIGraphicSimple DisabledTickGraphic;

			ControlBounds Margin;

			int32 TextSpacing = 0;
			
			CheckBoxEvent eventToggled;

			CheckBoxEvent eventPress;
			CheckBoxEvent eventRelease;

			static CheckBoxEvent eventAnyPress;
			static CheckBoxEvent eventAnyRelease;
		private:
			void Initialize(const StyleSkin* skin);
			void Initialize(const CheckboxVisualSettings& settings);

			void UpdateSize();

			void OnPress();
			void OnRelease();
			void OnMouseHover();
			void OnMouseOut();

			bool m_mouseDown = false;
			bool m_mouseHover = false;

			String m_text;
		};

		class APAPI CheckboxGroup : public Control
		{
			RTTI_DERIVED(CheckboxGroup, Control);
		public:
			CheckboxGroup(const List<CheckBox*>& checkbox);
			virtual ~CheckboxGroup();

			virtual void Update(const AppTime* time) override;
			virtual void Draw(Sprite* sprite) override;

			int getSelectedIndex() const { return m_selectedIndex; }
			CheckBox* getSubCheckbox(int i) { return m_checkbox[i]; }
			int getSubCheckboxCount() const { return m_checkbox.getCount(); }

			UIEventHandler eventSelectionChanged;

		private:
			void Initialize(const StyleSkin* skin);
			void Checkbox_Press(CheckBox* ctrl);

			List<CheckBox*> m_checkbox;
			int m_selectedIndex = -1;
		};
	}
}
#endif