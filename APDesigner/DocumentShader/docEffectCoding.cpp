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

#include "docEffectCoding.h"

#include "ShaderAtomType.h"
#include "MainWindow.h"

#include "Collections/List2D.h"
#include "Graphics/GraphicsCommon.h"

#include "DocumentModel/MaterialDocument.h"

#include "UILib/Form.h"
#include "UILib/Button.h"
#include "UILib/PictureBox.h"
#include "UILib/Label.h"
#include "UILib/FontManager.h"
#include "UILib/ComboBox.h"
#include "UILib/CheckBox.h"
#include "UILib/StyleSkin.h"
#include "UILib/CheckBox.h"
#include "UILib/List.h"
#include "Utility/StringUtils.h"
#include "Vfs/PathUtils.h"

namespace APDesigner
{
	EffectDocument::EffectDocument(MainWindow* window, const String& file)
		: Document(window), m_filePath(file)
	{
		String name = PathUtils::GetFileNameNoExt(file);
		getDocumentForm()->setTitle(L"Effect: " + name);
		getDocumentForm()->setMinimumSize(Point(1100,665));

		{
			Label * lbl = new Label(Point(10, 22), L"Vertex Code" , 100);
			lbl->SetSkin(window->getUISkin());
			m_labels.Add(lbl);

			lbl = new Label(Point(525, 22), L"Pixel Shader" , 100);
			lbl->SetSkin(window->getUISkin());
			m_labels.Add(lbl);




			m_tbVertexCode = new TextBox(Point(10, 45),240,600,L"");
			m_tbVertexCode->setScrollbarType(TextBox::SBT_Both);
			m_tbVertexCode->SetSkin(window->getUISkin());

			m_tbPixelCode = new TextBox(Point(525, 45),240,600,L"");
			m_tbPixelCode->setScrollbarType(TextBox::SBT_Both);
			m_tbPixelCode->SetSkin(window->getUISkin());


		}
		{
			List2D<String> emptyList(2, 2);

			m_vsParams = new ListView(Point(260, 45), Point(240, 200), emptyList);
			m_vsParams->SetSkin(window->getUISkin());

			m_vsParams->getColumnHeader().Add(ListView::Header(L"Name",120));
			m_vsParams->getColumnHeader().Add(ListView::Header(L"Usage",120));
			m_vsParams->setFullRowSelect(true);



			m_psParams = new ListView(Point(775, 45), Point(240, 200), emptyList);
			m_psParams->SetSkin(window->getUISkin());

			m_psParams->getColumnHeader().Add(ListView::Header(L"Name",120));
			m_psParams->getColumnHeader().Add(ListView::Header(L"Usage",120));
			m_psParams->setFullRowSelect(true);



			Label* lbl = new Label(Point(260, 22), L"Parameters(VS)" , 200);
			lbl->SetSkin(window->getUISkin());
			m_labels.Add(lbl);

			lbl = new Label(Point(775, 22), L"Parameters(PS)" , 200);
			lbl->SetSkin(window->getUISkin());
			m_labels.Add(lbl);


			int sy = 250;
			int lineHeight = 25;
			m_vsAddParam = new Button(Point(260, sy), 100, L"Add");
			m_vsAddParam->SetSkin(window->getUISkin());

			m_vsRemoveParam = new Button(Point(260+140, sy), 100, L"Remove");
			m_vsRemoveParam->SetSkin(window->getUISkin());


			sy+= lineHeight+15;
			lbl = new Label(Point(260, sy), L"Usage" , 100);
			lbl->SetSkin(window->getUISkin());
			m_labels.Add(lbl);

			List<String> items;
			for (FastMap<String, EffectParamUsage>::Enumerator e = EffectParameter::getParameterUsageEnumeration();e.MoveNext();)
				items.Add(*e.getCurrentKey());
			m_cbUsage = new ComboBox(Point(260+100, sy), 130, items);
			m_cbUsage->SetSkin(window->getUISkin());

			
			sy+= lineHeight;
			m_cbIsCustom = new CheckBox(Point(260, sy), L"Use Custom Usage", false);
			m_cbIsCustom->SetSkin(window->getUISkin());
			
			sy+= lineHeight;
			lbl = new Label(Point(260, sy), L"Custom" , 100);
			lbl->SetSkin(window->getUISkin());
			m_labels.Add(lbl);
			m_tbCustomUsage = new TextBox(Point(260+100, sy), 140);
			m_tbCustomUsage->SetSkin(window->getUISkin());

			sy+= lineHeight;
			m_cbHasSamplerState = new CheckBox(Point(260, sy), L"Has Sampler", false);
			m_cbHasSamplerState->SetSkin(window->getUISkin());


			sy += lineHeight;
			lbl = new Label(Point(260, sy), L"AddressU" , 100);
			lbl->SetSkin(window->getUISkin());
			m_labels.Add(lbl);
			
			items.Clear();
			items.Add(GraphicsCommonUtils::ToString(TA_Wrap));
			items.Add(GraphicsCommonUtils::ToString(TA_Clamp));
			items.Add(GraphicsCommonUtils::ToString(TA_Border));
			items.Add(GraphicsCommonUtils::ToString(TA_Mirror));
			items.Add(GraphicsCommonUtils::ToString(TA_MirrorOnce));
			m_cbAddressU = new ComboBox(Point(260+100, sy), 140, items);
			m_cbAddressU->SetSkin(window->getUISkin());

			sy+= lineHeight;
			lbl = new Label(Point(260, sy), L"AddressV" , 100);
			lbl->SetSkin(window->getUISkin());
			m_labels.Add(lbl);

			m_cbAddressV = new ComboBox(Point(260+100, sy), 140, items);
			m_cbAddressV->SetSkin(window->getUISkin());

			sy+= lineHeight;
			lbl = new Label(Point(260, sy), L"AddressW" , 100);
			lbl->SetSkin(window->getUISkin());
			m_labels.Add(lbl);

			m_cbAddressW = new ComboBox(Point(260+100, sy), 140, items);
			m_cbAddressW->SetSkin(window->getUISkin());

			sy+= lineHeight;
			lbl = new Label(Point(260, sy), L"BorderColor" , 100);
			lbl->SetSkin(window->getUISkin());
			m_labels.Add(lbl);

			m_cfBorderColor = new ColorField(Point(260, sy), CV_Black);
			m_cfBorderColor->SetSkin(window->getUISkin());

			sy+= lineHeight;
			lbl = new Label(Point(260, sy), L"MagFilter" , 100);
			lbl->SetSkin(window->getUISkin());
			m_labels.Add(lbl);

			items.Clear();
			items.Add(GraphicsCommonUtils::ToString(TFLT_Point));
			items.Add(GraphicsCommonUtils::ToString(TFLT_Linear));
			items.Add(GraphicsCommonUtils::ToString(TFLT_Anisotropic));
			items.Add(GraphicsCommonUtils::ToString(TFLT_PyramidalQuad));
			items.Add(GraphicsCommonUtils::ToString(TFLT_GaussianQuad));
			m_cbMagFilter = new ComboBox(Point(260+100, sy), 140, items);
			m_cbMagFilter->SetSkin(window->getUISkin());

			sy+= lineHeight;
			lbl = new Label(Point(260, sy), L"MinFilter" , 100);
			lbl->SetSkin(window->getUISkin());
			m_labels.Add(lbl);
			m_cbMinFilter = new ComboBox(Point(260+100, sy), 140, items);
			m_cbMinFilter->SetSkin(window->getUISkin());

			sy+= lineHeight;
			lbl = new Label(Point(260, sy), L"MipFilter" , 100);
			lbl->SetSkin(window->getUISkin());
			m_labels.Add(lbl);

			items.Clear();
			items.Add(GraphicsCommonUtils::ToString(TFLT_None));
			items.Add(GraphicsCommonUtils::ToString(TFLT_Point));
			items.Add(GraphicsCommonUtils::ToString(TFLT_Linear));
			m_cbMipFilter = new ComboBox(Point(260+100, sy), 140, items);
			m_cbMipFilter->SetSkin(window->getUISkin());

			sy+= lineHeight;
			lbl = new Label(Point(260, sy), L"MaxAniso." , 100);
			lbl->SetSkin(window->getUISkin());
			m_labels.Add(lbl);
			m_tbMaxAnisotropy = new TextBox(Point(260+100, sy), 140);
			m_tbMaxAnisotropy->SetSkin(window->getUISkin());

			sy+= lineHeight;
			lbl = new Label(Point(260, sy), L"MaxMipLevel" , 100);
			lbl->SetSkin(window->getUISkin());
			m_labels.Add(lbl);
			m_tbMaxMipLevel = new TextBox(Point(260+100, sy), 140);
			m_tbMaxMipLevel->SetSkin(window->getUISkin());

			sy+= lineHeight;
			lbl = new Label(Point(260, sy), L"MipLODBias" , 100);
			lbl->SetSkin(window->getUISkin());
			m_labels.Add(lbl);
			m_tbMipMapLODBias = new TextBox(Point(260+100, sy), 140);
			m_tbMipMapLODBias->SetSkin(window->getUISkin());

		}
		{

		}
	}

