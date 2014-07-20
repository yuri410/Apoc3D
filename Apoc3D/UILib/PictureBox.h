#pragma once
#ifndef APOC3D_PICTUREBOX_H
#define APOC3D_PICTUREBOX_H

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

using namespace Apoc3D::Math;

namespace Apoc3D
{
	namespace UI
	{
		typedef EventDelegate2<Sprite*, Apoc3D::Math::Rectangle*> PictureDrawEventHandler;

		class APAPI PictureBox : public Control
		{
		public:
			PictureBox(const Point& position, int border)
				: Control(position), m_mouseOver(false), m_border(border), m_texture(0)
			{

			}

			virtual void Initialize(RenderDevice* device);

			virtual void Update(const GameTime* const time);
			virtual void Draw(Sprite* sprite);

			Texture* getImage() const { return m_texture; }
			void setImage(Texture* tex) { m_texture = tex; }

			PictureDrawEventHandler eventPictureDraw;

			RTTI_UpcastableDerived(Control);
		private:
			RenderDevice* m_device;
			Texture* m_texture;
			Apoc3D::Math::Rectangle m_srcRect;

			bool m_mouseOver;
			int m_border;

			void UpdateEvents();
		};
	}
}

#endif