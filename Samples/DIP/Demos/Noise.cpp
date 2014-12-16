#include "Noise.h"
#include "../DIP1.h"
#include "../ImageLibrary.h"

namespace dip
{
	DemoNoise::DemoNoise(DIP1* parent, RenderDevice* device, const StyleSkin* skin)
		: SubDemo(L"Noise Generation and Reduction")
	{
		ObjectFactory* fac = device->getObjectFactory();

		Texture* original = ImageLibrary::getSingleton().m_original;

		m_noise = fac->CreateTexture(original->getWidth(), original->getHeight(), 1, TU_Static, FMT_Luminance8);
		m_noise2 = fac->CreateTexture(original->getWidth(), original->getHeight(), 1, TU_Static, FMT_Luminance8);
		m_result1_1 = fac->CreateTexture(original->getWidth(), original->getHeight(), 1, TU_Static, FMT_Luminance8);
		m_result1_2 = fac->CreateTexture(original->getWidth(), original->getHeight(), 1, TU_Static, FMT_Luminance8);
		m_result2_1 = fac->CreateTexture(original->getWidth(), original->getHeight(), 1, TU_Static, FMT_Luminance8);
		m_result2_2 = fac->CreateTexture(original->getWidth(), original->getHeight(), 1, TU_Static, FMT_Luminance8);

		m_form = new Form(skin, device, FBS_Fixed, getName());
		m_form->setSize(256 * 4 + 5 * 5, 620);
		m_form->Position.Y = 100;
		m_form->ReleaseControls = true;

		ControlCollection& cc = m_form->getControls();

		cc.Add(m_sbpLabel1 = new Label(skin, Point(12, 330), L"", 200, TextHAlign::Center));
		cc.Add(m_sbpLabel2 = new Label(skin, Point(12, 390), L"", 200, TextHAlign::Center));

		m_sbPercentage1 = new SliderBar(skin, Point(30, 360), BarDirection::Horizontal, 150);
		m_sbPercentage2 = new SliderBar(skin, Point(30, 420), BarDirection::Horizontal, 150);
		m_sbPercentage1->eventValueChanged.Bind(this, &DemoNoise::SliderPercentage1_ValueChange);
		m_sbPercentage2->eventValueChanged.Bind(this, &DemoNoise::SliderPercentage2_ValueChange);
		m_sbPercentage1->CurrentValue = m_percentage1;
		m_sbPercentage2->CurrentValue = m_percentage2;
		cc.Add(m_sbPercentage1);
		cc.Add(m_sbPercentage2);

		Button* m_btnProcess = new Button(skin, Point(30, 460), 100, L"Process");
		m_btnProcess->eventRelease.Bind(this, &DemoNoise::Process);
		cc.Add(m_btnProcess);


		int sy = 5 + skin->FormTitle->Height;
		PictureBox* pbOriginal = new PictureBox(skin, Point(5, sy), 1, original);
		pbOriginal->setSize(256, 256);
		cc.Add(pbOriginal);

		PictureBox* pbNoise = new PictureBox(skin, Point(5 + 256 + 5, sy), 1, m_noise);
		pbNoise->setSize(256, 256);
		cc.Add(pbNoise);

		PictureBox* pbAF1 = new PictureBox(skin, Point(5 + (256 + 5) * 2, sy), 1, m_result1_1);
		pbAF1->setSize(256, 256);
		cc.Add(pbAF1);

		PictureBox* pbMF1 = new PictureBox(skin, Point(5 + (256 + 5) * 3, sy), 1, m_result1_2);
		pbMF1->setSize(256, 256);
		cc.Add(pbMF1);


		sy += 260;
		cc.Add(new Label(skin, Point(5, sy), L"Original Image", 256, TextHAlign::Center));
		cc.Add(m_noiseLabel1 = new Label(skin, Point(5 + 256 + 5, sy), L"", 256, TextHAlign::Center));
		cc.Add(new Label(skin, Point(5 + (256 + 5) * 2, sy), L"Averaging Filter[3x3]", 256, TextHAlign::Center));
		cc.Add(new Label(skin, Point(5 + (256 + 5) * 3, sy), L"Median Filter[3x3]", 256, TextHAlign::Center));


		sy += 25;
		PictureBox* pbNoise2 = new PictureBox(skin, Point(5 + 256 + 5, sy), 1, m_noise2);
		pbNoise2->setSize(256, 256);
		cc.Add(pbNoise2);

		PictureBox* pbAF2 = new PictureBox(skin, Point(5 + (256 + 5) * 2, sy), 1, m_result2_1);
		pbAF2->setSize(256, 256);
		cc.Add(pbAF2);

		PictureBox* pbMF2 = new PictureBox(skin, Point(5 + (256 + 5) * 3, sy), 1, m_result2_2);
		pbMF2->setSize(256, 256);
		cc.Add(pbMF2);

		sy += 256;
		cc.Add(m_noiseLabel2 = new Label(skin, Point(5 + 256 + 5, sy), L"Salt & Pepper Noise[20%]", 256, TextHAlign::Center));
		cc.Add(new Label(skin, Point(5 + (256 + 5) * 2, sy), L"Averaging Filter[5x5]", 256, TextHAlign::Center));
		cc.Add(new Label(skin, Point(5 + (256 + 5) * 3, sy), L"Median Filter[5x5]", 256, TextHAlign::Center));

		SliderPercentage1_ValueChange(m_sbPercentage1, false);
		SliderPercentage2_ValueChange(m_sbPercentage2, false);

		SystemUI::Add(m_form);
	}

