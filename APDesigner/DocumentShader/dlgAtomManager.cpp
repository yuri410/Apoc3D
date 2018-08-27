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

#include "dlgAtomManager.h"

#include "ShaderAtomType.h"
#include "docShaderAtom.h"
#include "APDesigner/MainWindow.h"
#include "APDesigner/BuildService/BuildService.h"
#include "APDesigner/UIResources.h"
#include "APDesigner/Document.h"

namespace APDesigner
{
	AtomManagerDialog::AtomManagerDialog(MainWindow* window)
		: m_mainWindow(window), m_currentProject(0), m_skin(window->getUISkin())
	{
		RenderDevice* device = window->getDevice();

		m_form = new Form(m_skin, device, FBS_Fixed);

		m_form->setSize(600,600);
		m_form->Position = Point(100, 100);

		m_form->setTitle(L"Shader Atom Manager");


		{
			List2D<String> emptyList(3, 2);

			ListView* listView = new ListView(m_skin, Point(5, 25), Point(590, 500), emptyList);
			
			listView->getColumnHeader().Add(ListView::Header(L"Name",150));
			listView->getColumnHeader().Add(ListView::Header(L"Type",150));
			listView->getColumnHeader().Add(ListView::Header(L"Profile",150));
			listView->FullRowSelect = true;

			m_form->getControls().Add(listView);
			m_atomList = listView;
			//m_resourceView->eventSelectionChanged().bind(this, &ResourcePane::TreeView_SelectionChanged);
		}


		m_addItem = new Button(m_skin, Point(10, 550), 100, L"Add...");
		m_addItem->eventRelease.Bind(this, &AtomManagerDialog::BtnAdd_Release);
		m_form->getControls().Add(m_addItem);

		m_removeItem = new Button(m_skin, Point(120+10,550), 100, L"Delete");
		m_removeItem->eventRelease.Bind(this, &AtomManagerDialog::BtnRemove_Release);
		m_form->getControls().Add(m_removeItem);

		m_editItem = new Button(m_skin, Point(240+10,550), 100, L"Open");
		m_editItem->eventRelease.Bind(this, &AtomManagerDialog::BtnEdit_Release);
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
		SystemUI::Add(m_form);

		UpdateAtomListView();
	}

	void AtomManagerDialog::Update(const GameTime* time)
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

		for (auto e : ShaderAtomLibraryManager::getSingleton())
		{
			ShaderAtomType* type = e.Value;

			String row[3];
			row[0] = e.Key;
			
			row[1] = ShaderTypeConverter.ToString(type->getShaderType());
			
			row[2] = L"SM ";
			row[2].append(StringUtils::IntToString(type->getMajorSMVersion()));
			row[2].append(L".");
			row[2].append(StringUtils::IntToString(type->getMinorSMVersion()));

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

	void AtomManagerDialog::BtnAdd_Release(Button* ctrl)
	{

	}
	void AtomManagerDialog::BtnRemove_Release(Button* ctrl)
	{

	}
	void AtomManagerDialog::BtnEdit_Release(Button* ctrl)
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