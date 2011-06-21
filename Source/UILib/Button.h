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

#include "Common.h"
#include "Control.h"

using namespace Apoc3D::Graphics::RenderSystem;

namespace Apoc3D
{
	namespace UI
	{
		class Button : public Control
		{
		private:
			Apoc3D::Math::Rectangle m_btnDestRect[3];

			Texture* m_InvalidTexture;
			Texture* m_DisabledTexture;
			Texture* m_MouseDownTexture;
			Texture* m_MouseOverTexture;
			Texture* m_NormalTexture;

			Font* m_fontRef;
			Point m_textPos;
			Point m_textSize;

			PointF m_origin;
			float m_scale;

			bool m_mouseOver;
			bool m_mouseDown;

			void DrawDefaultButton(Sprite* spriteBatch);
			void DrawCustomButton(Sprite* spriteBatch);
			void UpdateEvents();
		public:
			Button(const Point& position, const String& text)
				: Control(position, text), m_mouseOver(false), m_mouseDown(false),
				m_InvalidTexture(0), m_DisabledTexture(0), m_MouseDownTexture(0), m_MouseOverTexture(0), m_NormalTexture(0)
			{
				//BackColor = backColor;
				//ForeColor = foreColor;
			}
			Button(const Point& position, int width, const String& text)
				: Control(position, text, Point(width, 0)), m_mouseOver(false), m_mouseDown(false),
				m_InvalidTexture(0), m_DisabledTexture(0), m_MouseDownTexture(0), m_MouseOverTexture(0), m_NormalTexture(0)
			{
				//BackColor = backColor;
				//ForeColor = foreColor;
			}
			Button(const Point& position, const Point& size, const String& text)
				: Control(position, text, size), m_mouseOver(false), m_mouseDown(false),
				m_InvalidTexture(0), m_DisabledTexture(0), m_MouseDownTexture(0), m_MouseOverTexture(0), m_NormalTexture(0)
			{
				//BackColor = backColor;
				//ForeColor = foreColor;
			}
		
			
			virtual void Initialize(RenderDevice* device);

			virtual void Draw(Sprite* sprite);
			virtual void Update(const GameTime* const time);

		};
	}
}

#endif