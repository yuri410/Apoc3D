#include "Singleton.h"

namespace Apoc3D
{
	namespace Core
	{
		template<class T>
		T* Singleton<T>::ms_instance = 0;
	}
}