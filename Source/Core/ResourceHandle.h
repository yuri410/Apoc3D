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
http://www.gnu.org/copyleft/gpl.txt.

-----------------------------------------------------------------------------
*/
#ifndef RESOURCEHANDLE_H
#define RESOURCEHANDLE_H

#include "Common.h"
#include "Resource.h"

namespace Apoc3D
{
	namespace Core
	{
		template <class ResType>
		class APAPI ResourceHandle
		{
			typedef typename ResType::ResHandleTemplateConstraint CF_XXX; 

		private:
			ResType* m_resource;
			bool m_dummy;
			void _Ref( )
			{
				if (!m_dummy && m_resource->isManaged())
				{
					m_resource->_Ref();
				}
			}
			void _Unref( )
			{
				if (!m_dummy && m_resource->isManaged())
				{
					m_resource->_Unref();
				}
			}

			ResourceHandle(const ResourceHandle& another) { }
		protected:
			
		public:
			ResourceHandle(ResType* res)
				: m_resource(res), m_dummy(false)
			{
				_Ref();
			}
			ResourceHandle(ResType* res, bool dummy)
				: m_resource(res), m_dummy(dummy)
			{
				_Ref();
			}
			virtual ~ResourceHandle(void)
			{
				_Unref();
				if (m_dummy)
				{
					delete m_resource;
				}
				else
				{
					if (!m_resource->getReferenceCount())
						delete m_resource;
					m_resource = 0;
				}
			}

			//void Build(ResType* res);

			void Touch()
			{
				if (!m_dummy)
				{
					m_resource->Use();
				}
			}
			void TouchSync()
			{
				if (!m_dummy)
				{
					m_resource->UseSync();
				}
			}

			inline ResourceState getState() const
			{
				return m_resource->getState();
			}

			ResType* getWeakRef() const { return m_resource; }

			ResType* operator ->()
			{
				if (!m_dummy)
				{
					Touch();
				}
				
				return m_resource;
			}

		};
	};
};
#endif