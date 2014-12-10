/*
-----------------------------------------------------------------------------
This source file is part of Apoc3D Engine

Copyright (c) 2009+ Tao Xin

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
#include "DIP1.h"
#include "DIPMath.h"

#include "Demos/Histogram.h"
#include "Demos/Noise.h"

namespace dip
{

    DIP1::DIP1(RenderWindow* wnd)
        : Game(wnd) { }

	DIP1::~DIP1()
	{		

	}

	void DIP1::Work13(MenuItem* ctrl)
	{
		m_frmW13->Show();
	}
	void DIP1::Work13_Process(Button* ctrl)
	{
		Matrix preTransl;
		Matrix rotation;

		Matrix postTransl;

		Matrix::CreateTranslation(preTransl, -127,-127,0);
		Matrix::CreateRotationZ(rotation, Math::PI/6.0f);

		Matrix::CreateTranslation(postTransl, 127,127,0);

		Matrix temp, temp2;
		Matrix::Multiply(temp, preTransl, rotation);
		Matrix::Multiply(temp2, temp, postTransl);
		//Matrix::Inverse(temp, temp2);

		DataRectangle dataR1 = m_w13result->Lock(0, LOCK_None);

		DataRectangle dataR = m_original256->Lock(0, LOCK_ReadOnly);
		const byte* srcData = (const byte*)dataR.getDataPointer();
		byte* dstR1 = (byte*)dataR1.getDataPointer();
		for (int i=0;i<dataR1.getHeight();i++)
		{
			for (int j=0;j<dataR1.getWidth();j++)
			{
				Vector3 p = Vector3((float)j,(float)i,0);

				p = Vector3::TransformCoordinate(p, temp2);

				int sx = (int)p.X;
				int sy = (int)p.Y;


				if (sx <0 || sy <0 || sx >= dataR.getWidth() || sy >= dataR.getHeight())
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
		m_original256->Unlock(0);

		m_w13result->Unlock(0);
	}
	void DIP1::Work21(MenuItem* ctrl)
	{
		m_frmW21->Show();
	}
	void DIP1::Work21_Process(Button* ctrl)
	{
		Work21_SubProcess(m_original512, m_w21result1_1, m_w21result1_2, m_w21result1_3,
			StringUtils::ParseInt32(m_w21Thres1_1->getText()), StringUtils::ParseInt32(m_w21Thres1_2->getText()), StringUtils::ParseInt32(m_w21Thres1_3->getText()));
		Work21_SubProcess(m_originalAlt, m_w21result2_1, m_w21result2_2, m_w21result2_3,
			StringUtils::ParseInt32(m_w21Thres2_1->getText()), StringUtils::ParseInt32(m_w21Thres2_2->getText()), StringUtils::ParseInt32(m_w21Thres2_3->getText()));

	}
	void DIP1::Work21_SubProcess(Texture* original, Texture* resultTex1, Texture* resultTex2, Texture* resultTex3,
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
		for (int i=0;i<dataR.getHeight();i++)
		{
			for (int j=0;j<dataR.getWidth();j++)
			{
				// ==== roberts =====
				int sy = abs(i-1) % dataR.getHeight();
				int sx = (j+1) % dataR.getWidth();
				int gradX = srcData[sy * dataR.getPitch() + sx] - srcData[i*dataR.getPitch()+j];
				int gradY = srcData[sy * dataR.getPitch() + j] - srcData[i*dataR.getPitch()+sx];

				float grad = sqrtf((float)gradX*gradX + gradY*gradY);
				dstR1[j] = grad > threshold1 ? 0xff : 0;

				// ==== sobel ====
				byte buffer[9];
				for (int ii=-1;ii<=1;ii++)
					for (int jj=-1;jj<=1;jj++)
					{
						int srcX = abs(j+jj) % dataR.getWidth();
						int srcY = abs(i+ii) % dataR.getHeight();

						buffer[(ii+1)*3+jj+1] = srcData[srcY*dataR.getPitch()+srcX];
					}

				int sobelX = buffer[0] + 2*buffer[3] + buffer[6] - buffer[2] - 2*buffer[3+2]-buffer[6+2];
				int sobelY = buffer[0] + 2*buffer[1] + buffer[2] - buffer[6] - 2*buffer[6+1]-buffer[6+2];
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

		// 找到零交叉点
		DataRectangle dataR3 = resultTex3->Lock(0, LOCK_None);
		byte* dstR3 = (byte*)dataR3.getDataPointer();
		for (int i=0;i<dataR3.getHeight();i++)
		{
			for (int j=0;j<dataR3.getWidth();j++)
			{
				int center = lapDataBuffer[i * dataR3.getWidth()+j];
				int bottom = lapDataBuffer[((i+1)%dataR3.getHeight())*dataR3.getWidth()+j];
				int top = lapDataBuffer[(abs(i-1)%dataR3.getHeight())*dataR3.getWidth()+j];
				int right = lapDataBuffer[i*dataR3.getWidth()+(j+1)%dataR3.getWidth()];
				int left = lapDataBuffer[i*dataR3.getWidth()+abs(j-1)%dataR3.getWidth()];

				if (center < 0 && bottom > 0 &&
					abs(center-bottom) > threshold3)
				{
					dstR3[j] = 0xff;
				}
				else if (center < 0 && top > 0 &&
					abs(center-top) > threshold3)
				{
					dstR3[j] = 0xff;
				}
				else if (center < 0 && right > 0 && 
					abs(center-right) > threshold3)
				{
					dstR3[j] = 0xff;
				}
				else if (center < 0 && left > 0 &&
					abs(center-left) > threshold3)
				{
					dstR3[j] = 0xff;
				}
				else if (center == 0)
				{
					if (bottom>0 && top<0 && 
						abs(top-bottom)>2*threshold3)
					{
						dstR3[j] = 0xff;
					}
					else if (bottom < 0 && top > 0 &&
						abs(top-bottom)>2*threshold3)
					{
						dstR3[j] = 0xff;
					}
					else if (right>0 && left<0 &&
						abs(right-left) > 2 * threshold3)
					{
						dstR3[j] = 0xff;
					}
					else if (right<0 && left>0 &&
						abs(right-left) > 2 * threshold3)
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

	void DIP1::Work22(MenuItem* ctrl)
	{
		m_frmW22->Show();
	}
	void DIP1::Work22_Process(Button* ctrl)
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


		DataRectangle dataR = inputTex->Lock(0,LOCK_None);

		const byte* srcPtr = (const byte*)dataR.getDataPointer();

		for (int i=0;i<inputTex->getHeight();i++)
		{
			for (int j=0;j<inputTex->getWidth();j++)
			{
				grayValues[i*inputTex->getWidth()+j] = (float)srcPtr[j];
			}
			srcPtr+= dataR.getPitch();
		}

		inputTex->Unlock(0);

		fft2(grayValues, freqDom, inputTex->getWidth(), inputTex->getHeight(), true);

		delete[] grayValues;


		DataRectangle dataR1 = m_w22resultAmp->Lock(0,LOCK_None);
		DataRectangle dataR2 = m_w22resultPhase->Lock(0,LOCK_None);

		byte* destPtr = (byte*)dataR1.getDataPointer();
		byte* destPtr2 = (byte*)dataR2.getDataPointer();

		for (int i=0;i<inputTex->getHeight();i++)
		{
			for (int j=0;j<inputTex->getWidth();j++)
			{
				destPtr[j] = (byte)Math::Clamp(freqDom[i*inputTex->getWidth()+j].Mod()*0.01f,0.0f,255.0f);
				destPtr2[j] = (byte)Math::Clamp(freqDom[i*inputTex->getWidth()+j].Angle()/(Math::PI*2)*255,0.0f,255.0f);
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
	void DIP1::Work22_Subprocess(const Complex* source, const FreqDomainFilter& filter, Texture* resultTex)
	{
		Texture* inputTex = m_w22OriginalImage->Graphic.Graphic;

		int pixels = inputTex->getWidth() * inputTex->getHeight();
		Complex* temp = new Complex[pixels];
		float* result = new float[pixels];
		memcpy(temp, source, sizeof(Complex) * pixels);


		for (int i=0;i<inputTex->getHeight();i++)
		{
			for (int j=0;j<inputTex->getWidth();j++)
			{
				float weight = filter.GetWeight(j,i);

				Complex cwei(weight,weight);

				Complex& c = temp[i*inputTex->getHeight()+j];
				
				c=cwei*c;
				//c = cwei;
			}
		}

		ifft2(temp, result, inputTex->getWidth(), inputTex->getHeight(), true);
		

		DataRectangle dataR = resultTex->Lock(0,LOCK_None);

		byte* destPtr = (byte*)dataR.getDataPointer();

		for (int i=0;i<inputTex->getHeight();i++)
		{
			for (int j=0;j<inputTex->getWidth();j++)
			{
				destPtr[j] = (byte)Math::Clamp(result[i*inputTex->getWidth()+j],0.0f,255.0f);
			}
			destPtr += dataR.getPitch();
		}

		resultTex->Unlock(0);
		delete[] temp;
		delete[] result;
	}
	void DIP1::Work22_SelImage(ComboBox* ctrl)
	{
		switch (m_w22ImageSel->getSelectedIndex())
		{
			case 0:
				m_w22OriginalImage->Graphic = m_original256;
				break;
			case 1:
				m_w22OriginalImage->Graphic = m_originalAlt256;
				break;
			case 2:
				m_w22OriginalImage->Graphic = m_originalAperture256;
				break;
		}
	}
	void DIP1::Work23(MenuItem* ctrl)
	{
		m_frmW23->Show();
	}
	void DIP1::Work23_Process(Button* ctrl)
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

		for (int i=0;i<256;i++)
		{
			hist[i] = 0;
			hMap[i] = i;
		}

		for (int i=0;i<dataR.getHeight();i++)
		{
			for (int j=0;j<dataR.getWidth();j++)
			{
				byte grayValue = srcData[i*dataR.getPitch()+j];
				hist[grayValue]++;
			}
		}

		for (int i=0;i<256;i++)
		{
			hist[i] /= (dataR.getWidth()*dataR.getHeight());
			feq[i] = hist[i];
		}

		for (int i=0;i<255;i++)
		{
			for (int j=0;j<255-i;j++)
			{
				if (hist[j] > hist[j+1])
				{
					float temp = hist[j];
					hist[j] = hist[j+1];
					hist[j+1] = temp;

					for (int k=0;k<256;k++)
					{
						if (hMap[k] == j)
							hMap[k] = j+1;
						else if (hMap[k]==j+1)
							hMap[k] = j;
					}
				}
			}
		}
		

		for (int i=0;i<255;i++)
		{
			if (hist[i] == 0)
				continue;

			for (int j=1;j<255;j++)
			{
				for (int k=0;k<256;k++)
				{
					if (hMap[k] == j)
					{
						strCode[k] = L"1" + strCode[k];
					}
					else if (hMap[k] == j+1)
					{
						strCode[k] = L"0" + strCode[k];
					}
				}

				hist[j+1] += hist[j];

				for (int k=0;k<256;k++)
				{
					if (hMap[k] == j)
						hMap[k] = j+1;
				}

				for (int m=j+1;m<255;m++)
				{
					if (hist[m] > hist[m+1])
					{
						float temp = hist[m];
						hist[m] = hist[m+1];
						hist[m+1] = temp;

						for (int k=0;k<256;k++)
						{
							if (hMap[k] == m)
								hMap[k] = m+1;
							else if (hMap[k] == m+1)
								hMap[k] = m;
						}
					}
					else break;
				}
			}

			break;
		}

		m_w23result->getItems().Clear();
		for (int i=0;i<256;i++)
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
	void DIP1::Work23_SelImage(ComboBox* ctrl)
	{
		switch (m_w23ImageSel->getSelectedIndex())
		{
		case 0:
			m_w23OriginalImage->Graphic = m_original;
			break;
		case 1:
			m_w23OriginalImage->Graphic = m_originalGrad;
			break;
		}
	}

	void DIP1::WorkDM(MenuItem* ctrl)
	{
		m_frmDM->Show();
	}
	void DIP1::WorkDM_Process(Button* ctrl)
	{

		for (int i=0;i<m_resultDisplays.getCount();i++)
		{
			m_frmDM->getControls().Remove(m_resultDisplays[i]);
			delete m_resultDisplays[i];
		}
		m_resultDisplays.Clear();

		double* dmSources[DMCount];
		for (int k=0;k<DMCount;k++)
		{
			Texture* inputTex = m_dmOriginals[k];
			Texture* outputTex = m_dmSpecturm[k];

			int pixels = inputTex->getWidth() * inputTex->getHeight();
			Complex* freqDom = new Complex[pixels];
			float* grayValues = new float[pixels];


			DataRectangle dataR = inputTex->Lock(0,LOCK_None);

			const byte* srcPtr = (const byte*)dataR.getDataPointer();

			for (int i=0;i<inputTex->getHeight();i++)
			{
				for (int j=0;j<inputTex->getWidth();j++)
				{
					grayValues[i*inputTex->getWidth()+j] = (float)srcPtr[j];
				}
				srcPtr+= dataR.getPitch();
			}

			inputTex->Unlock(0);

			fft2(grayValues, freqDom, inputTex->getWidth(), inputTex->getHeight(), true);

			delete[] grayValues;

			dmSources[k] = new double[inputTex->getHeight()*inputTex->getWidth()];// freqDom;


			dataR = outputTex->Lock(0,LOCK_None);

			byte* destPtr = (byte*)dataR.getDataPointer();

			for (int i=0;i<outputTex->getHeight();i++)
			{
				for (int j=0;j<outputTex->getWidth();j++)
				{
					destPtr[j] = (byte)Math::Clamp(freqDom[i*inputTex->getWidth()+j].Mod()*0.01f,0.0f,255.0f);
					dmSources[k][i*inputTex->getWidth()+j] = freqDom[i*inputTex->getWidth()+j].Mod();
				}
				destPtr += dataR.getPitch();
			}

			outputTex->Unlock(0);
		}

		//double** data = new double*[DMCount];
		SignatureVector** vectors = new SignatureVector*[DMCount];
		for (int k=0;k<DMCount;k++)
		{
			//data[k] = new double[512];
			double* sigData = new double[512];
			
			//double* temp = new double[512];
			bool ret = Matrix_EigenValue(dmSources[k],256,2000,0.1,sigData);

			assert(ret);

			vectors[k] = new SignatureVector(sigData, 512);

			//delete[] temp;
		}


		List<int> icoords(DMCount);
		for (int i=0;i<DMCount;i++)
		{
			if (m_dmInitCluster[i]->Checked)
			{
				icoords.Add(i);
			}
		}

		if (icoords.getCount()>0)
		{
			KMeans km(vectors, DMCount, icoords);
			km.Start();

			const Cluster* const* cluster = km.getClusters();
			int sx = 5;
			for (int i=0;i<icoords.getCount();i++)
			{
				const Cluster* clus = cluster[i];
				for (int j=0;j<clus->CurrentMemberShip.getCount();j++)
				{
					int imgIdx = clus->CurrentMemberShip[j];
					PictureBox* pb = new PictureBox(m_UIskin, Point(5 + sx,60+256+70), 1, m_dmOriginals[imgIdx]);
					pb->setSize(128, 128);
					m_frmDM->getControls().Add(pb);
					m_resultDisplays.Add(pb);
					sx+=133;
				}
				if (i != icoords.getCount()-1)
				{
					PictureBox* pb = new PictureBox(m_UIskin, Point(5 + sx,60+256+70), 1);
					pb->setSize(32,128);
					m_frmDM->getControls().Add(pb);
					m_resultDisplays.Add(pb);
					sx+=37;
				}
			}
		}
		
		for (int k=0;k<DMCount;k++)
		{
			delete[] dmSources[k];
			delete vectors[k];
		}
		delete[] vectors;
	}

	void DIP1::Draw(const GameTime* time)
	{
		m_device->Clear(CLEAR_ColorAndDepth, CV_DarkGray, 1,0);
		m_device->BeginFrame();
		
		m_sprite->Begin((Sprite::SpriteSettings)(Sprite::SPR_RestoreState | Sprite::SPR_AlphaBlended));
		{
			Apoc3D::Math::RectangleF dstRect = { 900, 50, 128, 128 };
			Apoc3D::Math::RectangleF srcRect = { 64, 64, 128, 128 };

			m_sprite->DrawRoundedRect(m_colorPhoto, dstRect, &srcRect, 25.0f, 4, CV_White);

			dstRect.Y += 192;
			m_sprite->DrawRoundedRectBorder(m_colorPhoto, dstRect, &srcRect, 2, 25.0f, 4, CV_White);


			dstRect.Y += 192;
			dstRect.Height *= 0.7f;
			m_sprite->DrawCircle(m_colorPhoto, dstRect, &srcRect, Math::PI*0.25f, Math::PI*1.75f, 16, CV_White);

			dstRect.Y += 192;
			m_sprite->DrawCircleArc(m_colorPhoto, dstRect, &srcRect, 5, Math::PI*0.25f, Math::PI*1.75f, 16, CV_White);
			
			dstRect.X -= 192;
			dstRect.Width = dstRect.Height = 4;
			m_sprite->DrawCircleArc(m_white16, dstRect, nullptr, 1, 6, CV_White);
		}

		m_sprite->End();
		
		for (int i = 0; i < m_subdemos.getCount(); i++)
		{
			m_subdemos[i]->Draw(time, m_sprite);
		}

		Game::Draw(time);


		m_device->EndFrame();

	}
	void DIP1::Initialize()
	{
		m_window->setTitle(L"DIP Demos");
		Game::Initialize();

		m_sprite = m_device->getObjectFactory()->CreateSprite();
	}
	void DIP1::Finalize()
	{
		Game::Finalize();
	}
	void DIP1::Load()
	{
		Game::Load();

		{
			FileLocateRule rule;
			LocateCheckPoint cp;
			cp.AddPath(L"dipworks.pak");
			rule.AddCheckPoint(cp);

			FileLocation fl = FileSystem::getSingleton().Locate(L"image1.tex", rule);
			m_original = TextureManager::getSingleton().CreateUnmanagedInstance(m_device, fl, false);

			fl = FileSystem::getSingleton().Locate(L"image256.tex", rule);
			m_colorPhoto = TextureManager::getSingleton().CreateUnmanagedInstance(m_device, fl, false);

			fl = FileSystem::getSingleton().Locate(L"image256_gray.tex", rule);
			m_original256 = TextureManager::getSingleton().CreateUnmanagedInstance(m_device, fl, false);

			fl = FileSystem::getSingleton().Locate(L"image512_gray.tex", rule);
			m_original512 = TextureManager::getSingleton().CreateUnmanagedInstance(m_device, fl, false);

			fl = FileSystem::getSingleton().Locate(L"image2.tex", rule);
			m_originalAlt = TextureManager::getSingleton().CreateUnmanagedInstance(m_device, fl, false);

			fl = FileSystem::getSingleton().Locate(L"image2_256l8.tex", rule);
			m_originalAlt256 = TextureManager::getSingleton().CreateUnmanagedInstance(m_device, fl, false);

			fl = FileSystem::getSingleton().Locate(L"image3_256l8.tex", rule);
			m_originalAperture256 = TextureManager::getSingleton().CreateUnmanagedInstance(m_device, fl, false);

			fl = FileSystem::getSingleton().Locate(L"grad.tex", rule);
			m_originalGrad = TextureManager::getSingleton().CreateUnmanagedInstance(m_device, fl, false);


			ObjectFactory* fac = m_device->getObjectFactory();

			m_w13result = fac->CreateTexture(m_original256->getWidth(), m_original256->getHeight(),1, TU_Static, FMT_Luminance8); 
			
			m_w21result1_1 = fac->CreateTexture(m_original512->getWidth(), m_original512->getHeight(),1, TU_Static, FMT_Luminance8); 
			m_w21result1_2 = fac->CreateTexture(m_original512->getWidth(), m_original512->getHeight(),1, TU_Static, FMT_Luminance8); 
			m_w21result1_3 = fac->CreateTexture(m_original512->getWidth(), m_original512->getHeight(),1, TU_Static, FMT_Luminance8); 
			m_w21result2_1 = fac->CreateTexture(m_originalAlt->getWidth(), m_originalAlt->getHeight(),1, TU_Static, FMT_Luminance8); 
			m_w21result2_2 = fac->CreateTexture(m_originalAlt->getWidth(), m_originalAlt->getHeight(),1, TU_Static, FMT_Luminance8); 
			m_w21result2_3 = fac->CreateTexture(m_originalAlt->getWidth(), m_originalAlt->getHeight(),1, TU_Static, FMT_Luminance8); 

			m_w22result1_1 = fac->CreateTexture(m_original256->getWidth(), m_original256->getHeight(),1, TU_Static, FMT_Luminance8); 
			m_w22result2_1 = fac->CreateTexture(m_original256->getWidth(), m_original256->getHeight(),1, TU_Static, FMT_Luminance8); 
			m_w22result1_2 = fac->CreateTexture(m_original256->getWidth(), m_original256->getHeight(),1, TU_Static, FMT_Luminance8); 
			m_w22result2_2 = fac->CreateTexture(m_original256->getWidth(), m_original256->getHeight(),1, TU_Static, FMT_Luminance8); 

			m_w22resultAmp = fac->CreateTexture(m_original256->getWidth(), m_original256->getHeight(),1, TU_Static, FMT_Luminance8); 
			m_w22resultPhase = fac->CreateTexture(m_original256->getWidth(), m_original256->getHeight(),1, TU_Static, FMT_Luminance8); 

			m_white16 = fac->CreateTexture(16, 16, 1, TU_Static, FMT_A8R8G8B8);
			m_white16->FillColor(CV_White);
		}

		{
			FileLocateRule rule;
			LocateCheckPoint cp;
			cp.AddPath(L"dmining.pak");
			rule.AddCheckPoint(cp);

			ObjectFactory* fac = m_device->getObjectFactory();
			{
				FileLocation fl = FileSystem::getSingleton().Locate(L"a4.tex", rule);
				m_dmOriginals[0] = TextureManager::getSingleton().CreateUnmanagedInstance(m_device, fl, false);

				fl = FileSystem::getSingleton().Locate(L"a3.tex", rule);
				m_dmOriginals[1] = TextureManager::getSingleton().CreateUnmanagedInstance(m_device, fl, false);

				fl = FileSystem::getSingleton().Locate(L"a2.tex", rule);
				m_dmOriginals[2] = TextureManager::getSingleton().CreateUnmanagedInstance(m_device, fl, false);

				fl = FileSystem::getSingleton().Locate(L"a1.tex", rule);
				m_dmOriginals[3] = TextureManager::getSingleton().CreateUnmanagedInstance(m_device, fl, false);

				fl = FileSystem::getSingleton().Locate(L"b1.tex", rule);
				m_dmOriginals[4] = TextureManager::getSingleton().CreateUnmanagedInstance(m_device, fl, false);

				fl = FileSystem::getSingleton().Locate(L"b2.tex", rule);
				m_dmOriginals[5] = TextureManager::getSingleton().CreateUnmanagedInstance(m_device, fl, false);

				fl = FileSystem::getSingleton().Locate(L"ba_1.tex", FileLocateRule::Default);
				m_dmOriginals[6] = TextureManager::getSingleton().CreateUnmanagedInstance(m_device, fl, false);

				fl = FileSystem::getSingleton().Locate(L"b4.tex", rule);
				m_dmOriginals[7] = TextureManager::getSingleton().CreateUnmanagedInstance(m_device, fl, false);

			}

			for (int i = 0; i < DMCount; i++)
			{
				m_dmSpecturm[i] = fac->CreateTexture(m_dmOriginals[i]->getWidth(), m_dmOriginals[i]->getHeight(), 1, TU_Static, FMT_Luminance8);
			}
		}

		m_console->Minimize();

		m_subdemos.Add(new DemoHistogram(this, m_device, m_UIskin));
		m_subdemos.Add(new DemoNoise(this, m_device, m_UIskin));


		// Transformation
		{
			m_frmW13 = new Form(m_UIskin, m_device, FBS_Fixed, L"Transformation");
			m_frmW13->setSize(256 * 2 + 5 * 3, 500 - 128);
			m_frmW13->Position.Y = 100;

			int sy = 5 + m_UIskin->FormTitle->Height;
			PictureBox* pbOriginal = new PictureBox(m_UIskin, Point(5,sy), 1, m_original);
			pbOriginal->setSize(256,256);
			m_frmW13->getControls().Add(pbOriginal);

			PictureBox* pbResult = new PictureBox(m_UIskin, Point(5+256+5,sy), 1, m_w13result);
			pbResult->setSize(256,256);
			m_frmW13->getControls().Add(pbResult);

			Button* btnProcess = new Button(m_UIskin, Point(12,460-128),100, L"Process");
			btnProcess->eventRelease.Bind(this, &DIP1::Work13_Process);
			m_frmW13->getControls().Add(btnProcess);

			sy += 260;
			Label* lbl = new Label(m_UIskin,Point(5, sy), L"Original Image[256x256]", 256, TextHAlign::Center);
			m_frmW13->getControls().Add(lbl);

			lbl = new Label(m_UIskin,Point(5 + 256 + 5, sy), L"Rotated", 256, TextHAlign::Center);
			m_frmW13->getControls().Add(lbl);


			SystemUI::Add(m_frmW13);
		}

		// Edge Filter
		{
			m_frmW21 = new Form(m_UIskin, m_device, FBS_Fixed, L"Edge Filter");
			m_frmW21->setSize(256 * 4 + 5 * 5, 700);
			m_frmW21->Position.Y = 100;


			int sy = 5 + m_UIskin->FormTitle->Height;

			PictureBox* pbOriginal = new PictureBox(m_UIskin, Point(5,sy), 1, m_original512);
			pbOriginal->setSize(256,256);
			m_frmW21->getControls().Add(pbOriginal);

			PictureBox* pbResult = new PictureBox(m_UIskin, Point(5+256+5,sy), 1, m_w21result1_1);
			pbResult->setSize(256,256);
			m_frmW21->getControls().Add(pbResult);

			PictureBox* pbResult2 = new PictureBox(m_UIskin, Point(5+(256+5)*2,sy), 1, m_w21result1_2);
			pbResult2->setSize(256,256);
			m_frmW21->getControls().Add(pbResult2);

			PictureBox* pbResult3 = new PictureBox(m_UIskin,Point(5+(256+5)*3,sy), 1, m_w21result1_3);
			pbResult3->setSize(256,256);
			m_frmW21->getControls().Add(pbResult3);


			Button* btnProcess = new Button(m_UIskin, Point(12,620),100, L"Process");
			btnProcess->eventRelease.Bind(this, &DIP1::Work21_Process);
			m_frmW21->getControls().Add(btnProcess);

			sy += 260;
			Label* lbl = new Label(m_UIskin, Point(5, sy), L"Original Image[512x512]", 256, TextHAlign::Center);
			m_frmW21->getControls().Add(lbl);

			lbl = new Label(m_UIskin, Point(5 + 256 + 5, sy), L"Roberts", 256, TextHAlign::Center);
			m_frmW21->getControls().Add(lbl);

			lbl = new Label(m_UIskin, Point(5 + (256 + 5)*2, sy), L"Sobel", 256, TextHAlign::Center);
			m_frmW21->getControls().Add(lbl);

			lbl = new Label(m_UIskin, Point(5 + (256 + 5)*3, sy), L"Laplacian", 256, TextHAlign::Center);
			m_frmW21->getControls().Add(lbl);

			sy += 25;

			lbl = new Label(m_UIskin, Point(5 + 256 + 5, sy), L"Threshold:", 156, TextHAlign::Center);
			m_frmW21->getControls().Add(lbl);

			lbl = new Label(m_UIskin, Point(5 + (256 + 5)*2, sy), L"Threshold:", 156, TextHAlign::Center);
			m_frmW21->getControls().Add(lbl);

			lbl = new Label(m_UIskin, Point(5 + (256 + 5)*3, sy), L"Threshold:", 156, TextHAlign::Center);
			m_frmW21->getControls().Add(lbl);

			m_w21Thres1_1 = new TextBox(m_UIskin, Point(5 + 256 + 5 + 156, sy), 100, L"15");
			m_frmW21->getControls().Add(m_w21Thres1_1);

			m_w21Thres1_2 = new TextBox(m_UIskin, Point(5 + (256 + 5)*2 + 156, sy), 100, L"15");
			m_frmW21->getControls().Add(m_w21Thres1_2);

			m_w21Thres1_3 = new TextBox(m_UIskin, Point(5 + (256 + 5)*3 + 156, sy), 100, L"40");
			m_frmW21->getControls().Add(m_w21Thres1_3);


			sy += 25;

			PictureBox* pbOriginal2 = new PictureBox(m_UIskin, Point(5,sy), 1, m_originalAlt);
			pbOriginal2->setSize(256,256);
			m_frmW21->getControls().Add(pbOriginal2);

			PictureBox* pbResult2_1 = new PictureBox(m_UIskin, Point(5+256+5,sy), 1, m_w21result2_1);
			pbResult2_1->setSize(256,256);
			m_frmW21->getControls().Add(pbResult2_1);

			PictureBox* pbResult2_2 = new PictureBox(m_UIskin, Point(5+(256+5)*2,sy), 1, m_w21result2_2);
			pbResult2_2->setSize(256,256);
			m_frmW21->getControls().Add(pbResult2_2);

			PictureBox* pbResult2_3 = new PictureBox(m_UIskin, Point(5+(256+5)*3,sy), 1, m_w21result2_3);
			pbResult2_3->setSize(256,256);
			m_frmW21->getControls().Add(pbResult2_3);


			sy+=256;
			lbl = new Label(m_UIskin, Point(5, sy), L"Original Image[512x512]", 256, TextHAlign::Center);
			m_frmW21->getControls().Add(lbl);

			lbl = new Label(m_UIskin, Point(5 + 256 + 5, sy), L"Roberts", 256, TextHAlign::Center);
			m_frmW21->getControls().Add(lbl);

			lbl = new Label(m_UIskin, Point(5 + (256 + 5)*2, sy), L"Sobel", 256, TextHAlign::Center);
			m_frmW21->getControls().Add(lbl);

			lbl = new Label(m_UIskin, Point(5 + (256 + 5)*3, sy), L"Laplacian", 256, TextHAlign::Center);
			m_frmW21->getControls().Add(lbl);

			sy += 25;

			lbl = new Label(m_UIskin, Point(5 + 256 + 5, sy), L"Threshold:", 156, TextHAlign::Center);
			m_frmW21->getControls().Add(lbl);

			lbl = new Label(m_UIskin, Point(5 + (256 + 5)*2, sy), L"Threshold:", 156, TextHAlign::Center);
			m_frmW21->getControls().Add(lbl);

			lbl = new Label(m_UIskin, Point(5 + (256 + 5)*3, sy), L"Threshold:", 156, TextHAlign::Center);
			m_frmW21->getControls().Add(lbl);

			m_w21Thres2_1 = new TextBox(m_UIskin, Point(5 + 256 + 5 + 156, sy), 100, L"40");
			m_frmW21->getControls().Add(m_w21Thres2_1);

			m_w21Thres2_2 = new TextBox(m_UIskin, Point(5 + (256 + 5)*2 + 156, sy), 100, L"40");
			m_frmW21->getControls().Add(m_w21Thres2_2);

			m_w21Thres2_3 = new TextBox(m_UIskin, Point(5 + (256 + 5)*3 + 156, sy), 100, L"120");
			m_frmW21->getControls().Add(m_w21Thres2_3);
			
			SystemUI::Add(m_frmW21);
		}
		// Freq Domain Filters
		{
			m_frmW22 = new Form(m_UIskin, m_device, FBS_Fixed, L"Freq Domain Filters");
			m_frmW22->setSize(256 * 4 + 5 * 5, 700);
			m_frmW22->Position.Y = 100;

			Button* btnProcess = new Button(m_UIskin, Point(12,450),100, L"Process");
			btnProcess->eventRelease.Bind(this, &DIP1::Work22_Process);
			m_frmW22->getControls().Add(btnProcess);

			List<String> items; items.Add(L"Image1"); items.Add(L"Image2"); items.Add(L"Image3");
			m_w22ImageSel = new ComboBox(m_UIskin, Point(12, 480), 100, items);
			m_w22ImageSel->eventSelectionChanged.Bind(this, &DIP1::Work22_SelImage);
			m_frmW22->getControls().Add(m_w22ImageSel);

			int sy = 5 + m_UIskin->FormTitle->Height;

			PictureBox* pbOriginal = new PictureBox(m_UIskin, Point(5,sy), 1, m_original256);
			pbOriginal->setSize(256,256);
			m_frmW22->getControls().Add(pbOriginal);
			m_w22OriginalImage = pbOriginal;

			{
				PictureBox* pbAmp = new PictureBox(m_UIskin, Point(5+(256+5)*1,sy), 1, m_w22resultAmp);
				pbAmp->setSize(256,256);
				m_frmW22->getControls().Add(pbAmp);

				PictureBox* pbPhase = new PictureBox(m_UIskin, Point(5+(256+5)*1,sy+260+25), 1, m_w22resultPhase);
				pbPhase->setSize(256,256);
				m_frmW22->getControls().Add(pbPhase);
			}

			PictureBox* pbResult = new PictureBox(m_UIskin, Point(5+(256+5)*2,sy), 1, m_w22result1_1);
			pbResult->setSize(256,256);
			m_frmW22->getControls().Add(pbResult);

			PictureBox* pbResult2 = new PictureBox(m_UIskin, Point(5+(256+5)*3,sy), 1, m_w22result1_2);
			pbResult2->setSize(256,256);
			m_frmW22->getControls().Add(pbResult2);



			sy += 260;
			Label* lbl = new Label(m_UIskin, Point(5, sy), L"Original Image[256x256]", 256, TextHAlign::Center);
			m_frmW22->getControls().Add(lbl);

			lbl = new Label(m_UIskin, Point(5 + (256 + 5)*2, sy), L"ILPF", 256, TextHAlign::Center);
			m_frmW22->getControls().Add(lbl);

			lbl = new Label(m_UIskin, Point(5 + (256 + 5)*3, sy), L"IHPF", 256, TextHAlign::Center);
			m_frmW22->getControls().Add(lbl);


			sy += 25;
			PictureBox* pbResult1_2 = new PictureBox(m_UIskin, Point(5+(256+5)*2,sy), 1, m_w22result2_1);
			pbResult1_2->setSize(256,256);
			m_frmW22->getControls().Add(pbResult1_2);

			PictureBox* pbResult2_2 = new PictureBox(m_UIskin, Point(5+(256+5)*3,sy), 1, m_w22result2_2);
			pbResult2_2->setSize(256,256);
			m_frmW22->getControls().Add(pbResult2_2);

			sy+=256;


			lbl = new Label(m_UIskin, Point(5 + (256 + 5)*2, sy), L"FLPF", 256, TextHAlign::Center);
			m_frmW22->getControls().Add(lbl);

			lbl = new Label(m_UIskin, Point(5 + (256 + 5)*3, sy), L"FHPF", 256, TextHAlign::Center);
			m_frmW22->getControls().Add(lbl);



			sy += 25;

			lbl = new Label(m_UIskin, Point(5 + (256 + 5)*2, sy), L"Radius(%):", 156, TextHAlign::Center);
			m_frmW22->getControls().Add(lbl);

			lbl = new Label(m_UIskin, Point(5 + (256 + 5)*3, sy), L"Radius(%):", 156, TextHAlign::Center);
			m_frmW22->getControls().Add(lbl);

			m_w22Thres1_1 = new TextBox(m_UIskin, Point(5 + (256 + 5)*2 + 156, sy), 100, L"15");
			m_frmW22->getControls().Add(m_w22Thres1_1);

			m_w22Thres2_1 = new TextBox(m_UIskin, Point(5 + (256 + 5)*3 + 156, sy), 100, L"1");
			m_frmW22->getControls().Add(m_w22Thres2_1);


			SystemUI::Add(m_frmW22);
		}
		// Huffman
		{
			m_frmW23 = new Form(m_UIskin, m_device, FBS_Fixed, L"Huffman");
			m_frmW23->setSize(256 * 3 + 5 * 4, 500);
			m_frmW23->Position.Y = 100;

			m_w23OriginalImage = new PictureBox(m_UIskin, Point(5,5 + m_UIskin->FormTitle->Height), 1, m_original);
			m_w23OriginalImage->setSize(256,256);
			m_frmW23->getControls().Add(m_w23OriginalImage);


			List2D<String> items(4,1);
			m_w23result = new ListView(m_UIskin, Point(5+256+5, 5 + m_UIskin->FormTitle->Height), Point(400, 400),items);
			m_w23result->getColumnHeader().Add(ListView::Header(L"GrayValue", 100));
			m_w23result->getColumnHeader().Add(ListView::Header(L"Chance", 100));
			m_w23result->getColumnHeader().Add(ListView::Header(L"HaffmanCode", 100));
			m_w23result->getColumnHeader().Add(ListView::Header(L"CodeLength", 100));
			m_frmW23->getControls().Add(m_w23result);

			Button* m_btnProcess = new Button(m_UIskin, Point(12,370),100, L"Process");
			m_btnProcess->eventRelease.Bind(this, &DIP1::Work23_Process);
			m_frmW23->getControls().Add(m_btnProcess);

			List<String> items2; items2.Add(L"Image1"); items2.Add(L"Image2");
			m_w23ImageSel = new ComboBox(m_UIskin, Point(12, 340), 100, items2);
			m_w23ImageSel->eventSelectionChanged.Bind(this, &DIP1::Work23_SelImage);
			m_frmW23->getControls().Add(m_w23ImageSel);

			m_w23comRate = new Label(m_UIskin, Point(12,410), L"", 256);
			m_frmW23->getControls().Add(m_w23comRate);

			SystemUI::Add(m_frmW23);
		}

		// Clustering
		{
			m_frmDM = new Form(m_UIskin, m_device, FBS_Fixed, L"Clustering");
			m_frmDM->setSize(133 * DMCount + 5 * 2 + 128, 720);
			m_frmDM->Position.Y = 100;

			Button* m_btnProcess = new Button(m_UIskin, Point(12,27),100, L"Process");
			m_btnProcess->eventRelease.Bind(this, &DIP1::WorkDM_Process);
			m_frmDM->getControls().Add(m_btnProcess);

			
			for (int i=0;i<DMCount;i++)
			{
				PictureBox* pbOriginal = new PictureBox(m_UIskin, Point(5 + i * 133,60), 1, m_dmOriginals[i]);
				pbOriginal->setSize(128,128);
				m_frmDM->getControls().Add(pbOriginal);
			}


			for (int i=0;i<DMCount;i++)
			{
				m_dmPicBoxSpecturm[i] = new PictureBox(m_UIskin, Point(5 + i * 133,60+128+15), 1, m_dmSpecturm[i]);
				m_dmPicBoxSpecturm[i]->setSize(128,128);
				m_frmDM->getControls().Add(m_dmPicBoxSpecturm[i]);

				m_dmInitCluster[i] = new CheckBox(m_UIskin, Point(5 + i * 133,60+128+20+128),L"Initial Cluster", false);
				m_dmInitCluster[i]->SetSize(128, 30);
				m_frmDM->getControls().Add(m_dmInitCluster[i]);

			}


			SystemUI::Add(m_frmDM);
		}

		{
			m_aboutDlg = new Form(m_UIskin, m_device, FBS_Fixed, L"About");
			m_aboutDlg->setSize(460, 175);

			Label* lbl = new Label(m_UIskin, Point(30 + 128, 30),
				L"DIP Demos by Tao Xin. \n"
				L"Powered by Apoc3D.",
				m_aboutDlg->getWidth() - 40 - 128, TextHAlign::Left);
			
			PictureBox* photo = new PictureBox(m_UIskin, Point(15, 30), 1, m_colorPhoto);
			photo->setSize(128, 128);

			m_aboutDlg->getControls().Add(photo);
			m_aboutDlg->getControls().Add(lbl);

			SystemUI::Add(m_aboutDlg);
		}


		m_mainMenu = new MenuBar(m_UIskin);


		{
			MenuItem* pojMenu = new MenuItem(L"Demos");
			SubMenu* pojSubMenu = new SubMenu(m_UIskin, nullptr);

			for (SubDemo* sd : m_subdemos)
			{
				MenuItem* mi = new MenuItem(sd->getName());
				mi->event.Bind(this, &DIP1::MenuItem_OpenDemo);
				mi->UserPointer = sd;
				pojSubMenu->Add(mi, nullptr);
			}

			MenuItem* mi = new MenuItem(L"Transformation");
			mi->event.Bind(this, &DIP1::Work13);
			pojSubMenu->Add(mi, 0);

			mi = new MenuItem(L"-");
			pojSubMenu->Add(mi, 0);

			mi = new MenuItem(L"Edge Filters");
			mi->event.Bind(this, &DIP1::Work21);
			pojSubMenu->Add(mi, 0);

			mi = new MenuItem(L"Freq Domain Filters");
			mi->event.Bind(this, &DIP1::Work22);
			pojSubMenu->Add(mi, 0);

			mi = new MenuItem(L"Huffman");
			mi->event.Bind(this, &DIP1::Work23);
			pojSubMenu->Add(mi, 0);

			mi = new MenuItem(L"Clustering");
			mi->event.Bind(this, &DIP1::WorkDM);
			pojSubMenu->Add(mi, 0);


			m_mainMenu->Add(pojMenu, pojSubMenu);
		}

		{
			MenuItem* aboutMenu = new MenuItem(L"Help");

			SubMenu* aboutSubMenu = new SubMenu(m_UIskin, nullptr);

			MenuItem* mi = new MenuItem(L"About...");
			mi->event.Bind(this, &DIP1::ShowAbout);
			aboutSubMenu->Add(mi, 0);

			m_mainMenu->Add(aboutMenu, aboutSubMenu);
		}

		SystemUI::MainMenu = m_mainMenu;

	}

	void DIP1::Unload()
	{
		Game::Unload();

		m_subdemos.DeleteAndClear();

		delete m_white16;

		delete m_original;
		delete m_original512;
		delete m_colorPhoto;
		delete m_original256;
		delete m_originalAlt;
		delete m_originalAlt256;
		delete m_originalAperture256;
		delete m_originalGrad;


		delete m_w13result;

		delete m_w21result1_1;
		delete m_w21result1_2;
		delete m_w21result1_3;
		delete m_w21result2_1;
		delete m_w21result2_2;
		delete m_w21result2_3;

		delete m_w22result1_1;
		delete m_w22result2_1;
		delete m_w22result1_2;
		delete m_w22result2_2;
		delete m_w22resultAmp;
		delete m_w22resultPhase;

		for (int i = 0; i < DMCount; i++)
		{
			delete m_dmOriginals[i];
			delete m_dmSpecturm[i];
		}

		delete m_sprite;
	}

	void DIP1::MenuItem_OpenDemo(MenuItem* mi)
	{
		for (SubDemo* sd : m_subdemos)
		{
			if (sd == mi->UserPointer)
			{
				sd->Show();
				break;
			}
		}
	}

	void DIP1::Update(const GameTime* time)
	{
		Game::Update(time);

		for (int i = 0; i < m_subdemos.getCount();i++)
		{
			m_subdemos[i]->Update(time);
		}
	}

	void DIP1::ShowAbout(MenuItem* ctrl)
	{
		m_aboutDlg->ShowModal();
	}


	void DIP1::OnFrameStart() { }
	void DIP1::OnFrameEnd() { }

}
