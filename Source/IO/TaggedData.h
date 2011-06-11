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
#ifndef BINARYDATA_H
#define BINARYDATA_H

#include "Common.h"

using namespace Apoc3D::Math;

using namespace std;

namespace Apoc3D
{
	namespace IO
	{
		

		/** Tagged data is a collection of key and values. 
			The keys are name of the corresponding value in string form. 
			The value is a series of binary bytes which can represent anything.

			This class implements a reader for Tagged Data.
		*/
		class APAPI TaggedDataReader
		{
		private:
			struct Entry
			{
				String Name;
				int64 Offset;
				uint32 Size;

				Entry(String name, int64 offset, uint32 size)
					: Name(name), Offset(offset), Size(size)
				{

				}
			};
			//template class APAPI unordered_map<String, Entry>;
			typedef unordered_map<String, Entry> SectionTable;
			
			bool m_endianDependent;
			int m_sectCount;
			SectionTable m_positions;
			Stream* m_stream;

			char m_buffer[32];


			inline void FillBuffer(const String& name, uint32 len);

			inline bool TryFillBuffer(const String& name, uint32 len);

			const Entry* FindEntry(const String& name) const
			{
				SectionTable::const_iterator iter = m_positions.find(name);
				if (iter != m_positions.end())
				{
					return &iter->second;
				}
				return 0;
			}
		public:
			Stream* getBaseStream() const { return m_stream; }

			TaggedDataReader(Stream* strm);
			~TaggedDataReader();

			bool Contains(const String& name) const
			{
				SectionTable::const_iterator iter = m_positions.find(name);
				return (iter != m_positions.end());
			}

			BinaryReader* TryGetData(const String& name) const;
			BinaryReader* GetData(const String& name) const;

			inline Stream* GetDataStream(const String& name) const;

			inline int64 GetDataInt64(const String& name);
			inline uint64 GetDataUInt64(const String& name);
			inline int32 GetDataInt32(const String& name);
			inline uint32 GetDataUInt32(const String& name);
			inline int16 GetDataInt16(const String& name);
			inline uint16 GetDataUInt16(const String& name);
			inline bool GetDataBool(const String& name);
			inline float GetDataSingle(const String& name);
			inline double GetDataDouble(const String& name);


			inline bool TryGetDataInt64(const String& name, int64& v);
			inline bool TryGetDataUInt64(const String& name, uint64& v);
			inline bool TryGetDataInt32(const String& name, int32& v);
			inline bool TryGetDataUInt32(const String& name, uint32& v);
			inline bool TryGetDataInt16(const String& name, int16& v);
			inline bool TryGetDataUInt16(const String& name, uint16& v);
			inline bool TryGetDataBool(const String& name, bool& v);
			inline bool TryGetDataSingle(const String& name, float& v);
			inline bool TryGetDataDouble(const String& name, double& v);

			inline int64 GetChunkOffset(const String& name) const;

			inline void Close();

		};

		class APAPI TaggedDataWriter
		{
		private:
			struct Entry
			{
				String Name;
				MemoryOutStream* Buffer;
				Entry(const String& name)
					: Name(name)
				{
				}
			};

			typedef unordered_map<String, Entry> SectionTable;

			bool m_endianDependent;
			SectionTable m_positions;
			char m_buffer[32];

			const Entry* FindEntry(const String& name) const
			{
				SectionTable::const_iterator iter = m_positions.find(name);
				if (iter != m_positions.end())
				{
					return &iter->second;
				}
				return 0;
			}
		public:
			TaggedDataWriter(bool isWritringFile)
				: m_endianDependent(!isWritringFile)
			{ }
			~TaggedDataWriter();


			BinaryWriter* AddEntry(const String& name);
			Stream* AddEntryStream(const String& name);

			inline void AddEntry(const String& name, int64 value);
			inline void AddEntry(const String& name, uint64 value);
			inline void AddEntry(const String& name, int32 value);
			inline void AddEntry(const String& name, uint32 value);
			inline void AddEntry(const String& name, int16 value);
			inline void AddEntry(const String& name, uint16 value);
			inline void AddEntry(const String& name, float value);
			inline void AddEntry(const String& name, double value);
			inline void AddEntry(const String& name, bool value);
			
			BinaryWriter* GetData(const String& name);

			inline void SetData(const String& name, int64 value);
			inline void SetData(const String& name, uint64 value);
			inline void SetData(const String& name, int32 value);
			inline void SetData(const String& name, uint32 value);
			inline void SetData(const String& name, int16 value);
			inline void SetData(const String& name, uint16 value);
			inline void SetData(const String& name, float value);
			inline void SetData(const String& name, double value);
			inline void SetData(const String& name, bool value);

			void Save(Stream* stream) const;
		};
	}
}

#endif