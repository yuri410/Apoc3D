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
#include "Panes.h"

#include "UIResources.h"
#include "Document.h"
#include "EditorExtensionManager.h"

#include "TextureViewer.h"
#include "MainWindow.h"

#include "BuildService/BuildService.h"
#include "CommonDialog/FileDialog.h"

#include "apoc3d/Core/Logging.h"
#include "apoc3d/Config/ConfigurationSection.h"
#include "apoc3d/Collections/EnumConverterHelper.h"
#include "apoc3d/Graphics/RenderSystem/Sprite.h"
#include "apoc3d/UILib/Button.h"
#include "apoc3d/UILib/ComboBox.h"
#include "apoc3d/UILib/CheckBox.h"
#include "apoc3d/UILib/FontManager.h"
#include "apoc3d/UILib/Form.h"
#include "apoc3d/UILib/List.h"
#include "apoc3d/UILib/Label.h"
#include "apoc3d/UILib/Menu.h"
#include "apoc3d/UILib/StyleSkin.h"
#include "apoc3d/Project/Project.h"

#include "apoc3d/Vfs/File.h"
#include "apoc3d/Vfs/PathUtils.h"

#include "apoc3d/Utility/StringUtils.h"

#include <Shlwapi.h>

using namespace Apoc3D::Utility;
using namespace Apoc3D::VFS;
using namespace APDesigner::CommonDialog;

