#include "FreqDomainFilters.h"
#include "../DIPMath.h"
#include "../DIP1.h"
#include "../ImageLibrary.h"

namespace dip
{
	DemoFreqDomainFilters::DemoFreqDomainFilters(DIP1* parent, RenderDevice* device, const StyleSkin* skin)
		: SubDemo(L"Freq Domain Filters")
	{
		Texture* original256 = ImageLibrary::getSingleton().m_original256;

		ObjectFactory* fac = device->getObjectFactory();

		m_w22result1_1 = fac->CreateTexture(original256->getWidth(), original256->getHeight(), 1, TU_Static, FMT_Luminance8);
		m_w22result2_1 = fac->CreateTexture(original256->getWidth(), original256->getHeight(), 1, TU_Static, FMT_Luminance8);
		m_w22result1_2 = fac->CreateTexture(original256->getWidth(), original256->getHeight(), 1, TU_Static, FMT_Luminance8);
		m_w22result2_2 = fac->CreateTexture(original256->getWidth(), original256->getHeight(), 1, TU_Static, FMT_Luminance8);

		m_w22resultAmp = fac->CreateTexture(original256->getWidth(), original256->getHeight(), 1, TU_Static, FMT_Luminance8);
		m_w22resultPhase = fac->CreateTexture(original256->getWidth(), original256->getHeight(), 1, TU_Static, FMT_Luminance8);

		m_frmW22 = new Form(skin, device, FBS_Fixed, L"Freq Domain Filters");
		m_frmW22->setSize(256 * 4 + 5 * 5, 700);
		m_frmW22->Position.Y = 100;
		m_frmW22->ReleaseControls = true;

		Button* btnProcess = new Button(skin, Point(12, 450), 100, L"Process");
		btnProcess->eventRelease.Bind(this, &DemoFreqDomainFilters::Work22_Process);
		m_frmW22->getControls().Add(btnProcess);

		List<String> items; items.Add(L"Image1"); items.Add(L"Image2"); items.Add(L"Image3");
		m_w22ImageSel = new ComboBox(skin, Point(12, 480), 100, items);
		m_w22ImageSel->eventSelectionChanged.Bind(this, &DemoFreqDomainFilters::Work22_SelImage);
		m_frmW22->getControls().Add(m_w22ImageSel);

		int sy = 5 + skin->FormTitle->Height;

		PictureBox* pbOriginal = new PictureBox(skin, Point(5, sy), 1, original256);
		pbOriginal->setSize(256, 256);
		m_frmW22->getControls().Add(pbOriginal);
		m_w22OriginalImage = pbOriginal;

		{
			PictureBox* pbAmp = new PictureBox(skin, Point(5 + (256 + 5) * 1, sy), 1, m_w22resultAmp);
			pbAmp->setSize(256, 256);
			m_frmW22->getControls().Add(pbAmp);

			PictureBox* pbPhase = new PictureBox(skin, Point(5 + (256 + 5) * 1, sy + 260 + 25), 1, m_w22resultPhase);
			pbPhase->setSize(256, 256);
			m_frmW22->getControls().Add(pbPhase);
		}

		PictureBox* pbResult = new PictureBox(skin, Point(5 + (256 + 5) * 2, sy), 1, m_w22result1_1);
		pbResult->setSize(256, 256);
		m_frmW22->getControls().Add(pbResult);

		PictureBox* pbResult2 = new PictureBox(skin, Point(5 + (256 + 5) * 3, sy), 1, m_w22result1_2);
		pbResult2->setSize(256, 256);
		m_frmW22->getControls().Add(pbResult2);



		sy += 260;
		Label* lbl = new Label(skin, Point(5, sy), L"Original Image[256x256]", 256, TextHAlign::Center);
		m_frmW22->getControls().Add(lbl);

		lbl = new Label(skin, Point(5 + (256 + 5) * 2, sy), L"ILPF", 256, TextHAlign::Center);
		m_frmW22->getControls().Add(lbl);

		lbl = new Label(skin, Point(5 + (256 + 5) * 3, sy), L"IHPF", 256, TextHAlign::Center);
		m_frmW22->getControls().Add(lbl);


		sy += 25;
		PictureBox* pbResult1_2 = new PictureBox(skin, Point(5 + (256 + 5) * 2, sy), 1, m_w22result2_1);
		pbResult1_2->setSize(256, 256);
		m_frmW22->getControls().Add(pbResult1_2);

		PictureBox* pbResult2_2 = new PictureBox(skin, Point(5 + (256 + 5) * 3, sy), 1, m_w22result2_2);
		pbResult2_2->setSize(256, 256);
		m_frmW22->getControls().Add(pbResult2_2);

		sy += 256;


		lbl = new Label(skin, Point(5 + (256 + 5) * 2, sy), L"FLPF", 256, TextHAlign::Center);
		m_frmW22->getControls().Add(lbl);

		lbl = new Label(skin, Point(5 + (256 + 5) * 3, sy), L"FHPF", 256, TextHAlign::Center);
		m_frmW22->getControls().Add(lbl);



		sy += 25;

		lbl = new Label(skin, Point(5 + (256 + 5) * 2, sy), L"Radius(%):", 156, TextHAlign::Center);
		m_frmW22->getControls().Add(lbl);

		lbl = new Label(skin, Point(5 + (256 + 5) * 3, sy), L"Radius(%):", 156, TextHAlign::Center);
		m_frmW22->getControls().Add(lbl);

		m_w22Thres1_1 = new TextBox(skin, Point(5 + (256 + 5) * 2 + 156, sy), 100, L"15");
		m_frmW22->getControls().Add(m_w22Thres1_1);

		m_w22Thres2_1 = new TextBox(skin, Point(5 + (256 + 5) * 3 + 156, sy), 100, L"1");
		m_frmW22->getControls().Add(m_w22Thres2_1);


		SystemUI::Add(m_frmW22);
	}
	DemoFreqDomainFilters::~DemoFreqDomainFilters()
	{
		SystemUI::Remove(m_frmW22);
		delete m_frmW22;

		delete m_w22result1_1;
		delete m_w22result2_1;
		delete m_w22result1_2;
		delete m_w22result2_2;
		delete m_w22resultAmp;
		delete m_w22resultPhase;
	}

