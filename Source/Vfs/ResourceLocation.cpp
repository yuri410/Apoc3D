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
#include "ResourceLocation.h"

#include "Utility/StringUtils.h"
#include "PathUtils.h"
#include "io/Streams.h"

using namespace Apoc3D::Utility;

namespace Apoc3D
{	
	namespace VFS
	{
		static uint64 GetFileSize(const String& file)
		{
			return 0;
		}

		HashHandle ResourceLocation::GetHashCode() const
		{
			return StringUtils::GetHashCode(m_name);
		}

		FileLocation::FileLocation(Archive* pack, const String& filePath, Stream* stm)
			: ResourceLocation(filePath, stm->getLength()), 
			m_parent(pack), m_path(filePath), m_stream(stm)
		{

		}
		FileLocation::FileLocation(const FileLocation& fl)
			: ResourceLocation(fl.getName(), fl.getSize()), 
			m_parent(fl.m_parent), m_path(fl.m_path), m_stream(0)
		{
		}
		
		FileLocation::FileLocation(const String& filePath, int64 size)
			: ResourceLocation(filePath, size), 
			m_parent(0), m_path(filePath), m_stream(0)
		{

		}
		FileLocation::FileLocation(const String& filePath)
			: ResourceLocation(filePath, GetFileSize(filePath)),
			m_parent(0), m_path(filePath), m_stream(0)
		{			

		}

		Stream* FileLocation::GetReadStream() const
		{
			if (m_stream)
				return m_stream;
			return new FileStream(m_path);
		}

		MemoryLocation::MemoryLocation(void* pos, int64 size)
			: ResourceLocation(L"[ADDR]" + StringUtils::ToString(size), size), m_data(pos)
		{
		}

		Stream* MemoryLocation::GetReadStream() const
		{
			return new MemoryStream(reinterpret_cast<byte*>(m_data), m_size);
		}
		Stream* MemoryLocation::GetWriteStream() const
		{
			return new MemoryStream(reinterpret_cast<byte*>(m_data), m_size);
		}
	}
}