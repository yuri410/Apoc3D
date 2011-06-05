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
			template class APAPI vector<int32>;

			class APAPI Bone
			{
			private:

			public:
				Matrix Transfrom;
				String Name;
				int32 Parent;
				FastList<int32> Children;
				int32 Index;

				Bone() { }
				Bone(int32 index)
					: Index(index), Parent(-1)
				{
					Transfrom.LoadIdentity();
				}
				Bone(int32 index, const Matrix& transform, const FastList<int32>& children, int32 parent, const String& name)
					: Index(index), Transfrom(transform), Children(children), Parent(parent), Name(name)
				{

				}

				~Bone() { }
			};
			/** Describes the position of a single bone at a single point in time.
			*/
			class APAPI ModelKeyframe
			{
			private:
				int32 m_bone;
				float m_time;
				Matrix m_transform;

			public:
				/** Gets the index of the target bone that is animated by this keyframe.
				*/
				int32 getBone() const { return m_bone; }

				/** Gets the time offset from the start of the animation to this keyframe.
				*/
				float getTime() const { return m_time; }

				/** Gets the bone transform for this keyframe.
				*/
				const Matrix& getTransform() const { return m_transform; }

				ModelKeyframe(int32 bone, float time, const Matrix& transform)
					: m_bone(bone), m_time(time), m_transform(transform)
				{

				}
				ModelKeyframe() { }
				~ModelKeyframe() { }
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

			template class APAPI vector<MaterialAnimationKeyframe>;

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
