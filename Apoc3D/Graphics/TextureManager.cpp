/* -----------------------------------------------------------------------
 * This source file is part of Apoc3D Framework
 * 
 * Copyright (c) 2009-2018 Tao Xin
 * 
 * This content of this file is subject to the terms of the Mozilla Public 
 * License v2.0. If a copy of the MPL was not distributed with this file, 
 * you can obtain one at http://mozilla.org/MPL/2.0/.
 * 
 * This program is distributed in the hope that it will be useful, 
 * WITHOUT WARRANTY OF ANY KIND; either express or implied. See the 
 * Mozilla Public License for more details.
 * 
 * ------------------------------------------------------------------------
 */

#include "TextureManager.h"
#include "RenderSystem/RenderDevice.h"
#include "RenderSystem/Texture.h"
#include "apoc3d/Vfs/ResourceLocation.h"
#include "apoc3d/Core/ResourceHandle.h"
#include "apoc3d/Core/Logging.h"
#include "apoc3d/Utility/StringUtils.h"

using namespace Apoc3D::Utility;

namespace Apoc3D
{
	namespace Graphics
	{
		SINGLETON_IMPL(TextureManager);

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
				L"Texture manager initialized with a cache size " + StringUtils::IntToString(CacheSize) + (UseCache ? L". Use async streaming." : L"."), 
				LOGLVL_Infomation);
		}


		TextureManager::~TextureManager(void)
		{
			if (m_redirectLocation)
				delete m_redirectLocation;
		}



		Texture* TextureManager::CreateUnmanagedInstance(RenderDevice* rd, const FileLocation& fl, bool generateMips)
		{
			const FileLocation* selectedFloc = &fl;

			ObjectFactory* factory = rd->getObjectFactory();
			if (m_redirectLocation)
			{
				selectedFloc = m_redirectLocation;
			}

			Texture* result = factory->CreateTexture(*selectedFloc, generateMips ? (TextureUsage)(TU_AutoMipMap | TU_Static) : TU_Static, false);
			//result->Load();
			return result;
		}
		ResourceHandle<Texture>* TextureManager::CreateInstance(RenderDevice* rd, const FileLocation& fl, bool generateMips)
		{
			const FileLocation* selectedFloc = &fl;

			if (m_redirectLocation)
			{
				selectedFloc = m_redirectLocation;
			}

			Resource* retrived = Exists(selectedFloc->GetHashString());
			if (retrived == nullptr)
			{
				ObjectFactory* factory = rd->getObjectFactory();
				Texture* tex = factory->CreateTexture(*selectedFloc, generateMips ? (TextureUsage)(TU_AutoMipMap | TU_Static) : TU_Static, true);
				
				NotifyNewResource(tex);

				if (!usesAsync() && tex->getState() == ResourceState::Loaded)
				{
					return new ResourceHandle<Texture>((Texture*)tex, ResourceHandle<Texture>::FLG_Untouching);		
				}
				return new ResourceHandle<Texture>((Texture*)tex);	
			}
			return new ResourceHandle<Texture>((Texture*)retrived);
		}

	}
}
