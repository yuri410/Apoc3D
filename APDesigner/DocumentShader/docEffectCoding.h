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

#ifndef EFFECTDOCUMENT_H
#define EFFECTDOCUMENT_H

#include "APDesigner/Document.h"

namespace APDesigner
{
	class EffectDocument : public Document
	{
	public:
		EffectDocument(MainWindow* window, EditorExtension* ext, const String& file, const String& vsPath, const String& psPath);
		~EffectDocument();

		virtual void LoadRes();
		virtual void SaveRes();
		virtual bool IsReadOnly() { return false; };

		virtual void Initialize(RenderDevice* device);
		virtual void Update(const GameTime* time);
		virtual void Render();
	private:
		void CreateVSParamPanel(MainWindow* window);
		void CreatePSParamPanel(MainWindow* window);
		void RefreshParameterList();
		void DownloadParameter(const EffectParameter& p, bool isVS);
		void UploadParameter(EffectParameter& p, bool isVS);



		void Form_Resized(Control* ctrl);
		void CBVSHasSampler_Changed(CheckBox* ctrl);
		void CBVSIsCustom_Changed(CheckBox* ctrl);
		void CBPSHasSampler_Changed(CheckBox* ctrl);
		void CBPSIsCustom_Changed(CheckBox* ctrl);
		void PSAddParam_Clicked(Button* ctrl);
		void PSRemoveParam_Clicked(Button* ctrl);
		void PSApplyParam_Clicked(Button* ctrl);
		void VSAddParam_Clicked(Button* ctrl);
		void VSRemoveParam_Clicked(Button* ctrl);
		void VSApplyParam_Clicked(Button* ctrl);
		void VSParams_Selected(int x, int y);
		void PSParams_Selected(int x, int y);

		String m_filePath;
		String m_vsPath;
		String m_psPath;

		List<EffectParameter> m_parameters;

		TextBox* m_tbVertexCode;
		TextBox* m_tbPixelCode;
		
		// param
		ListView* m_vsParams;
		ListView* m_psParams;


		Button* m_vsAddParam;
		Button* m_vsRemoveParam;
		Button* m_vsApplyParam;
		// 
		ComboBox* m_cbVsUsage;
		CheckBox* m_cbVsIsCustom;
		TextBox* m_tbVsCustomUsage;

		// sampler state editor
		CheckBox* m_cbVsHasSamplerState;
		ComboBox* m_cbVsAddressU;
		ComboBox* m_cbVsAddressV;
		ComboBox* m_cbVsAddressW;
		ColorField* m_cfVsBorderColor;
		ComboBox* m_cbVsMagFilter;
		ComboBox* m_cbVsMinFilter;
		ComboBox* m_cbVsMipFilter;
		TextBox* m_tbVsMaxAnisotropy;
		TextBox* m_tbVsMaxMipLevel;
		TextBox* m_tbVsMipMapLODBias;

		Button* m_psAddParam;
		Button* m_psRemoveParam;
		Button* m_psApplyParam;
		// 
		ComboBox* m_cbPsUsage;
		CheckBox* m_cbPsIsCustom;
		TextBox* m_tbPsCustomUsage;

		// sampler state editor
		CheckBox* m_cbPsHasSamplerState;
		ComboBox* m_cbPsAddressU;
		ComboBox* m_cbPsAddressV;
		ComboBox* m_cbPsAddressW;
		ColorField* m_cfPsBorderColor;
		ComboBox* m_cbPsMagFilter;
		ComboBox* m_cbPsMinFilter;
		ComboBox* m_cbPsMipFilter;
		TextBox* m_tbPsMaxAnisotropy;
		TextBox* m_tbPsMaxMipLevel;
		TextBox* m_tbPsMipMapLODBias;

		List<Label*> m_labels;

	};
}

#endif