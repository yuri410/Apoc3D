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

#include <Shlwapi.h>

using namespace APDesigner::CommonDialog;

#pragma comment (lib, "Shlwapi.lib")

namespace APDesigner
{
	/************************************************************************/
	/*                                                                      */
	/************************************************************************/
	ToolsPane::ToolsPane(MainWindow* window)
		: m_mainWindow(window), m_currentTools(nullptr)
	{
		m_pane = new Form(window->getUISkin(), window->getDevice(), FBS_Pane);
		m_pane->IsBackgroundForm = true;

		m_pane->setMinimumSize(Point(50,50));
		m_pane->setSize(50, 600);
		m_pane->Position = Point(0, window->getMenuBar()->getHeight());

		m_pane->setTitle(L"");
		//m_pane->setTitle(m_pane->Text);

		m_btSave = new Button(window->getUISkin(), Point(7, 10), Point(35, 35), L"");
		m_btSave->OverlayIcon = UIGraphicSimple(UIResources::GetTexture(L"adui_save"));
		m_btSave->DisabledOverlayIcon = m_btSave->OverlayIcon;
		m_btSave->DisabledOverlayIcon.ModColor = CV_PackLA(0xff, 64);

		//m_btSave->setCustomModColorMouseOver(CV_Silver);
		
		m_btSave->eventRelease.Bind(this,&ToolsPane::BtnSave_Release);
		m_pane->getControls().Add(m_btSave);
	}
	ToolsPane::~ToolsPane()
	{
		delete m_pane;
		delete m_btSave;
	}

	void ToolsPane::BtnSave_Release(Button* ctrl)
	{
		if (m_mainWindow->getCurrentDocument())
		{
			/*IndenpendentEditor* ide = up_cast<IndenpendentEditor*>(m_mainWindow->getCurrentDocument());

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
			else*/
			{
				m_mainWindow->getCurrentDocument()->SaveRes();
			}
			
		}
		//m_mainWindow->SaveProject();
	}

	void ToolsPane::Initialize(RenderDevice* device)
	{
		SystemUI::Add(m_pane);
		m_pane->Show();
	}

	void ToolsPane::Update(const GameTime* time)
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

		m_pane->Position.X = m_mainWindow->getUIAreaSize().X - m_pane->getWidth();
		m_pane->Position.Y = m_mainWindow->getMenuBar()->getHeight();

