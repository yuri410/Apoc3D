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

#ifndef ANIMATIONPLAYERS_H
#define ANIMATIONPLAYERS_H

#include "Common.h"
#include "Math/Matrix.h"
#include "Collections/FastList.h"
#include "AnimationTypes.h"

using namespace Apoc3D::Collections;
using namespace Apoc3D::Core;
using namespace Apoc3D::Math;

namespace Apoc3D
{
	namespace Graphics
	{
		namespace Animation
		{
			typedef fastdelegate::FastDelegate0<void> AnimationCompeletedHandler;
			/** This class serves as a base class for various animation players.  It contains
				common functionality to deal with a clip, playing it back at a speed, 
				notifying clients of completion, etc.
			*/
			class APAPI ModelAnimationPlayerBase
			{
			private:
				/** Clip currently being played
				*/
				const ModelAnimationClip* m_currentClipValue;

				/** Current timeindex and keyframe in the clip
				*/
				float m_currentTimeValue;
				int m_currentKeyframe;

				/** Speed of playback
				*/
				float m_playbackRate;

				/** The amount of time for which the animation will play.
				*/
				float m_duration;

				/** Amount of time elapsed while playing
				*/
				float m_elapsedPlaybackTime;

				/** Whether or not playback is paused
				*/
				bool m_paused;
				
				AnimationCompeletedHandler m_eventCompeleted;
			protected:
				/** Virtual method allowing subclasses to do any initialization of data when the clip is initialized.
				*/
				virtual void InitClip() { }

				/** Virtual method allowing subclasses to set any data associated with a particular keyframe.
				*/
				virtual void SetKeyframe(const ModelKeyframe& keyframe) { }

				/** Virtual method allowing subclasses to perform data needed after the animation 
					has been updated for a new time index.
				*/
				virtual void OnUpdate() { }

				ModelAnimationPlayerBase()
					: m_currentClipValue(0), m_currentTimeValue(0), m_currentKeyframe(0), m_playbackRate(0),
					m_duration(0), m_elapsedPlaybackTime(0), m_paused(false)
				{

				}
			public:
				/** Invoked when playback has completed.
				*/
				AnimationCompeletedHandler& eventCompleted() { return m_eventCompeleted; }

				/** Gets the clip currently being decoded.
				*/
				const ModelAnimationClip* getCurrentClip() const { return m_currentClipValue; }

				/** Gets the current key frame index
				*/
				int getCurrentKeyframe() const { return m_currentKeyframe; }
				/** Sets the current key frame index
				*/
				void setCurrentKeyframe(int value)
				{
					const FastList<ModelKeyframe>& keyframes = m_currentClipValue->getKeyframes();
					float time = keyframes[value].getTime();
					setCurrentTimeValue( time );
				}

				/** Gets the current play position.
				*/
				float getCurrentTimeValue() const { return m_currentTimeValue; }
				/** Set the current play position.
				*/
				void setCurrentTimeValue(float value);

				/** Starts playing a clip
				*/
				void StartClip(const ModelAnimationClip* clip, float playbackRate = 1.0f, float duration = 9999999)
				{
					assert(clip);

					m_currentClipValue = clip;
					m_currentKeyframe = 0;
					setCurrentTimeValue(0);
					m_elapsedPlaybackTime = 0;
					m_paused = false;

					m_playbackRate = playbackRate;
					m_duration = duration;

					InitClip();
				}

				/** Will pause the playback of the current clip
				*/
				void PauseClip()
				{
					m_paused = true;
				}

				/** Will resume playback of the current clip
				*/
				void ResumeClip()
				{
					m_paused = false;
				}

				/** Called during the update loop to move the animation forward
				*/
				virtual void Update(const GameTime* const time);

				virtual void GetTransform(int boneID, Matrix& result) = 0;
			};

