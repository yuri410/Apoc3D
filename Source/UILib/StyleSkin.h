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

#ifndef CLASSICSTYLESKIN_H
#define CLASSICSTYLESKIN_H

#include "UICommon.h"
#include "Math/Rectangle.h"
#include "Math/ColorValue.h"

using namespace Apoc3D::Math;
using namespace Apoc3D::Graphics::RenderSystem;
using namespace Apoc3D::VFS;

namespace Apoc3D
{
	namespace UI
	{
		class APAPI StyleSkin
		{
		public:
			Texture* ButtonTexture;
			Apoc3D::Math::Rectangle BtnSrcRect[3];
			uint BtnDimColor;
			uint BtnHighLightColor;
			uint BtnTextDimColor;
			uint BtnTextHighLightColor;

			int BtnVertPadding;
			int BtnHozPadding;



			Texture* WhitePixelTexture;
			Apoc3D::Math::Rectangle BtnRowSrcRect[2];
			Apoc3D::Math::Rectangle BtnRowSeparator;
			String ControlFontName;

			Texture* FormBorderTexture[10];
			Texture* SubMenuArrowTexture;

			Texture* FormCloseButton;
			Texture* FormMinimizeButton;
			Texture* FormMaximizeButton;
			Texture* FormRestoreButton;

			ColorValue ForeColor;
			ColorValue BackColor;

		public:
			// create default skin
			StyleSkin(RenderDevice* device, const FileLocateRule& rule);
			~StyleSkin();
		};
	}
}

#endif