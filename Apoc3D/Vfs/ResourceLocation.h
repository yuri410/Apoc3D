#pragma once
#ifndef APOC3D_RESOURCELOCATION_H
#define APOC3D_RESOURCELOCATION_H

/**
 * -----------------------------------------------------------------------------
 * This source file is part of Apoc3D Engine
 * 
 * Copyright (c) 2009+ Tao Xin
 * 
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 * 
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 * 
 * You should have received a copy of the GNU General Public License
 * along with this program.  if not, write to the Free Software Foundation, 
 * Inc., 59 Temple Place - Suite 330, Boston, MA 02111-1307, USA, or go to
 * http://www.gnu.org/copyleft/gpl.txt.
 * 
 * -----------------------------------------------------------------------------
 */

#include "apoc3d/Common.h"

using namespace Apoc3D::Core;
using namespace Apoc3D::IO;

namespace Apoc3D
{	
	namespace VFS
	{
		/**
		 *  An abstract class representing the location of a data storage.
		 */
		class APAPI ResourceLocation 
		{
		public:
			virtual ~ResourceLocation() { }

			virtual Stream* GetWriteStream() const = 0;
			virtual Stream* GetReadStream() const = 0;

			virtual bool CanRead() const = 0;
			virtual bool CanWrite() const = 0;

			virtual ResourceLocation* Clone() const = 0;


			int64 getSize() const { return m_size; }
			
			const String& getName() const { return m_name; }
			const String& GetHashString() const { return m_name; }
			uint64 GetHashCode() const;

			RTTI_UpcastableBase;
		protected:
			int64 m_size;

			ResourceLocation(const String& name, int64 size)
				: m_name(name), m_size(size)
			{
			}

			ResourceLocation(const ResourceLocation& another)
				: m_name(another.m_name), m_size(another.m_size)
			{
			}

		private:
			ResourceLocation& operator=(const FileLocation& fl) { return *this; }

			String m_name;
		};

		/**
		 *  Represents the location of a data storage which is stored in a file, including
		 *  those in archives.
		 *  The location can be specified manually or provided by the FileSystem.
		 */
		class APAPI FileLocation : public ResourceLocation
		{
			RTTI_UpcastableDerived(FileLocation, ResourceLocation);
		public:
			FileLocation();
			FileLocation(const String& filePath);
			FileLocation(const FileLocation& fl);
			FileLocation(Archive* pack, const String& filePath, const String& entryName);
			virtual ~FileLocation();

			virtual Stream* GetWriteStream() const { return nullptr;}
			virtual Stream* GetReadStream() const;

			virtual bool CanRead() const { return true; }
			virtual bool CanWrite() const { return false; }

			virtual ResourceLocation* Clone() const { return new FileLocation(*this); }
			
			bool isInArchive() const { return !!m_parent; }
			const String& getPath() const { return m_path; } 

		protected:
			FileLocation(const String& filePath, int64 size);

		private:
			Archive* m_parent;
			String m_path;
			String m_entryName;

		};

		/** 
		 *  Represents the location of a data storage which is stored in memory.
		 */
		class APAPI MemoryLocation : public ResourceLocation
		{
			RTTI_UpcastableDerived(MemoryLocation, ResourceLocation);
		public:
			MemoryLocation(void* pos, int64 size);
			MemoryLocation(const MemoryLocation& ml);

			virtual bool CanRead() const { return true; }
			virtual bool CanWrite() const { return true; }

			virtual Stream* GetWriteStream() const;
			virtual Stream* GetReadStream() const;

			virtual ResourceLocation* Clone() const { return new MemoryLocation(*this); }

		private:
			void* m_data;
		};

		/**
		 *  Represents the location of a data storage which can be accessed through stream
		 */
		class APAPI StreamLocation : public ResourceLocation
		{
			RTTI_UpcastableDerived(StreamLocation, ResourceLocation);
		public:
			StreamLocation(Stream* strm);
			StreamLocation(const StreamLocation& sl);

			virtual bool CanRead() const; 
			virtual bool CanWrite() const; 

			virtual Stream* GetWriteStream() const;
			virtual Stream* GetReadStream() const;

			virtual ResourceLocation* Clone() const  { return new StreamLocation(*this); }

		private:
			Stream* m_stream;
		};
	}
}
#endif