			/** The animation player contains a single transformation that is used to move/position/scale
				something.
			*/
			class APAPI RootAnimationPlayer : public ModelAnimationPlayerBase
			{
			private:
				Matrix m_currentTransfrom;

			protected:
				/** Initializes the transformation to the identity
				*/
				virtual void InitClip();

				/** Sets the key frame by storing the current transform
				*/
				virtual void SetKeyframe(const ModelKeyframe& keyframe);
				
			public:
				RootAnimationPlayer()
				{
					m_currentTransfrom.LoadIdentity();
				}
				/** Gets the current transformation being applied
				*/
				void GetCurrentTransform(Matrix& result)
				{
					result = m_currentTransfrom;
				}

				virtual void GetTransform(int boneID, Matrix& result)
				{
					result = m_currentTransfrom;
				}
			};

			/** This animation player knows how to play an animation on a rigid model, applying transformations
				to each of the objects in the model over time
			*/
			class APAPI RigidAnimationPlayer : public ModelAnimationPlayerBase
			{
			private:
				/** This is an array of the transforms to each object in the model
				*/
				Matrix* m_boneTransforms;
				int32 m_boneTransformCount;

				/** Initializes all the bone transforms to the identity
				*/
				virtual void InitClip()
				{
					for (int i=0;i<m_boneTransformCount;i++)
						m_boneTransforms[i].LoadIdentity();
				}

				/** Sets the key frame for a bone to a transform
				*/
				virtual void SetKeyframe(const ModelKeyframe& keyframe)
				{
					m_boneTransforms[keyframe.getBone()] = keyframe.getTransform();
				}
			public:
				/** Create a new rigid animation player
				*/
				RigidAnimationPlayer(int count)
					: m_boneTransformCount(count)
				{
					m_boneTransforms = new Matrix[count];
				}
				~RigidAnimationPlayer()
				{
					delete[] m_boneTransforms;
				}

				/** Gets the current bone transform matrices for the animation
				*/
				const Matrix* GetBoneTransforms() const { return m_boneTransforms; }

				int32 getBoneTransformCount() const { return m_boneTransformCount; }

				virtual void GetTransform(int boneID, Matrix& result)
				{
					if (boneID !=-1)
					{
						result = m_boneTransforms[boneID];
					}
					result.LoadIdentity();
				}
			};

			/** The animation player manipulates a skinned model
			*/
			class APAPI SkinnedAnimationPlayer : public ModelAnimationPlayerBase
			{
			private:
				bool m_isFirstPlay;
				Matrix* m_boneTransforms;
				Matrix* m_worldTransforms;
				Matrix* m_skinTransforms;

				const FastList<Matrix>* m_bindPose;
				const FastList<Matrix>* m_inverseBindPose;
				const FastList<int32>* m_skeletonHierarchy;

			protected:
				/** Initializes the animation clip
				*/
				virtual void InitClip()
				{
					if (m_isFirstPlay)
					{
						memcpy(m_boneTransforms, m_bindPose->getInternalPointer(), m_bindPose->getCount() * sizeof(Matrix));
						m_isFirstPlay = false;
					}
				}

				/** Sets the key frame for the passed in frame
				*/
				virtual void SetKeyframe(const ModelKeyframe& keyframe);

				/** Updates the transformations ultimately needed for rendering
				*/
				virtual void OnUpdate();
			public:
				SkinnedAnimationPlayer(const FastList<Matrix>* bindPose, 
					const FastList<Matrix>* inverseBindPose, 
					const FastList<int32>* skeletonHierarchy)
					: m_bindPose(bindPose), m_inverseBindPose(inverseBindPose), 
					m_skeletonHierarchy(skeletonHierarchy), 
					m_isFirstPlay(true)
				{
					m_boneTransforms = new Matrix[bindPose->getCount()];
					memset(m_boneTransforms, 0, sizeof(Matrix) * bindPose->getCount());

					m_worldTransforms = new Matrix[bindPose->getCount()];
					memset(m_worldTransforms, 0, sizeof(Matrix) * bindPose->getCount());

					m_skinTransforms = new Matrix[bindPose->getCount()];
					memset(m_worldTransforms, 0, sizeof(Matrix) * bindPose->getCount());
				}
				~SkinnedAnimationPlayer()
				{
					delete[] m_boneTransforms;
					delete[] m_worldTransforms;
					delete[] m_skinTransforms;
				}

