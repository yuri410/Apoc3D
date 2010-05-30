#ifndef SCENEOBJECT_H
#define SCENEOBJECT_H

#pragma once

#include "..\Common.h"

namespace Apoc3D
{
	namespace Core
	{
		class _Export SceneObject
		{
		private:
			bool m_hasSubObjects;

		public:
			bool hasSubObjects() { return m_hasSubObjects; }

			SceneObject(bool hasSubObjs) 
				: m_hasSubObjects(hasSubObjs)
			{
			}

			~SceneObject(){}
		};
	};
};
#endif