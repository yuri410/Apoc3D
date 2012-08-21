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
		void Form_Resized(Control* ctrl);

		String m_filePath;

		List<EffectParameter> m_parameters;

		TextBox* m_tbVertexCode;
		TextBox* m_tbPixelCode;
		
		// param
		ListView* m_vsParams;
		ListView* m_psParams;

		// 
		ComboBox* m_cbUsage;
		CheckBox* m_cbIsCustom;
		TextBox* m_tbCustomUsage;

		// sampler state editor
		CheckBox* m_cbHasSamplerState;
		ComboBox* m_cbAddressU;
		ComboBox* m_cbAddressV;
		ComboBox* m_cbAddressW;
		ColorField* m_cfBorderColor;
		ComboBox* m_cbMagFilter;
		ComboBox* m_cbMinFilter;
		ComboBox* m_cbMipFilter;
		TextBox* m_tbMaxAnisotropy;
		TextBox* m_tbMaxMipLevel;
		TextBox* m_tbMipMapLODBias;

		Button* m_vsAddParam;
		Button* m_vsRemoveParam;


		FastList<Label*> m_labels;

	};
}

#endif