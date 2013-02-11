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
#ifndef APOC3D_RESOURCEHANDLE_H
#define APOC3D_RESOURCEHANDLE_H

#include "Common.h"
#include "Resource.h"
#include "ResourceManager.h"

namespace Apoc3D
{
	namespace Core
	{
		/**
		 *   A proxy class to access a managed resources.
		 *  It helps informing resources accessing records and the change of reference count.
		 */
		template <class ResType>
		class ResourceHandle
		{
			/** 
			 *  used to make sure the ResType are derived from the Resource class
			 */
			typedef typename ResType::ResHandleTemplateConstraint CF_XXX;
		public:
			ResourceHandle(ResType* res)
				: m_resource(res), m_dummy(false), m_disposal(true)
			{
				_Ref();
			}
			ResourceHandle(ResType* res, bool dummy, bool disposal = true)
				: m_resource(res), m_dummy(dummy), m_disposal(disposal)
			{
				_Ref();
			}
			virtual ~ResourceHandle(void)
			{
				_Unref();
				
				if (m_dummy) 
				{
					if (m_disposal)
						delete m_resource;
				}
				else
				{
					if (!m_resource->getReferenceCount() && 
						(m_resource->isManaged() && !m_resource->getManager()->usesAsync())
						)
					{
						m_resource->Unload();
						delete m_resource;
					}
					m_resource = 0;
				}
			}

			//void Build(ResType* res);

			/**
			 *  Notify the resource that it will be, or being used.
			 */
			void Touch()
			{
				if (!m_dummy)
				{
					m_resource->Use();
				}
			}
			/**
			 *  Notify the resource that it will be, or being used.
			 *  If the resource is not loaded, the caller's thread will be suspended until the resource is ready.
			 */
			void TouchSync()
			{
				if (!m_dummy)
				{
					m_resource->UseSync();
				}
			}

			/** 
			 *  Gets the resource's state.
			 */
			inline ResourceState getState() const
			{
				return m_resource->getState();
			}

			/**
			 *  Gets the pointer to the resource object without informing it.
			 *  This is used in special cases where the resource objects contains data that is not changed during any load/unload operations.
			 */
			ResType* getWeakRef() const { return m_resource; }

			ResType* operator ->()
			{
				if (!m_dummy)
				{
					Touch();
				}
				
				return m_resource;
			}

		private:
			ResType* m_resource;
			bool m_dummy;
			bool m_disposal;
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
			ResourceHandle& operator =(const ResourceHandle& other)
			{
				return *this;
			}
		protected:
		};
	};
};
#endif