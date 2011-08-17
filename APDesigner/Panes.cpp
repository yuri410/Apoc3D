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
#include "UILib/Button.h"
#include "UILib/Label.h"
#include "Project/Project.h"
#include "UIResources.h"
#include "Document.h"

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

		m_btSave = new Button(Point(5,20), L"");
		m_btSave->setNormalTexture(UIResources::GetTexture(L"adui_save"));
		m_btSave->setCustomModColorMouseOver(CV_Silver);
		
		m_btSave->eventRelease().bind(this,&ToolsPane::BtnSave_Release);
		m_pane->getControls().Add(m_btSave);
	}
	ToolsPane::~ToolsPane()
	{
		delete m_pane;
		delete m_btSave;
	}

	void ToolsPane::BtnSave_Release(Control* ctrl)
	{
		if (m_mainWindow->getCurrentDocument())
		{
			m_mainWindow->getCurrentDocument()->SaveRes();
		}
		//m_mainWindow->SaveProject();
	}

	void ToolsPane::Initialize(RenderDevice* device)
	{


		m_pane->Initialize(device);

		UIRoot::Add(m_pane);
		m_pane->Show();
	}

	void ToolsPane::Update(const GameTime* const time)
	{
		
		if (m_mainWindow->getCurrentDocument())
		{
			if (m_mainWindow->getCurrentDocument()->IsReadOnly())
			{
				m_btSave->Enabled = false;
			}
			else
			{
				m_btSave->Enabled = true;
			}
		}
		else
		{
			m_btSave->Enabled = false;
		}

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

			TreeViewNode* node1 = new TreeViewNode(L"No project opened");
			treeview->getNodes().Add(node1);

			m_form->getControls().Add(treeview);
			m_resourceView = treeview;
		}

		m_infoDisplay = new Label(Point(15,330),L"",1);
		m_infoDisplay->SetSkin(window->getUISkin());
		m_form->getControls().Add(m_infoDisplay);

		m_addItem = new Button(Point(15,375),L"Add...");
		m_addItem->SetSkin(window->getUISkin());
		m_addItem->eventRelease().bind(this, &ResourcePane::BtnAdd_Release);
		m_form->getControls().Add(m_addItem);

		m_removeItem = new Button(Point(120,375),L"Delete");
		m_removeItem->SetSkin(window->getUISkin());
		m_removeItem->eventRelease().bind(this, &ResourcePane::BtnRemove_Release);
		m_form->getControls().Add(m_removeItem);

		m_openItem = new Button(Point(15,405),L"Open");
		m_openItem->SetSkin(window->getUISkin());
		m_openItem->eventRelease().bind(this, &ResourcePane::BtnOpen_Release);
		m_form->getControls().Add(m_openItem);

	}
	ResourcePane::~ResourcePane()
	{
		delete m_form;

		delete m_infoDisplay;
		delete m_addItem;
		delete m_removeItem;
		delete m_openItem;

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
	
		m_infoDisplay->Position.Y = m_resourceView->Size.Y + m_resourceView->Position.Y + 5;

		m_removeItem->Position.Y = m_addItem->Position.Y = m_infoDisplay->Position.Y + 80;
		m_openItem->Position.Y = m_removeItem->Position.Y+m_removeItem->Size.Y+15;

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

					TreeViewNode* newNode = new TreeViewNode(items[i]->getName(), UIResources::GetTexture(L"adui_NewProject1"));
					newNode->UserData = items[i];

					BuildTreeViewNodes(newNode, fld->SubItems);

					if (parentNode)
					{
						parentNode->getNodes().Add(newNode);
					}
					else
					{
						m_resourceView->getNodes().Add(newNode);
					}

				}
				break;
			case PRJITEM_Model:
				{
					ProjectResModel* mdl = static_cast<ProjectResModel*>(data);
					TreeViewNode* newNode = new TreeViewNode(items[i]->getName(), UIResources::GetTexture(L"adui_Scene"));
					newNode->UserData = items[i];


					if (parentNode)
					{
						parentNode->getNodes().Add(newNode);
					}
					else
					{
						m_resourceView->getNodes().Add(newNode);
					}
				}
				break;
			case PRJITEM_Texture:
				{
					ProjectResTexture* tex = static_cast<ProjectResTexture*>(data);
					TreeViewNode* newNode = new TreeViewNode(items[i]->getName(), UIResources::GetTexture(L"adui_Image"));
					newNode->UserData = items[i];


					if (parentNode)
					{
						parentNode->getNodes().Add(newNode);
					}
					else
					{
						m_resourceView->getNodes().Add(newNode);
					}
				}
				break;
			}
		}
	}
	void ResourcePane::BuildTreeViewNodes(const FastList<ProjectItem*> items)
	{
		BuildTreeViewNodes(0, items);
	}

	//void ResourcePane::TreeView_SelectionChanged(Control* ctrl)
	//{

	//}

	void ResourcePane::BtnAdd_Release(Control* ctrl)
	{

	}
	void ResourcePane::BtnRemove_Release(Control* ctrl)
	{

	}
	void ResourcePane::BtnOpen_Release(Control* ctrl)
	{

	}

}