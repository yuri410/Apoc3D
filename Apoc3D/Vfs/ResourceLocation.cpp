/* -----------------------------------------------------------------------
 * This source file is part of Apoc3D Framework
 * 
 * Copyright (c) 2009-2018 Tao Xin
 * 
 * This content of this file is subject to the terms of the Mozilla Public 
 * License v2.0. If a copy of the MPL was not distributed with this file, 
 * you can obtain one at http://mozilla.org/MPL/2.0/.
 * 
 * This program is distributed in the hope that it will be useful, 
 * WITHOUT WARRANTY OF ANY KIND; either express or implied. See the 
 * Mozilla Public License for more details.
 * 
 * ------------------------------------------------------------------------
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
		uint64 ResourceLocation::GetHashCode() const
		{
			return StringUtils::GetHashCode(m_name);
		}

		//////////////////////////////////////////////////////////////////////////

		FileLocation::FileLocation(Archive* pack, const String& filePath, const String& entryName)
			: ResourceLocation(filePath, pack->GetEntrySize(entryName)), 
			m_parent(pack), m_path(filePath), m_entryName(entryName)
		{

		}

		FileLocation::FileLocation(const String& filePath, int64 size)
			: ResourceLocation(filePath, size), m_parent(nullptr), m_path(filePath)
		{

		}
		FileLocation::FileLocation(const String& filePath)
			: ResourceLocation(filePath, File::GetFileSize(filePath)), m_parent(nullptr), m_path(filePath)
		{

		}
		FileLocation::FileLocation()
			: ResourceLocation(L"", 0), m_parent(nullptr)
		{

		}
		FileLocation::~FileLocation()
		{
		}

		FileLocation::FileLocation(FileLocation&& fl)
			: ResourceLocation(std::forward<ResourceLocation>(fl)), m_parent(fl.m_parent),
			m_path(std::move(fl.m_path)), m_entryName(std::move(fl.m_entryName))
		{

		}

		FileLocation& FileLocation::operator=(FileLocation&& rhs)
		{
			if (this != &rhs)
			{
				ResourceLocation::operator=(std::forward<ResourceLocation>(rhs));

				m_parent = rhs.m_parent;
				m_path = std::move(rhs.m_path);
				m_entryName = std::move(rhs.m_entryName);
			}
			return *this;
		}

		Stream* FileLocation::GetReadStream() const
		{
			if (m_parent)
			{
				Stream* s = m_parent->GetEntryStream(m_entryName);
				assert(s);

				VirtualStream* strm = up_cast<VirtualStream*>(s);
				assert(strm);
				return strm;
			}
			
			return new FileStream(m_path);
		}

		//////////////////////////////////////////////////////////////////////////

		MemoryLocation::MemoryLocation(void* pos, int64 size)
			: ResourceLocation(L"[ADDR]" + StringUtils::UIntToStringHex((uintptr)pos), size), m_data(pos)
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
			: ResourceLocation(L"[STRM]" + StringUtils::UIntToStringHex((uint64)strm), strm->getLength()), 
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