#pragma once
#ifndef APOC3D_STREAM_H
#define APOC3D_STREAM_H

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

#include "apoc3d/Collections/List.h"
#include "apoc3d/Collections/Queue.h"
#include <fstream>

using namespace Apoc3D::Collections;

namespace Apoc3D
{
	namespace IO
	{
		/** Defines represent reference points in streams for seeking. */
		enum struct SeekMode
		{
			Begin,
			Current,
			End
		};

		/** Provides a generic access of a sequence of bytes. */
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
			
			virtual void Write(const char* src, int64 count) = 0;

			virtual void Seek(int64 offset, SeekMode mode) = 0;

			virtual void Flush() = 0;

			int ReadByte();
			void WriteByte(byte value);

			char* ReadAllToAllocatedBuffer();

			bool isEndofStream() { return getPosition() >= getLength(); }

		};

		/** Provides read-only access to a file as a stream */
		class APAPI FileStream : public Stream
		{
			RTTI_DERIVED(FileStream, Stream);
		public:
			static const int32 SequencialCountTrigger = 16;

			FileStream(const String& filename);
			FileStream(FileStream&& o);
			virtual ~FileStream();

			virtual bool IsReadEndianIndependent() const override { return true; }
			virtual bool IsWriteEndianIndependent() const override { return true; }

			virtual bool CanRead() const override { return true; }
			virtual bool CanWrite() const override { return false; }

			virtual int64 getLength() const override { return m_length; }

			virtual void setPosition(int64 offset) override;
			virtual int64 getPosition() override;

			virtual int64 Read(char* dest, int64 count) override;
			virtual void Write(const char* src, int64 count) override;

			virtual void Seek(int64 offset, SeekMode mode) override;
			
			virtual void Flush() override { }

		private:
			void ClearReadBuffer();
			
			int64 ReadImpl(char* dest, int64 count);
			void SeekImpl(int64 offset, SeekMode mode);
			int64 GetPositionImpl();

			bool isBuffered() const { return m_sequentialCount >= SequencialCountTrigger; }
//#define USE_WIN32_FILE
#ifdef USE_WIN32_FILE// APOC3D_PLATFORM == APOC3D_PLATFORM_WINDOWS
			void* m_file;
#else
			std::ifstream m_in;
#endif

			int64 m_length;

			int32 m_sequentialCount = 0;
			int32 m_endofStream = false;
			FixedQueue<char, 4096> m_readBuffer;
		};

		/** Provides write-only access to a file as a stream */
		class APAPI FileOutStream : public Stream
		{
			RTTI_DERIVED(FileOutStream, Stream);
		public:
			FileOutStream(const String& filename, bool noTrunc = false);
			FileOutStream(FileOutStream& o);
			virtual ~FileOutStream();

			virtual bool IsReadEndianIndependent() const override { return true; }
			virtual bool IsWriteEndianIndependent() const override { return true; }

			virtual bool CanRead() const override { return false; }
			virtual bool CanWrite() const override { return true; }

			virtual int64 getLength() const override { return m_length; }

			virtual void setPosition(int64 offset) override;
			virtual int64 getPosition() override;

			virtual int64 Read(char* dest, int64 count) override;
			virtual void Write(const char* src, int64 count) override;

			virtual void Seek(int64 offset, SeekMode mode) override;
			
			virtual void Flush() override;

		private:
			std::ofstream m_out;
			int64 m_length = 0;
			char m_buffer[4096];
		};

		/** Provides access to a space in memory as a stream */
		class APAPI MemoryStream : public Stream
		{
			RTTI_DERIVED(MemoryStream, Stream);
		public:
			MemoryStream(const char* data, int64 length)
				: m_data(const_cast<char*>(data))
				, m_length(length)
				, m_readonly(true) { }

			MemoryStream(char* data, int64 length)
				: m_data(data), m_length(length) { }

			virtual ~MemoryStream()
			{ }

			virtual bool IsReadEndianIndependent() const override { return false; }
			virtual bool IsWriteEndianIndependent() const override { return false; }

			virtual bool CanRead() const override { return true; }
			virtual bool CanWrite() const override { return !m_readonly; }

