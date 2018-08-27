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

#include "AnimationPlayers.h"
#include "apoc3d/Core/GameTime.h"

using namespace Apoc3D::Collections;

namespace Apoc3D
{
	namespace Graphics
	{
		namespace Animation
		{
			void ModelAnimationPlayerBase::setCurrentTimeValue(float value)
			{
				float time = value;

				// If the position moved backwards, reset the keyframe index.
				if (time < m_currentTimeValue)
				{
					m_currentTimeValue = 0;
					InitClip();
				}

				m_currentTimeValue = time;

				const List<ModelKeyframe>& keyframes = m_currentClipValue->getKeyframes();

				while (m_currentKeyframe < keyframes.getCount())
				{
					const ModelKeyframe& keyframe = keyframes[m_currentKeyframe];

					if (keyframe.getTime()>m_currentTimeValue)
					{
						break;
					}

					// Use this keyframe
					SetKeyframe(keyframe);

					m_currentKeyframe++;
				}
			}

			void ModelAnimationPlayerBase::Update(const GameTime* time)
			{
				if (!m_currentClipValue)
					return;

				if (m_paused)
					return;

				float t = time->getElapsedTime();

				// Adjust for the rate
				if (m_playbackRate != 1.0f)
					t *= m_playbackRate;

				m_elapsedPlaybackTime += t;

				// See if we should terminate
				if (m_elapsedPlaybackTime >= m_duration && m_duration != 0 ||
					m_elapsedPlaybackTime >= m_currentClipValue->getDuration() && m_duration == 0)
				{
					if (!eventCompleted.empty())
						eventCompleted();

					m_currentClipValue = 0;

					return;
				}

				// Update the animation position. 
				t += m_currentTimeValue;

				// If we reached the end, loop back to the start.
				while (t >= m_currentClipValue->getDuration())
					t -= m_currentClipValue->getDuration();

				setCurrentTimeValue(t);

				OnUpdate();
			}

			void RootAnimationPlayer::InitClip(bool newClip)
			{
				const ModelAnimationClip* clip = getCurrentClip();
				const List<ModelKeyframe>& keyframes = clip->getKeyframes();

				m_currentTransfrom = keyframes.getCount() > 0 ? keyframes[0].getTransform() : Matrix::Identity;
			}
			void RootAnimationPlayer::SetKeyframe(const ModelKeyframe& keyframe)
			{
				m_currentTransfrom = keyframe.getTransform();
			}

			/************************************************************************/
			/*                                                                      */
			/************************************************************************/

			void RigidAnimationPlayer::InitClip(bool newClip)
			{
				if (newClip)
				{
					for (int i=0;i<m_meshTransformCount;i++)
					{
						m_initialTransforms[i].LoadIdentity();

						const ModelAnimationClip* clip = getCurrentClip();
						for (int j=0;j<clip->getKeyframes().getCount();j++)
						{
							if (clip->getKeyframes()[j].getObjectIndex() == i)
							{
								m_initialTransforms[i] = clip->getKeyframes()[j].getTransform();
								break;
							}
						}
					}
				}
				else
				{
					memcpy(m_meshTransforms, m_initialTransforms, sizeof(Matrix) * m_meshTransformCount);
				}
			}

			/************************************************************************/
			/*                                                                      */
			/************************************************************************/


			void SkinnedAnimationPlayer::InitClip(bool newClip /* = false */)
			{
				if (newClip)
				{
					for (int i=0;i<m_bones->getCount();i++)
					{
						m_boneTransforms[i] = m_bones->operator[](i).getBindPoseTransform();
					}
					//memcpy(m_boneTransforms, m_bindPose->getInternalPointer(), m_bindPose->getCount() * sizeof(Matrix));

				}
			}
			void SkinnedAnimationPlayer::SetKeyframe(const ModelKeyframe& keyframe)
			{
				m_boneTransforms[keyframe.getObjectIndex()] = keyframe.getTransform();
			}
			void SkinnedAnimationPlayer::OnUpdate()
			{
				if (getCurrentClip())
				{
					//// Root bone.
					//m_worldTransforms[0] = m_boneTransforms[0];

					//Matrix::Multiply(m_skinTransforms[0], m_bones->operator[](0).getInvBindPoseTransform(), m_worldTransforms[0]);

					//// Child bones.
					for (int bone = 0; bone < getTransformCount(); bone++)
					{
						const Bone& b = m_bones->operator[](bone);
						const Matrix& matAnimation = m_boneTransforms[bone];
						// TODO: QuaternionKeyframe slerp
						int parentBone = b.Parent;

						if (parentBone<0)
							m_worldTransforms[bone] = matAnimation;
						else
							Matrix::Multiply(m_worldTransforms[bone], matAnimation, m_worldTransforms[parentBone]);

						Matrix tmp;
						Matrix::Multiply(tmp, 
							b.getBoneReferenceTransform(), 
							b.getInvBindPoseTransform());

						Matrix::Multiply(m_skinTransforms[bone], tmp, m_worldTransforms[bone]);
					}
				}
			}

			void MaterialAnimationPlayer::setCurrentKeyframe(int value)
			{
				const List<MaterialAnimationKeyframe>& keyframes = m_currentClipValue->Keyframes;
				float time = keyframes[value].getTime();
				m_currentTimeValue = time;

				SetKeyframe(keyframes[value]);
				m_currentKeyframe = value;
			}


			void MaterialAnimationPlayer::Update(const GameTime* gameTime)
			{
				if (!m_currentClipValue)
					return;

				if (m_paused)
					return;

				float t = gameTime->getElapsedTime();

				// Adjust for the rate
				if (m_playbackRate != 1.0f)
					t *= m_playbackRate;

				m_elapsedPlaybackTime += t;

				// See if we should terminate
				if (m_elapsedPlaybackTime >= m_duration && m_duration != 0 ||
					m_elapsedPlaybackTime >= m_currentClipValue->Duration && m_duration == 0)
				{
					if (!eventCompleted.empty())
						eventCompleted();

					m_currentClipValue = 0;

					return;
				}

				// Update the animation position. 
				t += m_currentTimeValue;

				// If we reached the end, loop back to the start.
				while (t >= m_currentClipValue->Duration)
					t -= m_currentClipValue->Duration;

				if (t<m_currentTimeValue)
				{
					m_currentKeyframe = 0;
				}

				for (int i = m_currentKeyframe;i<m_currentClipValue->Keyframes.getCount();i++)
				{
					const MaterialAnimationKeyframe& kf = m_currentClipValue->Keyframes[i];

					if (kf.getTime()>=t && (kf.getFlags() & MaterialAnimationKeyframe::MKF_Hidden) == 0)
					{
						m_currentKeyframe = i;
						SetKeyframe(kf);
						break;
					}
				}
				m_currentTimeValue = t;
			}

			void MaterialAnimationPlayer::SetKeyframe(const MaterialAnimationKeyframe& keyframe)
			{
				m_currentMaterialFrame = keyframe.getMaterialFrame();
			}
		}
	}
}