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

#include "Streams.h"
#include "apoc3d/Apoc3DException.h"

namespace Apoc3D
{
	namespace IO
	{
		FileStream::FileStream(const String& filename)
		{
			m_in = new ifstream(filename.c_str(), ios::in | ios::binary);
			m_in->exceptions( std::ios::failbit ); 

			uint64 oldPos = m_in->tellg();
			m_in->seekg(0, ios::end);
			m_length = m_in->tellg();
			m_in->seekg(oldPos, ios::beg);
		}
		FileStream::~FileStream()
		{
			delete m_in;
		}

		int64 FileStream::Read(char* dest, int64 count)
		{
			m_in->read(reinterpret_cast<char*>(dest), count);

			return m_in->gcount();
		}
		void FileStream::Write(const char* src, int64 count)
		{
			throw Apoc3DException::createException(EX_NotSupported, L"Can't write");
		}
		void FileStream::Seek(int64 offset, SeekMode mode)
		{
			ios::seekdir dir;
			switch (mode)
			{
			case SEEK_Begin:
				dir = ios::beg;
				break;
			case SEEK_End:
				dir = ios::end;
				break;
			default:
				dir = ios::cur;
				break;
			}

			m_in->seekg(offset, dir);
		}

		void FileStream::Close()
		{
			m_in->close();
			
		}

		/************************************************************************/
		/*                                                                      */
		/************************************************************************/

		FileOutStream::FileOutStream(const String& filename)
		{
			m_out = new ofstream(filename.c_str(), ios::out | ios::binary | ios::trunc);
			m_length = 0;
		}
		FileOutStream::~FileOutStream()
		{
			delete m_out;
		}

		int64 FileOutStream::Read(char* dest, int64 count)
		{
			throw Apoc3DException::createException(EX_NotSupported, L"Can't read");
		}
		void FileOutStream::Write(const char* src, int64 count)
		{
			m_out->write(src, count);
			//m_length += count;
			int64 p = getPosition();
			if (p>m_length)
				m_length = p;
		}
		void FileOutStream::Seek(int64 offset, SeekMode mode)
		{
			ios::seekdir dir;
			switch (mode)
			{
			case SEEK_Begin:
				dir = ios::beg;
				break;
			case SEEK_End:
				dir = ios::end;
				break;
			default:
				dir = ios::cur;
				break;
			}

			m_out->seekp(offset, dir);
			int64 p = getPosition();
			if (p>m_length)
				m_length = p;
		}

		void FileOutStream::Close()
		{
			m_out->close();
		}

		void FileOutStream::Flush()
		{
			m_out->flush();
		}

		/************************************************************************/
		/*                                                                      */
		/************************************************************************/

		void MemoryStream::Write(const char* src, int64 count)
		{
			if (count > m_length)
			{
				throw Apoc3DException::createException(EX_EndOfStream, L"");
			}
			
			memcpy(m_data + m_position, src, static_cast<size_t>(count));			
			m_position += count;
		}

		void VirtualStream::setPosition(int64 offset)
		{
			m_baseStream->setPosition( offset + m_baseOffset );
		}

	};
};