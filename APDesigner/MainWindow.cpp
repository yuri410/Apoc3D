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

#include "MainWindow.h"

#include "Core/GameTime.h"

#include "Graphics/RenderSystem/RenderWindow.h"
#include "Graphics/RenderSystem/RenderDevice.h"
#include "Graphics/RenderSystem/ObjectFactory.h"
#include "Graphics/RenderSystem/Sprite.h"
#include "UILib/StyleSkin.h"
#include "UILib/FontManager.h"
#include "UILib/Control.h"
#include "UILib/Button.h"
#include "UILib/Form.h"

#include "Input/InputAPI.h"
#include "Vfs/FileLocateRule.h"
#include "Vfs/FileSystem.h"
#include "Vfs/Archive.h"
#include "Vfs/ResourceLocation.h"
#include "Math/ColorValue.h"

using namespace Apoc3D::Input;
using namespace Apoc3D::VFS;

namespace APDesigner
{
	MainWindow::MainWindow(RenderWindow* wnd)
		: m_window(wnd), m_UIskin(0)
	{

	}

	void MainWindow::Initialize()
	{
		m_device = m_window->getRenderDevice();
		m_window->setTitle(L"Apoc3D Designer");

	}
	void MainWindow::Finalize()
	{
		
	}

	void MainWindow::Load()
	{
		InputCreationParameters icp;
		icp.UseKeyboard = true;
		icp.UseMouse = true;

		InputAPIManager::getSingleton().InitializeInput(m_window, icp);

		{
			FileLocateRule rule;
			LocateCheckPoint pt;
			pt.AddPath(L"classic_ui.pak");
			rule.AddCheckPoint(pt);
			m_UIskin = new StyleSkin(m_device, rule);

			m_UIskin->ControlFontName = L"english";
		}

		{

			FileLocation* fl = FileSystem::getSingleton().Locate(L"english.fnt", FileLocateRule::Default);
			FontManager::getSingleton().LoadFont(m_device, L"english", fl);
		}
		m_font = FontManager::getSingleton().getFont(L"english");

		ObjectFactory* fac = m_device->getObjectFactory();
		m_sprite = fac->CreateSprite();



		m_pane = new ControlContainer();
		m_pane->Visible = true; m_pane->Enabled = true; m_pane->Position = Point(0,0); m_pane->Size = Point(400,400);

		m_btn = new Button(Point(5,5), L"UTTTest button!");
		m_btn->SetSkin(m_UIskin);


		m_form = new Form();
		m_form->SetSkin(m_UIskin);
		m_form->getControls().Add(m_btn);
		m_form->Initialize(m_device);
		//m_pane->getControls().Add(m_btn);

		UIRoot::Initialize(m_device);
		UIRoot::Add(m_form);
		//m_btn->Initialize(m_device);
		//m_pane->Initialize(m_device);
	}
	void MainWindow::Unload()
	{
		delete m_UIskin;
		delete m_sprite;
		
		delete m_btn;
		delete m_pane;
		delete m_form;
		UIRoot::Finalize();
	}
	void MainWindow::Update(const GameTime* const time)
	{
		InputAPIManager::getSingleton().Update(time);

		//m_pane->Update(time);
		//m_form->Update(time);
		UIRoot::Update(time);
	}
	void MainWindow::Draw(const GameTime* const time)
	{
		m_device->Clear(CLEAR_Target, CV_BlanchedAlmond, 1, 0);

		m_device->BeginFrame();

		
		//m_sprite->Begin(true);
		//m_font->DrawString(m_sprite, L"Apoc3D Designer\nfdsfds!!", 50,50, CV_White);
		//m_sprite->Draw(m_UIskin->ButtonTexture, 50,50, CV_White);
		UIRoot::Draw();
		//m_pane->Draw(m_sprite);

		//m_sprite->End();

		m_device->EndFrame();

	}

	void MainWindow::OnFrameStart()
	{

	}
	void MainWindow::OnFrameEnd()
	{

	}
}