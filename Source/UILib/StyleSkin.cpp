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

#include "StyleSkin.h"
#include "Graphics/RenderSystem/Texture.h"
#include "Graphics/RenderSystem/ObjectFactory.h"
#include "Graphics/RenderSystem/RenderDevice.h"
#include "Graphics/TextureManager.h"
#include "Vfs/FileSystem.h"
#include "Vfs/FileLocateRule.h"
#include "Math/ColorValue.h"

using namespace Apoc3D::Graphics;
using namespace Apoc3D::Math;
using namespace Apoc3D::Graphics::RenderSystem;
using namespace Apoc3D::VFS;

namespace Apoc3D
{
	namespace UI
	{
		StyleSkin::StyleSkin(RenderDevice* device, const FileLocateRule& rule)
			: ForeColor(CV_Black), BackColor(CV_LightGray), FormControlButtonColor(CV_Silver)
		{
			FileLocation* fl = FileSystem::getSingleton().Locate(L"ctl_btn_default.tex", rule);
			ButtonTexture = TextureManager::getSingleton().CreateUnmanagedInstance(device, fl, false);
			
			
			BtnSrcRect[0] = Apoc3D::Math::Rectangle(0, 0, ButtonTexture->getWidth() - 1, ButtonTexture->getHeight());
			BtnSrcRect[1] = Apoc3D::Math::Rectangle(ButtonTexture->getWidth() - 1, 0, 1, ButtonTexture->getHeight());
			BtnSrcRect[2] = Apoc3D::Math::Rectangle(ButtonTexture->getWidth() - 1, 0, -ButtonTexture->getWidth() + 1, ButtonTexture->getHeight());

			BtnHozPadding = 5;
			BtnVertPadding = 20;

			BtnDimColor = CV_DarkGray;
			BtnHighLightColor = CV_Gray;
			BtnTextDimColor = CV_Black;
			BtnTextHighLightColor = CV_Black;


			BtnRowSrcRect[0] = Apoc3D::Math::Rectangle(0,0,ButtonTexture->getWidth(),ButtonTexture->getHeight());
			BtnRowSrcRect[1] = Apoc3D::Math::Rectangle(ButtonTexture->getWidth()-1,0,1,ButtonTexture->getHeight());
			BtnRowSeparator = Apoc3D::Math::Rectangle(0,0,2,ButtonTexture->getHeight());


			WhitePixelTexture = device->getObjectFactory()->CreateTexture(1,1,1, TU_Static, FMT_A8R8G8B8);

			DataRectangle rect = WhitePixelTexture->Lock(0, LOCK_None);
			*(uint*)rect.getDataPointer() = PACK_COLOR(0xff,0xff,0xff,0xff);
			WhitePixelTexture->Unlock(0);


			fl = FileSystem::getSingleton().Locate(L"ctl_btn_close.tex", rule);
			FormCloseButton = TextureManager::getSingleton().CreateUnmanagedInstance(device, fl, false);
			fl = FileSystem::getSingleton().Locate(L"ctl_btn_maximize.tex", rule);
			FormMaximizeButton = TextureManager::getSingleton().CreateUnmanagedInstance(device, fl, false);
			fl = FileSystem::getSingleton().Locate(L"ctl_btn_minimize.tex", rule);
			FormMinimizeButton = TextureManager::getSingleton().CreateUnmanagedInstance(device, fl, false);
			fl = FileSystem::getSingleton().Locate(L"ctl_btn_restore.tex", rule);
			FormRestoreButton = TextureManager::getSingleton().CreateUnmanagedInstance(device, fl, false);

			fl = FileSystem::getSingleton().Locate(L"ctl_arrow.tex", rule);
			SubMenuArrowTexture = TextureManager::getSingleton().CreateUnmanagedInstance(device, fl, false);

			fl = FileSystem::getSingleton().Locate(L"ctl_frm_default_upperleft.tex", rule);
			FormBorderTexture[0] = TextureManager::getSingleton().CreateUnmanagedInstance(device, fl, false);
			fl = FileSystem::getSingleton().Locate(L"ctl_frm_default_uppercenter.tex", rule);
			FormBorderTexture[1] = TextureManager::getSingleton().CreateUnmanagedInstance(device, fl, false);
			fl = FileSystem::getSingleton().Locate(L"ctl_frm_default_upperright.tex", rule);
			FormBorderTexture[2] = TextureManager::getSingleton().CreateUnmanagedInstance(device, fl, false);
			fl = FileSystem::getSingleton().Locate(L"ctl_frm_default_midleft.tex", rule);
			FormBorderTexture[3] = TextureManager::getSingleton().CreateUnmanagedInstance(device, fl, false);
			fl = FileSystem::getSingleton().Locate(L"ctl_frm_default_midcenter.tex", rule);
			FormBorderTexture[4] = TextureManager::getSingleton().CreateUnmanagedInstance(device, fl, false);
			fl = FileSystem::getSingleton().Locate(L"ctl_frm_default_midright.tex", rule);
			FormBorderTexture[5] = TextureManager::getSingleton().CreateUnmanagedInstance(device, fl, false);
			fl = FileSystem::getSingleton().Locate(L"ctl_frm_default_lowerleft.tex", rule);
			FormBorderTexture[6] = TextureManager::getSingleton().CreateUnmanagedInstance(device, fl, false);
			fl = FileSystem::getSingleton().Locate(L"ctl_frm_default_lowercenter.tex", rule);
			FormBorderTexture[7] = TextureManager::getSingleton().CreateUnmanagedInstance(device, fl, false);
			fl = FileSystem::getSingleton().Locate(L"ctl_frm_default_lowerright.tex", rule);
			FormBorderTexture[8] = TextureManager::getSingleton().CreateUnmanagedInstance(device, fl, false);
			fl = FileSystem::getSingleton().Locate(L"ctl_frm_default_lowerright_resize.tex", rule);
			FormBorderTexture[9] = TextureManager::getSingleton().CreateUnmanagedInstance(device, fl, false);

			fl = FileSystem::getSingleton().Locate(L"ctl_radiobtn_checked.tex", rule);
			RadioBtnChecked = TextureManager::getSingleton().CreateUnmanagedInstance(device, fl, false);
			fl = FileSystem::getSingleton().Locate(L"ctl_radiobtn_unchecked.tex", rule);
			RadioBtnUnchecked = TextureManager::getSingleton().CreateUnmanagedInstance(device, fl, false);


			fl = FileSystem::getSingleton().Locate(L"ctl_scrollbar_hscrollbar_button.tex", rule);
			HScrollBar_Button = TextureManager::getSingleton().CreateUnmanagedInstance(device, fl, false);
			fl = FileSystem::getSingleton().Locate(L"ctl_scrollbar_hscrollbar_back.tex", rule);
			HScrollBar_Back = TextureManager::getSingleton().CreateUnmanagedInstance(device, fl, false);
			fl = FileSystem::getSingleton().Locate(L"ctl_scrollbar_hscrollbar_cursor.tex", rule);
			HScrollBar_Cursor = TextureManager::getSingleton().CreateUnmanagedInstance(device, fl, false);

			fl = FileSystem::getSingleton().Locate(L"ctl_scrollbar_vscrollbar_button.tex", rule);
			VScrollBar_Button = TextureManager::getSingleton().CreateUnmanagedInstance(device, fl, false);
			fl = FileSystem::getSingleton().Locate(L"ctl_scrollbar_vscrollbar_back.tex", rule);
			VScrollBar_Back = TextureManager::getSingleton().CreateUnmanagedInstance(device, fl, false);
			fl = FileSystem::getSingleton().Locate(L"ctl_scrollbar_vscrollbar_cursor.tex", rule);
			VScrollBar_Cursor = TextureManager::getSingleton().CreateUnmanagedInstance(device, fl, false);

			HSCursorLeft = Apoc3D::Math::Rectangle(
				0,0,
				3, HScrollBar_Cursor->getHeight());
			HSCursorRight = Apoc3D::Math::Rectangle(
				HScrollBar_Cursor->getWidth()-3,0,
				3, HScrollBar_Cursor->getHeight());
			HSCursorMiddle = Apoc3D::Math::Rectangle(
				3,0,
				1, HScrollBar_Cursor->getHeight());

			
			VSCursorTop = Apoc3D::Math::Rectangle(
				0,0,
				VScrollBar_Cursor->getWidth(), 3);
			VSCursorBottom = Apoc3D::Math::Rectangle(
				0, VScrollBar_Cursor->getHeight()-3,
				VScrollBar_Cursor->getWidth(), 3);
			VSCursorMiddle = Apoc3D::Math::Rectangle(
				0,3,
				HScrollBar_Cursor->getWidth(),1);

		}

		StyleSkin::~StyleSkin()
		{
			delete ButtonTexture;
			delete WhitePixelTexture;

			delete FormCloseButton;
			delete FormMaximizeButton;
			delete FormMinimizeButton;
			delete FormRestoreButton;

			for (int i=0;i<10;i++)
				delete FormBorderTexture[i];
			delete SubMenuArrowTexture;

			delete RadioBtnChecked;
			delete RadioBtnUnchecked;

			delete HScrollBar_Back;
			delete HScrollBar_Button;
			delete HScrollBar_Cursor;


			delete VScrollBar_Back;
			delete VScrollBar_Button;
			delete VScrollBar_Cursor;
		}
	}
}