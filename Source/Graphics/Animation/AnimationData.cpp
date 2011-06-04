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

#include "IO/BinaryReader.h"
#include "IO/BinaryWriter.h"
#include "IO/TaggedData.h"

#include "AnimationTypes.h"

namespace Apoc3D
{
	namespace Graphics
	{
		namespace Animation
		{

			static const String TAG_3_BindPoseTag = L"BindPose";
			static const String TAG_3_BindPoseCountTag = L"BindPoseCount";

			static const String TAG_3_InvBindPoseTag = L"InvBindPose";
			static const String TAG_3_InvBindPoseCountTag = L"InvBindPoseCount";

			static const String TAG_3_ModelAnimationClipTag = L"ModelAnimationClip";
			static const String TAG_3_ModelAnimationClipCountTag = L"ModelAnimationClipCount";

			static const String TAG_3_RootAnimationClipTag = L"RootAnimationClip";
			static const String TAG_3_RootAnimationClipCountTag = L"RootAnimationClipCount";

			static const String TAG_3_BoneHierarchyTag = L"BoneHierarchy";
			static const String TAG_3_BoneHierarchyCountTag = L"BoneHierarchyCount";

			static const String TAG_2_MaterialAnimationTag = L"MaterialAnimation2.0";
			static const String TAG_2_MaterialAnimationDurationTag = L"Duration";
			static const String TAG_2_MaterialAnimationKeyframesTag = L"Keyframes";
			static const String TAG_2_MaterialAnimationKeyframeCountTag = L"KeyframeCount";

			static const String TAG_3_MaterialAnimationTag = L"MaterialAnimation3.0";
			static const String TAG_3_MaterialAnimationCountTag = L"MaterialAnimation3.0Count";


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

				data->AddEntry(TAG_2_MaterialAnimationDurationTag, clip->Duration);
				data->AddEntry(TAG_2_MaterialAnimationKeyframeCountTag, 
					static_cast<uint32>(keyframes.getCount()));

				BinaryWriter* bw = data->AddEntry(TAG_2_MaterialAnimationKeyframesTag);
				for (size_t i=0;i<keyframes.getCount();i++)
				{
					bw->Write(keyframes[i].getTime());
					bw->Write(static_cast<int32>(keyframes[i].getMaterialFrame()));
				}
				bw->Close();
				delete bw;
				return data;
			}


			void AnimationData::Load(TaggedDataReader* data)
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
				
				if (data->Contains(TAG_3_MaterialAnimationTag))
				{
					int count = data->GetDataInt32(TAG_3_MaterialAnimationCountTag);

					m_mtrlAnimationClips.rehash(count);

					FastList<MaterialAnimationKeyframe> keyframes;
					BinaryReader* br = data->GetData(TAG_3_MaterialAnimationTag);
					
					for (int i=0;i<count;i++)
					{
						String key = br->ReadString();

						float duration = br->ReadSingle();

						int frameCount = br->ReadInt32();
						keyframes.ResizeDiscard(frameCount);
						
						for (int j=0;j<frameCount;i++)
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

				// bind pose
				if (data->Contains(TAG_3_BindPoseTag))
				{
					m_hasBindPose = true;

					int count = data->GetDataInt32(TAG_3_BindPoseCountTag);
					m_bindPose.ResizeDiscard(count);

					BinaryReader* br = data->GetData(TAG_3_BindPoseTag);

					for (int i=0;i<count;i++)
					{
						Matrix transfrom;
						br->ReadMatrix(transfrom);

						m_bindPose.Add(transfrom);
					}

					br->Close();
					delete br;
				}

				// inv bind pose
				if (data->Contains(TAG_3_InvBindPoseTag))
				{
					m_hasBindPose = true;

					int count = data->GetDataInt32(TAG_3_InvBindPoseCountTag);
					m_invBindPose.ResizeDiscard(count);

					BinaryReader* br = data->GetData(TAG_3_InvBindPoseTag);
					for (int i=0;i<count;i++)
					{
						Matrix transfrom;
						br->ReadMatrix(transfrom);

						m_invBindPose.Add(transfrom);
					}

					br->Close();
					delete br;
				}

				// animation clip tag
				if (data->Contains(TAG_3_ModelAnimationClipCountTag))
				{
					m_hasModelClip = true;

					int count = data->GetDataInt32(TAG_3_ModelAnimationClipCountTag);

					m_modelAnimationClips.rehash(count);

					BinaryReader* br = data->GetData(TAG_3_ModelAnimationClipTag);
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

							frames.Add(ModelKeyframe(bone, totalSec, transfrom));
						}

						ModelAnimationClip* clip = new ModelAnimationClip(duration, frames);
						
						m_modelAnimationClips.insert(ClipTable::value_type(key, clip));
					}
					br->Close();
					delete br;
				}

				// root animation clip tag
				if (data->GetData(TAG_3_RootAnimationClipCountTag))
				{
					m_hasRootClip = true;

					int count = data->GetDataInt32(TAG_3_RootAnimationClipCountTag);

					m_rootAnimationClips.rehash(count);

					BinaryReader* br = data->GetData(TAG_3_RootAnimationClipTag);
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

							frames.Add(ModelKeyframe(bone, totalSec, transfrom));
						}

