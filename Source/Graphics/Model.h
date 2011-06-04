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
#ifndef MODEL_H
#define MODEL_H

#include "Common.h"
#include "Core\Resource.h"
#include "Renderable.h"
#include "RenderOperationBuffer.h"
#include "Collections/FastList.h"
#include "ModelTypes.h"
#include "Animation/AnimationTypes.h"

using namespace Apoc3D::Collections;
using namespace Apoc3D::Core;
using namespace Apoc3D::Graphics;
using namespace Apoc3D::Graphics::Animation;
using namespace Apoc3D::Graphics::RenderSystem;
using namespace Apoc3D::VFS;

namespace Apoc3D
{
	namespace Graphics
	{
		class APAPI ModelSharedData
		{
		private:
			RenderDevice* m_renderDevice;

			FastList<Mesh*> m_entities;
			FastList<Bone> m_bones;
			int32 m_rootBone;
			AnimationData* m_animationData;

			ResourceLocation* m_resourceLocation;

		public:
			ModelSharedData(RenderDevice* device, ResourceLocation* rl)
				: m_renderDevice(device), m_resourceLocation(rl)
			{

			}
		};
		class APAPI Model : Renderable
		{
		private:
			enum AnimationControl
			{
				AC_Play,
				AC_Stop,
				AC_Resume,
				AC_Pause
			};

			RenderOperationBuffer opBuffer;

			/** table cast render operation index to entity index
			*/
			int* m_renderOpEntID;
			/** table cast render operation index to entity part index
			*/
			int* m_renderOpEntPartID;

			bool m_rigidAnimCompleted;
			bool m_rootAnimCompleted;
			bool m_skinAnimCompleted;
			bool m_mtrlAnimCompleted;

			ResourceHandle<ModelSharedData>* m_data;

			

		public:
			Model(void);
			~Model(void);
		};
	}
}
#endif