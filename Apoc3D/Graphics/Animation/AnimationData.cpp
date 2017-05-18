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

#include "AnimationData.h"

#include "apoc3d/IOLib/BinaryReader.h"
#include "apoc3d/IOLib/BinaryWriter.h"
#include "apoc3d/IOLib/TaggedData.h"
#include "apoc3d/Core/Logging.h"

#include "apoc3d/VFS/ResourceLocation.h"

namespace Apoc3D
{
	namespace Graphics
	{
		namespace Animation
		{
			const int MANI_ID = ((byte)'M' << 24) | ((byte)'A' << 16) | ((byte)'N' << 8) | ((byte)'I');
		
			//const String TAG_3_BindPoseTag = L"BindPose";
			//const String TAG_3_BindPoseCountTag = L"BindPoseCount";

			//const String TAG_3_InvBindPoseTag = L"InvBindPose";
			//const String TAG_3_InvBindPoseCountTag = L"InvBindPoseCount";

			constexpr TaggedDataKey TAG_3_SkinnedAnimationClipTag = "SkinnedAnimationClip";
			constexpr TaggedDataKey TAG_3_SkinnedAnimationClipCountTag = "SkinnedAnimationClipCount";

			constexpr TaggedDataKey TAG_3_RigidAnimationClipTag = "RigidAnimationClip";
			constexpr TaggedDataKey TAG_3_RigidAnimationClipCountTag = "RigidAnimationClipCount";

			constexpr TaggedDataKey TAG_3_MaterialAnimationTag = "MaterialAnimation3.0";
			constexpr TaggedDataKey TAG_3_1_MaterialAnimationTag = "MaterialAnimation3.1";
			constexpr TaggedDataKey TAG_3_MaterialAnimationCountTag = "MaterialAnimation3.0Count";

			constexpr TaggedDataKey TAG_3_RootBoneTag = "RootBone";
			constexpr TaggedDataKey TAG_3_RigidEntityCount = "RigidEntityCount";
			constexpr TaggedDataKey TAG_3_BonesTag = "Bones";
			constexpr TaggedDataKey TAG_3_BoneCountTag = "BoneCount";


			AnimationData::~AnimationData()
			{
				m_rigidAnimationClips.DeleteValuesAndClear();
				m_skinnedAnimationClips.DeleteValuesAndClear();
				m_mtrlAnimationClips.DeleteValuesAndClear();
			}

