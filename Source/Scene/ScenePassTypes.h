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
		
		struct SceneVariable
		{
			String Name;
			SceneVariableType Type;
			uint Value[16];
			String DefaultStringValue;

			RenderTarget* RTValue;
			ResourceHandle<Texture>* TextureValue;
			Effect* EffectValue;

			SceneVariable()
				: TextureValue(0), RTValue(0), EffectValue(0), Type(VARTYPE_Integer)
			{
				memset(Value, 0, sizeof(Value));
			}

			SceneVariable(const SceneVariable& other)
				: Name(other.Name), DefaultStringValue(other.DefaultStringValue),
				Type(other.Type), RTValue(other.RTValue), TextureValue(other.TextureValue), EffectValue(other.EffectValue)
			{
				memcpy(Value, other.Value, sizeof(Value));
			}
			SceneVariable& operator =(const SceneVariable& other)
			{
				Name = other.Name;
				DefaultStringValue = other.DefaultStringValue;

				Type = other.Type;
				RTValue = other.RTValue;
				TextureValue = other.TextureValue;
				EffectValue = other.EffectValue;
				memcpy(Value, other.Value, sizeof(Value));
				return *this;
			}
		};

		enum SceneOpCode
		{
			SOP_Add,
			SOP_And,
			SOP_Sub,
			SOP_Mul,
			SOP_Div,
			SOP_Or,
			SOP_Not,
			SOP_Pop,
			SOP_Load,
			SOP_SelectorID,
			SOP_JZ,
			SOP_JNZ,
			SOP_Clear,
			SOP_UseRT,
			SOP_VisibleTo,
			SOP_Render,
			SOP_RenderQuad
		};

		enum ScenePostEffectParamType
		{
			SPFX_TYPE_FLOATS,
			SPFX_TYPE_INTS,
			SPFX_TYPE_BOOLS
		};
		struct SceneOpArg
		{
			bool IsImmediate;
			uint DefaultValue[16];
			SceneVariable* Var;
		};
		struct SceneInstruction
		{
			SceneOpCode Operation;
			int Next;
			std::vector<SceneOpArg> Args;

			SceneInstruction() 
			{
			}
			SceneInstruction(SceneOpCode code)
				: Operation(code)
			{
				
			}

			SceneInstruction(const SceneInstruction& other)
				: Operation(other.Operation), Next(other.Next),
				Args(other.Args)
			{

			}
			SceneInstruction& operator =(const SceneInstruction& other)
			{
				Operation = other.Operation;
				Next = other.Next;

				Args = other.Args;
				return *this;
			}
		};
		struct ScenePassData
		{
			int32 SelectorID;
			String Name;
			int32 CameraID;
			std::vector<SceneInstruction> Instructions;

			ScenePassData()
			{

			}
			ScenePassData(const ScenePassData& other)
				: SelectorID(other.SelectorID), Name(other.Name),
				CameraID(other.CameraID), Instructions(other.Instructions)
			{

			}
			ScenePassData& operator =(const ScenePassData& other)
			{
				SelectorID = other.SelectorID;
				Name = other.Name;

				CameraID = other.CameraID;
				Instructions = other.Instructions;
				return *this;
			}
		};
	}
}
#endif