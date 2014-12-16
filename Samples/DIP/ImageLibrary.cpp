#include "ImageLibrary.h"


namespace dip
{
	SINGLETON_IMPL(ImageLibrary);

	void ImageLibrary::Load(RenderDevice* device)
	{
		FileLocateRule rule;
		LocateCheckPoint cp;
		cp.AddPath(L"dipworks.pak");
		rule.AddCheckPoint(cp);

		FileLocation fl = FileSystem::getSingleton().Locate(L"image1.tex", rule);
		m_original = TextureManager::getSingleton().CreateUnmanagedInstance(device, fl, false);

		fl = FileSystem::getSingleton().Locate(L"image256.tex", rule);
		m_colorPhoto = TextureManager::getSingleton().CreateUnmanagedInstance(device, fl, false);

		fl = FileSystem::getSingleton().Locate(L"image256_gray.tex", rule);
		m_original256 = TextureManager::getSingleton().CreateUnmanagedInstance(device, fl, false);

		fl = FileSystem::getSingleton().Locate(L"image512_gray.tex", rule);
		m_original512 = TextureManager::getSingleton().CreateUnmanagedInstance(device, fl, false);

		fl = FileSystem::getSingleton().Locate(L"image2.tex", rule);
		m_originalAlt = TextureManager::getSingleton().CreateUnmanagedInstance(device, fl, false);

		fl = FileSystem::getSingleton().Locate(L"image2_256l8.tex", rule);
		m_originalAlt256 = TextureManager::getSingleton().CreateUnmanagedInstance(device, fl, false);

		fl = FileSystem::getSingleton().Locate(L"image3_256l8.tex", rule);
		m_originalAperture256 = TextureManager::getSingleton().CreateUnmanagedInstance(device, fl, false);

		fl = FileSystem::getSingleton().Locate(L"grad.tex", rule);
		m_originalGrad = TextureManager::getSingleton().CreateUnmanagedInstance(device, fl, false);


		ObjectFactory* fac = device->getObjectFactory();

		m_white16 = fac->CreateTexture(16, 16, 1, TU_Static, FMT_A8R8G8B8);
		m_white16->FillColor(CV_White);
	}
	void ImageLibrary::Free()
	{
		delete m_white16;

		delete m_original;
		delete m_original512;
		delete m_colorPhoto;
		delete m_original256;
		delete m_originalAlt;
		delete m_originalAlt256;
		delete m_originalAperture256;
		delete m_originalGrad;

	}

}