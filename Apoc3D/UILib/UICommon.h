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
#include "..\EventDelegate.h"

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

		// These do not account for border padding
		bool guiDrawRegion9(Sprite* sprite, const Apoc3D::Math::Rectangle& dstRect, ColorValue cv, 
			Texture* texture, const Apoc3D::Math::Rectangle* srcRects, Apoc3D::Math::Rectangle* centerDstRegion = nullptr);

		bool guiDrawRegion3(Sprite* sprite, const Point& pt, int width, ColorValue cv, 
			Texture* texture, const Apoc3D::Math::Rectangle* srcRects, Apoc3D::Math::Rectangle* centerDstRegion = nullptr);

		bool guiDrawRegion3Vert(Sprite* sprite, const Point& pos, int height, ColorValue cv, 
			Texture* texture, const Apoc3D::Math::Rectangle* srcRects, Apoc3D::Math::Rectangle* centerDstRegion = nullptr);

		void guiDrawRegion3Capped(Sprite* sprite, const Point& pos, int32 maxWidth, int32 capWidth, 
			ColorValue cv, Texture* texture, const Apoc3D::Math::Rectangle* srcRects);

		void guiDrawRegion9Subbox(Sprite* sprite, const Apoc3D::Math::Rectangle& dstRect, ColorValue cv, 
			Texture* texture, const Apoc3D::Math::Rectangle* srcRects, uint32 subRegionFlags);

	}
}

#endif