
#include "SceneManager.h"


namespace Apoc3D
{
	namespace Core
	{
		SceneManager::SceneManager(void)
		{
		}


		SceneManager::~SceneManager(void)
		{
		}

		void SceneManager::AddObject(const SceneObject* obj)
		{
			m_objects.push_back(obj);
		} 
		bool SceneManager::RemoveObject(const SceneObject* obj)
		{
			m_objects.erase(std::find(m_objects.begin(), m_objects.end(), obj));
		}
	};
};