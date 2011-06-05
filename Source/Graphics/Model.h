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
#ifndef MODEL_H
#define MODEL_H

#include "Common.h"
#include "Core/Resource.h"
#include "Renderable.h"
#include "RenderOperationBuffer.h"
#include "Collections/FastList.h"
#include "ModelTypes.h"
#include "Animation/AnimationTypes.h"

using namespace Apoc3D::Collections;
using namespace Apoc3D::Core;
using namespace Apoc3D::Graphics;
using namespace Apoc3D::Graphics::Animation;
using namespace Apoc3D::Graphics::RenderSystem;
using namespace Apoc3D::VFS;

namespace Apoc3D
{
	namespace Graphics
	{
		class APAPI ModelSharedData : public Resource
		{
		private:
			RenderDevice* m_renderDevice;

			FastList<Mesh*> m_entities;
			
			ResourceLocation* m_resourceLocation;

		public:
			FastList<Mesh*>& getEntities() { return m_entities; }

			ModelSharedData(RenderDevice* device, ResourceLocation* rl);
			virtual ~ModelSharedData();

			virtual uint32 GetSize();
		protected:
			virtual void load();
			virtual void unload();
		};


		enum AnimationType
		{
			ANIMTYPE_Root,
			ANIMTYPE_Rigid,
			ANIMTYPE_Skinned,
			ANIMTYPE_Material
		};

		typedef fastdelegate::FastDelegate1<AnimationType, void> ModelAnimationCompletedHandler;

		class APAPI Model : public Renderable
		{
		private:
			enum AnimationControl
			{
				AC_Play,
				AC_Stop,
				AC_Resume,
				AC_Pause
			};

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
				if (!m_eventAnimCompleted.empty())
					m_eventAnimCompleted(ANIMTYPE_Root);
			}
			void RigidAnim_Competed()
			{
				m_rigidAnimCompleted = true;
				if (!m_eventAnimCompleted.empty())
					m_eventAnimCompleted(ANIMTYPE_Root);
			}
			void SkinAnim_Completed()
			{
				m_skinAnimCompleted = true;
				if (!m_eventAnimCompleted.empty())
					m_eventAnimCompleted(ANIMTYPE_Root);
			}
			void MtrlAnim_Completed()
			{
				m_mtrlAnimCompleted = true;
				if (!m_eventAnimCompleted.empty())
					m_eventAnimCompleted(ANIMTYPE_Root);
			}
			void InitializeAnimation();
			void UpdateAnimation();
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

			ModelSharedData* GetData();

			Model(ResourceHandle<ModelSharedData>* data, const AnimationData* animData = 0);
			~Model(void);

			void ReloadMaterialAnimation();			

			virtual const RenderOperationBuffer* GetRenderOperation();


			void Update(const GameTime* const time);

		};
	}
}
#endif