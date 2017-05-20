#pragma once
#ifndef APOC3D_ANIMATIONDATA_H
#define APOC3D_ANIMATIONDATA_H

/* -----------------------------------------------------------------------
 * This source file is part of Apoc3D Framework
 * 
 * Copyright (c) 2011-2017 Tao Xin
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
			/** Combines all the animation data needed to animate a object. */
			class APAPI AnimationData
			{
			public:
				typedef HashMap<String, ModelAnimationClip*> ClipTable;
				typedef HashMap<String, MaterialAnimationClip*> MtrlClipTable;
			public:

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
				void setRigidAnimationClips(const ClipTable& table) { m_rigidAnimationClips = table; m_hasRigidClip = table.getCount() != 0;}
				void setSkinnedAnimationClips(const ClipTable& table) { m_skinnedAnimationClips = table; m_hasSkinnedClip = table.getCount() != 0; }
				void setMaterialAnimationClips(const MtrlClipTable& table) { m_mtrlAnimationClips = table; m_hasMtrlClip = table.getCount() != 0; }
				

				/**  Load animation data from a Tagged Data Block  */
				void ReadData(TaggedDataReader* data);
				void WriteData(TaggedDataWriter* data) const;


				void Load(const ResourceLocation& rl);
				void Save(Stream& strm) const;

				AnimationData()
				{ }

				~AnimationData();

				AnimationData(const AnimationData&) = delete;
				AnimationData& operator=(const AnimationData&) = delete;

				int RigidEntityCount = 0;

			private:
				void ReadMaterialAnimationClips(BinaryReader* br, int32 count, bool readsFrameFlag);

				ClipTable m_rigidAnimationClips;
				ClipTable m_skinnedAnimationClips;
				MtrlClipTable m_mtrlAnimationClips;

				bool m_hasMtrlClip = false;
				bool m_hasSkinnedClip = false;
				bool m_hasRigidClip = false;


				List<Bone> m_bones;
				int32 m_rootBone = -1;

			};
		}
	}
}

#endif