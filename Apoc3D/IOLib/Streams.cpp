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

#include "Streams.h"
#include "apoc3d/ApocException.h"

using namespace std;

namespace Apoc3D
{
	namespace IO
	{
		/************************************************************************/
		/*  FileStream                                                          */
		/************************************************************************/

		FileStream::FileStream(const String& filename)
			: m_in(filename.c_str(), ios::in | ios::binary)
		{
			m_in.exceptions( std::ios::failbit ); 

			uint64 oldPos = m_in.tellg();
			m_in.seekg(0, ios::end);
			m_length = m_in.tellg();
			m_in.seekg(oldPos, ios::beg);

			m_in.exceptions(std::ios::goodbit);
		}
		FileStream::~FileStream()
		{
		}

		int64 FileStream::Read(char* dest, int64 count)
		{
			m_in.read(reinterpret_cast<char*>(dest), count);

			return m_in.gcount();
		}
		void FileStream::Write(const char* src, int64 count)
		{
			throw AP_EXCEPTION(ApocExceptionType::NotSupported, L"Can't write");
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

			m_in.seekg(offset, dir);
		}

		void FileStream::Close()
		{
			m_in.close();
		}

		void FileStream::setPosition(int64 offset)
		{
			m_in.seekg(offset, ios::beg); 
		}
		int64 FileStream::getPosition()
		{ 
			return m_in.tellg();
		}

		/************************************************************************/
		/*  FileOutStream                                                       */
		/************************************************************************/

		FileOutStream::FileOutStream(const String& filename)
			: m_out(filename.c_str(), ios::out | ios::binary | ios::trunc)
		{
			m_length = 0;
		}
		FileOutStream::~FileOutStream()
		{
		}

		int64 FileOutStream::Read(char* dest, int64 count)
		{
			throw AP_EXCEPTION(ApocExceptionType::NotSupported, L"Can't read");
		}
		void FileOutStream::Write(const char* src, int64 count)
		{
			m_out.write(src, count);
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

			m_out.seekp(offset, dir);
			int64 p = getPosition();
			if (p>m_length)
				m_length = p;
		}

		void FileOutStream::Close()
		{
			m_out.close();
		}

		void FileOutStream::Flush()
		{
			m_out.flush();
		}
		void FileOutStream::setPosition(int64 offset)
		{
			m_out.seekp(offset, ios::beg); 
			if (m_length<offset)
				m_length = offset;
		}
		int64 FileOutStream::getPosition()
		{
			return m_out.tellp(); 
		}
		/************************************************************************/
		/*  MemoryStream                                                        */
		/************************************************************************/

		int64 MemoryStream::Read(char* dest, int64 count)
		{
			if (m_position + count > m_length)
			{
				count = m_length - m_position;
			}

			memcpy(dest, m_data+m_position, static_cast<size_t>(count));

			m_position += count;
			return count;
		}

		void MemoryStream::Write(const char* src, int64 count)
		{
			if (m_position + count > m_length)
			{
				throwEndofStreamException();
			}
			
			memcpy(m_data + m_position, src, static_cast<size_t>(count));			
			m_position += count;
		}

		void MemoryStream::Seek(int64 offset, SeekMode mode)
		{
			switch (mode)
			{
			case SEEK_Begin:
				m_position = (int)offset;
				break;
			case SEEK_Current:
				m_position += (int)offset;
				break;
			case SEEK_End:
				m_position = m_length + (int)offset;
				break;
			}
			if (m_position < 0)
				m_position = 0;
			if (m_position > m_length)
				m_position = m_length;
		}

		void MemoryStream::throwEndofStreamException()
		{
			throw AP_EXCEPTION(ApocExceptionType::EndOfStream, L"");
		}

		/************************************************************************/
		/*  VirtualStream                                                       */
		/************************************************************************/

		void VirtualStream::setPosition(int64 offset)
		{
			m_baseStream->setPosition( offset + m_baseOffset );
		}

		void VirtualStream::Seek(int64 offset, SeekMode mode)
		{
			switch (mode)
			{
			case SEEK_Begin:
				if (offset > m_length)
				{
					offset = m_length;
				}
				if (offset < 0)
				{
					offset = 0;
				}
				m_baseStream->setPosition(offset+ m_baseOffset);
				break;
			case SEEK_Current:
				if (m_baseStream->getPosition() + offset > m_baseOffset + m_length)
				{
					offset = m_baseOffset + m_length - m_baseStream->getPosition();
				}
				if (m_baseStream->getPosition() + offset < m_baseOffset)
				{
					offset = m_baseOffset - m_baseStream->getPosition();
				}
				m_baseStream->Seek(offset, mode);
				break;
			case SEEK_End:
				if (offset > 0)
				{
					offset = 0;
				}

				if (offset < -m_length)
				{
					offset = -m_length;
				}
				m_baseStream->setPosition(m_length - offset+ m_baseOffset);
				break;
			}

		}

