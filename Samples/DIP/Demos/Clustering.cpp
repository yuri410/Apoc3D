
#include "Clustering.h"
#include "../DIPMath.h"

namespace dip
{
	DemoClustering::DemoClustering(DIP1* parent, RenderDevice* device, const StyleSkin* skin)
		: SubDemo(L"Clustering"), m_skin(skin)
	{

		{
			FileLocateRule rule;
			LocateCheckPoint cp;
			cp.AddPath(L"dmining.pak");
			rule.AddCheckPoint(cp);

			ObjectFactory* fac = device->getObjectFactory();
			{
				FileLocation fl = FileSystem::getSingleton().Locate(L"a4.tex", rule);
				m_dmOriginals[0] = TextureManager::getSingleton().CreateUnmanagedInstance(device, fl, false);

				fl = FileSystem::getSingleton().Locate(L"a3.tex", rule);
				m_dmOriginals[1] = TextureManager::getSingleton().CreateUnmanagedInstance(device, fl, false);

				fl = FileSystem::getSingleton().Locate(L"a2.tex", rule);
				m_dmOriginals[2] = TextureManager::getSingleton().CreateUnmanagedInstance(device, fl, false);

				fl = FileSystem::getSingleton().Locate(L"a1.tex", rule);
				m_dmOriginals[3] = TextureManager::getSingleton().CreateUnmanagedInstance(device, fl, false);

				fl = FileSystem::getSingleton().Locate(L"b1.tex", rule);
				m_dmOriginals[4] = TextureManager::getSingleton().CreateUnmanagedInstance(device, fl, false);

				fl = FileSystem::getSingleton().Locate(L"b2.tex", rule);
				m_dmOriginals[5] = TextureManager::getSingleton().CreateUnmanagedInstance(device, fl, false);

				fl = FileSystem::getSingleton().Locate(L"b3.tex", rule);
				m_dmOriginals[6] = TextureManager::getSingleton().CreateUnmanagedInstance(device, fl, false);

				fl = FileSystem::getSingleton().Locate(L"b4.tex", rule);
				m_dmOriginals[7] = TextureManager::getSingleton().CreateUnmanagedInstance(device, fl, false);

			}

			for (int i = 0; i < DMCount; i++)
			{
				m_dmSpecturm[i] = fac->CreateTexture(m_dmOriginals[i]->getWidth(), m_dmOriginals[i]->getHeight(), 1, TU_Static, FMT_Luminance8);
			}
		}

		m_frmDM = new Form(skin, device, FBS_Fixed, L"Clustering");
		m_frmDM->setSize(133 * DMCount + 5 * 2 + 128, 720);
		m_frmDM->Position.Y = 100;
		m_frmDM->ReleaseControls = true;

		Button* m_btnProcess = new Button(skin, Point(12, 27), 100, L"Process");
		m_btnProcess->eventRelease.Bind(this, &DemoClustering::WorkDM_Process);
		m_frmDM->getControls().Add(m_btnProcess);


		for (int i = 0; i < DMCount; i++)
		{
			PictureBox* pbOriginal = new PictureBox(skin, Point(5 + i * 133, 60), 1, m_dmOriginals[i]);
			pbOriginal->setSize(128, 128);
			m_frmDM->getControls().Add(pbOriginal);
		}


		for (int i = 0; i < DMCount; i++)
		{
			m_dmPicBoxSpecturm[i] = new PictureBox(skin, Point(5 + i * 133, 60 + 128 + 15), 1, m_dmSpecturm[i]);
			m_dmPicBoxSpecturm[i]->setSize(128, 128);
			m_frmDM->getControls().Add(m_dmPicBoxSpecturm[i]);

			m_dmInitCluster[i] = new CheckBox(skin, Point(5 + i * 133, 60 + 128 + 20 + 128), L"Initial Cluster", false);
			m_dmInitCluster[i]->SetSize(128, 30);
			m_frmDM->getControls().Add(m_dmInitCluster[i]);

		}


		SystemUI::Add(m_frmDM);
	}
	DemoClustering::~DemoClustering()
	{
		SystemUI::Remove(m_frmDM);
		delete m_frmDM;

		for (int i = 0; i < DMCount; i++)
		{
			delete m_dmOriginals[i];
			delete m_dmSpecturm[i];
		}

	}

	void DemoClustering::Show()
	{
		m_frmDM->Show();
	}



	void DemoClustering::WorkDM_Process(Button* ctrl)
	{

		for (int i = 0; i < m_resultDisplays.getCount(); i++)
		{
			m_frmDM->getControls().Remove(m_resultDisplays[i]);
			delete m_resultDisplays[i];
		}
		m_resultDisplays.Clear();

		double* dmSources[DMCount];
		for (int k = 0; k < DMCount; k++)
		{
			Texture* inputTex = m_dmOriginals[k];
			Texture* outputTex = m_dmSpecturm[k];

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

			dmSources[k] = new double[inputTex->getHeight()*inputTex->getWidth()];// freqDom;


			dataR = outputTex->Lock(0, LOCK_None);

			byte* destPtr = (byte*)dataR.getDataPointer();

			for (int i = 0; i < outputTex->getHeight(); i++)
			{
				for (int j = 0; j < outputTex->getWidth(); j++)
				{
					destPtr[j] = (byte)Math::Clamp(freqDom[i*inputTex->getWidth() + j].Mod()*0.01f, 0.0f, 255.0f);
					dmSources[k][i*inputTex->getWidth() + j] = freqDom[i*inputTex->getWidth() + j].Mod();
				}
				destPtr += dataR.getPitch();
			}

			outputTex->Unlock(0);
		}

		//double** data = new double*[DMCount];
		SignatureVector** vectors = new SignatureVector*[DMCount];
		for (int k = 0; k < DMCount; k++)
		{
			//data[k] = new double[512];
			double* sigData = new double[512];

			//double* temp = new double[512];
			bool ret = Matrix_EigenValue(dmSources[k], 256, 2000, 0.1, sigData);

			assert(ret);

			vectors[k] = new SignatureVector(sigData, 512);

			//delete[] temp;
		}


		List<int> icoords(DMCount);
		for (int i = 0; i < DMCount; i++)
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
			for (int i = 0; i < icoords.getCount(); i++)
			{
				const Cluster* clus = cluster[i];
				for (int j = 0; j < clus->CurrentMemberShip.getCount(); j++)
				{
					int imgIdx = clus->CurrentMemberShip[j];
					PictureBox* pb = new PictureBox(m_skin, Point(5 + sx, 60 + 256 + 70), 1, m_dmOriginals[imgIdx]);
					pb->setSize(128, 128);
					m_frmDM->getControls().Add(pb);
					m_resultDisplays.Add(pb);
					sx += 133;
				}
				if (i != icoords.getCount() - 1)
				{
					PictureBox* pb = new PictureBox(m_skin, Point(5 + sx, 60 + 256 + 70), 1);
					pb->setSize(32, 128);
					m_frmDM->getControls().Add(pb);
					m_resultDisplays.Add(pb);
					sx += 37;
				}
			}
		}

		for (int k = 0; k < DMCount; k++)
		{
			delete[] dmSources[k];
			delete vectors[k];
		}
		delete[] vectors;
	}

}