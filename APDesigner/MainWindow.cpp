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
#include "UILib/Menu.h"
#include "UILib/List.h"

#include "Panes.h"

#include "Input/InputAPI.h"
#include "Vfs/FileLocateRule.h"
#include "Vfs/FileSystem.h"
#include "Vfs/Archive.h"
#include "Vfs/ResourceLocation.h"
#include "Math/ColorValue.h"

#include "Document.h"
#include "ModelDocument.h"
#include "TextureViewer.h"

using namespace Apoc3D::Input;
using namespace Apoc3D::VFS;

namespace APDesigner
{

	MainWindow::MainWindow(RenderWindow* wnd)
		: m_window(wnd), m_UIskin(0), m_project(0)
	{

	}

	void MainWindow::AddDocument(Document* document)
	{
		UIRoot::Add(document->getDocumentForm());
		document->Initialize(m_device);
		m_documentList.Add(document);
	}

	void MainWindow::Initialize()
	{
		m_device = m_window->getRenderDevice();
		m_window->setTitle(L"Apoc3D Designer");
		InputCreationParameters icp;
		icp.UseKeyboard = true;
		icp.UseMouse = true;

		InputAPIManager::getSingleton().InitializeInput(m_window, icp);

	}
	void MainWindow::Finalize()
	{
		InputAPIManager::getSingleton().Finalize();
	}

