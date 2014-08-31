#pragma once
#ifndef APOC3D_BUTTON_H
#define APOC3D_BUTTON_H

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
#include "apoc3d/Math/ColorValue.h"

using namespace Apoc3D::Graphics::RenderSystem;

namespace Apoc3D
{
	namespace UI
	{
		class APAPI Button : public Control
		{
			RTTI_DERIVED(Button, Control);
		public:
			Button(const Point& position, const String& text)
				: Control(position, text), m_mouseOver(false), m_mouseDown(false),
				m_modColor(CV_White),m_modMouseOverColor(CV_White),m_modMouseDownColor(CV_White),m_modDisabledColor(CV_PackColor(0x7f,0x7f,0x7f,0x7f)),
				m_rotation(0), m_hasTextColorValue(false)
			{
				Size.X = Size.Y = 0;
			}
			Button(const Point& position, int width, const String& text)
				: Control(position, text, Point(width, 0)), m_mouseOver(false), m_mouseDown(false),
				m_modColor(CV_White),m_modMouseOverColor(CV_White),m_modMouseDownColor(CV_White),m_modDisabledColor(CV_PackColor(0x7f,0x7f,0x7f,0x7f)),
				m_rotation(0), m_hasTextColorValue(false)
			{
			}
			Button(const Point& position, const Point& size, const String& text)
				: Control(position, text, size), m_mouseOver(false), m_mouseDown(false),
				m_modColor(CV_White),m_modMouseOverColor(CV_White),m_modMouseDownColor(CV_White),m_modDisabledColor(CV_PackColor(0x7f,0x7f,0x7f,0x7f)),
				m_rotation(0), m_hasTextColorValue(false)
			{
			}
			
			virtual ~Button() { }

			virtual void Initialize(RenderDevice* device);

			virtual void Draw(Sprite* sprite);
			virtual void Update(const GameTime* time);


			void SetTextColorOverride(ColorValue cv);

			/** 
			 *  Gets the modulation color for custom button in normal state.
			 */
			ColorValue getCustomModColor() const { return m_modColor; }
			void setCustomModColor(ColorValue clr) { m_modColor = clr; }

			/**
			 *  Gets the modulation color for custom button when mouse hover.
			 */
			ColorValue getCustomModColorMouseOver() const { return m_modMouseOverColor; }
			void setCustomModColorMouseOver(ColorValue clr) { m_modMouseOverColor = clr; }

			/** 
			 *  Gets the modulation color for custom button when mouse down.
			 */
			ColorValue getCustomModColorMouseDown() const { return m_modMouseDownColor; }
			void setCustomModColorMouseDown(ColorValue clr) { m_modMouseDownColor = clr; }
			
			ColorValue getCustomDisabledColor() const { return m_modDisabledColor; }
			void setCustomDisabledColor(ColorValue clr) { m_modDisabledColor = clr; }


			void setRotation(float rot) { m_rotation = rot; }
			float getRotation() const { return m_rotation; }

			bool isMouseHover() const { return m_mouseOver; }

			UIGraphic DisabledTexture;
			UIGraphic NormalTexture;
			UIGraphic MouseOverTexture;
			UIGraphic MouseDownTexture;
			
			UIGraphic OverlayIcon;

		protected:

			bool m_mouseOver;
			bool m_mouseDown;

		private:
			Apoc3D::Math::Rectangle m_btnDestRect[9];

			ColorValue m_modColor;
			ColorValue m_modMouseDownColor;
			ColorValue m_modMouseOverColor;
			ColorValue m_modDisabledColor;

			Point m_textPos;
			Point m_textSize;

			float m_rotation;

			bool m_hasTextColorValue;
			ColorValue m_textColorOverride;

			void DrawDefaultButton(Sprite* spriteBatch);
			void DrawCustomButton(Sprite* spriteBatch);
			void UpdateEvents();
		};

		class APAPI ButtonRow : public Control
		{
			RTTI_DERIVED(ButtonRow, Control);
		public:
			typedef EventDelegate1<int> SelectedChangedHandler;

