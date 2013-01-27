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
#ifndef APOC3D_BINARYDATA_H
#define APOC3D_BINARYDATA_H

#include "Common.h"
#include "Collections/FastMap.h"
#include "Math/Vector.h"

using namespace Apoc3D::Collections;
using namespace Apoc3D::Math;

using namespace std;

namespace Apoc3D
{
	namespace IO
	{
		/**
		 *  Tagged data is a collection of key and values. 
		 *  The keys are name of the corresponding value in string form. 
		 *  The value is a series of binary bytes which can represent anything.
		 *  
		 *  As TaggedData is in a key-value form. the value can be a sub-TaggedData. And the value of sub-TaggedData
		 *  can have sub-sub-TaggedData.
		 *  In this way, respectively a tree of data can be stored with this class easily.
		 *  
		 *  This class implements a reader for Tagged Data.
		 */
		class APAPI TaggedDataReader
		{
		public:
			TaggedDataReader(Stream* strm);
			~TaggedDataReader();

			/**
			 *  Check if the data chunk has a key name of the specified
			 */
			bool Contains(const String& name) const { return m_positions.Contains(name); }

			BinaryReader* TryGetData(const String& name) const;
			BinaryReader* GetData(const String& name) const;

			Stream* GetDataStream(const String& name) const;

			int64 GetDataInt64(const String& name);
			uint64 GetDataUInt64(const String& name);
			int32 GetDataInt32(const String& name);
			uint32 GetDataUInt32(const String& name);
			int16 GetDataInt16(const String& name);
			uint16 GetDataUInt16(const String& name);
			bool GetDataBool(const String& name);
			float GetDataSingle(const String& name);
			double GetDataDouble(const String& name);

			void GetDataBool(const String& name, bool* value, int32 count);

			bool TryGetDataInt64(const String& name, int64& v);
			bool TryGetDataUInt64(const String& name, uint64& v);
			bool TryGetDataInt32(const String& name, int32& v);
			bool TryGetDataUInt32(const String& name, uint32& v);
			bool TryGetDataInt16(const String& name, int16& v);
			bool TryGetDataUInt16(const String& name, uint16& v);
			bool TryGetDataBool(const String& name, bool& v);
			bool TryGetDataSingle(const String& name, float& v);
			bool TryGetDataDouble(const String& name, double& v);

			bool TryGetDataBool(const String& name, bool* value, int32 count);


			void GetDataVector2(const String& name, Vector2& vec);
			void GetDataVector3(const String& name, Vector3& vec);
			void GetDataVector4(const String& name, Vector4& vec);
			void GetDataMatrix(const String& name, Matrix& mat);
			void GetDataColor4(const String& name, Color4& clr);
			void GetDataString(const String& name, String& str);

			bool TryGetVector2(const String& name, Vector2& vec);
			bool TryGetVector3(const String& name, Vector3& vec);
			bool TryGetVector4(const String& name, Vector4& vec);
			bool TryGetMatrix(const String& name, Matrix& mat);
			bool TryGetColor4(const String& name, Color4& clr);
			bool TryGetString(const String& name, String& str);


			int64 GetChunkOffset(const String& name) const;

			void Close();

			/** 
			 * Exports a list of key names to the specified list.
			 */
			void FillTagList(List<String>& nameTags) const;


			Stream* getBaseStream() const { return m_stream; }
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
				Entry() { }
			};

			typedef FastMap<String, Entry> SectionTable;

			inline void FillBuffer(const String& name, uint32 len);

			inline bool TryFillBuffer(const String& name, uint32 len);

			inline void _GetEntryBool(const Entry* e, bool* val, int32 len);
			inline void _GetBufferVector2(Vector2& vec);
			inline void _GetBufferVector3(Vector3& vec);
			inline void _GetBufferVector4(Vector4& vec);
			inline void _GetEntryMatrix(const Entry* e, Matrix& mat);
			inline void _GetBufferColor4(Color4& clr);
			inline void _GetEntryString(const Entry* e, String& str);

			const Entry* FindEntry(const String& name) const
			{
				return m_positions.TryGetValue(name);
			}


