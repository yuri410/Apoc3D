#pragma once
/*
-----------------------------------------------------------------------------
This source file is part of Apoc3D Engine

Copyright (c) 2009+ Tao Xin

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
