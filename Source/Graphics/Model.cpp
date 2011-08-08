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

#include "Mesh.h"
#include "Animation/AnimationData.h"
#include "Animation/AnimationPlayers.h"
#include "Core/ResourceHandle.h"
#include "Vfs/ResourceLocation.h"
#include "ModelManager.h"
#include "IOLib/ModelData.h"

//template class Apoc3D::Core::ResourceHandle<Apoc3D::Graphics::ModelSharedData>;

namespace Apoc3D
{
	namespace Graphics
	{
		/************************************************************************/
		/*                                                                      */
		/************************************************************************/

		ModelSharedData::ModelSharedData(RenderDevice* device, ResourceLocation* rl)
			: Resource(ModelManager::getSingletonPtr(), rl->getName()), m_renderDevice(device), m_resourceLocation(rl)
		{

		}
		ModelSharedData::~ModelSharedData()
		{
			delete m_resourceLocation;
		}

		void ModelSharedData::load()
		{
			if (m_resourceLocation)
			{
				ModelData data;
				data.Load(m_resourceLocation);

				for (int i=0; i< data.Entities.getCount();i++)
				{
					Mesh* mesh = new Mesh(m_renderDevice, data.Entities[i]);
					m_entities.Add(mesh);
				}
			}
		}
		void ModelSharedData::unload()
		{
			for (int i=0; i< m_entities.getCount();i++)
			{
				delete m_entities[i];
			}
			m_entities.Clear();
		}

		uint ModelSharedData::getSize()
		{
			if (m_resourceLocation)
			{
				return static_cast<uint32>(m_resourceLocation->getSize());
			}
			return 0;
		}
		/************************************************************************/
		/*                                                                      */
		/************************************************************************/

		Model::Model(ResourceHandle<ModelSharedData>* data, const AnimationData* animData)
			: m_animData(animData), m_selectedClipName(L"Take 001"),
			m_mtrlAnimCompleted(false), m_skinAnimCompleted(false), m_rootAnimCompleted(false), m_rigidAnimCompleted(false),
			m_mtrlPlayer(0), m_skinPlayer(0), m_rigidPlayer(0), m_rootPlayer(0),
			m_autoLoop(false), m_isOpBufferBuilt(false),
			m_renderOpEntID(0),m_renderOpEntPartID(0)
		{
		}


		Model::~Model(void)
		{
			for (int i=0;i<m_animInstance.getCount();i++)
			{
				delete m_animInstance[i];
			}
			if (m_mtrlPlayer)
				delete m_mtrlPlayer;
			//if (m_skinPlayer)
			//	delete m_skinPlayer;
			//if (m_rootPlayer)
			//	delete m_rootPlayer;
			//if (m_rigidPlayer)
			//	delete m_rigidPlayer;
			delete m_data;
			//delete opBuffer;
			if (m_renderOpEntPartID)
				delete[] m_renderOpEntPartID;
			if (m_renderOpEntID)
				delete[] m_renderOpEntID;
		}

