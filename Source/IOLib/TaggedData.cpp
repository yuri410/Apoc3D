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
#include "TaggedData.h"
#include "BinaryReader.h"
#include "BinaryWriter.h"
#include "Streams.h"
#include "Apoc3DException.h"

#include "IOUtils.h"

namespace Apoc3D
{
	namespace IO
	{
		TaggedDataReader::TaggedDataReader(Stream* strm)
			: m_stream(strm)
		{
			m_endianDependent = strm->IsReadEndianDependent();

			BinaryReader* br = new BinaryReader(new VirtualStream(strm, 0, strm->getLength()));

			m_sectCount = br->ReadInt32();

			for (int i=0; i<m_sectCount; i++)
			{
				String name = br->ReadString();

				uint size = br->ReadUInt32();

				m_positions.insert(pair<String, Entry>(name, Entry(name, strm->getPosition(), size)));
				strm->Seek(size, SEEK_Current);
			}

			br->Close();
			delete br;
		}
		TaggedDataReader::~TaggedDataReader()
		{
			delete m_stream;
		}
		void TaggedDataReader::FillBuffer(const String& name, uint32 len)
		{
			const Entry* ent = FindEntry(name);
			m_stream->setPosition(ent->Offset);
			m_stream->Read(m_buffer, len);
		}
		bool TaggedDataReader::TryFillBuffer(const String& name, uint32 len)
		{
			const Entry* ent = FindEntry(name);
			if (!ent) return false;
			m_stream->setPosition(ent->Offset);
			m_stream->Read(m_buffer, len);
			return true;
		}

		BinaryReader* TaggedDataReader::TryGetData(const String& name) const
		{
			const Entry* ent = FindEntry(name);
			if (ent)
			{
				return new BinaryReader(new VirtualStream(m_stream, ent->Offset, ent->Size));
			}
			return 0;
		}
		BinaryReader* TaggedDataReader::GetData(const String& name) const
		{
			const Entry* ent = FindEntry(name);
			if (ent)
			{
				return new BinaryReader(new VirtualStream(m_stream, ent->Offset, ent->Size));
			}
			throw Apoc3DException::createException(EX_KeyNotFound, name.c_str());
		}


		Stream* TaggedDataReader::GetDataStream(const String& name) const 
		{
			const Entry* ent = FindEntry(name);
			if (ent)
			{
				return new VirtualStream(m_stream, ent->Offset, ent->Size);
			}
			throw Apoc3DException::createException(EX_KeyNotFound, name.c_str());
		}

		int64 TaggedDataReader::GetDataInt64(const String& name)
		{
			FillBuffer(name, sizeof(int64));
			return ci64_le(m_buffer);
		}
		uint64 TaggedDataReader::GetDataUInt64(const String& name)
		{
			FillBuffer(name, sizeof(uint64));
			return cui64_le(m_buffer);
		}
		int32 TaggedDataReader::GetDataInt32(const String& name)
		{
			FillBuffer(name, sizeof(int32));
			return ci32_le(m_buffer);
		}
		uint32 TaggedDataReader::GetDataUInt32(const String& name)
		{
			FillBuffer(name, sizeof(uint32));
			return cui32_le(m_buffer);
		}
		int16 TaggedDataReader::GetDataInt16(const String& name)
		{
			FillBuffer(name, sizeof(int16));
			return ci16_le(m_buffer);
		}
		uint16 TaggedDataReader::GetDataUInt16(const String& name)
		{
			FillBuffer(name, sizeof(uint16));
			return cui16_le(m_buffer);
		}
		bool TaggedDataReader::GetDataBool(const String& name)
		{
			FillBuffer(name, sizeof(bool));
			return !!m_buffer[0];
		}
		float TaggedDataReader::GetDataSingle(const String& name)
		{
			FillBuffer(name, sizeof(float));
			return cr32_le(m_buffer);
		}
		double TaggedDataReader::GetDataDouble(const String& name)
		{
			FillBuffer(name, sizeof(double));
			return cr64_le(m_buffer);
		}


