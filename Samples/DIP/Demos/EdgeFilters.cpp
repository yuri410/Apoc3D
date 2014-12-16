#include "EdgeFilters.h"
#include "../DIP1.h"
#include "../ImageLibrary.h"

namespace dip
{
	DemoEdgeFilters::DemoEdgeFilters(DIP1* parent, RenderDevice* device, const StyleSkin* skin)
		: SubDemo(L"Edge Filter")
	{
		Texture* original512 = ImageLibrary::getSingleton().m_original512;
		Texture* originalAlt = ImageLibrary::getSingleton().m_originalAlt;

		ObjectFactory* fac = device->getObjectFactory();

		m_w21result1_1 = fac->CreateTexture(original512->getWidth(), original512->getHeight(), 1, TU_Static, FMT_Luminance8);
		m_w21result1_2 = fac->CreateTexture(original512->getWidth(), original512->getHeight(), 1, TU_Static, FMT_Luminance8);
		m_w21result1_3 = fac->CreateTexture(original512->getWidth(), original512->getHeight(), 1, TU_Static, FMT_Luminance8);
		m_w21result2_1 = fac->CreateTexture(originalAlt->getWidth(), originalAlt->getHeight(), 1, TU_Static, FMT_Luminance8);
		m_w21result2_2 = fac->CreateTexture(originalAlt->getWidth(), originalAlt->getHeight(), 1, TU_Static, FMT_Luminance8);
		m_w21result2_3 = fac->CreateTexture(originalAlt->getWidth(), originalAlt->getHeight(), 1, TU_Static, FMT_Luminance8);


		m_frmW21 = new Form(skin, device, FBS_Fixed, L"Edge Filter");
		m_frmW21->setSize(256 * 4 + 5 * 5, 700);
		m_frmW21->Position.Y = 100;
		m_frmW21->ReleaseControls = true;

		int sy = 5 + skin->FormTitle->Height;

		PictureBox* pbOriginal = new PictureBox(skin, Point(5, sy), 1, original512);
		pbOriginal->setSize(256, 256);
		m_frmW21->getControls().Add(pbOriginal);

		PictureBox* pbResult = new PictureBox(skin, Point(5 + 256 + 5, sy), 1, m_w21result1_1);
		pbResult->setSize(256, 256);
		m_frmW21->getControls().Add(pbResult);

		PictureBox* pbResult2 = new PictureBox(skin, Point(5 + (256 + 5) * 2, sy), 1, m_w21result1_2);
		pbResult2->setSize(256, 256);
		m_frmW21->getControls().Add(pbResult2);

		PictureBox* pbResult3 = new PictureBox(skin, Point(5 + (256 + 5) * 3, sy), 1, m_w21result1_3);
		pbResult3->setSize(256, 256);
		m_frmW21->getControls().Add(pbResult3);


		Button* btnProcess = new Button(skin, Point(12, 620), 100, L"Process");
		btnProcess->eventRelease.Bind(this, &DemoEdgeFilters::Work21_Process);
		m_frmW21->getControls().Add(btnProcess);

		sy += 260;
		Label* lbl = new Label(skin, Point(5, sy), L"Original Image[512x512]", 256, TextHAlign::Center);
		m_frmW21->getControls().Add(lbl);

		lbl = new Label(skin, Point(5 + 256 + 5, sy), L"Roberts", 256, TextHAlign::Center);
		m_frmW21->getControls().Add(lbl);

		lbl = new Label(skin, Point(5 + (256 + 5) * 2, sy), L"Sobel", 256, TextHAlign::Center);
		m_frmW21->getControls().Add(lbl);

		lbl = new Label(skin, Point(5 + (256 + 5) * 3, sy), L"Laplacian", 256, TextHAlign::Center);
		m_frmW21->getControls().Add(lbl);

		sy += 25;

		lbl = new Label(skin, Point(5 + 256 + 5, sy), L"Threshold:", 156, TextHAlign::Center);
		m_frmW21->getControls().Add(lbl);

		lbl = new Label(skin, Point(5 + (256 + 5) * 2, sy), L"Threshold:", 156, TextHAlign::Center);
		m_frmW21->getControls().Add(lbl);

		lbl = new Label(skin, Point(5 + (256 + 5) * 3, sy), L"Threshold:", 156, TextHAlign::Center);
		m_frmW21->getControls().Add(lbl);

		m_w21Thres1_1 = new TextBox(skin, Point(5 + 256 + 5 + 156, sy), 100, L"15");
		m_frmW21->getControls().Add(m_w21Thres1_1);

		m_w21Thres1_2 = new TextBox(skin, Point(5 + (256 + 5) * 2 + 156, sy), 100, L"15");
		m_frmW21->getControls().Add(m_w21Thres1_2);

		m_w21Thres1_3 = new TextBox(skin, Point(5 + (256 + 5) * 3 + 156, sy), 100, L"40");
		m_frmW21->getControls().Add(m_w21Thres1_3);


		sy += 25;

		PictureBox* pbOriginal2 = new PictureBox(skin, Point(5, sy), 1, originalAlt);
		pbOriginal2->setSize(256, 256);
		m_frmW21->getControls().Add(pbOriginal2);

		PictureBox* pbResult2_1 = new PictureBox(skin, Point(5 + 256 + 5, sy), 1, m_w21result2_1);
		pbResult2_1->setSize(256, 256);
		m_frmW21->getControls().Add(pbResult2_1);

		PictureBox* pbResult2_2 = new PictureBox(skin, Point(5 + (256 + 5) * 2, sy), 1, m_w21result2_2);
		pbResult2_2->setSize(256, 256);
		m_frmW21->getControls().Add(pbResult2_2);

		PictureBox* pbResult2_3 = new PictureBox(skin, Point(5 + (256 + 5) * 3, sy), 1, m_w21result2_3);
		pbResult2_3->setSize(256, 256);
		m_frmW21->getControls().Add(pbResult2_3);


		sy += 256;
		lbl = new Label(skin, Point(5, sy), L"Original Image[512x512]", 256, TextHAlign::Center);
		m_frmW21->getControls().Add(lbl);

		lbl = new Label(skin, Point(5 + 256 + 5, sy), L"Roberts", 256, TextHAlign::Center);
		m_frmW21->getControls().Add(lbl);

		lbl = new Label(skin, Point(5 + (256 + 5) * 2, sy), L"Sobel", 256, TextHAlign::Center);
		m_frmW21->getControls().Add(lbl);

		lbl = new Label(skin, Point(5 + (256 + 5) * 3, sy), L"Laplacian", 256, TextHAlign::Center);
		m_frmW21->getControls().Add(lbl);

		sy += 25;

		lbl = new Label(skin, Point(5 + 256 + 5, sy), L"Threshold:", 156, TextHAlign::Center);
		m_frmW21->getControls().Add(lbl);

		lbl = new Label(skin, Point(5 + (256 + 5) * 2, sy), L"Threshold:", 156, TextHAlign::Center);
		m_frmW21->getControls().Add(lbl);

		lbl = new Label(skin, Point(5 + (256 + 5) * 3, sy), L"Threshold:", 156, TextHAlign::Center);
		m_frmW21->getControls().Add(lbl);

		m_w21Thres2_1 = new TextBox(skin, Point(5 + 256 + 5 + 156, sy), 100, L"40");
		m_frmW21->getControls().Add(m_w21Thres2_1);

		m_w21Thres2_2 = new TextBox(skin, Point(5 + (256 + 5) * 2 + 156, sy), 100, L"40");
		m_frmW21->getControls().Add(m_w21Thres2_2);

		m_w21Thres2_3 = new TextBox(skin, Point(5 + (256 + 5) * 3 + 156, sy), 100, L"120");
		m_frmW21->getControls().Add(m_w21Thres2_3);

		SystemUI::Add(m_frmW21);
	}
	DemoEdgeFilters::~DemoEdgeFilters()
	{
		SystemUI::Remove(m_frmW21);
		delete m_frmW21;

		delete m_w21result1_1;
		delete m_w21result1_2;
		delete m_w21result1_3;
		delete m_w21result2_1;
		delete m_w21result2_2;
		delete m_w21result2_3;
	}