	DemoNoise::~DemoNoise()
	{
		SystemUI::Remove(m_form);
		delete m_form;
		delete m_noise;
		delete m_noise2;

		delete m_result1_1;
		delete m_result1_2;
		delete m_result2_1;
		delete m_result2_2;

	}


	void DemoNoise::Process(Button* ctrl)
	{
		Texture* original = ImageLibrary::getSingleton().m_original;

		const float percentage1 = m_percentage1;
		const float percentage2 = m_percentage2;

		DataRectangle dataR1 = m_noise->Lock(0, LOCK_None);
		DataRectangle dataR2 = m_noise2->Lock(0, LOCK_None);
		DataRectangle dataR = original->Lock(0, LOCK_ReadOnly);
		const byte* srcData = (const byte*)dataR.getDataPointer();
		byte* dstR1 = (byte*)dataR1.getDataPointer();
		byte* dstR2 = (byte*)dataR2.getDataPointer();
		for (int i = 0; i < dataR.getHeight(); i++)
		{
			for (int j = 0; j < dataR.getWidth(); j++)
			{
				float rnd = Randomizer::NextFloat() * 2 - 1;

				if (rnd > 1.0f - percentage1)
				{
					dstR1[j] = 0xff;
				}
				else if (rnd < -1 + percentage1)
				{
					dstR1[j] = 0;
				}
				else
				{
					dstR1[j] = srcData[j];
				}

				rnd = Randomizer::NextFloat() * 2 - 1;

				if (rnd > 1.0f - percentage2)
				{
					dstR2[j] = 0xff;
				}
				else if (rnd < -1 + percentage2)
				{
					dstR2[j] = 0;
				}
				else
				{
					dstR2[j] = srcData[j];
				}
			}
			srcData += dataR.getPitch();
			dstR1 += dataR1.getPitch();
			dstR2 += dataR2.getPitch();
		}
		original->Unlock(0);

		// ======================================================

		DataRectangle res11 = m_result1_1->Lock(0, LOCK_None);
		DataRectangle res21 = m_result2_1->Lock(0, LOCK_None);
		DataRectangle res12 = m_result1_2->Lock(0, LOCK_None);
		DataRectangle res22 = m_result2_2->Lock(0, LOCK_None);

		byte* res11ptr = (byte*)res11.getDataPointer();
		byte* res21ptr = (byte*)res21.getDataPointer();
		byte* res12ptr = (byte*)res12.getDataPointer();
		byte* res22ptr = (byte*)res22.getDataPointer();

		dstR1 = (byte*)dataR1.getDataPointer();
		dstR2 = (byte*)dataR2.getDataPointer();

		for (int i = 0; i < dataR1.getHeight(); i++)
		{
			for (int j = 0; j < dataR1.getWidth(); j++)
			{
				int total = 0;

				byte filter[9];

				for (int ii = -1; ii <= 1; ii++)
					for (int jj = -1; jj <= 1; jj++)
					{
						int boxX = jj + j;
						int boxY = ii + i;

						if (boxX < 0) boxX = 0;
						if (boxY < 0) boxY = 0;
						if (boxX >= dataR1.getWidth()) boxX = dataR1.getWidth();
						if (boxY >= dataR1.getHeight()) boxY = dataR1.getHeight();

						total += dstR1[boxY*dataR1.getPitch() + boxX];
						filter[(ii + 1) * 3 + jj + 1] = dstR1[boxY*dataR1.getPitch() + boxX];
					}
				total /= 9;

				int total2 = 0;
				byte filter2[25];
				for (int ii = -2; ii <= 2; ii++)
					for (int jj = -2; jj <= 2; jj++)
					{
						int boxX = jj + j;
						int boxY = ii + i;

						if (boxX < 0) boxX = 0;
						if (boxY < 0) boxY = 0;
						if (boxX >= dataR1.getWidth()) boxX = dataR1.getWidth();
						if (boxY >= dataR1.getHeight()) boxY = dataR1.getHeight();

						total2 += dstR2[boxY*dataR2.getPitch() + boxX];
						filter2[(ii + 2) * 5 + jj + 2] = dstR2[boxY*dataR2.getPitch() + boxX];
					}
				total2 /= 25;

				if (total > 255)total = 255;
				if (total2 > 255)total2 = 255;

				res11ptr[j] = (byte)total;
				res21ptr[j] = (byte)total2;

				for (int ii = 0; ii < 9; ii++)
					for (int jj = ii; jj < 9; jj++)
						if (filter[ii] < filter[jj])
						{
							byte temp = filter[jj];
							filter[jj] = filter[ii];
							filter[ii] = temp;
						}
				res12ptr[j] = filter[4];
				for (int ii = 0; ii < 25; ii++)
					for (int jj = ii; jj < 25; jj++)
						if (filter2[ii] < filter2[jj])
						{
							byte temp = filter2[jj];
							filter2[jj] = filter2[ii];
							filter2[ii] = temp;
						}
				res22ptr[j] = filter2[12];
			}
			res11ptr += res11.getPitch();
			res21ptr += res21.getPitch();
			res12ptr += res12.getPitch();
			res22ptr += res22.getPitch();
		}

		m_result1_1->Unlock(0);
		m_result2_1->Unlock(0);
		m_result1_2->Unlock(0);
		m_result2_2->Unlock(0);


		m_noise->Unlock(0);
		m_noise2->Unlock(0);
	}

	void DemoNoise::SliderPercentage1_ValueChange(SliderBar* sb, bool)
	{
		m_percentage1 = sb->CurrentValue;
		String pt = StringUtils::SingleToString(m_percentage1 * 100, StrFmt::fpdec<0>::val) + L"%";

		m_sbpLabel1->SetText(L"Noise Percentage 1: " + pt);
		m_noiseLabel1->SetText(L"Salt & Pepper Noise[" + pt + L"]");
	}
	void DemoNoise::SliderPercentage2_ValueChange(SliderBar* sb, bool)
	{
		m_percentage2 = sb->CurrentValue;
		String pt = StringUtils::SingleToString(m_percentage2 * 100, StrFmt::fpdec<0>::val) + L"%";

		m_sbpLabel2->SetText(L"Noise Percentage 2: " + StringUtils::SingleToString(m_percentage2 * 100, StrFmt::fpdec<0>::val) + L"%");
		m_noiseLabel2->SetText(L"Salt & Pepper Noise[" + pt + L"]");
	}

	void DemoNoise::Show()
	{
		m_form->Show();
	}
}