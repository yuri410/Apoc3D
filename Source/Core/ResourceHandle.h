/*
-----------------------------------------------------------------------------
This source file is part of Apoc3D Engine

Copyright (c) 2009+ Tao Games

This program is free software: you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation, either version 3 of the License, or
(at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program.  if not, write to the Free Software Foundation, 
Inc., 59 Temple Place - Suite 330, Boston, MA 02111-1307, USA, or go to
http://www.gnu.org/copyleft/lesser.txt.

-----------------------------------------------------------------------------
*/

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