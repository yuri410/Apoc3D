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
#ifndef D3DPLUGIN_H
#define D3DPLUGIN_H

#include "D3D9Common.h"
#include "Core/Plugin.h"
#include "D3D9GraphicsAPIFactory.h"

using namespace Apoc3D::Core;

#ifdef APOC3D_DYNLIB
extern "C" PLUGIN Plugin* Apoc3DGetPlugin();
#endif

namespace Apoc3D
{
	namespace Graphics
	{
		namespace D3D9RenderSystem
		{
			class D3D9RSPlugin : public Plugin
			{
			private:
				D3D9GraphicsAPIFactory m_factory;
			public:
				D3D9RSPlugin();
				virtual void Load();
				virtual void Unload();

				virtual String GetName() { return L"Direct3D9 Render System"; }
			};
		}
	}
}

#endif