#pragma comment (lib, "Shlwapi.lib")

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
		m_pane->Position = Point(0, window->getMenuBar()->Size.Y);

		m_pane->Text = L"";
		//m_pane->setTitle(m_pane->Text);

		m_btSave = new Button(Point(5,20), L"");
		m_btSave->setNormalTexture(UIResources::GetTexture(L"adui_save"));
		m_btSave->setCustomModColorMouseOver(CV_Silver);
		
		m_btSave->eventRelease().Bind(this,&ToolsPane::BtnSave_Release);
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
			IndenpendentEditor* ide = dynamic_cast<IndenpendentEditor*>(m_mainWindow->getCurrentDocument());

			if (ide && ide->NeedsSaveAs())
			{
				EditorExtension* eext = m_mainWindow->getCurrentDocument()->getExtension();
				String name = eext->GetName();
				List<String> fexts = eext->GetFileExtensions();

				SaveFileDialog dlg;

				// make filter
				wchar_t filter[512];
				memset(filter, 0, sizeof(filter));

					
				String right;
				//*.a;*.b
				for (int32 i=0;i<fexts.getCount();i++)
				{
					right.append(L"*");
					right.append(fexts[i]);
					if (i+1<fexts.getCount())
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
					ide->SaveAs(dlg.getFilePath()[0]);	
				}
			}
			else
			{
				m_mainWindow->getCurrentDocument()->SaveRes();
			}
			
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
		m_pane->Position.Y = m_mainWindow->getMenuBar()->Size.Y;

		m_pane->Size.Y = m_mainWindow->getUIAreaSize().Y - m_pane->Position.Y;
		//Point newSize = m_pane->Size;
		//newSize.X -= 10;
		//newSize.Y -= 30+300;
		//m_resourceView->SetSize(newSize);

	}


	//////////////////////////////////////////////////////////////////////////

	class ResourcePaneHelper
	{
	public:
		struct ItemTypeInformation
		{
			String IconName;
			String Name;
		};


		ResourcePaneHelper()
		{
			{
				ItemTypeInformation iti = { L"adui_Image", L"Texture" };
				AddEntry(PRJITEM_Texture, iti);
			}
			{
				ItemTypeInformation iti = { L"adui_NewProject1", L"Folder" };
				AddEntry(PRJITEM_Folder, iti);
			}
			{
				ItemTypeInformation iti = { L"adui_Scene", L"Model" };
				AddEntry(PRJITEM_Model, iti);
			}
			{
				ItemTypeInformation iti = { L"adui_effects_file", L"Effect" };
				AddEntry(PRJITEM_Effect, iti);
			}
			{
				ItemTypeInformation iti = { L"adui_effects_file", L"Custom Effect" };
				AddEntry(PRJITEM_CustomEffect, iti);
			}
			{
				ItemTypeInformation iti = { L"adui_new_document", L"Font" };
				AddEntry(PRJITEM_Font, iti);
			}
			{
				ItemTypeInformation iti = { L"adui_material_32x32", L"Material" };
				AddEntry(PRJITEM_Material, iti);
			}
			{
				ItemTypeInformation iti = { L"adui_import1", L"MaterialSet" };
				AddEntry(PRJITEM_MaterialSet, iti);
			}
			{
				ItemTypeInformation iti = { L"adui_skinning", L"Transform Anim" };
				AddEntry(PRJITEM_TransformAnimation, iti);
			}
			{
				ItemTypeInformation iti = { L"adui_surface", L"Material Anim" };
				AddEntry(PRJITEM_MaterialAnimation, iti);
			}
		}


		ItemTypeInformation* Lookup(ProjectItemType type)
		{
			return m_lookupTable.TryGetValue(type);
		}

	private:
		void AddEntry(ProjectItemType type, const ItemTypeInformation& itemInfo)
		{
			m_lookupTable.Add(type, itemInfo);
		}

		HashMap<int, ItemTypeInformation> m_lookupTable;
	};

	static ResourcePaneHelper ResPaneHelperInstance;

	/************************************************************************/
	/* ResourcePane                                                         */
	/************************************************************************/
	
	ResourcePane::ResourcePane(MainWindow* window)
		: m_mainWindow(window), m_currentProject(0), m_skin(window->getUISkin())
	{
		m_form = new Form(FBS_Pane);
		m_form->SetSkin(window->getUISkin());


		m_form->Size = Point(300,600);
		m_form->Position = Point(0, window->getMenuBar()->Size.Y);
		
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
			m_resourceView->eventSelectionChanged().Bind(this, &ResourcePane::TreeView_SelectionChanged);
		}

		m_infoDisplay = new Label(Point(5,355),L"",1);
		m_infoDisplay->SetSkin(window->getUISkin());
		m_form->getControls().Add(m_infoDisplay);

		m_addItem = new Button(Point(5,325),L"Add...");
		m_addItem->SetSkin(window->getUISkin());
		m_addItem->eventRelease().Bind(this, &ResourcePane::BtnAdd_Release);
		m_form->getControls().Add(m_addItem);

		m_removeItem = new Button(Point(m_addItem->Position.X + m_addItem->Size.X,325),L"Delete");
		m_removeItem->SetSkin(window->getUISkin());
		m_removeItem->eventRelease().Bind(this, &ResourcePane::BtnRemove_Release);
		m_form->getControls().Add(m_removeItem);

		m_openItem = new Button(Point(m_removeItem->Position.X + m_removeItem->Size.X, 325),L"Open");
		m_openItem->SetSkin(window->getUISkin());
		m_openItem->eventRelease().Bind(this, &ResourcePane::BtnOpen_Release);
		m_form->getControls().Add(m_openItem);

		m_forceBuildItem = new Button(Point(m_removeItem->Position.X + m_removeItem->Size.X, 325),L"Force Build");
		m_forceBuildItem->SetSkin(window->getUISkin());
		m_forceBuildItem->eventRelease().Bind(this, &ResourcePane::BtnForceBuild_Release);
		m_form->getControls().Add(m_forceBuildItem);

		m_applyModify = new Button(Point(5,400),L"Apply Settings");
		m_applyModify->SetSkin(window->getUISkin());
		m_applyModify->eventRelease().Bind(this, &ResourcePane::BtnApplyMod_Release);
		m_applyModify->Visible = false;
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

		m_removeItem->Position.X = m_addItem->Position.X + m_addItem->Size.X + 5;
		m_openItem->Position.X = m_removeItem->Position.X + m_removeItem->Size.X + 5;
		m_forceBuildItem->Position.X = m_openItem->Position.X + m_openItem->Size.X + 5;

		UIRoot::Add(m_form);
		m_form->Show();
	}

	void ResourcePane::Update(const GameTime* const time)
	{
		m_form->Position.X = 0;
		m_form->Position.Y = m_mainWindow->getMenuBar()->Size.Y;
		m_form->Size.Y = m_mainWindow->getUIAreaSize().Y - m_form->Position.Y;
		Point newSize = m_form->Size;
		newSize.X -= 10;
		newSize.Y -= 30+400;
		m_resourceView->SetSize(newSize);
	
		m_forceBuildItem->Position.Y = m_removeItem->Position.Y = m_addItem->Position.Y = m_openItem->Position.Y = m_resourceView->Size.Y + m_resourceView->Position.Y + + 5;

		m_infoDisplay->Position.Y = m_removeItem->Position.Y + m_removeItem->Size.Y + 5;

		m_applyModify->Position.Y = m_infoDisplay->Position.Y + 45;

		int sy = getPropertyFieldTop();
		for (int32 i=0;i<m_proplist.getCount();i++)
		{
			int top = sy + i * PropFieldSpacing;
			m_proplist[i].Desc->Position.Y = top;
			m_proplist[i].Editor->Position.Y = top;
			if (m_proplist[i].ExtraButton)
				m_proplist[i].ExtraButton->Position.Y = top;
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

			ResourcePaneHelper::ItemTypeInformation* itemType =
				ResPaneHelperInstance.Lookup(data->getType());

			if (itemType)
			{
				TreeViewNode* newNode = new TreeViewNode(items[i]->getName(), UIResources::GetTexture(itemType->IconName));
				newNode->UserData = items[i];

				ProjectFolder* fld = dynamic_cast<ProjectFolder*>(data);
				if (fld)
				{
					BuildTreeViewNodes(newNode, fld->SubItems);
				}
				
				if (parentNode)
				{
					parentNode->getNodes().Add(newNode);
				}
				else
				{
					m_resourceView->getNodes().Add(newNode);
				}
			}
		}
	}
	void ResourcePane::BuildTreeViewNodes(const FastList<ProjectItem*> items)
	{
		BuildTreeViewNodes(0, items);
	}

	void ResourcePane::NukePropertyList()
	{
		for (int i=0;i<m_proplist.getCount();i++)
		{
			m_form->getControls().Remove(m_proplist[i].Desc);
			delete m_proplist[i].Desc;

			m_form->getControls().Remove(m_proplist[i].Editor);
			delete m_proplist[i].Editor;

			if (m_proplist[i].ExtraButton)
			{
				m_form->getControls().Remove(m_proplist[i].ExtraButton);
				delete m_proplist[i].ExtraButton;
			}
		}
		m_proplist.Clear();

		m_applyModify->Visible = false;
	}
	void ResourcePane::AddPropertyPair(const String& a, const String& b)
	{
		int top = getPropertyFieldTop() + m_proplist.getCount() * PropFieldSpacing;
		

		int lw = getPropertyLabelWidth();
		int fw = getPropertyFieldWidth();

		Label* label = new Label(Point(PropFieldMargin, top), a, lw);
		TextBox* tb = new TextBox(Point(PropFieldMargin*2+lw, top), fw, b);

		PropItem item(a, label, tb, nullptr);
		m_proplist.Add(item);

		label->SetSkin(m_skin);
		label->Initialize(m_form->getRenderDevice());
		tb->SetSkin(m_skin);
		tb->Initialize(m_form->getRenderDevice());

		m_form->getControls().Add(label);
		m_form->getControls().Add(tb);
	}
	void ResourcePane::AddPropertyPath(const String& a, const String& b, bool isLoad)
	{
		int top = getPropertyFieldTop() + m_proplist.getCount() * PropFieldSpacing;

		int lw = getPropertyLabelWidth();
		int fw = getPropertyFieldWidth();

		Label* label = new Label(Point(PropFieldMargin, top), a, lw);
		TextBox* tb = new TextBox(Point(PropFieldMargin*2+lw, top), fw-30, b);
		Button* bb = new Button(Point(tb->Position.X+tb->Size.X, top), 30, L"...");
		bb->Size.Y = static_cast<int32>( FontManager::getSingleton().getFont(m_skin->ControlFontName)->getLineHeight());
;

		PropItem item(a, label, tb, bb);
		item.LoadOrSave = isLoad;
		m_proplist.Add(item);

		label->SetSkin(m_skin);
		label->Initialize(m_form->getRenderDevice());
		tb->SetSkin(m_skin);
		tb->Initialize(m_form->getRenderDevice());
		bb->SetSkin(m_skin);
		bb->Initialize(m_form->getRenderDevice());
		if (isLoad)
			bb->eventRelease().Bind(this, &ResourcePane::BtnBrowseOpen_Release);
		else
			bb->eventRelease().Bind(this, &ResourcePane::BtnBrowseSave_Release);

		m_form->getControls().Add(label);
		m_form->getControls().Add(tb);
		m_form->getControls().Add(bb);
	}
	void ResourcePane::AddPropertyDropdown(const String& name, const List<String>& list, int selectedIndex)
	{
		int top = getPropertyFieldTop() + m_proplist.getCount() * PropFieldSpacing;

		int lw = getPropertyLabelWidth();
		int fw = getPropertyFieldWidth();

		Label* label = new Label(Point(PropFieldMargin, top), name, lw);
		ComboBox* combo = new ComboBox(Point(PropFieldMargin*2+lw, top), fw, list);
		
		PropItem item(name, label, combo, nullptr);
		m_proplist.Add(item);
		
		label->SetSkin(m_skin);
		label->Initialize(m_form->getRenderDevice());
		combo->SetSkin(m_skin);
		combo->Initialize(m_form->getRenderDevice());

		if (selectedIndex != -1)
			combo->setSelectedIndex(selectedIndex);

		m_form->getControls().Add(label);
		m_form->getControls().Add(combo);
	}
	void ResourcePane::AddPropertyCheckbox(const String& name, bool checked)
	{
		int top = getPropertyFieldTop() + m_proplist.getCount() * PropFieldSpacing;

		int lw = getPropertyLabelWidth();
		int fw = getPropertyFieldWidth();

		Label* label = new Label(Point(PropFieldMargin, top), L"", lw); // dummy one
		CheckBox* cb = new CheckBox(Point(PropFieldMargin*2+lw, top), name, checked);
		
		PropItem item(name, label, cb, nullptr);
		m_proplist.Add(item);

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

				AddPropertyPath(L"DestinationFile", tex->DestinationFile, false);
				AddPropertyPath(L"SourceFile", tex->SourceFile, true);

				AddPropertyCheckbox(L"GenerateMipmaps", tex->GenerateMipmaps);

				List<String> items;
				ProjectTypeUtils::GetTextureBuildMethodConverter().DumpNames(items);
				AddPropertyDropdown(L"Method", items, items.IndexOf(ProjectTypeUtils::ToString(tex->Method)));
				
				AddPropertyCheckbox(L"Resize", tex->Resize);

				items.Clear();
				ProjectTypeUtils::GetTextureFilterTypeConverter().DumpNames(items);
				AddPropertyDropdown(L"ResizeFilterType", items, items.IndexOf(ProjectTypeUtils::ToString(tex->ResizeFilterType)));

				items.Clear();
				PixelFormatUtils::DumpPixelFormatName(items);
				AddPropertyDropdown(L"NewFormat", items, items.IndexOf(PixelFormatUtils::ToString(tex->NewFormat)));
			}
			break;
		case PRJITEM_Model:
			{
				ProjectResModel* mdl = static_cast<ProjectResModel*>(data);
				
				AddPropertyPath(L"DstFile", mdl->DstFile, false);
				AddPropertyPath(L"DstAnimationFile", mdl->DstAnimationFile, false);
				AddPropertyPath(L"SrcFile", mdl->SrcFile, true);


				List<String> items;
				ProjectTypeUtils::GetMeshBuildMethodConverter().DumpNames(items);
				AddPropertyDropdown(L"Method", items, items.IndexOf(ProjectTypeUtils::ToString(mdl->Method)));
			}
			break;
		case PRJITEM_Font:
			{
				//ProjectResFont* fnt = static_cast<ProjectResFont*>(data);


			}
			break;
		case PRJITEM_Folder:
			{
				ProjectFolder* folder = static_cast<ProjectFolder*>(data);
				AddPropertyPath(L"DstPack", folder->DestinationPack, false);
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

				AddPropertyPath(L"DstFile", mtrl->DestinationFile, false);
			}
			break;
		case PRJITEM_TransformAnimation:
			{
				ProjectResTAnim* ta = static_cast<ProjectResTAnim*>(data);
				AddPropertyPath(L"DstFile", ta->DestinationFile, false);
				AddPropertyPath(L"SrcFile", ta->SourceFile, true);


			}
			break;
		}

		m_applyModify->Visible = true;
	}

	void ResourcePane::ApplyProperties(ProjectItemData* data)
	{
		switch (data->getType())
		{
		case PRJITEM_Texture:
			{
				ProjectResTexture* tex = static_cast<ProjectResTexture*>(data);
				
				for (int32 i=0;i<m_proplist.getCount();i++)
				{
					const PropItem& item = m_proplist[i];
					if (item.Name == L"DestinationFile")
					{
						item.getAsTextbox(tex->DestinationFile);
					}
					else if (item.Name == L"SourceFile")
					{
						item.getAsTextbox(tex->SourceFile);
					}
					else if (item.Name == L"GenerateMipmaps")
					{
						item.getAsCheckBox(tex->GenerateMipmaps);
					}
					else if (item.Name == L"Method")
					{
						String temp; item.getAsCombo(temp);
						tex->Method = ProjectTypeUtils::ParseTextureBuildMethod(temp);
					}
					else if (item.Name == L"Resize")
					{
						item.getAsCheckBox(tex->Resize);
					}
					else if (item.Name == L"ResizeFilterType")
					{
						String temp; item.getAsCombo(temp);
						tex->ResizeFilterType = ProjectTypeUtils::ParseTextureFilterType(temp);
					}
					else if (item.Name == L"NewFormat")
					{
						String temp; item.getAsCombo(temp);
						tex->NewFormat = PixelFormatUtils::ConvertFormat(temp);
					}
				}
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
				m_infoDisplay->Text.append(L"  ");

				ResourcePaneHelper::ItemTypeInformation* itemType =
					ResPaneHelperInstance.Lookup(item->getType());

				if (itemType)
					m_infoDisplay->Text.append(L"[" + itemType->Name + L"]\n");
				else
					m_infoDisplay->Text.append(L"[Unknown]\n");
				
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
					BuildInterface::getSingleton().AddSingleBuildItem(item);
					BuildInterface::getSingleton().Execute();
					BuildInterface::getSingleton().BlockedWait();
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
	void ResourcePane::BtnForceBuild_Release(Control* ctrl)
	{
		if (m_resourceView->getSelectedNode())
		{
			ProjectItem* item = static_cast<ProjectItem*>(m_resourceView->getSelectedNode()->UserData);
			if (item)
			{
				LogManager::getSingleton().Write(LOG_System, String(L"Building asset '") + item->getName() + String(L"'..."));
				BuildInterface::getSingleton().AddSingleBuildItem(item);
				BuildInterface::getSingleton().Execute();
				BuildInterface::getSingleton().BlockedWait();
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

				ApplyProperties(item->getData());

				item->NotifyModified();
			}
		}
		NukePropertyList();
	}
	void ResourcePane::BtnBrowseOpen_Release(Control* ctrl)
	{
		if (!m_currentProject)
			return;

		for (int32 i=0;i<m_proplist.getCount();i++)
		{
			if (m_proplist[i].ExtraButton == ctrl)
			{
				OpenFileDialog dlg;

				// make filter
				wchar_t filter[512];
				memset(filter, 0, sizeof(filter));

					
				String right = L"*.*";

				String left = L"All Files (";
				left.append(right);
				left.append(L")");
				memcpy(filter, left.c_str(), sizeof(wchar_t)*left.length());
				filter[left.length()] = 0;

				memcpy(filter+(left.length()+1),right.c_str(), sizeof(wchar_t)*right.length());

				dlg.SetFilter(filter);

				if (dlg.ShowDialog() == DLGRES_OK)
				{
					TextBox* tb = dynamic_cast<TextBox*>(m_proplist[i].Editor);
					if (tb)
					{
						const String& path	= dlg.getFilePath()[0];

						wchar_t buffer[260];
						PathRelativePathTo(buffer, m_currentProject->getBasePath().c_str(), FILE_ATTRIBUTE_DIRECTORY, path.c_str(),FILE_ATTRIBUTE_NORMAL);

						String result = buffer;
						String toIgnore(1, '.');
						toIgnore.append(1, PathUtils::DirectorySeparator);

						if (StringUtils::StartsWith(result, toIgnore))
							result = result.substr(2);

						tb->setText(result);
					}
				}

				break;
			}
		}
	}
	void ResourcePane::BtnBrowseSave_Release(Control* ctrl)
	{
		if (!m_currentProject)
			return;

		for (int32 i=0;i<m_proplist.getCount();i++)
		{
			if (m_proplist[i].ExtraButton == ctrl)
			{
				SaveFileDialog dlg;

				// make filter
				wchar_t filter[512];
				memset(filter, 0, sizeof(filter));

					
				String right = L"*.*";

				String left = L"All Files (";
				left.append(right);
				left.append(L")");
				memcpy(filter, left.c_str(), sizeof(wchar_t)*left.length());
				filter[left.length()] = 0;

				memcpy(filter+(left.length()+1),right.c_str(), sizeof(wchar_t)*right.length());

				dlg.SetFilter(filter);

				if (dlg.ShowDialog() == DLGRES_OK)
				{
					TextBox* tb = dynamic_cast<TextBox*>(m_proplist[i].Editor);
					if (tb)
					{
						const String& path	= dlg.getFilePath()[0];

						wchar_t buffer[260];
						PathRelativePathTo(buffer, m_currentProject->getOutputPath().c_str(), FILE_ATTRIBUTE_DIRECTORY, path.c_str(),FILE_ATTRIBUTE_NORMAL);

						String result = buffer;
						String toIgnore(1, '.');
						toIgnore.append(1, PathUtils::DirectorySeparator);

						if (StringUtils::StartsWith(result, toIgnore))
							result = result.substr(2);
						
						tb->setText(result);
					}
				}

				break;
			}
		}
	}


	int ResourcePane::getPropertyFieldWidth() const
	{
		return (m_form->Size.X-PropFieldMargin*3)/2 + 25;
	}
	int ResourcePane::getPropertyLabelWidth() const
	{
		return (m_form->Size.X-PropFieldMargin*3)/2 - 25;
	}
	int ResourcePane::getPropertyFieldTop() const
	{
		return m_infoDisplay->Position.Y + 80;
	}

	/************************************************************************/
	/*                                                                      */
	/************************************************************************/

	
	bool ResourcePane::PropItem::getAsCombo( String& val) const
	{
		ComboBox* cb = dynamic_cast<ComboBox*>(Editor);
		if (cb)
		{
			int idx = cb->getSelectedIndex();
			val = cb->getItems()[idx];
			return true;
		}
		return false;
	}

	bool ResourcePane::PropItem::getAsTextbox(String& val) const
	{
		TextBox* tb = dynamic_cast<TextBox*>(Editor);
		if (tb)
		{
			val = dynamic_cast<TextBox*>(Editor)->Text;
			return true;
		}
		return false;
	}

	bool ResourcePane::PropItem::getAsCheckBox(bool& val) const
	{
		CheckBox* cb = dynamic_cast<CheckBox*>(Editor);
		if (cb)
		{
			val = cb->getValue();
			return true;
		}
		return false;
	}
}