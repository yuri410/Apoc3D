
#include "..\Common.h"

namespace Apoc3D
{
	namespace Core
	{
		template <class ResType>
		class ResourceHandle
		{
			typedef typename ResType::ResTempHelper CF_XXX; 

		private:
			ResType* m_resource;

			void _Ref(Resource* res);
			void _Unref(Resource* res);

		public:
			ResourceHandle(ResType* res)
			{
				m_resource = res;
				_Ref(res);
			}
			~ResourceHandle(void)
			{
				m_resource = 0;
				_Unref(m_resource);
			}

			void Touch();

			ResType* getWeakRef() { return m_resource; }

			ResType* operator ->()
			{
				Touch();
				return m_resource;
			}

		};
	};
};