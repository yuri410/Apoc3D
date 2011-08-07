#ifndef TEXTUREVIEWER_H
#define TEXTUREVIEWER_H

#include "APDCommon.h"

using namespace Apoc3D::Graphics;

namespace APDesigner
{
	class TextureThumbViewer
	{
	private:
		Texture* m_texture;

		Form* m_form;
		PictureBox* m_pictureBox;

	public:
		TextureThumbViewer();

		void PictureBox_Draw(Sprite* sprite, Apoc3D::Math::Rectangle* rect);

		void Show(const Point& position);
		void Hide();
	};
	class TextureViewer : public Document
	{

	};
}

#endif