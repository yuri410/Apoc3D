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

#include "IDEConfig.h"

#include "Panes.h"

#include "EditorExtensionManager.h"

#include "DocumentShader/dlgAtomManager.h"
#include "DocumentShader/ShaderAtomType.h"
#include "DocumentShader/ExtensionShader.h"
#include "DocumentModel/ExtensionModel.h"
#include "DocumentModel/ExtensionMaterial.h"
#include "DocumentUI/ExtensionUI.h"
#include "DocumentFont/ExtensionFont.h"

#include "Document.h"
#include "TextureViewer.h"
#include "UIResources.h"

#include "CommonDialog/FileDialog.h"
#include "CommonDialog/Win32InputBox.h"
#include "BuildService/BuildService.h"

#include <chrono>

using namespace APDesigner::CommonDialog;

namespace APDesigner
{

	MainWindow::MainWindow(RenderWindow* wnd)
		: m_window(wnd)
	{
		m_standardWorkdingDirCount = FileSystem::getSingleton().getNumWorkingDirectories();
		m_standardTextureLoc = FileLocateRule::Textures;
		m_standardMaterialLoc = FileLocateRule::Materials;
	}
	MainWindow::~MainWindow()
	{
		
	}

	void MainWindow::AddDocument(Document* document)
	{
		SystemUI::Add(document->getDocumentForm());
		document->Initialize(m_device);
		document->eventDocumentActivated.Bind(this, &MainWindow::Document_Activated);
		document->eventDocumentDeactivated.Bind(this, &MainWindow::Document_Deactivated);

		m_documentList.Add(document);
	}

	void MainWindow::Initialize()
	{
		m_device = m_window->getRenderDevice();
		
		UpdateWindowTitle();

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

		cfFlushConfig();
	}