	EffectDocument::~EffectDocument()
	{
		

		for (int i=0;i<m_labels.getCount();i++)
		{
			delete m_labels[i];
		}
	}
	void EffectDocument::Initialize(RenderDevice* device)
	{
		getDocumentForm()->getControls().Add(m_tbVertexCode);
		getDocumentForm()->getControls().Add(m_tbPixelCode);
		getDocumentForm()->getControls().Add(m_vsParams);
		getDocumentForm()->getControls().Add(m_psParams);
		getDocumentForm()->getControls().Add(m_cbUsage);
		getDocumentForm()->getControls().Add(m_cbIsCustom);
		getDocumentForm()->getControls().Add(m_tbCustomUsage);
		getDocumentForm()->getControls().Add(m_cbHasSamplerState);

		getDocumentForm()->getControls().Add(m_cbAddressU);
		getDocumentForm()->getControls().Add(m_cbAddressV);
		getDocumentForm()->getControls().Add(m_cbAddressW);
		getDocumentForm()->getControls().Add(m_cfBorderColor);
		getDocumentForm()->getControls().Add(m_cbMagFilter);
		getDocumentForm()->getControls().Add(m_cbMinFilter);
		getDocumentForm()->getControls().Add(m_cbMipFilter);

		getDocumentForm()->getControls().Add(m_tbMaxAnisotropy);
		getDocumentForm()->getControls().Add(m_tbMaxMipLevel);
		getDocumentForm()->getControls().Add(m_tbMipMapLODBias);


		getDocumentForm()->getControls().Add(m_vsAddParam);
		getDocumentForm()->getControls().Add(m_vsRemoveParam);


		for (int i=0;i<m_labels.getCount();i++)
		{
			getDocumentForm()->getControls().Add(m_labels[i]);
		}

		Document::Initialize(device);

	}