		m_pane->setHeight(m_mainWindow->getUIAreaSize().Y - m_pane->Position.Y);
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
				AddEntry(ProjectItemType::Texture, iti);
			}
			{
				ItemTypeInformation iti = { L"adui_NewProject1", L"Folder" };
				AddEntry(ProjectItemType::Folder, iti);
			}
			{
				ItemTypeInformation iti = { L"adui_Scene", L"Model" };
				AddEntry(ProjectItemType::Model, iti);
			}
			{
				ItemTypeInformation iti = { L"adui_effects_file", L"Effect" };
				AddEntry(ProjectItemType::Effect, iti);
			}
			{
				ItemTypeInformation iti = { L"adui_effects_file", L"Custom Effect" };
				AddEntry(ProjectItemType::CustomEffect, iti);
			}
			{
				ItemTypeInformation iti = { L"adui_new_document", L"Font" };
				AddEntry(ProjectItemType::Font, iti);
			}
			{
				ItemTypeInformation iti = { L"adui_new_document", L"FontCheck" };
				AddEntry(ProjectItemType::FontGlyphDist, iti);
			}
			{
				ItemTypeInformation iti = { L"adui_material_32x32", L"Material" };
				AddEntry(ProjectItemType::Material, iti);
			}
			{
				ItemTypeInformation iti = { L"adui_import1", L"MaterialSet" };
				AddEntry(ProjectItemType::MaterialSet, iti);
			}
			{
				ItemTypeInformation iti = { L"adui_skinning", L"Transform Anim" };
				AddEntry(ProjectItemType::TransformAnimation, iti);
			}
			{
				ItemTypeInformation iti = { L"adui_surface", L"Material Anim" };
				AddEntry(ProjectItemType::MaterialAnimation, iti);
			}
			{
				ItemTypeInformation iti = { L"adui_add_exisiting_document", L"Direct Copy" };
				AddEntry(ProjectItemType::Copy, iti);
			}
		}

		ItemTypeInformation* Lookup(ProjectItemType type) { return m_lookupTable.TryGetValue(static_cast<int32>(type)); }
	private:
		void AddEntry(ProjectItemType type, const ItemTypeInformation& itemInfo)
		{
			m_lookupTable.Add(static_cast<int32>(type), itemInfo);
		}

		HashMap<int32, ItemTypeInformation> m_lookupTable;
	};

	static ResourcePaneHelper ResPaneHelperInstance;

	/************************************************************************/
	/* ResourcePane                                                         */
	/************************************************************************/
	
	ResourcePane::ResourcePane(MainWindow* window)
		: m_mainWindow(window), m_currentProject(0), m_skin(window->getUISkin())
	{
		RenderDevice* device = window->getDevice();

		m_form = new Form(m_skin, device, FBS_Pane);
		m_form->IsBackgroundForm = true;

		m_form->setSize(300, 600);
		m_form->Position = Point(0, window->getMenuBar()->getHeight());
		
		m_form->setTitle(L"Resource Explorer.");

		{
			TreeView* treeview = new TreeView(m_skin, Point(5, 25), 200, 300);

			TreeViewNode* node1 = new TreeViewNode(L"No project opened");
			treeview->getNodes().Add(node1);

			m_form->getControls().Add(treeview);
			m_resourceView = treeview;
			m_resourceView->eventSelectionChanged.Bind(this, &ResourcePane::TreeView_SelectionChanged);
		}

		m_infoDisplay = new Label(m_skin, Point(5, 355), L"");
		m_form->getControls().Add(m_infoDisplay);

		m_addItem = new Button(m_skin, Point(5,325),L"Add...");
		m_addItem->eventRelease.Bind(this, &ResourcePane::BtnAdd_Release);
		m_form->getControls().Add(m_addItem);

		m_removeItem = new Button(m_skin, Point(m_addItem->Position.X + m_addItem->getWidth(), 325), L"Delete");
		m_removeItem->eventRelease.Bind(this, &ResourcePane::BtnRemove_Release);
		m_form->getControls().Add(m_removeItem);

		m_openItem = new Button(m_skin, Point(m_removeItem->Position.X + m_removeItem->getWidth(), 325),L"Open");
		m_openItem->eventRelease.Bind(this, &ResourcePane::BtnOpen_Release);
		m_form->getControls().Add(m_openItem);

		m_forceBuildItem = new Button(m_skin, Point(m_removeItem->Position.X + m_removeItem->getWidth(), 325),L"Force Build");
		m_forceBuildItem->eventRelease.Bind(this, &ResourcePane::BtnForceBuild_Release);
		m_form->getControls().Add(m_forceBuildItem);

		m_applyModify = new Button(m_skin, Point(5,400),L"Apply Settings");
		m_applyModify->eventRelease.Bind(this, &ResourcePane::BtnApplyMod_Release);
		m_applyModify->Visible = false;
		m_form->getControls().Add(m_applyModify);
	}
	ResourcePane::~ResourcePane()
	{
		NukePropertyList(false);
		delete m_form;
		delete m_infoDisplay;
		delete m_addItem;
		delete m_removeItem;
		delete m_openItem;
		delete m_applyModify;

		delete m_resourceView;
	}


	void ResourcePane::Initialize(RenderDevice* device)
	{
		m_removeItem->Position.X = m_addItem->Position.X + m_addItem->getWidth() + 5;
		m_openItem->Position.X = m_removeItem->Position.X + m_removeItem->getWidth() + 5;
		m_forceBuildItem->Position.X = m_openItem->Position.X + m_openItem->getWidth() + 5;

		SystemUI::Add(m_form);
		m_form->Show();
	}

	void ResourcePane::Update(const GameTime* time)
	{
		m_form->Position.X = 0;
		m_form->Position.Y = m_mainWindow->getMenuBar()->getHeight();
		m_form->setHeight(m_mainWindow->getUIAreaSize().Y - m_form->Position.Y);
		Point newSize = m_form->getSize();
		newSize.X -= 10;
		newSize.Y -= 30+400;
		m_resourceView->SetSize(newSize);
	
		m_forceBuildItem->Position.Y = m_removeItem->Position.Y = m_addItem->Position.Y = m_openItem->Position.Y = m_resourceView->getHeight() + m_resourceView->Position.Y + + 5;

		m_infoDisplay->Position.Y = m_removeItem->Position.Y + m_removeItem->getHeight() + 5;

		m_applyModify->Position.Y = m_infoDisplay->Position.Y + 45;

		int sy = getPropertyFieldTop();
		for (int32 i=0;i<m_proplist.getCount();i++)
		{
			PropItem& pi = m_proplist[i];

			int top = sy + i * PropFieldSpacing;
			if (pi.Desc)
			{
				pi.Desc->Position.Y = top;
			}
			if (pi.Editor)
			{
				pi.Editor->Position.Y = top;
			}
			if (pi.ExtraButton)
			{
				pi.ExtraButton->Position.Y = top;
			}
		}
	}

	void ResourcePane::UpdateToNewProject(Project* prj)
	{
		m_currentProject = prj;

		m_resourceView->NukeTreeViewNodes();

		const List<ProjectItem*>& items = prj->getItems();
		BuildTreeViewNodes(items);
	}



	void ResourcePane::BuildTreeViewNodes(TreeViewNode* parentNode, const List<ProjectItem*> items)
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

				ProjectFolder* fld = up_cast<ProjectFolder*>(data);
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
	void ResourcePane::BuildTreeViewNodes(const List<ProjectItem*> items)
	{
		BuildTreeViewNodes(0, items);
	}

	void ResourcePane::NukePropertyList(bool deferred)
	{
		for (int i=0;i<m_proplist.getCount();i++)
		{
			m_proplist[i].Destory(m_form, deferred);
		}
		m_proplist.Clear();

		m_applyModify->Visible = false;
	}
	void ResourcePane::AddPropertyPair(const String& a, const String& b)
	{
		int top = getPropertyFieldTop() + m_proplist.getCount() * PropFieldSpacing;
		
		int lw = getPropertyLabelWidth();
		int fw = getPropertyFieldWidth();

		Label* label = new Label(m_skin, Point(PropFieldMargin, top), a, lw);
		TextBox* tb = new TextBox(m_skin, Point(PropFieldMargin*2+lw, top), fw, b);

		PropItem item(a, label, tb, nullptr);
		
		item.Add(m_form, true);
		m_proplist.Add(item);
	}
	void ResourcePane::AddPropertyPath(const String& a, const String& b, bool isLoad)
	{
		int top = getPropertyFieldTop() + m_proplist.getCount() * PropFieldSpacing;

		int lw = getPropertyLabelWidth();
		int fw = getPropertyFieldWidth();

		Label* label = new Label(m_skin, Point(PropFieldMargin, top), a, lw);
		TextBox* tb = new TextBox(m_skin, Point(PropFieldMargin*2+lw, top), fw-30, b);
		Button* bb = new Button(m_skin, Point(tb->Position.X + tb->getWidth(), top), 30, L"...");
		bb->SetSizeY(m_skin->TitleTextFont->getLineHeightInt());

		PropItem item(a, label, tb, bb);
		item.LoadOrSave = isLoad;

		if (isLoad)
			bb->eventRelease.Bind(this, &ResourcePane::BtnBrowseOpen_Release);
		else
			bb->eventRelease.Bind(this, &ResourcePane::BtnBrowseSave_Release);

		item.Add(m_form, true);
		m_proplist.Add(item);

	}
	void ResourcePane::AddPropertyDropdown(const String& name, const List<String>& list, int selectedIndex)
	{
		int top = getPropertyFieldTop() + m_proplist.getCount() * PropFieldSpacing;

		int lw = getPropertyLabelWidth();
		int fw = getPropertyFieldWidth();

		Label* label = new Label(m_skin, Point(PropFieldMargin, top), name, lw);
		ComboBox* combo = new ComboBox(m_skin, Point(PropFieldMargin*2+lw, top), fw, list);
		
		PropItem item(name, label, combo, nullptr);
		
		if (selectedIndex != -1)
			combo->setSelectedIndex(selectedIndex);

		item.Add(m_form, true);
		m_proplist.Add(item);
	}
	void ResourcePane::AddPropertyCheckbox(const String& name, bool checked)
	{
		int top = getPropertyFieldTop() + m_proplist.getCount() * PropFieldSpacing;

		int lw = getPropertyLabelWidth();
		int fw = getPropertyFieldWidth();

		CheckBox* cb = new CheckBox(m_skin, Point(PropFieldMargin*2+lw, top), name, checked);
		
		PropItem item(name, nullptr, cb, nullptr);
		
		item.Add(m_form, true);
		m_proplist.Add(item);
	}
	void ResourcePane::ListNewProperties(ProjectItemData* data)
	{
		NukePropertyList(true);
		switch (data->getType())
		{
		case ProjectItemType::Texture:
			{
				ProjectResTexture* tex = static_cast<ProjectResTexture*>(data);

				AddPropertyPath(L"DestinationFile", tex->DestinationFile, false);
				AddPropertyPath(L"SourceFile", tex->SourceFile, true);

				AddPropertyCheckbox(L"GenerateMipmaps", tex->GenerateMipmaps);

				List<String> items;
				ProjectUtils::TextureBuildMethodConv.DumpNames(items);
				AddPropertyDropdown(L"Method", items, items.IndexOf(ProjectUtils::TextureBuildMethodConv.ToString(tex->Method)));
				
				AddPropertyPair(L"Resizing", tex->Resizing.ToString());

				items.Clear();
				ProjectUtils::TextureFilterTypeConv.DumpNames(items);
				AddPropertyDropdown(L"ResizeFilterType", items, items.IndexOf(ProjectUtils::TextureFilterTypeConv.ToString(tex->ResizeFilterType)));

				items.Clear();
				PixelFormatUtils::DumpPixelFormatName(items);
				AddPropertyDropdown(L"NewFormat", items, items.IndexOf(PixelFormatUtils::ToString(tex->NewFormat)));
			}
			break;
		case ProjectItemType::Model:
			{
				ProjectResModel* mdl = static_cast<ProjectResModel*>(data);
				
				AddPropertyPath(L"DstFile", mdl->DstFile, false);
				AddPropertyPath(L"DstAnimationFile", mdl->DstAnimationFile, false);
				AddPropertyPath(L"SrcFile", mdl->SrcFile, true);


				List<String> items;
				ProjectUtils::MeshBuildMethodConv.DumpNames(items);
				AddPropertyDropdown(L"Method", items, items.IndexOf(ProjectUtils::MeshBuildMethodConv.ToString(mdl->Method)));
			}
			break;
		case ProjectItemType::Font:
			{
				//ProjectResFont* fnt = static_cast<ProjectResFont*>(data);


			}
			break;
		case ProjectItemType::Folder:
			{
				ProjectFolder* folder = static_cast<ProjectFolder*>(data);
				AddPropertyPath(L"DstPack", folder->DestinationPack, false);
				AddPropertyPair(L"PackType", folder->PackType);
			}
			break;
		case ProjectItemType::Effect:
			{

			}
			break;
		case ProjectItemType::Material:
			{
				ProjectResMaterial* mtrl = static_cast<ProjectResMaterial*>(data);

				AddPropertyPath(L"DstFile", mtrl->DestinationFile, false);
			}
			break;
		case ProjectItemType::TransformAnimation:
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
		case ProjectItemType::Texture:
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
						tex->Method = ProjectUtils::TextureBuildMethodConv.Parse(temp);
					}
					else if (item.Name == L"Resizing")
					{
						String txt;
						item.getAsTextbox(txt);
						tex->Resizing.Parse(txt);
					}
					else if (item.Name == L"ResizeFilterType")
					{
						String temp; item.getAsCombo(temp);
						tex->ResizeFilterType = ProjectUtils::TextureFilterTypeConv.Parse(temp);
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
				String msg = item->getName() + L"  ";
				
				ResourcePaneHelper::ItemTypeInformation* itemType =
					ResPaneHelperInstance.Lookup(item->getType());

				if (itemType)
					msg.append(L"[" + itemType->Name + L"]\n");
				else
					msg.append(L"[Unknown]\n");
				
				if (item->IsOutDated())
				{
					msg.append(L"Built(but outdated).");
				}
				else
				{
					msg.append(L"Built(up to date).");
				}

				m_infoDisplay->SetText(msg);

				ListNewProperties(item->getData());
			}
			
		}
	}

	void ResourcePane::BtnAdd_Release(Button* ctrl)
	{

	}
	void ResourcePane::BtnRemove_Release(Button* ctrl)
	{

	}
	void ResourcePane::BtnOpen_Release(Button* ctrl)
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
				case ProjectItemType::Texture:
					{
						ProjectResTexture* tex = static_cast<ProjectResTexture*>(item->getData());
						String path = tex->GetAbsoluteDestinationPath(tex->DestinationFile);
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
	void ResourcePane::BtnForceBuild_Release(Button* ctrl)
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
	void ResourcePane::BtnApplyMod_Release(Button* ctrl)
	{
		if (m_resourceView->getSelectedNode())
		{
			ProjectItem* item = static_cast<ProjectItem*>(m_resourceView->getSelectedNode()->UserData);
			if (item)
			{
				//ConfigurationSection* temp = new ConfigurationSection(L"temp");
				//item->getData()->Save(temp, false);

				ApplyProperties(item->getData());

				//item->NotifyModified();
			}
		}
		NukePropertyList(true);
	}

	void ResourcePane::BtnBrowseOpen_Release(Button* ctrl) { OpenPaneFilePathBrowse(ctrl, true); }
	void ResourcePane::BtnBrowseSave_Release(Button* ctrl) { OpenPaneFilePathBrowse(ctrl, false); }

	void ResourcePane::OpenPaneFilePathBrowse(Button* ctrl, bool load)
	{
		if (m_currentProject == nullptr || m_selectedItem == nullptr)
			return;

		for (int32 i = 0; i < m_proplist.getCount(); i++)
		{
			const PropItem& pi = m_proplist[i];
			if (pi.ExtraButton == ctrl)
			{
				bool done = false;
				String selectedFilePath;
				if (load)
				{
					OpenFileDialog dlg;
					dlg.SetAllFilesFilter();

					if (dlg.ShowDialog() == DLGRES_OK)
					{
						done = true;
						selectedFilePath = dlg.getFilePath()[0];
					}
				}
				else
				{
					SaveFileDialog dlg;
					dlg.SetAllFilesFilter();

					if (dlg.ShowDialog() == DLGRES_OK)
					{
						done = true;
						selectedFilePath = dlg.getFilePath()[0];
					}
				}


				if (done)
				{
					TextBox* tb = up_cast<TextBox*>(pi.Editor);
					if (tb)
					{
						ProjectItemData* itmData = m_selectedItem->getData();
						
						String result;
						if (!PathUtils::GetRelativePath(itmData->GetAbsoluteSourcePathBase(false), selectedFilePath, result))
						{
							result = selectedFilePath;
						}

						String toIgnore(1, '.');
						toIgnore.append(1, PathUtils::DirectorySeparator);

						if (StringUtils::StartsWith(result, toIgnore))
							result = result.substr(2);

						tb->SetText(result);
					}
				}

				break;
			}
		}
	}

	int ResourcePane::getPropertyFieldWidth() const
	{
		return (m_form->getWidth() - PropFieldMargin * 3) / 2 + 25;
	}
	int ResourcePane::getPropertyLabelWidth() const
	{
		return (m_form->getWidth() - PropFieldMargin * 3) / 2 - 25;
	}
	int ResourcePane::getPropertyFieldTop() const
	{
		return m_infoDisplay->Position.Y + 80;
	}

	void ResourcePane::PropItem::Add(Form* pane, bool deferred)
	{
		if (deferred)
		{
			if (Desc)
				pane->getControls().DeferredAdd(Desc);

			if (Editor)
				pane->getControls().DeferredAdd(Editor);

			if (ExtraButton)
				pane->getControls().DeferredAdd(ExtraButton);
		}
		else
		{
			if (Desc)
				pane->getControls().Add(Desc);

			if (Editor)
				pane->getControls().Add(Editor);

			if (ExtraButton)
				pane->getControls().Add(ExtraButton);
		}
	}
	void ResourcePane::PropItem::Destory(Form* pane, bool deferred)
	{
		if (deferred)
		{
			if (Desc)
				pane->getControls().DeferredRemoval(Desc, true);

			if (Editor)
				pane->getControls().DeferredRemoval(Editor, true);

			if (ExtraButton)
				pane->getControls().DeferredRemoval(ExtraButton, true);
		}
		else
		{
			if (Desc)
			{
				pane->getControls().Remove(Desc);
				delete Desc;
			}

			if (Editor)
			{
				pane->getControls().Remove(Editor);
				delete Editor;
			}

			if (ExtraButton)
			{
				pane->getControls().Remove(ExtraButton);
				delete ExtraButton;
			}
		}
	}

	/************************************************************************/
	/*                                                                      */
	/************************************************************************/

	
	bool ResourcePane::PropItem::getAsCombo( String& val) const
	{
		ComboBox* cb = up_cast<ComboBox*>(Editor);
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
		TextBox* tb = up_cast<TextBox*>(Editor);
		if (tb)
		{
			val = tb->getText();
			return true;
		}
		return false;
	}

	bool ResourcePane::PropItem::getAsCheckBox(bool& val) const
	{
		CheckBox* cb = up_cast<CheckBox*>(Editor);
		if (cb)
		{
			val = cb->Checked;
			return true;
		}
		return false;
	}
}