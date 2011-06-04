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
#ifndef COLLECTIONSCOMMON_H
#define COLLECTIONSCOMMON_H

#include "Common.h"

using namespace Apoc3D::Core;

namespace Apoc3D
{
	namespace Collections
	{
		/** Defines methods to support the comparison of objects for equality.
		*/
		template<typename T>
		class APAPI IEqualityComparer
		{
		public:
			static const IEqualityComparer* Default;
			/** Determines whether the specified objects are equal.
			*/
			virtual bool Equals(const T& x, const T& y) const = 0;
			/** Returns a hash code for the specified object.
			*/
			virtual int64 GetHashCode(const T& obj) const = 0;
		};

		typedef Resource* LPResource;
		class ResourceEqualityComparer : public IEqualityComparer<LPResource>
		{
		public:
			virtual bool Equals(const LPResource& x, const LPResource& y) const;
			
			virtual int64 GetHashCode(const LPResource& obj) const;
		};
	}
}

#endif