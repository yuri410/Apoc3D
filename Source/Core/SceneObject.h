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
			bool hasSubObjects() const { return m_hasSubObjects; }

			SceneObject(const bool hasSubObjs) 
				: m_hasSubObjects(hasSubObjs)
			{
			}

			~SceneObject(){}
		};
	};
};
#endif