			void AnimationData::ReadData(TaggedDataReader* data)
			{
				RigidEntityCount = 0;
				if (data->Contains(TAG_3_RigidEntityCount))
					RigidEntityCount = data->GetInt32(TAG_3_RigidEntityCount);

				if (data->Contains(TAG_3_MaterialAnimationCountTag))
				{
					m_hasMtrlClip = true;

					int32 count = data->GetInt32(TAG_3_MaterialAnimationCountTag);
					m_mtrlAnimationClips.Resize(count);
					
					bool done = data->TryProcessData(TAG_3_MaterialAnimationTag, [this, count](BinaryReader* br) 
					{
						ReadMaterialAnimationClips(br, count, false);
					});

					if (!done)
					{
						data->TryProcessData(TAG_3_1_MaterialAnimationTag, [this, count](BinaryReader* br)
						{
							ReadMaterialAnimationClips(br, count, true);
						});
					}
				}

				// bones
				if (data->Contains(TAG_3_BoneCountTag))
				{
					int32 boneCount = data->GetInt32(TAG_3_BoneCountTag);

					data->ProcessData(TAG_3_BonesTag, [boneCount, this](BinaryReader* br2)
					{
						for (int32 i = 0; i < boneCount; i++)
						{
							int32 bidx = br2->ReadInt32();
							String name = br2->ReadString();

							Matrix bindpose;
							Matrix boneRef;
							br2->ReadMatrix(bindpose);
							br2->ReadMatrix(boneRef);

							int32 parentId = br2->ReadInt32();
							int32 cldCount = br2->ReadInt32();

							List<int32> children;
							children.ReserveDiscard(cldCount);

							for (int32& ch : children)
							{
								ch = br2->ReadInt32();
							}

							Bone bone = Bone(bidx, bindpose, children, parentId, name);

							bone.setBoneReferenceTransform(boneRef);
							m_bones.Add(bone);
						}
					});
					
					m_rootBone = data->GetInt32(TAG_3_RootBoneTag);
				}

				// skinned animation clip tag
				if (data->Contains(TAG_3_SkinnedAnimationClipCountTag))
				{
					m_hasSkinnedClip = true;

					int32 count = data->GetInt32(TAG_3_SkinnedAnimationClipCountTag);

					m_skinnedAnimationClips.Resize(count);

					data->ProcessData(TAG_3_SkinnedAnimationClipTag, [count, this](BinaryReader* br)
					{
						for (int32 i = 0; i < count; i++)
						{
							String key = br->ReadString();
							float duration = static_cast<float>(br->ReadDouble());
							int32 frameCount = br->ReadInt32();

							List<ModelKeyframe> frames(frameCount);

							for (int32 j = 0; j < frameCount; j++)
							{
								int32 bone = br->ReadInt32();

								float totalSec = static_cast<float>(br->ReadDouble());
								Matrix transfrom;
								br->ReadMatrix(transfrom);

								ModelKeyframe keyframe(bone, totalSec, transfrom);
								keyframe.setNextFrameIndex(br->ReadInt32());
								frames.Add(keyframe);
							}

							m_skinnedAnimationClips.Add(key, new ModelAnimationClip(duration, frames));
						}
					});
				}

				// rigid animation clip tag
				if (data->Contains(TAG_3_RigidAnimationClipCountTag))
				{
					m_hasRigidClip = true;

					int32 count = data->GetInt32(TAG_3_RigidAnimationClipCountTag);

					m_rigidAnimationClips.Resize(count);

					data->ProcessData(TAG_3_RigidAnimationClipTag, [this, count](BinaryReader* br)
					{
						for (int32 i = 0; i < count; i++)
						{
							String key = br->ReadString();
							float duration = static_cast<float>(br->ReadDouble());
							int32 frameCount = br->ReadInt32();

							List<ModelKeyframe> frames(frameCount);
							
							for (int32 j = 0; j < frameCount; j++)
							{
								int32 partId = br->ReadInt32();

								float totalSec = static_cast<float>(br->ReadDouble());
								Matrix transfrom;
								br->ReadMatrix(transfrom);

								ModelKeyframe keyframe(partId, totalSec, transfrom);
								keyframe.setNextFrameIndex(br->ReadInt32());
								frames.Add(keyframe);
							}

							m_rigidAnimationClips.Add(key, new ModelAnimationClip(duration, frames));
						}
					});
				}

			}

			void AnimationData::ReadMaterialAnimationClips(BinaryReader* br, int32 count, bool readsFrameFlag)
			{
				for (int32 i = 0; i < count; i++)
				{
					List<MaterialAnimationKeyframe> keyframes;
					String key = br->ReadString();
					float duration = br->ReadSingle();
					int32 frameCount = br->ReadInt32();
					keyframes.ResizeDiscard(frameCount);

					for (int32 j = 0; j < frameCount; j++)
					{
						float time = br->ReadSingle();
						int32 mid = br->ReadInt32();
						uint32 flags = 0;

						if (readsFrameFlag)
						{
							flags = br->ReadUInt32();
						}

						keyframes.Add(MaterialAnimationKeyframe(time, mid, flags));
					}

					m_mtrlAnimationClips.Add(key, new MaterialAnimationClip(duration, keyframes));
				}
			}