	void MainWindow::Load()
	{
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
		//m_font = FontManager::getSingleton().getFont(L"english");

		ObjectFactory* fac = m_device->getObjectFactory();
		m_sprite = fac->CreateSprite();



		//m_pane = new ControlContainer();
		//m_pane->Visible = true; m_pane->Enabled = true; m_pane->Position = Point(0,0); m_pane->Size = Point(400,400);

		//m_btn = new Button(Point(25,25), L"UTTTest button!");
		//m_btn->SetSkin(m_UIskin);

		
		

		//m_form = new Form(FBS_Pane);
		//m_form->SetSkin(m_UIskin);
		//m_form->getControls().Add(m_btn);
		//m_form->getControls().Add(m_mainMenu);
		
		//m_form->Size = Point(800,600);
		

		m_mainMenu = new Menu();
		m_mainMenu->SetSkin(m_UIskin);
		//m_form->setMenu(m_mainMenu);

		
		{
			MenuItem* ttt = new MenuItem(L"Test Menu");

			m_mainMenu->Add(ttt,0);

			MenuItem* tt2 = new MenuItem(L"Menu 2");
			SubMenu* sb = new SubMenu(0);
			sb->SetSkin(m_UIskin);
			sb->Add(new MenuItem(L"Item1"),0);
			sb->Add(new MenuItem(L"Item2"),0);

			m_mainMenu->Add(tt2, sb);

			m_mainMenu->Initialize(m_device);
			UIRoot::setMainMenu(m_mainMenu);
		}

		//{
		//	List<String> items;
		//	items.Add(L"I1");
		//	items.Add(L"I1sa");
		//	items.Add(L"dasad asdaA");
		//	items.Add(L"Server Running Time: 24/7 (all days, all weeks) Why is the server called cupcake? ");
		//	items.Add(L"Junk0");
		//	items.Add(L"Junk1");
		//	items.Add(L"Junk2");
		//	items.Add(L"Junk3");
		//	items.Add(L"Junk4");
		//	items.Add(L"Junk0");
		//	items.Add(L"Junk1");
		//	items.Add(L"Junk2");
		//	items.Add(L"Junk3");
		//	items.Add(L"Junk4");
		//	items.Add(L"Junk0");
		//	items.Add(L"Junk1");
		//	items.Add(L"Junk2");
		//	items.Add(L"Junk3");
		//	items.Add(L"Junk4");

		//	ListBox* listbox = new ListBox(Point(50,50), 300, 300, items);
		//	listbox->SetSkin(m_UIskin);
		//	listbox->setUseHorizontalScrollbar(true);
		//	m_form->getControls().Add(listbox);
		//}
		//{
		//	TreeView* treeview = new TreeView(Point(450, 50), 300, 300);
		//	treeview->SetSkin(m_UIskin);
		//	treeview->setUseHorizontalScrollbar(true);

		//	TreeViewNode* node1 = new TreeViewNode(L"Root");
		//	treeview->getNodes().Add(node1);

		//	TreeViewNode* node2 = new TreeViewNode(L"Root2");
		//	treeview->getNodes().Add(node2);
		//	TreeViewNode* node3 = new TreeViewNode(L"Root3 the long one. Project Hosting will be READ-ONLY Tuesday at 9:00am CDT for brief maintenance.");
		//	treeview->getNodes().Add(node3);

		//	node2->getNodes().Add(new TreeViewNode(L"SubNode1"));
		//	node2->getNodes().Add(new TreeViewNode(L"SubNode2"));
		//	node2->getNodes().Add(new TreeViewNode(L"SubNode3"));
		//	node2->getNodes().Add(new TreeViewNode(L"SubNode4"));
		//	node2->getNodes().Add(new TreeViewNode(L"SubNode5"));
		//	node2->getNodes().Add(new TreeViewNode(L"SubNode6"));
		//	node2->getNodes().Add(new TreeViewNode(L"SubNode7"));
		//	node2->getNodes().Add(new TreeViewNode(L"SubNode8"));
		//	node2->getNodes().Add(new TreeViewNode(L"SubNode9"));
		//	node2->getNodes().Add(new TreeViewNode(L"SubNode10"));
		//	node2->getNodes().Add(new TreeViewNode(L"SubNode11"));
		//	node2->getNodes().Add(new TreeViewNode(L"SubNode12"));

		//	node3->getNodes().Add(new TreeViewNode(L"SubNode1"));
		//	node3->getNodes().Add(new TreeViewNode(L"SubNode2"));
		//	node3->Expand();


		//	m_form->getControls().Add(treeview);
		//}
		//m_form->Initialize(m_device);
		//m_form->Text = L"Form 1. abcdefg.hijklmn.opq rst.uvw xyz.";
		//m_form->setTitle(m_form->Text);
		//m_pane->getControls().Add(m_btn);

		m_resourcePane = new ResourcePane(this);

		UIRoot::Initialize(m_device);
		//UIRoot::Add(m_form);
		m_resourcePane->Initialize(m_device);
		//m_form->Show();

		//m_btn->Initialize(m_device);
		//m_pane->Initialize(m_device);
		AddDocument(new ModelDocument(this));
	}
	void MainWindow::Unload()
	{
		delete m_UIskin;
		delete m_sprite;
		
		//delete m_btn;
		//delete m_pane;
		//delete m_form;
		UIRoot::Finalize();

		delete m_resourcePane;
	}
	void MainWindow::Update(const GameTime* const time)
	{
		InputAPIManager::getSingleton().Update(time);

		//m_pane->Update(time);
		//m_form->Update(time);
		UIRoot::Update(time);

		m_resourcePane->Update(time);

		for (int i=0;i<m_documentList.getCount();i++)
		{
			m_documentList[i]->Update(time);
		}
	}
	void MainWindow::Draw(const GameTime* const time)
	{
		m_device->Clear(CLEAR_Target, CV_BlanchedAlmond, 1, 0);
		
		m_device->BeginFrame();

		//m_sprite = 0;

		//m_sprite->Begin(true);
		//m_font->DrawString(m_sprite, L"Apoc3D Designer\nfdsfds!!", 50,50, CV_White);
		//m_sprite->Draw(m_UIskin->ButtonTexture, 50,50, CV_White);
		UIRoot::Draw();
		//m_pane->Draw(m_sprite);

		//m_sprite->End();

		m_device->EndFrame();

		Apoc3D::Math::Rectangle rect = UIRoot::GetUIArea(m_device);
		if (m_lastSize.X != rect.Width || m_lastSize.Y != rect.Height)
		{
			m_lastSize.X = rect.Width;
			m_lastSize.Y = rect.Height;

			

		}
		

	}

	void MainWindow::OnFrameStart()
	{

	}
	void MainWindow::OnFrameEnd()
	{

	}
}