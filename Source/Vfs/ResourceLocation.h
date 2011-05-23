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
#ifndef RESOURCELOCATION_H
#define RESOURCELOCATION_H


#include "Common.h"
#include "Core/IHashed.h"

using namespace Apoc3D::Core;

using namespace std;

namespace Apoc3D
{	
	namespace VFS
	{
		class APAPI ResourceLocation : public IHashed
		{
		private:
			String m_name;
		protected:
			uint64 m_size;

			ResourceLocation(const String& name, uint64 s)
				: m_name(name), m_size(s)
			{
			}
		public:
			virtual wostream* GetWriteStream() const = 0;
			virtual wistream* GetReadStream() const = 0;

			virtual bool isReadOnly() const = 0;

			uint64 getSize() const { return m_size; }
			
			virtual String GetHashString() const { return m_name; }
			virtual HashHandle GetHashCode() const;

			virtual bool CanRead() const = 0;
			virtual bool CanWrite() const = 0;
		};
		class APAPI FileLocation : public ResourceLocation
		{
		private:
			Archive* m_parent;
			String m_path;

		protected:
			FileLocation(const String& filePath, uint64 size);
		public:
			FileLocation(const String& filePath) {}
			FileLocation(const FileLocation& fl) {}
			FileLocation(Archive* pack, const String& filePath, wifstream* stm);

			virtual wostream* GetWriteStream() const { return 0;}
			virtual wistream* GetReadStream() const
			{}

			bool isInArchive() const { return !!m_parent; }
			const String& getPath() const { return m_path; } 
			
			virtual bool CanRead() const { return true; }
			virtual bool CanWrite() const { return false; }
		};
		class APAPI MemoryLocation : public ResourceLocation
		{
		private:
			void* m_data;
		public:
			MemoryLocation(void* pos, uint64 size);

			virtual bool CanRead() const { return true; }
			virtual bool CanWrite() const { return true; }

			virtual wostream* GetWriteStream() const
			{
				wostream* strm;
				
				return strm;
			}
			virtual wistream* GetReadStream() const
			{}
		};
	}
}
#endif