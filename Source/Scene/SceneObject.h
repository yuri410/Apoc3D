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
#ifndef SCENEOBJECT_H
#define SCENEOBJECT_H

#pragma once

#include "Common.h"
#include "Graphics\Renderable.h"

using namespace Apoc3D::Graphics;
using namespace Apoc3D::Core;

namespace Apoc3D
{
	namespace Scene
	{
		class APAPI SceneObject : public Renderable
		{
		private:
			bool m_hasSubObjects;
			SceneNode* m_parentNode;

		public:
			bool hasSubObjects() const { return m_hasSubObjects; }
			SceneNode* getParentNode() const { return m_parentNode; }

			SceneObject(const bool hasSubObjs = false) 
				: m_hasSubObjects(hasSubObjs), m_parentNode(0)
			{
			}

			~SceneObject(){}

			void NotifyParentNode(SceneNode* node)
			{
				m_parentNode = node;
			}

			virtual void PrepareVisibleObjects(const Camera* const camera, int32 level, BatchData* data) {}

			virtual void Update(const GameTime* const &time) = 0;
		};
	};
};
#endif