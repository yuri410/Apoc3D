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
#ifndef EFFECTDOCUMENT_H
#define EFFECTDOCUMENT_H

#include "Document.h"
#include "UILib/Control.h"
#include "Graphics/EffectSystem/EffectParameter.h"

using namespace Apoc3D;
using namespace Apoc3D::Graphics;
using namespace Apoc3D::Graphics::Animation;
using namespace Apoc3D::Graphics::RenderSystem;
using namespace Apoc3D::Graphics::EffectSystem;
using namespace Apoc3D::UI;
using namespace Apoc3D::Scene;

namespace APDesigner
{
	class EffectDocument : public Document
	{
	public:
		EffectDocument(MainWindow* window, const String& file);
		~EffectDocument();

		virtual void LoadRes();
		virtual void SaveRes();
		virtual bool IsReadOnly() { return false; };

		virtual void Initialize(RenderDevice* device);
		virtual void Update(const GameTime* const time);
		virtual void Render();
	private:
		void CreateVSParamPanel(MainWindow* window);
		void CreatePSParamPanel(MainWindow* window);
		void RefreshParameterList();
		void DownloadParameter(const EffectParameter& p, bool isVS);
		void UploadParameter(EffectParameter& p, bool isVS);



		void Form_Resized(Control* ctrl);
		void CBVSHasSampler_Changed(Control* ctrl);
		void CBVSIsCustom_Changed(Control* ctrl);
		void CBPSHasSampler_Changed(Control* ctrl);
		void CBPSIsCustom_Changed(Control* ctrl);
		void PSAddParam_Clicked(Control* ctrl);
		void PSRemoveParam_Clicked(Control* ctrl);
		void PSApplyParam_Clicked(Control* ctrl);
		void VSAddParam_Clicked(Control* ctrl);
		void VSRemoveParam_Clicked(Control* ctrl);
		void VSApplyParam_Clicked(Control* ctrl);
		void VSParams_Selected(int x, int y);
		void PSParams_Selected(int x, int y);

		String m_filePath;

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

		FastList<Label*> m_labels;

	};
}

#endif