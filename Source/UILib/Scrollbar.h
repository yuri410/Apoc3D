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

#ifndef SCROLLBAR_H
#define SCROLLBAR_H

#include "UICommon.h"
#include "Control.h"

namespace Apoc3D
{
	namespace UI
	{
		class HScrollbar : public Control
		{
		private:
			Button* m_btLeft;
			Button* m_btRight;

			Apoc3D::Math::Rectangle m_backArea;
			int m_value;
			int m_max;
			int m_step;
			bool m_isScrolling;
			bool m_inverted;

			Apoc3D::Math::Rectangle m_cursorArea;
			Apoc3D::Math::Rectangle m_cursorMidDest;

			Point m_cursorPos;
			Point m_cursorOffset;

			UIEventHandler m_eChangeValue;

			void btLeft_OnPress(Control* ctrl);
			void btRight_OnPress(Control* ctrl);

			void DrawBackground(Sprite* sprite);
			void DrawCursor(Sprite* sprite);



			void UpdateScrolling();
		public:
			int getStep() const { return m_step; }
			void setStep(int step) { m_step = step; }

			bool getIsInverted() const { return m_inverted; }
			void setIsInverted(bool val) { m_inverted = val; }

			int getValue() const { return m_value; }
			void setValue(int v) { m_value = v; if (m_value<0)m_value = 0; else if (m_value>m_max) m_value = m_max; }

			int getMax() const { return m_max; }
			void setMax(int v) { m_max = v; if (m_max<0)m_max =0; if (m_value>m_max) m_value = m_max; }

			UIEventHandler& eventValueChanged() const { return m_eChangeValue; }

			HScrollbar(const Point& position, int width);
			~HScrollbar();

			virtual void Initialize(RenderDevice* device);

			virtual void Update(const GameTime* const time);
			virtual void Draw(Sprite* sprite);

		};
		class VScrollBar : public Control
		{
		private:
			Button* m_btUp;
			Button* m_btDown;

		public:

		};
		class ScrollBar : public Control
		{
		public:
			enum ScrollBarType
			{
				SCRBAR_Horizontal,
				SCRBAR_Vertical
			};

		private:
			ScrollBarType m_type;

		};
	}
}

#endif