#pragma once
#ifndef APOC3D_RESOURCEHANDLE_H
#define APOC3D_RESOURCEHANDLE_H
/*
-----------------------------------------------------------------------------
This source file is part of Apoc3D Engine

Copyright (c) 2009+ Tao Xin

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
			/**
			 *  Means the handle will not touch resource, making unloaded loading.
			 *  This is helpful reducing overhead when all resources are preloaded.
			 */
			static const byte FLG_Untouching = 1;
			/**
			 *  This will force the resource being deleted with the destruction of the handle.
			 */
			static const byte FLG_ForceDisposal = 2;
			/**
			 *  This will make handle ignore reference counting.
			 *	When used with FLG_Untouching, this works as a reference wrapper to fit interfaces.
			 */
			static const byte FLG_NoRefCounting = 4;

			static const byte FLGMIX_ContainerHandle = FLG_Untouching | FLG_ForceDisposal;
			static const byte FLGMIX_ReferenceWrapperHandle = FLG_Untouching | FLG_NoRefCounting;

			ResourceHandle(ResType* res)
				: m_resource(res), m_flags(0)
			{
				_Ref();
			}
			ResourceHandle(ResType* res, byte flags)
				: m_resource(res), m_flags(flags)
			{
				_Ref();
			}
			
			// outdated
			ResourceHandle(ResType* res, bool dead);

			virtual ~ResourceHandle(void)
			{
				_Unref();
				
				if (m_flags & FLG_ForceDisposal) 
				{
					delete m_resource;
					m_resource = nullptr;
				}
				else
				{
					if (!shouldNotUseRefCount() &&
						m_resource->getReferenceCount()==0 && 
						(m_resource->isManaged() && !m_resource->getManager()->usesAsync())
						)
					{
						m_resource->Unload();
						delete m_resource;
					}
					m_resource = nullptr;
				}
			}

			//void Build(ResType* res);

			/**
			 *  Notify the resource that it will be, or being used.
			 */
			void Touch()
			{
				if (!shouldNotTouchResource())
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
				if (!shouldNotTouchResource())
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
				if (!shouldNotTouchResource())
				{
					Touch();
				}
				
				return m_resource;
			}

			bool shouldNotTouchResource() const { return (m_flags&FLG_Untouching)!=0; }
			bool shouldNotUseRefCount() const { return (m_flags&FLG_NoRefCounting)!=0; }
		private:
			ResType* m_resource;

			byte m_flags;

			void _Ref( )
			{
				if (!shouldNotUseRefCount() && m_resource->isManaged())
				{
					m_resource->_Ref();
				}
			}
			void _Unref( )
			{
				if (!shouldNotUseRefCount() && m_resource->isManaged())
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