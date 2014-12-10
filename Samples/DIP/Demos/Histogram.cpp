#include "Histogram.h"
#include "../DIP1.h"

namespace dip
{
	HistogramData::HistogramData()
	{
		ZeroArray(PixelCount);
	}

	void HistogramData::Draw(Sprite* sprite, const Apoc3D::Math::Rectangle& rect)
	{
		sprite->Draw(SystemUI::GetWhitePixel(), rect, CV_White);
		if (HighestPixelCount == 0)
			return;

		for (int i = 0; i < 256; i++)
		{
			int cx = (rect.Width * i) / 256 + rect.X;
			int nx = (rect.Width * (i + 1)) / 256 + rect.X;

			Apoc3D::Math::Rectangle spike;
			spike.X = cx;

			spike.Width = nx - cx + 1;
			if (spike.Width < 1)
				spike.Width=1;

			spike.Height = (PixelCount[i] * rect.Height) / HighestPixelCount;
			spike.Y = rect.Height - spike.Height + rect.Y;
			sprite->Draw(SystemUI::GetWhitePixel(), spike, CV_Black);
		}
	}
	void HistogramData::Build(Texture* texture)
	{
		assert(texture->getFormat() == FMT_Luminance8);

		ZeroArray(PixelCount);
		HighestPixelCount = 0;
		
		DataRectangle dataR = texture->Lock(0, LOCK_ReadOnly);
		const byte* srcData = (const byte*)dataR.getDataPointer();
		for (int i = 0; i < dataR.getHeight(); i++)
		{
			for (int j = 0; j<dataR.getWidth(); j++)
			{
				PixelCount[srcData[j]]++;
				if (PixelCount[srcData[j]] > HighestPixelCount)
					HighestPixelCount = PixelCount[srcData[j]];
			}
			srcData += dataR.getPitch();
		}
		texture->Unlock(0);
		TotalPixelCount = dataR.getWidth() * dataR.getHeight();
	}

	//////////////////////////////////////////////////////////////////////////

	DemoHistogram::DemoHistogram(DIP1* parent, RenderDevice* device, const StyleSkin* skin)
		: SubDemo(L"Histogram Processing"), m_original(parent->getSourceTexture())
	{
		ObjectFactory* fac = device->getObjectFactory();
		m_result1 = fac->CreateTexture(m_original->getWidth(), m_original->getHeight(), 1, TU_Static, FMT_Luminance8);
		m_result2 = fac->CreateTexture(m_original->getWidth(), m_original->getHeight(), 1, TU_Static, FMT_Luminance8);
		m_originalHgram.Build(m_original);


		m_form = new Form(skin, device, FBS_Fixed, getName());
		m_form->setSize(256 * 3 + 5 * 4, 510);
		m_form->Position.Y = 100;
		m_form->ReleaseControls = true;

		ControlCollection& cc = m_form->getControls();

		int sy = 5 + skin->FormTitle->Height;
		PictureBox* m_pbOriginal = new PictureBox(skin, Point(5, sy), 1, m_original);
		m_pbOriginal->setSize(256, 256);
		cc.Add(m_pbOriginal);

		PictureBox* m_pbResult1 = new PictureBox(skin, Point(5 + 256 + 5, sy), 1, m_result1);
		m_pbResult1->setSize(256, 256);
		cc.Add(m_pbResult1);

		PictureBox* m_pbResult2 = new PictureBox(skin, Point(5 + (256 + 5) * 2, sy), 1, m_result2);
		m_pbResult2->setSize(256, 256);
		cc.Add(m_pbResult2);


		Button* m_btnProcess = new Button(skin, Point(12, 470), 100, L"Process");
		m_btnProcess->eventRelease.Bind(this, &DemoHistogram::Process);
		cc.Add(m_btnProcess);

		sy = 283;
		cc.Add(new Label(skin, Point(5, sy), L"Original Image", 256, TextHAlign::Center));
		cc.Add(new Label(skin, Point(5 + 256 + 5, sy), L"(1.5, 1.2)", 256, TextHAlign::Center));
		cc.Add(new Label(skin, Point(5 + (256 + 5) * 2, sy), L"(0.7, 1.2)", 256, TextHAlign::Center));

		sy = 310;

		PictureBox* m_pbOriginalHG = new PictureBox(skin, Point(5, sy), 0);
		m_pbOriginalHG->setSize(256, 128);
		m_pbOriginalHG->eventPictureDraw.Bind(this, &DemoHistogram::Work11_HG1);
		cc.Add(m_pbOriginalHG);

		PictureBox* m_pbResult1HG = new PictureBox(skin, Point(5 + 256 + 5, sy), 0);
		m_pbResult1HG->setSize(256, 128);
		m_pbResult1HG->eventPictureDraw.Bind(this, &DemoHistogram::Work11_HG2);
		cc.Add(m_pbResult1HG);

		PictureBox* m_pbResult2HG = new PictureBox(skin, Point(5 + (256 + 5) * 2, sy), 0);
		m_pbResult2HG->setSize(256, 128);
		m_pbResult2HG->eventPictureDraw.Bind(this, &DemoHistogram::Work11_HG3);
		cc.Add(m_pbResult2HG);

		sy = 440;
		cc.Add(new Label(skin, Point(5, sy), L"Histogram", 256, TextHAlign::Center));
		cc.Add(new Label(skin, Point(5 + 256 + 5, sy), L"Histogram", 256, TextHAlign::Center));
		cc.Add(new Label(skin, Point(5 + (256 + 5) * 2, sy), L"Histogram", 256, TextHAlign::Center));

		SystemUI::Add(m_form);
	}

