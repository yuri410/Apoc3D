
#include "..\Common.h"

namespace Apoc3D
{
	namespace Core
	{
		template <class ResType>
		class ResourceHandle
		{
		private:
			ResType* m_resource;

			void Dummy()
			{
				Resource* res = static_cast<Resource*>(m_resource);
			}
		public:
			ResourceHandle(ResType* res)
			{
				m_resource = res;
			}
			~ResourceHandle(void)
			{
				m_resource = 0;
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