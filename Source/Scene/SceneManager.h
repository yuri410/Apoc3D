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

#include "Common.h"

using namespace std;
using namespace Apoc3D::Graphics;
using namespace Apoc3D::Core;

namespace Apoc3D
{
	namespace Scene
	{
		template class APOC3D_API vector<SceneObject*>;
		typedef vector<SceneObject*> ObjectList;
		class OperationList;
		class GeometryTable;
		class MaterialTable;
		class PriorityTable;
		class MaterialList;
		
		template class APOC3D_API vector<RenderOperation>;
		template class APOC3D_API unordered_map<BatchHandle, OperationList*>;
		template class APOC3D_API unordered_map<BatchHandle, GeometryTable*>;
		template class APOC3D_API unordered_map<uint32, MaterialTable*>;
		template class APOC3D_API unordered_map<BatchHandle, Material*>;

		class APOC3D_API OperationList : public vector<RenderOperation> { };
		class APOC3D_API GeometryTable : public unordered_map<BatchHandle, OperationList*> { };
		class APOC3D_API MaterialTable : public unordered_map<BatchHandle, GeometryTable*> { };
		class APOC3D_API PriorityTable : public unordered_map<uint32, MaterialTable*> { };
		class APOC3D_API MaterialList : public unordered_map<BatchHandle, Material*> { };
				
		class APOC3D_API BatchData
		{
		public:
			static const int MaxPriority = 10;

		private:
			PriorityTable m_priTable;
			MaterialList m_mtrlList;			

			int m_objectCount;

		public:
			BatchData() { }
			
			int getObjectCount() const { return m_objectCount; }
			
			void AddVisisbleObject(SceneObject* obj, int level);

			void Clear();
		};

		/* SceneManager keeps tracks of all scene objects.
		*/
		class APOC3D_API SceneManager
		{
		private:
			ObjectList m_objects;
		protected:
			
		public:
			SceneManager(void);
			~SceneManager(void);
		
			/* Adds a new scene object into scene
			*/
			virtual void AddObject(SceneObject* const obj);
			/* Removes a scene obejct from scene
			*/
			virtual bool RemoveObject(SceneObject* const obj);

			virtual void PrepareVisibleObjects(Camera* camera, BatchData* batchData) = 0;

			virtual void Update(const GameTime* const &time);
		};
	};
};

#endif