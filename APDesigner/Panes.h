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
	class ToolsPane
	{
	public:
		ToolsPane(MainWindow* window);
		~ToolsPane();

		void Initialize(RenderDevice* device);

		void Update(const GameTime* time);

		Form* getPane() const { return m_pane; }
	private:
		void BtnSave_Release(Button* ctrl);

		MainWindow* m_mainWindow;

		Form* m_pane;
		Button* m_btSave;

		ObjectTools* m_currentTools;

	};

	//class PropEditor
	//{
	//public:
	//	virtual void Show() = 0;
	//	virtual void Hide() = 0;
	//};
	//class TexturePropEditor : public PropEditor
	//{
	//public:
	//	TexturePropEditor(ResourcePane* pane);
	//	~TexturePropEditor();

	//	virtual void Show();
	//	virtual void Hide();

	//private:
	//	ComboBox* m_cbMethod;
	//	//ComboBox* m_cb
	//	
	//};

	class ResourcePane
	{
	public:
		ResourcePane(MainWindow* window);
		~ResourcePane();

		void Initialize(RenderDevice* device);

		void Update(const GameTime* time);

		void UpdateToNewProject(Project* prj);

	private:
		
		void BuildTreeViewNodes(TreeViewNode* parentNode, const List<ProjectItem*> items);
		void BuildTreeViewNodes(const List<ProjectItem*> items);

		void BtnAdd_Release(Button* ctrl);
		void BtnRemove_Release(Button* ctrl);
		void BtnOpen_Release(Button* ctrl);
		void BtnForceBuild_Release(Button* ctrl);
		void BtnApplyMod_Release(Button* ctrl);

		void OpenPaneFilePathBrowse(Button* ctrl,  bool load);

		void BtnBrowseOpen_Release(Button* ctrl);
		void BtnBrowseSave_Release(Button* ctrl);

		void TreeView_SelectionChanged(Control* ctrl);

		void NukePropertyList(bool deferred);
		void AddPropertyPair(const String& name, const String& b);
		void AddPropertyPath(const String& name, const String& b, bool isLoad);
		void AddPropertyDropdown(const String& name, const List<String>& list, int selectedIndex = -1);
		void AddPropertyCollection(const String& name);
		void AddPropertyCheckbox(const String& name, bool checked);

		void ListNewProperties(ProjectItemData* data);
		void ApplyProperties(ProjectItemData* data);

		int getPropertyLabelWidth() const;
		int getPropertyFieldWidth() const;
		int getPropertyFieldTop() const;

		static const int32 PropFieldMargin = 5;
		static const int32 PropFieldSpacing = 25;

		MainWindow* m_mainWindow;
		const StyleSkin* m_skin;

		Form* m_form;

		TreeView* m_resourceView;
		Label* m_infoDisplay;
		Button* m_addItem;
		Button* m_removeItem;
		Button* m_openItem;
		Button* m_forceBuildItem;

		Button* m_applyModify;

		Project* m_currentProject = nullptr;
		ProjectItem* m_selectedItem = nullptr;

		struct PropItem
		{
			String Name;
			Label* Desc;
			Control* Editor;

			Button* ExtraButton;

			bool LoadOrSave;

			PropItem() : Desc(nullptr), Editor(nullptr), ExtraButton(nullptr), LoadOrSave(false) { }
			PropItem(const String& name, Label* l, Control* edi, Button* eb)
				: Name(name), Desc(l), Editor(edi), ExtraButton(eb), LoadOrSave(false)
			{ }


			bool getAsTextbox(String& val) const;
			bool getAsCombo(String& val) const;
			bool getAsCheckBox(bool& val) const;

			void Add(Form* pane, bool deferred);
			void Destory(Form* pane, bool deferred);
		};
		
		List<PropItem> m_proplist;
	};

	//class PropertyPane
	//{
	//private:
	//	ObjectPropertyEditor* m_currentEditor;

	//	
	//public:

	//	PropertyPane(MainWindow* window);

	//	void Initialize(RenderDevice* device);
	//	void Update(const GameTime* time);
	//};
}