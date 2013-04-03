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


#include "UICommon.h"

#include "Control.h"

namespace Apoc3D
{
	namespace UI
	{
		class APAPI HScrollbar : public Control
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
			void setWidth(int w);
			void setPosition(const Point& pos);

			int getStep() const { return m_step; }
			void setStep(int step) { m_step = step; }

			bool getIsInverted() const { return m_inverted; }
			void setIsInverted(bool val) { m_inverted = val; }

			int getValue() const { return m_value; }
			void setValue(int v) { m_value = v; if (m_value<0)m_value = 0; else if (m_value>m_max) m_value = m_max; }

			int getMax() const { return m_max; }
			void setMax(int v) { m_max = v; if (m_max<0)m_max =0; if (m_value>m_max) m_value = m_max; }

			UIEventHandler& eventValueChanged() { return m_eChangeValue; }

			HScrollbar(const Point& position, int width);
			~HScrollbar();

			virtual void Initialize(RenderDevice* device);

			virtual void Update(const GameTime* const time);
			virtual void Draw(Sprite* sprite);

		};
		class APAPI VScrollBar : public Control
		{
		private:
			Button* m_btUp;
			Button* m_btDown;

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

			void btUp_OnPress(Control* ctrl);
			void btDown_OnPress(Control* ctrl);

			void DrawBackground(Sprite* sprite);
			void DrawCursor(Sprite* sprite);



			void UpdateScrolling();
		public:
			void setHeight(int w);
			void setPosition(const Point& pos);

			int getStep() const { return m_step; }
			void setStep(int step) { m_step = step; }

			bool getIsInverted() const { return m_inverted; }
			void setIsInverted(bool val) { m_inverted = val; }

			int getValue() const { return m_value; }
			void setValue(int v) { m_value = v; if (m_value<0)m_value = 0; else if (m_value>m_max) m_value = m_max; }

			int getMax() const { return m_max; }
			void setMax(int v) { m_max = v; if (m_max<0)m_max =0; if (m_value>m_max) m_value = m_max; }

			UIEventHandler& eventValueChanged() { return m_eChangeValue; }

			VScrollBar(const Point& position, int width);
			~VScrollBar();

			virtual void Initialize(RenderDevice* device);

			virtual void Update(const GameTime* const time);
			virtual void Draw(Sprite* sprite);

		};
		class APAPI ScrollBar : public Control
		{
		public:
			enum ScrollBarType
			{
				SCRBAR_Horizontal,
				SCRBAR_Vertical
			};

		private:
			ScrollBarType m_type;
			HScrollbar* m_hsbar;
			VScrollBar* m_vsbar;

		public:

			void setPosition(const Point& pos)
			{
				if (m_type == SCRBAR_Horizontal)
				{
					m_hsbar->Position = pos;
					m_hsbar->setPosition(pos);
				}
				else
				{
					m_vsbar->Position = pos;
					m_vsbar->setPosition(pos);
				}
			}
			const Point& getPosition() const 
			{
				if (m_type == SCRBAR_Horizontal)
					return m_hsbar->Position;
				return m_vsbar->Position;
			}

			int getValue() const
			{
				if (m_type == SCRBAR_Horizontal)
					return m_hsbar->getValue();
				return m_vsbar->getValue();
			}
			void setValue(int val)
			{
				if (m_type == SCRBAR_Horizontal)
					m_hsbar->setValue(val);
				else
					m_vsbar->setValue(val);
			}

			int getMax() const
			{
				if (m_type == SCRBAR_Horizontal)
					return m_hsbar->getMax();
				return m_vsbar->getMax();
			}
			void setMax(int v)
			{
				if (m_type == SCRBAR_Horizontal)
					m_hsbar->setMax(v);
				else
					m_vsbar->setMax(v);
			}

			int getStep() const
			{
				if (m_type == SCRBAR_Horizontal)
					return m_hsbar->getStep();
				return m_vsbar->getStep();
			}
			void setStep(int s)
			{
				if (m_type == SCRBAR_Horizontal)
					m_hsbar->setStep(s);
				else
					m_vsbar->setStep(s);
			}

			UIEventHandler& eventValueChanged()
			{
				if (m_type == SCRBAR_Horizontal)
					return m_hsbar->eventValueChanged();
				return m_vsbar->eventValueChanged();
			}

			bool getIsInverted() const
			{
				if (m_type == SCRBAR_Horizontal)
					return m_hsbar->getIsInverted();
				return m_vsbar->getIsInverted();
			}
			void setIsInverted(bool v)
			{
				if (m_type == SCRBAR_Horizontal)
					m_hsbar->setIsInverted(v);
				else
					m_vsbar->setIsInverted(v);
			}

			void setWidth(int v)
			{
				if (m_hsbar)
					m_hsbar->setWidth(v);
			}
			int getWidth() const
			{
				if (m_hsbar)
					return m_hsbar->Size.X;
				return 0;
			}
			void setHeight(int v)
			{
				if (m_vsbar)
					m_vsbar->setHeight(v);
			}
			int getHeight() const 
			{
				if (m_vsbar)
					return m_vsbar->Size.Y;
				return 0;
			}

			int getBarWidth() const
			{
				if (m_hsbar)
					return m_hsbar->Size.Y;
				if (m_vsbar)
					return m_vsbar->Size.X;
				return 0;
			}

			ScrollBar(const Point& position, ScrollBarType type, int size)
				: m_hsbar(0), m_vsbar(0), m_type(type)
			{
				if (m_type == SCRBAR_Horizontal)
				{
					m_hsbar = new HScrollbar(position, size);
				}
				else
				{
					m_vsbar = new VScrollBar(position, size);
				}
			}
			~ScrollBar()
			{
				if (m_hsbar)
					delete m_hsbar;
				if (m_vsbar)
					delete m_vsbar;
			}

			virtual void Initialize(RenderDevice* device)
			{
				if (m_hsbar)
				{
					m_hsbar->SetSkin(m_skin);
					m_hsbar->setOwner(getOwner());
					m_hsbar->Initialize(device);
				}
				if (m_vsbar)
				{
					m_vsbar->SetSkin(m_skin);
					m_vsbar->setOwner(getOwner());
					m_vsbar->Initialize(device);
				}
				
			}
			virtual void Update(const GameTime* const time)
			{
				if (Visible)
				{
					if (m_hsbar)
					{
						m_hsbar->setOwner(getOwner());
						m_hsbar->Update(time);
					}
					if (m_vsbar)
					{
						m_vsbar->setOwner(getOwner());
						m_vsbar->Update(time);
					}
				}
			}
			virtual void Draw(Sprite* sprite)
			{
				if (Visible)
				{
					if (m_hsbar)
						m_hsbar->Draw(sprite);
					if (m_vsbar)
						m_vsbar->Draw(sprite);
				}
			}

		};
	}
}

#endif