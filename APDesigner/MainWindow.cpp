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

#include "MainWindow.h"


#include "Panes.h"

#include "EditorExtensionManager.h"

#include "DocumentShader/dlgAtomManager.h"
#include "DocumentShader/ShaderAtomType.h"
#include "DocumentShader/ExtensionShader.h"
#include "DocumentModel/ExtensionModel.h"
#include "DocumentModel/ExtensionMaterial.h"

#include "Document.h"
#include "TextureViewer.h"
#include "UIResources.h"

#include "CommonDialog/FileDialog.h"
#include "CommonDialog/Win32InputBox.h"
#include "BuildService/BuildService.h"

#include "apoc3d/Core/GameTime.h"
#include "apoc3d/Config/XmlConfigurationFormat.h"
#include "apoc3d/Config/ConfigurationManager.h"
#include "apoc3d/Graphics/RenderSystem/RenderWindow.h"
#include "apoc3d/Graphics/RenderSystem/RenderDevice.h"
#include "apoc3d/Graphics/RenderSystem/ObjectFactory.h"
#include "apoc3d/Graphics/RenderSystem/Sprite.h"
#include "apoc3d/Graphics/EffectSystem/EffectManager.h"
#include "apoc3d/UILib/StyleSkin.h"
#include "apoc3d/UILib/FontManager.h"
#include "apoc3d/UILib/Control.h"
#include "apoc3d/UILib/Button.h"
#include "apoc3d/UILib/Form.h"
#include "apoc3d/UILib/Menu.h"
#include "apoc3d/UILib/List.h"
#include "apoc3d/UILib/Console.h"
#include "apoc3d/Project/Project.h"

#include "apoc3d/Input/InputAPI.h"
#include "apoc3d/Vfs/FileLocateRule.h"
#include "apoc3d/Vfs/FileSystem.h"
#include "apoc3d/Vfs/Archive.h"
#include "apoc3d/Vfs/ResourceLocation.h"
#include "apoc3d/Vfs/PathUtils.h"
#include "apoc3d/Math/ColorValue.h"


using namespace Apoc3D::Graphics::EffectSystem;
using namespace Apoc3D::Input;
using namespace Apoc3D::VFS;

using namespace APDesigner::CommonDialog;

namespace APDesigner
{

	MainWindow::MainWindow(RenderWindow* wnd)
		: m_window(wnd), m_UIskin(0), m_project(0), m_currentDocument(0)
	{

	}
	MainWindow::~MainWindow()
	{
		
	}

	void MainWindow::AddDocument(Document* document)
	{
		UIRoot::Add(document->getDocumentForm());
		document->Initialize(m_device);
		document->eventDocumentActivated().bind(this, &MainWindow::Document_Activated);
		document->eventDocumentDeactivated().bind(this, &MainWindow::Document_Deactivated);
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
		ShaderAtomLibraryManager::Initialize();
		EditorExtensionManager::Initialize();
		BuildInterface::Initialize();
	}
	void MainWindow::Finalize()
	{
		EditorExtensionManager::Finalize();
		ShaderAtomLibraryManager::Finalize();
		InputAPIManager::getSingleton().FinalizeInput();
		BuildInterface::Finalize();
	}

