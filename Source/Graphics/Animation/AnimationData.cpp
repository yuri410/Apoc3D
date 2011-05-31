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

				vector<MaterialAnimationKeyframe> keyframes;
				BinaryReader* br = data->GetData(TAG_2_MaterialAnimationKeyframesTag);

				for (uint32 i=0;i<count;i++)
				{
					float time = br->ReadSingle();
					int32 mid = br->ReadInt32();

					keyframes.push_back(MaterialAnimationKeyframe(time, mid));
				}

				br->Close();

				m_mtrlAnimationClips.insert(MtrlClipTable::value_type(L"Take 001", MaterialAnimationClip(duration, keyframes)));

				delete br;
			}
			TaggedDataWriter* AnimationData::SaveMtrlAnimation2()
			{
				TaggedDataWriter* data = new TaggedDataWriter(true);

				const MaterialAnimationClip* clip = m_mtrlAnimationClips.begin()->second;
				data->AddEntry(TAG_2_MaterialAnimationDurationTag, clip->Duration);
				data->AddEntry(TAG_2_MaterialAnimationKeyframeCountTag, 
					static_cast<uint32>(clip->Keyframes.size()));

				const vector<MaterialAnimationKeyframe>& keyframes = clip->Keyframes;

				BinaryWriter* bw = data->AddEntry(TAG_2_MaterialAnimationKeyframesTag);
				for (size_t i=0;i<keyframes.size();i++)
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

					vector<MaterialAnimationKeyframe> keyframes;
					BinaryReader* br = data->GetData(TAG_3_MaterialAnimationTag);
					
					for (int i=0;i<count;i++)
					{
						String key = br->ReadString();

						float duration = static_cast<float>(br->ReadDouble());

						int frameCount = br->ReadInt32();
						keyframes.reserve(frameCount);
						
						for (int j=0;j<frameCount;i++)
						{
							float time = br->ReadSingle();
							int32 mid = br->ReadInt32();

							keyframes.push_back(MaterialAnimationKeyframe(time, mid));
						}

						MaterialAnimationClip* clip = new MaterialAnimationClip(duration, keyframes);

						m_mtrlAnimationClips.insert(ClipTable::value_type(key, clip));
					}

					br->Close();

					delete br;
				}

				// bind pose
				if (data->Contains(TAG_3_BindPoseTag))
				{
					m_hasBindPose = true;

					int count = data->GetDataInt32(TAG_3_BindPoseCountTag);
					m_bindPose.reserve(count);

					BinaryReader* br = data->GetData(TAG_3_BindPoseTag);

					for (int i=0;i<count;i++)
					{
						Matrix transfrom;
						br->ReadMatrix(transfrom);

						m_bindPose.push_back(transfrom);
					}

					br->Close();
					delete br;
				}

				// inv bind pose
				if (data->Contains(TAG_3_InvBindPoseTag))
				{
					int count = data->GetDataInt32(TAG_3_InvBindPoseCountTag);
					m_invBindPose.reserve(count);

					BinaryReader* br = data->GetData(TAG_3_InvBindPoseTag);
					for (int i=0;i<count;i++)
					{
						Matrix transfrom;
						br->ReadMatrix(transfrom);

						m_invBindPose.push_back(transfrom);
					}

					br->Close();
					delete br;
				}

				// animation clip tag
				if (data->Contains(TAG_3_ModelAnimationClipCountTag))
				{
					int count = data->GetDataInt32(TAG_3_ModelAnimationClipCountTag);

					m_modelAnimationClips.rehash(count);

					BinaryReader* br = data->GetData(TAG_3_ModelAnimationClipTag);
					for (int i=0;i<count;i++)
					{
						String key = br->ReadString();

						float duration = static_cast<float>(br->ReadDouble());

						int frameCount = br->ReadInt32();

						vector<ModelKeyframe> frames;
						frames.reserve(frameCount);

						for (int j=0;j<frameCount;j++)
						{
							int32 bone = br->ReadInt32();

							float totalSec = static_cast<float>(br->ReadDouble());
							Matrix transfrom;
							br->ReadMatrix(transfrom);

							frames.push_back(ModelKeyframe(bone, totalSec, transfrom));
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
					int count = data->GetDataInt32(TAG_3_RootAnimationClipCountTag);

					m_rootAnimationClips.rehash(count);

					BinaryReader* br = data->GetData(TAG_3_RootAnimationClipTag);
					for (int i=0;i<count;i++)
					{
						String key = br->ReadString();


						float duration = static_cast<float>(br->ReadDouble());

						int frameCount = br->ReadInt32();

						vector<ModelKeyframe> frames;
						frames.reserve(frameCount);

						for (int j=0;j<frameCount;j++)
						{
							int32 bone = br->ReadInt32();

							float totalSec = static_cast<float>(br->ReadDouble());
							Matrix transfrom;
							br->ReadMatrix(transfrom);

							frames.push_back(ModelKeyframe(bone, totalSec, transfrom));
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
					
					m_skeletonHierarchy.reserve(count);

					BinaryReader* br = data->GetData(TAG_3_BoneHierarchyTag);
					for (int i=0;i<count;i++)
					{
						m_skeletonHierarchy.push_back(br->ReadInt32());
					}
					br->Close();
					delete br;
				}

			}

			TaggedDataWriter* AnimationData::Save()
			{
				if (m_hasBindPose)
				{

				}
				
				if (m_hasSkeleton)
				{

				}
			}
		}
	}
}