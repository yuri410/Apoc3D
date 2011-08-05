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
#ifndef ANIMATIONTYPES_H
#define ANIMATIONTYPES_H

#include "Common.h"
#include "Math/Matrix.h"
#include "Collections/FastList.h"

using namespace Apoc3D::Collections;
using namespace Apoc3D::Math;

using namespace std;

namespace Apoc3D
{
	namespace Graphics
	{
		namespace Animation
		{
			//template class APAPI vector<int32>;

			class APAPI Bone
			{
			private:
				Matrix m_bindPoseTransfrom;
				Matrix m_boneReferenceTransform;
				Matrix m_invBindPoseTransfrom;
				Matrix m_invBoneReferenceTransform;

			public:
				String Name;
				int32 Parent;
				FastList<int32> Children;
				int32 Index;

				const Matrix& getBindPoseTransform() const { return m_bindPoseTransfrom; }
				const Matrix& getInvBindPoseTransform() const { return m_invBindPoseTransfrom; }
				const Matrix& getBoneReferenceTransform() const { return m_boneReferenceTransform; }
				const Matrix& getInvBoneReferenceTransform() const { return m_invBoneReferenceTransform; }


				void setBindPoseTransform(const Matrix& value)
				{
					m_bindPoseTransfrom = value;
					Matrix::Inverse(m_invBindPoseTransfrom, value);
				}
				void setBoneReferenceTransform(const Matrix& value)
				{
					m_invBindPoseTransfrom = value;
					Matrix::Inverse(m_invBoneReferenceTransform, value);
				}


				Bone() 
				{
				}
				Bone(int32 index)
					: Index(index), Parent(-1)
				{
					m_bindPoseTransfrom.LoadIdentity();
					m_invBindPoseTransfrom.LoadIdentity();
					m_boneReferenceTransform.LoadIdentity();
					m_invBoneReferenceTransform.LoadIdentity();
				}
				Bone(int32 index, const Matrix& transform, const FastList<int32>& children, int32 parent, const String& name)
					: Index(index), m_bindPoseTransfrom(transform), Children(children), Parent(parent), Name(name)
				{
					m_boneReferenceTransform.LoadIdentity();
					m_invBoneReferenceTransform.LoadIdentity();
				}

				~Bone() { }
			};



			/** Describes the position of a single animated object(bone, mesh) at a single point in time.
				The build engine will flatten the animation frames, each keyframe here
				only represents one transform for one entity(bone, mesh) at a specific time.
				Unlike regular keyframe definition, each keyframe have multiple transformation
				for multiple entities. The build engine will split 'regular keyframe' 
				into the keyframes with a same time.
			*/
			class APAPI ModelKeyframe
			{
			private:
				int32 m_nextFrameIndex;
				int32 m_objIndex;
				float m_time;
				Matrix m_transform;
				
			public:
				/** Gets the index of the target bone(skinned) or mesh(rigid) that is animated by this keyframe.
				*/
				int32 getObjectIndex() const { return m_objIndex; }

				/** Gets the time offset from the start of the animation to this keyframe.
				*/
				float getTime() const { return m_time; }

				/** Gets the bone transform for this keyframe.
				*/
				const Matrix& getTransform() const { return m_transform; }

				/** Gets the next frame index of the animation for this animated object. 
					The value can be used for interpolation between frames.
					A value of -1 means the player cannot use interpolation for this object right now.
				*/
				int32 getNextFrameIndex() const
				{
					return m_nextFrameIndex;
				}

				ModelKeyframe(int32 index, float time, const Matrix& transform)
					: m_objIndex(index), m_time(time), m_transform(transform), m_nextFrameIndex(-1)
				{

				}
				ModelKeyframe() { }
				~ModelKeyframe() { }

				void setNextFrameIndex(int32 idx)
				{
					m_nextFrameIndex = idx;
				}
			};

			/** Describes the material frame at a single point in time.
			*/
			class APAPI MaterialAnimationKeyframe
			{
			private:
				float m_time;
				int32 m_materialFrame;

			public:
				float getTime() const { return m_time; }
				int32 getMaterialFrame() const { return m_materialFrame; }

				MaterialAnimationKeyframe(float time, int32 mtrlID)
					: m_time(time), m_materialFrame(mtrlID)
				{
				}
				MaterialAnimationKeyframe() { }
			};

			template class vector<ModelKeyframe>;

			/** A model animation clip holds all the keyframes needed to describe a single model animation.
			*/
			class APAPI ModelAnimationClip
			{
			private:
				float m_duration;
				FastList<ModelKeyframe> m_keyFrames;

			public:
				/** Gets the total length of the model animation clip
				*/
				float getDuration() const { return m_duration; }
				/** Gets a combined list containing all the keyframes for all bones,
					sorted by time.
				*/
				const FastList<ModelKeyframe>& getKeyframes() const { return m_keyFrames; }

				ModelAnimationClip(float duration, const FastList<ModelKeyframe>& keyframes)
					: m_duration(duration), m_keyFrames(keyframes)
				{

				}
				~ModelAnimationClip() { }
			};

			//template class APAPI vector<MaterialAnimationKeyframe>;

			class APAPI MaterialAnimationClip
			{
			public:
				float Duration;
				FastList<MaterialAnimationKeyframe> Keyframes;

				MaterialAnimationClip(float duration, const FastList<MaterialAnimationKeyframe>& keyframes)
					: Duration(duration), Keyframes(keyframes)
				{

				}
				~MaterialAnimationClip() { }
			};

		}
	}
}

#endif
