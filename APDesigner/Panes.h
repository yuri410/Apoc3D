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
#ifndef PANES_H
#define PANES_H

#include "APDCommon.h"

#include "Collections/FastList.h"

using namespace Apoc3D;
using namespace Apoc3D::Collections;
using namespace Apoc3D::Graphics;
using namespace Apoc3D::Graphics::RenderSystem;
using namespace Apoc3D::UI;
using namespace Apoc3D::Core;

namespace APDesigner
{
	class ToolsPane
	{
	private:
		MainWindow* m_mainWindow;

		Form* m_pane;
		Button* m_btSave;

		ObjectTools* m_currentTools;

		void BtnSave_Release(Control* ctrl);
	public:
		Form* getPane() const { return m_pane; }
		ToolsPane(MainWindow* window);
		~ToolsPane();


		void Initialize(RenderDevice* device);

		void Update(const GameTime* const time);
	};


	class ResourcePane
	{
	private:
		//static const int ItemHeight = 60;
		MainWindow* m_mainWindow;

		Form* m_form;

		TreeView* m_resourceView;

		Project* m_currentProject;

		void NukeTreeViewNodes(FastList<TreeViewNode*>& nodes);
		void NukeTreeView();

		void BuildTreeViewNodes(TreeViewNode* parentNode, const FastList<ProjectItem*> items);
		void BuildTreeViewNodes(const FastList<ProjectItem*> items);
	public:
		
		ResourcePane(MainWindow* window);
		~ResourcePane();

		void Initialize(RenderDevice* device);

		void Update(const GameTime* const time);

		void UpdateToNewProject(Project* prj);
	};

	class PropertyPane
	{
	private:
		ObjectPropertyEditor* m_currentEditor;

		
	public:

		PropertyPane(MainWindow* window);

		void Initialize(RenderDevice* device);
		void Update(const GameTime* const time);
	};
}

#endif