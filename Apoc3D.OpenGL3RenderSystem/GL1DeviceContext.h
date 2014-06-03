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
#ifndef GL1DDEVICECONTNET_H
#define GL1DDEVICECONTNET_H

#include "GL1Common.h"

#include "apoc3d/Collections/HashMap.h"
#include "apoc3d/Graphics/RenderSystem/DeviceContext.h"

using namespace Apoc3D::Collections;
using namespace Apoc3D::Graphics::RenderSystem;

namespace Apoc3D
{
	namespace Graphics
	{
		namespace GL1RenderSystem
		{
			class GL1DeviceContext : public DeviceContext
			{
			protected:
				virtual RenderView* create(const RenderParameters &pm);
			public:
				void NotifyWindowClosed(GL1RenderWindow* wnd);
				

				GL1DeviceContext();
				~GL1DeviceContext();

				virtual RenderDevice* getRenderDevice();


			private:

				GL1RenderWindow* m_window;
			};
		}
	}
}

#endif