						ModelAnimationClip* clip = new ModelAnimationClip(duration, frames);
						m_rootAnimationClips.insert(ClipTable::value_type(key, clip));
					}
				}

				// bone hierarchy tag
				if (data->GetData(TAG_3_BoneHierarchyCountTag))
				{
					int count = data->GetDataInt32(TAG_3_BoneHierarchyCountTag);
					m_hasSkeleton = true;
					
					m_skeletonHierarchy.ResizeDiscard(count);

					BinaryReader* br = data->GetData(TAG_3_BoneHierarchyTag);
					for (int i=0;i<count;i++)
					{
						m_skeletonHierarchy.Add(br->ReadInt32());
					}
					br->Close();
					delete br;
				}

			}

			TaggedDataWriter* AnimationData::Save()
			{
				TaggedDataWriter* data = new TaggedDataWriter(true);

				if (m_hasMtrlClip)
				{
					data->AddEntry(TAG_3_MaterialAnimationCountTag, m_mtrlAnimationClips.size());

					BinaryWriter* bw = data->AddEntry(TAG_3_MaterialAnimationTag);
					for (MtrlClipTable::iterator iter = m_mtrlAnimationClips.begin(); iter != m_mtrlAnimationClips.end(); iter++)
					{
						bw->Write(iter->first);

						const MaterialAnimationClip* clip = iter->second;
						bw->Write(clip->Duration);

						const FastList<MaterialAnimationKeyframe>& keyFrames = clip->Keyframes;
						bw->Write(static_cast<int32>(keyFrames.getCount()));

						for (size_t i = 0; i < keyFrames.getCount(); i++)
						{
							bw->Write(keyFrames[i].getTime());
							bw->Write(keyFrames[i].getMaterialFrame());
						}
					}
					bw->Close();
					delete bw;
				}

				if (m_hasBindPose)
				{
					{
						data->AddEntry(TAG_3_BindPoseCountTag, static_cast<int32>(m_bindPose.getCount()));

						BinaryWriter* bw = data->AddEntry(TAG_3_BindPoseTag);

						for (int32 i=0;i<m_bindPose.getCount();i++)
						{
							bw->Write(m_bindPose[i]);
						}

						bw->Close();
						delete bw;
					}
					{
						data->AddEntry(TAG_3_InvBindPoseCountTag, static_cast<int32>(m_invBindPose.getCount()));

						BinaryWriter* bw = data->AddEntry(TAG_3_InvBindPoseTag);
						for (int32 i = 0; i < m_invBindPose.getCount(); i++)
						{
							bw->Write(m_invBindPose[i]);
						}
						bw->Close();
						delete bw;
					}
				}
				
				if (m_hasModelClip)
				{
					data->AddEntry(TAG_3_ModelAnimationClipCountTag, m_modelAnimationClips.size());

					BinaryWriter* bw = data->AddEntry(TAG_3_ModelAnimationClipTag);
					for (ClipTable::iterator iter = m_modelAnimationClips.begin(); iter != m_modelAnimationClips.end(); iter++)
					{
						bw->Write(iter->first);

						const ModelAnimationClip* clip = iter->second;
						bw->Write(static_cast<double>(clip->getDuration()));

						const FastList<ModelKeyframe>& keyFrames = clip->getKeyframes();

						bw->Write(static_cast<int32>(keyFrames.getCount()));

						for (int32 i = 0; i < keyFrames.getCount(); i++)
						{
							bw->Write(keyFrames[i].getBone());
							bw->Write(static_cast<double>(keyFrames[i].getTime()));
							bw->Write(keyFrames[i].getTransform());
						}
					}
					bw->Close();
					delete bw;
				}
				if (m_hasRootClip)
				{
					data->AddEntry(TAG_3_RootAnimationClipCountTag, m_rootAnimationClips.size());

					BinaryWriter* bw = data->AddEntry(TAG_3_RootAnimationClipTag);
					for (ClipTable::iterator iter = m_rootAnimationClips.begin(); iter != m_rootAnimationClips.end(); iter++)
					{
						bw->Write(iter->first);

						const ModelAnimationClip* clip = iter->second;
						bw->Write(static_cast<double>(clip->getDuration()));
						
						const FastList<ModelKeyframe>& keyFrames = clip->getKeyframes();
						bw->Write(static_cast<int32>(keyFrames.getCount()));

						for (size_t i = 0; i < keyFrames.getCount(); i++)
						{
							bw->Write(keyFrames[i].getBone());
							bw->Write(static_cast<double>(keyFrames[i].getTime()));
							bw->Write(keyFrames[i].getTransform());
						}
					}
					bw->Close();
					delete bw;
				}

				if (m_hasSkeleton)
				{
					data->AddEntry(TAG_3_BoneHierarchyCountTag, static_cast<int32>(m_skeletonHierarchy.getCount()));

					BinaryWriter* bw = data->AddEntry(TAG_3_BoneHierarchyTag);
					for (size_t i = 0; i < m_skeletonHierarchy.getCount(); i++)
					{
						bw->Write(m_skeletonHierarchy[i]);
					}
					bw->Close();
					delete bw;
				}
				return data;
			}
		}
	}
}