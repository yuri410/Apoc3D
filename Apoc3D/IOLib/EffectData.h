#pragma once
#ifndef APOC3D_EFFECTDATA_H
#define APOC3D_EFFECTDATA_H

/* -----------------------------------------------------------------------
 * This source file is part of Apoc3D Framework
 * 
 * Copyright (c) 2011-2017 Tao Xin
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
#include "apoc3d/Graphics/EffectSystem/EffectParameter.h"

using namespace Apoc3D::Graphics::EffectSystem;
using namespace Apoc3D::Collections;
using namespace Apoc3D::VFS;
using namespace Apoc3D::IO;

namespace Apoc3D
{
	namespace IO
	{
		class APAPI EffectProfileData
		{
		public:
			static const char* Imp_HLSL;
			static const char* Imp_GLSL;

			char ImplementationType[5];

			/** The major version of shader model expected. */
			int MajorVer;
			int MinorVer;

			/** The length of shader code in bytes. */
			int VSLength = 0;
			int PSLength = 0;
			int GSLength = 0;

			/**
			 *  The shader code. Could be compiled HLSL microcode or GLSL source 
			 *  depends on the ImplementationType
			 */
			char* VSCode = nullptr;
			char* PSCode = nullptr;
			char* GSCode = nullptr;


			List<EffectParameter> Parameters;

			EffectProfileData();
			~EffectProfileData();
			
			EffectProfileData(EffectProfileData&& other);
			EffectProfileData& operator=(EffectProfileData&& other);

			EffectProfileData(const EffectProfileData&) = delete;
			EffectProfileData& operator=(const EffectProfileData&) = delete;

			void LoadV5(const String& name, BinaryReader* br);
			void SaveV5(BinaryWriter* bw);

			bool MatchImplType(const char* str) const;
			void SetImplType(const char* str);
			void SetImplType(const std::string& str);
		};

		/** 
		 *  Defines one entire effect's data stored in binary form and procedures to load/save them.
		 */
		class APAPI EffectData
		{
		public:
			String Name;

			List<EffectProfileData> Profiles;

			/**
			 *  Indicates whether this effect is from a AFX or CFX. 
			 *  AFX is loaded by AutomaticEffect, while CFX is the custom one.
			 */
			bool IsCFX = false;

			EffectData();
			~EffectData();

			EffectData(const EffectData&) = delete;
			EffectData& operator=(const EffectData&) = delete;

			void Load(const ResourceLocation& rl);
			void Save(Stream& strm) const;

			void SortProfiles();
		private:
			void LoadFXV5(BinaryReader* br);

		};
	}
}

#endif