		bool TaggedDataReader::TryGetDataInt64(const String& name, int64& v)
		{
			bool e = TryFillBuffer(name, sizeof(int64));
			if (e)
				v = ci64_le(m_buffer);
			return false;
		}
		bool TaggedDataReader::TryGetDataUInt64(const String& name, uint64& v)
		{
			bool e = TryFillBuffer(name, sizeof(uint64));
			if (e)
				v = cui64_le(m_buffer);
			return false;
		}
		bool TaggedDataReader::TryGetDataInt32(const String& name, int32& v)
		{
			bool e = TryFillBuffer(name, sizeof(int32));
			if (e)
				v = ci32_le(m_buffer);
			return false;
		}
		bool TaggedDataReader::TryGetDataUInt32(const String& name, uint32& v)
		{
			bool e = TryFillBuffer(name, sizeof(uint32));
			if (e)
				v = cui32_le(m_buffer);
			return false;
		}
		bool TaggedDataReader::TryGetDataInt16(const String& name, int16& v)
		{
			bool e = TryFillBuffer(name, sizeof(int16));
			if (e)
				v = ci16_le(m_buffer);
			return false;
		}
		bool TaggedDataReader::TryGetDataUInt16(const String& name, uint16& v)
		{
			bool e = TryFillBuffer(name, sizeof(uint16));
			if (e)
				v = cui16_le(m_buffer);
			return false;
		}
		bool TaggedDataReader::TryGetDataBool(const String& name, bool& v)
		{
			bool e = TryFillBuffer(name, sizeof(bool));
			if (e)
				v = !!m_buffer[0];
			return false;
		}
		bool TaggedDataReader::TryGetDataSingle(const String& name, float& v)
		{
			bool e = TryFillBuffer(name, sizeof(float));
			if (e)
				v = cr32_le(m_buffer);
			return false;
		}
		bool TaggedDataReader::TryGetDataDouble(const String& name, double& v)
		{
			bool e = TryFillBuffer(name, sizeof(double));
			if (e)
				v = cr64_le(m_buffer);
			return false;
		}

		int64 TaggedDataReader::GetChunkOffset(const String& name) const
		{
			const Entry* ent = FindEntry(name);
			if (!ent)
				throw Apoc3DException::createException(EX_KeyNotFound, name.c_str());

			return ent->Offset;
		}
		void TaggedDataReader::Close() 
		{
			m_stream->Close();
		}
		
		/************************************************************************/
		/* TaggedDataWriter                                                     */
		/************************************************************************/


		TaggedDataWriter::Entry::Entry(const String& name)
			: Name(name)
		{
			Buffer = new MemoryOutStream(1024);
		}


		TaggedDataWriter::~TaggedDataWriter()
		{
			for (auto e = m_positions.begin(); e!= m_positions.end(); e++)
			{
				MemoryOutStream* ee = e->second.Buffer;
				delete ee;
			}
			m_positions.clear();
		}



		BinaryWriter* TaggedDataWriter::AddEntry(const String& name)
		{
			Entry ent = Entry(name);
			m_positions.insert(pair<String, Entry>(name, ent));
			return new BinaryWriter(new VirtualStream(ent.Buffer, 0));
		}
		Stream* TaggedDataWriter::AddEntryStream(const String& name)
		{
			Entry ent = Entry(name);
			m_positions.insert(pair<String, Entry>(name, ent));
			return (new VirtualStream(ent.Buffer, 0));
		}

		
		