	void DemoFreqDomainFilters::Show()
	{
		m_frmW22->Show();
	}

	void DemoFreqDomainFilters::Work22_Process(Button* ctrl)
	{
		Texture* inputTex = m_w22OriginalImage->Graphic.Graphic;
		float percentageL = StringUtils::ParseSingle(m_w22Thres1_1->getText())*0.01f * inputTex->getWidth() * 0.5f;
		float percentageH = StringUtils::ParseSingle(m_w22Thres2_1->getText())*0.01f * inputTex->getWidth() * 0.5f;

		IPF ilpf(false, inputTex->getWidth(), inputTex->getHeight(),
			percentageL);
		IPF ihpf(true, inputTex->getWidth(), inputTex->getHeight(),
			percentageH);
		FPF flpf(false, inputTex->getWidth(), inputTex->getHeight(),
			percentageL);
		FPF fhpf(true, inputTex->getWidth(), inputTex->getHeight(),
			percentageH);

		int pixels = inputTex->getWidth() * inputTex->getHeight();
		Complex* freqDom = new Complex[pixels];
		float* grayValues = new float[pixels];


		DataRectangle dataR = inputTex->Lock(0, LOCK_None);

		const byte* srcPtr = (const byte*)dataR.getDataPointer();

		for (int i = 0; i < inputTex->getHeight(); i++)
		{
			for (int j = 0; j < inputTex->getWidth(); j++)
			{
				grayValues[i*inputTex->getWidth() + j] = (float)srcPtr[j];
			}
			srcPtr += dataR.getPitch();
		}

		inputTex->Unlock(0);

		fft2(grayValues, freqDom, inputTex->getWidth(), inputTex->getHeight(), true);

		delete[] grayValues;


		DataRectangle dataR1 = m_w22resultAmp->Lock(0, LOCK_None);
		DataRectangle dataR2 = m_w22resultPhase->Lock(0, LOCK_None);

		byte* destPtr = (byte*)dataR1.getDataPointer();
		byte* destPtr2 = (byte*)dataR2.getDataPointer();

		for (int i = 0; i < inputTex->getHeight(); i++)
		{
			for (int j = 0; j < inputTex->getWidth(); j++)
			{
				destPtr[j] = (byte)Math::Clamp(freqDom[i*inputTex->getWidth() + j].Mod()*0.01f, 0.0f, 255.0f);
				destPtr2[j] = (byte)Math::Clamp(freqDom[i*inputTex->getWidth() + j].Angle() / (Math::PI * 2) * 255, 0.0f, 255.0f);
			}
			destPtr += dataR1.getPitch();
			destPtr2 += dataR2.getPitch();
		}

		m_w22resultAmp->Unlock(0);
		m_w22resultPhase->Unlock(0);




		Work22_Subprocess(freqDom, ilpf, m_w22result1_1);

		Work22_Subprocess(freqDom, ihpf, m_w22result1_2);

		Work22_Subprocess(freqDom, flpf, m_w22result2_1);

		Work22_Subprocess(freqDom, fhpf, m_w22result2_2);



		delete[] freqDom;

	}
	void DemoFreqDomainFilters::Work22_Subprocess(const Complex* source, const FreqDomainFilter& filter, Texture* resultTex)
	{
		Texture* inputTex = m_w22OriginalImage->Graphic.Graphic;

		int pixels = inputTex->getWidth() * inputTex->getHeight();
		Complex* temp = new Complex[pixels];
		float* result = new float[pixels];
		memcpy(temp, source, sizeof(Complex) * pixels);


		for (int i = 0; i < inputTex->getHeight(); i++)
		{
			for (int j = 0; j < inputTex->getWidth(); j++)
			{
				float weight = filter.GetWeight(j, i);

				Complex cwei(weight, weight);

				Complex& c = temp[i*inputTex->getHeight() + j];

				c = cwei*c;
				//c = cwei;
			}
		}

		ifft2(temp, result, inputTex->getWidth(), inputTex->getHeight(), true);


		DataRectangle dataR = resultTex->Lock(0, LOCK_None);

		byte* destPtr = (byte*)dataR.getDataPointer();

		for (int i = 0; i < inputTex->getHeight(); i++)
		{
			for (int j = 0; j < inputTex->getWidth(); j++)
			{
				destPtr[j] = (byte)Math::Clamp(result[i*inputTex->getWidth() + j], 0.0f, 255.0f);
			}
			destPtr += dataR.getPitch();
		}

		resultTex->Unlock(0);
		delete[] temp;
		delete[] result;
	}
	void DemoFreqDomainFilters::Work22_SelImage(ComboBox* ctrl)
	{
		Texture* original256 = ImageLibrary::getSingleton().m_original256;
		Texture* originalAlt256 = ImageLibrary::getSingleton().m_originalAlt256;
		Texture* originalAperture256 = ImageLibrary::getSingleton().m_originalAperture256;

		switch (m_w22ImageSel->getSelectedIndex())
		{
			case 0:
				m_w22OriginalImage->Graphic = original256;
				break;
			case 1:
				m_w22OriginalImage->Graphic = originalAlt256;
				break;
			case 2:
				m_w22OriginalImage->Graphic = originalAperture256;
				break;
		}
	}
}