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

#include "Core/Logging.h"
#include "Config/ConfigurationSection.h"
#include "Graphics/RenderSystem/Sprite.h"
#include "MainWindow.h"
#include "UILib/Form.h"
#include "UILib/List.h"
#include "UILib/Button.h"
#include "UILib/Label.h"
#include "UILib/ComboBox.h"
#include "UILib/CheckBox.h"
#include "Project/Project.h"
#include "UIResources.h"
#include "Document.h"
#include "EditorExtensionManager.h"

#include "TextureViewer.h"

#include "Vfs/File.h"
#include "Vfs/PathUtils.h"

#include "BuildService/BuildService.h"

#include "Utility/StringUtils.h"

using namespace Apoc3D::Utility;
using namespace Apoc3D::VFS;

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
	/* ResourcePane                                                         */
	/************************************************************************/
	
	ResourcePane::ResourcePane(MainWindow* window)
		: m_mainWindow(window), m_currentProject(0), m_skin(window->getUISkin())
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
			m_resourceView->eventSelectionChanged().bind(this, &ResourcePane::TreeView_SelectionChanged);
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

		m_openItem = new Button(Point(15,400),L"Open");
		m_openItem->SetSkin(window->getUISkin());
		m_openItem->eventRelease().bind(this, &ResourcePane::BtnOpen_Release);
		m_form->getControls().Add(m_openItem);

		m_applyModify = new Button(Point(120,400),L"Apply");
		m_applyModify->SetSkin(window->getUISkin());
		m_applyModify->eventRelease().bind(this, &ResourcePane::BtnApplyMod_Release);
		m_form->getControls().Add(m_applyModify);
	}
	ResourcePane::~ResourcePane()
	{
		delete m_form;
		NukePropertyList();
		delete m_infoDisplay;
		delete m_addItem;
		delete m_removeItem;
		delete m_openItem;
		delete m_applyModify;

		NukeTreeView();
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
		newSize.Y -= 30+400;
		m_resourceView->SetSize(newSize);
	
		m_infoDisplay->Position.Y = m_resourceView->Size.Y + m_resourceView->Position.Y + 5;

		m_removeItem->Position.Y = m_addItem->Position.Y = m_infoDisplay->Position.Y + 80;
		m_openItem->Position.Y = m_removeItem->Position.Y+m_removeItem->Size.Y+5;
		m_applyModify->Position.Y=m_openItem->Position.Y;

		for (int i=0;i<m_propLeft.getCount();i++)
		{
			int top = m_openItem->Position.Y + m_openItem->Size.Y + 15 + i * 25;
			m_propLeft[i]->Position.Y = top;
		}
		for (int i=0;i<m_propRight.getCount();i++)
		{
			int top = m_openItem->Position.Y + m_openItem->Size.Y + 15 + i * 25;
			m_propRight[i]->Position.Y = top;
		}
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
			case PRJITEM_Effect:
				{
					ProjectResEffect* eff = static_cast<ProjectResEffect*>(data);
					TreeViewNode* newNode = new TreeViewNode(items[i]->getName(), UIResources::GetTexture(L"adui_effects_file"));
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
			case PRJITEM_CustomEffect:
				{
					ProjectResCustomEffect* eff = static_cast<ProjectResCustomEffect*>(data);
					TreeViewNode* newNode = new TreeViewNode(items[i]->getName(), UIResources::GetTexture(L"adui_effects_file"));
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
			case PRJITEM_Font:
				{
					ProjectResFont* fnt = static_cast<ProjectResFont*>(data);
					TreeViewNode* newNode = new TreeViewNode(items[i]->getName(), UIResources::GetTexture(L"adui_new_document"));
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
			case PRJITEM_Material:
				{
					ProjectResMaterial* fnt = static_cast<ProjectResMaterial*>(data);
					TreeViewNode* newNode = new TreeViewNode(items[i]->getName(), UIResources::GetTexture(L"adui_material_32x32"));
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
			case PRJITEM_TransformAnimation:
				{
					ProjectResTAnim* fnt = static_cast<ProjectResTAnim*>(data);
					TreeViewNode* newNode = new TreeViewNode(items[i]->getName(), UIResources::GetTexture(L"adui_rebuild_all_32X32"));
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

	void ResourcePane::NukePropertyList()
	{
		for (int i=0;i<m_propLeft.getCount();i++)
		{
			m_form->getControls().Remove(m_propLeft[i]);
			delete m_propLeft[i];
		}
		m_propLeft.Clear();

		for (int i=0;i<m_propRight.getCount();i++)
		{
			m_form->getControls().Remove(m_propRight[i]);
			delete m_propRight[i];
		}
		m_propRight.Clear();
	}
	void ResourcePane::AddPropertyPair(const String& a, const String& b)
	{
		int top = m_openItem->Position.Y + m_openItem->Size.Y + 15 + m_propLeft.getCount() * 25;
		//for (int i=0;i<m_propLeft.getCount();i++)
		//{
		//	if (m_propLeft[i]->Position.Y + m_propLeft[i]->Size.Y + 40> top )
		//		top = m_propLeft[i]->Position.Y + m_propLeft[i]->Size.Y + 40;
		//}

		Label* label = new Label(Point(5, top), a, 100);

		TextBox* tb = new TextBox(Point(110, top), 100, b);
		m_propLeft.Add(label);
		m_propRight.Add(tb);
		label->SetSkin(m_skin);
		label->Initialize(m_form->getRenderDevice());
		tb->SetSkin(m_skin);
		tb->Initialize(m_form->getRenderDevice());

		m_form->getControls().Add(label);
		m_form->getControls().Add(tb);
	}
	void ResourcePane::AddPropertyDropdown(const String& name, const List<String>& list, int selectedIndex)
	{
		int top = m_openItem->Position.Y + m_openItem->Size.Y + 15 + m_propLeft.getCount() * 25;

		Label* label = new Label(Point(5, top), name, 100);
		ComboBox* combo = new ComboBox(Point(110, top), 100, list);
		
		m_propLeft.Add(label);
		m_propRight.Add(combo);
		label->SetSkin(m_skin);
		label->Initialize(m_form->getRenderDevice());
		combo->SetSkin(m_skin);
		combo->Initialize(m_form->getRenderDevice());

		if (selectedIndex != -1)
			combo->setSelectedIndex(selectedIndex);

		m_form->getControls().Add(label);
		m_form->getControls().Add(combo);
	}
	void ResourcePane::AddPropertyDropdown_PixelFormat(const String& name, const PixelFormat& selectedFmt)
	{
		int top = m_openItem->Position.Y + m_openItem->Size.Y + 15 + m_propLeft.getCount() * 25;

		List<String> items;

		int selectedIndex = -1;
		for (int i=0;i<FMT_Count;i++)
		{
			PixelFormat cfmt = static_cast<PixelFormat>(i);
			items.Add(PixelFormatUtils::ToString(cfmt));
			if (selectedFmt == i)
				selectedIndex = i;
		}

		Label* label = new Label(Point(5, top), name, 100);
		ComboBox* combo = new ComboBox(Point(110, top), 100, items);
		
		m_propLeft.Add(label);
		m_propRight.Add(combo);
		label->SetSkin(m_skin);
		label->Initialize(m_form->getRenderDevice());
		combo->SetSkin(m_skin);
		combo->Initialize(m_form->getRenderDevice());

		if (selectedIndex!=-1)
			combo->setSelectedIndex(selectedIndex);

		m_form->getControls().Add(label);
		m_form->getControls().Add(combo);
	}
	void ResourcePane::AddPropertyCheckbox(const String& name, bool checked)
	{
		int top = m_openItem->Position.Y + m_openItem->Size.Y + 15 + m_propLeft.getCount() * 25;

		Label* label = new Label(Point(5, top), L"", 100); // dummy one
		CheckBox* cb = new CheckBox(Point(5, top), name, checked);
		m_propLeft.Add(label);
		m_propRight.Add(cb);
		label->SetSkin(m_skin);
		label->Initialize(m_form->getRenderDevice());
		cb->SetSkin(m_skin);
		cb->Initialize(m_form->getRenderDevice());

		m_form->getControls().Add(label);
		m_form->getControls().Add(cb);
	}
	void ResourcePane::ListNewProperties(ProjectItemData* data)
	{
		NukePropertyList();
		switch (data->getType())
		{
		case PRJITEM_Texture:
			{
				ProjectResTexture* tex = static_cast<ProjectResTexture*>(data);

				//AddPropertyPair(L"AssembleCubemap", StringUtils::ToString(tex->AssembleCubemap));
				//AddPropertyPair(L"AssembleVolumeMap", StringUtils::ToString(tex->AssembleVolumeMap));
				AddPropertyPair(L"DestinationFile", tex->DestinationFile);
				AddPropertyPair(L"SourceFile", tex->SourceFile);

				AddPropertyCheckbox(L"GenerateMipmaps", tex->GenerateMipmaps);

				List<String> items;
				items.Add(ProjectResTexture::ToString(ProjectResTexture::TEXBUILD_D3D));
				items.Add(ProjectResTexture::ToString(ProjectResTexture::TEXBUILD_Devil));
				items.Add(ProjectResTexture::ToString(ProjectResTexture::TEXBUILD_BuiltIn));
				AddPropertyDropdown(L"Method", items, items.IndexOf(ProjectResTexture::ToString(tex->Method)));
				
				AddPropertyCheckbox(L"Resize", tex->Resize);

				items.Clear();
				items.Add(ProjectResTexture::ToString(ProjectResTexture::TFLT_Nearest));
				items.Add(ProjectResTexture::ToString(ProjectResTexture::TFLT_Box));
				items.Add(ProjectResTexture::ToString(ProjectResTexture::TFLT_BSpline));
				AddPropertyDropdown(L"ResizeFilterType", items, items.IndexOf(ProjectResTexture::ToString(tex->ResizeFilterType)));

				AddPropertyDropdown_PixelFormat(L"NewFormat", tex->NewFormat);
			}
			break;
		case PRJITEM_Model:
			{
				ProjectResModel* mdl = static_cast<ProjectResModel*>(data);
				
				AddPropertyPair(L"DstFile", mdl->DstFile);
				AddPropertyPair(L"DstAnimationFile", mdl->DstAnimationFile);
				AddPropertyPair(L"SrcFile", mdl->SrcFile);


				List<String> items;
				items.Add(ProjectResModel::ToString(ProjectResModel::MESHBUILD_ASS));
				items.Add(ProjectResModel::ToString(ProjectResModel::MESHBUILD_FBX));
				items.Add(ProjectResModel::ToString(ProjectResModel::MESHBUILD_D3D));
				AddPropertyDropdown(L"Method", items, items.IndexOf(ProjectResModel::ToString(mdl->Method)));
			}
			break;
		case PRJITEM_Font:
			{
				ProjectResFont* fnt = static_cast<ProjectResFont*>(data);


			}
			break;
		case PRJITEM_Folder:
			{
				ProjectFolder* folder = static_cast<ProjectFolder*>(data);
				AddPropertyPair(L"DstPack", folder->DestinationPack);
				AddPropertyPair(L"PackType", folder->PackType);
			}
			break;
		case PRJITEM_Effect:
			{

			}
			break;
		case PRJITEM_Material:
			{
				ProjectResMaterial* mtrl = static_cast<ProjectResMaterial*>(data);

				AddPropertyPair(L"DstFile", mtrl->DestinationFile);
			}
			break;
		case PRJITEM_TransformAnimation:
			{
				ProjectResTAnim* ta = static_cast<ProjectResTAnim*>(data);
				AddPropertyPair(L"DstFile", ta->DestinationFile);
				AddPropertyPair(L"SrcFile", ta->SourceFile);


			}
			break;
		}
	}

	void ResourcePane::TreeView_SelectionChanged(Control* ctrl)
	{
		if (m_resourceView->getSelectedNode())
		{
			ProjectItem* item = static_cast<ProjectItem*>(m_resourceView->getSelectedNode()->UserData);
			if (item)
			{
				m_infoDisplay->Text = item->getName();
				m_infoDisplay->Text.append(L"\n");


				switch (item->getType())
				{
				case PRJITEM_Texture:
					m_infoDisplay->Text.append(L"[Texture]\n");
					break;
				case PRJITEM_Model:
					m_infoDisplay->Text.append(L"[Model]\n");
					break;
				case PRJITEM_Folder:
					m_infoDisplay->Text.append(L"[Folder]\n");
					break;
				case PRJITEM_Effect:
					m_infoDisplay->Text.append(L"[Effect]\n");
					break;
				case PRJITEM_Font:
					m_infoDisplay->Text.append(L"[Font]\n");
					break;
				case PRJITEM_Material:
					m_infoDisplay->Text.append(L"[Material]\n");
					break;
				case PRJITEM_TransformAnimation:
					m_infoDisplay->Text.append(L"[TransformAnim]\n");
					break;
				}

				if (item->IsOutDated())
				{
					m_infoDisplay->Text.append(L"Built(but outdated).");
				}
				else
				{
					m_infoDisplay->Text.append(L"Built(up to date).");
				}
				ListNewProperties(item->getData());
			}
			
		}
	}

	void ResourcePane::BtnAdd_Release(Control* ctrl)
	{

	}
	void ResourcePane::BtnRemove_Release(Control* ctrl)
	{

	}
	void ResourcePane::BtnOpen_Release(Control* ctrl)
	{
		if (m_resourceView->getSelectedNode())
		{
			ProjectItem* item = static_cast<ProjectItem*>(m_resourceView->getSelectedNode()->UserData);
			if (item)
			{
				if (item->IsOutDated())
				{
					LogManager::getSingleton().Write(LOG_System, String(L"Building asset '") + item->getName() + String(L"'..."));
					BuildInterface::BuildSingleItem(item);
					LogManager::getSingleton().Write(LOG_System, BuildInterface::LastResult);
				}

				switch (item->getType())
				{
				case PRJITEM_Texture:
					{
						ProjectResTexture* tex = static_cast<ProjectResTexture*>(item->getData());
						String path = PathUtils::Combine(m_currentProject->getOutputPath(), tex->DestinationFile);
						if (File::FileExists(path))
						{
							TextureViewer* tv = new TextureViewer(m_mainWindow, tex->DestinationFile, path);
							tv->LoadRes();
							m_mainWindow->AddDocument(tv);

						}

					}
					break;
				default:
					{
						EditorExtension* ext = EditorExtensionManager::getSingleton().FindSuitableExtension(item);
						if (ext)
						{
							Document* doc = ext->OpenItem(item);
							doc->LoadRes();
							m_mainWindow->AddDocument(doc);
						}
						
					}
				}
			}
			
		}
	}
	void ResourcePane::BtnApplyMod_Release(Control* ctrl)
	{
		if (m_resourceView->getSelectedNode())
		{
			ProjectItem* item = static_cast<ProjectItem*>(m_resourceView->getSelectedNode()->UserData);
			if (item)
			{
				//ConfigurationSection* temp = new ConfigurationSection(L"temp");
				//item->getData()->Save(temp, false);

				

				item->NotifyModified();
			}
		}
		NukePropertyList();
	}
}