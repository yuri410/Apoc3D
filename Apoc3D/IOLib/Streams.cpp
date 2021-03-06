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

#include "Streams.h"
#include "apoc3d/Math/Math.h"

#ifdef USE_WIN32_FILE
#include <Windows.h>
#endif

using namespace std;

namespace Apoc3D
{
	namespace IO
	{
		int Stream::ReadByte()
		{
			char buffer;
			if (Read(&buffer, 1) == 0)
			{
				return -1;
			}
			return reinterpret_cast<const byte&>(buffer);
		}

		void Stream::WriteByte(byte value) 
		{
			Write(reinterpret_cast<const char*>(&value), 1); 
		}

		char* Stream::ReadAllToAllocatedBuffer()
		{
			int64 len = getLength();
			char* buffer = new char[(size_t)len];
			Read(buffer, len);
			return buffer;
		}

		/************************************************************************/
		/*  FileStream                                                          */
		/************************************************************************/

		FileStream::FileStream(const String& filename)
		{
#ifdef USE_WIN32_FILE
			m_file = CreateFile(filename.c_str(), GENERIC_READ, 0, 0, OPEN_EXISTING, FILE_FLAG_OPEN_NO_RECALL, NULL);
			assert(m_file != INVALID_HANDLE_VALUE);

			LARGE_INTEGER pos = { 0 };
			LARGE_INTEGER endPos = { 0 };

			SetFilePointerEx(m_file, pos, &endPos, FILE_END);
			SetFilePointerEx(m_file, pos, NULL, FILE_BEGIN);

			m_length = endPos.QuadPart;
#else
			m_in.rdbuf()->pubsetbuf(0, 0);

			m_in.open(filename.c_str(), ios::in | ios::binary);

			m_in.exceptions(std::ios::failbit);

			uint64 oldPos = m_in.tellg();
			m_in.seekg(0, ios::end);
			m_length = m_in.tellg();
			m_in.seekg(oldPos, ios::beg);

			m_in.exceptions(std::ios::goodbit);
#endif	
		}

		FileStream::FileStream(FileStream&& o)
#ifdef USE_WIN32_FILE
			: m_file(o.m_file) 
#else
			: m_in(std::move(o.m_in))
#endif
			, m_length(o.m_length)
			, m_sequentialCount(o.m_sequentialCount)
			, m_endofStream(o.m_endofStream)
		{
			m_readBuffer = o.m_readBuffer;

#ifdef USE_WIN32_FILE
			o.m_file = INVALID_HANDLE_VALUE;
#endif
		}

		FileStream::~FileStream()
		{
#ifdef USE_WIN32_FILE
			if (m_file != INVALID_HANDLE_VALUE)
			{
				CloseHandle(m_file);
			}
#else
			m_in.close();
#endif	

		}

		int64 FileStream::Read(char* dest, int64 count)
		{
			if (!isBuffered())
			{
				m_sequentialCount++;

				return ReadImpl(dest, count);
			}
			else
			{
				if (count > m_readBuffer.getCapacity())
				{
					int32 existingCount = m_readBuffer.getCount();
					m_readBuffer.CopyTo(dest, existingCount);
					count -= existingCount;
					dest += existingCount;
					m_readBuffer.Clear();

					int64 actual = ReadImpl(dest, count);

					return actual + existingCount;
				}
				else
				{
					if (!m_endofStream && m_readBuffer.getCount() < count)
					{
						// refill buffer
						int32 toFill = m_readBuffer.getCapacity() - m_readBuffer.getCount();
						if (toFill > 0)
						{
							char buf[4096];

							//m_in.read(buf, toFill);

							int32 actual = static_cast<int32>(ReadImpl(buf, toFill));// static_cast<int32>(m_in.gcount());
							m_readBuffer.Enqueue(buf, actual);

							m_endofStream |= actual < toFill;
						}
					}

					count = Math::_Min<int64>(count, m_readBuffer.getCount());
					m_readBuffer.CopyTo(dest, static_cast<int32>(count));
					m_readBuffer.DequeueOnly(static_cast<int32>(count));
					return count;
				}	
			}
		}
		void FileStream::Write(const char* src, int64 count)
		{
			AP_EXCEPTION(ErrorID::NotSupported, L"Can't write");
		}

		void FileStream::Seek(int64 offset, SeekMode mode)
		{
			if (mode == SeekMode::Current && isBuffered())
			{
				mode = SeekMode::Begin;
				offset -= (int64)m_readBuffer.getCount();
			}

			ClearReadBuffer();

			SeekImpl(offset, mode);
		}
		
		void FileStream::setPosition(int64 offset)
		{
			ClearReadBuffer();

			SeekImpl(offset, SeekMode::Begin);
		}

		int64 FileStream::getPosition()
		{
			if (!isBuffered())
				return GetPositionImpl();
			return GetPositionImpl() - (int64)m_readBuffer.getCount(); 
		}

