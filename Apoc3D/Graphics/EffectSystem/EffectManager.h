#pragma once
#ifndef APOC3D_EFFECTSYSTEM_H
#define APOC3D_EFFECTSYSTEM_H

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

#include "apoc3d/Collections/List.h"
#include "apoc3d/Collections/HashMap.h"

using namespace Apoc3D::Collections;
using namespace Apoc3D::Core;
using namespace Apoc3D::Graphics::RenderSystem;
using namespace Apoc3D::VFS;

namespace Apoc3D
{
	namespace Graphics
	{
		namespace EffectSystem
		{
			/**
			 *  Keeps track of all shader effects.
			 *  All fx are recommended to be loaded at the beginning of your program.
			 */
			class APAPI EffectManager
			{
				SINGLETON_DECL(EffectManager);
			public:
				EffectManager() { }
				~EffectManager();

				bool HasEffect(const String& name) const;
				Effect* getEffect(const String& name) const;

				/** Loads all of a project's effects from a APBuild-generated xml list automatically. */
				void LoadEffectsFromList(RenderDevice* device, const ResourceLocation& rl);
				void LoadAutomaticEffect(RenderDevice* device, const ResourceLocation& rl);

				void RegisterCustomEffect(CustomShaderEffect* effect);

				void ReloadAutomaticEffects();
				
				void UnloadAllEffects();

				
				void Update(const AppTime* time);

				void FillEffects(List<Effect*>& list);

			private:
				typedef HashMap<String, Effect*> EffectTable;

				EffectTable m_fxTable;

				struct AutomaticEffectInfo
				{
					String Name;
					ResourceLocation* Location = nullptr;

					AutomaticEffectInfo() { }
					AutomaticEffectInfo(const String& name, const ResourceLocation* loc);
					~AutomaticEffectInfo();

					AutomaticEffectInfo(const AutomaticEffectInfo&) = delete;
					AutomaticEffectInfo& operator=(const AutomaticEffectInfo&) = delete;

					AutomaticEffectInfo(AutomaticEffectInfo&&);
					AutomaticEffectInfo& operator=(AutomaticEffectInfo&& rhs);
				};

				List<AutomaticEffectInfo> m_autoEffectInfo;
			};
		};
	};
};
#endif