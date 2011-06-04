/*
-----------------------------------------------------------------------------
This source file is part of Apoc3D Engine

Copyright (c) 2009+ Tao Games

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

#include "Resource.h"
#include "ResourceManager.h"

namespace Apoc3D
{
	namespace Core
	{
		Resource::Resource(ResourceManager* manager, const String& hashString)
			: m_manager(manager), m_hashString(hashString), m_refCount(0), m_state(RS_Unloaded)
		{
			m_resLoader = new ResourceLoadOperation(this);
			m_resUnloader = new ResourceUnloadOperation(this);
		}
		Resource::~Resource()
		{
			if (isManaged())
			{
				m_manager->NotifyReleaseResource(this);
			}
			delete m_resLoader;
			delete m_resUnloader;
		}

		void Resource::Use()		
		{
			if (isManaged())
			{
				if (getState() == RS_Unloaded)
					Load();
			}			
		}
		void Resource::UseSync()
		{
			if (isManaged())
			{
				m_generation->Use(this);

				LoadSync();
			}
		}

		void Resource::LoadSync()
		{
			if (isManaged())
			{
				ResourceState state = getState();
				switch (state)
				{
				case RS_Loading:
					break;
				case RS_Unloading:
					break;
				case RS_Pending:
					break;
				case RS_Loaded:
					break;
				case RS_Unloaded:
					load();
					setState(RS_Loaded);
					break;
				}
			}
		}
		void Resource::Load()
		{
			if (isManaged())
			{
				m_generation->Use(this);

				ResourceState state = getState();
				switch (state)
				{
				case RS_Loading:
				case RS_Unloading:
				case RS_Pending:
				case RS_Loaded:
					return;
				}

				setState(RS_Pending);
				m_manager->AddTask(m_resLoader);
			}
		}

		void Resource::Unload()
		{
			if (isManaged())
			{
				assert((m_state & RS_Loaded) == RS_Loaded);

				ResourceState state = getState();
				switch (state)
				{
				case RS_Loading:
				case RS_Unloading:
				case RS_Pending:
				case RS_Unloaded:
					return;
				}

				setState(RS_Pending);
				m_manager->AddTask(m_resUnloader);
			}
		}
	}
}