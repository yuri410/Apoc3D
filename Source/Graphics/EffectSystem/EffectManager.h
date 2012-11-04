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
#ifndef EFFECTSYSTEM_H
#define EFFECTSYSTEM_H

#include "Common.h"
#include "Core/Singleton.h"

using namespace Apoc3D::Graphics::RenderSystem;
using namespace Apoc3D::Core;
using namespace Apoc3D::VFS;
using namespace std;

namespace Apoc3D
{
	namespace Graphics
	{
		namespace EffectSystem
		{
			//template class APAPI unordered_map<String, Effect*>;
			typedef unordered_map<String, Effect*> EffectTable;

			/** Keeps track of all shader effects.
			 *  All fx are recommended to be loaded at the beginning of your program.
			 */
			class APAPI EffectManager : public Singleton<EffectManager>
			{
			private:
				EffectTable m_fxTable;

			public:
				EffectManager() { }
				~EffectManager() { }

				bool HasEffect(const String& name) const;
				Effect* getEffect(const String& name) const;

				//void ReloadEffect(RenderDevice* device, const ResourceLocation* fl);
				void LoadEffect(RenderDevice* device, const ResourceLocation* rl);
				
				void RegisterCustomEffect(CustomShaderEffect* effect);

				/** Loads all of a project's effects from a APBuild-generated xml list automatically.
				*/
				void LoadEffectFromList(RenderDevice* device, const ResourceLocation* rl);

				void Update(const GameTime* const time);

				SINGLETON_DECL_HEARDER(EffectManager);
			};
		};
	};
};
#endif