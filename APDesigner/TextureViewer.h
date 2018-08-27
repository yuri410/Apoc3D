#pragma once

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

#include "APDCommon.h"
#include "Document.h"

namespace APDesigner
{
	class TextureThumbViewer
	{
	public:
		TextureThumbViewer(MainWindow* window);

		void PictureBox_Draw(Sprite* sprite, Apoc3D::Math::Rectangle* rect);

		void Show(const Point& position, Texture* texture);
		void Hide();
	private:
		Texture* m_texture = nullptr;

		Form* m_form;
		PictureBox* m_pictureBox;

		Texture* m_previewSlices[4];
		Texture* m_previewCubeFaces[6];

		RenderDevice* m_device;
	};

	class TextureViewer : public Document
	{
	public:
		TextureViewer(MainWindow* window, const String& name, const String& filePath);
		~TextureViewer();


		virtual void Initialize(RenderDevice* device);

		virtual void LoadRes();
		virtual void SaveRes();
		virtual bool IsReadOnly() { return true; };

		virtual void Update(const GameTime* time);
	private:
		void PixtureBox_Draw(Sprite* sprite, Apoc3D::Math::Rectangle* dstRect);
		void BtnZoomIn_Pressed(Button* ctrl);
		void BtnZoomOut_Pressed(Button* ctrl);

		String m_name;
		String m_filePath;
		Font* m_descFont;
		Texture* m_texture;
		PictureBox* m_pictureBox;
		Button* m_btnZoomIn;
		Button* m_btnZoomOut;

		int m_scale;

	};
}
