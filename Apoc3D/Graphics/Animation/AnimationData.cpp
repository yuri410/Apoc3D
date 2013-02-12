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
			static const int MANI_ID = ((byte)'M' << 24) | ((byte)'A' << 16) | ((byte)'N' << 8) | ((byte)'I');
		
			//static const String TAG_3_BindPoseTag = L"BindPose";
			//static const String TAG_3_BindPoseCountTag = L"BindPoseCount";

			//static const String TAG_3_InvBindPoseTag = L"InvBindPose";
			//static const String TAG_3_InvBindPoseCountTag = L"InvBindPoseCount";

			static const String TAG_3_SkinnedAnimationClipTag = L"SkinnedAnimationClip";
			static const String TAG_3_SkinnedAnimationClipCountTag = L"SkinnedAnimationClipCount";

			static const String TAG_3_RigidAnimationClipTag = L"RigidAnimationClip";
			static const String TAG_3_RigidAnimationClipCountTag = L"RigidAnimationClipCount";

			static const String TAG_2_ModelAnimationClipTag = L"ModelAnimationClip";
			static const String TAG_2_ModelAnimationClipCountTag = L"ModelAnimationClipCount";

			static const String TAG_2_RootAnimationClipTag = L"RootAnimationClip";
			static const String TAG_2_RootAnimationClipCountTag = L"RootAnimationClipCount";

			//static const String TAG_3_BoneHierarchyTag = L"BoneHierarchy";
			//static const String TAG_3_BoneHierarchyCountTag = L"BoneHierarchyCount";

			static const String TAG_2_MaterialAnimationTag = L"MaterialAnimation2.0";
			static const String TAG_2_MaterialAnimationDurationTag = L"Duration";
			static const String TAG_2_MaterialAnimationKeyframesTag = L"Keyframes";
			static const String TAG_2_MaterialAnimationKeyframeCountTag = L"KeyframeCount";

			static const String TAG_3_MaterialAnimationTag = L"MaterialAnimation3.0";
			static const String TAG_3_MaterialAnimationCountTag = L"MaterialAnimation3.0Count";

			static const String TAG_3_RootBoneTag = L"RootBone";
			static const String TAG_3_RigidEntityCount = L"RigidEntityCount";
			static const String TAG_3_BonesTag = L"Bones";
			static const String TAG_3_BoneCountTag = L"BoneCount";

			void AnimationData::LoadMtrlAnimation2(TaggedDataReader* data)
			{
				float duration = data->GetDataSingle(TAG_2_MaterialAnimationDurationTag);
				uint32 count = data->GetDataUInt32(TAG_2_MaterialAnimationKeyframeCountTag);

				FastList<MaterialAnimationKeyframe> keyframes;
				BinaryReader* br = data->GetData(TAG_2_MaterialAnimationKeyframesTag);

				for (uint32 i=0;i<count;i++)
				{
					float time = br->ReadSingle();
					int32 mid = br->ReadInt32();

					keyframes.Add(MaterialAnimationKeyframe(time, mid));
				}

				br->Close();

				m_mtrlAnimationClips.insert(
					MtrlClipTable::value_type(L"Take 001", new MaterialAnimationClip(duration, keyframes)));

				delete br;
			}
			TaggedDataWriter* AnimationData::SaveMtrlAnimation2()
			{
				TaggedDataWriter* data = new TaggedDataWriter(true);

				const MaterialAnimationClip* clip = m_mtrlAnimationClips.begin()->second;
				const FastList<MaterialAnimationKeyframe>& keyframes = clip->Keyframes;

				data->AddEntrySingle(TAG_2_MaterialAnimationDurationTag, clip->Duration);
				data->AddEntryUInt32(TAG_2_MaterialAnimationKeyframeCountTag, 
					static_cast<uint32>(keyframes.getCount()));

				BinaryWriter* bw = data->AddEntry(TAG_2_MaterialAnimationKeyframesTag);
				for (int i=0;i<keyframes.getCount();i++)
				{
					bw->Write(keyframes[i].getTime());
					bw->Write(static_cast<int32>(keyframes[i].getMaterialFrame()));
				}
				bw->Close();
				delete bw;
				return data;
			}


			void AnimationData::ReadData(TaggedDataReader* data)
			{
				if (data->Contains(TAG_2_MaterialAnimationTag))
				{
					// Note that the MaterialAnimation tagged data block hierarchy 
					// is different from version 2.
					// So if the function parameter 'data' is the AnimationData block in the version 2 model,
					// the following line will not be executed. 
					// because in version 2 TAG_2_MaterialAnimationTag is with the model
					// data block instead of the AnimationData block.

					// This if branch is written for upgrading version 2 models purposes. In the conversion process,
					// this method should be called twice. The first time call, parameter 'data' is model data block,
					// so material animation will be loaded. Then the second time, call with AnimationData block
					// as the 'data' parameter, so the rest animation data will be loaded.
					BinaryReader* br = data->GetData(TAG_2_MaterialAnimationTag);
					TaggedDataReader* animData = br->ReadTaggedDataBlock();
					LoadMtrlAnimation2(animData);
					animData->Close();
					delete animData;

					br->Close();
					delete br;
				}
				
				RigidEntityCount = 0;
				if (data->Contains(TAG_3_RigidEntityCount))
					RigidEntityCount = data->GetDataInt32(TAG_3_RigidEntityCount);

				if (data->Contains(TAG_3_MaterialAnimationTag))
				{
					m_hasMtrlClip = true;

					int count = data->GetDataInt32(TAG_3_MaterialAnimationCountTag);

					m_mtrlAnimationClips.rehash(count);

					BinaryReader* br = data->GetData(TAG_3_MaterialAnimationTag);
					
					for (int i=0;i<count;i++)
					{
						FastList<MaterialAnimationKeyframe> keyframes;
						String key = br->ReadString();

						float duration = br->ReadSingle();

						int frameCount = br->ReadInt32();
						keyframes.ResizeDiscard(frameCount);
						
						for (int j=0;j<frameCount;j++)
						{
							float time = br->ReadSingle();
							int32 mid = br->ReadInt32();

							keyframes.Add(MaterialAnimationKeyframe(time, mid));
						}

						MaterialAnimationClip* clip = new MaterialAnimationClip(duration, keyframes);

						m_mtrlAnimationClips.insert(MtrlClipTable::value_type(key, clip));
					}

					br->Close();

					delete br;
				}
				// bones
				if (data->Contains(TAG_3_BoneCountTag))
				{
					int boenCount = data->GetDataInt32(TAG_3_BoneCountTag);

					BinaryReader* br2 = data->GetData(TAG_3_BonesTag);

					for (int i = 0; i < boenCount; i++)
					{
						int bidx = br2->ReadInt32();
						String name = br2->ReadString();

						Matrix bindpose;
						Matrix boneRef;
						br2->ReadMatrix(bindpose);
						br2->ReadMatrix(boneRef);

						int parentId = br2->ReadInt32();

						int cldCount = br2->ReadInt32();

						FastList<int> children(cldCount);
						for (int j = 0; j < cldCount; j++)
						{
							children.Add(br2->ReadInt32());
						}

						Bone bone = Bone(bidx, bindpose, children, parentId, name);
						
						bone.setBoneReferenceTransform(boneRef);
						m_bones.Add(bone);
					}
					br2->Close();
					delete br2;
					m_rootBone = data->GetDataInt32(TAG_3_RootBoneTag);
				}

				// skinned animation clip tag
				if (data->Contains(TAG_3_SkinnedAnimationClipCountTag))
				{
					m_hasSkinnedClip = true;

					int count = data->GetDataInt32(TAG_3_SkinnedAnimationClipCountTag);

					m_skinnedAnimationClips.rehash(count);

					BinaryReader* br = data->GetData(TAG_3_SkinnedAnimationClipTag);
					for (int i=0;i<count;i++)
					{
						String key = br->ReadString();

						float duration = static_cast<float>(br->ReadDouble());

						int frameCount = br->ReadInt32();

						FastList<ModelKeyframe> frames(frameCount);

						for (int j=0;j<frameCount;j++)
						{
							int32 bone = br->ReadInt32();

							float totalSec = static_cast<float>(br->ReadDouble());
							Matrix transfrom;
							br->ReadMatrix(transfrom);

							ModelKeyframe keyframe(bone, totalSec, transfrom);
							keyframe.setNextFrameIndex(br->ReadInt32());
							frames.Add(keyframe);
						}

						ModelAnimationClip* clip = new ModelAnimationClip(duration, frames);
						
						m_skinnedAnimationClips.insert(ClipTable::value_type(key, clip));
					}
					br->Close();
					delete br;
				}

				// rigid animation clip tag
				if (data->Contains(TAG_3_RigidAnimationClipCountTag))
				{
					m_hasRigidClip = true;

					int count = data->GetDataInt32(TAG_3_RigidAnimationClipCountTag);

					m_rigidAnimationClips.rehash(count);

					BinaryReader* br = data->GetData(TAG_3_RigidAnimationClipTag);
					for (int i=0;i<count;i++)
					{
						String key = br->ReadString();


						float duration = static_cast<float>(br->ReadDouble());

						int frameCount = br->ReadInt32();

						FastList<ModelKeyframe> frames(frameCount);

						for (int j=0;j<frameCount;j++)
						{
							int32 bone = br->ReadInt32();

							float totalSec = static_cast<float>(br->ReadDouble());
							Matrix transfrom;
							br->ReadMatrix(transfrom);

							ModelKeyframe keyframe(bone, totalSec, transfrom);
							keyframe.setNextFrameIndex(br->ReadInt32());
							frames.Add(keyframe);
						}

						ModelAnimationClip* clip = new ModelAnimationClip(duration, frames);
						m_rigidAnimationClips.insert(ClipTable::value_type(key, clip));
					}
				}

			}

			TaggedDataWriter* AnimationData::WriteData() const
			{
				TaggedDataWriter* data = new TaggedDataWriter(true);

				data->AddEntryInt32(TAG_3_RigidEntityCount, RigidEntityCount);

				if (m_hasMtrlClip)
				{
					data->AddEntryInt32(TAG_3_MaterialAnimationCountTag, (int32)m_mtrlAnimationClips.size());

					BinaryWriter* bw = data->AddEntry(TAG_3_MaterialAnimationTag);
					for (MtrlClipTable::const_iterator iter = m_mtrlAnimationClips.begin(); iter != m_mtrlAnimationClips.end(); iter++)
					{
						bw->Write(iter->first);

						const MaterialAnimationClip* clip = iter->second;
						bw->Write(clip->Duration);

						const FastList<MaterialAnimationKeyframe>& keyFrames = clip->Keyframes;
						bw->Write(static_cast<int32>(keyFrames.getCount()));

						for (int i = 0; i < keyFrames.getCount(); i++)
						{
							bw->Write(keyFrames[i].getTime());
							bw->Write(keyFrames[i].getMaterialFrame());
						}
					}
					bw->Close();
					delete bw;
				}

				// Bones
				if (m_bones.getCount())
				{
					data->AddEntryInt32(TAG_3_BoneCountTag, static_cast<int32>(m_bones.getCount()));

					BinaryWriter* bw = data->AddEntry(TAG_3_BonesTag);
					for (int i = 0; i < m_bones.getCount(); i++)
					{
						bw->Write(m_bones[i].Index);
						bw->Write(m_bones[i].Name);
						bw->Write(m_bones[i].getBindPoseTransform());
						bw->Write(m_bones[i].getBoneReferenceTransform());
						bw->Write(m_bones[i].Parent);

						int cldCount = static_cast<int32>(m_bones[i].Children.getCount());
						bw->Write(cldCount);

						for (int j = 0; j < cldCount; j++)
						{
							bw->Write(m_bones[i].Children[j]);
						}

					}
					bw->Close();
					delete bw;

					data->AddEntryInt32(TAG_3_RootBoneTag, m_rootBone);
				}

				if (m_hasSkinnedClip)
				{
					data->AddEntryUInt32(TAG_3_SkinnedAnimationClipCountTag, m_skinnedAnimationClips.size());

					BinaryWriter* bw = data->AddEntry(TAG_3_SkinnedAnimationClipTag);
					for (ClipTable::const_iterator iter = m_skinnedAnimationClips.begin(); iter != m_skinnedAnimationClips.end(); iter++)
					{
						bw->Write(iter->first);

						const ModelAnimationClip* clip = iter->second;
						bw->Write(static_cast<double>(clip->getDuration()));

						const FastList<ModelKeyframe>& keyFrames = clip->getKeyframes();

						bw->Write(static_cast<int32>(keyFrames.getCount()));

						for (int32 i = 0; i < keyFrames.getCount(); i++)
						{
							bw->Write(static_cast<int32>(keyFrames[i].getObjectIndex()));
							bw->Write(static_cast<double>(keyFrames[i].getTime()));
							bw->Write(keyFrames[i].getTransform());
							bw->Write(static_cast<int32>(keyFrames[i].getNextFrameIndex()));
						}
					}
					bw->Close();
					delete bw;
				}
				if (m_hasRigidClip)
				{
					data->AddEntryUInt32(TAG_3_RigidAnimationClipCountTag, m_rigidAnimationClips.size());

					BinaryWriter* bw = data->AddEntry(TAG_3_RigidAnimationClipTag);
					for (ClipTable::const_iterator iter = m_rigidAnimationClips.begin(); iter != m_rigidAnimationClips.end(); iter++)
					{
						bw->Write(iter->first);

						const ModelAnimationClip* clip = iter->second;
						bw->Write(static_cast<double>(clip->getDuration()));
						
						const FastList<ModelKeyframe>& keyFrames = clip->getKeyframes();
						bw->Write(static_cast<int32>(keyFrames.getCount()));

						for (int i = 0; i < keyFrames.getCount(); i++)
						{
							bw->Write(static_cast<int32>(keyFrames[i].getObjectIndex()));
							bw->Write(static_cast<double>(keyFrames[i].getTime()));
							bw->Write(keyFrames[i].getTransform());
							bw->Write(static_cast<int32>(keyFrames[i].getNextFrameIndex()));
						}
					}
					bw->Close();
					delete bw;
				}

				return data;
			}

			void AnimationData::Load(const ResourceLocation* rl)
			{
				BinaryReader* br = new BinaryReader(rl->GetReadStream());

				int32 id = br->ReadInt32();
				if (id == MANI_ID)
				{
					TaggedDataReader* data = br->ReadTaggedDataBlock();

					ReadData(data);

					data->Close();
					delete data;
				}
				else
				{
					LogManager::getSingleton().Write(LOG_Graphics, L"Invalid animation file." + rl->getName(), LOGLVL_Error);
				}

				br->Close();
				delete br;
			}
			void AnimationData::Save(Stream* strm) const
			{
				BinaryWriter* bw = new BinaryWriter(strm);

				bw->Write(MANI_ID);

				TaggedDataWriter* mdlData = WriteData();
				bw->Write(mdlData);
				delete mdlData;

				bw->Close();
				delete bw;
			}
		}
	}
}