	void EffectDocument::LoadRes()
	{
		//ShaderAtomType* requested = ShaderAtomLibraryManager::getSingleton().FindAtomType(m_atomName);
		//if (requested)
		//{
		//	if (m_currentWorkingCopy)
		//	{
		//		delete m_currentWorkingCopy;
		//	}

		//	m_currentWorkingCopy = new ShaderAtomType(*requested);

		//	m_tbCode->setText(m_currentWorkingCopy->getCodeBody());
		//	m_tbName->setText(m_currentWorkingCopy->getName());

		//	if (m_currentWorkingCopy->getMajorSMVersion()>0 && 
		//		m_currentWorkingCopy->getMajorSMVersion()-1<m_cbProfile->getItems().getCount())
		//	{
		//		m_cbProfile->setSelectedIndex(m_currentWorkingCopy->getMajorSMVersion()-1);
		//	}

		//	switch (m_currentWorkingCopy->getShaderType())
		//	{
		//	case SHDT_All:
		//		m_cbShaderType->setSelectedIndex(2);
		//		break;
		//	case SHDT_Pixel:
		//		m_cbShaderType->setSelectedIndex(1);
		//		break;
		//	case SHDT_Vertex:
		//		m_cbShaderType->setSelectedIndex(0);
		//		break;
		//	}

		//	for (int i=0;i<m_currentWorkingCopy->Ports().getCount();i++)
		//	{
		//		ShaderAtomPort& port = m_currentWorkingCopy->Ports()[i];
		//		String row[4];
		//		row[0] = port.Name;

		//		row[1] = port.IsInputOrOutput ? L"In" : L"Out";

		//		if (port.Usage == EPUSAGE_Unknown)
		//		{
		//			row[1].append(L"[Const]");
		//		}
		//		else
		//		{
		//			row[1].append(L"[Vary]");
		//		}

		//		row[2] = ShaderNetUtils::ToString(port.DataType);

		//		row[3] = port.Usage == EPUSAGE_Unknown ? port.VaringTypeName : EffectParameter::ToString(port.Usage);

		//		m_lbPorts->getItems().AddRow(row);
		//	}

		//}
	}
	void EffectDocument::SaveRes()
	{
		//if (m_currentWorkingCopy)
		//{
		//	ShaderAtomType* requested = ShaderAtomLibraryManager::getSingleton().FindAtomType(m_atomName);
		//	if (requested)
		//	{
		//		requested->CopyFrom(m_currentWorkingCopy);
		//	}
		//}
	}

	void EffectDocument::Render()
	{

	}

	void EffectDocument::Update(const GameTime* const time)
	{
		Document::Update(time);
	}

	void EffectDocument::Form_Resized(Control* ctrl)
	{
		//m_tbCode->setWidth(getDocumentForm()->Size.X-50);
		//m_tbCode->Size.Y = getDocumentForm()->Size.Y;
	}
}