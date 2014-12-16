#include "Huffman.h"
#include "../ImageLibrary.h"

namespace dip
{
	DemoHuffman::DemoHuffman(DIP1* parent, RenderDevice* device, const StyleSkin* skin)
		: SubDemo(L"Huffman")
	{
		Texture* original = ImageLibrary::getSingleton().m_original;

		m_frmW23 = new Form(skin, device, FBS_Fixed, L"Huffman");
		m_frmW23->setSize(256 * 3 + 5 * 4, 500);
		m_frmW23->Position.Y = 100;

		m_w23OriginalImage = new PictureBox(skin, Point(5, 5 + skin->FormTitle->Height), 1, original);
		m_w23OriginalImage->setSize(256, 256);
		m_frmW23->getControls().Add(m_w23OriginalImage);


		List2D<String> items(4, 1);
		m_w23result = new ListView(skin, Point(5 + 256 + 5, 5 + skin->FormTitle->Height), Point(400, 400), items);
		m_w23result->getColumnHeader().Add(ListView::Header(L"GrayValue", 100));
		m_w23result->getColumnHeader().Add(ListView::Header(L"Chance", 100));
		m_w23result->getColumnHeader().Add(ListView::Header(L"HaffmanCode", 100));
		m_w23result->getColumnHeader().Add(ListView::Header(L"CodeLength", 100));
		m_frmW23->getControls().Add(m_w23result);

		Button* m_btnProcess = new Button(skin, Point(12, 370), 100, L"Process");
		m_btnProcess->eventRelease.Bind(this, &DemoHuffman::Work23_Process);
		m_frmW23->getControls().Add(m_btnProcess);

		List<String> items2; items2.Add(L"Image1"); items2.Add(L"Image2");
		m_w23ImageSel = new ComboBox(skin, Point(12, 340), 100, items2);
		m_w23ImageSel->eventSelectionChanged.Bind(this, &DemoHuffman::Work23_SelImage);
		m_frmW23->getControls().Add(m_w23ImageSel);

		m_w23comRate = new Label(skin, Point(12, 410), L"", 256);
		m_frmW23->getControls().Add(m_w23comRate);

		SystemUI::Add(m_frmW23);
	}
	DemoHuffman::~DemoHuffman()
	{

	}

	void DemoHuffman::Show()
	{
		m_frmW23->Show();
	}

	void DemoHuffman::Work23_Process(Button* ctrl)
	{
		Texture* source = m_w23OriginalImage->Graphic.Graphic;

		DataRectangle dataR = source->Lock(0, LOCK_ReadOnly);
		const byte* srcData = (const byte*)dataR.getDataPointer();


		String* strCode = new String[256];
		float* hist = new float[256];
		float* feq = new float[256];
		int* hMap = new int[256];
		float entr = 0;
		float codeLeng = 0;

		for (int i = 0; i < 256; i++)
		{
			hist[i] = 0;
			hMap[i] = i;
		}

		for (int i = 0; i < dataR.getHeight(); i++)
		{
			for (int j = 0; j < dataR.getWidth(); j++)
			{
				byte grayValue = srcData[i*dataR.getPitch() + j];
				hist[grayValue]++;
			}
		}

		for (int i = 0; i < 256; i++)
		{
			hist[i] /= (dataR.getWidth()*dataR.getHeight());
			feq[i] = hist[i];
		}

		for (int i = 0; i < 255; i++)
		{
			for (int j = 0; j<255 - i; j++)
			{
				if (hist[j] > hist[j + 1])
				{
					float temp = hist[j];
					hist[j] = hist[j + 1];
					hist[j + 1] = temp;

					for (int k = 0; k < 256; k++)
					{
						if (hMap[k] == j)
							hMap[k] = j + 1;
						else if (hMap[k] == j + 1)
							hMap[k] = j;
					}
				}
			}
		}


		for (int i = 0; i < 255; i++)
		{
			if (hist[i] == 0)
				continue;

			for (int j = 1; j < 255; j++)
			{
				for (int k = 0; k < 256; k++)
				{
					if (hMap[k] == j)
					{
						strCode[k] = L"1" + strCode[k];
					}
					else if (hMap[k] == j + 1)
					{
						strCode[k] = L"0" + strCode[k];
					}
				}

				hist[j + 1] += hist[j];

				for (int k = 0; k < 256; k++)
				{
					if (hMap[k] == j)
						hMap[k] = j + 1;
				}

				for (int m = j + 1; m<255; m++)
				{
					if (hist[m] > hist[m + 1])
					{
						float temp = hist[m];
						hist[m] = hist[m + 1];
						hist[m + 1] = temp;

						for (int k = 0; k < 256; k++)
						{
							if (hMap[k] == m)
								hMap[k] = m + 1;
							else if (hMap[k] == m + 1)
								hMap[k] = m;
						}
					}
					else break;
				}
			}

			break;
		}

		m_w23result->getItems().Clear();
		for (int i = 0; i < 256; i++)
		{
			String line[4];
			line[0] = StringUtils::IntToString(i);
			line[1] = StringUtils::SingleToString(feq[i], StrFmt::fpdec<8>::val);
			line[2] = strCode[i];
			if (line[2].empty())
			{
				line[3] = L"0";
			}
			else
			{
				line[3] = StringUtils::UIntToString(strCode[i].length());

				codeLeng += feq[i] * strCode[i].length();

				if (feq[i]>0)
				{
					entr -= feq[i] * logf(feq[i]) / logf(2.0f);
				}

			}
			m_w23result->getItems().AddRow(line);
		}
		source->Unlock(0);

		m_w23comRate->SetText(L"Image Entropy: " + StringUtils::SingleToString(entr, StrFmt::fp<3>::val) +
			L"\nCompression Ratio: " + StringUtils::SingleToString(codeLeng / 8.0f, StrFmt::fp<4>::val));

		delete[] hist;
		delete[] feq;
		delete[] hMap;
	}
	void DemoHuffman::Work23_SelImage(ComboBox* ctrl)
	{
		Texture* original = ImageLibrary::getSingleton().m_original;
		Texture* originalGrad = ImageLibrary::getSingleton().m_originalGrad;

		switch (m_w23ImageSel->getSelectedIndex())
		{
			case 0:
				m_w23OriginalImage->Graphic = original;
				break;
			case 1:
				m_w23OriginalImage->Graphic = originalGrad;
				break;
		}
	}
}
