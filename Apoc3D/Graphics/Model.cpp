/*
-----------------------------------------------------------------------------
This source file is part of Apoc3D Engine

Copyright (c) 2009+ Tao Xin

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
#include "apoc3d/Core/ResourceHandle.h"
#include "apoc3d/IOLib/ModelData.h"
#include "apoc3d/Vfs/ResourceLocation.h"
#include "ModelManager.h"


//template class Apoc3D::Core::ResourceHandle<Apoc3D::Graphics::ModelSharedData>;

namespace Apoc3D
{
	namespace Graphics
	{
		/************************************************************************/
		/*   ModelSharedData                                                    */
		/************************************************************************/

		ModelSharedData::ModelSharedData(RenderDevice* device, const ModelData* mdlData)
			: Resource(), m_resourceLocation(0), m_renderDevice(device)
		{
			for (int i=0;i<mdlData->Entities.getCount();i++)
			{
				Mesh* mesh = new Mesh(m_renderDevice, mdlData->Entities[i]);
				m_entities.Add(mesh);
			}
		}
		ModelSharedData::ModelSharedData(RenderDevice* device, const ResourceLocation& rl, bool managed)
			: Resource(managed ? ModelManager::getSingletonPtr():0, rl.getName()), m_renderDevice(device), m_resourceLocation(rl.Clone())
		{
			if (!managed)
			{
				load();
			}
		}
		ModelSharedData::ModelSharedData(RenderDevice* device, const List<Mesh*>& entities)
			: Resource(), m_resourceLocation(0), m_renderDevice(device), m_entities(entities)
		{

		}
		ModelSharedData::~ModelSharedData()
		{
			if (m_resourceLocation)
				delete m_resourceLocation;
			for (int i=0; i< m_entities.getCount();i++)
			{
				delete m_entities[i];
			}
			m_entities.Clear();
		}

		void ModelSharedData::load()
		{
			if (m_resourceLocation)
			{
				ModelData data;
				data.Load(*m_resourceLocation);

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

		void ModelSharedData::Save(ModelData* data)
		{
			for (int i=0;i<m_entities.getCount();i++)
			{
				MeshData* meshData = new MeshData();
				m_entities[i]->Save(meshData);
				data->Entities.Add(meshData);
			}
		}

		/************************************************************************/
		/*  Model                                                               */
		/************************************************************************/

		Model::Model(ResourceHandle<ModelSharedData>* data, const AnimationData* animData)
			: m_data(data), m_animData(animData), m_selectedClipName(L"Take 001"),
			m_mtrlAnimCompleted(false), m_skinAnimCompleted(false), m_rigidAnimCompleted(false),
			m_mtrlPlayer(nullptr), m_skinPlayer(nullptr), m_rigidPlayer(nullptr),
			m_autoLoop(true), m_isOpBufferBuilt(false),
			m_renderOpEntID(nullptr),m_renderOpEntPartID(nullptr),
			m_skinState(APS_Stopped), m_rigidState(APS_Stopped), m_mtrlState(APS_Stopped)
		{
			InitializeAnimation();
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


		ModelSharedData* Model::GetData()
		{
			m_data->TouchSync();
			return m_data->getWeakRef();
		}

		void Model::UpdateAnimation()
		{
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

		RenderOperationBuffer* Model::GetRenderOperation(int lod)
		{
			if (m_data->getWeakRef()->isManaged() && m_data->getState() != RS_Loaded)
			{
				m_data->Touch();
				return 0;
			}

			UpdateAnimation();

			ResourceHandle<ModelSharedData>& data = *m_data;

			const List<Mesh*>& entities = data->getEntities();
			if (!m_isOpBufferBuilt)
			{
				BuildROPBuffer();
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

							Matrix result;
							Matrix::Multiply(result, rop.RootTransform, temp);
							rop.RootTransform = result;
						}
					}
					else
					{
						rop.RootTransform = Matrix::Identity;
					}

					if (m_skinPlayer)
					{
						rop.PartTransform.Transfroms = m_skinPlayer->GetSkinTransforms();
						rop.PartTransform.Count = m_skinPlayer->getTransformCount();
					}
				}

			}
			return &m_opBuffer;
		}
		RenderOperationBuffer* Model::GetRenderOperationSubEntity(int index)
		{
			if (m_data->getWeakRef()->isManaged() && m_data->getState() != RS_Loaded)
			{
				m_data->Touch();
				return 0;
			}

			UpdateAnimation();

			ResourceHandle<ModelSharedData>& data = *m_data;

			const List<Mesh*>& entities = data->getEntities();

			RenderOperationBuffer* opBuf = entities[index]->GetRenderOperation(0);
			m_opBuffer.Clear();
			m_opBuffer.Add(&opBuf->operator [](0), opBuf->getCount());

			for (int j = 0; j < opBuf->getCount(); j++)
			{
				int opid = j;

				if (m_skinPlayer)
				{
					m_opBuffer[opid].PartTransform.Transfroms = m_skinPlayer->GetSkinTransforms();
					m_opBuffer[opid].PartTransform.Count = m_skinPlayer->getTransformCount();
				}
				if (m_animInstance.getCount())
				{
					m_opBuffer[opid].RootTransform = Matrix::Identity;

					for (int k = 0; k < m_animInstance.getCount(); k++)
					{
						int entId = index;

						Matrix temp;
						m_animInstance[k]->GetTransform(entId, temp);

						Matrix result;
						Matrix::Multiply(result, m_opBuffer[opid].RootTransform, temp);
						m_opBuffer[opid].RootTransform = result;

					}
				}
				else
				{
					m_opBuffer[opid].RootTransform = Matrix::Identity;
				}
			}
			return &m_opBuffer;
		}

		
		RenderOperationBuffer* Model::GetRenderOperationSubEntitySimple(int index)
		{
			assert(!m_data->getWeakRef()->isManaged() || !m_data->getWeakRef()->getManager()->usesAsync());

			const List<Mesh*>& entities = m_data->getWeakRef()->getEntities();

			RenderOperationBuffer* opBuf = entities[index]->GetRenderOperation(0);
			m_opBuffer.Clear();
			m_opBuffer.AddBuffer(opBuf, Matrix::Identity);

			return &m_opBuffer;
		}
		RenderOperationBuffer* Model::GetRenderOperationSubEntityRaw(int index)
		{
			assert(!m_data->getWeakRef()->isManaged() || !m_data->getWeakRef()->getManager()->usesAsync());

			const List<Mesh*>& entities = m_data->getWeakRef()->getEntities();

			return entities[index]->GetRenderOperation(0);
		}

		void Model::BuildROPBuffer()
		{
			assert(!m_isOpBufferBuilt);
			assert(m_renderOpEntPartID == nullptr);
			assert(m_renderOpEntPartID == nullptr);

			ResourceHandle<ModelSharedData>& data = *m_data;

			const List<Mesh*>& entities = data->getEntities();

			RenderOperationBuffer** entOps = new RenderOperationBuffer*[entities.getCount()];

			int opCount = 0;
			for (int i = 0; i < entities.getCount(); i++)
			{
				entOps[i] = entities[i]->GetRenderOperation(0);

				opCount += entOps[i]->getCount();
			}

			int dstIdx = 0;
			m_opBuffer.ReserveDiscard(opCount);				
			m_renderOpEntID = new int[opCount];
			m_renderOpEntPartID = new int[opCount];

			for (int i = 0; i < entities.getCount(); i++)
			{
				RenderOperationBuffer* ops = entOps[i];
				m_opBuffer.AddBuffer(ops);

				for (int j = 0; j < ops->getCount(); j++)
				{
					int opid = dstIdx + j;
					m_renderOpEntID[opid] = i;
					m_renderOpEntPartID[opid] = j;

					RenderOperation& rop = m_opBuffer[opid];

					if (m_skinPlayer)
					{
						rop.PartTransform.Transfroms = m_skinPlayer->GetSkinTransforms();
						rop.PartTransform.Count = m_skinPlayer->getTransformCount();
					}
					if (m_animInstance.getCount())
					{
						rop.RootTransform = Matrix::Identity;

						for (int k = 0; k < m_animInstance.getCount(); k++)
						{
							int entId = i;

							Matrix temp;
							m_animInstance[k]->GetTransform(entId, temp);

							Matrix result;
							Matrix::Multiply(result, m_opBuffer[opid].RootTransform, temp);
							rop.RootTransform = result;
						}
					}
					else
					{
						rop.RootTransform = Matrix::Identity;
					}
				}

				dstIdx += ops->getCount();
			}
			delete[] entOps;
		}
		void Model::RebuildROPCache()
		{
			m_isOpBufferBuilt = false; 

			m_opBuffer.Clear();

			delete[] m_renderOpEntID;
			delete[] m_renderOpEntPartID;
			m_renderOpEntID = nullptr;
			m_renderOpEntPartID = nullptr;
		}

		void Model::Update(const GameTime* const time)
		{
			for (int i=0; i<m_animInstance.getCount(); i++)
			{
				m_animInstance[i]->Update(time);
			}
			if (m_mtrlPlayer)
				m_mtrlPlayer->Update(time);
		}


		void Model::InitializeAnimation()
		{
			if (!m_animData)
				return;

			ReloadRigidAnimation();
			ReloadSkinAnimation();
			ReloadMaterialAnimation();
		}

		void Model::PlayAnimation(AnimationType type)
		{
			if ((type & ANIMTYPE_Skinned) != 0) ControlSkinnedAnimation(AC_Play);
			if ((type & ANIMTYPE_Rigid) != 0) ControlRigidAnimation(AC_Play);
			if ((type & ANIMTYPE_Material) != 0) ControlMaterialAnimation(AC_Play);
		}
		void Model::PauseAnimation(AnimationType type)
		{
			if ((type & ANIMTYPE_Skinned) != 0) ControlSkinnedAnimation(AC_Pause);
			if ((type & ANIMTYPE_Rigid) != 0) ControlRigidAnimation(AC_Pause);
			if ((type & ANIMTYPE_Material) != 0) ControlMaterialAnimation(AC_Pause);
		}
		void Model::ResumeAnimation(AnimationType type)
		{
			if ((type & ANIMTYPE_Skinned) != 0) ControlSkinnedAnimation(AC_Resume);
			if ((type & ANIMTYPE_Rigid) != 0) ControlRigidAnimation(AC_Resume);
			if ((type & ANIMTYPE_Material) != 0) ControlMaterialAnimation(AC_Resume);
		}
		void Model::StopAnimation(AnimationType type)
		{
			if ((type & ANIMTYPE_Skinned) != 0) ControlSkinnedAnimation(AC_Stop);
			if ((type & ANIMTYPE_Rigid) != 0) ControlRigidAnimation(AC_Stop);
			if ((type & ANIMTYPE_Material) != 0) ControlMaterialAnimation(AC_Stop);
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
				//AnimationData::ClipTable::const_iterator iter = table.find(m_selectedClipName);
				//if (iter != table.end())
				ModelAnimationClip* clip;
				if (table.TryGetValue(m_selectedClipName, clip))
				{
					switch (ctrl)
					{
					case AC_Play:
						m_skinPlayer->StartClip(clip, 1, 0);
						m_skinState = APS_Playing;
						break;
					case AC_Pause:
						m_skinPlayer->PauseClip();
						m_skinState = APS_Paused;
						break;
					case AC_Stop:
						m_skinPlayer->PauseClip();
						m_skinPlayer->setCurrentKeyframe(clip->getKeyframes().getCount()>10 ? 10 : 0);
						m_skinState = APS_Stopped;
						break;
					case AC_Resume:
						m_skinPlayer->ResumeClip();
						m_skinState = APS_Playing;
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
				const AnimationData::ClipTable& table = m_animData->getRigidAnimationClips();
				ModelAnimationClip* clip;

				if (table.TryGetValue(m_selectedClipName, clip))
				{
					switch (ctrl)
					{
					case AC_Play:
						m_rigidPlayer->StartClip(clip, 1, 0);
						m_rigidState = APS_Playing;
						break;
					case AC_Pause:
						m_rigidPlayer->PauseClip();
						m_rigidState = APS_Paused;
						break;
					case AC_Stop:
						m_rigidPlayer->PauseClip();
						m_rigidPlayer->setCurrentKeyframe(0);
						m_rigidState = APS_Stopped;
						break;
					case AC_Resume:
						m_rigidPlayer->ResumeClip();
						m_rigidState = APS_Playing;
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
				MaterialAnimationClip* clip;

				if (table.TryGetValue(m_selectedClipName, clip))
				{
					switch (ctrl)
					{
					case AC_Play:
						m_mtrlPlayer->StartClip(clip, 1, 0);
						m_mtrlState = APS_Playing;
						break;
					case AC_Pause:
						m_mtrlPlayer->PauseClip();
						m_mtrlState = APS_Paused;
						break;
					case AC_Stop:
						m_mtrlPlayer->PauseClip();
						m_mtrlPlayer->setCurrentKeyframe(0);
						m_mtrlState = APS_Stopped;
						break;
					case AC_Resume:
						m_mtrlPlayer->ResumeClip();
						m_mtrlState = APS_Playing;
						break;
					}
				}
			}
		}

		
		void Model::ReloadAnimation(AnimationType type)
		{
			if ((type & ANIMTYPE_Rigid) != 0) ReloadSkinAnimation(); 
			if ((type & ANIMTYPE_Skinned) != 0) ReloadRigidAnimation();
			if ((type & ANIMTYPE_Material) != 0) ReloadMaterialAnimation();
		}
		void Model::ReloadMaterialAnimation()
		{
			if (m_mtrlPlayer)				
			{
				m_mtrlPlayer->eventCompleted().clear();
				delete m_mtrlPlayer;
				m_mtrlPlayer = nullptr;
			}

			if (m_animData->hasMtrlClip())
			{
				const AnimationData::MtrlClipTable& table = m_animData->getMaterialAnimationClips();
				if (table.getCount())
				{
					m_mtrlPlayer = new MaterialAnimationPlayer();
					m_mtrlPlayer->eventCompleted().bind(this, &Model::MtrlAnim_Completed);
				}
			}
			m_mtrlState = APS_Stopped;
		}
		void Model::ReloadSkinAnimation()
		{
			if (m_skinPlayer)
			{
				m_skinPlayer->eventCompleted().clear();
				delete m_skinPlayer;
				m_skinPlayer = nullptr;
			}

			if (m_animData->hasSkinnedClip())
			{
				const AnimationData::ClipTable& table = m_animData->getSkinnedAnimationClips();

				if (table.getCount())
				{
					const List<Bone>* bones = &m_animData->getBones();

					assert(bones->getCount());
					m_skinPlayer = new SkinnedAnimationPlayer(bones);

					m_animInstance.Add(m_skinPlayer);
					m_skinPlayer->eventCompleted().bind(this, &Model::SkinAnim_Completed);
				}
			}

			m_skinState = APS_Stopped;
		}
		void Model::ReloadRigidAnimation()
		{
			if (m_rigidPlayer)				
			{
				m_rigidPlayer->eventCompleted().clear();
				delete m_rigidPlayer;
				m_rigidPlayer = nullptr;
			}

			if (m_animData->hasRigidClip())
			{
				const AnimationData::ClipTable& table = m_animData->getRigidAnimationClips();

				if (table.getCount())
				{
					//m_rootPlayer = new RootAnimationPlayer();
					//
					//m_animInstance.Add(m_rootPlayer);

					//m_rootPlayer->eventCompleted().bind(this, &Model::RootAnim_Completed);

					m_rigidPlayer = new RigidAnimationPlayer(m_animData->RigidEntityCount);

					m_animInstance.Add(m_rigidPlayer);

					m_rigidPlayer->eventCompleted().bind(this, &Model::RigidAnim_Competed);
				}
			}
			m_rigidState = APS_Stopped;
		}


		void Model::SetSelectedClipName(const String& name)
		{
			if (m_selectedClipName != name)
			{
				InitializeAnimation();
			}
		}

		float Model::GetCurrentAnimationDuration(AnimationType type) const
		{
			if ((type & ANIMTYPE_Rigid) != 0)
			{
				if (m_rigidPlayer && m_rigidPlayer->getCurrentClip())
					return m_rigidPlayer->getCurrentClip()->getDuration();
			}

			if ((type & ANIMTYPE_Skinned) != 0)
			{
				if (m_skinPlayer && m_skinPlayer->getCurrentClip())
					return m_skinPlayer->getCurrentClip()->getDuration();
			}

			if ((type & ANIMTYPE_Material) != 0)
			{
				if (m_mtrlPlayer && m_mtrlPlayer->getCurrentClip())
					return m_mtrlPlayer->getCurrentClip()->Duration;
			}

			return 0;
		}

		bool Model::HasAnimation(AnimationType type) const
		{
			if ((type & ANIMTYPE_Rigid) != 0)
			{
				return !!m_rigidPlayer;
			}
			if ((type & ANIMTYPE_Skinned) != 0)
			{
				return !!m_skinPlayer;
			}
			if ((type & ANIMTYPE_Material) != 0)
			{
				return !!m_mtrlPlayer;
			}
			throw AP_EXCEPTION(ApocExceptionType::Argument, L"type");
		}

		const ModelAnimationClip* GetModelAnimClip(const String& clipName, const AnimationData::ClipTable& clipTable);

		const ModelAnimationClip* Model::GetSkinAnimationSelectedClip() const
		{
			if (!m_animData) return 0;
			if (!m_skinPlayer) return 0;

			const AnimationData::ClipTable& table = m_animData->getSkinnedAnimationClips();
			return GetModelAnimClip(m_selectedClipName, table);
		}
		const MaterialAnimationClip* Model::GetMaterialDurationSelectedClip() const
		{
			if (!m_animData)
				return 0;
			if (!m_mtrlPlayer)
				return 0;

			const AnimationData::MtrlClipTable& table = m_animData->getMaterialAnimationClips();
			if (table.getCount())
			{
				MaterialAnimationClip* clip;

				if (table.TryGetValue(m_selectedClipName, clip))
				{
					return clip;
				}
			}
			return nullptr;
		}
		const ModelAnimationClip* Model::GetRigidAnimationSelectedClip() const
		{
			if (!m_animData) return 0;
			if (!m_skinPlayer) return 0;

			const AnimationData::ClipTable& table = m_animData->getRigidAnimationClips();
			return GetModelAnimClip(m_selectedClipName, table);
		}

		void Model::SetCurrentAnimationKeyFrame(AnimationType type, int index)
		{
			if ((type & ANIMTYPE_Rigid) != 0)
			{
				m_rigidPlayer->setCurrentKeyframe(index);
			}
			if ((type & ANIMTYPE_Skinned) != 0)
			{
				m_skinPlayer->setCurrentKeyframe(index);
			}
			if ((type & ANIMTYPE_Material) != 0)
			{
				m_mtrlPlayer->setCurrentKeyframe(index);
			}
		}
		int Model::GetCurrentAnimationKeyFrame(AnimationType type)
		{
			if ((type & ANIMTYPE_Rigid) != 0)
			{
				return m_rigidPlayer->getCurrentKeyframe();
			}
			if ((type & ANIMTYPE_Skinned) != 0)
			{
				return m_skinPlayer->getCurrentKeyframe();
			}
			if ((type & ANIMTYPE_Material) != 0)
			{
				return m_mtrlPlayer->getCurrentKeyframe();
			}
			throw AP_EXCEPTION(ApocExceptionType::Argument, L"type");
		}
		
		const ModelAnimationClip* GetModelAnimClip(const String& clipName, const AnimationData::ClipTable& clipTable)
		{
			if (clipTable.getCount())
			{
				ModelAnimationClip* clip;

				if (clipTable.TryGetValue(clipName, clip))
				{
					return clip;
				}
			}
			return nullptr;
		}
	}
}
