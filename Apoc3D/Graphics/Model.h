#pragma once
#ifndef APOC3D_MODEL_H
#define APOC3D_MODEL_H

/**
 * -----------------------------------------------------------------------------
 * This source file is part of Apoc3D Engine
 * 
 * Copyright (c) 2009+ Tao Xin
 * 
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 * 
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 * 
 * You should have received a copy of the GNU General Public License
 * along with this program.  if not, write to the Free Software Foundation, 
 * Inc., 59 Temple Place - Suite 330, Boston, MA 02111-1307, USA, or go to
 * http://www.gnu.org/copyleft/gpl.txt.
 * 
 * -----------------------------------------------------------------------------
 */


#include "Renderable.h"
#include "RenderOperationBuffer.h"
#include "ModelTypes.h"
#include "Animation/AnimationTypes.h"
#include "../EventDelegate.h"

#include "apoc3d/Core/Resource.h"

using namespace Apoc3D::Collections;
using namespace Apoc3D::Core;
using namespace Apoc3D::Graphics;
using namespace Apoc3D::Graphics::Animation;
using namespace Apoc3D::Graphics::RenderSystem;
using namespace Apoc3D::VFS;
using namespace Apoc3D::IO;

namespace Apoc3D
{
	namespace Graphics
	{
		/** 
		 *  Model's geometry part, which is considered as resources of the vertex and index data.
         *  
		 *  Internally, models can share the same ModelSharedData, if the geometry is from the same source(i.e. the same model .mesh file).
		 *  At the same time, they can use different animation files. 
		 *
		 *  Model's data, ModelSharedData, consist of meshes and resource management operations.
		 *  
		 */
		class APAPI ModelSharedData : public Resource
		{
		public:
			FastList<Mesh*>& getEntities() { return m_entities; }

			ModelSharedData(RenderDevice* device, const FastList<Mesh*>& entities);
			ModelSharedData(RenderDevice* device, const ModelData* mdlData);
			ModelSharedData(RenderDevice* device, ResourceLocation* rl, bool managed = true);
			virtual ~ModelSharedData();

			virtual uint32 getSize();

			void Save(ModelData* data);
		protected:
			virtual void load();
			virtual void unload();


		private:
			RenderDevice* m_renderDevice;

			FastList<Mesh*> m_entities;

			ResourceLocation* m_resourceLocation;

		};


		enum AnimationType
		{
			ANIMTYPE_Root,
			ANIMTYPE_Rigid,
			ANIMTYPE_Skinned,
			ANIMTYPE_Material
		};

		typedef EventDelegate1<AnimationType> ModelAnimationCompletedHandler;

		/** A model is a set of subsets called Mesh, with additionally animation controller and data.
		 *
		 *  When loading model, ModelManager is recommended, the model manager will create ModelSharedData, which is then
		 *  used as Model's constructor's parameter.
		 */
		class APAPI Model : public Renderable
		{
		public:
			ModelAnimationCompletedHandler& eventAnimationCompeleted() { return m_eventAnimCompleted; }

			FastList<ModelAnimationPlayerBase*>& getCustomAnimation() { return m_animInstance; }
			
			float GetSkinAnimationDuration() const;
			float GetAnimationDuration() const;

			void PlayAnimation()
			{
				ControlRootAnimation(AC_Play);
				ControlSkinnedAnimation(AC_Play);
				ControlRigidAnimation(AC_Play);
				ControlMaterialAnimation(AC_Play);
			}
			void PauseAnimation()
			{
				ControlRootAnimation(AC_Pause);
				ControlSkinnedAnimation(AC_Pause);
				ControlRigidAnimation(AC_Pause);
				ControlMaterialAnimation(AC_Pause);
			}
			void ResumeAnimation()
			{
				ControlRootAnimation(AC_Resume);
				ControlSkinnedAnimation(AC_Resume);
				ControlRigidAnimation(AC_Resume);
				ControlMaterialAnimation(AC_Resume);
			}
			void StopAnimation()
			{
				ControlRootAnimation(AC_Stop);
				ControlSkinnedAnimation(AC_Stop);
				ControlRigidAnimation(AC_Stop);
				ControlMaterialAnimation(AC_Stop);
			}

			/** Gets the ModelSharedData using. 
			 *  The caller's thread will be suspended if ModelSharedData is loaded.
			 *
			 */
			ModelSharedData* GetData();

			/** Initializes a new model from ModelSharedData and an optional AnimationData.
			 *
			 *  The AnimationData need to be deleted manually when no longer used, as the Model class will not modify it.
			 */
			Model(ResourceHandle<ModelSharedData>* data, const AnimationData* animData = 0);
			~Model(void);

			void ReloadMaterialAnimation();			
			void RebuildROPCache() { m_isOpBufferBuilt = false; m_opBuffer.Clear(); }

			virtual RenderOperationBuffer* GetRenderOperation(int lod);
			virtual RenderOperationBuffer* GetRenderOperationSubEntity(int index);

			/** The update will do the animation works if the model has animation.
			*/
			void Update(const GameTime* const time);
			
		private:
			enum AnimationControl
			{
				AC_Play,
				AC_Stop,
				AC_Resume,
				AC_Pause
			};

			/** Whether the RenderOperationBuffer is pre-calculated and stored. 
			 *  The RenderOperations inside are build first time the model is drawn for performance considerations.
			 *  This will not affect animation; because even though the RenderOperations are pre-calculated, the 
			 *  matrices are re-calculated based on animation each frame.
			 */
			bool m_isOpBufferBuilt;
			RenderOperationBuffer m_opBuffer;

			/** table cast render operation index to entity index
			*/
			int* m_renderOpEntID;
			/** table cast render operation index to entity part index
			*/
			int* m_renderOpEntPartID;

			/** states used for looping check. does not go public
			*/
			bool m_rigidAnimCompleted;
			bool m_rootAnimCompleted;
			bool m_skinAnimCompleted;
			bool m_mtrlAnimCompleted;

			ResourceHandle<ModelSharedData>* m_data;
			const AnimationData* m_animData;

			SkinnedAnimationPlayer* m_skinPlayer;
			RootAnimationPlayer* m_rootPlayer;
			RigidAnimationPlayer* m_rigidPlayer;
			MaterialAnimationPlayer* m_mtrlPlayer;

			FastList<ModelAnimationPlayerBase*> m_animInstance;

			bool m_autoLoop;
			String m_selectedClipName;

			ModelAnimationCompletedHandler m_eventAnimCompleted;



			void ControlRootAnimation(AnimationControl ctrl);
			void ControlSkinnedAnimation(AnimationControl ctrl);
			void ControlRigidAnimation(AnimationControl ctrl);
			void ControlMaterialAnimation(AnimationControl ctrl);

			void RootAnim_Completed()
			{
				m_rootAnimCompleted = true;
				m_eventAnimCompleted.Invoke(ANIMTYPE_Root);
			}
			void RigidAnim_Competed()
			{
				m_rigidAnimCompleted = true;
				m_eventAnimCompleted.Invoke(ANIMTYPE_Root);
			}
			void SkinAnim_Completed()
			{
				m_skinAnimCompleted = true;
				m_eventAnimCompleted.Invoke(ANIMTYPE_Root);
			}
			void MtrlAnim_Completed()
			{
				m_mtrlAnimCompleted = true;
				m_eventAnimCompleted.Invoke(ANIMTYPE_Root);
			}
			void InitializeAnimation();
			void UpdateAnimation();
		};
	}
}
#endif