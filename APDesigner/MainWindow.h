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

#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include "APDCommon.h"

#include "Graphics/RenderSystem/RenderWindowHandler.h"
#include "Math/Point.h"

using namespace Apoc3D::Graphics::RenderSystem;
using namespace Apoc3D::UI;
using namespace Apoc3D;
using namespace Apoc3D::Math;


namespace APDesigner
{
	/** The main window and the main class
	*/
	class MainWindow : public RenderWindowHandler
	{
	public:
		const StyleSkin* getUISkin() const { return m_UIskin; }
		const Point& getUIAreaSize() const { return m_lastSize; }
		RenderDevice* getDevice() const { return m_device; }
		Form* getToolsPane() const;

		Document* getCurrentDocument() const { return m_currentDocument; }

		MainWindow(RenderWindow* wnd);
		~MainWindow();

		void AddDocument(Document* document);

		void SwitchedTo(Document* document);
		

		virtual void Initialize();
		virtual void Finalize();

		virtual void Load();
		virtual void Unload();
		virtual void Update(const GameTime* const time);
		virtual void Draw(const GameTime* const time);

		virtual void OnFrameStart();
		virtual void OnFrameEnd();

	private:
		void Menu_ToolItem(Control* ctl);

		StyleSkin* m_UIskin;

		RenderWindow* m_window;
		RenderDevice* m_device;

		Sprite* m_sprite;

		Document* m_currentDocument;
		FastList<Document*> m_documentList;
		//Font* m_font;
		Console* m_console;
		//ControlContainer* m_pane;
		//Button* m_btn;
		//Form* m_form;
		Menu* m_mainMenu;

		AtomManagerDialog* m_atomManager;
		ResourcePane* m_resourcePane;
		ToolsPane* m_toolsPane;
		Project* m_project;
		String m_projectFilePath;

		Point m_lastSize;

		void Menu_Tools_AtomManager(Control* ctl);

		void Menu_NewProject(Control* ctl);
		void Menu_OpenProject(Control* ctl);
		void Menu_SaveProject(Control* ctl);
		void Menu_CloseProject(Control* ctl);
		void Menu_Insert(Control* ctl);
		void Menu_Exit(Control* ctl);
		void Menu_BuildAll(Control* ctl);
		void OpenProject(const String& path);
		void SaveProject(const String& path);
		void CloseProject();

		void Document_Activated(Document* doc);
		void Document_Deactivated(Document* doc);
		void UpdateProjectEffect(const FastList<ProjectItem*>& items);
		void UpdateProjectEffect();
	};
}

#endif