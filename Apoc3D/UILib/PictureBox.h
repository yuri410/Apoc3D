#pragma once
#ifndef APOC3D_PICTUREBOX_H
#define APOC3D_PICTUREBOX_H

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

using namespace Apoc3D::Math;

namespace Apoc3D
{
	namespace UI
	{
		typedef EventDelegate<Sprite*, Apoc3D::Math::Rectangle*> PictureDrawEventHandler;

		class APAPI PictureBox : public Control
		{
			RTTI_DERIVED(PictureBox, Control);
		public:
			typedef EventDelegate<PictureBox*, const AppTime*> InteractiveUpdateEventHandler;

			PictureBox(const Point& position, int border);
			PictureBox(const Point& position, int border, Texture* texture);
			PictureBox(const StyleSkin* skin, const Point& position, int border);
			PictureBox(const StyleSkin* skin, const Point& position, int border, Texture* texture);

			virtual ~PictureBox();

			virtual void Update(const AppTime* time);
			virtual void Draw(Sprite* sprite);

			void setSize(const Point& sz) { m_size = sz; }
			void setSize(int32 w, int32 h) { m_size = Point(w, h); }
			void setWidth(int32 w) { m_size.X = w; }
			void setHeight(int32 h) { m_size.Y = h; }
			
			UIGraphicSimple Graphic;

			PictureDrawEventHandler eventPictureDraw;

			UIEventHandler eventMouseHover;
			UIEventHandler eventMouseOut;
			UIEventHandler eventPress;
			UIEventHandler eventRelease;

			InteractiveUpdateEventHandler eventInteractiveUpdate;

		private:
			void OnMouseHover();
			void OnMouseOut();
			void OnPress();
			void OnRelease();

			bool m_mouseDown = false;
			bool m_mouseHover = false;
			int m_border = 0;
			
		};
	}
}

#endif