		void FileStream::ClearReadBuffer()
		{
			if (isBuffered())
			{
				m_sequentialCount = 0;

				if (m_endofStream)
				{
					m_endofStream = false;
#ifndef USE_WIN32_FILE
					m_in.clear(ios::eofbit);
#endif
				}
				m_readBuffer.Clear();
			}
			else m_sequentialCount = 0;
		}


		int64 FileStream::ReadImpl(char* dest, int64 count)
		{
#ifdef USE_WIN32_FILE
			DWORD amount = (DWORD)count;
			DWORD actuall;
			ReadFile(m_file, dest, amount, &actuall, NULL);
			return actuall;
#else
			m_in.read(dest, count);
			return m_in.gcount();
#endif
		}

		void FileStream::SeekImpl(int64 offset, SeekMode mode)
		{
#ifdef USE_WIN32_FILE
			DWORD movMethod = FILE_BEGIN;
			LARGE_INTEGER movDistance;
			movDistance.QuadPart = offset;

			if (mode == SeekMode::Current)
				movMethod = FILE_CURRENT;
			else if (mode == SeekMode::End)
				movMethod = FILE_END;

			SetFilePointerEx(m_file, movDistance, NULL, movMethod);
#else
			ios::seekdir dir = ios::beg;

			if (mode == SeekMode::Current)
				dir = ios::cur;
			else if (mode == SeekMode::End)
				dir = ios::end;

			m_in.seekg(offset, dir);
#endif
		}

		int64 FileStream::GetPositionImpl()
		{
#ifdef USE_WIN32_FILE
			LARGE_INTEGER curPos;
			LARGE_INTEGER d = {0};
			SetFilePointerEx(m_file, d, &curPos, FILE_CURRENT);
			return curPos.QuadPart;
#else
			return m_in.tellg();
#endif
		}

		/************************************************************************/
		/*  FileOutStream                                                       */
		/************************************************************************/

		FileOutStream::FileOutStream(const String& filename, bool noTrunc)
		{
			m_out.rdbuf()->pubsetbuf(m_buffer, countof(m_buffer));

			std::ios::openmode mode = ios::out | ios::binary;
			if (!noTrunc)
				mode |= ios::trunc;
			else
				mode |= ios::in;

			m_out.open(filename.c_str(), mode);
		}

		FileOutStream::FileOutStream(FileOutStream& o)
			: m_out(std::move(o.m_out))
			, m_length(o.m_length)
		{
			memcpy(m_buffer, o.m_buffer, sizeof(m_buffer));
		}

		FileOutStream::~FileOutStream()
		{
			m_out.close();
		}

		int64 FileOutStream::Read(char* dest, int64 count)
		{
			AP_EXCEPTION(ErrorID::NotSupported, L"Can't read");
			return 0;
		}
		void FileOutStream::Write(const char* src, int64 count)
		{
			m_out.write(src, count);
			//m_length += count;
			int64 p = getPosition();
			if (p > m_length)
				m_length = p;
		}
		void FileOutStream::Seek(int64 offset, SeekMode mode)
		{
			ios::seekdir dir;
			switch (mode)
			{
				case SeekMode::Begin: dir = ios::beg; break;
				case SeekMode::End: dir = ios::end; break;
				default: dir = ios::cur; break;
			}

			m_out.seekp(offset, dir);
			int64 p = getPosition();
			if (p > m_length)
				m_length = p;
		}

		void FileOutStream::Flush()
		{
			m_out.flush();
		}
		void FileOutStream::setPosition(int64 offset)
		{
			m_out.seekp(offset, ios::beg);
			if (m_length < offset)
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
			if (m_readonly)
				return;

			if (m_position + count > m_length)
			{
				EndofStreamError();
				return;
			}
			
			memcpy(m_data + m_position, src, static_cast<size_t>(count));			
			m_position += count;
		}

		void MemoryStream::Seek(int64 offset, SeekMode mode)
		{
			switch (mode)
			{
				case SeekMode::Begin:   m_position = offset; break;
				case SeekMode::Current: m_position += offset; break;
				case SeekMode::End:     m_position = m_length + offset; break;
			}
			if (m_position < 0)
				m_position = 0;
			if (m_position > m_length)
				m_position = m_length;
		}

		void MemoryStream::EndofStreamError()
		{
			AP_EXCEPTION(ErrorID::EndOfStream, L"MemoryStream");
		}

		/************************************************************************/
		/*  VirtualStream                                                       */
		/************************************************************************/

		VirtualStream::VirtualStream(Stream* strm)
			: m_baseStream(strm), m_isOutput(true), m_length(strm->getLength())
		{
			strm->setPosition(0);
		}

