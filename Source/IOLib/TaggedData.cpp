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

#include "Math/Matrix.h"
#include "Math/Color.h"

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

				m_positions.Add(name, Entry(name, strm->getPosition(), size));
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
			m_stream->Read(reinterpret_cast<char*>(m_buffer), len);
		}

		bool TaggedDataReader::TryFillBuffer(const String& name, uint32 len)
		{
			const Entry* ent = FindEntry(name);
			if (!ent) return false;
			m_stream->setPosition(ent->Offset);
			m_stream->Read(reinterpret_cast<char*>(m_buffer), len);
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
			return e;
		}
		bool TaggedDataReader::TryGetDataUInt64(const String& name, uint64& v)
		{
			bool e = TryFillBuffer(name, sizeof(uint64));
			if (e)
				v = cui64_le(m_buffer);
			return e;
		}
		bool TaggedDataReader::TryGetDataInt32(const String& name, int32& v)
		{
			bool e = TryFillBuffer(name, sizeof(int32));
			if (e)
				v = ci32_le(m_buffer);
			return e;
		}
		bool TaggedDataReader::TryGetDataUInt32(const String& name, uint32& v)
		{
			bool e = TryFillBuffer(name, sizeof(uint32));
			if (e)
				v = cui32_le(m_buffer);
			return e;
		}
		bool TaggedDataReader::TryGetDataInt16(const String& name, int16& v)
		{
			bool e = TryFillBuffer(name, sizeof(int16));
			if (e)
				v = ci16_le(m_buffer);
			return e;
		}
		bool TaggedDataReader::TryGetDataUInt16(const String& name, uint16& v)
		{
			bool e = TryFillBuffer(name, sizeof(uint16));
			if (e)
				v = cui16_le(m_buffer);
			return e;
		}
		bool TaggedDataReader::TryGetDataBool(const String& name, bool& v)
		{
			bool e = TryFillBuffer(name, sizeof(bool));
			if (e)
				v = !!m_buffer[0];
			return e;
		}
		bool TaggedDataReader::TryGetDataSingle(const String& name, float& v)
		{
			bool e = TryFillBuffer(name, sizeof(float));
			if (e)
				v = cr32_le(m_buffer);
			return e;
		}
		bool TaggedDataReader::TryGetDataDouble(const String& name, double& v)
		{
			bool e = TryFillBuffer(name, sizeof(double));
			if (e)
				v = cr64_le(m_buffer);
			return e;
		}



		void TaggedDataReader::GetDataVector2(const String& name, Vector2& vec)
		{
			FillBuffer(name, sizeof(float) * 2);
			GetBufferVector2(vec);
		}
		void TaggedDataReader::GetDataVector3(const String& name, Vector3& vec)
		{
			FillBuffer(name, sizeof(float) * 3);
			GetBufferVector3(vec);
		}
		void TaggedDataReader::GetDataVector4(const String& name, Vector4& vec)
		{
			FillBuffer(name, sizeof(float) * 4);
			GetBufferVector4(vec);
		}
		void TaggedDataReader::GetDataMatrix(const String& name, Matrix& mat)
		{
			const Entry* ent = FindEntry(name);
			assert(ent);
			GetEntryMatrix(ent, mat);
		}
		void TaggedDataReader::GetDataColor4(const String& name, Color4& clr)
		{
			FillBuffer(name, sizeof(float) * 4);
			GetBufferColor4(clr);
		}
		void TaggedDataReader::GetDataString(const String& name, String& str)
		{
			const Entry* ent = FindEntry(name);
			assert(ent);
			GetEntryString(ent, str);
		}

		bool TaggedDataReader::TryGetVector2(const String& name, Vector2& vec)
		{
			bool e = TryFillBuffer(name, sizeof(float) * 2);
			if (e) GetBufferVector2(vec); 
			return e;
		}
		bool TaggedDataReader::TryGetVector3(const String& name, Vector3& vec)
		{
			bool e = TryFillBuffer(name, sizeof(float) * 3);
			if (e) GetBufferVector3(vec); 
			return e;
		}
		bool TaggedDataReader::TryGetVector4(const String& name, Vector4& vec)
		{
			bool e = TryFillBuffer(name, sizeof(float) * 4);
			if (e) GetBufferVector4(vec); 
			return e;
		}
		bool TaggedDataReader::TryGetMatrix(const String& name, Matrix& mat)
		{
			const Entry* ent = FindEntry(name);
			if (ent)
			{
				GetEntryMatrix(ent, mat);
				return true;
			}
			return false;
		}
		bool TaggedDataReader::TryGetColor4(const String& name, Color4& clr)
		{
			bool e = TryFillBuffer(name, sizeof(float) * 4);
			if (e) GetBufferColor4(clr); 
			return e;
		}
		bool TaggedDataReader::TryGetString(const String& name, String& str)
		{
			const Entry* ent = FindEntry(name);
			if (ent)
			{
				GetEntryString(ent, str);
				return true;
			}
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

		void TaggedDataReader::FillTagList(List<String>& nameTags) const
		{
			for (SectionTable::Enumerator e = m_positions.GetEnumerator();e.MoveNext();)
			{
				nameTags.Add(*e.getCurrentKey());
			}
		}
		

		void TaggedDataReader::GetBufferVector2(Vector2& vec)
		{
			v2x(vec) = cr32_le(m_buffer);
			v2y(vec) = cr32_le(m_buffer + sizeof(float));
		}
		void TaggedDataReader::GetBufferVector3(Vector3& vec)
		{
			v3x(vec) = cr32_le(m_buffer);
			v3y(vec) = cr32_le(m_buffer + sizeof(float));
			v3z(vec) = cr32_le(m_buffer + sizeof(float) * 2);
		}
		void TaggedDataReader::GetBufferVector4(Vector4& vec)
		{
			v4x(vec) = cr32_le(m_buffer);
			v4y(vec) = cr32_le(m_buffer + sizeof(float));
			v4z(vec) = cr32_le(m_buffer + sizeof(float) * 2);
			v4w(vec) = cr32_le(m_buffer + sizeof(float) * 3);
		}
		void TaggedDataReader::GetEntryMatrix(const TaggedDataReader::Entry* ent, Matrix& mat)
		{
			VirtualStream vStrm(m_stream, ent->Offset, ent->Size);
			BinaryReader br(&vStrm);
			br.SuspendStreamRelease();

			br.ReadMatrix(mat);

			br.Close();
		}
		void TaggedDataReader::GetBufferColor4(Color4& clr)
		{
			clr.Red = cr32_le(m_buffer);
			clr.Green = cr32_le(m_buffer + sizeof(float));
			clr.Blue = cr32_le(m_buffer + sizeof(float) * 2);
			clr.Alpha = cr32_le(m_buffer + sizeof(float) * 3);
		}
		void TaggedDataReader::GetEntryString(const TaggedDataReader::Entry* ent, String& str)
		{
			VirtualStream vStrm(m_stream, ent->Offset, ent->Size);
			BinaryReader br(&vStrm);
			br.SuspendStreamRelease();

			str = br.ReadString();

			br.Close();
		}

		/************************************************************************/
		/* TaggedDataWriter                                                     */
		/************************************************************************/
		
		TaggedDataWriter::TaggedDataWriter(bool isEndianIndependent)
			: m_endianDependent(!isEndianIndependent)
		{ }

		TaggedDataWriter::~TaggedDataWriter()
		{
			for (SectionTable::Enumerator e = m_positions.GetEnumerator(); e.MoveNext();)
			{
				MemoryOutStream* ee = e.getCurrentValue()->Buffer;
				delete ee;
			}
			m_positions.Clear();
		}

		TaggedDataWriter::Entry::Entry(const String& name)
			: Name(name)
		{
			Buffer = new MemoryOutStream(1024);
		}
		void TaggedDataWriter::Entry::ResetWritePosition() const
		{
			Buffer->setPosition(0);
		}


		BinaryWriter* TaggedDataWriter::AddEntry(const String& name)
		{
			Entry ent = Entry(name);
			m_positions.Add(name, ent);
			return new BinaryWriter(new VirtualStream(ent.Buffer, 0));
		}
		Stream* TaggedDataWriter::AddEntryStream(const String& name)
		{
			Entry ent = Entry(name);
			m_positions.Add(name, ent);
			return (new VirtualStream(ent.Buffer, 0));
		}

		
		bool TaggedDataWriter::Contains(const String& name) const
		{
			return !!FindEntry(name);
		}

		BinaryWriter* TaggedDataWriter::GetData(const String& name)
		{
			const Entry* ent = FindEntry(name);
			return new BinaryWriter(new VirtualStream(ent->Buffer, 0));
		}


		void TaggedDataWriter::AddEntry(const String& name, int64 value)
		{
			Entry ent = Entry(name); m_positions.Add(name, ent);
			_SetEntryDataInt64(ent, value);
		}
		void TaggedDataWriter::AddEntry(const String& name, uint64 value)
		{
			Entry ent = Entry(name); m_positions.Add(name, ent);
			_SetEntryDataUInt64(ent, value);
		}
		void TaggedDataWriter::AddEntry(const String& name, int32 value)
		{
			Entry ent = Entry(name); m_positions.Add(name, ent);
			_SetEntryDataInt32(ent, value);
		}
		void TaggedDataWriter::AddEntry(const String& name, uint32 value)
		{
			Entry ent = Entry(name); m_positions.Add(name, ent);
			_SetEntryDataUInt32(ent, value);
		}
		void TaggedDataWriter::AddEntry(const String& name, int16 value)
		{
			Entry ent = Entry(name); m_positions.Add(name, ent);
			_SetEntryDataInt16(ent, value);
		}
		void TaggedDataWriter::AddEntry(const String& name, uint16 value)
		{
			Entry ent = Entry(name); m_positions.Add(name, ent);
			_SetEntryDataUInt16(ent, value);
		}
		void TaggedDataWriter::AddEntry(const String& name, float value)
		{
			Entry ent = Entry(name); m_positions.Add(name, ent);
			_SetEntryDataSingle(ent, value);
		}
		void TaggedDataWriter::AddEntry(const String& name, double value)
		{
			Entry ent = Entry(name); m_positions.Add(name, ent);
			_SetEntryDataDouble(ent, value);
		}
		void TaggedDataWriter::AddEntry(const String& name, bool value)
		{
			Entry ent = Entry(name); m_positions.Add(name, ent);
			_SetEntryDataBool(ent, value);
		}

		void TaggedDataWriter::SetData(const String& name, int64 value)
		{
			const Entry* ent = FindEntry(name);
			assert(ent);
			ent->ResetWritePosition();
			_SetEntryDataInt64(*ent, value);
		}
		void TaggedDataWriter::SetData(const String& name, uint64 value)
		{
			const Entry* ent = FindEntry(name);
			assert(ent);
			ent->ResetWritePosition();
			_SetEntryDataUInt64(*ent, value);
		}
		void TaggedDataWriter::SetData(const String& name, int32 value)
		{
			const Entry* ent = FindEntry(name);
			assert(ent);
			ent->ResetWritePosition();
			_SetEntryDataInt32(*ent, value);
		}
		void TaggedDataWriter::SetData(const String& name, uint32 value)
		{
			const Entry* ent = FindEntry(name);
			assert(ent);
			ent->ResetWritePosition();
			_SetEntryDataUInt32(*ent, value);
		}
		void TaggedDataWriter::SetData(const String& name, int16 value)
		{
			const Entry* ent = FindEntry(name);
			assert(ent);
			ent->ResetWritePosition();
			_SetEntryDataInt16(*ent, value);
		}
		void TaggedDataWriter::SetData(const String& name, uint16 value)
		{
			const Entry* ent = FindEntry(name);
			assert(ent);
			ent->ResetWritePosition();
			_SetEntryDataUInt16(*ent, value);
		}
		void TaggedDataWriter::SetData(const String& name, float value)
		{
			const Entry* ent = FindEntry(name);
			assert(ent);
			ent->ResetWritePosition();
			_SetEntryDataSingle(*ent, value);
		}
		void TaggedDataWriter::SetData(const String& name, double value)
		{
			const Entry* ent = FindEntry(name);
			assert(ent);
			ent->ResetWritePosition();
			_SetEntryDataDouble(*ent, value);
		}
		void TaggedDataWriter::SetData(const String& name, bool value)
		{
			const Entry* ent = FindEntry(name);
			assert(ent);
			ent->ResetWritePosition();
			_SetEntryDataBool(*ent, value);
		}

		void TaggedDataWriter::AddEntryVector2(const String& name, const Vector2& vec)
		{
			Entry ent = Entry(name); m_positions.Add(name, ent);
			_SetEntryDataVector2(ent, vec);
		}
		void TaggedDataWriter::AddEntryVector3(const String& name, const Vector3& vec)
		{
			Entry ent = Entry(name); m_positions.Add(name, ent);
			_SetEntryDataVector3(ent, vec);
		}
		void TaggedDataWriter::AddEntryVector4(const String& name, const Vector4& vec)
		{
			Entry ent = Entry(name); m_positions.Add(name, ent);
			_SetEntryDataVector4(ent, vec);
		}
		void TaggedDataWriter::AddEntryMatrix(const String& name, const Matrix& mat)
		{
			Entry ent = Entry(name); m_positions.Add(name, ent);
			_SetEntryDataMatrix(ent, mat);
		}
		void TaggedDataWriter::AddEntryColor4(const String& name, const Color4& clr)
		{
			Entry ent = Entry(name); m_positions.Add(name, ent);
			_SetEntryDataColor4(ent, clr);
		}
		void TaggedDataWriter::AddEntryString(const String& name, const String& str)
		{
			Entry ent = Entry(name); m_positions.Add(name, ent);
			_SetEntryDataString(ent, str);
		}

		void TaggedDataWriter::SetDataVector2(const String& name, const Vector2& vec)
		{
			const Entry* ent = FindEntry(name); assert(ent);
			_SetEntryDataVector2(*ent, vec);
		}
		void TaggedDataWriter::SetDataVector3(const String& name, const Vector3& vec)
		{
			const Entry* ent = FindEntry(name); assert(ent);
			_SetEntryDataVector3(*ent, vec);
		}
		void TaggedDataWriter::SetDataVector4(const String& name, const Vector4& vec)
		{
			const Entry* ent = FindEntry(name); assert(ent);
			_SetEntryDataVector4(*ent, vec);
		}
		void TaggedDataWriter::SetDataMatrix(const String& name, const Matrix& mat)
		{
			const Entry* ent = FindEntry(name); assert(ent);
			_SetEntryDataMatrix(*ent, mat);
		}
		void TaggedDataWriter::SetDataColor4(const String& name, const Color4& clr)
		{
			const Entry* ent = FindEntry(name); assert(ent);
			_SetEntryDataColor4(*ent, clr);
		}
		void TaggedDataWriter::SetDataString(const String& name, const String& str)
		{
			const Entry* ent = FindEntry(name); assert(ent);
			_SetEntryDataString(*ent, str);
		}

		void TaggedDataWriter::Save(Stream* stream) const
		{
			BinaryWriter* bw = new BinaryWriter(stream);

			bw->Write(static_cast<uint32>(m_positions.getCount()));

			for (SectionTable::Enumerator e = m_positions.GetEnumerator(); e.MoveNext();)
			{
				Entry* ent = e.getCurrentValue();
				MemoryOutStream* memBlock = ent->Buffer;

				bw->Write(*e.getCurrentKey());
				bw->Write(static_cast<uint32>(memBlock->getLength()));
				bw->Write(memBlock->getPointer(), memBlock->getLength());
			}

			bw->Close();
			delete bw;
		}




		void TaggedDataWriter::_SetEntryDataInt64(const TaggedDataWriter::Entry& ent, int64 value)
		{
			if (m_endianDependent)
				i64tomb_dep(value, m_buffer);
			else
				i64tomb_le(value, m_buffer);

			ent.Buffer->Write(m_buffer, sizeof(value));
		}
		void TaggedDataWriter::_SetEntryDataUInt64(const TaggedDataWriter::Entry& ent, uint64 value)
		{
			if (m_endianDependent)
				ui64tomb_dep(value, m_buffer);
			else
				ui64tomb_le(value, m_buffer);

			ent.Buffer->Write(m_buffer, sizeof(value));
		}
		void TaggedDataWriter::_SetEntryDataInt32(const TaggedDataWriter::Entry& ent, int32 value)
		{
			if (m_endianDependent)
				i32tomb_dep(value, m_buffer);
			else
				i32tomb_le(value, m_buffer);

			ent.Buffer->Write(m_buffer, sizeof(value));
		}
		void TaggedDataWriter::_SetEntryDataUInt32(const TaggedDataWriter::Entry& ent, uint32 value)
		{
			if (m_endianDependent)
				ui32tomb_dep(value, m_buffer);
			else
				ui32tomb_le(value, m_buffer);

			ent.Buffer->Write(m_buffer, sizeof(value));
		}
		void TaggedDataWriter::_SetEntryDataInt16(const TaggedDataWriter::Entry& ent, int16 value)
		{
			if (m_endianDependent)
				i16tomb_dep(value, m_buffer);
			else
				i16tomb_le(value, m_buffer);

			ent.Buffer->Write(m_buffer, sizeof(value));
		}
		void TaggedDataWriter::_SetEntryDataUInt16(const TaggedDataWriter::Entry& ent, uint16 value)
		{
			if (m_endianDependent)
				ui16tomb_dep(value, m_buffer);
			else
				ui16tomb_le(value, m_buffer);

			ent.Buffer->Write(m_buffer, sizeof(value));
		}
		void TaggedDataWriter::_SetEntryDataSingle(const TaggedDataWriter::Entry& ent, float value)
		{
			if (m_endianDependent)
				r32tomb_dep(value, m_buffer);
			else
				r32tomb_le(value, m_buffer);

			ent.Buffer->Write(m_buffer, sizeof(value));
		}
		void TaggedDataWriter::_SetEntryDataDouble(const TaggedDataWriter::Entry& ent, double value)
		{
			if (m_endianDependent)
				r64tomb_dep(value, m_buffer);
			else
				r64tomb_le(value, m_buffer);

			ent.Buffer->Write(m_buffer, sizeof(value));
		}
		void TaggedDataWriter::_SetEntryDataBool(const TaggedDataWriter::Entry& ent, bool value)
		{
			m_buffer[0] = value ? 1 : 0;

			ent.Buffer->Write(m_buffer, sizeof(value));
		}

		void TaggedDataWriter::_SetEntryDataVector2(const TaggedDataWriter::Entry& ent, const Vector2& vec)
		{
			if (m_endianDependent)
			{
				r32tomb_dep(v2x(vec), m_buffer);
				r32tomb_dep(v2y(vec), m_buffer + sizeof(float));
			}
			else
			{
				r32tomb_le(v2x(vec), m_buffer);
				r32tomb_le(v2y(vec), m_buffer + sizeof(float));
			}
			ent.Buffer->Write(m_buffer, sizeof(float) * 2);
		}
		void TaggedDataWriter::_SetEntryDataVector3(const TaggedDataWriter::Entry& ent, const Vector3& vec)
		{
			if (m_endianDependent)
			{
				r32tomb_dep(v3x(vec), m_buffer);
				r32tomb_dep(v3y(vec), m_buffer + sizeof(float));
				r32tomb_dep(v3z(vec), m_buffer + sizeof(float) * 2);
			}
			else
			{
				r32tomb_le(v3x(vec), m_buffer);
				r32tomb_le(v3y(vec), m_buffer + sizeof(float));
				r32tomb_le(v3z(vec), m_buffer + sizeof(float) * 2);
			}
			ent.Buffer->Write(m_buffer, sizeof(float) * 3);
		}
		void TaggedDataWriter::_SetEntryDataVector4(const TaggedDataWriter::Entry& ent, const Vector4& vec)
		{
			if (m_endianDependent)
			{
				r32tomb_dep(v4x(vec), m_buffer);
				r32tomb_dep(v4y(vec), m_buffer + sizeof(float));
				r32tomb_dep(v4z(vec), m_buffer + sizeof(float) * 2);
				r32tomb_dep(v4w(vec), m_buffer + sizeof(float) * 3);
			}
			else
			{
				r32tomb_le(v4x(vec), m_buffer);
				r32tomb_le(v4y(vec), m_buffer + sizeof(float));
				r32tomb_le(v4z(vec), m_buffer + sizeof(float) * 2);
				r32tomb_le(v4w(vec), m_buffer + sizeof(float) * 3);
			}
			ent.Buffer->Write(m_buffer, sizeof(float) * 4);
		}
		void TaggedDataWriter::_SetEntryDataMatrix(const TaggedDataWriter::Entry& ent, const Matrix& mat)
		{
			VirtualStream vStrm(ent.Buffer, 0);
			BinaryWriter bw(&vStrm);
			bw.SuspendStreamRelease();

			bw.Write(mat);

			bw.Close();
		}
		void TaggedDataWriter::_SetEntryDataColor4(const TaggedDataWriter::Entry& ent, const Color4& clr)
		{
			if (m_endianDependent)
			{
				r32tomb_dep(clr.Red, m_buffer);
				r32tomb_dep(clr.Green, m_buffer + sizeof(float));
				r32tomb_dep(clr.Blue, m_buffer + sizeof(float) * 2);
				r32tomb_dep(clr.Alpha, m_buffer + sizeof(float) * 3);
			}
			else
			{
				r32tomb_le(clr.Red, m_buffer);
				r32tomb_le(clr.Green, m_buffer + sizeof(float));
				r32tomb_le(clr.Blue, m_buffer + sizeof(float) * 2);
				r32tomb_le(clr.Alpha, m_buffer + sizeof(float) * 3);
			}
			ent.Buffer->Write(m_buffer, sizeof(float) * 4);
		}
		void TaggedDataWriter::_SetEntryDataString(const TaggedDataWriter::Entry& ent, const String& str)
		{
			VirtualStream vStrm(ent.Buffer, 0);
			BinaryWriter bw(&vStrm);
			bw.SuspendStreamRelease();

			bw.Write(str);

			bw.Close();
		}



	}
}
