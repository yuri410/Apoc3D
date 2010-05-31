
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
			const ResType* m_resource;

			void _Ref(const Resource* res);
			void _Unref(const Resource* res);

		protected:
			ResourceHandle(const ResType* res)
			{
				m_resource = res;
				_Ref(res);
			}
			
		public:
			~ResourceHandle(void)
			{
				m_resource = 0;
				_Unref(m_resource);
			}

			void Touch();

			ResType* getWeakRef() const { return m_resource; }

			ResType* operator ->()
			{
				Touch();
				return m_resource;
			}

		};
	};
};