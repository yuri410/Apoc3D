#pragma once
#ifndef APOC3D_SCENEPASS_H
#define APOC3D_SCENEPASS_H

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
#include "ScenePassTypes.h"

#include "apoc3d/Collections/Stack.h"

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
			~ScenePass();

			/** Begins executing the pass' procedure for once. */
			void Invoke(const List<Camera*>& cameras, SceneManager* sceMgr, BatchData* batchData);
			
			/** Gets the camera used in this scene pass. */
			const Camera* getCurrentCamera() const { return m_currentCamera; }

			/** 
			 *  Gets the object select ID of this pass during its rendering process.
			 */
			int32 getSelectorID() const { return m_selectorID; }
			void setSelectorID(int32 id) { m_selectorID = id; }

			/**
			 *  The scene pass will use the camera at the given index(ID) 
			 *  registered at SceneRenderer next rendering.
			 */
			int32 getPreferredCameraID() const { return m_cameraID; }
			void setPreferredCameraID(int32 id) { m_cameraID = id; }

			/** Gets the name of this pass. */
			const String& getName() const { return m_name; }

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
			void UseDS(const SceneInstruction& inst);

		};
	};
};
#endif