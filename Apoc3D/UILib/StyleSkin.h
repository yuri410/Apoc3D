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

#include "apoc3d/Math/Rectangle.h"
#include "apoc3d/Math/ColorValue.h"

using namespace Apoc3D::Math;
using namespace Apoc3D::Graphics::RenderSystem;
using namespace Apoc3D::VFS;

namespace Apoc3D
{
	namespace UI
	{
		struct ControlBounds
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

		class APAPI StyleSkin
		{
		public:
			StyleSkin(RenderDevice* device, const FileLocateRule& rule);
			~StyleSkin();

			Font* ContentTextFont;
			Font* TitleTextFont;
			ColorValue TextColor;
			ColorValue ControlFaceColor;
			ColorValue ControlDarkShadeColor;
			ColorValue ControlLightShadeColor;
			ColorValue ButtonDisabledColorMod;
			ColorValue MIDBackgroundColor;
			ColorValue BorderColor;

			ControlBounds ButtonPadding;
			ControlBounds ButtonMargin;
			Font* ButtonFont;
			Apoc3D::Math::Rectangle ButtonRegionsNormal[9];
			Apoc3D::Math::Rectangle ButtonRegionsHover[9];
			Apoc3D::Math::Rectangle ButtonRegionsDown[9];
			


			Font* TextBoxFont;
			ControlBounds TextBoxPadding;
			ControlBounds TextBoxMargin;
			Apoc3D::Math::Rectangle TextBox[3];

			ControlBounds TextBoxExMargin;
			Apoc3D::Math::Rectangle TextBoxEx[9];

			ControlBounds CheckBoxMargin;
			Font* CheckBoxFont;
			Apoc3D::Math::Rectangle CheckBoxDisable;
			Apoc3D::Math::Rectangle CheckBoxNormal;
			Apoc3D::Math::Rectangle CheckBoxHover;
			Apoc3D::Math::Rectangle CheckBoxDown;
			Apoc3D::Math::Rectangle CheckBoxChecked;
			int32 CheckBoxTextSpacing;

			ControlBounds RadioButtonMargin;
			Font* RadioButtonFont;
			Apoc3D::Math::Rectangle RadioButtonDisable;
			Apoc3D::Math::Rectangle RadioButtonNormal;
			Apoc3D::Math::Rectangle RadioButtonHover;
			Apoc3D::Math::Rectangle RadioButtonDown;
			Apoc3D::Math::Rectangle RadioButtonChecked;
			int32 RadioButtonTextSpacing;

			ControlBounds DropDownButtonMargin;
			Apoc3D::Math::Rectangle DropDownButtonNormal;
			Apoc3D::Math::Rectangle DropDownButtonHover;
			Apoc3D::Math::Rectangle DropDownButtonDown;

			Font* FormFont;
			ControlBounds FormTitlePadding;
			Apoc3D::Math::Rectangle FormTitle[3];
			Apoc3D::Math::Rectangle FormBody[9];
			Apoc3D::Math::Rectangle FormResizer;

			Apoc3D::Math::Rectangle FormCBIconMax;
			Apoc3D::Math::Rectangle FormCBIconMin;
			Apoc3D::Math::Rectangle FormCBIconRestore;
			Apoc3D::Math::Rectangle FormCBIconClose;

			Apoc3D::Math::Rectangle FormCBMaxDisabled;
			Apoc3D::Math::Rectangle FormCBMaxNormal;
			Apoc3D::Math::Rectangle FormCBMaxHover;
			Apoc3D::Math::Rectangle FormCBMaxDown;

			Apoc3D::Math::Rectangle FormCBMinDisabled;
			Apoc3D::Math::Rectangle FormCBMinNormal;
			Apoc3D::Math::Rectangle FormCBMinHover;
			Apoc3D::Math::Rectangle FormCBMinDown;

			Apoc3D::Math::Rectangle FormCBCloseDisabled;
			Apoc3D::Math::Rectangle FormCBCloseNormal;
			Apoc3D::Math::Rectangle FormCBCloseHover;
			Apoc3D::Math::Rectangle FormCBCloseDown;

			Apoc3D::Math::Rectangle FormCBRestoreDisabled;
			Apoc3D::Math::Rectangle FormCBRestoreNormal;
			Apoc3D::Math::Rectangle FormCBRestoreHover;
			Apoc3D::Math::Rectangle FormCBRestoreDown;

			
			Apoc3D::Math::Rectangle ProgressBarBG[3];
			Apoc3D::Math::Rectangle ProgressBarFilled[3];


			Apoc3D::Math::Rectangle HSilderBG[3];
			Apoc3D::Math::Rectangle HSilderFilled[3];
			Apoc3D::Math::Rectangle HSliderHandle;



			Apoc3D::Math::Rectangle VScrollBarCursor[3];
			Apoc3D::Math::Rectangle VScrollBarBG;
			Apoc3D::Math::Rectangle VScrollBarUp;
			Apoc3D::Math::Rectangle VScrollBarDown;

			Apoc3D::Math::Rectangle HScrollBarCursor[3];
			Apoc3D::Math::Rectangle HScrollBarBG;
			Apoc3D::Math::Rectangle HScrollBarLeft;
			Apoc3D::Math::Rectangle HScrollBarRight;

			ControlBounds ListBoxMargin;
			ControlBounds ListBoxPadding;
			Font* ListBoxFont;
			Apoc3D::Math::Rectangle ListBoxBackground[9];


			Apoc3D::Math::Rectangle SubMenuArrow;

			Apoc3D::Math::Rectangle HShade;

			Texture* WhitePixelTexture;
			Texture* SkinTexture;

		private:
			Font* GetFontName(const String& alias);

			void ParseMargin(Apoc3D::Config::ConfigurationSection* sect, ControlBounds& result);
			void ParsePadding(Apoc3D::Config::ConfigurationSection* sect, ControlBounds& result);
			void Parse9Region(Apoc3D::Config::ConfigurationSection* sect, Apoc3D::Math::Rectangle srcRects[9], Apoc3D::Collections::HashMap<String, const Apoc3D::Math::Rectangle*>& cachedRegions);
			void Parse3Region(Apoc3D::Config::ConfigurationSection* sect, Apoc3D::Math::Rectangle srcRects[3], Apoc3D::Collections::HashMap<String, const Apoc3D::Math::Rectangle*>& cachedRegions);
			void ParseRegion(Apoc3D::Config::ConfigurationSection* sect, Apoc3D::Math::Rectangle& srcRect, Apoc3D::Collections::HashMap<String, const Apoc3D::Math::Rectangle*>& cachedRegions);

			void Push9Region(Apoc3D::Config::ConfigurationSection* sect, Apoc3D::Math::Rectangle srcRects[9]);
			void Push3Region(Apoc3D::Config::ConfigurationSection* sect, Apoc3D::Math::Rectangle srcRects[3]);
			void PushRegion(Apoc3D::Config::ConfigurationSection* sect, Apoc3D::Math::Rectangle& srcRect);
		};
	}
}

#endif