	void MainWindow::Load()
	{
		cfInitializeConfig();

		{
			FileLocateRule rule = { { L"system.pak\\effects.pak" } };
			FileLocation fl = FileSystem::getSingleton().Locate(L"standard.afx", rule);
			EffectManager::getSingleton().LoadAutomaticEffect(m_device, fl);
		}

		UIResources::Initialize(m_device);
		{
			FileLocateRule rule = { { L"system.pak\\skin.pak" } };
			
			m_UIskin = new StyleSkin(m_device, rule);
		}

		{
			FileLocation fl = FileSystem::getSingleton().Locate(L"SysShaderLib.xml", FileLocateRule::Default);
			ShaderAtomLibraryManager::getSingleton().Load(fl);
		}

		EditorExtensionManager::getSingleton().RegisterExtension(new ExtensionShaderNetwork(this));
		EditorExtensionManager::getSingleton().RegisterExtension(new ExtensionModel(this));
		EditorExtensionManager::getSingleton().RegisterExtension(new ExtensionMaterial(this));
		EditorExtensionManager::getSingleton().RegisterExtension(new ExtensionEffect(this));
		EditorExtensionManager::getSingleton().RegisterExtension(new ExtensionUI(this));
		EditorExtensionManager::getSingleton().RegisterExtension(new ExtensionFont(this));
		//m_font = FontManager::getSingleton().getFont(L"english");

		ObjectFactory* fac = m_device->getObjectFactory();
		m_sprite = fac->CreateSprite();

		LoadMenus();

		m_resourcePane = new ResourcePane(this);
		m_toolsPane = new ToolsPane(this);
		m_atomManager = new AtomManagerDialog(this);

		SystemUI::Initialize(m_device);
		//UIRoot::Add(m_form);
		m_resourcePane->Initialize(m_device);
		m_toolsPane->Initialize(m_device);
		m_atomManager->Initialize(m_device);

		m_console = new Console(m_device, m_UIskin, Point(600,100), Point(400,400));
	}
	void MainWindow::LoadMenus()
	{
		m_mainMenu = new MenuBar(m_UIskin);

		{
			MenuItemSetupInfo buildMenuInfo =
			{
				L"Project",
				{
					{ L"New Project...", { this, &MainWindow::Menu_NewProject } },
					{ L"Open Project...", { this, &MainWindow::Menu_OpenProject } },
					{ L"Save Project", { this, &MainWindow::Menu_SaveProject }, &m_savePrjMemuItem },
					{ L"Recent Projects", nullptr, &m_recentPrjSubMenu, true },

					{ L"-" },
					{ L"Exit", { this, &MainWindow::Menu_Exit } },
				}
			};

			m_mainMenu->Add(buildMenuInfo, m_UIskin, nullptr);
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
			MenuItemSetupInfo buildMenuInfo =
			{
				L"Build",
				{
					{ L"QuickBuild", nullptr, &m_quickbuildSubMenu, true },
					{ L"-" },
					{ L"Build All", { this, &MainWindow::Menu_BuildAll }, &m_buildMemuItem },
				}
			};

			m_mainMenu->Add(buildMenuInfo, m_UIskin, nullptr);
		}
		{
			MenuItem* toolsMenu = new MenuItem(L"Tools");

			SubMenu* toolSubMenu = new SubMenu(m_UIskin, nullptr);


			for (EditorExtensionManager::ExtensionEnumerator e = EditorExtensionManager::getSingleton().GetEnumerator();e.MoveNext();)
			{
				EditorExtension* ext = e.getCurrentValue();

				if (ext->SupportsIndependentEditing())
				{
					SubMenu* subMenu = new SubMenu(m_UIskin, nullptr);

					MenuItem* mi2 = new MenuItem(L"New Document...");
					//mi2->event.bind(this, &MainWindow::Menu_ToolItem);
					mi2->UserPointer = ext;
					subMenu->Add(mi2, nullptr);

					mi2 = new MenuItem(L"Open Document...");
					mi2->event.Bind(this, &MainWindow::Menu_ToolItemOpen);
					mi2->UserPointer = ext;
					subMenu->Add(mi2, nullptr);

					MenuItem* mi=new MenuItem(ext->GetName());
					//mi->event.bind(this, &MainWindow::Menu_ToolItem);
					mi->UserPointer = ext;

					toolSubMenu->Add(mi,subMenu);	
				}
			}


			MenuItem* mi=new MenuItem(L"-");
			toolSubMenu->Add(mi, nullptr);

			mi=new MenuItem(L"Shader Atom Manager");
			mi->event.Bind(this, &MainWindow::Menu_Tools_AtomManager);
			toolSubMenu->Add(mi, nullptr);

			m_mainMenu->Add(toolsMenu,toolSubMenu);
		}
		{
			SystemUI::MainMenu = m_mainMenu;
		}

		UpdateRecentProjects();
	}
	void MainWindow::Unload()
	{
		SystemUI::Finalize();
		delete m_UIskin;
		delete m_sprite;

		delete m_console;
		delete m_resourcePane;
		delete m_atomManager;
		UIResources::Finalize();
	}
	void MainWindow::Update(const GameTime* time)
	{
		if (m_window->getIsActive())
			InputAPIManager::getSingleton().Update(time);

		EffectManager::getSingleton().Update(time);

		//m_pane->Update(time);
		//m_form->Update(time);
		SystemUI::Update(time);
		m_console->Update(time);
		m_resourcePane->Update(time);
		m_toolsPane->Update(time);
		m_atomManager->Update(time);

		LinkedList<Document*> recycleList;

		for (int i = 0; i < m_documentList.getCount(); i++)
		{
			m_documentList[i]->Update(time);
			if (!m_documentList[i]->getDocumentForm()->Visible)
			{
				recycleList.PushBack(m_documentList[i]);
			}
		}
		for (Document* doc : recycleList)
		{
			if (doc == m_currentDocument)
			{
				m_currentDocument = nullptr;
			}
			m_documentList.Remove(doc);
			delete doc;
		}

		m_buildMemuItem->Enabled = !!m_project;
		m_savePrjMemuItem->Enabled = !!m_project;

		if (BuildInterface::getSingleton().MainThreadUpdate(time, nullptr))
		{
			UpdateProjectEffect();
		}
	}
	void MainWindow::Draw(const GameTime* time)
	{
		m_device->Clear(CLEAR_ColorAndDepth, m_UIskin->MIDBackgroundColor, 1, 0);
		
		m_device->BeginFrame();

		//m_sprite = 0;

		//m_sprite->Begin(true);
		//m_font->DrawString(m_sprite, L"Apoc3D Designer\nfdsfds!!", 50,50, CV_White);
		//m_sprite->Draw(m_UIskin->ButtonTexture, 50,50, CV_White);
		SystemUI::Draw();
		//m_pane->Draw(m_sprite);

		//m_sprite->End();

		for (int i = 0; i < m_documentList.getCount(); i++)
		{
			m_documentList[i]->Render();
		}

		m_device->EndFrame();

		Apoc3D::Math::Rectangle rect = SystemUI::GetUIArea(m_device);
		if (m_lastSize != rect.getSize())
		{
			m_lastSize = rect.getSize();
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
		if (m_project)
			CloseProject();

		if (m_project == nullptr)
		{
			m_project = new Project();
			m_projectFilePath = path;
			
			Configuration* conf = ConfigurationManager::getSingleton().CreateInstance(FileLocation(path));// XMLConfigurationFormat::Instance.Load(fl);

			m_project->Parse(conf->get(L"Project"));
			delete conf;

			UpdateWindowTitle();
			
			m_resourcePane->UpdateToNewProject(m_project);
			m_project->SetPath(PathUtils::GetDirectory(path), nullptr);

			SetProjectWorkingDir(m_project);


			// Once a project is loaded or built, the effects used will be registered/updated in the EffectSystem.
			// So the editing tools using these effect will work perfectly
			UpdateProjectEffect();
			RefreshMaterialList();

			cfAddRecentProject(m_project->getName(), path);
			UpdateRecentProjects();
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
	void MainWindow::UpdateProjectEffect(const List<ProjectItem*>& items)
	{
		for (int i = 0; i < items.getCount(); i++)
		{
			ProjectItem* itm = items[i];
			if (itm->getType() == ProjectItemType::Effect)
			{
				if (!itm->IsOutDated())
				{
					ProjectResEffect* eff = static_cast<ProjectResEffect*>(itm->getData());
					String df = eff->GetAbsoluteDestinationPath(eff->DestFile);

					LogManager::getSingleton().Write(LOG_Graphics, L"Updating effect " + eff->DestFile);

					EffectManager::getSingleton().LoadAutomaticEffect(m_device, FileLocation(df));
				}
			}
			else if (itm->getType() == ProjectItemType::Folder)
			{
				ProjectFolder* fld = static_cast<ProjectFolder*>(itm->getData());
				UpdateProjectEffect(fld->SubItems);
			}
		}
	}
	void MainWindow::UpdateWindowTitle()
	{
		String newTitle = L"Apoc3D IDE";
		if (m_project)
		{
			newTitle.append(L" - ");
			newTitle.append(m_project->getName());
		}
		m_window->setTitle(newTitle);
	}
	void MainWindow::UpdateRecentProjects()
	{
		m_recentPrjSubMenu->Clear();
		m_quickbuildSubMenu->Clear();

		const Queue<std::pair<String, String>>& recents = cfGetRecentProjects();

		for (int i = 0; i < recents.getCount(); i++)
		{
			const std::pair<String, String>& p = recents.GetElement(i);
			String fileName = PathUtils::GetFileName(p.second);
			MenuItem* mi = new MenuItem(p.first + L" (" + fileName + L")");

			mi->event.Reset();
			mi->event.UseDestructProtection = true;
			mi->event.Bind(this, &MainWindow::Menu_OpenRecentProject);
			mi->UserPointer = reinterpret_cast<void*>(static_cast<uintptr>(i));
			m_recentPrjSubMenu->Add(mi, nullptr);

			MenuItem* mi2 = new MenuItem(*mi);
			mi2->event.Reset();
			mi2->event.UseDestructProtection = true;
			mi2->event.Bind(this, &MainWindow::Menu_QuickBuildRecentProject);
			m_quickbuildSubMenu->Add(mi2, nullptr);
		}

	}

	void MainWindow::RefreshMaterialList()
	{
		m_projectMaterialNames.Clear();
		if (m_project)
		{
			RefreshMaterialList(m_project->getItems());
		}
	}

	void ParseMaterialTree(const String& baseMtrlName, const ConfigurationSection* sect, List<std::pair<String, String>>& resultNameList)
	{
		// build a name
		String name = baseMtrlName;
		if (name.size())
		{
			name.append(L"_");
		}
		name.append(sect->getName());

		String comment;
		sect->tryGetAttribute(L"Comment", comment);
		
		resultNameList.Add(std::make_pair(name, comment));

		// go into sub sections
		for (ConfigurationSection::SubSectionEnumerator e = sect->GetSubSectionEnumrator(); e.MoveNext();)
		{
			ParseMaterialTree(name, e.getCurrentValue(), resultNameList);
		}
	}

	void MainWindow::RefreshMaterialList(const List<ProjectItem*>& items)
	{
		for (ProjectItem* itm : items)
		{
			if (itm->getType() == ProjectItemType::MaterialSet)
			{
				ProjectResMaterialSet* eff = static_cast<ProjectResMaterialSet*>(itm->getData());
				String cpath = PathUtils::Combine(eff->GetAbsoluteSourcePathBase(), eff->SourceFile);

				LogManager::getSingleton().Write(LOG_Graphics, L"Updating material table " + itm->getName());

				Configuration config;
				XMLConfigurationFormat::Instance.Load(FileLocation(cpath), &config);

				for (ConfigurationSection* ss : config[L"Materials"]->getSubSections())
				{
					ParseMaterialTree(L"", ss, m_projectMaterialNames);
				}
			}
			else if (itm->getType() == ProjectItemType::Folder)
			{
				ProjectFolder* fld = static_cast<ProjectFolder*>(itm->getData());
				RefreshMaterialList(fld->SubItems);
			}
		}
	}

	void MainWindow::SetProjectWorkingDir(const Project* prj)
	{
		while (FileSystem::getSingleton().getNumWorkingDirectories() > m_standardWorkdingDirCount)
			FileSystem::getSingleton().PopWrokingDirectory();

		FileSystem::getSingleton().AddWrokingDirectory(prj->getOutputPath());

		FileLocateRule::Textures = m_standardTextureLoc;
		FileLocateRule::Materials = m_standardMaterialLoc;

		// project has default texture dirs
		// this directory is added to the FileLocateRule by APDesigner, so that
		// textures can be correctly used
		if (m_project->getTexturePath().size())
		{
			LocateCheckPoint cp = { m_project->getTexturePath() };
			
			FileLocateRule::Textures.AddCheckPoint(cp);
			LogManager::getSingleton().Write(LOG_System, L"Adding texture dir: '" + m_project->getTexturePath() + L"'");
		}

		if (m_project->getMaterialPath().size())
		{
			LocateCheckPoint cp = { m_project->getMaterialPath() };
			
			FileLocateRule::Materials.AddCheckPoint(cp);
			LogManager::getSingleton().Write(LOG_System, L"Adding material dir: '" + m_project->getMaterialPath() + L"'");
		}
		if (m_project->getOutputPath() != m_project->getBasePath())
		{
			LogManager::getSingleton().Write(LOG_System, L"This project is using an alternative output directory: '" + m_project->getOutputPath() + L"'");
		}
	}

	void MainWindow::Menu_CloseProject(MenuItem* itm)
	{
		//delete m_project;
		//m_project = nullptr;
	}
	//void MainWindow::Menu_Insert(Control* ctl)
	//{
	//	if (m_project)
	//	{

	//	}
	//}
	void MainWindow::Menu_NewProject(MenuItem* itm)
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
				m_project->SetPath(PathUtils::GetDirectory(path), nullptr);
				m_project->setTexturePath(PathUtils::Combine(path, L"textures"));

				SetProjectWorkingDir(m_project);

				// Once a project is loaded or built, the effects used will be registered/updated in the EffectSystem.
				// So the editing tools using these effect will work perfectly
				UpdateProjectEffect();
				RefreshMaterialList();
			}
		}
		
	}
	void MainWindow::Menu_OpenProject(MenuItem* itm)
	{
		OpenFileDialog dlg;
		dlg.SetFilter(L"Project file(*.aproj;*.xml)\0*.aproj;*.xml\0\0");
		if (dlg.ShowDialog() == DLGRES_OK)
		{
			OpenProject(dlg.getFilePath()[0]);
		}
	}
	void MainWindow::Menu_SaveProject(MenuItem* itm)
	{
		if (m_project)
			SaveProject(m_projectFilePath);
	}
	void MainWindow::Menu_Exit(MenuItem* itm)
	{
		m_window->Exit();
	}
	void MainWindow::Menu_BuildAll(MenuItem* itm)
	{
		if (m_project)
		{
			LogManager::getSingleton().Write(LOG_System, String(L"Building project '") + m_project->getName() + String(L"'..."));
			BuildInterface::getSingleton().AddBuild(m_project);
			BuildInterface::getSingleton().Execute();
		}
	}
	
