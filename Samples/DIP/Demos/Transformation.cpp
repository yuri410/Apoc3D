#include "Transformation.h"
#include "../DIP1.h"
#include "../ImageLibrary.h"

namespace dip
{
	DemoTransformation::DemoTransformation(DIP1* parent, RenderDevice* device, const StyleSkin* skin)
		: SubDemo(L"Transformation")
	{
		Texture* original256 = ImageLibrary::getSingleton().m_original256;

		ObjectFactory* fac = device->getObjectFactory();

		m_result = fac->CreateTexture(original256->getWidth(), original256->getHeight(), 1, TU_Static, FMT_Luminance8);

		m_form = new Form(skin, device, FBS_Fixed, L"Transformation");
		m_form->setSize(256 * 2 + 5 * 3, 500 - 128);
		m_form->Position.Y = 100;
		m_form->ReleaseControls = true;

		int sy = 5 + skin->FormTitle->Height;
		PictureBox* pbOriginal = new PictureBox(skin, Point(5, sy), 1, original256);
		pbOriginal->setSize(256, 256);
		m_form->getControls().Add(pbOriginal);

		PictureBox* pbResult = new PictureBox(skin, Point(5 + 256 + 5, sy), 1, m_result);
		pbResult->setSize(256, 256);
		m_form->getControls().Add(pbResult);

		Button* btnProcess = new Button(skin, Point(12, 460 - 128), 100, L"Process");
		btnProcess->eventRelease.Bind(this, &DemoTransformation::Button_Process);
		m_form->getControls().Add(btnProcess);

		sy += 260;
		Label* lbl = new Label(skin, Point(5, sy), L"Original Image[256x256]", 256, TextHAlign::Center);
		m_form->getControls().Add(lbl);

		lbl = new Label(skin, Point(5 + 256 + 5, sy), L"Rotated", 256, TextHAlign::Center);
		m_form->getControls().Add(lbl);


		SystemUI::Add(m_form);
	}
	DemoTransformation::~DemoTransformation()
	{
		SystemUI::Remove(m_form);
		delete m_form;
		delete m_result;

	}

	void DemoTransformation::Show()
	{
		m_form->Show();
	}

	void DemoTransformation::Button_Process(Button* ctrl)
	{
		Texture* original256 = ImageLibrary::getSingleton().m_original256;

		Matrix preTransl;
		Matrix rotation;

		Matrix postTransl;

		Matrix::CreateTranslation(preTransl, -127, -127, 0);
		Matrix::CreateRotationZ(rotation, Math::PI / 6.0f);

		Matrix::CreateTranslation(postTransl, 127, 127, 0);

		Matrix temp, temp2;
		Matrix::Multiply(temp, preTransl, rotation);
		Matrix::Multiply(temp2, temp, postTransl);
		//Matrix::Inverse(temp, temp2);

		DataRectangle dataR1 = m_result->Lock(0, LOCK_None);

		DataRectangle dataR = original256->Lock(0, LOCK_ReadOnly);
		const byte* srcData = (const byte*)dataR.getDataPointer();
		byte* dstR1 = (byte*)dataR1.getDataPointer();
		for (int i = 0; i < dataR1.getHeight(); i++)
		{
			for (int j = 0; j < dataR1.getWidth(); j++)
			{
				Vector3 p = Vector3((float)j, (float)i, 0);

				p = Vector3::TransformCoordinate(p, temp2);

				int sx = (int)p.X;
				int sy = (int)p.Y;


				if (sx < 0 || sy < 0 || sx >= dataR.getWidth() || sy >= dataR.getHeight())
				{
					dstR1[j] = 0;
				}
				else
				{
					dstR1[j] = (byte)(srcData[sy * dataR.getPitch() + sx]);
				}
			}
			dstR1 += dataR1.getPitch();
		}
		original256->Unlock(0);

		m_result->Unlock(0);
	}
}