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

#include "docShaderAtom.h"

#include "ShaderAtomType.h"
#include "APDesigner/MainWindow.h"

namespace APDesigner
{
	ShaderAtomDocument::ShaderAtomDocument(MainWindow* window, EditorExtension* ext, const String& name)
		: Document(window, ext), m_atomName(name), m_currentWorkingCopy(0)
	{
		getDocumentForm()->setTitle(L"Shader Atom: " + name);
		getDocumentForm()->setMinimumSize(Point(970,635));

		const StyleSkin* skin = window->getUISkin();

		{
			Label* lbl = new Label(skin, Point(23, 33), L"Name", 80);
			m_labels.Add(lbl);

			m_tbName = new TextBox(skin, Point(100, 33), 120);

			lbl = new Label(skin, Point(250, 33), L"Type", 80);
			m_labels.Add(lbl);

			List<String> items;
			items.Add(ShaderTypeConverter.ToString(ShaderType::Vertex));
			items.Add(ShaderTypeConverter.ToString(ShaderType::Pixel));
			items.Add(ShaderTypeConverter.ToString(ShaderType::All));
			m_cbShaderType = new ComboBox(skin, Point(330, 33), 120, items);


			lbl = new Label(skin, Point(23, 66), L"Profile", 80);
			m_labels.Add(lbl);

			items.Clear();
			items.Add(L"Shader Model 1");
			items.Add(L"Shader Model 2");
			items.Add(L"Shader Model 3");

			m_cbProfile = new ComboBox(skin, Point(100, 66), 120, items);

			m_tbCode = new TextBox(skin, Point(33, 100),500,500,L"");
			m_tbCode->EnableAllScrollBars();
		}
		{
			List2D<String> emptyList(3, 2);

			m_lbPorts = new ListView(skin, Point(550, 25), Point(400, 550), emptyList);

			m_lbPorts->getColumnHeader().Add(ListView::Header(L"Name",150));
			m_lbPorts->getColumnHeader().Add(ListView::Header(L"Type",75));
			m_lbPorts->getColumnHeader().Add(ListView::Header(L"Format",75));
			m_lbPorts->getColumnHeader().Add(ListView::Header(L"Usage",100));
			m_lbPorts->FullRowSelect = true;

		}
	}

	ShaderAtomDocument::~ShaderAtomDocument()
	{
		delete m_tbName;
		delete m_cbShaderType;
		delete m_tbCode;

		for (int i=0;i<m_labels.getCount();i++)
		{
			delete m_labels[i];
		}
	}
	void ShaderAtomDocument::Initialize(RenderDevice* device)
	{
		getDocumentForm()->getControls().Add(m_tbName);
		getDocumentForm()->getControls().Add(m_tbCode);
		getDocumentForm()->getControls().Add(m_cbShaderType);
		getDocumentForm()->getControls().Add(m_cbProfile);
		getDocumentForm()->getControls().Add(m_lbPorts);

		for (int i=0;i<m_labels.getCount();i++)
		{
			getDocumentForm()->getControls().Add(m_labels[i]);
		}

		Document::Initialize(device);

	}

	void ShaderAtomDocument::LoadRes()
	{
		ShaderAtomType* requested = ShaderAtomLibraryManager::getSingleton().FindAtomType(m_atomName);
		if (requested)
		{
			if (m_currentWorkingCopy)
			{
				delete m_currentWorkingCopy;
			}

			m_currentWorkingCopy = new ShaderAtomType(*requested);
			
			m_tbCode->SetText(m_currentWorkingCopy->getCodeBody());
			m_tbName->SetText(m_currentWorkingCopy->getName());

			if (m_currentWorkingCopy->getMajorSMVersion()>0 && 
				m_currentWorkingCopy->getMajorSMVersion()-1<m_cbProfile->getItems().getCount())
			{
				m_cbProfile->setSelectedIndex(m_currentWorkingCopy->getMajorSMVersion()-1);
			}

			switch (m_currentWorkingCopy->getShaderType())
			{
				case ShaderType::All:
					m_cbShaderType->setSelectedIndex(2);
					break;
				case ShaderType::Pixel:
					m_cbShaderType->setSelectedIndex(1);
					break;
				case ShaderType::Vertex:
					m_cbShaderType->setSelectedIndex(0);
					break;
			}

			for (int i=0;i<m_currentWorkingCopy->Ports().getCount();i++)
			{
				ShaderAtomPort& port = m_currentWorkingCopy->Ports()[i];
				String row[4];
				row[0] = port.Name;

				row[1] = port.IsInputOrOutput ? L"In" : L"Out";

				if (port.Usage == EPUSAGE_Unknown)
				{
					row[1].append(L"[Const]");
				}
				else
				{
					row[1].append(L"[Vary]");
				}

				row[2] = ShaderNetUtils::ToString(port.DataType);

				row[3] = port.Usage == EPUSAGE_Unknown ? port.VaringTypeName : EffectParameter::ToString(port.Usage);

				m_lbPorts->getItems().AddRow(row);
			}
			
		}
	}
	void ShaderAtomDocument::SaveRes()
	{
		if (m_currentWorkingCopy)
		{
			ShaderAtomType* requested = ShaderAtomLibraryManager::getSingleton().FindAtomType(m_atomName);
			if (requested)
			{
				requested->CopyFrom(m_currentWorkingCopy);
			}
		}
	}

	void ShaderAtomDocument::Render()
	{

	}

	void ShaderAtomDocument::Update(const AppTime* time)
	{
		Document::Update(time);
	}

	void ShaderAtomDocument::Form_Resized(Control* ctrl)
	{
		m_tbCode->setWidth(getDocumentForm()->getWidth() - 50);
		m_tbCode->setHeight(getDocumentForm()->getHeight());
	}
}