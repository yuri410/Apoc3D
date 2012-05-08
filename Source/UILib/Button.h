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
#ifndef BUTTON_H
#define BUTTON_H

#include "Control.h"
#include "Collections/FastList.h"

using namespace Apoc3D::Collections;
using namespace Apoc3D::Graphics::RenderSystem;

namespace Apoc3D
{
	namespace UI
	{
		class APAPI Button : public Control
		{
		private:
			Apoc3D::Math::Rectangle m_btnDestRect[3];

			Texture* m_InvalidTexture;
			Texture* m_DisabledTexture;
			Texture* m_MouseDownTexture;
			Texture* m_MouseOverTexture;
			Texture* m_NormalTexture;
			ColorValue m_modColor;
			ColorValue m_modMouseDownColor;
			ColorValue m_modMouseOverColor;
			ColorValue m_modDisabledColor;
			
			Point m_textPos;
			Point m_textSize;

			PointF m_origin;
			float m_scale;


			void DrawDefaultButton(Sprite* spriteBatch);
			void DrawCustomButton(Sprite* spriteBatch);
			void UpdateEvents();
		protected:

			bool m_mouseOver;
			bool m_mouseDown;
		public:
			Button(const Point& position, const String& text)
				: Control(position, text), m_mouseOver(false), m_mouseDown(false),
				m_InvalidTexture(0), m_DisabledTexture(0), m_MouseDownTexture(0), m_MouseOverTexture(0), m_NormalTexture(0),
				m_modColor(CV_White),m_modMouseOverColor(CV_White),m_modMouseDownColor(CV_White),m_modDisabledColor(PACK_COLOR(0x7f,0x7f,0x7f,0x7f))
			{
				//BackColor = backColor;
				//ForeColor = foreColor;
			}
			Button(const Point& position, int width, const String& text)
				: Control(position, text, Point(width, 0)), m_mouseOver(false), m_mouseDown(false),
				m_InvalidTexture(0), m_DisabledTexture(0), m_MouseDownTexture(0), m_MouseOverTexture(0), m_NormalTexture(0),
				m_modColor(CV_White),m_modMouseOverColor(CV_White),m_modMouseDownColor(CV_White),m_modDisabledColor(PACK_COLOR(0x7f,0x7f,0x7f,0x7f))
			{
				//BackColor = backColor;
				//ForeColor = foreColor;
			}
			Button(const Point& position, const Point& size, const String& text)
				: Control(position, text, size), m_mouseOver(false), m_mouseDown(false),
				m_InvalidTexture(0), m_DisabledTexture(0), m_MouseDownTexture(0), m_MouseOverTexture(0), m_NormalTexture(0),
				m_modColor(CV_White),m_modMouseOverColor(CV_White),m_modMouseDownColor(CV_White),m_modDisabledColor(PACK_COLOR(0x7f,0x7f,0x7f,0x7f))
			{
				//BackColor = backColor;
				//ForeColor = foreColor;
			}
		
			/** Gets the modulation color for custom button in normal state.
			*/
			ColorValue getCustomModColor() const { return m_modColor; }
			void setCustomModColor(ColorValue clr) { m_modColor = clr; }

			/** Gets the modulation color for custom button when mouse hover.
			*/
			ColorValue getCustomModColorMouseOver() const { return m_modMouseOverColor; }
			void setCustomModColorMouseOver(ColorValue clr) { m_modMouseOverColor = clr; }

			/** Gets the modulation color for custom button when mouse down.
			*/
			ColorValue getCustomModColorMouseDown() const { return m_modMouseDownColor; }
			void setCustomModColorMouseDown(ColorValue clr) { m_modMouseDownColor = clr; }
			
			ColorValue getCustomDisabledColor() const { return m_modDisabledColor; }
			void setCustomDisabledColor(ColorValue clr) { m_modDisabledColor = clr; }


			Texture* getInvalidTexture() const { return m_InvalidTexture; }
			void setInvalidTexture(Texture* texture) { m_InvalidTexture = texture; }

			Texture* getNormalTexture() const { return m_NormalTexture; }
			void setNormalTexture(Texture* texture) { m_NormalTexture = texture; }

			Texture* getMouseOverTexture() const { return m_MouseOverTexture; }
			void setMouseOverTexture(Texture* texture) { m_MouseOverTexture = texture; }

			Texture* getMouseDownTexture() const { return m_MouseDownTexture; }
			void setMouseDownTexture(Texture* texture) { m_MouseDownTexture = texture; }

			Texture* getDisabledTexture() const { return m_DisabledTexture; }
			void setDiabledTexture(Texture* texture) { m_DisabledTexture = texture; }
			
			virtual void Initialize(RenderDevice* device);

			virtual void Draw(Sprite* sprite);
			virtual void Update(const GameTime* const time);

		};

		class APAPI ButtonGroup : public Control
		{
		private:
			FastList<Button*> m_button;

			int m_selectedIndex;

			UIEventHandler m_eChangeSelection;

			void Button_OnRelease(Control* sender);
		public:
			int getSelectedIndex() const { return m_selectedIndex; }

			Button* getSelectedButton() const { return m_button[m_selectedIndex]; }

			const String& getSelectedText() const;
			void setSelectedText(const String& text);

			UIEventHandler& eventSelectionChanged() { return m_eChangeSelection; }

			ButtonGroup(const FastList<Button*> buttons);
			ButtonGroup(const FastList<Button*> buttons, int selected);

			virtual void Initialize(RenderDevice* device);
			virtual void Update(const GameTime* const time);

			virtual void Draw(Sprite* sprite);
			
		};

		class APAPI ButtonRow : public Control
		{
		public:
			ButtonRow(const Point& position, float width, const List<String>& titles);
			~ButtonRow();

			virtual void Initialize(RenderDevice* device);

			virtual void Update(const GameTime* const time);
			virtual void Draw(Sprite* sprite);

			int getSelectedIndex() const { return m_selectedIndex; }
		private:
			int m_selectedIndex;

			bool m_mouseDown;
			//Vector2 m_size;
			Point m_tailPos;

			int m_count;
			String* m_titles;
			Point* m_texPos;
			Apoc3D::Math::Rectangle* m_rect;
			Apoc3D::Math::Rectangle* m_btRect;

			int m_hoverIndex;

		};

		class APAPI RadioButton : public Control
		{
		private:
			Point m_textOffset;

			bool m_mouseDown;
			bool m_mouseOver;
			bool m_checked;
			bool m_canUncheck;

			void UpdateEvents();
		public:
			bool isChecked() const { return m_checked; }
			bool canUncheck() const { return m_canUncheck; }

			RadioButton(const Point& position, const String& text, bool checked);
			~RadioButton() {}
			void Toggle()
			{
				if (m_checked && m_canUncheck)
				{
					m_checked = false;
				}
				else if (!m_checked)
				{
					m_checked = true;
				}
			}

			virtual void Initialize(RenderDevice* device);
			virtual void Update(const GameTime* const time);
			virtual void Draw(Sprite* sprite);
		};

		//class APAPI RadioButtonGroup : public Control
		//{

		//};
	}
}

#endif