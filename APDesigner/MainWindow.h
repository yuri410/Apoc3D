#pragma once

/* -----------------------------------------------------------------------
 * This source file is part of Apoc3D Framework
 * 
 * Copyright (c) 2009-2018 Tao Xin
 * 
 * This content of this file is subject to the terms of the Mozilla Public 
 * License v2.0. If a copy of the MPL was not distributed with this file, 
 * you can obtain one at http://mozilla.org/MPL/2.0/.
 * 
 * This program is distributed in the hope that it will be useful, 
 * WITHOUT WARRANTY OF ANY KIND; either express or implied. See the 
 * Mozilla Public License for more details.
 * 
 * ------------------------------------------------------------------------
 */

#include "APDCommon.h"

namespace APDesigner
{
	/** The main window and the main class
	*/
	class APDAPI MainWindow : public RenderWindowHandler
	{
	public:
		MainWindow(RenderWindow* wnd);
		~MainWindow();

		void AddDocument(Document* document);

		void SwitchedTo(Document* document);
		

		virtual void Initialize();
		virtual void Finalize();

		virtual void Load();
		virtual void Unload();
		virtual void Update(const GameTime* time);
		virtual void Draw(const GameTime* time);

		virtual void OnFrameStart();
		virtual void OnFrameEnd();

		void RefreshMaterialList();

		const StyleSkin* getUISkin() const { return m_UIskin; }
		const Point& getUIAreaSize() const { return m_lastSize; }
		RenderDevice* getDevice() const { return m_device; }
		RenderWindow* getWindow() const { return m_window; }
		Form* getToolsPane() const;

		Document* getCurrentDocument() const { return m_currentDocument; }

		MenuBar* getMenuBar() const { return m_mainMenu; }

		const List<std::pair<String, String>>& getProjectMaterialDesc() const { return m_projectMaterialNames; }
	private:
		void RefreshMaterialList(const List<ProjectItem*>& items);
		void SetProjectWorkingDir(const Project* prj);

		String GetProjectStampFilePath(const String* projectFilePath);

		void LoadMenus();

		void Menu_Tools_AtomManager(MenuItem* itm);

		void Menu_ToolItemOpen(MenuItem* itm);

		void Menu_NewProject(MenuItem* itm);
		void Menu_OpenProject(MenuItem* itm);
		void Menu_SaveProject(MenuItem* itm);
		void Menu_CloseProject(MenuItem* itm);
		void Menu_Insert(MenuItem* itm);
		void Menu_Exit(MenuItem* itm);
		void Menu_DetectOrphaned(MenuItem* itm);
		void Menu_BuildAll(MenuItem* itm);
		void Menu_OpenRecentProject(MenuItem* itm);
		void Menu_QuickBuildRecentProject(MenuItem* item);

		void OpenProject(const String& path);
		void SaveProject(const String& path);
		void CloseProject();

		void Document_Activated(Document* doc);
		void Document_Deactivated(Document* doc);

		void UpdateProjectEffect(const List<ProjectItem*>& items);
		void UpdateProjectEffect();

		void UpdateWindowTitle();
		void UpdateRecentProjects();

		void LogBuildMessages();

		StyleSkin* m_UIskin = nullptr;

		RenderWindow* m_window = nullptr;
		RenderDevice* m_device = nullptr;

		Sprite* m_sprite = nullptr;

		Document* m_currentDocument = nullptr;
		List<Document*> m_documentList;
		Console* m_console = nullptr;

		MenuBar* m_mainMenu = nullptr;
		MenuItem* m_buildMemuItem = nullptr;
		MenuItem* m_savePrjMemuItem = nullptr;
		SubMenu* m_recentPrjSubMenu = nullptr;
		SubMenu* m_quickbuildSubMenu = nullptr;

		AtomManagerDialog* m_atomManager = nullptr;
		OrphanDetector* m_orphanDetector = nullptr;
		ResourcePane* m_resourcePane = nullptr;
		ToolsPane* m_toolsPane = nullptr;
		Project* m_project = nullptr;
		String m_projectFilePath;

		Point m_lastSize;

		List<std::pair<String, String>> m_projectMaterialNames;

		int32 m_standardWorkdingDirCount;
		FileLocateRule m_standardTextureLoc;
		FileLocateRule m_standardMaterialLoc;
	};
}
