#include "ResourceHandle.h"

#include "Resource.h"

namespace Apoc3D
{
	namespace Core
	{
		template<class ResType>
		void ResourceHandle<ResType>::_Ref(Resource* res)
		{
			(static_cast<Resource*>(res))->_Ref();
		}

		template<class ResType>
		void ResourceHandle<ResType>::_Unref(Resource* res)
		{
			(static_cast<Resource*>(res))->_Unref();
		}

		template<class ResType>
		void ResourceHandle<ResType>::Touch()
		{
			
		}
	}
}