			bool m_endianDependent;
			int m_sectCount;
			SectionTable m_positions;
			Stream* m_stream;

			char m_buffer[32];

		};

		/**
		 * This class implements a writer for Tagged Data.
		 */
		class APAPI TaggedDataWriter
		{
		public:
			/**
			 * param isEndianIndependent true if the data medium is a fixed Endianness across platform.
			 *		 false for situations like CPU memory.
			 */
			TaggedDataWriter(bool isEndianIndependent);
			~TaggedDataWriter();


			BinaryWriter* AddEntry(const String& name);
			Stream* AddEntryStream(const String& name);

			void AddEntry(const String& name, int64 value);
			void AddEntry(const String& name, uint64 value);
			void AddEntry(const String& name, int32 value);
			void AddEntry(const String& name, uint32 value);
			void AddEntry(const String& name, int16 value);
			void AddEntry(const String& name, uint16 value);
			void AddEntry(const String& name, float value);
			void AddEntry(const String& name, double value);
			void AddEntryBool(const String& name, bool value);
			void AddEntryBool(const String& name, const bool* value, int32 count);

			void AddEntryVector2(const String& name, const Vector2& vec);
			void AddEntryVector3(const String& name, const Vector3& vec);
			void AddEntryVector4(const String& name, const Vector4& vec);
			void AddEntryMatrix(const String& name, const Matrix& mat);
			void AddEntryColor4(const String& name, const Color4& clr);
			void AddEntryString(const String& name, const String& str);

			BinaryWriter* GetData(const String& name);

			void SetData(const String& name, int64 value);
			void SetData(const String& name, uint64 value);
			void SetData(const String& name, int32 value);
			void SetData(const String& name, uint32 value);
			void SetData(const String& name, int16 value);
			void SetData(const String& name, uint16 value);
			void SetData(const String& name, float value);
			void SetData(const String& name, double value);
			void SetData(const String& name, bool value);
			
			void SetDataVector2(const String& name, const Vector2& vec);
			void SetDataVector3(const String& name, const Vector3& vec);
			void SetDataVector4(const String& name, const Vector4& vec);
			void SetDataMatrix(const String& name, const Matrix& mat);
			void SetDataColor4(const String& name, const Color4& clr);
			void SetDataString(const String& name, const String& str);


			void Save(Stream* stream) const;

			bool Contains(const String& name) const;
		private:
			struct Entry
			{
				String Name;
				MemoryOutStream* Buffer;
				Entry(const String& name);

				Entry() { }
				void ResetWritePosition() const;
			};

			typedef FastMap<String, Entry> SectionTable;

			const Entry* FindEntry(const String& name) const
			{
				return m_positions.TryGetValue(name);
			}

			void _SetEntryDataInt64(const Entry& ent, int64 value);
			void _SetEntryDataUInt64(const Entry& ent, uint64 value);
			void _SetEntryDataInt32(const Entry& ent, int32 value);
			void _SetEntryDataUInt32(const Entry& ent, uint32 value);
			void _SetEntryDataInt16(const Entry& ent, int16 value);
			void _SetEntryDataUInt16(const Entry& ent, uint16 value);
			void _SetEntryDataSingle(const Entry& ent, float value);
			void _SetEntryDataDouble(const Entry& ent, double value);
			void _SetEntryDataBool(const Entry& ent, bool value);
			void _SetEntryDataBool(const Entry& ent, const bool* value, int32 count);

			void _SetEntryDataVector2(const Entry& ent, const Vector2& vec);
			void _SetEntryDataVector3(const Entry& ent, const Vector3& vec);
			void _SetEntryDataVector4(const Entry& ent, const Vector4& vec);
			void _SetEntryDataMatrix(const Entry& ent, const Matrix& mat);
			void _SetEntryDataColor4(const Entry& ent, const Color4& clr);
			void _SetEntryDataString(const Entry& ent, const String& str);



			bool m_endianDependent;
			SectionTable m_positions;
			char m_buffer[32];

		};
	}
}

#endif