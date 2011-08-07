#include "TextureViewer.h"

#include "Graphics/RenderSystem/Sprite.h"
#include "Graphics/RenderSystem/Texture.h"
#include "Math/ColorValue.h"
#include "UILib/Form.h"
#include "UILib/PictureBox.h"

namespace APDesigner
{
	TextureThumbViewer::TextureThumbViewer(RenderDevice* device)
		: m_texture(0), m_generatedThumb(0)
	{
		m_form = new Form(Form::FBS_None, L"Texture thumbnail");

		m_pictureBox = new PictureBox(Point::Zero, 0, 1);
		m_form->getControls().Add(m_pictureBox);
		m_form->Initialize(device);
	}
	void TextureThumbViewer::PictureBox_Draw(Sprite* sprite, Apoc3D::Math::Rectangle* rect)
	{
		switch (m_texture->getType())
		{
		case TT_Texture1D:
			break;
		case TT_Texture2D:
			sprite->Draw(m_texture, *rect, 0, CV_White);
			break;
		case TT_CubeTexture:
			
			break;
		}
	}

	void TextureThumbViewer::Show(const Point& position, Texture* texture)
	{
		m_texture = texture;

		switch(m_texture->getType())
		{
		case TT_CubeTexture:
			break;
		case TT_Texture3D:
			break;
		}

		m_form->Position = position;
		m_form->Show();
	}

	void TextureThumbViewer::Hide()
	{
		m_form->Close();
		if (m_generatedThumb)
			delete m_generatedThumb;
		m_texture = 0;
	}
}