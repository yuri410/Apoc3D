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
#ifndef SCENEPASSTYPES_H
#define SCENEPASSTYPES_H

#include "Common.h"
#include "Collections/FastList.h"

using namespace Apoc3D::Core;
using namespace Apoc3D::Collections;
using namespace Apoc3D::VFS;
using namespace Apoc3D::Graphics;
using namespace Apoc3D::Graphics::RenderSystem;
using namespace Apoc3D::Graphics::EffectSystem;

namespace Apoc3D
{
	namespace Scene
	{
		enum SceneVariableType
		{
			VARTYPE_RenderTarget,
			VARTYPE_Matrix,
			VARTYPE_Vector4,
			VARTYPE_Vector3,
			VARTYPE_Vector2,
			VARTYPE_Texture,
			//VARTYPE_Camera,
			VARTYPE_Integer,
			VARTYPE_Boolean,
			VARTYPE_Effect
		};
		//enum SceneVariableBinding
		//{
		//	SVBIND_Camera
		//};

		struct SceneVariable
		{
			SceneVariableType Type;
			uint DefaultValue[16];
			String DefaultStringValue;

			RenderTarget* RTValue;
			ResourceHandle<Texture>* TextureValue;
			Effect* EffectValue;
		};

		enum SceneOpCode
		{
			SOP_Pop,
			SOP_Load,
			SOP_SelectorID,
			SOP_JZ,
			SOP_VisibleTo,
			SOP_Render
		};
		struct SceneInstruction
		{
			SceneOpCode Operation;
			const SceneVariable* A;
			const SceneVariable* B;
			int Next;

			SceneInstruction() 
			{
			}
			SceneInstruction(SceneOpCode code, const  SceneVariable* a = 0, const SceneVariable* b = 0)
				: Operation(code), A(a), B(b)
			{

			}
		};
		struct ScenePassData
		{
			int32 SelectorID;
			FastList<SceneInstruction> Instructions;
			FastList<SceneVariable> Variables;
		};
	}
}
#endif