		VirtualStream::VirtualStream(Stream* strm, int64 baseOffset)
			: m_baseStream(strm), m_isOutput(true), m_baseOffset(baseOffset)
		{
			strm->setPosition(baseOffset);
		}

		VirtualStream::VirtualStream(Stream* strm, int64 baseOffset, int64 length)
			: VirtualStream(strm, baseOffset, length, false) { }

		VirtualStream::VirtualStream(Stream* strm, int64 baseOffset, int64 length, bool releaseStream)
			: m_baseStream(strm), m_length(length), m_baseOffset(baseOffset), m_releaseStream(releaseStream)
		{
			strm->setPosition(baseOffset);
		}

		VirtualStream::~VirtualStream()
		{
			if (m_releaseStream)
				DELETE_AND_NULL(m_baseStream);
		}

		VirtualStream::VirtualStream(VirtualStream&& other)
			: m_baseStream(other.m_baseStream), m_length(other.m_length), m_baseOffset(other.m_baseOffset),
			m_isOutput(other.m_isOutput), m_releaseStream(other.m_releaseStream)
		{
			other.m_releaseStream = false;
			other.m_baseStream = 0;
		}

		VirtualStream& VirtualStream::operator=(VirtualStream&& other)
		{
			if (this != &other)
			{
				this->~VirtualStream();
				new (this)VirtualStream(std::move(other));
			}
			return *this;
		}
		int64 VirtualStream::getLength() const
		{
			return m_isOutput ? m_baseStream->getLength() : m_length;
		}


		void VirtualStream::setPosition(int64 offset)
		{
			m_baseStream->setPosition( offset + m_baseOffset );
		}
		int64 VirtualStream::getPosition() 
		{
			return m_baseStream->getPosition() - m_baseOffset;
		}

		int64 VirtualStream::Read(char* dest, int64 count)
		{
			//if (getPosition() + count > getLength())
			//{
			//	count = getLength() - getPosition();
			//}
			return m_baseStream->Read(dest, count);
		}
		void VirtualStream::Write(const char* src, int64 count)
		{
			m_baseStream->Write(src, count);
		}

		void VirtualStream::Seek(int64 offset, SeekMode mode)
		{
			switch (mode)
			{
				case SeekMode::Begin:

					if (offset > m_length)
						offset = m_length;
					if (offset < 0)
						offset = 0;

					m_baseStream->setPosition(offset + m_baseOffset);
					break;

				case SeekMode::Current:
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

				case SeekMode::End:
					if (offset > 0)
						offset = 0;
					if (offset < -m_length)
						offset = -m_length;

					m_baseStream->setPosition(m_length - offset + m_baseOffset);
					break;

			}
		}

		/************************************************************************/
		/* MemoryOutStream                                                      */
		/************************************************************************/

		MemoryOutStream::MemoryOutStream(MemoryOutStream&& rhs)
			: m_data(std::move(rhs.m_data)), m_position(rhs.m_position), m_length(rhs.m_length)
		{ }

		MemoryOutStream& MemoryOutStream::operator=(MemoryOutStream&& rhs)
		{
			if (this != &rhs)
			{
				m_data = std::move(rhs.m_data);
				m_length = rhs.m_length;
				m_position = rhs.m_position;
			}
			return *this;
		}

		int64 MemoryOutStream::Read(char* dest, int64 count)
		{
			if (m_position + count > m_length)
			{
				count = m_length - m_position;
			}

			for (int64 i = 0; i < count; i++)
				dest[i] = m_data[static_cast<int32>(i + m_position)];

			m_position += count;
			return count;
		}
		void MemoryOutStream::Write(const char* src, int64 count)
		{
			for (int64 i = 0; i < count; i++)
			{
				if (m_position >= m_length)
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
				case SeekMode::Begin:   m_position = offset; break;
				case SeekMode::Current: m_position += offset; break;
				case SeekMode::End:     m_position = m_length + offset; break;
			}
			if (m_position < 0)
				m_position = 0;
			if (m_position > m_length)
				m_position = m_length;
		}

		void MemoryOutStream::Clear()
		{
			m_data.Clear();
			m_length = 0;
			m_position = 0;
		}

		/************************************************************************/
		/* PipeStream                                                           */
		/************************************************************************/
		PipeOutStream::~PipeOutStream()
		{ }

		void PipeOutStream::PopAll(char* dest)
		{
			if (dest)
			{
				memcpy(dest, getDataPointer(), (size_t)getLength());
			}

			Clear();
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
				if (existing > 0)
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
				memcpy(dest, m_buffer + m_head, numHeadToEnd);

				int32 remaining = count - numHeadToEnd;
				assert(remaining <= m_head);
				memcpy(dest + numHeadToEnd, m_buffer, remaining);
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
			assert(count > 0);
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
					assert(remaining <= m_tail);

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
			m_size += actualCount;
		}

	};
};