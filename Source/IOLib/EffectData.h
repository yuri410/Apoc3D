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

#ifndef APOC3D_EFFECTDATA_H
#define APOC3D_EFFECTDATA_H

#include "Common.h"
#include "Collections/FastList.h"
#include "Graphics/EffectSystem/EffectParameter.h"

using namespace Apoc3D::Graphics::EffectSystem;
using namespace Apoc3D::Collections;
using namespace Apoc3D::VFS;
using namespace Apoc3D::IO;

namespace Apoc3D
{
	namespace IO
	{
		/** Defines one entire effect's data stored in binary form and procedures to load/save them.
		*/
		class APAPI EffectData
		{
		public:
			String Name;
			/** The vertex shader code. Could be compiled HLSL microcode or GLSL source 
			 *  depends on the
			 */
			char* VSCode;
			char* PSCode;
			/** The length of VSCode in bytes.
			*/
			int VSLength;
			int PSLength;
			
			/** The major version of shader model expected.
			*/
			int MajorVer;
			int MinorVer;

			/** Indicates whether this effect is from a AFX or CFX. 
				AFX is loaded by AutomaticEffect, while CFX is the custom one.
			*/
			bool IsCFX;

			List<EffectParameter> Parameters;

			EffectData() : VSCode(0), PSCode(0), VSLength(0),PSLength(0),MajorVer(0), MinorVer(0),IsCFX(false) { }
			~EffectData() 
			{
				if (VSCode) delete[] VSCode; 
				if (PSCode) delete[] PSCode;
			}

			void Load(const ResourceLocation* rl);
			void Save(Stream* strm) const;

		private:
			void LoadAFXV3(BinaryReader* br);
			void LoadAFXV3_1(BinaryReader* br);
		};
	}
}

#endif