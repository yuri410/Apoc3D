#pragma once
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

#include "APDCommon.h"

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

		void Update(const GameTime* const time);

		void UpdateToNewProject(Project* prj);

	private:
		
		void BuildTreeViewNodes(TreeViewNode* parentNode, const FastList<ProjectItem*> items);
		void BuildTreeViewNodes(const FastList<ProjectItem*> items);

		void BtnAdd_Release(Control* ctrl);
		void BtnRemove_Release(Control* ctrl);
		void BtnOpen_Release(Control* ctrl);
		void BtnForceBuild_Release(Control* ctrl);
		void BtnApplyMod_Release(Control* ctrl);

		void BtnBrowseOpen_Release(Control* ctrl);
		void BtnBrowseSave_Release(Control* ctrl);

		void TreeView_SelectionChanged(Control* ctrl);

		void NukePropertyList();
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

		Project* m_currentProject;

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
			{
			}


			bool getAsTextbox(String& val) const;
			bool getAsCombo(String& val) const;
			bool getAsCheckBox(bool& val) const;
		};
		
		FastList<PropItem> m_proplist;
	};

	//class PropertyPane
	//{
	//private:
	//	ObjectPropertyEditor* m_currentEditor;

	//	
	//public:

	//	PropertyPane(MainWindow* window);

	//	void Initialize(RenderDevice* device);
	//	void Update(const GameTime* const time);
	//};
}