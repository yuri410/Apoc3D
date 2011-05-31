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
#ifndef ANIMATIONDATA_H
#define ANIMATIONDATA_H

#include "Common.h"
#include "Math/Matrix.h"

using namespace std;

using namespace Apoc3D::Math;
using namespace Apoc3D::IO;

namespace Apoc3D
{
	namespace Graphics
	{
		namespace Animation
		{
			template class APAPI unordered_map<String, ModelAnimationClip*>;
			template class APAPI unordered_map<String, MaterialAnimationClip*>;

			/** Combines all the animation data needed to animate a object.
			*/
			class APAPI AnimationData
			{
			public:
				typedef unordered_map<String, ModelAnimationClip*> ClipTable;
				typedef unordered_map<String, MaterialAnimationClip*> MtrlClipTable;
			private:
				ClipTable m_rootAnimationClips;
				ClipTable m_modelAnimationClips;
				MtrlClipTable m_mtrlAnimationClips;

				vector<Matrix> m_bindPose;
				vector<Matrix> m_invBindPose;
				vector<int32> m_skeletonHierarchy;

				bool m_hasBindPose;
				bool m_hasSkeleton;
				bool m_hasMtrlClip;
				bool m_hasModelClip;
				bool m_hasRootClip;

				void LoadMtrlAnimation2(TaggedDataReader* data);
				TaggedDataWriter* SaveMtrlAnimation2();

				//void LoadMtrlAnimation3(TaggedDataReader* data);
				//TaggedDataWriter* SaveMtrlAnimation3();
			public:
				/** Gets a collection of animation clips that operate on the root of the object.
					These are stored by name in a map, so there could for instance be 
					clips for "Walk", "Run", "JumpReallyHigh", etc.
				*/
				const ClipTable& getRootAnimationClips() const { return m_rootAnimationClips; }
				/** Gets a collection of model animation clips. These are stored by name in a
					map, so there could for instance be clips for "Walk", "Run",
					"JumpReallyHigh", etc.
				*/
				const ClipTable& getModelAnimationClips() const { return m_modelAnimationClips; }
				
				const MtrlClipTable& getMaterialAnimationClips() const { return m_mtrlAnimationClips; }

				/** Bindpose matrices for each bone in the skeleton,
					relative to the parent bone.
				*/
				const vector<Matrix>& getBindPose() const { return m_bindPose; }
				/** Vertex to bonespace transforms for each bone in the skeleton.
				*/
				const vector<Matrix>& getInvBindPose() const { return m_invBindPose; }
				/** For each bone in the skeleton, stores the index of the parent bone.
				*/
				const vector<int32>& getSkeletonHierarchy() const { return m_skeletonHierarchy; }

				/** Load animation data from a Tagged Data Block
				*/
				void Load(TaggedDataReader* data);
				TaggedDataWriter* Save();

				AnimationData()
					: m_hasBindPose(false), m_hasSkeleton(false),
					  m_hasModelClip(false), m_hasRootClip(false), m_hasMtrlClip(false)
				{

				}

				//AnimationData(
				//	const ClipTable& modelAnimationClips, 
				//	const ClipTable& rootAnimationClips,
				//	const MtrlClipTable& mtrlAnimationClip,
				//	const vector<Matrix>& bindPose,
				//	const vector<Matrix>& inverseBindPose,
				//	const vector<int32>& skeletonHierarchy)
				//	: m_modelAnimationClips(m_modelAnimationClips),m_rootAnimationClips(rootAnimationClips),
				//	m_mtrlAnimationClips(mtrlAnimationClip), m_bindPose(bindPose), m_invBindPose(inverseBindPose),
				//	m_skeletonHierarchy(skeletonHierarchy)
				//{

				//}

				~AnimationData(){}
			};
		}
	}
}

#endif