	void MainWindow::Load()
	{
		{
			FileLocateRule rule;
			LocateCheckPoint pt;
			pt.AddPath(L"system.pak\\effects.pak");
			rule.AddCheckPoint(pt);
			FileLocation* fl = FileSystem::getSingleton().Locate(L"standard.afx", rule);
			EffectManager::getSingleton().LoadEffect(m_device, fl);
		}

		UIResources::Initialize(m_device);
		{
			FileLocateRule rule;
			LocateCheckPoint pt;
			pt.AddPath(L"system.pak\\classic_skin.pak");
			rule.AddCheckPoint(pt);
			m_UIskin = new StyleSkin(m_device, rule);

			m_UIskin->ControlFontName = L"english";
			FileLocation* fl = FileSystem::getSingleton().Locate(L"english.fnt", rule);
			FontManager::getSingleton().LoadFont(m_device, L"english", fl);
		}

		{
			FileLocation* fl = FileSystem::getSingleton().Locate(L"SysShaderLib.xml", FileLocateRule::Default);
			ShaderAtomLibraryManager::getSingleton().Load(fl);
			delete fl;
		}
		
		EditorExtensionManager::getSingleton().RegisterExtension(new ExtensionShaderNetwork(this));
		EditorExtensionManager::getSingleton().RegisterExtension(new ExtensionModel(this));
		EditorExtensionManager::getSingleton().RegisterExtension(new ExtensionMaterial(this));
		EditorExtensionManager::getSingleton().RegisterExtension(new ExtensionEffect(this));
		//m_font = FontManager::getSingleton().getFont(L"english");

		ObjectFactory* fac = m_device->getObjectFactory();
		m_sprite = fac->CreateSprite();

		LoadMenus();

		m_resourcePane = new ResourcePane(this);
		m_toolsPane = new ToolsPane(this);
		m_atomManager = new AtomManagerDialog(this);

		UIRoot::Initialize(m_device);
		//UIRoot::Add(m_form);
		m_resourcePane->Initialize(m_device);
		m_toolsPane->Initialize(m_device);
		m_atomManager->Initialize(m_device);

		m_console = new Console(m_device, m_UIskin, Point(600,100), Point(400,400));
	}
	void MainWindow::LoadMenus()
	{
		m_mainMenu = new Menu();
		m_mainMenu->SetSkin(m_UIskin);

		{
			MenuItem* pojMenu = new MenuItem(L"Project");


			SubMenu* pojSubMenu = new SubMenu(0);
			pojSubMenu->SetSkin(m_UIskin);

			MenuItem* mi=new MenuItem(L"New Project...");
			mi->event().bind(this, &MainWindow::Menu_NewProject);
			pojSubMenu->Add(mi,0);

			mi=new MenuItem(L"Open Project...");
			mi->event().bind(this, &MainWindow::Menu_OpenProject);
			pojSubMenu->Add(mi,0);

			m_savePrjMemuItem = new MenuItem(L"Save Project");
			m_savePrjMemuItem->event().bind(this, &MainWindow::Menu_SaveProject);
			pojSubMenu->Add(m_savePrjMemuItem, 0);

			//mi=new MenuItem(L"Insert...");
			//mi->event().bind(this, &MainWindow::Menu_Insert);
			//pojSubMenu->Add(mi,0);

			mi=new MenuItem(L"-");
			pojSubMenu->Add(mi,0);

			mi=new MenuItem(L"Exit");
			mi->event().bind(this, &MainWindow::Menu_Exit);
			pojSubMenu->Add(mi,0);

			m_mainMenu->Add(pojMenu,pojSubMenu);
		}
		//{
		//	MenuItem* fileMenu = new MenuItem(L"File");
		//	
		//	SubMenu* fileSubMenu = new SubMenu(0);
		//	fileSubMenu->SetSkin(m_UIskin);
		//	
		//	MenuItem* mi=new MenuItem(L"Save");
		//	//mi->event().bind(this, &MainWindow::Menu_NewProject);
		//	fileSubMenu->Add(mi,0);

		//	m_mainMenu->Add(fileMenu,fileSubMenu);
		//}
		{
			MenuItem* buildMenu = new MenuItem(L"Build");

			SubMenu* buildSubMenu = new SubMenu(0);
			buildSubMenu->SetSkin(m_UIskin);

			m_buildMemuItem = new MenuItem(L"Build All");
			m_buildMemuItem->event().bind(this, &MainWindow::Menu_BuildAll);
			buildSubMenu->Add(m_buildMemuItem, 0);

			m_mainMenu->Add(buildMenu, buildSubMenu);
		}
		{
			MenuItem* toolsMenu = new MenuItem(L"Tools");

			SubMenu* toolSubMenu = new SubMenu(nullptr);
			toolSubMenu->SetSkin(m_UIskin);


			for (EditorExtensionManager::ExtensionEnumerator e = EditorExtensionManager::getSingleton().GetEnumerator();e.MoveNext();)
			{
				EditorExtension* ext = *e.getCurrentValue();

				if (ext->SupportsIndependentEditing())
				{
					SubMenu* subMenu = new SubMenu(nullptr);
					subMenu->SetSkin(m_UIskin);

					MenuItem* mi2 = new MenuItem(L"New Document...");
					//mi2->event().bind(this, &MainWindow::Menu_ToolItem);
					mi2->UserPointer = ext;
					subMenu->Add(mi2, nullptr);

					mi2 = new MenuItem(L"Open Document...");
					mi2->event().bind(this, &MainWindow::Menu_ToolItemOpen);
					mi2->UserPointer = ext;
					subMenu->Add(mi2, nullptr);

					MenuItem* mi=new MenuItem(ext->GetName());
					//mi->event().bind(this, &MainWindow::Menu_ToolItem);
					mi->UserPointer = ext;

					toolSubMenu->Add(mi,subMenu);	
				}
			}


			MenuItem* mi=new MenuItem(L"-");
			toolSubMenu->Add(mi, nullptr);

			mi=new MenuItem(L"Shader Atom Manager");
			mi->event().bind(this, &MainWindow::Menu_Tools_AtomManager);
			toolSubMenu->Add(mi, nullptr);

			m_mainMenu->Add(toolsMenu,toolSubMenu);
		}
		{
			m_mainMenu->Initialize(m_device);
			UIRoot::setMainMenu(m_mainMenu);
		}
	}
	void MainWindow::Unload()
	{
		UIRoot::Finalize();
		delete m_UIskin;
		delete m_sprite;

		delete m_console;
		delete m_resourcePane;
		delete m_atomManager;
		UIResources::Finalize();
	}
	void MainWindow::Update(const GameTime* const time)
	{
		if (m_window->getIsActive())
			InputAPIManager::getSingleton().Update(time);

		EffectManager::getSingleton().Update(time);

		//m_pane->Update(time);
		//m_form->Update(time);
		UIRoot::Update(time);
		m_console->Update(time);
		m_resourcePane->Update(time);
		m_toolsPane->Update(time);
		m_atomManager->Update(time);

		list<Document*> recycleList;

		for (int i=0;i<m_documentList.getCount();i++)
		{
			m_documentList[i]->Update(time);
			if (!m_documentList[i]->getDocumentForm()->Visible)
			{
				recycleList.push_back(m_documentList[i]);
			}
		}
		for (list<Document*>::iterator iter = recycleList.begin();iter!=recycleList.end();iter++)
		{
			m_documentList.Remove(*iter);
			delete *iter;
		}

		m_buildMemuItem->Enabled = !!m_project;
		m_savePrjMemuItem->Enabled = !!m_project;

		if (BuildInterface::getSingleton().MainThreadUpdate(time))
		{
			UpdateProjectEffect();
		}
	}
	void MainWindow::Draw(const GameTime* const time)
	{
		m_device->Clear(CLEAR_ColorAndDepth, CV_DarkGray, 1, 0);
		
		m_device->BeginFrame();

		//m_sprite = 0;

		//m_sprite->Begin(true);
		//m_font->DrawString(m_sprite, L"Apoc3D Designer\nfdsfds!!", 50,50, CV_White);
		//m_sprite->Draw(m_UIskin->ButtonTexture, 50,50, CV_White);
		UIRoot::Draw();
		//m_pane->Draw(m_sprite);

		//m_sprite->End();

		for (int i=0;i<m_documentList.getCount();i++)
		{
			m_documentList[i]->Render();
		}

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

	void MainWindow::OpenProject(const String& path)
	{
		if (!m_project)
		{
			m_project = new Project();
			m_projectFilePath = path;
			FileLocation* fl = new FileLocation(path);
			Configuration* conf = ConfigurationManager::getSingleton().CreateInstance(fl);// XMLConfigurationFormat::Instance.Load(fl);

			m_project->Parse(conf->get(L"Project"));
			delete conf;
			delete fl;

			m_resourcePane->UpdateToNewProject(m_project);
			m_project->setBasePath(PathUtils::GetDirectory(path));

			FileSystem::getSingleton().AddWrokingDirectory(m_project->getOutputPath());

			// project has default texture dirs
			// this directory is added to the FileLocateRule by APDesigner, so that
			// textures can be correctly used
			if (m_project->getTexturePath().size())
			{
				LocateCheckPoint cp;
				cp.AddPath(m_project->getTexturePath());
				FileLocateRule::Textures.AddCheckPoint(cp);
				LogManager::getSingleton().Write(LOG_System, L"Adding texture dir: '" + m_project->getTexturePath() + L"'");
			}
			if (m_project->getMaterialPath().size())
			{
				LocateCheckPoint cp;
				cp.AddPath(m_project->getMaterialPath());
				FileLocateRule::Materials.AddCheckPoint(cp);
				LogManager::getSingleton().Write(LOG_System, L"Adding material dir: '" + m_project->getMaterialPath() + L"'");
			}
			if (m_project->getOutputPath() != m_project->getBasePath())
			{
				LogManager::getSingleton().Write(LOG_System, L"This project is using an alternative output directory: '" + m_project->getOutputPath() + L"'");
			}

			// Once a project is loaded or built, the effects used will be registered/updated in the EffectSystem.
			// So the editing tools using these effect will work perfectly
			UpdateProjectEffect();
		}
	}
	void MainWindow::CloseProject()
	{
		
	}
	void MainWindow::SaveProject(const String& path)
	{
		m_project->Save(path);
	}
	void MainWindow::UpdateProjectEffect()
	{
		if (m_project)
		{
			UpdateProjectEffect(m_project->getItems());
		}
	}
	void MainWindow::UpdateProjectEffect(const FastList<ProjectItem*>& items)
	{
		for (int i=0;i<items.getCount();i++)
		{
			ProjectItem* itm = items[i];
			if (itm->getType() == PRJITEM_Effect)
			{
				if (!itm->IsOutDated())
				{
					ProjectResEffect* eff = static_cast<ProjectResEffect*>(itm->getData());
					String df = PathUtils::Combine(m_project->getOutputPath(), eff->DestFile);

					LogManager::getSingleton().Write(LOG_Graphics, L"Updating effect " + eff->DestFile);

					FileLocation* fl = new FileLocation(df);
					EffectManager::getSingleton().LoadEffect(m_device, fl);
					delete fl;
				}
			}
			else if (itm->getType() == PRJITEM_Folder)
			{
				ProjectFolder* fld = static_cast<ProjectFolder*>(itm->getData());
				UpdateProjectEffect(fld->SubItems);
			}
		}
	}
	void MainWindow::Menu_CloseProject(Control* ctl)
	{

	}
	//void MainWindow::Menu_Insert(Control* ctl)
	//{
	//	if (m_project)
	//	{

	//	}
	//}
	void MainWindow::Menu_NewProject(Control* ctl)
	{
		wchar_t nameBuffer[512] = { 0 };

		if (CWin32InputBox::InputBox(L"Project Name", L"Please enter a name for your project.", nameBuffer, 512))
		{
			SaveFileDialog dlg;
			dlg.SetFilter(L"Project file(*.aproj;*.xml)\0*.aproj;*.xml\0\0");
			dlg.Title = L"Save Project";
			if (dlg.ShowDialog() == DLGRES_OK)
			{
				String path = dlg.getFilePath()[0];
				m_project = new Project();
				m_projectFilePath = path;

				m_resourcePane->UpdateToNewProject(m_project);
				m_project->setBasePath(PathUtils::GetDirectory(path));

				FileSystem::getSingleton().AddWrokingDirectory(m_project->getOutputPath());

				m_project->setTexturePath(PathUtils::Combine(path, L"textures"));

				// project has default texture dirs
				// this directory is added to the FileLocateRule by APDesigner, so that
				// textures can be correctly used
				if (m_project->getTexturePath().size())
				{
					LocateCheckPoint cp;
					cp.AddPath(m_project->getTexturePath());
					FileLocateRule::Textures.AddCheckPoint(cp);
					LogManager::getSingleton().Write(LOG_System, L"Adding texture dir: '" + m_project->getTexturePath() + L"'");
				}
				// Once a project is loaded or built, the effects used will be registered/updated in the EffectSystem.
				// So the editing tools using these effect will work perfectly
				UpdateProjectEffect();
			}
		}
		
	}
	void MainWindow::Menu_OpenProject(Control* ctl)
	{
		OpenFileDialog dlg;
		dlg.SetFilter(L"Project file(*.aproj;*.xml)\0*.aproj;*.xml\0\0");
		if (dlg.ShowDialog() == DLGRES_OK)
		{
			OpenProject(dlg.getFilePath()[0]);
		}
	}
	void MainWindow::Menu_SaveProject(Control* ctl)
	{
		if (m_project)
			SaveProject(m_projectFilePath);
	}
	void MainWindow::Menu_Exit(Control* ctl)
	{
		m_window->Exit();
	}
	void MainWindow::Menu_BuildAll(Control* ctl)
	{
		if (m_project)
		{
			LogManager::getSingleton().Write(LOG_System, String(L"Building project '") + m_project->getName() + String(L"'..."));
			BuildInterface::getSingleton().AddBuild(m_project);
			BuildInterface::getSingleton().Execute();
		}
	}
	
	void MainWindow::Menu_ToolItemOpen(Control* ctl)
	{
		SubMenu* sm = dynamic_cast<SubMenu*>(ctl);

		if (sm)
		{
			int id = sm->getHoverIndex();
			if (id!=-1)
			{
				MenuItem* item = sm->getItems()[id]; 

				EditorExtension* eext = static_cast<EditorExtension*>(item->UserPointer);

				String name = eext->GetName();
				std::vector<String> fexts = eext->GetFileExtensions();

				OpenFileDialog dlg;

				// make filter
				wchar_t filter[512];
				memset(filter, 0, sizeof(filter));

					
				String right;
				//*.a;*.b
				for (size_t i=0;i<fexts.size();i++)
				{
					right.append(L"*");
					right.append(fexts[i]);
					if (i+1<fexts.size())
					{
						right.append(L";");
					}
				}

				String left = name;
				left.append(L"Files (");
				left.append(right);
				left.append(L")");
				memcpy(filter, left.c_str(), sizeof(wchar_t)*left.length());
				filter[left.length()] = 0;

				memcpy(filter+(left.length()+1),right.c_str(), sizeof(wchar_t)*right.length());

				dlg.SetFilter(filter);

				if (dlg.ShowDialog() == DLGRES_OK)
				{
					Document* doc = eext->DirectOpen(dlg.getFilePath()[0]);
					doc->LoadRes();
					this->AddDocument(doc);
				}
				
			}
		}
	}
	void MainWindow::Menu_Tools_AtomManager(Control* ctl)
	{
		m_atomManager->Show();
	}
	void MainWindow::Document_Activated(Document* doc)
	{
		m_currentDocument = doc;
	}

	void MainWindow::Document_Deactivated(Document* doc)
	{
		if (m_currentDocument == doc)
		{
			m_currentDocument = 0;
		}
	}

	void MainWindow::LogBuildMessages()
	{
		//bool hasWarning = false;
		//bool hasFailed = false;
		//String allWarningLines;
		//String allErrorLines;
		//for (size_t i=0;i<BuildInterface::LastResult.size();i++)
		//{
		//	bool isError = BuildInterface::LastResult[i].find(L"[Error]") != String::npos;
		//	bool isWarning = BuildInterface::LastResult[i].find(L"[Warning]") != String::npos;

		//	LogMessageLevel level = LOGLVL_Infomation;
		//	if (isWarning)
		//	{
		//		level = LOGLVL_Warning;
		//		allWarningLines.append(BuildInterface::LastResult[i]);
		//		allWarningLines.append(L"\n");
		//	}
		//	if (isError)
		//	{
		//		level = LOGLVL_Error;
		//		allErrorLines.append(BuildInterface::LastResult[i]);
		//		allErrorLines.append(L"\n");
		//	}
		//	LogManager::getSingleton().Write(LOG_System, BuildInterface::LastResult[i], level);

		//	hasFailed |= isError;
		//	hasWarning |= hasWarning;
		//}

		//if (hasFailed || hasWarning)
		//{
		//	String msg = hasFailed ? L"There are build errors." : L"There are some potential build issues.";
		//	msg.append(L"\n");
		//	msg.append(L"\n");
		//	msg.append(allErrorLines);
		//	msg.append(L"\n");
		//	msg.append(allWarningLines);

		//	ShowMessageBox(msg, L"Build Result", hasFailed, hasWarning, false);
		//}
	}

	Form* MainWindow::getToolsPane() const
	{
		return m_toolsPane->getPane();
	}
}