	void MainWindow::Menu_OpenRecentProject(MenuItem* itm)
	{
		int32 idx = static_cast<int32>(reinterpret_cast<uintptr>(itm->UserPointer));

		const Queue<std::pair<String, String>>& prjs = cfGetRecentProjects();
		if (idx < prjs.getCount())
		{
			String path = prjs.GetElement(idx).second;
			if (File::FileExists(path))
				OpenProject(path);
			else
			{
				ShowMessageBox(L"File not found", L"Error", true);
				cfRemoveRecentProject(idx);
			}
		}
	}
	void MainWindow::Menu_QuickBuildRecentProject(MenuItem* item)
	{
		int32 idx = static_cast<int32>(reinterpret_cast<uintptr>(item->UserPointer));

		const Queue<std::pair<String, String>>& prjs = cfGetRecentProjects();
		if (idx < prjs.getCount())
		{
			String path = prjs.GetElement(idx).second;
			if (File::FileExists(path))
			{
				Project* prj = new Project();
				FileLocation fl(path);
				Configuration* conf = ConfigurationManager::getSingleton().CreateInstance(fl);
				
				prj->Parse(conf->get(L"Project"));

				prj->SetPath(PathUtils::GetDirectory(path), nullptr);
				delete conf;

				BuildInterface::getSingleton().AddBuild(prj);
				BuildInterface::getSingleton().Execute();
			}
			else
			{
				ShowMessageBox(L"File not found", L"Error", true);
				cfRemoveRecentProject(idx);
			}
		}
	}

	void MainWindow::Menu_ToolItemOpen(MenuItem* itm)
	{
		MenuItem* item = itm;

		EditorExtension* eext = static_cast<EditorExtension*>(item->UserPointer);

		String name = eext->GetName();
		List<String> fexts = eext->GetFileExtensions();

		OpenFileDialog dlg;
		dlg.SetFilter(eext->GetName(), eext->GetFileExtensions());

		if (dlg.ShowDialog() == DLGRES_OK)
		{
			Document* doc = eext->DirectOpen(dlg.getFilePath()[0]);
			doc->LoadRes();
			this->AddDocument(doc);
		}
	}
	void MainWindow::Menu_Tools_AtomManager(MenuItem* itm)
	{
		m_atomManager->Show();
	}
	void MainWindow::Document_Activated(Document* doc)
	{
		m_currentDocument = doc;
	}

	void MainWindow::Document_Deactivated(Document* doc)
	{
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