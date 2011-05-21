#ifndef SINGLETON_H
#define SINGLETON_H

#pragma once

#include "..\Common.h"

namespace Apoc3D
{
	namespace Core
	{
		template<class T>
		class APOC3D_API Singleton
		{
		private:
			static T* ms_instance;
		protected:
			Singleton()
			{
				ms_instance = static_cast<T*> (this);
			}
			~Singleton() { delete ms_instance; }
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