			void AnimationData::WriteData(TaggedDataWriter* data) const
			{
				data->AddInt32(TAG_3_RigidEntityCount, RigidEntityCount);

				if (m_hasMtrlClip)
				{
					data->AddInt32(TAG_3_MaterialAnimationCountTag, (int32)m_mtrlAnimationClips.getCount());

					data->AddEntry(TAG_3_1_MaterialAnimationTag, [this](BinaryWriter* bw) 
					{
						for (auto e : m_mtrlAnimationClips)
						{
							const String& name = e.Key;
							const MaterialAnimationClip* clip = e.Value;

							bw->WriteString(name);
							bw->WriteSingle(clip->Duration);

							const List<MaterialAnimationKeyframe>& keyFrames = clip->Keyframes;
							bw->WriteInt32(keyFrames.getCount());

							for (const MaterialAnimationKeyframe& kf : keyFrames)
							{
								bw->WriteSingle(kf.getTime());
								bw->WriteInt32(kf.getMaterialFrame());
								bw->WriteUInt32(kf.getFlags());
							}
						}
					});
				}

				// Bones
				if (m_bones.getCount())
				{
					data->AddInt32(TAG_3_BoneCountTag, static_cast<int32>(m_bones.getCount()));

					data->AddEntry(TAG_3_BonesTag, [this](BinaryWriter* bw)
					{
						for (const Bone& bone : m_bones)
						{
							bw->WriteInt32(bone.Index);
							bw->WriteString(bone.Name);
							bw->WriteMatrix(bone.getBindPoseTransform());
							bw->WriteMatrix(bone.getBoneReferenceTransform());
							bw->WriteInt32(bone.Parent);

							int32 cldCount = static_cast<int32>(bone.Children.getCount());
							bw->WriteInt32(cldCount);

							for (int32 ch : bone.Children)
							{
								bw->WriteInt32(ch);
							}
						}
					});


					data->AddInt32(TAG_3_RootBoneTag, m_rootBone);
				}

				if (m_hasSkinnedClip)
				{
					data->AddInt32(TAG_3_SkinnedAnimationClipCountTag, m_skinnedAnimationClips.getCount());

					data->AddEntry(TAG_3_SkinnedAnimationClipTag, [this](BinaryWriter* bw)
					{
						for (auto& e : m_skinnedAnimationClips)
						{
							const String& name = e.Key;
							const ModelAnimationClip* clip = e.Value;

							bw->WriteString(name);
							bw->WriteDouble(static_cast<double>(clip->getDuration()));

							const List<ModelKeyframe>& keyFrames = clip->getKeyframes();

							bw->WriteInt32(keyFrames.getCount());

							for (const ModelKeyframe& frame : keyFrames)
							{
								bw->WriteInt32(static_cast<int32>(frame.getObjectIndex()));
								bw->WriteDouble(static_cast<double>(frame.getTime()));
								bw->WriteMatrix(frame.getTransform());
								bw->WriteInt32(static_cast<int32>(frame.getNextFrameIndex()));
							}
						}
					});
				}

				if (m_hasRigidClip)
				{
					data->AddInt32(TAG_3_RigidAnimationClipCountTag, m_rigidAnimationClips.getCount());

					data->AddEntry(TAG_3_RigidAnimationClipTag, [this](BinaryWriter* bw)
					{
						for (auto e : m_rigidAnimationClips)
						{
							const String& name = e.Key;
							const ModelAnimationClip* clip = e.Value;

							bw->WriteString(name);
							bw->WriteDouble(static_cast<double>(clip->getDuration()));

							const List<ModelKeyframe>& keyFrames = clip->getKeyframes();
							bw->WriteInt32(keyFrames.getCount());

							for (const ModelKeyframe& kf : keyFrames)
							{
								bw->WriteInt32(static_cast<int32>(kf.getObjectIndex()));
								bw->WriteDouble(static_cast<double>(kf.getTime()));
								bw->WriteMatrix(kf.getTransform());
								bw->WriteInt32(static_cast<int32>(kf.getNextFrameIndex()));
							}
						}
					});
				}
			}

			void AnimationData::Load(const ResourceLocation& rl)
			{
				BinaryReader br(rl);

				int32 id = br.ReadInt32();
				if (id == MANI_ID)
				{
					br.ReadTaggedDataBlock([this](TaggedDataReader* data) 
					{
						ReadData(data);
					});
				}
				else
				{
					LogManager::getSingleton().Write(LOG_Graphics, L"Invalid animation file." + rl.getName(), LOGLVL_Error);
				}

			}
			void AnimationData::Save(Stream& strm) const
			{
				BinaryWriter bw(&strm, false);

				bw.WriteInt32(MANI_ID);
				bw.WriteTaggedDataBlock([this](TaggedDataWriter* data) 
				{
					WriteData(data);
				});
			}
		}
	}
}