	DemoHistogram::~DemoHistogram()
	{
		SystemUI::Remove(m_form);

		delete m_form;
		delete m_result1;
		delete m_result2;
	}


	void DemoHistogram::Show()
	{
		m_form->Show();
	}

	void DemoHistogram::Process(Button* ctrl)
	{
		DataRectangle dataR1 = m_result1->Lock(0, LOCK_None);
		DataRectangle dataR2 = m_result2->Lock(0, LOCK_None);

		DataRectangle dataR = m_original->Lock(0, LOCK_ReadOnly);
		const byte* srcData = (const byte*)dataR.getDataPointer();
		byte* dstR1 = (byte*)dataR1.getDataPointer();
		byte* dstR2 = (byte*)dataR2.getDataPointer();
		for (int i = 0; i < dataR.getHeight(); i++)
		{
			for (int j = 0; j < dataR.getWidth(); j++)
			{
				float val = srcData[j] * 1.5f + 1.2f;
				val = Math::Clamp(val, 0.0f, 255.0f);

				dstR1[j] = (byte)(val);

				val = srcData[j] * 0.7f + 1.2f;
				val = Math::Clamp(val, 0.0f, 255.0f);

				dstR2[j] = (byte)(val);
			}
			srcData += dataR.getPitch();
			dstR1 += dataR1.getPitch();
			dstR2 += dataR2.getPitch();
		}
		m_original->Unlock(0);

		m_result1->Unlock(0);
		m_result2->Unlock(0);

		m_result1HG.Build(m_result1);
		m_result2HG.Build(m_result2);
	}

	void DemoHistogram::Work11_HG1(Sprite* sprite, Apoc3D::Math::Rectangle* dstRect)
	{
		m_originalHgram.Draw(sprite, *dstRect);
	}
	void DemoHistogram::Work11_HG2(Sprite* sprite, Apoc3D::Math::Rectangle* dstRect)
	{
		m_result1HG.Draw(sprite, *dstRect);
	}
	void DemoHistogram::Work11_HG3(Sprite* sprite, Apoc3D::Math::Rectangle* dstRect)
	{
		m_result2HG.Draw(sprite, *dstRect);
	}
}