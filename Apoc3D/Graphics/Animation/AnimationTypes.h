#pragma once
#ifndef APOC3D_ANIMATIONTYPES_H
#define APOC3D_ANIMATIONTYPES_H

/* -----------------------------------------------------------------------
 * This source file is part of Apoc3D Framework
 * 
 * Copyright (c) 2009-2018 Tao Xin
 * 
 * This content of this file is subject to the terms of the Mozilla Public 
 * License v2.0. If a copy of the MPL was not distributed with this file, 
 * you can obtain one at http://mozilla.org/MPL/2.0/.
 * 
 * This program is distributed in the hope that it will be useful, 
 * WITHOUT WARRANTY OF ANY KIND; either express or implied. See the 
 * Mozilla Public License for more details.
 * 
 * ------------------------------------------------------------------------
 */

#include "apoc3d/Math/Matrix.h"
#include "apoc3d/Collections/List.h"

using namespace Apoc3D::Collections;
using namespace Apoc3D::Math;

namespace Apoc3D
{
	namespace Graphics
	{
		namespace Animation
		{
			/** 
			 *  Defines a bone in model. 
			 *
			 *  Bind poses for animation purposes are stored with this class.
			 */
			class APAPI Bone
			{
			public:
				String Name;
				int32 Parent;
				List<int32> Children;
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
					m_boneReferenceTransform = value;
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
				Bone(int32 index, const Matrix& transform, const List<int32>& children, int32 parent, const String& name)
					: Index(index), m_bindPoseTransfrom(transform), Children(children), Parent(parent), Name(name)
				{
					Matrix::Inverse(m_invBindPoseTransfrom, transform);
					m_invBoneReferenceTransform.LoadIdentity();
				}

				~Bone() { }

			private:
				Matrix m_bindPoseTransfrom;
				Matrix m_boneReferenceTransform;
				Matrix m_invBindPoseTransfrom;
				Matrix m_invBoneReferenceTransform;

			};



			/**
			 *  Describes the position of a single animated object(bone, mesh) at a single point in time.
			 * 
			 *  The build engine will flatten the animation frames, each keyframe here
			 *  only represents one transform for one entity(bone, mesh) at a specific time.
			 *
			 *  Unlike regular keyframe definition, each keyframe have multiple transformation
			 *  for multiple entities. The build engine will split 'regular keyframe' 
			 *  into the keyframes with a same time. 
			 */
			class APAPI ModelKeyframe
			{
			public:
				/** 
				 *  Gets the index of the target bone(skinned) or mesh(rigid) that is animated by this keyframe.
				 */
				int32 getObjectIndex() const { return m_objIndex; }

				/**
				 *  Gets the time offset from the start of the animation to this keyframe.
				 */
				float getTime() const { return m_time; }

				/**
				 *  Gets the bone transform for this keyframe.
				 */
				const Matrix& getTransform() const { return m_transform; }

				/**
				 *  Gets the next frame index of the animation for this animated object. 
				 *  The value can be used for interpolation between frames.
				 *  A value of -1 means the player cannot use interpolation for this object right now.
				 */
				int32 getNextFrameIndex() const
				{
					return m_nextFrameIndex;
				}

				ModelKeyframe(int32 objIndex, float time, const Matrix& transform)
					: m_objIndex(objIndex), m_time(time), m_transform(transform), m_nextFrameIndex(-1)
				{

				}
				ModelKeyframe() { }
				~ModelKeyframe() { }

				void setNextFrameIndex(int32 idx)
				{
					m_nextFrameIndex = idx;
				}

			private:
				int32 m_nextFrameIndex;
				int32 m_objIndex;
				float m_time;
				Matrix m_transform;

			};

			/**
			 *  Describes the material key frame at a single point in time.
			 *  For more information about material key frame, please see Mesh.
			 */
			class APAPI MaterialAnimationKeyframe
			{
			public:
				enum MaterialKeyframeFlags
				{
					MKF_None = 0,
					MKF_Hidden = 1
				};

				float getTime() const { return m_time; }
				int32 getMaterialFrame() const { return m_materialFrame; }
				uint32 getFlags() const { return m_flags; }

				MaterialAnimationKeyframe(float time, int32 mtrlID, uint32 flags = 0)
					: m_time(time), m_materialFrame(mtrlID), m_flags(flags)
				{
				}
				MaterialAnimationKeyframe() { }

			private:
				float m_time;
				int32 m_materialFrame;

				uint32 m_flags;
			};

			/**
			 *  A model animation clip holds all the keyframes needed to describe a single model animation.
			 */
			class APAPI ModelAnimationClip
			{
			public:
				ModelAnimationClip(float duration, const List<ModelKeyframe>& keyframes)
					: m_duration(duration), m_keyFrames(keyframes)
				{

				}
				~ModelAnimationClip() { }

				void Transform(const Matrix& t);
				
				/**
				 *  Gets the total length of the model animation clip
				 */
				float getDuration() const { return m_duration; }
				/**
				 *  Gets a combined list containing all the keyframes for all bones,
				 *  sorted by time.
				 */
				const List<ModelKeyframe>& getKeyframes() const { return m_keyFrames; }

			private:
				float m_duration;
				List<ModelKeyframe> m_keyFrames;

			};

			//template class APAPI vector<MaterialAnimationKeyframe>;

			class APAPI MaterialAnimationClip
			{
			public:
				float Duration;
				List<MaterialAnimationKeyframe> Keyframes;

				MaterialAnimationClip(float duration, const List<MaterialAnimationKeyframe>& keyframes)
					: Duration(duration), Keyframes(keyframes)
				{

				}
				~MaterialAnimationClip() { }
			};

		}
	}
}

#endif
