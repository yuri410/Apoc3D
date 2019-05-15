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

#include "docEffectCoding.h"

#include "ShaderAtomType.h"
#include "APDesigner/MainWindow.h"
#include "APDesigner/DocumentModel/MaterialDocument.h"
#include "APDesigner/CommonDialog/Win32InputBox.h"

#include <fstream>
#include <vector>

namespace APDesigner
{
	EffectDocument::EffectDocument(MainWindow* window, EditorExtension* ext, const String& file, const String& vsPath, const String& psPath)
		: Document(window, ext), m_filePath(file), m_vsPath(vsPath), m_psPath(psPath)
	{
		String name = PathUtils::GetFileNameNoExt(file);
		getDocumentForm()->setTitle(L"Effect: " + name);
		getDocumentForm()->setMinimumSize(Point(1100,665));

		const StyleSkin* skin = window->getUISkin();

		{
			Label * lbl = new Label(skin, Point(10, 22), L"Vertex Code", 100);
			m_labels.Add(lbl);

			lbl = new Label(skin, Point(525, 22), L"Pixel Shader", 100);
			m_labels.Add(lbl);


			m_tbVertexCode = new TextBox(skin, Point(10, 45), 240, 600, L"");
			m_tbVertexCode->EnableAllScrollBars();

			m_tbPixelCode = new TextBox(skin, Point(525, 45), 240, 600, L"");
			m_tbPixelCode->EnableAllScrollBars();
		}

		{
			List2D<String> emptyList(2, 2);

			m_vsParams = new ListView(skin, Point(260, 45), Point(240, 200), emptyList);

			m_vsParams->getColumnHeader().Add(ListView::Header(L"Name",120));
			m_vsParams->getColumnHeader().Add(ListView::Header(L"Usage",120));
			m_vsParams->FullRowSelect = true;
			m_vsParams->eventSelected.Bind(this, &EffectDocument::VSParams_Selected);

			m_psParams = new ListView(skin, Point(775, 45), Point(240, 200), emptyList);

			m_psParams->getColumnHeader().Add(ListView::Header(L"Name",120));
			m_psParams->getColumnHeader().Add(ListView::Header(L"Usage",120));
			m_psParams->FullRowSelect = true;
			m_psParams->eventSelected.Bind(this, &EffectDocument::PSParams_Selected);

			Label* lbl = new Label(skin, Point(260, 22), L"Parameters(VS)" , 200);
			m_labels.Add(lbl);

			lbl = new Label(skin, Point(775, 22), L"Parameters(PS)" , 200);
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
		
		m_labels.DeleteAndClear();
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
		getDocumentForm()->getControls().Add(m_vsApplyParam);

		getDocumentForm()->getControls().Add(m_psAddParam);
		getDocumentForm()->getControls().Add(m_psRemoveParam);
		getDocumentForm()->getControls().Add(m_psApplyParam);

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

	void Parse(ConfigurationSection* ps, EffectParameter& ep)
	{
		ep.Usage = EffectParameter::ParseParamUsage(ps->getAttribute(L"Usage"));
		if (ep.Usage == EPUSAGE_CustomMaterialParam)
		{
			ep.CustomMaterialParamName = ps->getAttribute(L"CustomUsage");
		}
		else if (ep.Usage == EPUSAGE_InstanceBlob)
		{
			ep.InstanceBlobIndex = ps->GetAttributeInt(L"BlobIndex");
		}
	}
	void EffectDocument::LoadRes()
	{
		Configuration plist;
		XMLConfigurationFormat::Instance.Load(FileLocation(m_filePath), &plist);

		for (ConfigurationSection* ps : plist[L"VS"]->getSubSections())
		{
			EffectParameter ep(ps->getName());

			Parse(ps, ep);

			ep.ProgramType = ShaderType::Vertex;
			ep.SamplerState.Parse(ps);
			m_parameters.Add(ep);
		}

		for (ConfigurationSection* ps : plist[L"PS"]->getSubSections())
		{
			EffectParameter ep(ps->getName());

			Parse(ps, ep);

			ep.ProgramType = ShaderType::Pixel;
			ep.SamplerState.Parse(ps);
			m_parameters.Add(ep);
		}
		
		{
			FileLocation fl(m_vsPath);
			String code = Encoding::ReadAllText(fl, Encoding::TEC_Unknown);

			m_tbVertexCode->SetText(code);
		}
		{
			FileLocation fl(m_psPath);
			String code = Encoding::ReadAllText(fl, Encoding::TEC_Unknown);
			
			m_tbPixelCode->SetText(code);
		}
		
		RefreshParameterList();

	}
	void EffectDocument::SaveRes()
	{
		Configuration plist(L"Root");

		ConfigurationSection* vs = new ConfigurationSection(L"VS");
		ConfigurationSection* ps = new ConfigurationSection(L"PS");
		for (int i = 0; i < m_parameters.getCount(); i++)
		{
			EffectParameter& ep = m_parameters[i];

			ConfigurationSection* sect = new ConfigurationSection(ep.Name);
			sect->AddAttributeString(L"Usage", EffectParameter::ToString(ep.Usage));

			if (ep.Usage == EPUSAGE_CustomMaterialParam)
				sect->AddAttributeString(L"CustomUsage", ep.CustomMaterialParamName);
			else if (ep.Usage == EPUSAGE_CustomMaterialParam)
				sect->AddAttributeString(L"BlobIndex", StringUtils::IntToString(ep.InstanceBlobIndex));

			if (ep.RegisterIndex == 99)
				ep.SamplerState.Save(sect);

			if (ep.ProgramType == ShaderType::Vertex)
				vs->AddSection(sect);
			else
				ps->AddSection(sect);
		}
		plist.Add(vs);
		plist.Add(ps);

		//plist->Save(m_filePath);
		XMLConfigurationFormat::Instance.Save(&plist, FileOutStream(m_filePath));
	}

	void EffectDocument::Render()
	{

	}

	void EffectDocument::Update(const AppTime* time)
	{
		Document::Update(time);
	}

	void EffectDocument::CreateVSParamPanel(MainWindow* window)
	{
		const StyleSkin* skin = window->getUISkin();

		int sx = 260;
		int sy = 250;
		int lineHeight = 25;
		m_vsAddParam = new Button(skin, Point(sx, sy), 60, L"Add");
		m_vsAddParam->eventPress.Bind(this, &EffectDocument::VSAddParam_Clicked);
		
		m_vsRemoveParam = new Button(skin, Point(sx+70, sy), 90, L"Remove");
		m_vsRemoveParam->eventPress.Bind(this, &EffectDocument::VSRemoveParam_Clicked);

		m_vsApplyParam = new Button(skin, Point(sx+170, sy), 90, L"Apply");
		m_vsApplyParam->eventPress.Bind(this, &EffectDocument::VSApplyParam_Clicked);



		sy+= lineHeight+15;
		Label* lbl = new Label(skin, Point(sx, sy), L"Usage" , 100);
		m_labels.Add(lbl);

		List<String> items;
		EffectParameter::FillParameterUsageNames(items);
		m_cbVsUsage = new ComboBox(skin, Point(sx+100, sy), 140, items);


		sy+= lineHeight;
		m_cbVsIsCustom = new CheckBox(skin, Point(sx, sy), L"Use Custom Usage", false);
		m_cbVsIsCustom->eventToggled.Bind(this, &EffectDocument::CBVSIsCustom_Changed);

		sy+= lineHeight;
		lbl = new Label(skin, Point(sx, sy), L"Custom" , 100);
		m_labels.Add(lbl);
		m_tbVsCustomUsage = new TextBox(skin, Point(sx+100, sy), 140);

		sy+= lineHeight;
		m_cbVsHasSamplerState = new CheckBox(skin, Point(sx, sy), L"Has Sampler", false);
		m_cbVsHasSamplerState->eventToggled.Bind(this, &EffectDocument::CBVSHasSampler_Changed);

		sy += lineHeight;
		lbl = new Label(skin, Point(sx, sy), L"AddressU" , 100);
		m_labels.Add(lbl);

		items.Clear();
		items.Add(TextureAddressModeConverter.ToString(TextureAddressMode::Wrap));
		items.Add(TextureAddressModeConverter.ToString(TextureAddressMode::Clamp));
		items.Add(TextureAddressModeConverter.ToString(TextureAddressMode::Border));
		items.Add(TextureAddressModeConverter.ToString(TextureAddressMode::Mirror));
		items.Add(TextureAddressModeConverter.ToString(TextureAddressMode::MirrorOnce));
		m_cbVsAddressU = new ComboBox(skin, Point(sx+100, sy), 140, items);

		sy+= lineHeight;
		lbl = new Label(skin, Point(sx, sy), L"AddressV" , 100);
		m_labels.Add(lbl);

		m_cbVsAddressV = new ComboBox(skin, Point(sx+100, sy), 140, items);

		sy+= lineHeight;
		lbl = new Label(skin, Point(sx, sy), L"AddressW" , 100);
		m_labels.Add(lbl);

		m_cbVsAddressW = new ComboBox(skin, Point(sx+100, sy), 140, items);

		sy+= lineHeight;
		lbl = new Label(skin, Point(sx, sy), L"BorderColor" , 100);
		m_labels.Add(lbl);

		m_cfVsBorderColor = new ColorField(skin, Point(sx, sy), L"", CV_Black);

		sy+= lineHeight;
		lbl = new Label(skin, Point(sx, sy), L"MagFilter" , 100);
		m_labels.Add(lbl);

		items.Clear();
		items.Add(TextureFilterConverter.ToString(TextureFilter::Point));
		items.Add(TextureFilterConverter.ToString(TextureFilter::Linear));
		items.Add(TextureFilterConverter.ToString(TextureFilter::Anisotropic));
		m_cbVsMagFilter = new ComboBox(skin, Point(sx+100, sy), 140, items);

		sy+= lineHeight;
		lbl = new Label(skin, Point(sx, sy), L"MinFilter" , 100);
		m_labels.Add(lbl);
		m_cbVsMinFilter = new ComboBox(skin, Point(sx+100, sy), 140, items);

		sy+= lineHeight;
		lbl = new Label(skin, Point(sx, sy), L"MipFilter" , 100);
		m_labels.Add(lbl);

		items.Clear();
		items.Add(TextureFilterConverter.ToString(TextureFilter::None));
		items.Add(TextureFilterConverter.ToString(TextureFilter::Point));
		items.Add(TextureFilterConverter.ToString(TextureFilter::Linear));
		m_cbVsMipFilter = new ComboBox(skin, Point(sx+100, sy), 140, items);

		sy+= lineHeight;
		lbl = new Label(skin, Point(sx, sy), L"MaxAniso." , 100);
		m_labels.Add(lbl);
		m_tbVsMaxAnisotropy = new TextBox(skin, Point(sx+100, sy), 140);

		sy+= lineHeight;
		lbl = new Label(skin, Point(sx, sy), L"MaxMipLevel" , 100);
		m_labels.Add(lbl);
		m_tbVsMaxMipLevel = new TextBox(skin, Point(sx+100, sy), 140);

		sy+= lineHeight;
		lbl = new Label(skin, Point(sx, sy), L"MipLODBias" , 100);
		m_labels.Add(lbl);
		m_tbVsMipMapLODBias = new TextBox(skin, Point(sx+100, sy), 140);
	}
	void EffectDocument::CreatePSParamPanel(MainWindow* window)
	{
		const StyleSkin* skin = window->getUISkin();

		int sx = 260 + 525;
		int sy = 250;
		int lineHeight = 25;
		m_psAddParam = new Button(skin, Point(sx, sy), 60, L"Add");
		m_psAddParam->eventPress.Bind(this, &EffectDocument::PSAddParam_Clicked);

		m_psRemoveParam = new Button(skin, Point(sx+70, sy), 90, L"Remove");
		m_psRemoveParam->eventPress.Bind(this, &EffectDocument::PSRemoveParam_Clicked);

		m_psApplyParam = new Button(skin, Point(sx+170, sy), 90, L"Apply");
		m_psApplyParam->eventPress.Bind(this, &EffectDocument::PSApplyParam_Clicked);


		sy+= lineHeight+15;
		Label* lbl = new Label(skin, Point(sx, sy), L"Usage" , 100);
		m_labels.Add(lbl);

		List<String> items;
		EffectParameter::FillParameterUsageNames(items);
		m_cbPsUsage = new ComboBox(skin, Point(sx+100, sy), 140, items);


		sy+= lineHeight;
		m_cbPsIsCustom = new CheckBox(skin, Point(sx, sy), L"Use Custom Usage", false);
		m_cbPsIsCustom->eventToggled.Bind(this, &EffectDocument::CBPSIsCustom_Changed);

		sy+= lineHeight;
		lbl = new Label(skin, Point(sx, sy), L"Custom" , 100);
		m_labels.Add(lbl);
		m_tbPsCustomUsage = new TextBox(skin, Point(sx+100, sy), 140);

		sy+= lineHeight;
		m_cbPsHasSamplerState = new CheckBox(skin, Point(sx, sy), L"Has Sampler", false);
		m_cbPsHasSamplerState->eventToggled.Bind(this, &EffectDocument::CBPSHasSampler_Changed);

		sy += lineHeight;
		lbl = new Label(skin, Point(sx, sy), L"AddressU" , 100);
		m_labels.Add(lbl);

		items.Clear();
		items.Add(TextureAddressModeConverter.ToString(TextureAddressMode::Wrap));
		items.Add(TextureAddressModeConverter.ToString(TextureAddressMode::Clamp));
		items.Add(TextureAddressModeConverter.ToString(TextureAddressMode::Border));
		items.Add(TextureAddressModeConverter.ToString(TextureAddressMode::Mirror));
		items.Add(TextureAddressModeConverter.ToString(TextureAddressMode::MirrorOnce));
		m_cbPsAddressU = new ComboBox(skin, Point(sx+100, sy), 140, items);

		sy+= lineHeight;
		lbl = new Label(skin, Point(sx, sy), L"AddressV" , 100);
		m_labels.Add(lbl);

		m_cbPsAddressV = new ComboBox(skin, Point(sx+100, sy), 140, items);

		sy+= lineHeight;
		lbl = new Label(skin, Point(sx, sy), L"AddressW" , 100);
		m_labels.Add(lbl);

		m_cbPsAddressW = new ComboBox(skin, Point(sx+100, sy), 140, items);

		sy+= lineHeight;
		lbl = new Label(skin, Point(sx, sy), L"BorderColor" , 100);
		m_labels.Add(lbl);

		m_cfPsBorderColor = new ColorField(skin, Point(sx, sy), L"", CV_Black);

		sy+= lineHeight;
		lbl = new Label(skin, Point(sx, sy), L"MagFilter" , 100);
		m_labels.Add(lbl);

		items.Clear();
		items.Add(TextureFilterConverter.ToString(TextureFilter::Point));
		items.Add(TextureFilterConverter.ToString(TextureFilter::Linear));
		items.Add(TextureFilterConverter.ToString(TextureFilter::Anisotropic));
		m_cbPsMagFilter = new ComboBox(skin, Point(sx+100, sy), 140, items);

		sy+= lineHeight;
		lbl = new Label(skin, Point(sx, sy), L"MinFilter" , 100);
		m_labels.Add(lbl);
		m_cbPsMinFilter = new ComboBox(skin, Point(sx+100, sy), 140, items);

		sy+= lineHeight;
		lbl = new Label(skin, Point(sx, sy), L"MipFilter" , 100);
		m_labels.Add(lbl);

		items.Clear();
		items.Add(TextureFilterConverter.ToString(TextureFilter::None));
		items.Add(TextureFilterConverter.ToString(TextureFilter::Point));
		items.Add(TextureFilterConverter.ToString(TextureFilter::Linear));
		m_cbPsMipFilter = new ComboBox(skin, Point(sx+100, sy), 140, items);

		sy+= lineHeight;
		lbl = new Label(skin, Point(sx, sy), L"MaxAniso." , 100);
		m_labels.Add(lbl);
		m_tbPsMaxAnisotropy = new TextBox(skin, Point(sx+100, sy), 140);

		sy+= lineHeight;
		lbl = new Label(skin, Point(sx, sy), L"MaxMipLevel" , 100);
		m_labels.Add(lbl);
		m_tbPsMaxMipLevel = new TextBox(skin, Point(sx+100, sy), 140);

		sy+= lineHeight;
		lbl = new Label(skin, Point(sx, sy), L"MipLODBias" , 100);
		m_labels.Add(lbl);
		m_tbPsMipMapLODBias = new TextBox(skin, Point(sx+100, sy), 140);
	}

	void EffectDocument::Form_Resized(Control* ctrl)
	{
		//m_tbCode->setWidth(getDocumentForm()->Size.X-50);
		//m_tbCode->Size.Y = getDocumentForm()->Size.Y;
	}

	void EffectDocument::RefreshParameterList()
	{
		m_vsParams->getItems().Clear();
		m_psParams->getItems().Clear();

		for (int i=0;i<m_parameters.getCount();i++)
		{
			String usage;

			usage = EffectParameter::ToString(m_parameters[i].Usage);
			

			String items[2];
			items[0] = m_parameters[i].Name;
			items[1] = usage;
			
			if (m_parameters[i].ProgramType == ShaderType::Vertex)
			{
				m_vsParams->getItems().AddRow(items);
			}
			else
			{
				m_psParams->getItems().AddRow(items);
			}
		}
	}


	void EffectDocument::UploadParameter(EffectParameter& p, bool isVS)
	{
		ComboBox* cbTemp;
		CheckBox* chTemp;


		chTemp = isVS ? m_cbVsIsCustom : m_cbPsIsCustom;
		cbTemp = isVS ? m_cbVsUsage : m_cbPsUsage;
		bool isCustomUsage = chTemp->Checked && cbTemp->getSelectedIndex();
		//if (p.IsCustomUsage)
		//{
		//	p.Usage = EPUSAGE_Unknown;
		//	p.CustomUsage = isVS ? m_tbVsCustomUsage->Text : m_tbPsCustomUsage->Text;
		//}
		//else
		//{
		//	p.Usage =  EffectParameter::ParseParamUsage(cbTemp->getItems()[cbTemp->getSelectedIndex()]);// EffectParameter::ParseParamUsage(cbTemp->Text);
		//	p.CustomUsage = L"";
		//}
		p.Usage = isCustomUsage ?  EPUSAGE_CustomMaterialParam :  EffectParameter::ParseParamUsage(cbTemp->getItems()[cbTemp->getSelectedIndex()]);
		p.CustomMaterialParamName = isVS ? m_tbVsCustomUsage->getText() : m_tbPsCustomUsage->getText();

		chTemp = isVS ? m_cbVsHasSamplerState : m_cbPsHasSamplerState;

		if (chTemp->Checked)
		{
			p.RegisterIndex = 99;
			cbTemp = (isVS ? m_cbVsAddressU : m_cbPsAddressU);
			if (cbTemp->getSelectedIndex() !=-1)
				p.SamplerState.AddressU = TextureAddressModeConverter.Parse(cbTemp->getItems()[cbTemp->getSelectedIndex()]);

			cbTemp = (isVS ? m_cbVsAddressV : m_cbPsAddressV);
			if (cbTemp->getSelectedIndex() !=-1)
				p.SamplerState.AddressV = TextureAddressModeConverter.Parse(cbTemp->getItems()[cbTemp->getSelectedIndex()]);

			cbTemp = (isVS ? m_cbVsAddressW : m_cbPsAddressW);
			if (cbTemp->getSelectedIndex() !=-1)
				p.SamplerState.AddressW = TextureAddressModeConverter.Parse(cbTemp->getItems()[cbTemp->getSelectedIndex()]);

			p.SamplerState.BorderColor = (isVS? m_cfVsBorderColor : m_cfPsBorderColor)->GetValue();

			cbTemp = (isVS ? m_cbVsMagFilter : m_cbPsMagFilter);
			if (cbTemp->getSelectedIndex() !=-1)
				p.SamplerState.MagFilter = TextureFilterConverter.Parse(cbTemp->getItems()[cbTemp->getSelectedIndex()]);

			cbTemp = (isVS ? m_cbVsMinFilter : m_cbPsMinFilter);
			if (cbTemp->getSelectedIndex() !=-1)
				p.SamplerState.MinFilter = TextureFilterConverter.Parse(cbTemp->getItems()[cbTemp->getSelectedIndex()]);

			cbTemp = (isVS ? m_cbVsMipFilter : m_cbPsMipFilter);
			if (cbTemp->getSelectedIndex() !=-1)
				p.SamplerState.MipFilter = TextureFilterConverter.Parse(cbTemp->getItems()[cbTemp->getSelectedIndex()]);

			p.SamplerState.MaxAnisotropy = StringUtils::ParseInt32((isVS ? m_tbVsMaxAnisotropy : m_tbPsMaxAnisotropy)->getText());
			p.SamplerState.MaxMipLevel = StringUtils::ParseInt32((isVS ? m_tbVsMaxMipLevel : m_tbPsMaxMipLevel)->getText());
			p.SamplerState.MipMapLODBias = StringUtils::ParseInt32((isVS ? m_tbVsMipMapLODBias : m_tbPsMipMapLODBias)->getText());

		}
		else
		{
			p.SamplerState = ShaderSamplerState();
		}
	}

	void EffectDocument::DownloadParameter(const EffectParameter& p, bool isVS)
	{
		ComboBox* cbTemp;
		CheckBox* chTemp;
		int idx = -1;
		String strV = EffectParameter::ToString(p.Usage);
		cbTemp = isVS ? m_cbVsUsage : m_cbPsUsage;
		for (int i=0;i<cbTemp->getItems().getCount();i++)
			if (cbTemp->getItems()[i]==strV)
			{
				idx = i;break;
			}
		cbTemp->setSelectedIndex(idx);

		chTemp = isVS ? m_cbVsIsCustom : m_cbPsIsCustom;
		chTemp->Checked = p.Usage == EPUSAGE_CustomMaterialParam;
		if (idx == -1)
			chTemp->Checked = true;

		if (isVS)
			CBVSIsCustom_Changed(0);
		else
			CBPSIsCustom_Changed(0);
		
		(isVS? m_tbVsCustomUsage : m_tbPsCustomUsage)->SetText(p.CustomMaterialParamName);

		bool hasSamplerState = false;
		idx = -1;
		strV = TextureAddressModeConverter.ToString(p.SamplerState.AddressU);
		cbTemp = isVS ? m_cbVsAddressU : m_cbPsAddressU;
		for (int i=0;i<cbTemp->getItems().getCount();i++)
			if (cbTemp->getItems()[i]==strV)
			{
				idx = i;break;
			}
		cbTemp->setSelectedIndex(idx);
		hasSamplerState |= p.SamplerState.AddressU != TextureAddressMode::Wrap;

		idx = -1;
		strV = TextureAddressModeConverter.ToString(p.SamplerState.AddressV);
		cbTemp = isVS ? m_cbVsAddressV : m_cbPsAddressV;
		for (int i=0;i<cbTemp->getItems().getCount();i++)
			if (cbTemp->getItems()[i]==strV)
			{
				idx = i;break;
			}
		cbTemp->setSelectedIndex(idx);
		hasSamplerState |= p.SamplerState.AddressV != TextureAddressMode::Wrap;

		idx = -1;
		strV = TextureAddressModeConverter.ToString(p.SamplerState.AddressW);
		cbTemp = isVS ? m_cbVsAddressW : m_cbPsAddressW;
		for (int i=0;i<cbTemp->getItems().getCount();i++)
			if (cbTemp->getItems()[i]==strV)
			{
				idx = i;break;
			}
		cbTemp->setSelectedIndex(idx);
		hasSamplerState |= p.SamplerState.AddressW != TextureAddressMode::Wrap;

		(isVS? m_cfVsBorderColor : m_cfPsBorderColor)->SetValue(Color4(p.SamplerState.BorderColor));
		hasSamplerState |= !!p.SamplerState.BorderColor;

		idx = -1;
		strV = TextureFilterConverter.ToString(p.SamplerState.MagFilter);
		cbTemp = isVS ? m_cbVsMagFilter : m_cbPsMagFilter;
		for (int i=0;i<cbTemp->getItems().getCount();i++)
			if (cbTemp->getItems()[i]==strV)
			{
				idx = i;break;
			}
		cbTemp->setSelectedIndex(idx);
		hasSamplerState |= p.SamplerState.MagFilter != TextureFilter::Point;

		idx = -1;
		strV = TextureFilterConverter.ToString(p.SamplerState.MinFilter);
		cbTemp = isVS ? m_cbVsMinFilter : m_cbPsMinFilter;
		for (int i=0;i<cbTemp->getItems().getCount();i++)
			if (cbTemp->getItems()[i]==strV)
			{
				idx = i;break;
			}
		cbTemp->setSelectedIndex(idx);
		hasSamplerState |= p.SamplerState.MinFilter != TextureFilter::Point;

		idx = -1;
		strV = TextureFilterConverter.ToString(p.SamplerState.MipFilter);
		cbTemp = isVS ? m_cbVsMipFilter : m_cbPsMipFilter;
		for (int i=0;i<cbTemp->getItems().getCount();i++)
			if (cbTemp->getItems()[i]==strV)
			{
				idx = i;break;
			}
		cbTemp->setSelectedIndex(idx);
		hasSamplerState |= p.SamplerState.MipFilter != TextureFilter::None;

		(isVS ? m_tbVsMaxAnisotropy : m_tbPsMaxAnisotropy)->SetText(StringUtils::IntToString(p.SamplerState.MaxAnisotropy));
		hasSamplerState |= p.SamplerState.MaxAnisotropy!=1;

		(isVS ? m_tbVsMaxMipLevel : m_tbPsMaxMipLevel)->SetText(StringUtils::IntToString(p.SamplerState.MaxMipLevel));
		hasSamplerState |= !!p.SamplerState.MaxMipLevel;

		(isVS ? m_tbVsMipMapLODBias : m_tbPsMipMapLODBias)->SetText(StringUtils::IntToString(p.SamplerState.MipMapLODBias));
		hasSamplerState |= !!p.SamplerState.MipMapLODBias;

		if (p.Usage >= EPUSAGE_Tex0 && p.Usage <= EPUSAGE_Tex16)
			hasSamplerState = true;

		(isVS ? m_cbVsHasSamplerState : m_cbPsHasSamplerState)->Checked = hasSamplerState;

		if (isVS)
			CBVSHasSampler_Changed(0);
		else
			CBPSHasSampler_Changed(0);
	}

	void EffectDocument::VSParams_Selected(int x, int y)
	{
		String name = m_vsParams->getItems().at(x,0);
		for (int i=0;i<m_parameters.getCount();i++)
		{
			if (m_parameters[i].ProgramType == ShaderType::Vertex && 
				m_parameters[i].Name == name)
			{
				DownloadParameter(m_parameters[i], true);
				break;
			}
		}
	}
	void EffectDocument::PSParams_Selected(int x, int y)
	{
		String name = m_psParams->getItems().at(x,0);
		for (int i=0;i<m_parameters.getCount();i++)
		{
			if (m_parameters[i].ProgramType == ShaderType::Pixel &&
				m_parameters[i].Name == name)
			{
				DownloadParameter(m_parameters[i], false);
				break;
			}
		}
	}

	void EffectDocument::VSAddParam_Clicked(Button* ctrl)
	{
		wchar_t buffer[260] = {0};
		if (CWin32InputBox::InputBox(L"Name", L"", buffer, 260) )
		{
			String name = buffer;
			if (name.size())
			{
				EffectParameter ep(name);
				ep.ProgramType = ShaderType::Vertex;
				m_parameters.Add(ep);
				RefreshParameterList();
			}
		}
	}
	void EffectDocument::VSRemoveParam_Clicked(Button* ctrl)
	{
		if (m_vsParams->getSelectedRowIndex() !=-1)
		{
			String name = m_vsParams->getItems().at(m_vsParams->getSelectedRowIndex(),0);

			for (int i=0;i<m_parameters.getCount();i++)
			{
				if (m_parameters[i].ProgramType == ShaderType::Vertex &&
					m_parameters[i].Name == name)
				{
					m_parameters.RemoveAt(i);
					break;
				}
			}
			RefreshParameterList();
		}
	}
	void EffectDocument::VSApplyParam_Clicked(Button* ctrl)
	{
		int x = m_vsParams->getSelectedRowIndex();
		if (x!=-1)
		{
			String name = m_vsParams->getItems().at(x,0);
			for (int i=0;i<m_parameters.getCount();i++)
			{
				if (m_parameters[i].ProgramType == ShaderType::Vertex &&
					m_parameters[i].Name == name)
				{
					UploadParameter(m_parameters[i], true);
					break;
				}
			}
			RefreshParameterList();
		}
	}

	void EffectDocument::PSAddParam_Clicked(Button* ctrl)
	{
		wchar_t buffer[260] = {0};
		if (CWin32InputBox::InputBox(L"Name", L"", buffer, 260) )
		{
			String name = buffer;
			if (name.size())
			{
				EffectParameter ep(name);
				ep.ProgramType = ShaderType::Pixel;
				m_parameters.Add(ep);
				RefreshParameterList();
			}
		}
	}
	void EffectDocument::PSRemoveParam_Clicked(Button* ctrl)
	{
		if (m_psParams->getSelectedRowIndex() !=-1)
		{
			String name = m_psParams->getItems().at(m_psParams->getSelectedRowIndex(),0);

			for (int i=0;i<m_parameters.getCount();i++)
			{
				if (m_parameters[i].ProgramType == ShaderType::Pixel &&
					m_parameters[i].Name == name)
				{
					m_parameters.RemoveAt(i);
					break;
				}
			}
			RefreshParameterList();
		}
	}
	void EffectDocument::PSApplyParam_Clicked(Button* ctrl)
	{
		int x = m_psParams->getSelectedRowIndex();
		if (x != -1)
		{
			String name = m_psParams->getItems().at(x, 0);
			for (int i = 0; i < m_parameters.getCount(); i++)
			{
				if (m_parameters[i].ProgramType == ShaderType::Pixel &&
					m_parameters[i].Name == name)
				{
					UploadParameter(m_parameters[i], false);
					break;
				}
			}
			RefreshParameterList();
		}
	}

	void EffectDocument::CBVSHasSampler_Changed(CheckBox* ctrl)
	{
		bool v = m_cbVsHasSamplerState->Checked;
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
	void EffectDocument::CBVSIsCustom_Changed(CheckBox* ctrl)
	{
		bool v = m_cbVsIsCustom->Checked;

		m_tbVsCustomUsage->Visible = v;
		m_cbVsUsage->Visible = !v;
	}

	void EffectDocument::CBPSHasSampler_Changed(CheckBox* ctrl)
	{
		bool v = m_cbPsHasSamplerState->Checked;
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
	void EffectDocument::CBPSIsCustom_Changed(CheckBox* ctrl)
	{
		bool v = m_cbPsIsCustom->Checked;

		m_tbPsCustomUsage->Visible = v;
		m_cbPsUsage->Visible = !v;
	}
}