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
#ifndef SIMPLESCENEMANAGER_H
#define SIMPLESCENEMANAGER_H
#pragma once

#include "..\Common.h"
#include "SceneManager.h"

namespace Apoc3D
{
	namespace Scene
	{
		class SimpleSceneNode;

		class SimpleSceneManager : public SceneManager
		{
		private:
			SimpleSceneNode* m_defaultNode;

		public:
			SimpleSceneManager(void);
			~SimpleSceneManager(void);

			virtual void AddObject(SceneObject* const obj);
			virtual void RemoveObject(SceneObject* const obj);

			virtual void PrepareVisibleObjects(Camera* camera, BatchData* batchData);

		};
	}
}
#endif