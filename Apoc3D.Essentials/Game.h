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

#ifndef AEGAME_H
#define AEGAME_H

#include "Common.h"

#include "Graphics/RenderSystem/RenderWindowHandler.h"


using namespace Apoc3D;
using namespace Apoc3D::Graphics::RenderSystem;
using namespace Apoc3D::Math;
using namespace Apoc3D::UI;

namespace Apoc3DEx
{
	class APAPI Game : public RenderWindowHandler
	{
	public:
		Game(RenderWindow* wnd);
		~Game();


		virtual void Initialize();
		virtual void Finalize();

		virtual void Load();
		virtual void Unload();
		virtual void Update(const GameTime* const time);
		virtual void Draw(const GameTime* const time);

		virtual void OnFrameStart();
		virtual void OnFrameEnd();

		void Exit();

		RenderDevice* getRenderDevice() const { return m_device; }
		RenderWindow* getWindow() const { return m_window; }

		const StyleSkin* getSystemUISkin() const { return m_UIskin; }
	protected:
		RenderWindow* m_window;
		RenderDevice* m_device;

		StyleSkin* m_UIskin;
		Console* m_console;
	};
}
#endif