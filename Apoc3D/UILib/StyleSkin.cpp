/*
-----------------------------------------------------------------------------
This source file is part of Apoc3D Engine

Copyright (c) 2009+ Tao Xin

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

#include "StyleSkin.h"
#include "apoc3d/Config/Configuration.h"
#include "apoc3d/Config/XmlConfigurationFormat.h"
#include "apoc3d/Config/ConfigurationSection.h"
#include "apoc3d/Graphics/RenderSystem/Texture.h"
#include "apoc3d/Graphics/RenderSystem/RenderDevice.h"
#include "apoc3d/Graphics/TextureManager.h"
#include "apoc3d/Vfs/FileSystem.h"
#include "apoc3d/Vfs/FileLocateRule.h"
#include "apoc3d/Vfs/ResourceLocation.h"
#include "apoc3d/Math/ColorValue.h"
#include "apoc3d/Utility/StringUtils.h"
#include "FontManager.h"

using namespace Apoc3D::Config;
using namespace Apoc3D::Graphics;
using namespace Apoc3D::Math;
using namespace Apoc3D::Graphics::RenderSystem;
using namespace Apoc3D::Utility;
using namespace Apoc3D::VFS;

namespace Apoc3D
{
	namespace UI
	{
		StyleSkin::StyleSkin(RenderDevice* device, const FileLocateRule& rule)
		{
			FileLocation fl = FileSystem::getSingleton().Locate(L"skin.xml", rule);
			Configuration* config = XMLConfigurationFormat::Instance.Load(fl);
			
			ConfigurationSection* basicSect = config->get(L"Basic");
			String contentTextFontName = basicSect->getValue(L"TextFont");
			String titleTextFontName = basicSect->getValue(L"TitleFont");
			String texturePackName = basicSect->getValue(L"TexturePack");

			fl = FileSystem::getSingleton().Locate(texturePackName, rule);
			SkinTexture = TextureManager::getSingleton().CreateUnmanagedInstance(device, fl);

			TextColor = ParseColorValue(basicSect->getValue(L"TextColor"));
			TextColorDisabled = ParseColorValue(basicSect->getValue(L"TextColorDisabled"));
			TextSelectionColor = ParseColorValue(basicSect->getValue(L"TextSelectionColor"));
			TextSelectionColorDisabled = ParseColorValue(basicSect->getValue(L"TextSelectionColorDisabled"));

			ControlFaceColor = ParseColorValue(basicSect->getValue(L"ControlFaceColor"));
			ControlDarkShadeColor = ParseColorValue(basicSect->getValue(L"ControlDarkShadeColor"));
			ControlLightShadeColor = ParseColorValue(basicSect->getValue(L"ControlLightShadeColor"));

			MIDBackgroundColor = ParseColorValue(basicSect->getValue(L"MIDBackgroundColor"));

			BorderColor = ParseColorValue(basicSect->getValue(L"BorderColor"));

			if (!FontManager::getSingleton().hasFont(contentTextFontName))
			{
				fl = FileSystem::getSingleton().Locate(contentTextFontName + L".fnt", rule);
				ContentTextFont = FontManager::getSingleton().LoadFont(device, contentTextFontName, fl);
			}
			else
			{
				ContentTextFont = FontManager::getSingleton().getFont(contentTextFontName);
			}

			if (!FontManager::getSingleton().hasFont(titleTextFontName))
			{
				fl = FileSystem::getSingleton().Locate(titleTextFontName + L".fnt", rule);
				TitleTextFont = FontManager::getSingleton().LoadFont(device, titleTextFontName, fl);
			}
			else
			{
				TitleTextFont = FontManager::getSingleton().getFont(titleTextFontName);
			}


			HashMap<String, const Apoc3D::Math::Rectangle*> cachedRegions;
			{ // Button
				ConfigurationSection* btnSect = config->get(L"Button");

				ButtonFont = GetFontName(btnSect->getAttribute(L"Font"));
				ParseMargin(btnSect, ButtonMargin);
				ParsePadding(btnSect, ButtonPadding);

				ConfigurationSection* normalSect = btnSect->getSection(L"Normal");
				ConfigurationSection* downSect = btnSect->getSection(L"Down");
				ConfigurationSection* hoverSect = btnSect->getSection(L"Hover");
				ConfigurationSection* disabledSect = btnSect->getSection(L"Disabled");

				Parse9Region(normalSect, ButtonNormalRegions, cachedRegions);
				Parse9Region(downSect, ButtonDownRegions, cachedRegions);
				Parse9Region(hoverSect, ButtonHoverRegions, cachedRegions);
				Parse9Region(disabledSect, ButtonDisabledRegions, cachedRegions);

				Offset9Region(normalSect, ButtonNormalRegions);
				Offset9Region(downSect, ButtonDownRegions);
				Offset9Region(hoverSect, ButtonHoverRegions);
				Offset9Region(disabledSect, ButtonDisabledRegions);

				ParseColorValue(normalSect, ButtonNormalColor);
				ParseColorValue(downSect, ButtonHoverColor);
				ParseColorValue(hoverSect, ButtonDownColor);
				ParseColorValue(disabledSect, ButtonDisabledColor);

				cachedRegions.Clear();
			}

			{ // TextBox
				ConfigurationSection* textBoxSect = config->get(L"TextBox");

				TextBoxFont = GetFontName(textBoxSect->getAttribute(L"Font"));
				ParseMargin(textBoxSect, TextBoxMargin);
				ParsePadding(textBoxSect, TextBoxPadding);

				ConfigurationSection* normalSect = textBoxSect->getSection(L"Normal");
				Parse3Region(normalSect, TextBoxRegions, cachedRegions);
				Offset3Region(normalSect, TextBoxRegions);
				ParseColorValue(normalSect, TextBoxColor);

				cachedRegions.Clear();
			}
			{ // TextFieldEx
				ConfigurationSection* textBoxExSect = config->get(L"TextFieldMultiline");
				
				ParseMargin(textBoxExSect, TextBoxExMargin);

				ConfigurationSection* normalSect = textBoxExSect->getSection(L"Normal");
				Parse9Region(normalSect, TextBoxExRegions, cachedRegions);
				Offset9Region(normalSect, TextBoxExRegions);
				ParseColorValue(normalSect, TextBoxExColor);

				cachedRegions.Clear();
			}


			{ // CheckBox
				ConfigurationSection* checkBoxSect = config->get(L"CheckBox");

				CheckBoxFont = GetFontName(checkBoxSect->getAttribute(L"Font"));
				ParseMargin(checkBoxSect, CheckBoxMargin);

				CheckBoxTextSpacing = checkBoxSect->GetAttributeInt(L"TextSpacing");

				ConfigurationSection* disabledSect = checkBoxSect->getSection(L"Disabled");
				ConfigurationSection* normalSect = checkBoxSect->getSection(L"Normal");
				ConfigurationSection* hoverSect = checkBoxSect->getSection(L"Hover");
				ConfigurationSection* downSect = checkBoxSect->getSection(L"Down");
				ConfigurationSection* checkedSect = checkBoxSect->getSection(L"Checked");
				ConfigurationSection* disabledCheckedSect = checkBoxSect->getSection(L"DisabledChecked");

				ParseRegion(disabledSect, CheckBoxDisabledRegion, cachedRegions);
				ParseRegion(normalSect, CheckBoxNormalRegion, cachedRegions);
				ParseRegion(hoverSect, CheckBoxHoverRegion, cachedRegions);
				ParseRegion(downSect, CheckBoxDownRegion, cachedRegions);
				ParseRegion(checkedSect, CheckBoxCheckedRegion, cachedRegions);
				ParseRegion(disabledCheckedSect, CheckBoxDisabledCheckedRegion, cachedRegions);

				OffsetRegion(disabledSect, CheckBoxDisabledRegion);
				OffsetRegion(normalSect, CheckBoxNormalRegion);
				OffsetRegion(hoverSect, CheckBoxHoverRegion);
				OffsetRegion(downSect, CheckBoxDownRegion);
				OffsetRegion(checkedSect, CheckBoxCheckedRegion);
				OffsetRegion(disabledCheckedSect, CheckBoxDisabledCheckedRegion);

				ParseColorValue(disabledSect, CheckBoxDisabledColor);
				ParseColorValue(normalSect, CheckBoxNormalColor);
				ParseColorValue(hoverSect, CheckBoxHoverColor);
				ParseColorValue(downSect, CheckBoxDownColor);
				ParseColorValue(checkedSect, CheckBoxCheckedColor);
				ParseColorValue(disabledCheckedSect, CheckBoxDisableCheckedColor);

				cachedRegions.Clear();
			}

			{ // RadioButton
				ConfigurationSection* radioButtonSect = config->get(L"RadioButton");

				RadioButtonFont = GetFontName(radioButtonSect->getAttribute(L"Font"));
				ParseMargin(radioButtonSect, RadioButtonMargin);
				RadioButtonTextSpacing = radioButtonSect->GetAttributeInt(L"TextSpacing");

				ConfigurationSection* disabledSect = radioButtonSect->getSection(L"Disabled");
				ConfigurationSection* normalSect = radioButtonSect->getSection(L"Normal");
				ConfigurationSection* hoverSect = radioButtonSect->getSection(L"Hover");
				ConfigurationSection* downSect = radioButtonSect->getSection(L"Down");
				ConfigurationSection* checkedSect = radioButtonSect->getSection(L"Checked");
				ConfigurationSection* disabledCheckedSect = radioButtonSect->getSection(L"DisabledChecked");

				ParseRegion(disabledSect, RadioButtonDisabledRegion, cachedRegions);
				ParseRegion(normalSect, RadioButtonNormalRegion, cachedRegions);
				ParseRegion(hoverSect, RadioButtonHoverRegion, cachedRegions);
				ParseRegion(downSect, RadioButtonDownRegion, cachedRegions);
				ParseRegion(checkedSect, RadioButtonCheckedRegion, cachedRegions);
				ParseRegion(disabledCheckedSect, RadioButtonDisabledCheckedRegion, cachedRegions);

				OffsetRegion(disabledSect, RadioButtonDisabledRegion);
				OffsetRegion(normalSect, RadioButtonNormalRegion);
				OffsetRegion(hoverSect, RadioButtonHoverRegion);
				OffsetRegion(downSect, RadioButtonDownRegion);
				OffsetRegion(checkedSect, RadioButtonCheckedRegion);
				OffsetRegion(disabledCheckedSect, RadioButtonDisabledCheckedRegion);

				ParseColorValue(disabledSect, RadioButtonDisabledColor);
				ParseColorValue(normalSect, RadioButtonNormalColor);
				ParseColorValue(hoverSect, RadioButtonHoverColor);
				ParseColorValue(downSect, RadioButtonDownColor);
				ParseColorValue(checkedSect, RadioButtonCheckedColor);
				ParseColorValue(disabledCheckedSect, RadioButtonDisabledCheckedColor);

				cachedRegions.Clear();
			}

			{ // DropdownButton
				ConfigurationSection* dropdownSect = config->get(L"DropdownButton");

				ParseOffset(dropdownSect, DropdownButtonOffset);

				ParseMargin(dropdownSect, DropdownButtonMargin);

				ConfigurationSection* normalSect = dropdownSect->getSection(L"Normal");
				ConfigurationSection* hoverSect = dropdownSect->getSection(L"Hover");
				ConfigurationSection* downSect = dropdownSect->getSection(L"Down");

				ParseRegion(normalSect, DropdownButtonNormalRegion, cachedRegions);
				ParseRegion(hoverSect, DropdownButtonHoverRegion, cachedRegions);
				ParseRegion(downSect, DropdownButtonDownRegion, cachedRegions);

				OffsetRegion(normalSect, DropdownButtonNormalRegion);
				OffsetRegion(hoverSect, DropdownButtonHoverRegion);
				OffsetRegion(downSect, DropdownButtonDownRegion);
				
				ParseColorValue(normalSect, DropdownButtonNormalColor);
				ParseColorValue(hoverSect, DropdownButtonHoverColor);
				ParseColorValue(downSect, DropdownButtonDownColor);

				cachedRegions.Clear();
			}

			{ // VScrollBar
				ConfigurationSection* vsBar = config->get(L"VScrollBar");

				ConfigurationSection* bgSect = vsBar->getSection(L"Background");
				ConfigurationSection* cursor = vsBar->getSection(L"Cursor")->getSection(L"Normal");
				ConfigurationSection* backSect = vsBar->getSection(L"UpButton")->getSection(L"Normal");
				ConfigurationSection* forwardSect = vsBar->getSection(L"DownButton")->getSection(L"Normal");

				ParseRegion(bgSect, VScrollBarBG, cachedRegions);
				Parse3Region(cursor, VScrollBarCursor, cachedRegions);

				OffsetRegion(bgSect, VScrollBarBG);
				Offset3Region(cursor, VScrollBarCursor);

				cachedRegions.Clear();
				ParseRegion(backSect, VScrollBarUp, cachedRegions);
				cachedRegions.Clear();
				ParseRegion(forwardSect, VScrollBarDown, cachedRegions);

				OffsetRegion(backSect, VScrollBarUp);
				OffsetRegion(forwardSect, VScrollBarDown);

				cachedRegions.Clear();
			}


			{ // HScrollBar
				ConfigurationSection* vsBar = config->get(L"HScrollBar");

				ConfigurationSection* bgSect = vsBar->getSection(L"Background");
				ConfigurationSection* cursor = vsBar->getSection(L"Cursor")->getSection(L"Normal");
				ConfigurationSection* backSect = vsBar->getSection(L"BackButton")->getSection(L"Normal");
				ConfigurationSection* forwardSect = vsBar->getSection(L"ForwardButton")->getSection(L"Normal");

				ParseRegion(bgSect, HScrollBarBG, cachedRegions);
				Parse3Region(cursor, HScrollBarCursor, cachedRegions);

				OffsetRegion(bgSect, HScrollBarBG);
				Offset3Region(cursor, HScrollBarCursor);

				cachedRegions.Clear();
				ParseRegion(backSect, HScrollBarLeft, cachedRegions);
				cachedRegions.Clear();
				ParseRegion(forwardSect, HScrollBarRight, cachedRegions);

				OffsetRegion(backSect, HScrollBarLeft);
				OffsetRegion(forwardSect, HScrollBarRight);

				cachedRegions.Clear();
			}


			{ // ListBox
				ConfigurationSection* listBox = config->get(L"ListBox");
				ListBoxFont = GetFontName(listBox->getAttribute(L"Font"));

				ParseMargin(listBox, ListBoxMargin);
				ParsePadding(listBox, ListBoxPadding);

				ConfigurationSection* normal = listBox->getSection(L"Normal");
				Parse9Region(normal, ListBoxBackground, cachedRegions);
				
				Offset9Region(normal, ListBoxBackground);
				cachedRegions.Clear();
			}


			{ // ProgressBar
				ConfigurationSection* pbSect = config->get(L"ProgressBar");

				ProgressBarVS.Graphic = SkinTexture;
				ProgressBarVS.FontRef = GetFontName(pbSect->getAttribute(L"Font"));

				ParseMargin(pbSect, ProgressBarVS.Margin);

				ConfigurationSection* textSect = pbSect->getSection(L"Text");
				textSect->TryGetAttributeBool(L"HasShadow", ProgressBarVS.HasTextShadow);
				ParseColorValue(textSect, L"TextColor", ProgressBarVS.TextColor);
				ParseColorValue(textSect, L"TextColorDisabled", ProgressBarVS.TextColorDisabled);
				ParseColorValue(textSect, L"TextShadowColor", ProgressBarVS.TextShadowColor);
				ParseColorValue(textSect, L"TextShadowColorDisabled", ProgressBarVS.TextShadowColorDisabled);
				ParsePoint(textSect, L"TextShadowOffset", ProgressBarVS.TextShadowOffset);

				ConfigurationSection* bgSect = pbSect->getSection(L"Background");
				ConfigurationSection* barSect = pbSect->getSection(L"Bar");

				Parse3Region(bgSect, ProgressBarVS.BackgroundRegions, cachedRegions);
				Parse3Region(barSect, ProgressBarVS.BarRegions, cachedRegions);

				Offset3Region(bgSect, ProgressBarVS.BackgroundRegions);
				Offset3Region(barSect, ProgressBarVS.BarRegions);

				ParseColorValue(bgSect, ProgressBarVS.BackgroundColor);
				ParseColorValue(barSect, ProgressBarVS.BarColor);

				cachedRegions.Clear();

				int32 temp[2];
				barSect->GetAttributeIntsChecked(L"StartEndPadding", temp);
				ProgressBarVS.BarStartPad = temp[0];
				ProgressBarVS.BarEndPad = temp[1];
			}

			{ // HSliderBar
				ConfigurationSection* pbSect = config->get(L"HSliderBar");

				HSliderBar.Graphic = SkinTexture;
				ParseMargin(pbSect, HSliderBar.Margin);

				ConfigurationSection* bgSect = pbSect->getSection(L"Background");
				ConfigurationSection* barSect = pbSect->getSection(L"Bar");
				
				Apoc3D::Math::Rectangle handleRect;

				Parse3Region(bgSect, HSliderBar.BackgroundRegions, cachedRegions);
				Parse3Region(barSect, HSliderBar.BarRegions, cachedRegions);
				
				Offset3Region(bgSect, HSliderBar.BackgroundRegions);
				Offset3Region(barSect, HSliderBar.BarRegions);
				
				ParseColorValue(bgSect, HSliderBar.BackgroundColor);
				ParseColorValue(barSect, HSliderBar.BarColor);

				cachedRegions.Clear();

				ConfigurationSection* handleSect = pbSect->getSection(L"Handle");
				{
					ConfigurationSection* normalSect = handleSect->getSection(L"Normal");
					ConfigurationSection* downSect = handleSect->getSection(L"Down");
					ConfigurationSection* hoverSect = handleSect->getSection(L"Hover");
					ConfigurationSection* disabledSect = handleSect->getSection(L"Disabled");

					Apoc3D::Math::Rectangle normalRect, downRect, hoverRect, disabledRect;
					ColorValue normalColor = CV_White;
					ColorValue downColor = CV_White;
					ColorValue hoverColor = CV_White;
					ColorValue disabledColor = CV_White;

					ParseRegion(normalSect, normalRect, cachedRegions);
					ParseRegion(downSect, downRect, cachedRegions);
					ParseRegion(hoverSect, hoverRect, cachedRegions);
					ParseRegion(disabledSect, disabledRect, cachedRegions);

					OffsetRegion(normalSect, normalRect);
					OffsetRegion(downSect, downRect);
					OffsetRegion(hoverSect, hoverRect);
					OffsetRegion(disabledSect, disabledRect);

					ParseColorValue(normalSect, normalColor);
					ParseColorValue(downSect, downColor);
					ParseColorValue(hoverSect, hoverColor);
					ParseColorValue(disabledSect, disabledColor);

					HSliderBar.HandleNormalGraphic = UIGraphicSimple(SkinTexture, normalRect, normalColor);
					HSliderBar.HandleHoverGraphic = UIGraphicSimple(SkinTexture, hoverRect, hoverColor);
					HSliderBar.HandleDownGraphic = UIGraphicSimple(SkinTexture, downRect, downColor);
					HSliderBar.HandleDisabledGraphic = UIGraphicSimple(SkinTexture, disabledRect, disabledColor);
				}

				cachedRegions.Clear();

				int32 temp[2];
				barSect->GetAttributeIntsChecked(L"StartEndPadding", temp);
				HSliderBar.BarStartPad = temp[0];
				HSliderBar.BarEndPad = temp[1];
				
				ParseOffset(handleSect, HSliderBar.HandleOffset);
				ParseMargin(handleSect, HSliderBar.HandleMargin);
			}

			{ // VSliderBar


				cachedRegions.Clear();
			}


			{ // Form
				ConfigurationSection* formSect = config->get(L"Form");
				FormFont = GetFontName(formSect->getAttribute(L"Font"));

				ParsePadding(formSect, FormTitlePadding);

				ConfigurationSection* titleNormalSect = formSect->getSection(L"TitleNormal");
				ConfigurationSection* backgroundSect = formSect->getSection(L"Background");
				ConfigurationSection* resizer = formSect->getSection(L"Resizer");

				Parse3Region(titleNormalSect, FormTitle, cachedRegions);
				Parse9Region(backgroundSect, FormBody, cachedRegions);
				ParseRegion(resizer, FormResizer, cachedRegions);

				Offset3Region(titleNormalSect, FormTitle);
				Offset9Region(backgroundSect, FormBody);
				OffsetRegion(resizer, FormResizer);

				cachedRegions.Clear();

				ConfigurationSection* iconSect = formSect->getSection(L"ControlBoxIcon");
				{
					ConfigurationSection* maxSect = iconSect->getSection(L"MaxOverlay");
					ConfigurationSection* minSect = iconSect->getSection(L"MinOverlay");
					ConfigurationSection* closeSect = iconSect->getSection(L"CloseOverlay");
					ConfigurationSection* restoreSect = iconSect->getSection(L"RestoreOverlay");

					ParseRegion(maxSect, FormCBIconMax, cachedRegions);
					ParseRegion(minSect, FormCBIconMin, cachedRegions);
					ParseRegion(closeSect, FormCBIconClose, cachedRegions);
					ParseRegion(restoreSect, FormCBIconRestore, cachedRegions);

					OffsetRegion(maxSect, FormCBIconMax);
					OffsetRegion(minSect, FormCBIconMin);
					OffsetRegion(closeSect, FormCBIconClose);
					OffsetRegion(restoreSect, FormCBIconRestore);

					cachedRegions.Clear();
				}

				struct ButtonRef
				{
					ConfigurationSection* section;
					Apoc3D::Math::Rectangle& disabled;
					Apoc3D::Math::Rectangle& normal;
					Apoc3D::Math::Rectangle& hover;
					Apoc3D::Math::Rectangle& down;
				} cbBtnRegions[4] =
				{
					{ formSect->getSection(L"MaximizeButton"), FormCBMaxDisabled, FormCBMaxNormal, FormCBMaxHover, FormCBMaxDown },
					{ formSect->getSection(L"CloseButton"), FormCBCloseDisabled, FormCBCloseNormal, FormCBCloseHover, FormCBCloseDown },
					{ formSect->getSection(L"MinimizeButton"), FormCBMinDisabled, FormCBMinNormal, FormCBMinHover, FormCBMinDown },
					{ formSect->getSection(L"RestoreButton"), FormCBRestoreDisabled, FormCBRestoreNormal, FormCBRestoreHover, FormCBRestoreDown },
				};

				for (const ButtonRef& e : cbBtnRegions)
				{
					ConfigurationSection* cbbSect = e.section;

					ConfigurationSection* disabledSect = cbbSect->getSection(L"Disabled");
					ConfigurationSection* normalSect = cbbSect->getSection(L"Normal");
					ConfigurationSection* hoverSect = cbbSect->getSection(L"Hover");
					ConfigurationSection* downSect = cbbSect->getSection(L"Down");

					ParseRegion(disabledSect, e.disabled, cachedRegions);
					ParseRegion(normalSect, e.normal, cachedRegions);
					ParseRegion(hoverSect, e.hover, cachedRegions);
					ParseRegion(downSect, e.down, cachedRegions);

					OffsetRegion(disabledSect, e.disabled);
					OffsetRegion(normalSect, e.normal);
					OffsetRegion(hoverSect, e.hover);
					OffsetRegion(downSect, e.down);

					cachedRegions.Clear();
				}

				cachedRegions.Clear();
			}

			// Menu
			{
				ConfigurationSection* menuSect = config->get(L"Menu");

				ConfigurationSection* subMenuArrow = menuSect->getSection(L"SubMenuArrow");
				ConfigurationSection* hshadeSect = menuSect->getSection(L"HShade");

				ParseRegion(subMenuArrow, SubMenuArrow, cachedRegions);
				ParseRegion(hshadeSect, HShade, cachedRegions);

				OffsetRegion(subMenuArrow, SubMenuArrow);
				OffsetRegion(hshadeSect, HShade);

				cachedRegions.Clear();
			}

			delete config;
		}

		StyleSkin::~StyleSkin()
		{
			DELETE_AND_NULL(SkinTexture);
		}

		Font* StyleSkin::GetFontName(const String& alias)
		{
			return alias == L"Title" ? TitleTextFont : ContentTextFont;
		}

		void StyleSkin::ParseMargin(ConfigurationSection* sect, ControlBounds& result)
		{
			int32 margins[4];
			if (sect->TryGetAttributeIntsChecked(L"Margin", margins))
			{
				result.SetFromLeftTopRightBottom(margins);
			}
		}
		void StyleSkin::ParsePadding(ConfigurationSection* sect, ControlBounds& result)
		{
			int32 padding[4];
			if (sect->TryGetAttributeIntsChecked(L"Padding", padding))
			{
				result.SetFromLeftTopRightBottom(padding);
			}
		}

		void StyleSkin::Parse9Region(ConfigurationSection* sect, Apoc3D::Math::Rectangle (&srcRects)[9], HashMap<String, const Apoc3D::Math::Rectangle*>& cachedRegions)
		{
			String ref;
			if (sect->tryGetAttribute(L"RegionRef", ref))
			{
				//assert(sect->getAttribute(L"Type") == L"9region_ref");
				FillArray(srcRects, cachedRegions[ref]);
				return;
			}

			//assert(sect->getAttribute(L"Type") == L"9region");
			int32 parts[4 * 9];
			StringUtils::SplitParseIntsChecked(sect->getValue(), parts, L"[], ");
			
			for (int i = 0; i < 9; i++)
			{
				Apoc3D::Math::Rectangle& rect = srcRects[i];

				rect.X = parts[i * 4];
				rect.Y = parts[i * 4 + 1];
				rect.Width = parts[i * 4 + 2];
				rect.Height = parts[i * 4 + 3];
			}

			cachedRegions.Add(sect->getName(), srcRects);
		}
		void StyleSkin::Parse3Region(ConfigurationSection* sect, Apoc3D::Math::Rectangle (&srcRects)[3], HashMap<String, const Apoc3D::Math::Rectangle*>& cachedRegions)
		{
			String ref;
			if (sect->tryGetAttribute(L"RegionRef", ref))
			{
				//assert(sect->getAttribute(L"Type") == L"9region_ref");
				FillArray(srcRects, cachedRegions[ref]);
				return;
			}

			//assert(sect->getAttribute(L"Type") == L"9region");

			int32 parts[4 * 3];
			StringUtils::SplitParseIntsChecked(sect->getValue(), parts, L"[], ");

			for (int i = 0; i < 3; i++)
			{
				Apoc3D::Math::Rectangle& rect = srcRects[i];

				rect.X = parts[i * 4];
				rect.Y = parts[i * 4 + 1];
				rect.Width = parts[i * 4 + 2];
				rect.Height = parts[i * 4 + 3];
			}

			cachedRegions.Add(sect->getName(), srcRects);
		}
		void StyleSkin::ParseRegion(ConfigurationSection* sect, Apoc3D::Math::Rectangle& srcRect, HashMap<String, const Apoc3D::Math::Rectangle*>& cachedRegions)
		{
			String ref;
			if (sect->tryGetAttribute(L"RegionRef", ref))
			{
				//assert(sect->getAttribute(L"Type") == L"9region_ref");
				srcRect = *cachedRegions[ref];
				return;
			}

			//assert(sect->getAttribute(L"Type") == L"9region");
			
			int32 parts[4];
			StringUtils::SplitParseIntsChecked(sect->getValue(), parts, L"[], ");

			srcRect.X = parts[0];
			srcRect.Y = parts[1];
			srcRect.Width = parts[2];
			srcRect.Height = parts[3];

			cachedRegions.Add(sect->getName(), &srcRect);
		}

		void StyleSkin::ParseOffset(Apoc3D::Config::ConfigurationSection* sect, Point& result) { return ParsePoint(sect, L"Offset", result); }
		void StyleSkin::ParsePoint(Apoc3D::Config::ConfigurationSection* sect, const String& attName, Point& result)
		{
			int32 coordArr[2];
			if (sect->TryGetAttributeIntsChecked(attName, coordArr))
			{
				result.X = coordArr[0];
				result.Y = coordArr[1];
			}
		}

		void StyleSkin::ParseColorValue(Apoc3D::Config::ConfigurationSection* sect, ColorValue& result) { ParseColorValue(sect, L"Color", result); }
		void StyleSkin::ParseColorValue(Apoc3D::Config::ConfigurationSection* sect, const String& attName, ColorValue& result)
		{
			String exp;
			if (sect->tryGetAttribute(attName, exp))
			{
				if (!ColorValueConverter.TryParse(exp, result))
				{
					result = StringUtils::ParseUInt32Hex(exp);
				}
			}
		}
		ColorValue StyleSkin::ParseColorValue(const String& txt)
		{
			ColorValue result;
			if (!ColorValueConverter.TryParse(txt, result))
			{
				result = StringUtils::ParseUInt32Hex(txt);
			}
			return result;
		}

		void StyleSkin::Offset9Region(ConfigurationSection* sect, Apoc3D::Math::Rectangle (&srcRects)[9])
		{
			int32 coordArr[2];
			if (sect->TryGetAttributeIntsChecked(L"Coord", coordArr))
			{
				for (Apoc3D::Math::Rectangle& r : srcRects)
				{
					r.X += coordArr[0];
					r.Y += coordArr[1];
				}
			}
		}
		void StyleSkin::Offset3Region(ConfigurationSection* sect, Apoc3D::Math::Rectangle(&srcRects)[3])
		{
			int32 coordArr[2];
			if (sect->TryGetAttributeIntsChecked(L"Coord", coordArr))
			{
				for (Apoc3D::Math::Rectangle& r : srcRects)
				{
					r.X += coordArr[0];
					r.Y += coordArr[1];
				}
			}
		}
		void StyleSkin::OffsetRegion(ConfigurationSection* sect, Apoc3D::Math::Rectangle& srcRect)
		{
			int32 coordArr[2];
			if (sect->TryGetAttributeIntsChecked(L"Coord", coordArr))
			{
				srcRect.X += coordArr[0];
				srcRect.Y += coordArr[1];
			}
		}
	}
}