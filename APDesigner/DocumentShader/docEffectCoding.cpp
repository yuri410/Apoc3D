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
#include "CommonDialog/Win32InputBox.h"

using namespace Apoc3D::Graphics;
using namespace Apoc3D::Graphics::RenderSystem;

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
			m_vsParams->eventSelected().bind(this, &EffectDocument::VSParams_Selected);

			m_psParams = new ListView(Point(775, 45), Point(240, 200), emptyList);
			m_psParams->SetSkin(window->getUISkin());

			m_psParams->getColumnHeader().Add(ListView::Header(L"Name",120));
			m_psParams->getColumnHeader().Add(ListView::Header(L"Usage",120));
			m_psParams->setFullRowSelect(true);
			m_psParams->eventSelected().bind(this, &EffectDocument::PSParams_Selected);

			Label* lbl = new Label(Point(260, 22), L"Parameters(VS)" , 200);
			lbl->SetSkin(window->getUISkin());
			m_labels.Add(lbl);

			lbl = new Label(Point(775, 22), L"Parameters(PS)" , 200);
			lbl->SetSkin(window->getUISkin());
			m_labels.Add(lbl);

		}
		// pixel shader params
		{
			CreatePSParamPanel(window);
			
		}
		// vertex shader params
		{
			CreateVSParamPanel(window);
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

		getDocumentForm()->getControls().Add(m_cbVsUsage);
		getDocumentForm()->getControls().Add(m_cbVsIsCustom);
		getDocumentForm()->getControls().Add(m_tbVsCustomUsage);
		getDocumentForm()->getControls().Add(m_cbVsHasSamplerState);

		getDocumentForm()->getControls().Add(m_cbVsAddressU);
		getDocumentForm()->getControls().Add(m_cbVsAddressV);
		getDocumentForm()->getControls().Add(m_cbVsAddressW);
		getDocumentForm()->getControls().Add(m_cfVsBorderColor);
		getDocumentForm()->getControls().Add(m_cbVsMagFilter);
		getDocumentForm()->getControls().Add(m_cbVsMinFilter);
		getDocumentForm()->getControls().Add(m_cbVsMipFilter);

		getDocumentForm()->getControls().Add(m_tbVsMaxAnisotropy);
		getDocumentForm()->getControls().Add(m_tbVsMaxMipLevel);
		getDocumentForm()->getControls().Add(m_tbVsMipMapLODBias);


		getDocumentForm()->getControls().Add(m_vsAddParam);
		getDocumentForm()->getControls().Add(m_vsRemoveParam);

		getDocumentForm()->getControls().Add(m_psAddParam);
		getDocumentForm()->getControls().Add(m_psRemoveParam);


		getDocumentForm()->getControls().Add(m_cbPsUsage);
		getDocumentForm()->getControls().Add(m_cbPsIsCustom);
		getDocumentForm()->getControls().Add(m_tbPsCustomUsage);
		getDocumentForm()->getControls().Add(m_cbPsHasSamplerState);

		getDocumentForm()->getControls().Add(m_cbPsAddressU);
		getDocumentForm()->getControls().Add(m_cbPsAddressV);
		getDocumentForm()->getControls().Add(m_cbPsAddressW);
		getDocumentForm()->getControls().Add(m_cfPsBorderColor);
		getDocumentForm()->getControls().Add(m_cbPsMagFilter);
		getDocumentForm()->getControls().Add(m_cbPsMinFilter);
		getDocumentForm()->getControls().Add(m_cbPsMipFilter);

		getDocumentForm()->getControls().Add(m_tbPsMaxAnisotropy);
		getDocumentForm()->getControls().Add(m_tbPsMaxMipLevel);
		getDocumentForm()->getControls().Add(m_tbPsMipMapLODBias);





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

	void EffectDocument::CreateVSParamPanel(MainWindow* window)
	{
		int sx = 260;
		int sy = 250;
		int lineHeight = 25;
		m_vsAddParam = new Button(Point(sx, sy), 100, L"Add");
		m_vsAddParam->SetSkin(window->getUISkin());
		m_vsAddParam->eventPress().bind(this, &EffectDocument::VSAddParam_Clicked);
		
		m_vsRemoveParam = new Button(Point(sx+140, sy), 100, L"Remove");
		m_vsRemoveParam->SetSkin(window->getUISkin());
		m_vsRemoveParam->eventPress().bind(this, &EffectDocument::VSRemoveParam_Clicked);

		sy+= lineHeight+15;
		Label* lbl = new Label(Point(sx, sy), L"Usage" , 100);
		lbl->SetSkin(window->getUISkin());
		m_labels.Add(lbl);

		List<String> items;
		for (FastMap<String, EffectParamUsage>::Enumerator e = EffectParameter::getParameterUsageEnumeration();e.MoveNext();)
			items.Add(*e.getCurrentKey());
		m_cbVsUsage = new ComboBox(Point(sx+100, sy), 130, items);
		m_cbVsUsage->SetSkin(window->getUISkin());


		sy+= lineHeight;
		m_cbVsIsCustom = new CheckBox(Point(sx, sy), L"Use Custom Usage", false);
		m_cbVsIsCustom->SetSkin(window->getUISkin());
		m_cbVsIsCustom->eventToggled().bind(this, &EffectDocument::CBVSIsCustom_Changed);

		sy+= lineHeight;
		lbl = new Label(Point(sx, sy), L"Custom" , 100);
		lbl->SetSkin(window->getUISkin());
		m_labels.Add(lbl);
		m_tbVsCustomUsage = new TextBox(Point(sx+100, sy), 140);
		m_tbVsCustomUsage->SetSkin(window->getUISkin());

		sy+= lineHeight;
		m_cbVsHasSamplerState = new CheckBox(Point(sx, sy), L"Has Sampler", false);
		m_cbVsHasSamplerState->SetSkin(window->getUISkin());
		m_cbVsHasSamplerState->eventToggled().bind(this, &EffectDocument::CBVSHasSampler_Changed);

		sy += lineHeight;
		lbl = new Label(Point(sx, sy), L"AddressU" , 100);
		lbl->SetSkin(window->getUISkin());
		m_labels.Add(lbl);

		items.Clear();
		items.Add(GraphicsCommonUtils::ToString(TA_Wrap));
		items.Add(GraphicsCommonUtils::ToString(TA_Clamp));
		items.Add(GraphicsCommonUtils::ToString(TA_Border));
		items.Add(GraphicsCommonUtils::ToString(TA_Mirror));
		items.Add(GraphicsCommonUtils::ToString(TA_MirrorOnce));
		m_cbVsAddressU = new ComboBox(Point(sx+100, sy), 140, items);
		m_cbVsAddressU->SetSkin(window->getUISkin());

		sy+= lineHeight;
		lbl = new Label(Point(sx, sy), L"AddressV" , 100);
		lbl->SetSkin(window->getUISkin());
		m_labels.Add(lbl);

		m_cbVsAddressV = new ComboBox(Point(sx+100, sy), 140, items);
		m_cbVsAddressV->SetSkin(window->getUISkin());

		sy+= lineHeight;
		lbl = new Label(Point(sx, sy), L"AddressW" , 100);
		lbl->SetSkin(window->getUISkin());
		m_labels.Add(lbl);

		m_cbVsAddressW = new ComboBox(Point(sx+100, sy), 140, items);
		m_cbVsAddressW->SetSkin(window->getUISkin());

		sy+= lineHeight;
		lbl = new Label(Point(sx, sy), L"BorderColor" , 100);
		lbl->SetSkin(window->getUISkin());
		m_labels.Add(lbl);

		m_cfVsBorderColor = new ColorField(Point(sx, sy), CV_Black);
		m_cfVsBorderColor->SetSkin(window->getUISkin());

		sy+= lineHeight;
		lbl = new Label(Point(sx, sy), L"MagFilter" , 100);
		lbl->SetSkin(window->getUISkin());
		m_labels.Add(lbl);

		items.Clear();
		items.Add(GraphicsCommonUtils::ToString(TFLT_Point));
		items.Add(GraphicsCommonUtils::ToString(TFLT_Linear));
		items.Add(GraphicsCommonUtils::ToString(TFLT_Anisotropic));
		items.Add(GraphicsCommonUtils::ToString(TFLT_PyramidalQuad));
		items.Add(GraphicsCommonUtils::ToString(TFLT_GaussianQuad));
		m_cbVsMagFilter = new ComboBox(Point(sx+100, sy), 140, items);
		m_cbVsMagFilter->SetSkin(window->getUISkin());

		sy+= lineHeight;
		lbl = new Label(Point(sx, sy), L"MinFilter" , 100);
		lbl->SetSkin(window->getUISkin());
		m_labels.Add(lbl);
		m_cbVsMinFilter = new ComboBox(Point(sx+100, sy), 140, items);
		m_cbVsMinFilter->SetSkin(window->getUISkin());

		sy+= lineHeight;
		lbl = new Label(Point(sx, sy), L"MipFilter" , 100);
		lbl->SetSkin(window->getUISkin());
		m_labels.Add(lbl);

		items.Clear();
		items.Add(GraphicsCommonUtils::ToString(TFLT_None));
		items.Add(GraphicsCommonUtils::ToString(TFLT_Point));
		items.Add(GraphicsCommonUtils::ToString(TFLT_Linear));
		m_cbVsMipFilter = new ComboBox(Point(sx+100, sy), 140, items);
		m_cbVsMipFilter->SetSkin(window->getUISkin());

		sy+= lineHeight;
		lbl = new Label(Point(sx, sy), L"MaxAniso." , 100);
		lbl->SetSkin(window->getUISkin());
		m_labels.Add(lbl);
		m_tbVsMaxAnisotropy = new TextBox(Point(sx+100, sy), 140);
		m_tbVsMaxAnisotropy->SetSkin(window->getUISkin());

		sy+= lineHeight;
		lbl = new Label(Point(sx, sy), L"MaxMipLevel" , 100);
		lbl->SetSkin(window->getUISkin());
		m_labels.Add(lbl);
		m_tbVsMaxMipLevel = new TextBox(Point(sx+100, sy), 140);
		m_tbVsMaxMipLevel->SetSkin(window->getUISkin());

		sy+= lineHeight;
		lbl = new Label(Point(sx, sy), L"MipLODBias" , 100);
		lbl->SetSkin(window->getUISkin());
		m_labels.Add(lbl);
		m_tbVsMipMapLODBias = new TextBox(Point(sx+100, sy), 140);
		m_tbVsMipMapLODBias->SetSkin(window->getUISkin());
	}
	void EffectDocument::CreatePSParamPanel(MainWindow* window)
	{
		int sx = 260 + 525;
		int sy = 250;
		int lineHeight = 25;
		m_psAddParam = new Button(Point(sx, sy), 100, L"Add");
		m_psAddParam->SetSkin(window->getUISkin());
		m_psAddParam->eventPress().bind(this, &EffectDocument::PSAddParam_Clicked);

		m_psRemoveParam = new Button(Point(sx+140, sy), 100, L"Remove");
		m_psRemoveParam->SetSkin(window->getUISkin());
		m_psRemoveParam->eventPress().bind(this, &EffectDocument::PSRemoveParam_Clicked);

		sy+= lineHeight+15;
		Label* lbl = new Label(Point(sx, sy), L"Usage" , 100);
		lbl->SetSkin(window->getUISkin());
		m_labels.Add(lbl);

		List<String> items;
		for (FastMap<String, EffectParamUsage>::Enumerator e = EffectParameter::getParameterUsageEnumeration();e.MoveNext();)
			items.Add(*e.getCurrentKey());
		m_cbPsUsage = new ComboBox(Point(sx+100, sy), 130, items);
		m_cbPsUsage->SetSkin(window->getUISkin());


		sy+= lineHeight;
		m_cbPsIsCustom = new CheckBox(Point(sx, sy), L"Use Custom Usage", false);
		m_cbPsIsCustom->SetSkin(window->getUISkin());
		m_cbPsIsCustom->eventToggled().bind(this, &EffectDocument::CBPSIsCustom_Changed);

		sy+= lineHeight;
		lbl = new Label(Point(sx, sy), L"Custom" , 100);
		lbl->SetSkin(window->getUISkin());
		m_labels.Add(lbl);
		m_tbPsCustomUsage = new TextBox(Point(sx+100, sy), 140);
		m_tbPsCustomUsage->SetSkin(window->getUISkin());

		sy+= lineHeight;
		m_cbPsHasSamplerState = new CheckBox(Point(sx, sy), L"Has Sampler", false);
		m_cbPsHasSamplerState->SetSkin(window->getUISkin());
		m_cbPsHasSamplerState->eventToggled().bind(this, &EffectDocument::CBPSHasSampler_Changed);

		sy += lineHeight;
		lbl = new Label(Point(sx, sy), L"AddressU" , 100);
		lbl->SetSkin(window->getUISkin());
		m_labels.Add(lbl);

		items.Clear();
		items.Add(GraphicsCommonUtils::ToString(TA_Wrap));
		items.Add(GraphicsCommonUtils::ToString(TA_Clamp));
		items.Add(GraphicsCommonUtils::ToString(TA_Border));
		items.Add(GraphicsCommonUtils::ToString(TA_Mirror));
		items.Add(GraphicsCommonUtils::ToString(TA_MirrorOnce));
		m_cbPsAddressU = new ComboBox(Point(sx+100, sy), 140, items);
		m_cbPsAddressU->SetSkin(window->getUISkin());

		sy+= lineHeight;
		lbl = new Label(Point(sx, sy), L"AddressV" , 100);
		lbl->SetSkin(window->getUISkin());
		m_labels.Add(lbl);

		m_cbPsAddressV = new ComboBox(Point(sx+100, sy), 140, items);
		m_cbPsAddressV->SetSkin(window->getUISkin());

		sy+= lineHeight;
		lbl = new Label(Point(sx, sy), L"AddressW" , 100);
		lbl->SetSkin(window->getUISkin());
		m_labels.Add(lbl);

		m_cbPsAddressW = new ComboBox(Point(sx+100, sy), 140, items);
		m_cbPsAddressW->SetSkin(window->getUISkin());

		sy+= lineHeight;
		lbl = new Label(Point(sx, sy), L"BorderColor" , 100);
		lbl->SetSkin(window->getUISkin());
		m_labels.Add(lbl);

		m_cfPsBorderColor = new ColorField(Point(sx, sy), CV_Black);
		m_cfPsBorderColor->SetSkin(window->getUISkin());

		sy+= lineHeight;
		lbl = new Label(Point(sx, sy), L"MagFilter" , 100);
		lbl->SetSkin(window->getUISkin());
		m_labels.Add(lbl);

		items.Clear();
		items.Add(GraphicsCommonUtils::ToString(TFLT_Point));
		items.Add(GraphicsCommonUtils::ToString(TFLT_Linear));
		items.Add(GraphicsCommonUtils::ToString(TFLT_Anisotropic));
		items.Add(GraphicsCommonUtils::ToString(TFLT_PyramidalQuad));
		items.Add(GraphicsCommonUtils::ToString(TFLT_GaussianQuad));
		m_cbPsMagFilter = new ComboBox(Point(sx+100, sy), 140, items);
		m_cbPsMagFilter->SetSkin(window->getUISkin());

		sy+= lineHeight;
		lbl = new Label(Point(sx, sy), L"MinFilter" , 100);
		lbl->SetSkin(window->getUISkin());
		m_labels.Add(lbl);
		m_cbPsMinFilter = new ComboBox(Point(sx+100, sy), 140, items);
		m_cbPsMinFilter->SetSkin(window->getUISkin());

		sy+= lineHeight;
		lbl = new Label(Point(sx, sy), L"MipFilter" , 100);
		lbl->SetSkin(window->getUISkin());
		m_labels.Add(lbl);

		items.Clear();
		items.Add(GraphicsCommonUtils::ToString(TFLT_None));
		items.Add(GraphicsCommonUtils::ToString(TFLT_Point));
		items.Add(GraphicsCommonUtils::ToString(TFLT_Linear));
		m_cbPsMipFilter = new ComboBox(Point(sx+100, sy), 140, items);
		m_cbPsMipFilter->SetSkin(window->getUISkin());

		sy+= lineHeight;
		lbl = new Label(Point(sx, sy), L"MaxAniso." , 100);
		lbl->SetSkin(window->getUISkin());
		m_labels.Add(lbl);
		m_tbPsMaxAnisotropy = new TextBox(Point(sx+100, sy), 140);
		m_tbPsMaxAnisotropy->SetSkin(window->getUISkin());

		sy+= lineHeight;
		lbl = new Label(Point(sx, sy), L"MaxMipLevel" , 100);
		lbl->SetSkin(window->getUISkin());
		m_labels.Add(lbl);
		m_tbPsMaxMipLevel = new TextBox(Point(sx+100, sy), 140);
		m_tbPsMaxMipLevel->SetSkin(window->getUISkin());

		sy+= lineHeight;
		lbl = new Label(Point(sx, sy), L"MipLODBias" , 100);
		lbl->SetSkin(window->getUISkin());
		m_labels.Add(lbl);
		m_tbPsMipMapLODBias = new TextBox(Point(sx+100, sy), 140);
		m_tbPsMipMapLODBias->SetSkin(window->getUISkin());
	}

	void EffectDocument::Form_Resized(Control* ctrl)
	{
		//m_tbCode->setWidth(getDocumentForm()->Size.X-50);
		//m_tbCode->Size.Y = getDocumentForm()->Size.Y;
	}

	void EffectDocument::RefreshParameterList()
	{
		m_parameters.Clear();
		for (int i=0;i<m_parameters.getCount();i++)
		{
			String usage;

			if (m_parameters[i].IsCustomUsage)
			{
				usage = m_parameters[i].CustomUsage;
			}
			else
			{
				usage = EffectParameter::ToString(m_parameters[i].TypicalUsage);
			}

			String items[2] = {m_parameters[i].Name, usage};
			
			if (m_parameters[i].ProgramType == SHDT_Vertex)
			{
				m_vsParams->getItems().AddRow(items);
			}
			else
			{
				m_psParams->getItems().AddRow(items);
			}
		}
	}
	void EffectDocument::UploadPSParameter(EffectParameter& p)
	{

	}
	void EffectDocument::UploadVSParameter(EffectParameter& p)
	{

	}
	void EffectDocument::DownloadPSParameter(const EffectParameter& p)
	{

	}
	void EffectDocument::DownloadVSParameter(const EffectParameter& p)
	{
		int idx = -1;
		String strV = EffectParameter::ToString(p.TypicalUsage);
		for (int i=0;i<m_cbVsUsage->getItems().getCount();i++)
			if (m_cbVsUsage->getItems()[i]==strV)
			{
				idx = i;break;
			}
		m_cbVsUsage->setSelectedIndex(idx);
		m_cbVsIsCustom->setValue(p.IsCustomUsage);
		
		m_tbVsCustomUsage->Text = p.CustomUsage;

		
	}

	void EffectDocument::VSParams_Selected(int x, int y)
	{
		String name = m_vsParams->getItems().at(x,0);
		for (int i=0;i<m_parameters.getCount();i++)
		{
			if (m_parameters[i].ProgramType == SHDT_Vertex && 
				m_parameters[i].Name == name)
			{
				DownloadVSParameter(m_parameters[i]);
			}
		}
	}
	void EffectDocument::PSParams_Selected(int x, int y)
	{
		String name = m_psParams->getItems().at(x,0);
		for (int i=0;i<m_parameters.getCount();i++)
		{
			if (m_parameters[i].ProgramType == SHDT_Pixel && 
				m_parameters[i].Name == name)
			{
				DownloadVSParameter(m_parameters[i]);
			}
		}
	}

	void EffectDocument::VSAddParam_Clicked(Control* ctrl)
	{
		wchar_t buffer[260] = {0};
		if (CWin32InputBox::InputBox(L"Name", L"", buffer, 260) )
		{
			String name = buffer;
			if (name.size())
			{
				EffectParameter ep(name);
				ep.ProgramType = SHDT_Vertex;
				m_parameters.Add(ep);
				RefreshParameterList();
			}
		}
	}
	void EffectDocument::VSRemoveParam_Clicked(Control* ctrl)
	{
		if (m_vsParams->getSelectedRowIndex() !=-1)
		{
			String name = m_vsParams->getItems().at(m_vsParams->getSelectedRowIndex(),0);

			for (int i=0;i<m_parameters.getCount();i++)
			{
				if (m_parameters[i].ProgramType == SHDT_Vertex && 
					m_parameters[i].Name == name)
				{
					m_parameters.RemoveAt(i);
					break;
				}
			}
			RefreshParameterList();
		}
	}
	void EffectDocument::PSAddParam_Clicked(Control* ctrl)
	{
		wchar_t buffer[260] = {0};
		if (CWin32InputBox::InputBox(L"Name", L"", buffer, 260) )
		{
			String name = buffer;
			if (name.size())
			{
				EffectParameter ep(name);
				ep.ProgramType = SHDT_Pixel;
				m_parameters.Add(ep);
				RefreshParameterList();
			}
		}
	}
	void EffectDocument::PSRemoveParam_Clicked(Control* ctrl)
	{
		if (m_psParams->getSelectedRowIndex() !=-1)
		{
			String name = m_psParams->getItems().at(m_psParams->getSelectedRowIndex(),0);

			for (int i=0;i<m_parameters.getCount();i++)
			{
				if (m_parameters[i].ProgramType == SHDT_Pixel && 
					m_parameters[i].Name == name)
				{
					m_parameters.RemoveAt(i);
					break;
				}
			}
			RefreshParameterList();
		}
	}

	void EffectDocument::CBVSHasSampler_Changed(Control* ctrl)
	{
		bool v = m_cbVsHasSamplerState->getValue();
		m_cbVsAddressU->Visible = v;
		m_cbVsAddressV->Visible = v;
		m_cbVsAddressW->Visible = v;
		m_cbVsMagFilter->Visible = v;
		m_cbVsMinFilter->Visible = v;
		m_cbVsMipFilter->Visible = v;
		m_cfVsBorderColor->Visible = v;
		m_tbVsMipMapLODBias->Visible = v;
		m_tbVsMaxMipLevel->Visible = v;
		m_tbVsMaxAnisotropy->Visible = v;
	}
	void EffectDocument::CBVSIsCustom_Changed(Control* ctrl)
	{
		bool v = m_cbVsIsCustom->getValue();

		m_tbVsCustomUsage->Visible = v;
		m_cbVsUsage->Visible = !v;
	}

	void EffectDocument::CBPSHasSampler_Changed(Control* ctrl)
	{
		bool v = m_cbPsHasSamplerState->getValue();
		m_cbPsAddressU->Visible = v;
		m_cbPsAddressV->Visible = v;
		m_cbPsAddressW->Visible = v;
		m_cbPsMagFilter->Visible = v;
		m_cbPsMinFilter->Visible = v;
		m_cbPsMipFilter->Visible = v;
		m_cfPsBorderColor->Visible = v;
		m_tbPsMipMapLODBias->Visible = v;
		m_tbPsMaxMipLevel->Visible = v;
		m_tbPsMaxAnisotropy->Visible = v;
	}
	void EffectDocument::CBPSIsCustom_Changed(Control* ctrl)
	{
		bool v = m_cbPsIsCustom->getValue();

		m_tbPsCustomUsage->Visible = v;
		m_cbPsUsage->Visible = !v;
	}
}