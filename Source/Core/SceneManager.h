#ifndef SCENEMANAGER_H
#define SCENEMANAGER_H

#pragma once

#include "..\Common.h"


#include <vector>

namespace Apoc3D
{
	namespace Core
	{
		typedef std::vector<SceneObject*> ObjectList;
		/*
		  SceneManager keeps tracks of all scene objects.
		*/
		class _Export SceneManager
		{
		private:
			ObjectList m_objects;

		public:
			SceneManager(void);
			~SceneManager(void);
		
			/*
			  Adds a new scene object into scene
			*/
			virtual void AddObject(SceneObject* obj);
			/*
			  Removes a scene obejct from scene
			*/
			virtual bool RemoveObject(SceneObject* obj);
		};
	};
};

#endif