		/************************************************************************/
		/* MemoryOutStream                                                      */
		/************************************************************************/

		int64 MemoryOutStream::Read(char* dest, int64 count)
		{
			if (m_position + count > m_length)
			{
				count = m_length - m_position;
			}

			for (int64 i=0;i<count;i++)
				dest[i] = m_data[static_cast<int32>(i+m_position)];

			m_position += count;
			return count;
		}
		void MemoryOutStream::Write(const char* src, int64 count)
		{
			for (int64 i=0;i<count;i++)
			{
				if (m_position>=m_length)
				{
					m_data.Add(src[i]);
					m_length++;
				}
				else
				{
					m_data[static_cast<int32>(m_position)] = src[i];
				}
				m_position++;
			}

		}

		void MemoryOutStream::Seek(int64 offset, SeekMode mode)
		{
			switch (mode)
			{
			case SEEK_Begin:
				m_position = (int)offset;
				break;
			case SEEK_Current:
				m_position += (int)offset;
				break;
			case SEEK_End:
				m_position = m_length + (int)offset;
				break;
			}
			if (m_position < 0)
				m_position = 0;
			if (m_position > m_length)
				m_position = m_length;
		}


		/************************************************************************/
		/* BufferedStreamReader                                                 */
		/************************************************************************/

		int32 BufferedStreamReader::Read(char* dest, int32 count)
		{
			int32 ret;
			if (getBufferContentSize()>count)
			{
				ReadBuffer(dest, count);
				ret = count;
			}
			else
			{
				ret = 0;

				int32 existing = getBufferContentSize();
				if (existing>0)
				{
					ReadBuffer(dest, existing);
					ret = existing;
					count -= existing;
				}

				int32 actual = static_cast<int32>(m_baseStream->Read(dest + existing, count));
				m_endofStream = actual < count;
				ret += actual;
			}

			if (!m_endofStream && m_size<BufferSize/2)
			{
				FillBuffer();
			}

			return ret;
		}
		bool BufferedStreamReader::ReadByte(char& result)
		{
			if (!m_endofStream && m_size<BufferSize/2)
			{
				FillBuffer();
			}

			if (m_size)
			{
				result = m_buffer[m_head];
				m_head = (m_head + 1) % BufferSize;
				m_size--;
				return true;
			}
			return false;
		}

		void BufferedStreamReader::ClearBuffer()
		{
			m_head = 0;
			m_tail = 0;
			m_size = 0;
		}
		void BufferedStreamReader::ReadBuffer(char* dest, int32 count)
		{
			assert(m_size >= count);

			if (m_head + count > BufferSize)
			{
				int32 numHeadToEnd = BufferSize - m_head;
				memcpy(dest, m_buffer+m_head, numHeadToEnd);

				int32 remaining = count - numHeadToEnd;
				assert(remaining<=m_head);
				memcpy(dest+numHeadToEnd, m_buffer, remaining);
			}
			else
			{
				memcpy(dest, m_buffer + m_head, count);
			}

			m_head = (m_head + count) % BufferSize;
			m_size -= count;
		}

		void BufferedStreamReader::FillBuffer()
		{
			int32 count = BufferSize - m_size;
			assert(count>0);
			int32 actualCount = 0;

			if (m_tail + count > BufferSize)
			{
				int32 numTailToEnd = BufferSize - m_tail;
				int32 actual = static_cast<int32>(m_baseStream->Read(m_buffer + m_tail, numTailToEnd));
				m_endofStream = actual < numTailToEnd;
				actualCount = actual;

				if (!m_endofStream)
				{
					int32 remaining = count - numTailToEnd;
					assert(remaining<=m_tail);

					actual = static_cast<int32>(m_baseStream->Read(m_buffer, remaining));
					m_endofStream = actual < remaining;
					actualCount += actual;
				}
			}
			else
			{
				int32 actual = static_cast<int32>(m_baseStream->Read(m_buffer + m_tail, count));
				m_endofStream = actual < count;
				actualCount = actual;
			}

			m_tail = (m_tail + actualCount) % BufferSize;
			m_size+=actualCount;
		}

	};
};