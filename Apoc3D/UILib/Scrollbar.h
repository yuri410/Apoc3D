#pragma once
#ifndef APOC3D_SCROLLBAR_H
#define APOC3D_SCROLLBAR_H

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

namespace Apoc3D
{
	namespace UI
	{
		struct ScrollBarVisualSettings
		{
			bool HasBackgroundGraphic = false;
			bool HasHandleGraphic = false;
			bool HasDisabledBackgroundGraphic = false;
			bool HasDisabledHandleGraphic = false;
			bool HasBorderPadding = false;
			bool HasHandlePadding = false;

			UIGraphic BackgroundGraphic;
			UIGraphic HandleGraphic;
			UIGraphic DisabledBackgroundGraphic;
			UIGraphic DisabledHandleGraphic;

			ButtonVisualSettings DecrButton;
			ButtonVisualSettings IncrButton;

			ControlBounds BorderPadding;
			ControlBounds HandlePadding;

		};

		class APAPI ScrollBar : public Control
		{
			RTTI_DERIVED(ScrollBar, Control);
		public:
			typedef EventDelegate1<ScrollBar*> ScrollBarEvent;

			enum ScrollBarType
			{
				SCRBAR_Horizontal,
				SCRBAR_Vertical
			};
			ScrollBar(const ScrollBarVisualSettings& settings, const Point& position, ScrollBarType type, int32 length);
			ScrollBar(const StyleSkin* skin, const Point& position, ScrollBarType type, int32 length);
			~ScrollBar();

			void Draw(Sprite* sprite);
			void Update(const GameTime* time);

			void UpdateButtonPosition();
			void Reset() { m_value = 0; }

			void ForceScroll(int32 chg);
			void SetValue(int32 val);
			
			void SetLength(int32 len);
			int32 GetLength() const;

			bool isMouseHover() const { return m_isMouseHovering; }
			const Apoc3D::Math::Rectangle& getMouseHoverArea() const { assert(m_isMouseHovering); return m_mouseHoverArea; }

			int32 getValue() const { return m_value; }

			Button* getDecrButton() const { return m_decrButton; }
			Button* getIncrButton() const { return m_incrButton; }

			int32 Maximum = 0;
			int32 Step = 1;

			UIGraphic BackgroundGraphic;
			UIGraphic HandleGraphic;
			UIGraphic DisabledBackgroundGraphic;
			UIGraphic DisabledHandleGraphic;

			ControlBounds BorderPadding;
			ControlBounds HandlePadding;

			bool IsInverted = false;

			ScrollBarEvent eventValueChanged;
			
		private:
			void PostInit(int32 length);
			void DecrButton_Pressed(Button* btn);
			void IncrButton_Pressed(Button* btn);

			int32 CalculateBarLength() const;
			int32 GetScrollableLength() const;
			Apoc3D::Math::Rectangle CalculateHandleArea() const;

			bool m_isMouseHovering = false;
			bool m_isDragging = false;
			ScrollBarType m_type;

			int32 m_value = 0;
			//int32 m_length;

			Button* m_decrButton;
			Button* m_incrButton;

			
			Apoc3D::Math::Rectangle m_mouseHoverArea;


			//int32 m_disabledAlpha;
		};

	
	}
}

#endif