		void TaggedDataWriter::AddEntry(const String& name, int64 value)
		{
			Entry ent = Entry(name);
			m_positions.insert(pair<String, Entry>(name, ent));

			if (m_endianDependent)
			{
				i64tomb_dep(value, m_buffer);
			}
			else
			{
				i64tomb_le(value, m_buffer);
			}

			ent.Buffer->Write(m_buffer, sizeof(value));
		}
		void TaggedDataWriter::AddEntry(const String& name, uint64 value)
		{
			Entry ent = Entry(name);
			m_positions.insert(pair<String, Entry>(name, ent));

			if (m_endianDependent)
			{
				ui64tomb_dep(value, m_buffer);
			}
			else
			{
				ui64tomb_le(value, m_buffer);
			}

			ent.Buffer->Write(m_buffer, sizeof(value));
		}
		void TaggedDataWriter::AddEntry(const String& name, int32 value)
		{
			Entry ent = Entry(name);
			m_positions.insert(pair<String, Entry>(name, ent));

			if (m_endianDependent)
			{
				i32tomb_dep(value, m_buffer);
			}
			else
			{
				i32tomb_le(value, m_buffer);
			}

			ent.Buffer->Write(m_buffer, sizeof(value));
		}
		void TaggedDataWriter::AddEntry(const String& name, uint32 value)
		{
			Entry ent = Entry(name);
			m_positions.insert(pair<String, Entry>(name, ent));

			if (m_endianDependent)
			{
				ui32tomb_dep(value, m_buffer);
			}
			else
			{
				ui32tomb_le(value, m_buffer);
			}

			ent.Buffer->Write(m_buffer, sizeof(value));
		}
		void TaggedDataWriter::AddEntry(const String& name, int16 value)
		{
			Entry ent = Entry(name);
			m_positions.insert(pair<String, Entry>(name, ent));

			if (m_endianDependent)
			{
				i16tomb_dep(value, m_buffer);
			}
			else
			{
				i16tomb_le(value, m_buffer);
			}

			ent.Buffer->Write(m_buffer, sizeof(value));
		}
		void TaggedDataWriter::AddEntry(const String& name, uint16 value)
		{
			Entry ent = Entry(name);
			m_positions.insert(pair<String, Entry>(name, ent));

			if (m_endianDependent)
			{
				ui16tomb_dep(value, m_buffer);
			}
			else
			{
				ui16tomb_le(value, m_buffer);
			}

			ent.Buffer->Write(m_buffer, sizeof(value));
		}
		void TaggedDataWriter::AddEntry(const String& name, float value)
		{
			Entry ent = Entry(name);
			m_positions.insert(pair<String, Entry>(name, ent));

			if (m_endianDependent)
			{
				r32tomb_dep(value, m_buffer);
			}
			else
			{
				r32tomb_le(value, m_buffer);
			}

			ent.Buffer->Write(m_buffer, sizeof(value));
		}
		void TaggedDataWriter::AddEntry(const String& name, double value)
		{
			Entry ent = Entry(name);
			m_positions.insert(pair<String, Entry>(name, ent));

			if (m_endianDependent)
			{
				r64tomb_dep(value, m_buffer);
			}
			else
			{
				r64tomb_le(value, m_buffer);
			}

			ent.Buffer->Write(m_buffer, sizeof(value));
		}
		void TaggedDataWriter::AddEntry(const String& name, bool value)
		{
			Entry ent = Entry(name);
			m_positions.insert(pair<String, Entry>(name, ent));

			m_buffer[0] = value ? 1 : 0;

			ent.Buffer->Write(m_buffer, sizeof(value));
		}

		BinaryWriter* TaggedDataWriter::GetData(const String& name)
		{
			const Entry* ent = FindEntry(name);
			return new BinaryWriter(new VirtualStream(ent->Buffer, 0));
		}

