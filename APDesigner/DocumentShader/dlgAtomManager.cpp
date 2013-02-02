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
#include "dlgAtomManager.h"

#include "Core/Logging.h"
#include "Config/ConfigurationSection.h"
#include "Graphics/RenderSystem/Sprite.h"
#include "MainWindow.h"
#include "UILib/Form.h"
#include "UILib/List.h"
#include "UILib/Button.h"
#include "UILib/Label.h"
#include "Project/Project.h"
#include "UIResources.h"
#include "Document.h"
#include "ShaderAtomType.h"
#include "Graphics/GraphicsCommon.h"
#include "docShaderAtom.h"

#include "Vfs/File.h"
#include "Vfs/PathUtils.h"

#include "BuildService/BuildService.h"

#include "Utility/StringUtils.h"

using namespace Apoc3D::Utility;
using namespace Apoc3D::VFS;


namespace APDesigner
{
	AtomManagerDialog::AtomManagerDialog(MainWindow* window)
		: m_mainWindow(window), m_currentProject(0), m_skin(window->getUISkin())
	{
		m_form = new Form(FBS_Fixed);
		m_form->SetSkin(window->getUISkin());


		m_form->Size = Point(600,600);
		m_form->Position = Point(100, 100);

		m_form->Text = L"Shader Atom Manager";
		m_form->setTitle(m_form->Text);


		{
			List2D<String> emptyList(3, 2);

			ListView* listView = new ListView(Point(5, 25), Point(590, 500), emptyList);
			listView->SetSkin(window->getUISkin());
			
			listView->getColumnHeader().Add(ListView::Header(L"Name",150));
			listView->getColumnHeader().Add(ListView::Header(L"Type",150));
			listView->getColumnHeader().Add(ListView::Header(L"Profile",150));
			listView->setFullRowSelect(true);

			m_form->getControls().Add(listView);
			m_atomList = listView;
			//m_resourceView->eventSelectionChanged().bind(this, &ResourcePane::TreeView_SelectionChanged);
		}


		m_addItem = new Button(Point(10,550),L"Add...");
		m_addItem->Size.X = 100;
		m_addItem->SetSkin(window->getUISkin());
		m_addItem->eventRelease().bind(this, &AtomManagerDialog::BtnAdd_Release);
		m_form->getControls().Add(m_addItem);

		m_removeItem = new Button(Point(120+10,550),L"Delete");
		m_removeItem->Size.X = 100;
		m_removeItem->SetSkin(window->getUISkin());
		m_removeItem->eventRelease().bind(this, &AtomManagerDialog::BtnRemove_Release);
		m_form->getControls().Add(m_removeItem);

		m_editItem = new Button(Point(240+10,550),L"Open");
		m_editItem->Size.X = 100;
		m_editItem->SetSkin(window->getUISkin());
		m_editItem->eventRelease().bind(this, &AtomManagerDialog::BtnEdit_Release);
		m_form->getControls().Add(m_editItem);


	}

	AtomManagerDialog::~AtomManagerDialog()
	{
		delete m_form;
		delete m_addItem;
		delete m_removeItem;
		delete m_editItem;
	}

	void AtomManagerDialog::Initialize(RenderDevice* device)
	{
		m_form->Initialize(device);

		UIRoot::Add(m_form);

		UpdateAtomListView();
	}

	void AtomManagerDialog::Update(const GameTime* const time)
	{

	}
	void AtomManagerDialog::UpdateToNewProject(Project* prj)
	{
		m_currentProject = prj;
		UpdateAtomListView();
	}
	void AtomManagerDialog::UpdateAtomListView()
	{
		m_atomList->getItems().Clear();

		ShaderAtomLibraryManager::LibraryEnumerator e = ShaderAtomLibraryManager::getSingleton().GetEnumerator();
		for (;e.MoveNext();)
		{
			ShaderAtomType* type = *e.getCurrentValue();

			String row[3];
			row[0] = *e.getCurrentKey();
			
			row[1] = GraphicsCommonUtils::ToString(type->getShaderType());
			
			row[2] = L"SM ";
			row[2].append(StringUtils::ToString(type->getMajorSMVersion()));
			row[2].append(L".");
			row[2].append(StringUtils::ToString(type->getMinorSMVersion()));

			m_atomList->getItems().AddRow(row);
		}

		if (m_currentProject)
		{

		}
	}

	void AtomManagerDialog::Show()
	{
		m_form->Show();
	}
	void AtomManagerDialog::Hide()
	{
		m_form->Close();
	}

	void AtomManagerDialog::BtnAdd_Release(Control* ctrl)
	{

	}
	void AtomManagerDialog::BtnRemove_Release(Control* ctrl)
	{

	}
	void AtomManagerDialog::BtnEdit_Release(Control* ctrl)
	{
		int index = m_atomList->getSelectedRowIndex();

		if (index !=-1)
		{
			String name = m_atomList->getItems().at(index,0);
			ShaderAtomType* type =  ShaderAtomLibraryManager::getSingleton().FindAtomType(name);
			if (type)
			{
				ShaderAtomDocument* doc = new ShaderAtomDocument(m_mainWindow, nullptr, name);
				
				m_mainWindow->AddDocument(doc);
				doc->LoadRes();
			}
		}
		
	}
}