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
#include "Panes.h"

#include "Graphics/RenderSystem/Sprite.h"
#include "MainWindow.h"
#include "UILib/Form.h"
#include "UILib/List.h"
#include "Project/Project.h"

namespace APDesigner
{
	/************************************************************************/
	/*                                                                      */
	/************************************************************************/
	ToolsPane::ToolsPane(MainWindow* window)
		: m_mainWindow(window)
	{
		m_pane = new Form(FBS_Pane);
		m_pane->SetSkin(window->getUISkin());
		
		m_pane->setMinimumSize(Point(50,50));
		m_pane->Size = Point(50,600);
		m_pane->Position = Point(0, 17);

		m_pane->Text = L"Tools.";
		//m_pane->setTitle(m_pane->Text);


	}
	ToolsPane::~ToolsPane()
	{
		delete m_pane;
	}


	void ToolsPane::Initialize(RenderDevice* device)
	{
		m_pane->Initialize(device);

		UIRoot::Add(m_pane);
		m_pane->Show();
	}

	void ToolsPane::Update(const GameTime* const time)
	{
		
		m_pane->Position.X = m_mainWindow->getUIAreaSize().X - m_pane->Size.X;
		m_pane->Position.Y = 17;
		m_pane->Size.Y = m_mainWindow->getUIAreaSize().Y - 17;
		//Point newSize = m_pane->Size;
		//newSize.X -= 10;
		//newSize.Y -= 30+300;
		//m_resourceView->SetSize(newSize);

	}
	/************************************************************************/
	/*                                                                      */
	/************************************************************************/
	
	ResourcePane::ResourcePane(MainWindow* window)
		: m_mainWindow(window), m_currentProject(0)
	{
		m_form = new Form(FBS_Pane);
		m_form->SetSkin(window->getUISkin());


		m_form->Size = Point(225,600);
		m_form->Position = Point(0, 17);
		
		m_form->Text = L"Resource Explorer.";
		m_form->setTitle(m_form->Text);


		{
			TreeView* treeview = new TreeView(Point(5, 25), 200, 300);
			treeview->SetSkin(window->getUISkin());
			treeview->setUseHorizontalScrollbar(true);

			TreeViewNode* node1 = new TreeViewNode(L"Root");
			treeview->getNodes().Add(node1);

			TreeViewNode* node2 = new TreeViewNode(L"Root2");
			treeview->getNodes().Add(node2);
			TreeViewNode* node3 = new TreeViewNode(L"Root3 the long one. Project Hosting will be READ-ONLY Tuesday at 9:00am CDT for brief maintenance.");
			treeview->getNodes().Add(node3);

			node2->getNodes().Add(new TreeViewNode(L"SubNode1"));
			node2->getNodes().Add(new TreeViewNode(L"SubNode2"));
			node2->getNodes().Add(new TreeViewNode(L"SubNode3"));
			node2->getNodes().Add(new TreeViewNode(L"SubNode4"));
			node2->getNodes().Add(new TreeViewNode(L"SubNode5"));
			node2->getNodes().Add(new TreeViewNode(L"SubNode6"));
			node2->getNodes().Add(new TreeViewNode(L"SubNode7"));
			node2->getNodes().Add(new TreeViewNode(L"SubNode8"));
			node2->getNodes().Add(new TreeViewNode(L"SubNode9"));
			node2->getNodes().Add(new TreeViewNode(L"SubNode10"));
			node2->getNodes().Add(new TreeViewNode(L"SubNode11"));
			node2->getNodes().Add(new TreeViewNode(L"SubNode12"));

			node3->getNodes().Add(new TreeViewNode(L"SubNode1"));
			node3->getNodes().Add(new TreeViewNode(L"SubNode2"));
			node3->Expand();


			m_form->getControls().Add(treeview);
			m_resourceView = treeview;
		}
	}
	ResourcePane::~ResourcePane()
	{
		delete m_form;



		delete m_resourceView;
	}


	void ResourcePane::Initialize(RenderDevice* device)
	{
		m_form->Initialize(device);

		UIRoot::Add(m_form);
		m_form->Show();
	}

	void ResourcePane::Update(const GameTime* const time)
	{
		m_form->Position.X = 0;
		m_form->Position.Y = 17;
		m_form->Size.Y = m_mainWindow->getUIAreaSize().Y - 17-1;
		Point newSize = m_form->Size;
		newSize.X -= 10;
		newSize.Y -= 30+300;
		m_resourceView->SetSize(newSize);
		
	}

	void ResourcePane::UpdateToNewProject(Project* prj)
	{
		m_currentProject = prj;
		NukeTreeView();

		const FastList<ProjectItem*>& items = prj->getItems();
		BuildTreeViewNodes(items);
		
	}


	void ResourcePane::NukeTreeViewNodes(FastList<TreeViewNode*>& nodes)
	{
		for (int i=0;i<nodes.getCount();i++)
		{
			if (nodes[i]->getNodes().getCount())
			{
				NukeTreeViewNodes(nodes[i]->getNodes());
			}
			delete nodes[i];
		}
		nodes.Clear();
	}
	void ResourcePane::NukeTreeView()
	{
		NukeTreeViewNodes(m_resourceView->getNodes());
	}

	void ResourcePane::BuildTreeViewNodes(TreeViewNode* parentNode, const FastList<ProjectItem*> items)
	{
		for (int i=0;i<items.getCount();i++)
		{
			ProjectItemData* data = items[i]->getData();
			switch(data->getType())
			{
			case PRJITEM_Folder:
				{
					
					ProjectFolder* fld = static_cast<ProjectFolder*>(data);

					fld->SubItems;
				}
				break;
			}
		}
	}
	void ResourcePane::BuildTreeViewNodes(const FastList<ProjectItem*> items)
	{
		
	}
}