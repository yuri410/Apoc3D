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
#ifndef SCENE_PROCEDURE_H
#define SCENE_PROCEDURE_H

#include "Common.h"
#include "Collections/FastList.h"
#include "ScenePassTypes.h"
//#include "Graphics/GraphicsCommon.h"

using namespace Apoc3D::Collections;
using namespace Apoc3D::VFS;
using namespace Apoc3D::Graphics;
using namespace Apoc3D::Graphics::RenderSystem;
using namespace Apoc3D::Graphics::EffectSystem;

using namespace std;

namespace Apoc3D
{
	namespace Scene
	{
		/* Represent a sequence of scene passes that can finally 
		   generate end result.

		   A SceneProcedure can be either normal passes (like shadow
		   mapping) or post effect passes (like bloom & HDR).

		   This class also manages resources and vars used in rendering.
		*/
		class APAPI SceneProcedure
		{
		private:
			RenderDevice* m_renderDevice;

			FastList<ScenePass*> m_passes;
			SceneVariable** m_vars;
			int m_varCount;

			FastList<RenderTarget*> m_createdRenderTarget;
			FastList<ResourceHandle<Texture>*> m_createdTextures;

			bool m_isAvailable;


			const Camera* m_lastCamera;
		public:
			const Camera* getLastCamera() const { return m_lastCamera; }

			SceneProcedure(RenderDevice* device);
			~SceneProcedure(void);

			bool IsAvailable();

			void Load(const ResourceLocation* rl);

			void Invoke(const FastList<Camera*> cameras, SceneManager* sceMgr, BatchData* batchData);
		};
	};
};
#endif