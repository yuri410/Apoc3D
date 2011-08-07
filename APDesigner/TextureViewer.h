#ifndef TEXTUREVIEWER_H
#define TEXTUREVIEWER_H

#include "APDCommon.h"
#include "Document.h"

using namespace Apoc3D::Math;
using namespace Apoc3D::Graphics;
using namespace Apoc3D::Graphics::RenderSystem;
using namespace Apoc3D::UI;

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

	};
}

#endif