		void TaggedDataWriter::SetData(const String& name, int64 value)
		{
			const Entry* ent = FindEntry(name);

			if (m_endianDependent)
			{
				i64tomb_dep(value, m_buffer);
			}
			else
			{
				i64tomb_le(value, m_buffer);
			}
			ent->Buffer->setPosition(0);
			ent->Buffer->Write(m_buffer, sizeof(value));
		}
		void TaggedDataWriter::SetData(const String& name, uint64 value)
		{
			const Entry* ent = FindEntry(name);

			if (m_endianDependent)
			{
				ui64tomb_dep(value, m_buffer);
			}
			else
			{
				ui64tomb_le(value, m_buffer);
			}
			ent->Buffer->setPosition(0);
			ent->Buffer->Write(m_buffer, sizeof(value));
		}
		void TaggedDataWriter::SetData(const String& name, int32 value)
		{
			const Entry* ent = FindEntry(name);

			if (m_endianDependent)
			{
				i32tomb_dep(value, m_buffer);
			}
			else
			{
				i32tomb_le(value, m_buffer);
			}
			ent->Buffer->setPosition(0);
			ent->Buffer->Write(m_buffer, sizeof(value));
		}
		void TaggedDataWriter::SetData(const String& name, uint32 value)
		{
			const Entry* ent = FindEntry(name);

			if (m_endianDependent)
			{
				ui32tomb_dep(value, m_buffer);
			}
			else
			{
				ui32tomb_le(value, m_buffer);
			}
			ent->Buffer->setPosition(0);
			ent->Buffer->Write(m_buffer, sizeof(value));
		}
		void TaggedDataWriter::SetData(const String& name, int16 value)
		{
			const Entry* ent = FindEntry(name);

			if (m_endianDependent)
			{
				i16tomb_dep(value, m_buffer);
			}
			else
			{
				i16tomb_le(value, m_buffer);
			}
			ent->Buffer->setPosition(0);
			ent->Buffer->Write(m_buffer, sizeof(value));
		}
		void TaggedDataWriter::SetData(const String& name, uint16 value)
		{
			const Entry* ent = FindEntry(name);

			if (m_endianDependent)
			{
				ui16tomb_dep(value, m_buffer);
			}
			else
			{
				ui16tomb_le(value, m_buffer);
			}
			ent->Buffer->setPosition(0);
			ent->Buffer->Write(m_buffer, sizeof(value));
		}
		void TaggedDataWriter::SetData(const String& name, float value)
		{
			const Entry* ent = FindEntry(name);

			if (m_endianDependent)
			{
				r32tomb_dep(value, m_buffer);
			}
			else
			{
				r32tomb_le(value, m_buffer);
			}
			ent->Buffer->setPosition(0);
			ent->Buffer->Write(m_buffer, sizeof(value));
		}
		void TaggedDataWriter::SetData(const String& name, double value)
		{
			const Entry* ent = FindEntry(name);

			if (m_endianDependent)
			{
				r64tomb_dep(value, m_buffer);
			}
			else
			{
				r64tomb_le(value, m_buffer);
			}
			ent->Buffer->setPosition(0);
			ent->Buffer->Write(m_buffer, sizeof(value));
		}
		void TaggedDataWriter::SetData(const String& name, bool value)
		{
			Entry ent = Entry(name);
			m_positions.insert(pair<String, Entry>(name, ent));

			m_buffer[0] = value ? 1 : 0;

			ent.Buffer->setPosition(0);
			ent.Buffer->Write(m_buffer, sizeof(value));
		}



		void TaggedDataWriter::Save(Stream* stream) const
		{
			BinaryWriter* bw = new BinaryWriter(stream);

			bw->Write(static_cast<uint32>(m_positions.size()));

			//int simulatedLen = 0;
			for (auto i = m_positions.begin();i!=m_positions.end();i++)
			{
				const MemoryOutStream* e = i->second.Buffer;
				bw->Write(i->first);
				bw->Write(static_cast<uint32>(e->getLength()));
				
				char buffer[4096];
				int bi = 0;
				for (auto j=0;j<e->getLength();j++)
				{
					buffer[bi++] = static_cast<char>(e->getPointer()->operator[](j));
					if (bi==4096)
					{
						bw->Write(buffer, 4096);
						//simulatedLen += 4096;
						//assert(simulatedLen + 76 == ((VirtualStream*)stream)->getBaseStream()->getLength());
						bi = 0;
					}
					
					//bw->Write(
						//static_cast<char>(e->getPointer()->operator[](j)));
				}
				if (bi)
					bw->Write(buffer, bi);

			}

			bw->Close();
			delete bw;
		}

	}
}
