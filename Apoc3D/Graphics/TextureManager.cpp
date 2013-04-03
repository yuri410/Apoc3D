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
#include "TextureManager.h"
#include "RenderSystem/RenderDevice.h"
#include "RenderSystem/ObjectFactory.h"
#include "RenderSystem/Texture.h"
#include "apoc3d/Vfs/ResourceLocation.h"
#include "apoc3d/Core/ResourceHandle.h"
#include "apoc3d/Core/Logging.h"
#include "apoc3d/Utility/StringUtils.h"

using namespace Apoc3D::Utility;

SINGLETON_DECL(Apoc3D::Graphics::TextureManager);

namespace Apoc3D
{
	namespace Graphics
	{
		int64 TextureManager::CacheSize = 200 * 1048576;
		bool TextureManager::UseCache = true;

		void TextureManager::SetRedirectLocation(FileLocation* fl)
		{
			if (m_redirectLocation)
			{
				delete m_redirectLocation;
			}
			m_redirectLocation = fl;
		}

		TextureManager::TextureManager()
			: ResourceManager(L"Texture Manager ", CacheSize, UseCache), m_redirectLocation(0)
		{
			LogManager::getSingleton().Write(LOG_System, 
				L"Texture manager initialized with a cache size " + StringUtils::ToString(CacheSize) + (UseCache ? L". Use async streaming." : L"."), 
				LOGLVL_Infomation);
		}


		TextureManager::~TextureManager(void)
		{
			if (m_redirectLocation)
				delete m_redirectLocation;
		}



		Texture* TextureManager::CreateUnmanagedInstance(RenderDevice* rd, FileLocation* fl, bool genMips)
		{
			assert(fl);
			ObjectFactory* factory = rd->getObjectFactory();
			if (m_redirectLocation)
			{
				delete fl;
				fl = new FileLocation(*m_redirectLocation);
			}
			Texture* result = factory->CreateTexture(fl, genMips? TU_AutoMipMap : TU_Static, false);
			//result->Load();
			return result;
		}
		ResourceHandle<Texture>* TextureManager::CreateInstance(RenderDevice* rd, FileLocation* fl, bool genMips)
		{
			assert(fl);
			if (m_redirectLocation)
			{
				delete fl;
				fl = new FileLocation(*m_redirectLocation);
			}

			Resource* retrived = Exists(fl->GetHashString());
			if (!retrived)
			{
				ObjectFactory* factory = rd->getObjectFactory();
				Texture* tex = factory->CreateTexture(fl, genMips? TU_AutoMipMap : TU_Static, true);
				retrived = tex;
				NotifyNewResource(tex);
			}
			return new ResourceHandle<Texture>((Texture*)retrived);
		}

	}
}
