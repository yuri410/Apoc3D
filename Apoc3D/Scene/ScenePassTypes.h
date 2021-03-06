#pragma once
#ifndef APOC3D_SCENEPASSTYPES_H
#define APOC3D_SCENEPASSTYPES_H

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

#include "apoc3d/ApocCommon.h"

#include "apoc3d/Collections/List.h"

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
		/**
		 * Currently the type system used inside scene script is hard coded. All types are check and 
		 * interpreted by fixed logic inside the engine. 
		 * In the future, a object-oriented type system may be implemented. Custom types,
		 * object handling may be easier to use/test. And the code will be more readable and less redundant.
		 */

		/** Defines the type of variable used in scene script */
		enum struct SceneVariableType
		{
			/**
			 ** Represents a render target.
			 *
			 *  Properties:
			 *  These are parameters required to declare the variable.
			 *   Width, Height
			 *   WidthP, HeightP
			 *   Format
			 *   SampleCount
			 *
			 *  Accessible Properties:
			 *  These properties can be accessed in code
			 *   Width
			 *   Height
			 */
			RenderTarget,
			/**
			 ** Represents a depth stencil buffer.
			 *
			 *  Properties:
			 *  These are parameters required to declare the variable.
			 *   Width, Height
			 *   WidthP, HeightP
			 *   Format
			 *   SampleCount
			 *
			 *  Accessible Properties:
			 *  These properties can be accessed in code
			 *   Width
			 *   Height
			 */
			DepthStencil,

			/**
			 *  A 4x4 float matrix 
			 *  Properties:
			 *  
			 */
			Matrix,
			Vector4,
			Vector3,
			Vector2,
			Single,

			Texture,
			
			Integer,
			Boolean,
			Effect,
			GaussBlurFilter
		};

		extern const Apoc3D::Utility::TypeDualConverter<SceneVariableType> SceneVariableTypeConverter;

		/** The information for the global variables used in a scene script */
		struct APAPI SceneVariable
		{
			String Name;
			SceneVariableType Type = SceneVariableType::Integer;
			uint Value[16];
			String DefaultStringValue;

			RenderTarget* RTValue = nullptr;
			DepthStencilBuffer* DSValue = nullptr;
			ResourceHandle<Texture>* TextureValue = nullptr;
			Effect* EffectValue = nullptr;

			void* ObjectValue = nullptr;

			SceneVariable()
			{
				memset(Value, 0, sizeof(Value));
			}
		};

		/** Defines some opcodes */
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
			SOP_UseDS,
			SOP_UseRT,
			SOP_VisibleTo,
			SOP_Render,
			SOP_RenderQuad
		};

		/**
		 *  Used to indicate the type of DrawQuad command's additional parameters, which is used 
		 *  to set up PostEffect's parameters.
		 */
		enum ScenePostEffectParamType
		{
			SPFX_TYPE_FLOATS,
			SPFX_TYPE_INTS,
			SPFX_TYPE_BOOLS,
			SPFX_TYPE_TEXTURE,
			SPFX_TYPE_VECTOR
		};

		/** The information for operands in op code in scene rendering */
		struct APAPI SceneOpArg
		{
			/**
			 *  Specifies if the argument is immediate or not.
			 *  An immediate is accessed directly on its value in DefaultValue, whereas
			 *  the value of others is from the referenced variable.
			 */
			bool IsImmediate;
			uint DefaultValue[16];

			/**
			 *  This StrData is used to store the name of the object's property to access. 
			 *  in the future, when type system is built up, this will be removed.
			 */
			String StrData;
			SceneVariable* Var;
		};

		/** The information for the minimum operation in the scene rendering procedure */
		struct APAPI SceneInstruction
		{
			SceneOpCode Operation;
			int Next;
			List<SceneOpArg> Args;

			SceneInstruction() { }

			SceneInstruction(SceneOpCode code)
				: Operation(code) { }
		};

		/**
		 *  Includes the data for a ScenePass. 
		 *  Object SelectorID, Name, a list of Instructions and CameraID make up this.
		 */
		struct APAPI ScenePassData
		{
			int32 SelectorID;
			String Name;
			int32 CameraID;
			List<SceneInstruction> Instructions;
		};
	}
}
#endif