				int32 getTransformCount() const { return m_bindPose->getCount(); }

				/** Gets the current bone transform matrices, relative to their parent bones.
				*/
				const Matrix* GetBoneTransform() const { return m_boneTransforms; }

				/** Gets the current bone transform matrices, in absolute format.
				*/
				const Matrix* GetWorldTransform() const { return m_worldTransforms; }

				/** Gets the current bone transform matrices, relative to the skinning bind pose.
				*/
				const Matrix* GetSkinTransforms() const { return m_skinTransforms; }

				virtual void GetTransform(int boneID, Matrix& result)
				{
					result.LoadIdentity();
				}
			};

			/**
			*/
			class APAPI MaterialAnimationPlayer
			{
			private:
				/** Clip currently being played
				*/
				const MaterialAnimationClip* m_currentClipValue;

				/** Current timeindex and keyframe in the clip
				*/
				float m_currentTimeValue;
				int m_currentKeyframe;

				/** Speed of playback
				*/
				float m_playbackRate;

				/** The amount of time for which the animation will play.
				*/
				float m_duration;

				/** Amount of time elapsed while playing
				*/
				float m_elapsedPlaybackTime;

				/** Whether or not playback is paused
				*/
				bool m_paused;
				
				AnimationCompeletedHandler m_eventCompeleted;

				int m_currentFrame;


				void InitClip() { }
				inline void SetKeyframe(const MaterialAnimationKeyframe& keyframe);
				
				virtual void Update(const GameTime* const gameTime);

			public:
				int getCurrentMaterialFrame() const { return m_currentFrame; }
				/** Invoked when playback has completed.
				*/
				AnimationCompeletedHandler& eventCompleted() { return m_eventCompeleted; }

				/** Gets the clip currently being decoded.
				*/
				const MaterialAnimationClip* getCurrentClip() const { return m_currentClipValue; }

				/** Gets the current key frame index
				*/
				int getCurrentKeyframe() const { return m_currentKeyframe; }
				/** Sets the current key frame index
				*/
				void setCurrentKeyframe(int value)
				{
					const FastList<MaterialAnimationKeyframe>& keyframes = m_currentClipValue->Keyframes;
					float time = keyframes[value].getTime();
					setCurrentTimeValue( time );
				}

				/** Gets the current play position.
				*/
				float getCurrentTimeValue() const { return m_currentTimeValue; }
				/** Set the current play position.
				*/
				void setCurrentTimeValue(float value);

				MaterialAnimationPlayer()
					: m_currentClipValue(0), m_currentTimeValue(0), m_currentKeyframe(0), m_playbackRate(0),
					m_duration(0), m_elapsedPlaybackTime(0), m_paused(false), m_currentFrame(0)
				{

				}

				/** Starts playing a clip
				*/
				void StartClip(const MaterialAnimationClip* clip, float playbackRate = 1, float duration = 9999999)
				{
					assert(clip);

					m_currentClipValue = clip;
					m_currentKeyframe = 0;
					setCurrentTimeValue(0);
					m_elapsedPlaybackTime = 0;
					m_paused = false;

					m_playbackRate = playbackRate;
					m_duration = duration;

					InitClip();
				}

				/** Will pause the playback of the current clip
				*/
				void PauseClip()
				{
					m_paused = true;
				}

				/** Will resume playback of the current clip
				*/
				void ResumeClip()
				{
					m_paused = false;
				}



			};
		}
	}
}
#endif