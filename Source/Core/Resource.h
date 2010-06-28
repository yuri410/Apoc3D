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
#ifndef RESOURCE_H
#define RESOURCE_H

#pragma once

#include "..\Common.h"

namespace Apoc3D
{
	namespace Core
	{
		enum _Export ResourceState
		{
			RS_Loaded,
			RS_Loading,
			RS_Unloaded,
			RS_Unloading
		};

		class _Export ResourceProcessor
		{
		public:
			virtual void Process(Resource* res) const = 0;
		};
		class _Export ResourceLoader : ResourceProcessor
		{
		public:
			virtual void Process(Resource* res) const = 0;
		};

		class _Export Resource
		{
		private:
			int m_refCount;
			bool m_loaded;
			ResourceLoader* m_resLoader;


		protected:
			virtual void load();
			virtual void unload() = 0;

			void Load();
			void Unload();
		public: 
			typedef Resource ResTempHelper;   

			Resource(ResourceLoader* loader);

			void _Ref() { }
			void _Unref() { }
		};
	};
};
#endif