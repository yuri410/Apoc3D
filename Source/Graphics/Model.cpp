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
#include "Model.h"
#include "Animation/AnimationData.h"
#include "Animation/AnimationPlayers.h"
#include "Core/ResourceHandle.h"
#include "Vfs/ResourceLocation.h"

namespace Apoc3D
{
	namespace Graphics
	{
		/************************************************************************/
		/*                                                                      */
		/************************************************************************/



		ModelSharedData::ModelSharedData(RenderDevice* device, ResourceLocation* rl)
			: m_renderDevice(device), m_resourceLocation(rl)
		{

		}
		ModelSharedData::~ModelSharedData()
		{
			Resource::~Resource();
			delete m_resourceLocation;
		}
		/************************************************************************/
		/*                                                                      */
		/************************************************************************/

		Model::Model(ResourceHandle<ModelSharedData>* data, const AnimationData* animData)
			: m_animData(animData), m_selectedClipName(L"Take 001"),
			m_mtrlAnimCompleted(false), m_skinAnimCompleted(false), m_rootAnimCompleted(false), m_rigidAnimCompleted(false),
			m_mtrlPlayer(0), m_skinPlayer(0), m_rigidPlayer(0), m_rootPlayer(0),
			m_autoLoop(false), m_isOpBufferBuilt(false)
		{
		}


		Model::~Model(void)
		{
			if (m_mtrlPlayer)
				delete m_mtrlPlayer;
			if (m_skinPlayer)
				delete m_skinPlayer;
			if (m_rootPlayer)
				delete m_rootPlayer;
			if (m_rigidPlayer)
				delete m_rigidPlayer;
			delete m_data;
			//delete opBuffer;
		}

		void Model::ControlRootAnimation(AnimationControl ctrl)
		{
			if (!m_rootPlayer)
				return;
			if (!m_animData)
				return;

			if (m_animData->hasRootClip())
			{
				const AnimationData::ClipTable& table = m_animData->getRootAnimationClips();
				AnimationData::ClipTable::const_iterator iter = table.find(m_selectedClipName);
				if (iter != table.end())
				{
					const ModelAnimationClip* clip = iter->second;

					switch (ctrl)
					{
					case AC_Play:
						m_rootPlayer->StartClip(clip, 1, 0);
						break;
					case AC_Pause:
						m_rootPlayer->PauseClip();
						break;
					case AC_Stop:
						m_rootPlayer->PauseClip();
						m_rootPlayer->setCurrentKeyframe(clip->getKeyframes().getCount()>10 ? 10 : 0);
						break;
					case AC_Resume:
						m_rootPlayer->ResumeClip();
						break;
					}
				}
			}
		}
		void Model::ControlSkinnedAnimation(AnimationControl ctrl)
		{
			if (!m_skinPlayer)
				return;
			if (!m_animData)
				return;


			if (m_animData->hasModelClip())
			{
				const AnimationData::ClipTable& table = m_animData->getModelAnimationClips();
				AnimationData::ClipTable::const_iterator iter = table.find(m_selectedClipName);
				if (iter != table.end())
				{
					const ModelAnimationClip* clip = iter->second;
					switch (ctrl)
					{
					case AC_Play:
						m_skinPlayer->StartClip(clip, 1, 0);
						break;
					case AC_Pause:
						m_skinPlayer->PauseClip();
						break;
					case AC_Stop:
						m_skinPlayer->PauseClip();
						m_skinPlayer->setCurrentKeyframe(clip->getKeyframes().getCount()>10 ? 10 : 0);
						break;
					case AC_Resume:
						m_skinPlayer->ResumeClip();
						break;
					}
				}
			}
		}
		void Model::ControlRigidAnimation(AnimationControl ctrl)
		{
			if (!m_rigidPlayer)
				return;
			if (!m_animData)
				return;

			if (m_animData->hasModelClip())
			{
				const AnimationData::ClipTable& table = m_animData->getModelAnimationClips();
				AnimationData::ClipTable::const_iterator iter = table.find(m_selectedClipName);
				if (iter != table.end())
				{
					const ModelAnimationClip* clip = iter->second;
					switch (ctrl)
					{
					case AC_Play:
						m_rigidPlayer->StartClip(clip, 1, 0);
						break;
					case AC_Pause:
						m_rigidPlayer->PauseClip();
						break;
					case AC_Stop:
						m_rigidPlayer->PauseClip();
						m_rigidPlayer->setCurrentKeyframe(0);
						break;
					case AC_Resume:
						m_rigidPlayer->ResumeClip();
						break;
					}
				}
			}
		}
		void Model::ControlMaterialAnimation(AnimationControl ctrl)
		{
			if (!m_mtrlPlayer)
				return;
			
			if (!m_animData)
				return;

			if (m_animData->hasMtrlClip())
			{
				const AnimationData::MtrlClipTable& table = m_animData->getMaterialAnimationClips();
				AnimationData::MtrlClipTable::const_iterator iter = table.find(m_selectedClipName);
				if (iter != table.end())
				{
					const MaterialAnimationClip* clip = iter->second;
					switch (ctrl)
					{
					case AC_Play:
						m_mtrlPlayer->StartClip(clip, 1, 0);
						break;
					case AC_Pause:
						m_mtrlPlayer->PauseClip();
						break;
					case AC_Stop:
						m_mtrlPlayer->PauseClip();
						m_mtrlPlayer->setCurrentKeyframe(0);
						break;
					case AC_Resume:
						m_mtrlPlayer->ResumeClip();
						break;
					}
				}
			}
		}

