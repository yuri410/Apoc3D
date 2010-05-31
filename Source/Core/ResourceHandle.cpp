#include "ResourceHandle.h"

#include "Resource.h"

namespace Apoc3D
{
	namespace Core
	{
		template<class ResType>
		void ResourceHandle<ResType>::_Ref(const Resource* res)
		{
			res->_Ref();
		}

		template<class ResType>
		void ResourceHandle<ResType>::_Unref(const Resource* res)
		{
			res->_Unref();
		}

		template<class ResType>
		void ResourceHandle<ResType>::Touch()
		{
			
		}
	}
}