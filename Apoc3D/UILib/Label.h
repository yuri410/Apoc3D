#pragma once
#ifndef APOC3D_LABEL_H
#define APOC3D_LABEL_H

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

#include "KeyboardHelper.h"

namespace Apoc3D
{
	namespace UI
	{
		class APAPI Label : public Control
		{
		public:
			enum Align
			{
				ALIGN_Left,
				ALIGN_Center,
				ALIGN_Right
			};
		private:
			Point m_textOffset;
			Point m_drawPos;
			Apoc3D::Math::Rectangle m_backgroundRect;
			int m_width;

			bool m_mouseOver;
			bool m_mouseDown;
			Align m_alignment;

			List<String> m_lines;

			void UpdateText();
			void UpdateEvents();

		public:
			Label(const Point& position, const String& text, int width, Align alignment = ALIGN_Left);
			~Label();

			virtual void Initialize(RenderDevice* device);
			virtual void Draw(Sprite* sprite);
			virtual void Update(const GameTime* const time);
		};

		class APAPI TextBox : public Control
		{
		public:
			enum ScrollBarType
			{
				SBT_None,
				SBT_Horizontal,
				SBT_Vertical,
				SBT_Both
			};

			TextBox(const Point& position, int width);
			TextBox(const Point& position, int width, const String& text);
			TextBox(const Point& position, int width, int height, const String& text);
			~TextBox();
			virtual void Initialize(RenderDevice* device);
			virtual void Update(const GameTime* const time);
			virtual void Draw(Sprite* sprite);

			void setText(const String& text);
			void setWidth(int w) { Size.X = w; InitDstRect(); }
			bool getHasFocus() const { return m_hasFocus; }
			void setHasFocus(bool value) { m_hasFocus = value; }
			bool getLocked() const { return m_locked; }
			void setLocked(bool value) { m_locked = value; }
			ScrollBarType getScrollbarType() const { return m_scrollBar; }
			void setScrollbarType(ScrollBarType barType) { m_scrollBar = barType; }

			UIEventHandler& eventReturnPressed() { return m_eEnterPressed; }
			UIEventHandler& eventContentChanged() { return m_eContentChanged; }
		private:
			Point m_textOffset;
			KeyboardHelper m_keyboard;

			Point m_curorLocation;
			Point m_previousLocation;

			Point m_cursorOffset;
			Point m_scrollOffset;

			bool m_hasFocus;

			bool m_multiline;
			List<String> m_lines;
			Point m_lineOffset;
			int m_visibleLines;

			bool m_locked;

			Apoc3D::Math::Rectangle m_destRect[9];
			Apoc3D::Math::Rectangle m_dRect;
			Apoc3D::Math::Rectangle m_sRect;

			ScrollBar* m_vscrollBar;
			ScrollBar* m_hscrollBar;
			ScrollBarType m_scrollBar;

			bool m_cursorVisible;
			float m_timer;
			bool m_timerStarted;
			Apoc3D::Math::Rectangle m_focusArea;
			
			UIEventHandler m_eEnterPressed;
			UIEventHandler m_eContentChanged;

			void InitScrollbars(RenderDevice* device);
			void InitDstRect();

			void Add(const String& text);
			void Keyboard_OnPress(KeyboardKeyCode code, KeyboardEventsArgs e);
			void Keyboard_OnPaste(String value);
			void vScrollbar_OnChangeValue(Control* ctrl);
			void hScrollbar_OnChangeValue(Control* ctrl);
			//void UpdateScrollPosition();
			void UpdateScrollbars(const GameTime* const time);

			void CheckFocus();

			void UpdateScrolling();
			void _DrawText(Sprite* sprite);
			void DrawMonoline(Sprite* sprite);
			void DrawMultiline(Sprite* sprite);
		};
	}
}
#endif