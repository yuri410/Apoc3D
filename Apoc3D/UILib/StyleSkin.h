#pragma once
#ifndef APOC3D_CLASSICSTYLESKIN_H
#define APOC3D_CLASSICSTYLESKIN_H

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

#include "apoc3d/Math/Rectangle.h"
#include "apoc3d/Math/ColorValue.h"

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
			//Apoc3D::Math::Rectangle BtnRowSrcRect[2];
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
			ColorValue FormControlButtonColor;

			Texture* RadioBtnChecked;
			Texture* RadioBtnUnchecked;



			//Apoc3D::Math::Rectangle HSCrsorArea;
			Apoc3D::Math::Rectangle HSCursorLeft;
			Apoc3D::Math::Rectangle HSCursorRight;
			Apoc3D::Math::Rectangle HSCursorMiddle;

			Apoc3D::Math::Rectangle VSCursorTop;
			Apoc3D::Math::Rectangle VSCursorBottom;
			Apoc3D::Math::Rectangle VSCursorMiddle;

			Texture* HScrollBar_Cursor;
			Texture* HScrollBar_Back;
			Texture* HScrollBar_Button;

			Texture* VScrollBar_Cursor;
			Texture* VScrollBar_Back;
			Texture* VScrollBar_Button;

			Texture* TextBox;

			Apoc3D::Math::Rectangle TextBoxSrcRects[9];
			Apoc3D::Math::Rectangle TextBoxSrcRectsSingle[3];

			Texture* ComboButton;
			Texture* CheckBoxTextures[2];
		public:
			// create default skin
			StyleSkin(RenderDevice* device, const FileLocateRule& rule);
			~StyleSkin();
		};
	}
}

#endif