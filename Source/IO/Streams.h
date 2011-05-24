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

#ifndef STREAM_H
#define STREAM_H

#include "Common.h"

using namespace std;

namespace Apoc3D
{
	namespace IO
	{
		enum APAPI SeekMode
		{
			SEEK_Begin,
			SEEK_Current,
			SEEK_End
		};
		/* 
		*/
		class APAPI Stream
		{
		protected:
			Stream() { }
		public:
			virtual ~Stream() { }

			virtual bool CanRead() const = 0;
			virtual bool CanWrite() const = 0;

			virtual int64 getLength() const = 0;

			virtual void setPosition(int64 offset) = 0;
			virtual int64 getPosition() const = 0;

			virtual int64 Read(char* dest, int64 count) = 0;
			virtual int ReadByte()
			{
				char buffer;
				if (Read(&buffer, 1) == 0)
				{
					return -1;
				}
				return reinterpret_cast<const byte&>(buffer);
			}

			
			virtual void Write(const char* src, int64 count) = 0;
			virtual void WriteByte(byte value)
			{
				Write(reinterpret_cast<const char*>(&value), 1);
			}

			virtual void Seek(int64 offset, SeekMode mode) = 0;

			virtual void Close() = 0;
		};

		class APAPI FileStream : public Stream
		{
		private:
			ifstream* m_in;
			int64 m_length;
		public:
			FileStream(const String& filename);
			virtual ~FileStream();

			virtual bool CanRead() const { return true; }
			virtual bool CanWrite() const { return false; }

			virtual int64 getLength() const
			{
				return m_length;
			}

			virtual void setPosition(int64 offset)
			{
				m_in->seekg(offset); 
			}
			virtual int64 getPosition() const
			{ 
				return m_in->tellg(); 
			}

			virtual int64 Read(char* dest, int64 count);
			virtual void Write(const char* src, int64 count);

			virtual void Seek(int64 offset, SeekMode mode);
			virtual void Close();
		};

		class APAPI MemoryStream : public Stream
		{
		private:
			int64 m_length;
			byte* m_data;
			int64 m_position;
		public:
			byte* getInternalPointer() const { return m_data; }

			MemoryStream(byte* data, int64 length)
				: m_data(data), m_length(length)
			{ }
			virtual ~MemoryStream()
			{
			}

			virtual bool CanRead() const { return true; }
			virtual bool CanWrite() const { return true; }


			virtual int64 getLength() const
			{
				return m_length;
			}

			virtual void setPosition(int64 offset)
			{
				m_position = offset;
			}
			virtual int64 getPosition() const
			{ 
				return m_position;
			}


			virtual int64 Read(char* dest, int64 count)
			{
				if (count > m_length)
				{
					count = m_length;
				}

				byte* dst = reinterpret_cast<byte*>(dest);
				memcpy(dst, m_data+m_position, static_cast<size_t>(count));
				
				m_position += count;
				return count;
			}
			virtual void Write(const char* src, int64 count);
			

			virtual void Seek(int64 offset, SeekMode mode)
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
			virtual void Close();
		};

		class APAPI VirtualStream : public Stream
		{
		private:
			Stream* m_baseStream;
			int64 m_length;
			int64 m_baseOffset;
			
			bool m_isOutput;

		public:
			Stream* getBaseStream() const { return m_baseStream; }
			bool isOutput() const { return m_isOutput; }

			int64 getBaseOffset() const { return m_baseOffset; }
			int64 getAbsolutePosition() const { return m_baseStream->getPosition(); }

			virtual bool CanRead() const { return m_baseStream->CanRead(); }
			virtual bool CanWrite() const { return m_baseStream->CanWrite(); }

			virtual int64 getLength() const
			{
				return m_isOutput ? m_baseStream->getLength() : m_length;
			}

			virtual void setPosition(int64 offset);
			virtual int64 getPosition() const
			{ 
				return m_baseStream->getPosition() - m_baseOffset;
			}

			VirtualStream(Stream* strm)
				: m_baseStream(strm), m_isOutput(true), m_length(strm->getLength())
			{
				strm->setPosition(0);
			}
			VirtualStream(Stream* strm, int64 baseOffset)
				: m_baseStream(strm), m_isOutput(true), m_baseOffset(baseOffset)
			{
				strm->setPosition(baseOffset);
			}

			VirtualStream(Stream* strm, int64 baseOffset, int64 length)
				: m_baseStream(strm), m_length(length), m_baseOffset(baseOffset)
			{
				strm->setPosition(baseOffset);
			}


			virtual int64 Read(char* dest, int64 count)
			{
				if (getPosition() + count > m_length)
				{
					count = m_length - getPosition();
				}

				return count;
			}
			virtual void Write(const char* src, int64 count);
			virtual void Seek(int64 offset, SeekMode mode)
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
					break;
				}
				return m_baseStream->Seek(offset, mode);
			}

			virtual void Close() { }
		};
	};
}

#endif