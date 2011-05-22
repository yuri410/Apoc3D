#ifndef SINGLETON_H
#define SINGLETON_H

#include "Common.h"

namespace Apoc3D
{
	namespace Core
	{
#define SINGLETON_DECL(T) namespace Core { template<> T* Singleton<T>::ms_instance = 0; }

		template<class T>
		class APAPI Singleton
		{
		protected:
			static T* ms_instance;

			Singleton()
			{
				ms_instance = static_cast<T*> (this);
			}
			virtual ~Singleton() { delete ms_instance; ms_instance = 0; }
		public:
			static T &getSingleton()
			{
				assert(ms_instance);
				return ms_instance;
			}

			static T* getSingletonPtr()
			{
				assert(ms_instance);
				return ms_instance;
			}
			
		};
	}
}
#endif