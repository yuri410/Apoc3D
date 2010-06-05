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
#ifndef SCENEMANAGER_H
#define SCENEMANAGER_H

#pragma once

#include "..\Common.h"


#include <vector>
#include <map>

using namespace std;

namespace Apoc3D
{
	namespace Core
	{
		typedef vector<const SceneObject*> ObjectList;

		typedef map<BatchHandle, GeometryData*> GeometryTable;
		typedef map<BatchHandle, GeometryTable*> MaterialTable;
		typedef map<int, MaterialTable*> PriorityTable;

		typedef map<BatchHandle, Material*> MaterialList;
		
		class _Export BatchData
		{
		public:
			static const int MaxPriority = 10;

		private:
			PriorityTable m_priTable;
			MaterialList m_mtrlTable;

			int m_objectCount;

		public:
			BatchData() { }
			
			int getObjectCount() const { return m_objectCount; }

			void Add(const RenderOperation* op, int count);
			void Clear();
		};

		/* SceneManager keeps tracks of all scene objects.
		*/
		class _Export SceneManager
		{
		private:
			ObjectList m_objects;

		public:
			SceneManager(void);
			~SceneManager(void);
		
			/* Adds a new scene object into scene
			*/
			virtual void AddObject(const SceneObject* obj);
			/* Removes a scene obejct from scene
			*/
			virtual bool RemoveObject(const SceneObject* obj);

			void RenderScene();
		};
	};
};

#endif