		float Model::GetSkinAnimationDuration() const
		{
			if (!m_animData)
				return 0;
			if (!m_skinPlayer)
				return 0;

			const AnimationData::ClipTable& table = m_animData->getModelAnimationClips();

			if (table.size())
			{
				AnimationData::ClipTable::const_iterator iter = table.find(m_selectedClipName);
				if (iter != table.end())
				{
					const ModelAnimationClip* clip = iter->second;
					return clip->getDuration();
				}
			}
			return 0;
		}

		float Model::GetAnimationDuration() const
		{
			if (m_skinPlayer)
			{
				return m_skinPlayer->getCurrentClip()->getDuration();
			}
			else if (m_rigidPlayer)
			{
				return m_rigidPlayer->getCurrentClip()->getDuration();
			}
			else if (m_rootPlayer)
			{
				return m_rootPlayer->getCurrentClip()->getDuration();
			}
			else if (m_mtrlPlayer)
			{
				return m_mtrlPlayer->getCurrentClip()->Duration;
			}
			return 0;
		}

		ModelSharedData* Model::GetData()
		{
			m_data->TouchSync();
			return m_data->getWeakRef();
		}


		void Model::InitializeAnimation()
		{
			if (!m_animData)
				return;

			
			if (m_animData->hasRootClip())
			{
				const AnimationData::ClipTable& table = m_animData->getRootAnimationClips();
				
				if (table.size())
				{
					m_rootPlayer = new RootAnimationPlayer();
					
					m_animInstance.Add(m_rootPlayer);

					m_rootPlayer->eventCompleted().bind(this, &Model::RootAnim_Completed);
				}
			}
			if (m_animData->hasModelClip())
			{
				const AnimationData::ClipTable& table = m_animData->getModelAnimationClips();

				if (table.size())
				{
					if (m_animData->hasBindPose())
					{
						m_skinPlayer = new SkinnedAnimationPlayer(
							&m_animData->getBindPose(), &m_animData->getInvBindPose(), &m_animData->getSkeletonHierarchy());

						m_animInstance.Add(m_skinPlayer);
						m_skinPlayer->eventCompleted().bind(this, &Model::SkinAnim_Completed);
					}
					else
					{
						m_rigidPlayer = new RigidAnimationPlayer(m_animData->getBones().getCount());

						m_animInstance.Add(m_rigidPlayer);

						m_rigidPlayer->eventCompleted().bind(this, &Model::RigidAnim_Competed);
					}
				}

			}
			if (m_animData->hasMtrlClip())
			{
				const AnimationData::MtrlClipTable& table = m_animData->getMaterialAnimationClips();
				if (table.size())
				{
					m_mtrlPlayer = new MaterialAnimationPlayer();
					
					m_animInstance.Add(m_rigidPlayer);

					m_mtrlPlayer->eventCompleted().bind(this, &Model::MtrlAnim_Completed);
				}
			}

		}
		void Model::ReloadMaterialAnimation()
		{
			if (m_mtrlPlayer)				
			{
				m_mtrlPlayer->eventCompleted().clear();
				delete m_mtrlPlayer;
				m_mtrlPlayer = 0;
			}

			if (m_animData->hasMtrlClip())
			{
				const AnimationData::MtrlClipTable& table = m_animData->getMaterialAnimationClips();
				if (table.size())
				{
					m_mtrlPlayer = new MaterialAnimationPlayer();
					m_mtrlPlayer->eventCompleted().bind(this, &Model::MtrlAnim_Completed);
				}
			}
		}
		void Model::UpdateAnimation()
		{
			if (m_rootAnimCompleted)
			{
				if (m_autoLoop)
				{
					ControlRootAnimation(AC_Play);
				}
				m_rootAnimCompleted = false;
			}
			if (m_rigidAnimCompleted)
			{
				if (m_autoLoop)
				{
					ControlRigidAnimation(AC_Play);
				}
				m_rigidAnimCompleted = false;
			}
			if (m_skinAnimCompleted)
			{
				if (m_autoLoop)
				{
					ControlSkinnedAnimation(AC_Play);
				}
				m_skinAnimCompleted = false;
			}
			if (m_mtrlAnimCompleted) 
            {
				if (m_autoLoop)
				{
					ControlMaterialAnimation(AC_Play);
				}
				m_mtrlAnimCompleted = false;
			}
		}

		const RenderOperationBuffer* Model::GetRenderOperation()
		{
			if (m_data->getState() != RS_Loaded && m_data->getWeakRef()->isManaged())
			{
				m_data->Touch();
				return 0;
			}

			UpdateAnimation();

			ResourceHandle<ModelSharedData>& data = *m_data;

			FastList<Mesh*> entities = data->getEntities();
			if (!m_isOpBufferBuilt)
			{

			}
			else
			{

			}
			return &m_opBuffer;
		}

		void Model::Update(const GameTime* const time)
		{
			for (int i=0; i<m_animInstance.getCount(); i++)
			{
				m_animInstance[i]->Update(time);
			}
		}
	}
}
