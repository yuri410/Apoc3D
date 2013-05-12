#pragma once
#ifndef APOC3D_BINARYDATA_H
#define APOC3D_BINARYDATA_H

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


#include "apoc3d/Common.h"

#include "apoc3d/Collections/HashMap.h"
#include "apoc3d/Math/Vector.h"

using namespace Apoc3D::Collections;
using namespace Apoc3D::Math;

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


			void GetAuto(const String& name, int64& value)	{ value = GetDataInt64(name); }
			void GetAuto(const String& name, uint64& value)	{ value = GetDataUInt64(name); }
			void GetAuto(const String& name, int32& value)	{ value = GetDataInt32(name); }
			void GetAuto(const String& name, uint32& value)	{ value = GetDataUInt32(name); }
			void GetAuto(const String& name, int16& value)	{ value = GetDataInt16(name); }
			void GetAuto(const String& name, uint16& value)	{ value = GetDataUInt16(name); }
			void GetAuto(const String& name, float& value)	{ value = GetDataSingle(name); }
			void GetAuto(const String& name, double& value)	{ value = GetDataDouble(name); }
			void GetAuto(const String& name, bool& value)	{ value = GetDataBool(name); }
			void GetAuto(const String& name, Vector2& vec)	{ GetDataVector2(name, vec); }
			void GetAuto(const String& name, Vector3& vec)	{ GetDataVector3(name, vec); }
			void GetAuto(const String& name, Vector4& vec)	{ GetDataVector4(name, vec); }
			void GetAuto(const String& name, Matrix& mat)	{ GetDataMatrix(name, mat); }
			void GetAuto(const String& name, Color4& clr)	{ GetDataColor4(name, clr); }
			void GetAuto(const String& name, Plane& plane)	{ GetDataPlane(name, plane); }
			void GetAuto(const String& name, Quaternion& quad)	{ GetDataQuaternion(name, quad); }
			void GetAuto(const String& name, String& str)	{ GetDataString(name, str); }

			void GetAuto(const String& name, int64* value, int32 count)	{ GetDataInt64(name, value, count); }
			void GetAuto(const String& name, uint64* value, int32 count)	{ GetDataUInt64(name, value, count); }
			void GetAuto(const String& name, int32* value, int32 count)	{ GetDataInt32(name, value, count); }
			void GetAuto(const String& name, uint32* value, int32 count)	{ GetDataUInt32(name, value, count); }
			void GetAuto(const String& name, int16* value, int32 count)	{ GetDataInt16(name, value, count); }
			void GetAuto(const String& name, uint16* value, int32 count)	{ GetDataUInt16(name, value, count); }
			void GetAuto(const String& name, float* value, int32 count)	{ GetDataSingle(name, value, count); }
			void GetAuto(const String& name, double* value, int32 count)	{ GetDataDouble(name, value, count); }
			void GetAuto(const String& name, bool* value, int32 count)		{ GetDataBool(name, value, count); }
			void GetAuto(const String& name, Vector2* vec, int32 count)	{ GetDataVector2(name, vec, count); }
			void GetAuto(const String& name, Vector3* vec, int32 count)	{ GetDataVector3(name, vec, count); }
			void GetAuto(const String& name, Vector4* vec, int32 count)	{ GetDataVector4(name, vec, count); }
			void GetAuto(const String& name, Matrix* mat, int32 count)		{ GetDataMatrix(name, mat, count); }
			void GetAuto(const String& name, Color4* clr, int32 count)		{ GetDataColor4(name, clr, count); }
			void GetAuto(const String& name, String* str, int32 count)		{ GetDataString(name, str, count); }



			int64 GetDataInt64(const String& name);
			uint64 GetDataUInt64(const String& name);
			int32 GetDataInt32(const String& name);
			uint32 GetDataUInt32(const String& name);
			int16 GetDataInt16(const String& name);
			uint16 GetDataUInt16(const String& name);
			bool GetDataBool(const String& name);
			float GetDataSingle(const String& name);
			double GetDataDouble(const String& name);

			void GetDataInt64(const String& name, int64* value, int32 count);
			void GetDataUInt64(const String& name, uint64* value, int32 count);
			void GetDataInt32(const String& name, int32* value, int32 count);
			void GetDataUInt32(const String& name, uint32* value, int32 count);
			void GetDataInt16(const String& name, int16* value, int32 count);
			void GetDataUInt16(const String& name, uint16* value, int32 count);
			void GetDataBool(const String& name, bool* value, int32 count);
			void GetDataSingle(const String& name, float* value, int32 count);
			void GetDataDouble(const String& name, double* value, int32 count);

			void GetDataVector2(const String& name, Vector2& vec);
			void GetDataVector3(const String& name, Vector3& vec);
			void GetDataVector4(const String& name, Vector4& vec);
			void GetDataMatrix(const String& name, Matrix& mat);
			void GetDataColor4(const String& name, Color4& clr);
			void GetDataPlane(const String& name, Plane& plane);
			void GetDataQuaternion(const String& name, Quaternion& quat);
			void GetDataString(const String& name, String& str);

			void GetDataVector2(const String& name, Vector2* vec, int32 count);
			void GetDataVector3(const String& name, Vector3* vec, int32 count);
			void GetDataVector4(const String& name, Vector4* vec, int32 count);
			void GetDataMatrix(const String& name, Matrix* mat, int32 count);
			void GetDataColor4(const String& name, Color4* clr, int32 count);
			void GetDataString(const String& name, String* str, int32 count);



			bool TryGetDataInt64(const String& name, int64& v);
			bool TryGetDataUInt64(const String& name, uint64& v);
			bool TryGetDataInt32(const String& name, int32& v);
			bool TryGetDataUInt32(const String& name, uint32& v);
			bool TryGetDataInt16(const String& name, int16& v);
			bool TryGetDataUInt16(const String& name, uint16& v);
			bool TryGetDataBool(const String& name, bool& v);
			bool TryGetDataSingle(const String& name, float& v);
			bool TryGetDataDouble(const String& name, double& v);

			bool TryGetDataInt64(const String& name, int64* value, int32 count);
			bool TryGetDataUInt64(const String& name, uint64* value, int32 count);
			bool TryGetDataInt32(const String& name, int32* value, int32 count);
			bool TryGetDataUInt32(const String& name, uint32* value, int32 count);
			bool TryGetDataInt16(const String& name, int16* value, int32 count);
			bool TryGetDataUInt16(const String& name, uint16* value, int32 count);
			bool TryGetDataBool(const String& name, bool* value, int32 count);
			bool TryGetDataSingle(const String& name, float* value, int32 count);
			bool TryGetDataDouble(const String& name, double* value, int32 count);

			bool TryGetVector2(const String& name, Vector2& vec);
			bool TryGetVector3(const String& name, Vector3& vec);
			bool TryGetVector4(const String& name, Vector4& vec);
			bool TryGetMatrix(const String& name, Matrix& mat);
			bool TryGetColor4(const String& name, Color4& clr);
			bool TryGetString(const String& name, String& str);

			bool TryGetVector2(const String& name, Vector2* vec, int32 count);
			bool TryGetVector3(const String& name, Vector3* vec, int32 count);
			bool TryGetVector4(const String& name, Vector4* vec, int32 count);
			bool TryGetMatrix(const String& name, Matrix* mat, int32 count);
			bool TryGetColor4(const String& name, Color4* clr, int32 count);
			bool TryGetString(const String& name, String* str, int32 count);



			int64 GetChunkOffset(const String& name) const;

			void Close();

			/** 
			 * Exports a list of key names to the specified list.
			 */
			void FillTagList(List<String>& nameTags) const;

			bool isEndianIndependent() const { return !m_endianDependent; }
			Stream* getBaseStream() const { return m_stream; }
		private:
			struct Entry
			{
				String Name;
				int64 Offset;
				uint32 Size;

				Entry(String name, int64 offset, uint32 size)
					: Name(name), Offset(offset), Size(size)
				{ }
				Entry() { }
			};

			typedef HashMap<String, Entry> SectionTable;

			inline void FillBuffer(const String& name, uint32 len);

			inline bool TryFillBuffer(const String& name, uint32 len);

			
			void _GetEntryInt64(const Entry* e, int64* val, int32 len);
			void _GetEntryUInt64(const Entry* e, uint64* val, int32 len);
			void _GetEntryInt32(const Entry* e, int32* val, int32 len);
			void _GetEntryUInt32(const Entry* e, uint32* val, int32 len);
			void _GetEntryInt16(const Entry* e, int16* val, int32 len);
			void _GetEntryUInt16(const Entry* e, uint16* val, int32 len);
			void _GetEntryBool(const Entry* e, bool* val, int32 len);
			void _GetEntrySingle(const Entry* e, float* val, int32 len);
			void _GetEntryDouble(const Entry* e, double* val, int32 len);

			inline void _GetBufferVector2(Vector2& vec);
			inline void _GetBufferVector3(Vector3& vec);
			inline void _GetBufferVector4(Vector4& vec);
			inline void _GetBufferColor4(Color4& clr);
			void _GetEntryMatrix(const Entry* e, Matrix& mat);
			void _GetEntryString(const Entry* e, String& str);

			void _GetEntryVector2(const Entry* ent, Vector2* value, int len);
			void _GetEntryVector3(const Entry* ent, Vector3* value, int len);
			void _GetEntryVector4(const Entry* ent, Vector4* value, int len);
			void _GetEntryMatrix(const Entry* ent, Matrix* value, int len);
			void _GetEntryColor4(const Entry* ent, Color4* value, int len);
			void _GetEntryString(const Entry* ent, String* value, int len);


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
			 *		 false for situations like system memory.
			 */
			TaggedDataWriter(bool isEndianIndependent);
			~TaggedDataWriter();

			BinaryWriter* AddEntry(const String& name);
			Stream* AddEntryStream(const String& name);


			void AddAuto(const String& name, int64 value)	{ AddEntryInt64(name, value); }
			void AddAuto(const String& name, uint64 value)	{ AddEntryUInt64(name, value); }
			void AddAuto(const String& name, int32 value)	{ AddEntryInt32(name, value); }
			void AddAuto(const String& name, uint32 value)	{ AddEntryUInt32(name, value); }
			void AddAuto(const String& name, int16 value)	{ AddEntryInt16(name, value); }
			void AddAuto(const String& name, uint16 value)	{ AddEntryUInt16(name, value); }
			void AddAuto(const String& name, float value)	{ AddEntrySingle(name, value); }
			void AddAuto(const String& name, double value)	{ AddEntryDouble(name, value); }
			void AddAuto(const String& name, bool value)	{ AddEntryBool(name, value); }
			void AddAuto(const String& name, const Vector2& vec)	{ AddEntryVector2(name, vec); }
			void AddAuto(const String& name, const Vector3& vec)	{ AddEntryVector3(name, vec); }
			void AddAuto(const String& name, const Vector4& vec)	{ AddEntryVector4(name, vec); }
			void AddAuto(const String& name, const Matrix& mat)	{ AddEntryMatrix(name, mat); }
			void AddAuto(const String& name, const Color4& clr)	{ AddEntryColor4(name, clr); }
			void AddAuto(const String& name, const Plane& plane)	{ AddEntryPlane(name, plane); }
			void AddAuto(const String& name, const Quaternion& quad)	{ AddEntryQuaternion(name, quad); }
			void AddAuto(const String& name, const String& str)	{ AddEntryString(name, str); }

			void AddAuto(const String& name, const int64* value, int32 count)	{ AddEntryInt64(name, value, count); }
			void AddAuto(const String& name, const uint64* value, int32 count)	{ AddEntryUInt64(name, value, count); }
			void AddAuto(const String& name, const int32* value, int32 count)	{ AddEntryInt32(name, value, count); }
			void AddAuto(const String& name, const uint32* value, int32 count)	{ AddEntryUInt32(name, value, count); }
			void AddAuto(const String& name, const int16* value, int32 count)	{ AddEntryInt16(name, value, count); }
			void AddAuto(const String& name, const uint16* value, int32 count)	{ AddEntryUInt16(name, value, count); }
			void AddAuto(const String& name, const float* value, int32 count)	{ AddEntrySingle(name, value, count); }
			void AddAuto(const String& name, const double* value, int32 count)	{ AddEntryDouble(name, value, count); }
			void AddAuto(const String& name, const bool* value, int32 count)	{ AddEntryBool(name, value, count); }
			void AddAuto(const String& name, const Vector2* vec, int32 count)	{ AddEntryVector2(name, vec, count); }
			void AddAuto(const String& name, const Vector3* vec, int32 count)	{ AddEntryVector3(name, vec, count); }
			void AddAuto(const String& name, const Vector4* vec, int32 count)	{ AddEntryVector4(name, vec, count); }
			void AddAuto(const String& name, const Matrix* mat, int32 count)	{ AddEntryMatrix(name, mat, count); }
			void AddAuto(const String& name, const Color4* clr, int32 count)	{ AddEntryColor4(name, clr, count); }
			void AddAuto(const String& name, const String* str, int32 count)	{ AddEntryString(name, str, count); }


			void AddEntryInt64(const String& name, int64 value);
			void AddEntryUInt64(const String& name, uint64 value);
			void AddEntryInt32(const String& name, int32 value);
			void AddEntryUInt32(const String& name, uint32 value);
			void AddEntryInt16(const String& name, int16 value);
			void AddEntryUInt16(const String& name, uint16 value);
			void AddEntrySingle(const String& name, float value);
			void AddEntryDouble(const String& name, double value);
			void AddEntryBool(const String& name, bool value);

			void AddEntryInt64(const String& name, const int64* value, int32 count);
			void AddEntryUInt64(const String& name, const uint64* value, int32 count);
			void AddEntryInt32(const String& name, const int32* value, int32 count);
			void AddEntryUInt32(const String& name, const uint32* value, int32 count);
			void AddEntryInt16(const String& name, const int16* value, int32 count);
			void AddEntryUInt16(const String& name, const uint16* value, int32 count);
			void AddEntrySingle(const String& name, const float* value, int32 count);
			void AddEntryDouble(const String& name, const double* value, int32 count);
			void AddEntryBool(const String& name, const bool* value, int32 count);
			
			void AddEntryVector2(const String& name, const Vector2& vec);
			void AddEntryVector3(const String& name, const Vector3& vec);
			void AddEntryVector4(const String& name, const Vector4& vec);
			void AddEntryMatrix(const String& name, const Matrix& mat);
			void AddEntryColor4(const String& name, const Color4& clr);
			void AddEntryPlane(const String& name, const Plane& plane);
			void AddEntryQuaternion(const String& name, const Quaternion& quad);
			void AddEntryString(const String& name, const String& str);

			void AddEntryVector2(const String& name, const Vector2* vec, int32 count);
			void AddEntryVector3(const String& name, const Vector3* vec, int32 count);
			void AddEntryVector4(const String& name, const Vector4* vec, int32 count);
			void AddEntryMatrix(const String& name, const Matrix* mat, int32 count);
			void AddEntryColor4(const String& name, const Color4* clr, int32 count);
			void AddEntryString(const String& name, const String* str, int32 count);

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
			
			void SetDataInt64(const String& name, const int64* value, int32 count);
			void SetDataUInt64(const String& name, const uint64* value, int32 count);
			void SetDataInt32(const String& name, const int32* value, int32 count);
			void SetDataUInt32(const String& name, const uint32* value, int32 count);
			void SetDataInt16(const String& name, const int16* value, int32 count);
			void SetDataUInt16(const String& name, const uint16* value, int32 count);
			void SetDataSingle(const String& name, const float* value, int32 count);
			void SetDataDouble(const String& name, const double* value, int32 count);
			void SetDataBool(const String& name, const bool* value, int32 count);

			void SetDataVector2(const String& name, const Vector2& vec);
			void SetDataVector3(const String& name, const Vector3& vec);
			void SetDataVector4(const String& name, const Vector4& vec);
			void SetDataMatrix(const String& name, const Matrix& mat);
			void SetDataColor4(const String& name, const Color4& clr);
			void SetDataString(const String& name, const String& str);
			
			void SetDataVector2(const String& name, const Vector2* vec, int32 count);
			void SetDataVector3(const String& name, const Vector3* vec, int32 count);
			void SetDataVector4(const String& name, const Vector4* vec, int32 count);
			void SetDataMatrix(const String& name, const Matrix* mat, int32 count);
			void SetDataColor4(const String& name, const Color4* clr, int32 count);
			void SetDataString(const String& name, const String* str, int32 count);


			Apoc3D::Config::ConfigurationSection* MakeDigest(const String& name) const;
			void Save(Stream* stream) const;

			bool Contains(const String& name) const;

			bool isEndianIndependent() const { return !m_endianDependent; }

		private:
			struct Entry
			{
				String Name;
				MemoryOutStream* Buffer;
				Entry(const String& name);

				Entry() { }
				void ResetWritePosition() const;
			};

			typedef HashMap<String, Entry> SectionTable;

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
			
			void _SetEntryDataInt64(const Entry& ent, const int64* value, int32 count);
			void _SetEntryDataUInt64(const Entry& ent, const uint64* value, int32 count);
			void _SetEntryDataInt32(const Entry& ent, const int32* value, int32 count);
			void _SetEntryDataUInt32(const Entry& ent, const uint32* value, int32 count);
			void _SetEntryDataInt16(const Entry& ent, const int16* value, int32 count);
			void _SetEntryDataUInt16(const Entry& ent, const uint16* value, int32 count);
			void _SetEntryDataSingle(const Entry& ent, const float* value, int32 count);
			void _SetEntryDataDouble(const Entry& ent, const double* value, int32 count);
			void _SetEntryDataBool(const Entry& ent, const bool* value, int32 count);



			void _SetEntryDataVector2(const Entry& ent, const Vector2& vec);
			void _SetEntryDataVector3(const Entry& ent, const Vector3& vec);
			void _SetEntryDataVector4(const Entry& ent, const Vector4& vec);
			void _SetEntryDataMatrix(const Entry& ent, const Matrix& mat);
			void _SetEntryDataColor4(const Entry& ent, const Color4& clr);
			void _SetEntryDataString(const Entry& ent, const String& str);

			void _SetEntryDataVector2(const Entry& ent, const Vector2* vec, int32 count);
			void _SetEntryDataVector3(const Entry& ent, const Vector3* vec, int32 count);
			void _SetEntryDataVector4(const Entry& ent, const Vector4* vec, int32 count);
			void _SetEntryDataMatrix(const Entry& ent, const Matrix* mat, int32 count);
			void _SetEntryDataColor4(const Entry& ent, const Color4* clr, int32 count);
			void _SetEntryDataString(const Entry& ent, const String* str, int32 count);



			bool m_endianDependent;
			SectionTable m_positions;
			char m_buffer[32];

		};
	}
}

#endif