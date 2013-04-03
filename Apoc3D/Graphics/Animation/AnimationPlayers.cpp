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

				const FastList<ModelKeyframe>& keyframes = m_currentClipValue->getKeyframes();

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

			void ModelAnimationPlayerBase::Update(const GameTime* const time)
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
					if (!m_eventCompeleted.empty())
						m_eventCompeleted();

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
				const FastList<ModelKeyframe>& keyframes = clip->getKeyframes();

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

			void MaterialAnimationPlayer::SetKeyframe(const MaterialAnimationKeyframe& keyframe)
			{
				m_currentFrame = keyframe.getMaterialFrame();
			}

			void MaterialAnimationPlayer::setCurrentTimeValue(float value)
			{
				float time = value;

				// If the position moved backwards, reset the keyframe index.
				if (time < m_currentTimeValue)
				{
					m_currentTimeValue = 0;
					InitClip();
					if (m_currentClipValue && m_currentClipValue->Keyframes.getCount()>0)
					{
						m_currentFrame = 0;
					}
				}

				m_currentTimeValue = time;

				const FastList<MaterialAnimationKeyframe>& keyframes = m_currentClipValue->Keyframes;

				while (m_currentKeyframe < keyframes.getCount())
				{
					const MaterialAnimationKeyframe& keyframe = keyframes[m_currentKeyframe];

					if (keyframe.getTime()>m_currentTimeValue)
					{
						break;
					}

					// Use this keyframe
					SetKeyframe(keyframe);

					m_currentKeyframe++;
				}
			}

			void MaterialAnimationPlayer::Update(const GameTime* const gameTime)
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
					if (!m_eventCompeleted.empty())
						m_eventCompeleted();

					m_currentClipValue = 0;

					return;
				}

				// Update the animation position. 
				t += m_currentTimeValue;

				// If we reached the end, loop back to the start.
				while (t >= m_currentClipValue->Duration)
					t -= m_currentClipValue->Duration;

				setCurrentTimeValue(t);

			}
		}
	}
}