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
			static const int MANI_ID = ((byte)'M' << 24) | ((byte)'A' << 16) | ((byte)'N' << 8) | ((byte)'I');
		
			//static const String TAG_3_BindPoseTag = L"BindPose";
			//static const String TAG_3_BindPoseCountTag = L"BindPoseCount";

			//static const String TAG_3_InvBindPoseTag = L"InvBindPose";
			//static const String TAG_3_InvBindPoseCountTag = L"InvBindPoseCount";

			static const String TAG_3_SkinnedAnimationClipTag = L"SkinnedAnimationClip";
			static const String TAG_3_SkinnedAnimationClipCountTag = L"SkinnedAnimationClipCount";

			static const String TAG_3_RigidAnimationClipTag = L"RigidAnimationClip";
			static const String TAG_3_RigidAnimationClipCountTag = L"RigidAnimationClipCount";

			static const String TAG_3_MaterialAnimationTag = L"MaterialAnimation3.0";
			static const String TAG_3_MaterialAnimationCountTag = L"MaterialAnimation3.0Count";

			static const String TAG_3_RootBoneTag = L"RootBone";
			static const String TAG_3_RigidEntityCount = L"RigidEntityCount";
			static const String TAG_3_BonesTag = L"Bones";
			static const String TAG_3_BoneCountTag = L"BoneCount";



			void AnimationData::ReadData(TaggedDataReader* data)
			{
				RigidEntityCount = 0;
				if (data->Contains(TAG_3_RigidEntityCount))
					RigidEntityCount = data->GetDataInt32(TAG_3_RigidEntityCount);

				if (data->Contains(TAG_3_MaterialAnimationTag))
				{
					m_hasMtrlClip = true;

					int count = data->GetDataInt32(TAG_3_MaterialAnimationCountTag);

					m_mtrlAnimationClips.Resize(count);

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

						m_mtrlAnimationClips.Add(key, clip);
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

					m_skinnedAnimationClips.Resize(count);

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
						
						m_skinnedAnimationClips.Add(key, clip);
					}
					br->Close();
					delete br;
				}

				// rigid animation clip tag
				if (data->Contains(TAG_3_RigidAnimationClipCountTag))
				{
					m_hasRigidClip = true;

					int count = data->GetDataInt32(TAG_3_RigidAnimationClipCountTag);

					m_rigidAnimationClips.Resize(count);

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
						m_rigidAnimationClips.Add(key, clip);
					}
				}

			}

			TaggedDataWriter* AnimationData::WriteData() const
			{
				TaggedDataWriter* data = new TaggedDataWriter(true);

				data->AddEntryInt32(TAG_3_RigidEntityCount, RigidEntityCount);

				if (m_hasMtrlClip)
				{
					data->AddEntryInt32(TAG_3_MaterialAnimationCountTag, (int32)m_mtrlAnimationClips.getCount());

					BinaryWriter* bw = data->AddEntry(TAG_3_MaterialAnimationTag);
					for (MtrlClipTable::Enumerator e = m_mtrlAnimationClips.GetEnumerator(); e.MoveNext();)
					{
						const String& name = *e.getCurrentKey();

						bw->WriteString(name);

						const MaterialAnimationClip* clip = *e.getCurrentValue();
						bw->WriteSingle(clip->Duration);

						const FastList<MaterialAnimationKeyframe>& keyFrames = clip->Keyframes;
						bw->WriteInt32(keyFrames.getCount());

						for (int i = 0; i < keyFrames.getCount(); i++)
						{
							bw->WriteSingle(keyFrames[i].getTime());
							bw->WriteInt32(keyFrames[i].getMaterialFrame());
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
						bw->WriteInt32(m_bones[i].Index);
						bw->WriteString(m_bones[i].Name);
						bw->WriteMatrix(m_bones[i].getBindPoseTransform());
						bw->WriteMatrix(m_bones[i].getBoneReferenceTransform());
						bw->WriteInt32(m_bones[i].Parent);

						int cldCount = static_cast<int32>(m_bones[i].Children.getCount());
						bw->WriteInt32(cldCount);

						for (int j = 0; j < cldCount; j++)
						{
							bw->WriteInt32(m_bones[i].Children[j]);
						}

					}
					bw->Close();
					delete bw;

					data->AddEntryInt32(TAG_3_RootBoneTag, m_rootBone);
				}

				if (m_hasSkinnedClip)
				{
					data->AddEntryInt32(TAG_3_SkinnedAnimationClipCountTag, m_skinnedAnimationClips.getCount());

					BinaryWriter* bw = data->AddEntry(TAG_3_SkinnedAnimationClipTag);
					for (ClipTable::Enumerator e = m_skinnedAnimationClips.GetEnumerator(); e.MoveNext();)
					{
						const String& name = *e.getCurrentKey();
						bw->WriteString(name);

						const ModelAnimationClip* clip = *e.getCurrentValue();
						bw->WriteDouble(static_cast<double>(clip->getDuration()));

						const FastList<ModelKeyframe>& keyFrames = clip->getKeyframes();

						bw->WriteInt32(keyFrames.getCount());

						for (int32 i = 0; i < keyFrames.getCount(); i++)
						{
							bw->WriteInt32(static_cast<int32>(keyFrames[i].getObjectIndex()));
							bw->WriteDouble(static_cast<double>(keyFrames[i].getTime()));
							bw->WriteMatrix(keyFrames[i].getTransform());
							bw->WriteInt32(static_cast<int32>(keyFrames[i].getNextFrameIndex()));
						}
					}
					bw->Close();
					delete bw;
				}
				if (m_hasRigidClip)
				{
					data->AddEntryInt32(TAG_3_RigidAnimationClipCountTag, m_rigidAnimationClips.getCount());

					BinaryWriter* bw = data->AddEntry(TAG_3_RigidAnimationClipTag);
					for (ClipTable::Enumerator e = m_rigidAnimationClips.GetEnumerator(); e.MoveNext();)
					{
						const String& name = *e.getCurrentKey();
						bw->WriteString(name);

						const ModelAnimationClip* clip = *e.getCurrentValue();
						bw->WriteDouble(static_cast<double>(clip->getDuration()));
						
						const FastList<ModelKeyframe>& keyFrames = clip->getKeyframes();
						bw->WriteInt32(keyFrames.getCount());

						for (int i = 0; i < keyFrames.getCount(); i++)
						{
							bw->WriteInt32(static_cast<int32>(keyFrames[i].getObjectIndex()));
							bw->WriteDouble(static_cast<double>(keyFrames[i].getTime()));
							bw->WriteMatrix(keyFrames[i].getTransform());
							bw->WriteInt32(static_cast<int32>(keyFrames[i].getNextFrameIndex()));
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

				bw->WriteInt32(MANI_ID);

				TaggedDataWriter* mdlData = WriteData();
				bw->WriteTaggedDataBlock(mdlData);
				delete mdlData;

				bw->Close();
				delete bw;
			}
		}
	}
}