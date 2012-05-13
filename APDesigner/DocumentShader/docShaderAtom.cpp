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

#include "docShaderAtom.h"

#include "ShaderAtomType.h"
#include "MainWindow.h"

#include "Collections/List2D.h"

#include "UILib/Form.h"
#include "UILib/Button.h"
#include "UILib/PictureBox.h"
#include "UILib/Label.h"
#include "UILib/FontManager.h"
#include "UILib/ComboBox.h"
#include "UILib/StyleSkin.h"
#include "UILib/CheckBox.h"
#include "UILib/List.h"
#include "Utility/StringUtils.h"

namespace APDesigner
{
	ShaderAtomDocument::ShaderAtomDocument(MainWindow* window, const String& name)
		: Document(window), m_atomName(name), m_currentWorkingCopy(0)
	{
		getDocumentForm()->setTitle(L"Shader Atom: " + name);
		getDocumentForm()->setMinimumSize(Point(970,635));

		{
			Label* lbl = new Label(Point(23, 33), L"Name", 80);
			lbl->SetSkin(window->getUISkin());
			m_labels.Add(lbl);

			m_tbName = new TextBox(Point(100, 33), 120);
			m_tbName->SetSkin(window->getUISkin());

			lbl = new Label(Point(250, 33), L"Type", 80);
			lbl->SetSkin(window->getUISkin());
			m_labels.Add(lbl);

			List<String> items;
			items.Add(GraphicsCommonUtils::ToString(SHDT_Vertex));
			items.Add(GraphicsCommonUtils::ToString(SHDT_Pixel));
			items.Add(GraphicsCommonUtils::ToString(SHDT_All));
			m_cbShaderType = new ComboBox(Point(330, 33), 120, items);
			m_cbShaderType->SetSkin(window->getUISkin());


			lbl = new Label(Point(23, 66), L"Profile", 80);
			lbl->SetSkin(window->getUISkin());
			m_labels.Add(lbl);

			items.Clear();
			items.Add(L"Shader Model 1");
			items.Add(L"Shader Model 2");
			items.Add(L"Shader Model 3");

			m_cbProfile = new ComboBox(Point(100, 66), 120, items);
			m_cbProfile->SetSkin(window->getUISkin());

			m_tbCode = new TextBox(Point(33, 100),500,500,L"");
			m_tbCode->setScrollbarType(TextBox::SBT_Both);
			m_tbCode->SetSkin(window->getUISkin());
		}
		{
			List2D<String> emptyList(3, 2);

			m_lbPorts = new ListView(Point(550, 25), Point(400, 550), emptyList);
			m_lbPorts->SetSkin(window->getUISkin());

			m_lbPorts->getColumnHeader().Add(ListView::Header(L"Name",150));
			m_lbPorts->getColumnHeader().Add(ListView::Header(L"Type",75));
			m_lbPorts->getColumnHeader().Add(ListView::Header(L"Format",75));
			m_lbPorts->getColumnHeader().Add(ListView::Header(L"Usage",100));
			m_lbPorts->setFullRowSelect(true);

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
			
			m_tbCode->setText(m_currentWorkingCopy->getCodeBody());
			m_tbName->setText(m_currentWorkingCopy->getName());

			if (m_currentWorkingCopy->getMajorSMVersion()>0 && 
				m_currentWorkingCopy->getMajorSMVersion()-1<m_cbProfile->getItems().getCount())
			{
				m_cbProfile->setSelectedIndex(m_currentWorkingCopy->getMajorSMVersion()-1);
			}

			switch (m_currentWorkingCopy->getShaderType())
			{
			case SHDT_All:
				m_cbShaderType->setSelectedIndex(2);
				break;
			case SHDT_Pixel:
				m_cbShaderType->setSelectedIndex(1);
				break;
			case SHDT_Vertex:
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

	void ShaderAtomDocument::Update(const GameTime* const time)
	{
		Document::Update(time);
	}

	void ShaderAtomDocument::Form_Resized(Control* ctrl)
	{
		m_tbCode->setWidth(getDocumentForm()->Size.X-50);
		m_tbCode->Size.Y = getDocumentForm()->Size.Y;
	}
}