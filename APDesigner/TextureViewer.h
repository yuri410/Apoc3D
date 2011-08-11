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
#ifndef TEXTUREVIEWER_H
#define TEXTUREVIEWER_H

#include "APDCommon.h"
#include "Document.h"

using namespace Apoc3D::Math;
using namespace Apoc3D::Graphics;
using namespace Apoc3D::Graphics::RenderSystem;
using namespace Apoc3D::UI;
using namespace Apoc3D::VFS;

namespace APDesigner
{
	class TextureThumbViewer
	{
	private:
		Texture* m_texture;

		Form* m_form;
		PictureBox* m_pictureBox;

		Texture* m_previewSlices[4];
		Texture* m_previewCubeFaces[6];

		RenderDevice* m_device;
	public:
		TextureThumbViewer(RenderDevice* device);

		void PictureBox_Draw(Sprite* sprite, Apoc3D::Math::Rectangle* rect);

		void Show(const Point& position, Texture* texture);
		void Hide();
	};

	class TextureViewer : public Document
	{
	private:
		
	public:
		TextureViewer(MainWindow* window, ResourceLocation* rl);
		~TextureViewer();
		virtual void LoadRes();
		virtual void SaveRes();
		virtual bool IsReadOnly() { return true; };
	};
}

#endif