		void Model::ControlRootAnimation(AnimationControl ctrl)
		{
			//if (!m_rootPlayer)
			//	return;
			//if (!m_animData)
			//	return;

			//if (m_animData->hasRigidClip())
			//{
			//	const AnimationData::ClipTable& table = m_animData->getRigidAnimationClips();
			//	AnimationData::ClipTable::const_iterator iter = table.find(m_selectedClipName);
			//	if (iter != table.end())
			//	{
			//		const ModelAnimationClip* clip = iter->second;

			//		switch (ctrl)
			//		{
			//		case AC_Play:
			//			m_rootPlayer->StartClip(clip, 1, 0);
			//			break;
			//		case AC_Pause:
			//			m_rootPlayer->PauseClip();
			//			break;
			//		case AC_Stop:
			//			m_rootPlayer->PauseClip();
			//			m_rootPlayer->setCurrentKeyframe(clip->getKeyframes().getCount()>10 ? 10 : 0);
			//			break;
			//		case AC_Resume:
			//			m_rootPlayer->ResumeClip();
			//			break;
			//		}
			//	}
			//}
		}
		void Model::ControlSkinnedAnimation(AnimationControl ctrl)
		{
			if (!m_skinPlayer)
				return;
			if (!m_animData)
				return;


			if (m_animData->hasSkinnedClip())
			{
				const AnimationData::ClipTable& table = m_animData->getSkinnedAnimationClips();
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

			if (m_animData->hasRigidClip())
			{
				const AnimationData::ClipTable& table = m_animData->getSkinnedAnimationClips();
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

			const AnimationData::ClipTable& table = m_animData->getSkinnedAnimationClips();

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

			
			if (m_animData->hasRigidClip())
			{
				const AnimationData::ClipTable& table = m_animData->getRigidAnimationClips();
				
				if (table.size())
				{
					//m_rootPlayer = new RootAnimationPlayer();
					//
					//m_animInstance.Add(m_rootPlayer);

					//m_rootPlayer->eventCompleted().bind(this, &Model::RootAnim_Completed);

					m_rigidPlayer = new RigidAnimationPlayer(m_animData->getBones().getCount());

					m_animInstance.Add(m_rigidPlayer);

					m_rigidPlayer->eventCompleted().bind(this, &Model::RigidAnim_Competed);
				}
			}
			if (m_animData->hasSkinnedClip())
			{
				const AnimationData::ClipTable& table = m_animData->getSkinnedAnimationClips();

				if (table.size())
				{
					const List<Bone>* bones = &m_animData->getBones();
					
					assert(bones->getCount());
					m_skinPlayer = new SkinnedAnimationPlayer(bones);

					m_animInstance.Add(m_skinPlayer);
					m_skinPlayer->eventCompleted().bind(this, &Model::SkinAnim_Completed);
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

		RenderOperationBuffer* Model::GetRenderOperation()
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
				RenderOperationBuffer** entOps = new RenderOperationBuffer*[entities.getCount()];

				int opCount = 0;
				for (int i = 0; i < entities.getCount(); i++)
				{
					entOps[i] = entities[i]->GetRenderOperation(0);

					opCount += entOps[i]->getCount();
				}

				int dstIdx = 0;
				//gmBuffer = new GeomentryData[opCount];
				m_opBuffer.ReserveDiscard(opCount);				
				m_renderOpEntID = new int[opCount];
				m_renderOpEntPartID = new int[opCount];

				for (int i = 0; i < entities.getCount(); i++)
				{
					//memcpy();
					//Array.Copy(entOps[i], 0, opBuffer, dstIdx, entOps[i].Length);
					//entOps[i] = 
					m_opBuffer.Add(&entOps[i]->operator[](0), entOps[i]->getCount());
					for (int j = 0; j < entOps[i]->getCount(); j++)
					{
						int opid = dstIdx + j;
						m_renderOpEntID[opid] = i;
						m_renderOpEntPartID[opid] = j;


						if (m_skinPlayer)
						{
							m_opBuffer[opid].BoneTransform.Transfroms = m_skinPlayer->GetSkinTransforms();
							m_opBuffer[opid].BoneTransform.Count = m_skinPlayer->getTransformCount();
						}
						if (m_animInstance.getCount())
						{
							m_opBuffer[opid].RootTransform = Matrix::Identity;

							for (int k = 0; k < m_animInstance.getCount(); k++)
							{
								int entId = i;

								//int boneId = entities[entId].ParentBoneID;
								Matrix temp;
								m_animInstance[k]->GetTransform(entId, temp);
								Matrix::Multiply(m_opBuffer[opid].RootTransform, m_opBuffer[opid].RootTransform, temp);
								//opBuffer[opid].Transformation *= animInstance[k].GetTransform(boneId);

							}
						}
						else
						{
							m_opBuffer[opid].RootTransform = Matrix::Identity;
						}
					}

					dstIdx += entOps[i]->getCount();
				}
				m_isOpBufferBuilt = true;
			}
			else
			{
				for (int i = 0; i < m_opBuffer.getCount(); i++)
				{
					RenderOperation& rop = m_opBuffer[i];
					if (m_mtrlPlayer) 
					{
						int partId = m_renderOpEntPartID[i];
						int entId = m_renderOpEntID[i];
						int frame = m_mtrlPlayer->getCurrentMaterialFrame();
						if (frame >= (int32)entities[entId]->getMaterials()->getFrameCount(partId)) 
						{
							frame = entities[entId]->getMaterials()->getFrameCount(partId)-1;//.Materials[partId].Length - 1;
						}
						rop.Material = entities[entId]->getMaterials()->getMaterial(partId, frame);
					}

					if (m_animInstance.getCount())
					{
						rop.RootTransform = Matrix::Identity;

						for (int k = 0; k < m_animInstance.getCount(); k++)
						{
							int entId = m_renderOpEntID[i];

							
							Matrix temp;
							m_animInstance[k]->GetTransform(entId, temp);
							Matrix::Multiply(rop.RootTransform, rop.RootTransform, temp);
						}
					}
					else
					{
						rop.RootTransform = Matrix::Identity;
					}
					
					if (m_skinPlayer)
					{
						rop.BoneTransform.Transfroms = m_skinPlayer->GetSkinTransforms();
						rop.BoneTransform.Count = m_skinPlayer->getTransformCount();
					}
					
				}

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
