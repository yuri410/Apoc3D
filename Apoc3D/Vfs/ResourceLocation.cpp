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
#include "ResourceLocation.h"

#include "Archive.h"
#include "PathUtils.h"
#include "File.h"

#include "apoc3d/Utility/StringUtils.h"
#include "apoc3d/IOLib/Streams.h"

using namespace Apoc3D::Utility;

namespace Apoc3D
{	
	namespace VFS
	{
		HashHandle ResourceLocation::GetHashCode() const
		{
			return StringUtils::GetHashCode(m_name);
		}

		FileLocation::FileLocation(Archive* pack, const String& filePath, const String& entryName)
			: ResourceLocation(filePath, pack->GetEntrySize(entryName)), 
			m_parent(pack), m_path(filePath), m_entryName(entryName)
		{

		}
		FileLocation::FileLocation(const FileLocation& fl)
			: ResourceLocation(fl.getName(), fl.getSize()), 
			m_parent(fl.m_parent), m_path(fl.m_path), m_entryName(fl.m_entryName)
		{
		}
		
		FileLocation::FileLocation(const String& filePath, int64 size)
			: ResourceLocation(filePath, size), 
			m_parent(0), m_path(filePath)
		{

		}
		FileLocation::FileLocation(const String& filePath)
			: ResourceLocation(filePath, File::GetFileSize(filePath)),
			m_parent(0), m_path(filePath)
		{

		}
		FileLocation::~FileLocation()
		{
		}
		Stream* FileLocation::GetReadStream() const
		{
			if (m_parent)
			{
				Stream* s = m_parent->GetEntryStream(m_entryName);
				assert(s);

				VirtualStream* strm = dynamic_cast<VirtualStream*>(s);

				if (!strm)
				{
					return new VirtualStream(*strm);
				}
				else
				{
					return strm;
				}
			}
			
			return new FileStream(m_path);
		}

		//////////////////////////////////////////////////////////////////////////

		MemoryLocation::MemoryLocation(void* pos, int64 size)
			: ResourceLocation(L"[ADDR]" + StringUtils::ToString(size), size), m_data(pos)
		{
		}

		Stream* MemoryLocation::GetReadStream() const
		{
			return new MemoryStream(reinterpret_cast<char*>(m_data), m_size);
		}
		Stream* MemoryLocation::GetWriteStream() const
		{
			return new MemoryStream(reinterpret_cast<char*>(m_data), m_size);
		}

		//////////////////////////////////////////////////////////////////////////

		StreamLocation::StreamLocation(Stream* strm)
			: ResourceLocation(L"[STRM]" + StringUtils::ToStringHex((uint64)strm), strm->getLength()), 
			m_stream(strm)
		{

		}

		Stream* StreamLocation::GetReadStream() const
		{
			if (m_stream->CanRead())
			{
				return new VirtualStream(m_stream);
			}
			return nullptr;
		}

		Stream* StreamLocation::GetWriteStream() const
		{
			if (m_stream->CanWrite())
			{
				return new VirtualStream(m_stream);
			}
			return nullptr;
		}

		bool StreamLocation::CanRead() const { return m_stream->CanRead(); }
		bool StreamLocation::CanWrite() const { return m_stream->CanWrite(); }

	}
}