			virtual int64 getLength() const override { return m_length; }
			virtual void setPosition(int64 offset) override { m_position = offset; }
			virtual int64 getPosition() override { return m_position; }

			virtual int64 Read(char* dest, int64 count) override;
			virtual void Write(const char* src, int64 count) override;
			
			virtual void Seek(int64 offset, SeekMode mode) override;
			
			virtual void Flush() override { }

		private:
			NO_INLINE static void EndofStreamError();

			int64 m_length = 0;
			char* m_data = nullptr;
			int64 m_position = 0;
			bool m_readonly = false;
		};

		/** 
		 *  Provides access to a portion of another stream as a new stream
		 *  Close operation done on this stream will not affect the parent stream.
		 */
		class APAPI VirtualStream : public Stream
		{
			RTTI_DERIVED(VirtualStream, Stream);
		public:
			VirtualStream(Stream* strm);
			VirtualStream(Stream* strm, int64 baseOffset);
			VirtualStream(Stream* strm, int64 baseOffset, int64 length);
			VirtualStream(Stream* strm, int64 baseOffset, int64 length, bool releaseStream);
			~VirtualStream();

			VirtualStream(VirtualStream&& other);
			VirtualStream& operator=(VirtualStream&& other);

			VirtualStream(const VirtualStream&) = delete;
			VirtualStream& operator=(const VirtualStream&) = delete;

			virtual bool IsReadEndianIndependent() const override { return m_baseStream->IsReadEndianIndependent(); }
			virtual bool IsWriteEndianIndependent() const override { return m_baseStream->IsWriteEndianIndependent(); }

			virtual bool CanRead() const override { return m_baseStream->CanRead(); }
			virtual bool CanWrite() const override { return m_baseStream->CanWrite(); }

			virtual int64 getLength() const override;

			virtual void setPosition(int64 offset) override;
			virtual int64 getPosition() override;

			virtual int64 Read(char* dest, int64 count) override;
			virtual void Write(const char* src, int64 count) override;
			virtual void Seek(int64 offset, SeekMode mode) override;

			virtual void Flush() override { m_baseStream->Flush(); }

			Stream* getBaseStream() const { return m_baseStream; }
			bool isOutput() const { return m_isOutput; }

			int64 getBaseOffset() const { return m_baseOffset; }
			int64 getAbsolutePosition() const { return m_baseStream->getPosition(); }

		private:
			Stream* m_baseStream;
			int64 m_length;
			int64 m_baseOffset = 0;

			bool m_isOutput = false;
			bool m_releaseStream = false;
		};

		/** Provides access to a dynamic length of space in memory as a stream */
		class APAPI MemoryOutStream : public Stream
		{
			RTTI_DERIVED(MemoryOutStream, Stream);
		public:
			MemoryOutStream(int64 preserved)
				: m_data((int32)preserved) { }

			MemoryOutStream(MemoryOutStream&&);
			MemoryOutStream& operator=(MemoryOutStream&&);

			virtual ~MemoryOutStream()
			{ }

			virtual bool IsReadEndianIndependent() const override { return false; }
			virtual bool IsWriteEndianIndependent() const override { return false; }

			virtual bool CanRead() const override { return true; }
			virtual bool CanWrite() const override { return true; }

			virtual int64 getLength() const override { return m_length; }

			virtual void setPosition(int64 offset) override { m_position = offset; }
			virtual int64 getPosition() override { return m_position; }

			virtual int64 Read(char* dest, int64 count) override;
			virtual void Write(const char* src, int64 count) override;

			virtual void Seek(int64 offset, SeekMode mode) override;
			
			virtual void Flush() override { }

			void Clear();

			const char* getDataPointer() const { return m_data.getElements(); }
			char* getDataPointer() { return m_data.getElements(); }

			char* AllocateArrayCopy() const { return m_data.AllocateArrayCopy(); }

		private:
			int64 m_length = 0;
			int64 m_position = 0;
			List<char> m_data;
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
			static const int32 BufferSize = 4096;

			BufferedStreamReader(Stream* strm)
				: m_baseStream(strm)
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
			bool m_endofStream = false;

			char m_buffer[BufferSize];

			int32 m_head = 0;
			int32 m_tail = 0;
			int32 m_size = 0;
		};
	};
}

#endif