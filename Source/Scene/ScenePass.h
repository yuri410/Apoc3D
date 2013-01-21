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
#ifndef APOC3D_SCENEPASS_H
#define APOC3D_SCENEPASS_H

#include "Common.h"
#include "ScenePassTypes.h"
#include "Collections/Stack.h"

using namespace Apoc3D::Collections;

namespace Apoc3D
{
	namespace Scene
	{
		/**
		 *  Define a sequence of operation for a rendering result.
		 *
		 *  A scene pass is a scene rendering operation that usually 
		 *  renders objects to a RenderTarget (or the like) which can 
		 *  be used for further scene rendering passes.
		 * 
		 *  As a a part of render script, the ScenePass controls 
		 *  a pass of scene rendering. It selects objects for rendering
		 *  by check the objects' Material::PassFlag against the selectorID as
		 *  declared in the script.
		 *
		 */
		class APAPI ScenePass
		{
		public:

			ScenePass(RenderDevice* dev, SceneRenderer* renderer, SceneProcedure* parent, const ScenePassData* passData);
			~ScenePass(void);

			/**
			 *  Begins executing the pass' procedure for once.
			 */
			void Invoke(const FastList<Camera*>& cameras, SceneManager* sceMgr, BatchData* batchData);
			
			/** 
			 *  Gets the camera used in this scene pass.
			 */
			const Camera* getCurrentCamera() const { return m_currentCamera; }

			/** 
			 *  Gets the sequence of this pass in a entire scene rendering process.
			 */
			int32 getSelectorID() const { return m_selectorID; }

			/**
			 *  Gets the name of this pass.
			 */
			String getName() const { return m_name; }

		private:
			struct ExecutionValue
			{
				uint Value[2];
			};
		private:
			RenderDevice* m_renderDevice;
			SceneRenderer* m_renderer;
			SceneProcedure* m_parentProc;
			int32 m_selectorID;
			String m_name;
			int32 m_cameraID;

			
			VertexBuffer* m_quadBuffer;
			VertexDeclaration* m_quadVtxDecl;

			List<SceneInstruction> m_instuctions;
			Stack<ExecutionValue> m_execStack;

			float m_floatBuffer[60];

			Camera* m_currentCamera;

			void Clear(const SceneInstruction& inst);
			void RenderQuad(const SceneInstruction& inst);
			void UseRT(const SceneInstruction& inst);

		};
	};
};
#endif