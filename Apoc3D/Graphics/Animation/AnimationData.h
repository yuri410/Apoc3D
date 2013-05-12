#pragma once
#ifndef APOC3D_ANIMATIONDATA_H
#define APOC3D_ANIMATIONDATA_H

/**
 * -----------------------------------------------------------------------------
 * This source file is part of Apoc3D Engine
 * 
 * Copyright (c) 2009+ Tao Xin
 * 
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 * 
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 * 
 * You should have received a copy of the GNU General Public License
 * along with this program.  if not, write to the Free Software Foundation, 
 * Inc., 59 Temple Place - Suite 330, Boston, MA 02111-1307, USA, or go to
 * http://www.gnu.org/copyleft/gpl.txt.
 * 
 * -----------------------------------------------------------------------------
 */

#include "AnimationTypes.h"
#include "apoc3d/Collections/HashMap.h"

using namespace Apoc3D::Collections;
using namespace Apoc3D::Math;
using namespace Apoc3D::IO;
using namespace Apoc3D::VFS;

namespace Apoc3D
{
	namespace Graphics
	{
		namespace Animation
		{
			/**
			 *  Combines all the animation data needed to animate a object.
			 */
			class APAPI AnimationData
			{
			public:
				typedef HashMap<String, ModelAnimationClip*> ClipTable;
				typedef HashMap<String, MaterialAnimationClip*> MtrlClipTable;
			public:
				int RigidEntityCount;
				//bool hasBindPose() const { return m_hasBindPose; }
				//bool hasSkeleton() const { return m_hasSkeleton; }

				bool hasMtrlClip() const { return m_hasMtrlClip; }
				bool hasSkinnedClip() const { return m_hasSkinnedClip; }
				bool hasRigidClip() const { return m_hasRigidClip; }

				const List<Bone>& getBones() const { return m_bones; }
				const int32 getRootBone() const { return m_rootBone; }

				/**
				 *  Gets a collection of animation clips that operate a whole mesh entity in a model.
				 *  These are stored by name in a map, so there could for instance be 
				 *  clips for "Walk", "Run", "JumpReallyHigh", etc.
				 */
				const ClipTable& getRigidAnimationClips() const { return m_rigidAnimationClips; }
				/** Gets a collection of model animation clips. These are stored by name in a
				 *  map, so there could for instance be clips for "Walk", "Run",
				 *  "JumpReallyHigh", etc.
				 */
				const ClipTable& getSkinnedAnimationClips() const { return m_skinnedAnimationClips; }
				
				const MtrlClipTable& getMaterialAnimationClips() const { return m_mtrlAnimationClips; }

				const List<Bone>* getBones() { return &m_bones; }

				void setBones(const List<Bone>& bones)
				{
					
					m_bones = bones;
				}
				void setRigidAnimationClips(const ClipTable& table) { m_rigidAnimationClips = table; m_hasRigidClip = !!table.getCount();}
				void setSkinnedAnimationClips(const ClipTable& table) { m_skinnedAnimationClips = table; m_hasSkinnedClip = !!table.getCount(); }
				void setMaterialAnimationClips(const MtrlClipTable& table) { m_mtrlAnimationClips = table; m_hasMtrlClip = !!table.getCount(); }
				

				///** Bindpose matrices for each bone in the skeleton,
				//	relative to the parent bone.
				//*/
				//const FastList<Matrix>& getBindPose() const { return m_bindPose; }
				///** Vertex to bonespace transforms for each bone in the skeleton.
				//*/
				//const FastList<Matrix>& getInvBindPose() const { return m_invBindPose; }
				///** For each bone in the skeleton, stores the index of the parent bone.
				//*/
				//const FastList<int32>& getSkeletonHierarchy() const { return m_skeletonHierarchy; }

				/**
				 *  Load animation data from a Tagged Data Block
				 */
				void ReadData(TaggedDataReader* data);
				TaggedDataWriter* WriteData() const;


				void Load(const ResourceLocation* rl);
				void Save(Stream* strm) const;

				AnimationData()
					: m_hasSkinnedClip(false), m_hasRigidClip(false), m_hasMtrlClip(false), 
					RigidEntityCount(0), m_rootBone(-1)
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

			private:
				ClipTable m_rigidAnimationClips;
				ClipTable m_skinnedAnimationClips;
				MtrlClipTable m_mtrlAnimationClips;

				//bool m_hasBindPose;
				//bool m_hasSkeleton;
				bool m_hasMtrlClip;
				bool m_hasSkinnedClip;
				bool m_hasRigidClip;


				List<Bone> m_bones;
				int32 m_rootBone;

			};
		}
	}
}

#endif