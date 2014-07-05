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
			FileLocation* fl = FileSystem::getSingleton().Locate(L"skin.xml", rule);
			Configuration* config = XMLConfigurationFormat::Instance.Load(fl);
			delete fl;

			String packTexName;

			ConfigurationSection* basicSect = config->get(L"Basic");
			String contentTextFontName = basicSect->getValue(L"TextFont");
			String titleTextFontName = basicSect->getValue(L"TitleFont");
			packTexName = basicSect->getValue(L"Pack");

			TextColor = StringUtils::ParseUInt32Hex(basicSect->getValue(L"TextColor"));
			ControlFaceColor = StringUtils::ParseUInt32Hex(basicSect->getValue(L"ControlFaceColor"));
			ControlDarkShadeColor = StringUtils::ParseUInt32Hex(basicSect->getValue(L"ControlDarkShadeColor"));
			ControlLightShadeColor = StringUtils::ParseUInt32Hex(basicSect->getValue(L"ControlLightShadeColor"));

			ButtonDisabledColorMod = StringUtils::ParseUInt32Hex(basicSect->getValue(L"ButtonDisabledColorMod"));
			MIDBackgroundColor = StringUtils::ParseUInt32Hex(basicSect->getValue(L"MIDBackgroundColor"));

			BorderColor = StringUtils::ParseUInt32Hex(basicSect->getValue(L"BorderColor"));

			if (!FontManager::getSingleton().hasFont(contentTextFontName))
			{
				fl = FileSystem::getSingleton().Locate(contentTextFontName + L".fnt", rule);
				ContentTextFont = FontManager::getSingleton().LoadFont(device, contentTextFontName, fl);
			}
			if (!FontManager::getSingleton().hasFont(titleTextFontName))
			{
				fl = FileSystem::getSingleton().Locate(titleTextFontName + L".fnt", rule);
				TitleTextFont = FontManager::getSingleton().LoadFont(device, titleTextFontName, fl);
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

				Parse9Region(normalSect, ButtonRegionsNormal, cachedRegions);
				Parse9Region(downSect, ButtonRegionsDown, cachedRegions);
				Parse9Region(hoverSect, ButtonRegionsHover, cachedRegions);

				Push9Region(normalSect, ButtonRegionsNormal);
				Push9Region(downSect, ButtonRegionsDown);
				Push9Region(hoverSect, ButtonRegionsHover);

				cachedRegions.Clear();
			}

			{ // TextBox
				ConfigurationSection* textBoxSect = config->get(L"TextBox");

				TextBoxFont = GetFontName(textBoxSect->getAttribute(L"Font"));
				ParseMargin(textBoxSect, TextBoxMargin);
				ParsePadding(textBoxSect, TextBoxPadding);

				ConfigurationSection* normalSect = textBoxSect->getSection(L"Normal");
				Parse3Region(normalSect, TextBox, cachedRegions);
				Push3Region(normalSect, TextBox);

				cachedRegions.Clear();
			}
			{ // TextFieldEx
				ConfigurationSection* textBoxExSect = config->get(L"TextFieldMultiline");
				
				ConfigurationSection* normalSect = textBoxExSect->getSection(L"Normal");
				Parse9Region(normalSect, TextBoxEx, cachedRegions);
				Push9Region(normalSect, TextBoxEx);

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

				ParseRegion(disabledSect, CheckBoxDisable, cachedRegions);
				ParseRegion(normalSect, CheckBoxNormal, cachedRegions);
				ParseRegion(hoverSect, CheckBoxHover, cachedRegions);
				ParseRegion(downSect, CheckBoxDown, cachedRegions);
				ParseRegion(checkedSect, CheckBoxChecked, cachedRegions);

				PushRegion(disabledSect, CheckBoxDisable);
				PushRegion(normalSect, CheckBoxNormal);
				PushRegion(hoverSect, CheckBoxHover);
				PushRegion(downSect, CheckBoxDown);
				PushRegion(checkedSect, CheckBoxChecked);

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

				ParseRegion(disabledSect, RadioButtonDisable, cachedRegions);
				ParseRegion(normalSect, RadioButtonNormal, cachedRegions);
				ParseRegion(hoverSect, RadioButtonHover, cachedRegions);
				ParseRegion(downSect, RadioButtonDown, cachedRegions);
				ParseRegion(checkedSect, RadioButtonChecked, cachedRegions);

				PushRegion(disabledSect, RadioButtonDisable);
				PushRegion(normalSect, RadioButtonNormal);
				PushRegion(hoverSect, RadioButtonHover);
				PushRegion(downSect, RadioButtonDown);
				PushRegion(checkedSect, RadioButtonChecked);

				cachedRegions.Clear();
			}

			{ // DropDownButton
				ConfigurationSection* dropDownSect = config->get(L"DropDownButton");

				ParseMargin(dropDownSect, DropDownButtonMargin);

				ConfigurationSection* normalSect = dropDownSect->getSection(L"Normal");
				ConfigurationSection* hoverSect = dropDownSect->getSection(L"Hover");
				ConfigurationSection* downSect = dropDownSect->getSection(L"Down");

				ParseRegion(normalSect, DropDownButtonNormal, cachedRegions);
				ParseRegion(hoverSect, DropDownButtonHover, cachedRegions);
				ParseRegion(downSect, DropDownButtonDown, cachedRegions);

				PushRegion(normalSect, DropDownButtonNormal);
				PushRegion(hoverSect, DropDownButtonHover);
				PushRegion(downSect, DropDownButtonDown);

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

				Push3Region(titleNormalSect, FormTitle);
				Push9Region(backgroundSect, FormBody);
				PushRegion(resizer, FormResizer);

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

					PushRegion(maxSect, FormCBIconMax);
					PushRegion(minSect, FormCBIconMin);
					PushRegion(closeSect, FormCBIconClose);
					PushRegion(restoreSect, FormCBIconRestore);

					cachedRegions.Clear();
				}

				ConfigurationSection* cbBtnSects[4] = 
				{
					formSect->getSection(L"MaximizeButton"),
					formSect->getSection(L"CloseButton"),
					formSect->getSection(L"MinimizeButton"),
					formSect->getSection(L"RestoreButton")
				};
				struct 
				{
					Apoc3D::Math::Rectangle& disabled;
					Apoc3D::Math::Rectangle& normal;
					Apoc3D::Math::Rectangle& hover;
					Apoc3D::Math::Rectangle& down;
				} cbBtnRegions[4] = 
				{
					{ FormCBMaxDisabled, FormCBMaxNormal, FormCBMaxHover, FormCBMaxDown },
					{ FormCBCloseDisabled, FormCBCloseNormal, FormCBCloseHover, FormCBCloseDown },
					{ FormCBMinDisabled, FormCBMinNormal, FormCBMinHover, FormCBMinDown },
					{ FormCBRestoreDisabled, FormCBRestoreNormal, FormCBRestoreHover, FormCBRestoreDown },
				};

				int32 cbbCount = sizeof(cbBtnSects) / sizeof(cbBtnSects[0]);
				for (int i=0;i<cbbCount;i++)
				{
					ConfigurationSection* cbbSect = cbBtnSects[i];
					{
						ConfigurationSection* disabledSect = cbbSect->getSection(L"Disabled");
						ConfigurationSection* normalSect = cbbSect->getSection(L"Normal");
						ConfigurationSection* hoverSect = cbbSect->getSection(L"Hover");
						ConfigurationSection* downSect = cbbSect->getSection(L"Down");

						ParseRegion(disabledSect, cbBtnRegions[i].disabled, cachedRegions);
						ParseRegion(normalSect, cbBtnRegions[i].normal, cachedRegions);
						ParseRegion(hoverSect, cbBtnRegions[i].hover, cachedRegions);
						ParseRegion(downSect, cbBtnRegions[i].down, cachedRegions);

						PushRegion(disabledSect, cbBtnRegions[i].disabled);
						PushRegion(normalSect, cbBtnRegions[i].normal);
						PushRegion(hoverSect, cbBtnRegions[i].hover);
						PushRegion(downSect, cbBtnRegions[i].down);

						cachedRegions.Clear();
					}
				}
				
				cachedRegions.Clear();
			}

			{ // ProgressBar
				ConfigurationSection* cbbSect = config->get(L"ProgressBar");

				ConfigurationSection* bgSect = cbbSect->getSection(L"Background");
				ConfigurationSection* fillSect = cbbSect->getSection(L"Filled");
				
				Parse3Region(bgSect, ProgressBarBG, cachedRegions);
				Parse3Region(fillSect, ProgressBarFilled, cachedRegions);
				
				Push3Region(bgSect, ProgressBarBG);
				Push3Region(fillSect, ProgressBarFilled);

				cachedRegions.Clear();
			}


			{ // HSlider
				ConfigurationSection* sdrSect = config->get(L"HSlider");

				ConfigurationSection* bgSect = sdrSect->getSection(L"Background");
				ConfigurationSection* fillSect = sdrSect->getSection(L"Filled");
				ConfigurationSection* handleSect = sdrSect->getSection(L"HandleNormal");

				Parse3Region(bgSect, HSilderBG, cachedRegions);
				Parse3Region(fillSect, HSilderFilled, cachedRegions);
				ParseRegion(handleSect, HSliderHandle, cachedRegions);

				Push3Region(bgSect, HSilderBG);
				Push3Region(fillSect, HSilderFilled);
				PushRegion(handleSect, HSliderHandle);

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

				PushRegion(bgSect, VScrollBarBG);
				Push3Region(cursor, VScrollBarCursor);

				cachedRegions.Clear();
				ParseRegion(backSect, VScrollBarUp, cachedRegions);
				cachedRegions.Clear();
				ParseRegion(forwardSect, VScrollBarDown, cachedRegions);

				PushRegion(backSect, VScrollBarUp);
				PushRegion(forwardSect, VScrollBarDown);

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

				PushRegion(bgSect, HScrollBarBG);
				Push3Region(cursor, HScrollBarCursor);

				cachedRegions.Clear();
				ParseRegion(backSect, HScrollBarLeft, cachedRegions);
				cachedRegions.Clear();
				ParseRegion(forwardSect, HScrollBarRight, cachedRegions);

				PushRegion(backSect, HScrollBarLeft);
				PushRegion(forwardSect, HScrollBarRight);

				cachedRegions.Clear();
			}


			{ // ListBox
				ConfigurationSection* listBox = config->get(L"ListBox");
				ListBoxFont = GetFontName(listBox->getAttribute(L"Font"));

				ParseMargin(listBox, ListBoxMargin);
				ParsePadding(listBox, ListBoxPadding);

				ConfigurationSection* normal = listBox->getSection(L"Normal");
				Parse9Region(normal, ListBoxBackground, cachedRegions);
				
				Push9Region(normal, ListBoxBackground);
				cachedRegions.Clear();
			}


			// Menu
			{
				ConfigurationSection* menuSect = config->get(L"Menu");

				ConfigurationSection* subMenuArrow = menuSect->getSection(L"SubMenuArrow");
				ConfigurationSection* hshadeSect = menuSect->getSection(L"HShade");

				ParseRegion(subMenuArrow, SubMenuArrow, cachedRegions);
				ParseRegion(hshadeSect, HShade, cachedRegions);

				PushRegion(subMenuArrow, SubMenuArrow);
				PushRegion(hshadeSect, HShade);

				cachedRegions.Clear();
			}

			delete config;

			WhitePixelTexture = device->getObjectFactory()->CreateTexture(1,1,1, TU_Static, FMT_A8R8G8B8);

			DataRectangle rect = WhitePixelTexture->Lock(0, LOCK_None);
			*(uint32*)rect.getDataPointer() = CV_White;
			WhitePixelTexture->Unlock(0);

			fl = FileSystem::getSingleton().Locate(packTexName, rule);
			SkinTexture = TextureManager::getSingleton().CreateUnmanagedInstance(device, fl, false);
		}

		StyleSkin::~StyleSkin()
		{
			delete WhitePixelTexture;
			delete SkinTexture;
		}

		Font* StyleSkin::GetFontName(const String& alias)
		{
			return alias == L"Title" ? TitleTextFont : ContentTextFont;
		}

		void StyleSkin::ParseMargin(ConfigurationSection* sect, ControlBounds& result)
		{
			int32 margins[4];
			if (sect->TryGetAttributeIntsChecked(L"Margin", margins, 4))
			{
				result.SetFromLeftTopRightBottom(margins);
			}
			else
			{
				result.SetZero();
			}
		}

		void StyleSkin::ParsePadding(ConfigurationSection* sect, ControlBounds& result)
		{
			int32 padding[4];
			if (sect->TryGetAttributeIntsChecked(L"Padding", padding, 4))
			{
				result.SetFromLeftTopRightBottom(padding);
			}
			else
			{
				result.SetZero();
			}
		}
		void StyleSkin::Parse9Region(ConfigurationSection* sect, Apoc3D::Math::Rectangle srcRects[9], HashMap<String, const Apoc3D::Math::Rectangle*>& cachedRegions)
		{
			String ref;
			if (sect->tryGetAttribute(L"RegionRef", ref))
			{
				memcpy(srcRects, cachedRegions[ref], sizeof(Apoc3D::Math::Rectangle) * 9);
				return;
			}

			FastList<int32> parts;
			StringUtils::SplitParseInts(sect->getValue(), parts, L"[], ");
			assert(parts.getCount() == 4 * 9);

			for (int i=0;i<9;i++)
			{
				Apoc3D::Math::Rectangle& rect = srcRects[i];
				
				rect.X = parts[i * 4];
				rect.Y = parts[i * 4 + 1];
				rect.Width = parts[i * 4 + 2];
				rect.Height = parts[i * 4 + 3];
			}

			cachedRegions.Add(sect->getName(), srcRects);
		}
		void StyleSkin::Parse3Region(ConfigurationSection* sect, Apoc3D::Math::Rectangle srcRects[3], HashMap<String, const Apoc3D::Math::Rectangle*>& cachedRegions)
		{
			String ref;
			if (sect->tryGetAttribute(L"RegionRef", ref))
			{
				memcpy(srcRects, cachedRegions[ref], sizeof(Apoc3D::Math::Rectangle) * 3);
				return;
			}

			FastList<int32> parts;
			StringUtils::SplitParseInts(sect->getValue(), parts, L"[], ");
			assert(parts.getCount() == 4 * 3);

			for (int i=0;i<3;i++)
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
				memcpy(&srcRect, cachedRegions[ref], sizeof(Apoc3D::Math::Rectangle));
				return;
			}

			FastList<int32> parts;
			StringUtils::SplitParseInts(sect->getValue(), parts, L"[], ");
			assert(parts.getCount() == 4);

			srcRect.X = parts[0];
			srcRect.Y = parts[1];
			srcRect.Width = parts[2];
			srcRect.Height = parts[3];

			cachedRegions.Add(sect->getName(), &srcRect);
		}


		void StyleSkin::Push9Region(ConfigurationSection* sect, Apoc3D::Math::Rectangle srcRects[9])
		{
			Point coord(0,0);
			FastList<int32> coordArr;
			if (sect->TryGetAttributeInts(L"Coord", coordArr))
			{
				assert(coordArr.getCount()==2);
				coord.X = coordArr[0];
				coord.Y = coordArr[1];

				for (int i=0;i<9;i++)
				{
					srcRects[i].X += coord.X;
					srcRects[i].Y += coord.Y;
				}
			}
		}
		void StyleSkin::Push3Region(ConfigurationSection* sect, Apoc3D::Math::Rectangle srcRects[3])
		{
			Point coord(0,0);
			FastList<int32> coordArr;
			if (sect->TryGetAttributeInts(L"Coord", coordArr))
			{
				assert(coordArr.getCount()==2);
				coord.X = coordArr[0];
				coord.Y = coordArr[1];

				for (int i=0;i<3;i++)
				{
					srcRects[i].X += coord.X;
					srcRects[i].Y += coord.Y;
				}
			}
		}
		void StyleSkin::PushRegion(ConfigurationSection* sect, Apoc3D::Math::Rectangle& srcRect)
		{
			Point coord(0,0);
			//FastList<int32> coordArr;
			int32 coordArr[2];
			if (sect->TryGetAttributeIntsChecked(L"Coord", coordArr, 2))
			{
				coord.X = coordArr[0];
				coord.Y = coordArr[1];

				srcRect.X += coord.X;
				srcRect.Y += coord.Y;
			}
		}
	}
}