			ButtonRow(const Point& position, int32 width, const List<String>& titles);
			ButtonRow(const Point& position, int32 width, int32 colCount, const List<String>& titles);
			~ButtonRow();

			virtual void Initialize(RenderDevice* device);

			virtual void Update(const GameTime* time);
			virtual void Draw(Sprite* sprite);

			int getSelectedIndex() const { return m_selectedIndex; }

			SelectedChangedHandler eventSelectedChanging;

		private:

			enum VerticalBorderStyle
			{
				VBS_Both,
				VBS_Top,
				VBS_Bottom,
				VBS_None
			};
			enum Region9Flags
			{
				R9_None = 0,
				R9_TopLeft = 1 << 0,
				R9_TopCenter = 1 << 1,
				R9_TopRight = 1 << 2,
				R9_MiddleLeft = 1 << 3,
				R9_MiddleCenter = 1 << 4,
				R9_MiddleRight = 1 << 5,
				R9_BottomLeft = 1 << 6,
				R9_BottomCenter = 1 << 7,
				R9_BottomRight = 1 << 8,

				R9Mix_TopBar = R9_TopLeft | R9_TopCenter | R9_TopRight,
				R9Mix_MiddleBar = R9_MiddleLeft | R9_MiddleCenter | R9_MiddleRight,
				R9Mix_BottomBar = R9_BottomLeft | R9_BottomCenter | R9_BottomRight,
				R9Mix_All = R9Mix_TopBar | R9Mix_MiddleBar | R9Mix_BottomBar
			};

			void Init(int32 width, const List<String>& titles);

			void UpdatePositions();

			void DrawButton(Sprite* sprite, int32 idx, int32 colType, VerticalBorderStyle rowType, const Apoc3D::Math::Rectangle* srcRect);

			void DrawRegion9Subbox(Sprite* sprite, const Apoc3D::Math::Rectangle& dstRect, ColorValue cv, Texture* texture, const Apoc3D::Math::Rectangle* srcRects, uint32 subRegionFlags);

			int32 m_selectedIndex;

			bool m_mouseDown;

			int32 m_countPerRow;
			int32 m_numRows;
			int32 m_count;
			int32 m_rowHeight;
			String* m_titles;
			Point* m_textPos;
			Point* m_textSize;
			Apoc3D::Math::Rectangle* m_buttonDstRect;

			int32 m_hoverIndex;

		};


		class APAPI ButtonGroup : public Control
		{
			RTTI_DERIVED(ButtonGroup, Control);
		public:

			ButtonGroup(const List<Button*> buttons);
			ButtonGroup(const List<Button*> buttons, int selected);

			virtual void Initialize(RenderDevice* device);
			virtual void Update(const GameTime* time);

			virtual void Draw(Sprite* sprite);

			int getSelectedIndex() const { return m_selectedIndex; }

			Button* getSelectedButton() const { return m_button[m_selectedIndex]; }

			const String& getSelectedText() const;
			void setSelectedText(const String& text);

			UIEventHandler eventSelectionChanged;

		private:
			List<Button*> m_button;
			int m_selectedIndex;
			
			void Button_OnRelease(Control* sender);
		};

		class APAPI RadioButton : public Control
		{
			RTTI_DERIVED(RadioButton, Control);
		public:
			RadioButton(const Point& position, const String& text, bool checked);
			~RadioButton() { }

			virtual void Initialize(RenderDevice* device);
			virtual void Update(const GameTime* time);
			virtual void Draw(Sprite* sprite);

			void Toggle();

			bool isChecked() const { return m_checked; }
			bool canUncheck() const { return m_canUncheck; }

		private:
			Point m_textOffset;

			bool m_mouseDown;
			bool m_mouseOver;
			bool m_checked;
			bool m_canUncheck;

			void UpdateEvents();
		};

		//class APAPI RadioButtonGroup : public Control
		//{

		//};
	}
}

#endif