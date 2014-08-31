#pragma once
#ifndef APOC3D_STREAM_H
#define APOC3D_STREAM_H

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

#include "apoc3d/Collections/List.h"
#include <fstream>

using namespace Apoc3D::Collections;

namespace Apoc3D
{
	namespace IO
	{
		/** 
		 *  Defines represent reference points in streams for seeking.
		 */
		enum SeekMode
		{
			SEEK_Begin,
			SEEK_Current,
			SEEK_End
		};
		/**
		 *  Provides a generic access of a sequence of bytes.
		 */
		class APAPI Stream
		{
			RTTI_BASE;
		protected:
			Stream() { }
		public:
			virtual ~Stream() { }

			virtual bool IsReadEndianIndependent() const = 0;
			virtual bool IsWriteEndianIndependent() const = 0;

			virtual bool CanRead() const = 0;
			virtual bool CanWrite() const = 0;

			virtual int64 getLength() const = 0;

			virtual void setPosition(int64 offset) = 0;
			virtual int64 getPosition() = 0;

			virtual int64 Read(char* dest, int64 count) = 0;
			int ReadByte()
			{
				char buffer;
				if (Read(&buffer, 1) == 0)
				{
					return -1;
				}
				return reinterpret_cast<const byte&>(buffer);
			}

			virtual void Write(const char* src, int64 count) = 0;

			void WriteByte(byte value) { Write(reinterpret_cast<const char*>(&value), 1); }


			virtual void Seek(int64 offset, SeekMode mode) = 0;

			virtual void Close() = 0;

			virtual void Flush() = 0;

		};

		/**
		 *  Provides read-only access to a file as a stream
		 */
		class APAPI FileStream : public Stream
		{
			RTTI_DERIVED(FileStream, Stream);
		public:
			FileStream(const String& filename);
			virtual ~FileStream();

			virtual bool IsReadEndianIndependent() const { return true; }
			virtual bool IsWriteEndianIndependent() const { return true; }

			virtual bool CanRead() const { return true; }
			virtual bool CanWrite() const { return false; }

			virtual int64 getLength() const { return m_length; }

			virtual void setPosition(int64 offset);
			virtual int64 getPosition();

			virtual int64 Read(char* dest, int64 count);
			virtual void Write(const char* src, int64 count);

			virtual void Seek(int64 offset, SeekMode mode);
			virtual void Close();

			virtual void Flush() { }

		private:
			std::ifstream m_in;
			int64 m_length;
		};
		/**
		 *  Provides write-only access to a file as a stream
		 */
		class APAPI FileOutStream : public Stream
		{
			RTTI_DERIVED(FileOutStream, Stream);
		public:
			FileOutStream(const String& filename);
			virtual ~FileOutStream();

			virtual bool IsReadEndianIndependent() const { return true; }
			virtual bool IsWriteEndianIndependent() const { return true; }

			virtual bool CanRead() const { return false; }
			virtual bool CanWrite() const { return true; }

			virtual int64 getLength() const { return m_length; }

			virtual void setPosition(int64 offset);
			virtual int64 getPosition();

			virtual int64 Read(char* dest, int64 count);
			virtual void Write(const char* src, int64 count);

			virtual void Seek(int64 offset, SeekMode mode);
			virtual void Close();

			virtual void Flush();

		private:
			std::ofstream m_out;
			int64 m_length;
		};
		/**
		 *  Provides access to a space in memory as a stream
		 */
		class APAPI MemoryStream : public Stream
		{
			RTTI_DERIVED(MemoryStream, Stream);
		public:
			virtual bool IsReadEndianIndependent() const { return false; }
			virtual bool IsWriteEndianIndependent() const { return false; }

			MemoryStream(char* data, int64 length)
				: m_data(data), m_length(length), m_position(0)
			{ }
			virtual ~MemoryStream()
			{
			}

			virtual bool CanRead() const { return true; }
			virtual bool CanWrite() const { return true; }

			virtual int64 getLength() const { return m_length; }
			virtual void setPosition(int64 offset) { m_position = offset; }
			virtual int64 getPosition() { return m_position; }

			virtual int64 Read(char* dest, int64 count);
			virtual void Write(const char* src, int64 count);
			
			virtual void Seek(int64 offset, SeekMode mode);
			virtual void Close() { }

			virtual void Flush() { }

			char* getDataPointer() const { return m_data; }

		private:
			NO_INLINE static void throwEndofStreamException();

			int64 m_length;
			char* m_data;
			int64 m_position;
		};

