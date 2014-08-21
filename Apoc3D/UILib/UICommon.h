#pragma once
#ifndef APOC3D_UICOMMON_H
#define APOC3D_UICOMMON_H

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

#include "apoc3d/Common.h"
#include "apoc3d/Math/Rectangle.h"
#include "apoc3d/Math/ColorValue.h"
#include "apoc3d/EventDelegate.h"

using namespace Apoc3D::Math;
using namespace Apoc3D::Graphics::RenderSystem;

namespace Apoc3D
{
	namespace UI
	{
		typedef EventDelegate1<Control*> UIEventHandler;

		typedef EventDelegate1<MenuItem*> MenuItemEventHandler;

		struct UIGraphic
		{
			Texture* Graphic;
			bool HasSourceRect;
			Apoc3D::Math::Rectangle SourceRect;

			UIGraphic() : HasSourceRect(false), Graphic(nullptr) { }
			UIGraphic(Texture* tex) : HasSourceRect(false), Graphic(tex) { }
			UIGraphic(Texture* tex, const Apoc3D::Math::Rectangle& srcRect) : Graphic(tex), HasSourceRect(true), SourceRect(srcRect) { }

			bool isSet() const { return !!Graphic; }
		};

		struct APAPI ControlBounds
		{
			enum SideIndex
			{
				SI_Left,
				SI_Top,
				SI_Right,
				SI_Bottom
			};


			int32 Left;
			int32 Right;
			int32 Top;
			int32 Bottom;

			ControlBounds() : Left(0), Right(0), Top(0), Bottom(0) { }
			ControlBounds(int32 left, int32 right, int32 top, int32 bottom) : Left(left), Right(right), Top(top), Bottom(bottom) { }
			ControlBounds(const Apoc3D::Math::Rectangle& graphicalArea, const Apoc3D::Math::Rectangle& hotArea);

			int32 getHorizontalSum() const { return Left + Right; }
			int32 getVerticalSum() const { return Top + Bottom; }

			Apoc3D::Math::Rectangle InflateRect(const Apoc3D::Math::Rectangle& rect) const;
			Apoc3D::Math::Rectangle ShrinkRect(const Apoc3D::Math::Rectangle& rect) const;

			int32 operator[](SideIndex idx) const;

			void SetFromLeftTopRightBottom(int32 padding[4]);
			void SetZero();
		};


		enum struct TextAlignment
		{
			Left,
			Center,
			Right
		};





		APAPI void guiGenerateRegion9Rects(const Apoc3D::Math::Rectangle& dstRect, Apoc3D::Math::Rectangle* srcRects, Apoc3D::Math::Rectangle* destRects);
		APAPI void guiGenerateRegion3Rects(const Point& pos, int32 width, Apoc3D::Math::Rectangle* srcRects, Apoc3D::Math::Rectangle* destRects);
		APAPI void guiGenerateRegion3VertRects(const Point& pos, int32 width, Apoc3D::Math::Rectangle* srcRects, Apoc3D::Math::Rectangle* destRects);

		
		APAPI void guiDrawRegion9(Sprite* sprite, const Apoc3D::Math::Rectangle& dstRect, ColorValue cv,
			Texture* texture, const Apoc3D::Math::Rectangle* srcRects, Apoc3D::Math::Rectangle* centerDstRegion = nullptr);

		APAPI void guiDrawRegion3(Sprite* sprite, const Point& pt, int width, ColorValue cv,
			Texture* texture, const Apoc3D::Math::Rectangle* srcRects, Apoc3D::Math::Rectangle* centerDstRegion = nullptr);

		APAPI void guiDrawRegion3Vert(Sprite* sprite, const Point& pos, int height, ColorValue cv,
			Texture* texture, const Apoc3D::Math::Rectangle* srcRects, Apoc3D::Math::Rectangle* centerDstRegion = nullptr);

		APAPI void guiDrawRegion3Capped(Sprite* sprite, const Point& pos, int32 maxWidth, int32 capWidth,
			ColorValue cv, Texture* texture, const Apoc3D::Math::Rectangle* srcRects);

		// No clips do fewer calculations, but can not draw stuff that are smaller than the start/end regions
		APAPI bool guiDrawRegion9Noclip(Sprite* sprite, const Apoc3D::Math::Rectangle& dstRect, ColorValue cv,
			Texture* texture, const Apoc3D::Math::Rectangle* srcRects, Apoc3D::Math::Rectangle* centerDstRegion = nullptr);

		APAPI bool guiDrawRegion3Noclip(Sprite* sprite, const Point& pt, int width, ColorValue cv,
			Texture* texture, const Apoc3D::Math::Rectangle* srcRects, Apoc3D::Math::Rectangle* centerDstRegion = nullptr);

		APAPI bool guiDrawRegion3VertNoclip(Sprite* sprite, const Point& pos, int height, ColorValue cv,
			Texture* texture, const Apoc3D::Math::Rectangle* srcRects, Apoc3D::Math::Rectangle* centerDstRegion = nullptr);

	}
}

#endif