	void DemoEdgeFilters::Show() 
	{
		m_frmW21->Show();
	}


	void DemoEdgeFilters::Work21_Process(Button* ctrl)
	{
		Texture* original512 = ImageLibrary::getSingleton().m_original512;
		Texture* originalAlt = ImageLibrary::getSingleton().m_originalAlt;

		Work21_SubProcess(original512, m_w21result1_1, m_w21result1_2, m_w21result1_3,
			StringUtils::ParseInt32(m_w21Thres1_1->getText()), StringUtils::ParseInt32(m_w21Thres1_2->getText()), StringUtils::ParseInt32(m_w21Thres1_3->getText()));
		Work21_SubProcess(originalAlt, m_w21result2_1, m_w21result2_2, m_w21result2_3,
			StringUtils::ParseInt32(m_w21Thres2_1->getText()), StringUtils::ParseInt32(m_w21Thres2_2->getText()), StringUtils::ParseInt32(m_w21Thres2_3->getText()));

	}
	void DemoEdgeFilters::Work21_SubProcess(Texture* original, Texture* resultTex1, Texture* resultTex2, Texture* resultTex3,
		int thres1, int thres2, int thres3)
	{
		int threshold1 = thres1;
		int threshold2 = thres2;
		int threshold3 = thres3;

		DataRectangle dataR1 = resultTex1->Lock(0, LOCK_None);
		DataRectangle dataR2 = resultTex2->Lock(0, LOCK_None);
		DataRectangle dataR = original->Lock(0, LOCK_ReadOnly);
		const byte* srcData = (const byte*)dataR.getDataPointer();
		byte* dstR1 = (byte*)dataR1.getDataPointer();
		byte* dstR2 = (byte*)dataR2.getDataPointer();
		int* lapDataBuffer = new int[dataR.getHeight() * dataR.getWidth()];
		for (int i = 0; i < dataR.getHeight(); i++)
		{
			for (int j = 0; j<dataR.getWidth(); j++)
			{
				// ==== roberts =====
				int sy = abs(i - 1) % dataR.getHeight();
				int sx = (j + 1) % dataR.getWidth();
				int gradX = srcData[sy * dataR.getPitch() + sx] - srcData[i*dataR.getPitch() + j];
				int gradY = srcData[sy * dataR.getPitch() + j] - srcData[i*dataR.getPitch() + sx];

				float grad = sqrtf((float)gradX*gradX + gradY*gradY);
				dstR1[j] = grad > threshold1 ? 0xff : 0;

				// ==== sobel ====
				byte buffer[9];
				for (int ii = -1; ii <= 1; ii++)
					for (int jj = -1; jj <= 1; jj++)
					{
						int srcX = abs(j + jj) % dataR.getWidth();
						int srcY = abs(i + ii) % dataR.getHeight();

						buffer[(ii + 1) * 3 + jj + 1] = srcData[srcY*dataR.getPitch() + srcX];
					}

				int sobelX = buffer[0] + 2 * buffer[3] + buffer[6] - buffer[2] - 2 * buffer[3 + 2] - buffer[6 + 2];
				int sobelY = buffer[0] + 2 * buffer[1] + buffer[2] - buffer[6] - 2 * buffer[6 + 1] - buffer[6 + 2];
				grad = sqrtf((float)sobelX*sobelX + sobelY*sobelY);

				dstR2[j] = grad > threshold2 ? 0xff : 0;

				// ==== Laplacian =====
				int g =
					buffer[0] +
					buffer[1] +
					buffer[2] +

					buffer[3] +
					buffer[5] +

					buffer[6] +
					buffer[7] +
					buffer[8] -

					8 * buffer[4];

				lapDataBuffer[i * dataR.getWidth() + j] = g;
			}
			dstR1 += dataR1.getPitch();
			dstR2 += dataR2.getPitch();
		}
		resultTex1->Unlock(0);
		resultTex2->Unlock(0);
		original->Unlock(0);

		// ÕÒµ½Áã½»²æµã
		DataRectangle dataR3 = resultTex3->Lock(0, LOCK_None);
		byte* dstR3 = (byte*)dataR3.getDataPointer();
		for (int i = 0; i<dataR3.getHeight(); i++)
		{
			for (int j = 0; j<dataR3.getWidth(); j++)
			{
				int center = lapDataBuffer[i * dataR3.getWidth() + j];
				int bottom = lapDataBuffer[((i + 1) % dataR3.getHeight())*dataR3.getWidth() + j];
				int top = lapDataBuffer[(abs(i - 1) % dataR3.getHeight())*dataR3.getWidth() + j];
				int right = lapDataBuffer[i*dataR3.getWidth() + (j + 1) % dataR3.getWidth()];
				int left = lapDataBuffer[i*dataR3.getWidth() + abs(j - 1) % dataR3.getWidth()];

				if (center < 0 && bottom > 0 &&
					abs(center - bottom) > threshold3)
				{
					dstR3[j] = 0xff;
				}
				else if (center < 0 && top > 0 &&
					abs(center - top) > threshold3)
				{
					dstR3[j] = 0xff;
				}
				else if (center < 0 && right > 0 &&
					abs(center - right) > threshold3)
				{
					dstR3[j] = 0xff;
				}
				else if (center < 0 && left > 0 &&
					abs(center - left) > threshold3)
				{
					dstR3[j] = 0xff;
				}
				else if (center == 0)
				{
					if (bottom > 0 && top < 0 &&
						abs(top - bottom)>2 * threshold3)
					{
						dstR3[j] = 0xff;
					}
					else if (bottom < 0 && top > 0 &&
						abs(top - bottom) > 2 * threshold3)
					{
						dstR3[j] = 0xff;
					}
					else if (right > 0 && left < 0 &&
						abs(right - left) > 2 * threshold3)
					{
						dstR3[j] = 0xff;
					}
					else if (right < 0 && left>0 &&
						abs(right - left) > 2 * threshold3)
					{
						dstR3[j] = 0xff;
					}
				}
				else
				{
					dstR3[j] = 0;
				}
			}
			dstR3 += dataR3.getPitch();
		}

		resultTex3->Unlock(0);
	}

}