		/** 
		 *  Provides access to a portion of another stream as a new stream
		 *  Close operation done on this stream will not affect the parent stream.
		 */
		class APAPI VirtualStream : public Stream
		{
			RTTI_DERIVED(VirtualStream, Stream);
		public:
			virtual bool IsReadEndianIndependent() const { return m_baseStream->IsReadEndianIndependent(); }
			virtual bool IsWriteEndianIndependent() const { return m_baseStream->IsWriteEndianIndependent(); }

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
			virtual int64 getPosition()
			{ 
				return m_baseStream->getPosition() - m_baseOffset;
			}

			VirtualStream(Stream* strm)
				: m_baseStream(strm), m_isOutput(true), m_length(strm->getLength()), m_baseOffset(0)
			{
				strm->setPosition(0);
			}
			VirtualStream(Stream* strm, int64 baseOffset)
				: m_baseStream(strm), m_isOutput(true), m_baseOffset(baseOffset)
			{
				strm->setPosition(baseOffset);
			}

			VirtualStream(Stream* strm, int64 baseOffset, int64 length)
				: m_baseStream(strm), m_length(length), m_baseOffset(baseOffset), m_isOutput(false)
			{
				strm->setPosition(baseOffset);
			}


			virtual int64 Read(char* dest, int64 count)
			{
				//if (getPosition() + count > getLength())
				//{
				//	count = getLength() - getPosition();
				//}
				if (count)
				{
					m_baseStream->Read(dest, count);
				}

				return count;
			}
			virtual void Write(const char* src, int64 count)
			{
				m_baseStream->Write(src, count);
			}
			virtual void Seek(int64 offset, SeekMode mode);

			virtual void Close() { }

			virtual void Flush() { m_baseStream->Flush(); }
		private:
			Stream* m_baseStream;
			int64 m_length;
			int64 m_baseOffset;

			bool m_isOutput;

		};

		/** 
		 *  Provides access to a dynamic length of space in memory as a stream
		 */
		class APAPI MemoryOutStream : public Stream
		{
			RTTI_DERIVED(MemoryOutStream, Stream);
		public:
			MemoryOutStream(int64 preserved)
				: m_length(0), m_position(0), m_data((int32)preserved)
			{
				
			}
			virtual ~MemoryOutStream()
			{
				
			}

			virtual bool IsReadEndianIndependent() const { return false; }
			virtual bool IsWriteEndianIndependent() const { return false; }

			virtual bool CanRead() const { return true; }
			virtual bool CanWrite() const { return true; }

			virtual int64 getLength() const { return m_length; }

			virtual void setPosition(int64 offset) { m_position = offset; }
			virtual int64 getPosition() { return m_position; }

			virtual int64 Read(char* dest, int64 count);
			virtual void Write(const char* src, int64 count);

			virtual void Seek(int64 offset, SeekMode mode);
			virtual void Close() { }

			virtual void Flush() { }

			const char* getDataPointer() const { return m_data.getElements(); }
			char* getDataPointer() { return m_data.getElements(); }

		private:
			int64 m_length;
			List<char> m_data;
			int64 m_position;
		};

		class APAPI PipeOutStream : public MemoryOutStream
		{
			RTTI_DERIVED(PipeOutStream, MemoryOutStream);
		public:
			PipeOutStream(int64 preserved)
				: MemoryOutStream(preserved)
			{ }

			virtual ~PipeOutStream();

			void PopAll(char* dest);
		};


		/**
		 *  This reader can read Stream with an internal buffer to 
		 *  avoid frequent calls to Stream's read methods (which is expensive for various reasons)
		 */
		class APAPI BufferedStreamReader
		{
		public:
			static const int32 BufferSize = 8192;

			BufferedStreamReader(Stream* strm)
				: m_baseStream(strm), m_endofStream(false),
				m_head(0), m_tail(0), m_size(0)
			{
				assert(strm->CanRead()); 
			}
			~BufferedStreamReader() 
			{
			}

			bool IsReadEndianIndependent() const { return m_baseStream->IsReadEndianIndependent(); }
			int32 getLength() const { return static_cast<int32>(m_baseStream->getLength()); }

			int32 Read(char* dest, int32 count);
			bool ReadByte(char& result);
		private:
			void ClearBuffer();
			void ReadBuffer(char* dest, int32 count);
			void FillBuffer();

			int getBufferContentSize() const { return m_size; }



			Stream* m_baseStream;
			bool m_endofStream;

			char m_buffer[BufferSize];

			int32 m_head;
			int32 m_tail;
			int32 m_size;
		};
	};
}

#endif