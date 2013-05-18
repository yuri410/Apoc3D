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
#include "TaggedData.h"
#include "BinaryReader.h"
#include "BinaryWriter.h"
#include "Streams.h"

#include "apoc3d/Apoc3DException.h"
#include "apoc3d/Config/ConfigurationSection.h"
#include "apoc3d/Math/Matrix.h"
#include "apoc3d/Math/Color.h"
#include "apoc3d/Math/Plane.h"
#include "apoc3d/Math/Quaternion.h"
#include "apoc3d/Math/Rectangle.h"
#include "apoc3d/Math/BoundingBox.h"
#include "apoc3d/Math/BoundingSphere.h"
#include "apoc3d/Math/Ray.h"
#include "apoc3d/Math/Viewport.h"
#include "apoc3d/Utility/StringUtils.h"

#include <sstream>

#include "IOUtils.h"

using namespace Apoc3D::Config;
using namespace Apoc3D::Utility;

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

		int64 TaggedDataReader::GetChunkOffset(const String& name) const
		{
			const Entry* ent = FindEntry(name);
			if (!ent)
				throw Apoc3DException::createException(EX_KeyNotFound, name.c_str());

			return ent->Offset;
		}
		void TaggedDataReader::Close() { m_stream->Close(); }

		void TaggedDataReader::FillTagList(List<String>& nameTags) const
		{
			for (SectionTable::Enumerator e = m_positions.GetEnumerator();e.MoveNext();)
			{
				nameTags.Add(*e.getCurrentKey());
			}
		}


		void TaggedDataReader::FillBuffer(const String& name, uint32 len)
		{
			assert(len <= sizeof(m_buffer));

			const Entry* ent = FindEntry(name);
			m_stream->setPosition(ent->Offset);
			m_stream->Read(reinterpret_cast<char*>(m_buffer), len);
		}
		void TaggedDataReader::FillBuffer(const Entry& ent, uint32 len)
		{
			assert(len <= sizeof(m_buffer));

			m_stream->setPosition(ent.Offset);
			m_stream->Read(reinterpret_cast<char*>(m_buffer), len);
		}
		void TaggedDataReader::FillBufferCurrent(uint32 len)
		{
			assert(len <= sizeof(m_buffer));
			m_stream->Read(reinterpret_cast<char*>(m_buffer), len);
		}
		bool TaggedDataReader::TryFillBuffer(const String& name, uint32 len)
		{
			const Entry* ent = FindEntry(name);
			if (!ent) return false;

			assert(len <= sizeof(m_buffer));
			m_stream->setPosition(ent->Offset);
			m_stream->Read(reinterpret_cast<char*>(m_buffer), len);
			return true;
		}

		BinaryReader* TaggedDataReader::TryGetData(const String& name) const
		{
			const Entry* ent = FindEntry(name);
			if (ent)
				return new BinaryReader(new VirtualStream(m_stream, ent->Offset, ent->Size));
			return nullptr;
		}
		BinaryReader* TaggedDataReader::GetData(const String& name) const
		{
			const Entry* ent = FindEntry(name);
			if (ent)
				return new BinaryReader(new VirtualStream(m_stream, ent->Offset, ent->Size));
			throw Apoc3DException::createException(EX_KeyNotFound, name.c_str());
		}


		Stream* TaggedDataReader::GetDataStream(const String& name) const 
		{
			const Entry* ent = FindEntry(name);
			if (ent)
				return new VirtualStream(m_stream, ent->Offset, ent->Size);
			throw Apoc3DException::createException(EX_KeyNotFound, name.c_str());
		}


#define TDR_GETDATA(name, value, getter) const Entry* ent = FindEntry(name); assert(ent); getter(ent, value);
#define TDR_GETDATA_ARR(name, value, count, getter) const Entry* ent = FindEntry(name); assert(ent); getter(ent, value, count);

		int64 TaggedDataReader::GetDataInt64(const String& name)	{ int64 r; TDR_GETDATA(name, r, _GetEntryInt64); return r; }
		uint64 TaggedDataReader::GetDataUInt64(const String& name)	{ uint64 r; TDR_GETDATA(name, r, _GetEntryUInt64); return r; }
		int32 TaggedDataReader::GetDataInt32(const String& name)	{ int32 r; TDR_GETDATA(name, r, _GetEntryInt32); return r; }
		uint32 TaggedDataReader::GetDataUInt32(const String& name)	{ uint32 r; TDR_GETDATA(name, r, _GetEntryUInt32); return r; }
		int16 TaggedDataReader::GetDataInt16(const String& name)	{ int16 r; TDR_GETDATA(name, r, _GetEntryInt16); return r; }
		uint16 TaggedDataReader::GetDataUInt16(const String& name)	{ uint16 r; TDR_GETDATA(name, r, _GetEntryUInt16); return r; }
		bool TaggedDataReader::GetDataBool(const String& name)		{ bool r; TDR_GETDATA(name, r, _GetEntryBool); return r; }
		float TaggedDataReader::GetDataSingle(const String& name)	{ float r; TDR_GETDATA(name, r, _GetEntrySingle); return r; }
		double TaggedDataReader::GetDataDouble(const String& name)	{ double r; TDR_GETDATA(name, r, _GetEntryDouble); return r; }

		void TaggedDataReader::GetDataInt64(const String& name, int64* value, int32 count)		{ TDR_GETDATA_ARR(name, value, count, _GetEntryInt64); }
		void TaggedDataReader::GetDataUInt64(const String& name, uint64* value, int32 count)	{ TDR_GETDATA_ARR(name, value, count, _GetEntryUInt64); }
		void TaggedDataReader::GetDataInt32(const String& name, int32* value, int32 count)		{ TDR_GETDATA_ARR(name, value, count, _GetEntryInt32); }
		void TaggedDataReader::GetDataUInt32(const String& name, uint32* value, int32 count)	{ TDR_GETDATA_ARR(name, value, count, _GetEntryUInt32); }
		void TaggedDataReader::GetDataInt16(const String& name, int16* value, int32 count)		{ TDR_GETDATA_ARR(name, value, count, _GetEntryInt16); }
		void TaggedDataReader::GetDataUInt16(const String& name, uint16* value, int32 count)	{ TDR_GETDATA_ARR(name, value, count, _GetEntryUInt16); }
		void TaggedDataReader::GetDataSingle(const String& name, float* value, int32 count)	{ TDR_GETDATA_ARR(name, value, count, _GetEntrySingle); }
		void TaggedDataReader::GetDataDouble(const String& name, double* value, int32 count)	{ TDR_GETDATA_ARR(name, value, count, _GetEntryDouble); }
		void TaggedDataReader::GetDataBool(const String& name, bool* value, int32 count)		{ TDR_GETDATA_ARR(name, value, count, _GetEntryBool); }
		
		void TaggedDataReader::GetDataVector2(const String& name, Vector2& vec)	{ TDR_GETDATA(name, vec, _GetEntryVector2); }
		void TaggedDataReader::GetDataVector3(const String& name, Vector3& vec)	{ TDR_GETDATA(name, vec, _GetEntryVector3); }
		void TaggedDataReader::GetDataVector4(const String& name, Vector4& vec)	{ TDR_GETDATA(name, vec, _GetEntryVector4); }
		void TaggedDataReader::GetDataColor4(const String& name, Color4& clr)		{ TDR_GETDATA(name, clr, _GetEntryColor4); }
		void TaggedDataReader::GetDataMatrix(const String& name, Matrix& mat)		{ TDR_GETDATA(name, mat, _GetEntryMatrix); }
		void TaggedDataReader::GetDataPlane(const String& name, Plane& plane)		{ TDR_GETDATA(name, plane, _GetEntryPlane); }
		void TaggedDataReader::GetDataQuaternion(const String& name, Quaternion& quat) { TDR_GETDATA(name, quat, _GetEntryQuaternion); }
		void TaggedDataReader::GetDataString(const String& name, String& str)		{ TDR_GETDATA(name, str, _GetEntryString); }
		void TaggedDataReader::GetDataRectangle(const String& name, Rectangle& rect)			{ TDR_GETDATA(name, rect, _GetEntryRectangle); }
		void TaggedDataReader::GetDataRectangleF(const String& name, RectangleF& rect)			{ TDR_GETDATA(name, rect, _GetEntryRectangleF); }
		void TaggedDataReader::GetDataPoint(const String& name, Point& pt)						{ TDR_GETDATA(name, pt, _GetEntryPoint); }
		void TaggedDataReader::GetDataPointF(const String& name, PointF& pt)					{ TDR_GETDATA(name, pt, _GetEntryPointF); }
		void TaggedDataReader::GetDataMathSize(const String& name, Apoc3D::Math::Size& sz)		{ TDR_GETDATA(name, sz, _GetEntryMathSize); }
		void TaggedDataReader::GetDataBoundingBox(const String& name, BoundingBox& bb)			{ TDR_GETDATA(name, bb, _GetEntryBoundingBox); }
		void TaggedDataReader::GetDataBoundingSphere(const String& name, BoundingSphere& bs)	{ TDR_GETDATA(name, bs, _GetEntryBoundingSphere); }
		void TaggedDataReader::GetDataRay(const String& name, Ray& r)							{ TDR_GETDATA(name, r, _GetEntryRay); }
		void TaggedDataReader::GetDataViewport(const String& name, Viewport& vp)				{ TDR_GETDATA(name, vp, _GetEntryViewport); }

		void TaggedDataReader::GetDataVector2(const String& name, Vector2* vec, int32 count)	{ TDR_GETDATA_ARR(name, vec, count, _GetEntryVector2); }
		void TaggedDataReader::GetDataVector3(const String& name, Vector3* vec, int32 count)	{ TDR_GETDATA_ARR(name, vec, count, _GetEntryVector3); }
		void TaggedDataReader::GetDataVector4(const String& name, Vector4* vec, int32 count)	{ TDR_GETDATA_ARR(name, vec, count, _GetEntryVector4); }
		void TaggedDataReader::GetDataColor4(const String& name, Color4* clr, int32 count)		{ TDR_GETDATA_ARR(name, clr, count, _GetEntryColor4); }
		void TaggedDataReader::GetDataMatrix(const String& name, Matrix* mat, int32 count)		{ TDR_GETDATA_ARR(name, mat, count, _GetEntryMatrix); }
		void TaggedDataReader::GetDataPlane(const String& name, Plane* str, int32 count)		{ TDR_GETDATA_ARR(name, str, count, _GetEntryPlane); }
		void TaggedDataReader::GetDataQuaternion(const String& name, Quaternion* quat, int32 count) { TDR_GETDATA_ARR(name, quat, count, _GetEntryQuaternion); }
		void TaggedDataReader::GetDataString(const String& name, String* str, int32 count)		{ TDR_GETDATA_ARR(name, str, count, _GetEntryString); }
		void TaggedDataReader::GetDataRectangle(const String& name, Rectangle* rect, int32 count)			{ TDR_GETDATA_ARR(name, rect, count, _GetEntryRectangle); }
		void TaggedDataReader::GetDataRectangleF(const String& name, RectangleF* rect, int32 count)		{ TDR_GETDATA_ARR(name, rect, count, _GetEntryRectangleF); }
		void TaggedDataReader::GetDataPoint(const String& name, Point* pt, int32 count)					{ TDR_GETDATA_ARR(name, pt, count, _GetEntryPoint); }
		void TaggedDataReader::GetDataPointF(const String& name, PointF* pt, int32 count)					{ TDR_GETDATA_ARR(name, pt, count, _GetEntryPointF); }
		void TaggedDataReader::GetDataMathSize(const String& name, Apoc3D::Math::Size* sz, int32 count)	{ TDR_GETDATA_ARR(name, sz, count, _GetEntryMathSize); }
		void TaggedDataReader::GetDataBoundingBox(const String& name, BoundingBox* bb, int32 count)		{ TDR_GETDATA_ARR(name, bb, count, _GetEntryBoundingBox); }
		void TaggedDataReader::GetDataBoundingSphere(const String& name, BoundingSphere* bs, int32 count)	{ TDR_GETDATA_ARR(name, bs, count, _GetEntryBoundingSphere); }
		void TaggedDataReader::GetDataRay(const String& name, Ray* r, int32 count)							{ TDR_GETDATA_ARR(name, r, count, _GetEntryRay); }
		void TaggedDataReader::GetDataViewport(const String& name, Viewport* vp, int32 count)				{ TDR_GETDATA_ARR(name, vp, count, _GetEntryViewport); }

#define TDR_TRYGETDATA(name, value, getter) const Entry* ent = FindEntry(name); if (ent) { getter(ent, value); return true; } return false;

		bool TaggedDataReader::TryGetDataInt64(const String& name, int64& v)	{ TDR_TRYGETDATA(name, v, _GetEntryInt64); }
		bool TaggedDataReader::TryGetDataUInt64(const String& name, uint64& v)	{ TDR_TRYGETDATA(name, v, _GetEntryUInt64); }
		bool TaggedDataReader::TryGetDataInt32(const String& name, int32& v)	{ TDR_TRYGETDATA(name, v, _GetEntryInt32); }
		bool TaggedDataReader::TryGetDataUInt32(const String& name, uint32& v)	{ TDR_TRYGETDATA(name, v, _GetEntryUInt32); }
		bool TaggedDataReader::TryGetDataInt16(const String& name, int16& v)	{ TDR_TRYGETDATA(name, v, _GetEntryInt16); }
		bool TaggedDataReader::TryGetDataUInt16(const String& name, uint16& v)	{ TDR_TRYGETDATA(name, v, _GetEntryUInt16); }
		bool TaggedDataReader::TryGetDataBool(const String& name, bool& v)		{ TDR_TRYGETDATA(name, v, _GetEntryBool); }
		bool TaggedDataReader::TryGetDataSingle(const String& name, float& v)	{ TDR_TRYGETDATA(name, v, _GetEntrySingle); }
		bool TaggedDataReader::TryGetDataDouble(const String& name, double& v)	{ TDR_TRYGETDATA(name, v, _GetEntryDouble); }

		bool TaggedDataReader::TryGetVector2(const String& name, Vector2& vec)			{ TDR_TRYGETDATA(name, vec, _GetEntryVector2); }
		bool TaggedDataReader::TryGetVector3(const String& name, Vector3& vec)			{ TDR_TRYGETDATA(name, vec, _GetEntryVector3); }
		bool TaggedDataReader::TryGetVector4(const String& name, Vector4& vec)			{ TDR_TRYGETDATA(name, vec, _GetEntryVector4); }
		bool TaggedDataReader::TryGetColor4(const String& name, Color4& clr)			{ TDR_TRYGETDATA(name, clr, _GetEntryColor4); }
		bool TaggedDataReader::TryGetString(const String& name, String& str)			{ TDR_TRYGETDATA(name, str, _GetEntryString); }
		bool TaggedDataReader::TryGetMatrix(const String& name, Matrix& mat)			{ TDR_TRYGETDATA(name, mat, _GetEntryMatrix); }
		bool TaggedDataReader::TryGetPlane(const String& name, Plane& plane)			{ TDR_TRYGETDATA(name, plane, _GetEntryPlane); }
		bool TaggedDataReader::TryGetQuaternion(const String& name, Quaternion& quat)  { TDR_TRYGETDATA(name, quat, _GetEntryQuaternion); }
		
#define TDR_TRYGETDATA_ARR(name, value, count, getter) const Entry* ent = FindEntry(name); if (ent) { getter(ent, value, count); return true; } return false;

		bool TaggedDataReader::TryGetDataInt64(const String& name, int64* value, int32 count)		{ TDR_TRYGETDATA_ARR(name, value, count, _GetEntryInt64); }
		bool TaggedDataReader::TryGetDataUInt64(const String& name, uint64* value, int32 count)	{ TDR_TRYGETDATA_ARR(name, value, count, _GetEntryUInt64); }
		bool TaggedDataReader::TryGetDataInt32(const String& name, int32* value, int32 count)		{ TDR_TRYGETDATA_ARR(name, value, count, _GetEntryInt32); }
		bool TaggedDataReader::TryGetDataUInt32(const String& name, uint32* value, int32 count)	{ TDR_TRYGETDATA_ARR(name, value, count, _GetEntryUInt32); }
		bool TaggedDataReader::TryGetDataInt16(const String& name, int16* value, int32 count)		{ TDR_TRYGETDATA_ARR(name, value, count, _GetEntryInt16); }
		bool TaggedDataReader::TryGetDataUInt16(const String& name, uint16* value, int32 count)	{ TDR_TRYGETDATA_ARR(name, value, count, _GetEntryUInt16); }
		bool TaggedDataReader::TryGetDataBool(const String& name, bool* value, int32 count)		{ TDR_TRYGETDATA_ARR(name, value, count, _GetEntryBool); }
		bool TaggedDataReader::TryGetDataSingle(const String& name, float* value, int32 count)		{ TDR_TRYGETDATA_ARR(name, value, count, _GetEntrySingle); }
		bool TaggedDataReader::TryGetDataDouble(const String& name, double* value, int32 count)	{ TDR_TRYGETDATA_ARR(name, value, count, _GetEntryDouble); }

		bool TaggedDataReader::TryGetVector2(const String& name, Vector2* vec, int32 count)		{ TDR_TRYGETDATA_ARR(name, vec, count, _GetEntryVector2); }
		bool TaggedDataReader::TryGetVector3(const String& name, Vector3* vec, int32 count)		{ TDR_TRYGETDATA_ARR(name, vec, count, _GetEntryVector3); }
		bool TaggedDataReader::TryGetVector4(const String& name, Vector4* vec, int32 count)		{ TDR_TRYGETDATA_ARR(name, vec, count, _GetEntryVector4); }
		bool TaggedDataReader::TryGetColor4(const String& name, Color4* clr, int32 count)			{ TDR_TRYGETDATA_ARR(name, clr, count, _GetEntryColor4); }
		bool TaggedDataReader::TryGetString(const String& name, String* str, int32 count)			{ TDR_TRYGETDATA_ARR(name, str, count, _GetEntryString); }
		bool TaggedDataReader::TryGetMatrix(const String& name, Matrix* mat, int32 count)			{ TDR_TRYGETDATA_ARR(name, mat, count, _GetEntryMatrix); }
		bool TaggedDataReader::TryGetPlane(const String& name, Plane* plane, int32 count)			{ TDR_TRYGETDATA_ARR(name, plane, count, _GetEntryPlane); }
		bool TaggedDataReader::TryGetQuaternion(const String& name, Quaternion* quat, int32 count)	{ TDR_TRYGETDATA_ARR(name, quat, count, _GetEntryQuaternion); }


		//////////////////////////////////////////////////////////////////////////
		//////////////////////////////////////////////////////////////////////////

#if LITTLE_ENDIAN
		void TaggedDataReader::_GetEntryInt64(const Entry* e, int64& val) { FillBuffer(*e, sizeof(int64)); val = ci64_le(m_buffer); }
		void TaggedDataReader::_GetEntryUInt64(const Entry* e, uint64& val) { FillBuffer(*e, sizeof(uint64)); val = cui64_le(m_buffer); }
		void TaggedDataReader::_GetEntryInt32(const Entry* e, int32& val) { FillBuffer(*e, sizeof(int32)); val = ci32_le(m_buffer); }
		void TaggedDataReader::_GetEntryUInt32(const Entry* e, uint32& val) { FillBuffer(*e, sizeof(uint32)); val = cui32_le(m_buffer);}
		void TaggedDataReader::_GetEntryInt16(const Entry* e, int16& val) { FillBuffer(*e, sizeof(int16)); val = ci16_le(m_buffer); }
		void TaggedDataReader::_GetEntryUInt16(const Entry* e, uint16& val) { FillBuffer(*e, sizeof(int16)); val = ci16_le(m_buffer); }
		void TaggedDataReader::_GetEntryBool(const Entry* e, bool& val) { FillBuffer(*e, sizeof(bool)); val = !!m_buffer[0]; }
		void TaggedDataReader::_GetEntrySingle(const Entry* e, float& val) { FillBuffer(*e, sizeof(float)); val = cr32_le(m_buffer); }
		void TaggedDataReader::_GetEntryDouble(const Entry* e, double& val) { FillBuffer(*e, sizeof(double)); val = cr64_le(m_buffer); }
#else
		void TaggedDataReader::_GetEntryInt64(const Entry* e, int64& val) { FillBuffer(*e, sizeof(int64)); val = m_endianDependent ? ci64_dep(m_buffer) : ci64_le(m_buffer); }
		void TaggedDataReader::_GetEntryUInt64(const Entry* e, uint64& val) { FillBuffer(*e, sizeof(uint64)); val = m_endianDependent ? cui64_dep(m_buffer) : cui64_le(m_buffer); }
		void TaggedDataReader::_GetEntryInt32(const Entry* e, int32& val) { FillBuffer(*e, sizeof(int32)); val = m_endianDependent ? ci32_dep(m_buffer) : ci32_le(m_buffer); }
		void TaggedDataReader::_GetEntryUInt32(const Entry* e, uint32& val) { FillBuffer(*e, sizeof(uint32)); val = m_endianDependent ? cui32_dep(m_buffer) : cui32_le(m_buffer);}
		void TaggedDataReader::_GetEntryInt16(const Entry* e, int16& val) { FillBuffer(*e, sizeof(int16)); val = m_endianDependent ? ci16_dep(m_buffer) : ci16_le(m_buffer); }
		void TaggedDataReader::_GetEntryUInt16(const Entry* e, uint16& val) { FillBuffer(*e, sizeof(int16)); val = m_endianDependent ? ci16_dep(m_buffer) : ci16_le(m_buffer); }
		void TaggedDataReader::_GetEntryBool(const Entry* e, bool& val) { FillBuffer(*e, sizeof(bool)); val = !!m_buffer[0]; }
		void TaggedDataReader::_GetEntrySingle(const Entry* e, float& val) { FillBuffer(*e, sizeof(float)); val = m_endianDependent ? cr32_dep(m_buffer) : cr32_le(m_buffer); }
		void TaggedDataReader::_GetEntryDouble(const Entry* e, double& val) { FillBuffer(*e, sizeof(double)); val = m_endianDependent ? cr64_dep(m_buffer) : cr64_le(m_buffer); }
#endif
		void TaggedDataReader::_GetEntryBool(const Entry* ent, bool* val, int32 len)
		{
			m_stream->setPosition(ent->Offset);
			while (len>=sizeof(m_buffer))
			{
				FillBufferCurrent(sizeof(m_buffer));
				for (int i=0;i<sizeof(m_buffer);i++) *val++ = !!m_buffer[i];
				len -= sizeof(m_buffer);
			}
			if (len > 0)
			{
				FillBufferCurrent(len);
				for (int i=0;i<len;i++) *val++ = !!m_buffer[i];
			}
		}
		void TaggedDataReader::_GetEntryInt64(const Entry* ent, int64* val, int32 len)
		{
			const int32 Chunk = 4;
			m_stream->setPosition(ent->Offset);
#if !LITTLE_ENDIAN
			if (m_endianDependent)
			{
				while (len>=Chunk)
				{
					FillBufferCurrent(sizeof(int64) * Chunk);
					for (int i=0;i<Chunk;i++) *val++ = ci64_dep(m_buffer + i * sizeof(int64));
					len -= Chunk;
				}
				if (len > 0)
				{
					FillBufferCurrent(sizeof(int64) * len);
					for (int i=0;i<len;i++) *val++ = ci64_dep(m_buffer + i * sizeof(int64));
				}
			}
			else
#endif
			{
				while (len>=Chunk)
				{
					FillBufferCurrent(sizeof(int64) * Chunk);
					for (int i=0;i<Chunk;i++) *val++ = ci64_le(m_buffer + i * sizeof(int64));
					len -= Chunk;
				}
				if (len > 0)
				{
					FillBufferCurrent(sizeof(int64) * len);
					for (int i=0;i<len;i++) *val++ = ci64_le(m_buffer + i * sizeof(int64));
				}
			}
		}
		void TaggedDataReader::_GetEntryUInt64(const Entry* ent, uint64* val, int32 len)
		{
			const int32 Chunk = 4;
			m_stream->setPosition(ent->Offset);
#if !LITTLE_ENDIAN
			if (m_endianDependent)
			{
				while (len>=Chunk)
				{
					FillBufferCurrent(sizeof(uint64) * Chunk);
					for (int i=0;i<Chunk;i++) *val++ = cui64_dep(m_buffer + i * sizeof(uint64));
					len -= Chunk;
				}
				if (len > 0)
				{
					FillBufferCurrent(sizeof(uint64) * len);
					for (int i=0;i<len;i++) *val++ = cui64_dep(m_buffer + i * sizeof(uint64));
				}
			}
			else
#endif
			{
				while (len>=Chunk)
				{
					FillBufferCurrent(sizeof(uint64) * Chunk);
					for (int i=0;i<Chunk;i++) *val++ = cui64_le(m_buffer + i * sizeof(uint64));
					len -= Chunk;
				}
				if (len > 0)
				{
					FillBufferCurrent(sizeof(uint64) * len);
					for (int i=0;i<len;i++) *val++ = cui64_le(m_buffer + i * sizeof(uint64));
				}
			}
		}
		void TaggedDataReader::_GetEntryInt32(const Entry* ent, int32* val, int32 len)
		{
			const int32 Chunk = 8;
			m_stream->setPosition(ent->Offset);
#if !LITTLE_ENDIAN
			if (m_endianDependent)
			{
				while (len>=Chunk)
				{
					FillBufferCurrent(sizeof(int32) * Chunk);
					for (int i=0;i<Chunk;i++) *val++ = ci32_dep(m_buffer + i * sizeof(int32));
					len -= Chunk;
				}
				if (len > 0)
				{
					FillBufferCurrent(sizeof(int32) * len);
					for (int i=0;i<len;i++) *val++ = ci32_dep(m_buffer + i * sizeof(int32));
				}
			}
			else
#endif
			{
				while (len>=Chunk)
				{
					FillBufferCurrent(sizeof(int32) * Chunk);
					for (int i=0;i<Chunk;i++) *val++ = ci32_le(m_buffer + i * sizeof(int32));
					len -= Chunk;
				}
				if (len > 0)
				{
					FillBufferCurrent(sizeof(int32) * len);
					for (int i=0;i<len;i++) *val++ = ci32_le(m_buffer + i * sizeof(int32));
				}
			}
		}
		void TaggedDataReader::_GetEntryUInt32(const Entry* ent, uint32* val, int32 len)
		{
			const int32 Chunk = 8;
			m_stream->setPosition(ent->Offset);
#if !LITTLE_ENDIAN
			if (m_endianDependent)
			{
				while (len>=Chunk)
				{
					FillBufferCurrent(sizeof(uint32) * Chunk);
					for (int i=0;i<Chunk;i++) *val++ = cui32_dep(m_buffer + i * sizeof(uint32));
					len -= Chunk;
				}
				if (len > 0)
				{
					FillBufferCurrent(sizeof(uint32) * len);
					for (int i=0;i<len;i++) *val++ = cui32_dep(m_buffer + i * sizeof(uint32));
				}
			}
			else
#endif
			{
				while (len>=Chunk)
				{
					FillBufferCurrent(sizeof(uint32) * Chunk);
					for (int i=0;i<Chunk;i++) *val++ = cui32_le(m_buffer + i * sizeof(uint32));
					len -= Chunk;
				}
				if (len > 0)
				{
					FillBufferCurrent(sizeof(uint32) * len);
					for (int i=0;i<len;i++) *val++ = cui32_le(m_buffer + i * sizeof(uint32));
				}
			}
		}
		void TaggedDataReader::_GetEntryInt16(const Entry* ent, int16* val, int32 len)
		{
			const int32 Chunk = 16;
			m_stream->setPosition(ent->Offset);
#if !LITTLE_ENDIAN
			if (m_endianDependent)
			{
				while (len>=Chunk)
				{
					FillBufferCurrent(sizeof(int16) * Chunk);
					for (int i=0;i<Chunk;i++) *val++ = ci16_dep(m_buffer + i * sizeof(int16));
					len -= Chunk;
				}
				if (len > 0)
				{
					FillBufferCurrent(sizeof(int16) * len);
					for (int i=0;i<len;i++) *val++ = ci16_dep(m_buffer + i * sizeof(int16));
				}
			}
			else
#endif
			{
				while (len>=Chunk)
				{
					FillBufferCurrent(sizeof(int16) * Chunk);
					for (int i=0;i<Chunk;i++) *val++ = ci16_le(m_buffer + i * sizeof(int16));
					len -= Chunk;
				}
				if (len > 0)
				{
					FillBufferCurrent(sizeof(int16) * len);
					for (int i=0;i<len;i++) *val++ = ci16_le(m_buffer + i * sizeof(int16));
				}
			}
		}
		void TaggedDataReader::_GetEntryUInt16(const Entry* ent, uint16* val, int32 len)
		{
			const int32 Chunk = 16;
			m_stream->setPosition(ent->Offset);
#if !LITTLE_ENDIAN
			if (m_endianDependent)
			{
				while (len>=Chunk)
				{
					FillBufferCurrent(sizeof(uint16) * Chunk);
					for (int i=0;i<Chunk;i++) *val++ = cui16_dep(m_buffer + i * sizeof(uint16));
					len -= Chunk;
				}
				if (len > 0)
				{
					FillBufferCurrent(sizeof(uint16) * len);
					for (int i=0;i<len;i++) *val++ = cui16_dep(m_buffer + i * sizeof(uint16));
				}
			}
			else
#endif
			{
				while (len>=Chunk)
				{
					FillBufferCurrent(sizeof(uint16) * Chunk);
					for (int i=0;i<Chunk;i++) *val++ = cui16_le(m_buffer + i * sizeof(uint16));
					len -= Chunk;
				}
				if (len > 0)
				{
					FillBufferCurrent(sizeof(uint16) * len);
					for (int i=0;i<len;i++) *val++ = cui16_le(m_buffer + i * sizeof(uint16));
				}
			}
		}
		void TaggedDataReader::_GetEntrySingle(const Entry* ent, float* val, int32 len)
		{
			const int32 Chunk = 8;
			m_stream->setPosition(ent->Offset);
#if !LITTLE_ENDIAN
			if (m_endianDependent)
			{
				while (len>=Chunk)
				{
					FillBufferCurrent(sizeof(float) * Chunk);
					for (int i=0;i<Chunk;i++) *val++ = cr32_dep(m_buffer + i * sizeof(float));
					len -= Chunk;
				}
				if (len > 0)
				{
					FillBufferCurrent(sizeof(float) * len);
					for (int i=0;i<len;i++) *val++ = cr32_dep(m_buffer + i * sizeof(float));
				}
			}
			else
#endif
			{
				while (len>=Chunk)
				{
					FillBufferCurrent(sizeof(float) * Chunk);
					for (int i=0;i<Chunk;i++) *val++ = cr32_le(m_buffer + i * sizeof(float));
					len -= Chunk;
				}
				if (len > 0)
				{
					FillBufferCurrent(sizeof(float) * len);
					for (int i=0;i<len;i++) *val++ = cr32_le(m_buffer + i * sizeof(float));
				}
			}
		}
		void TaggedDataReader::_GetEntryDouble(const Entry* ent, double* val, int32 len)
		{
			const int32 Chunk = 4;
			m_stream->setPosition(ent->Offset);
#if !LITTLE_ENDIAN
			if (m_endianDependent)
			{
				while (len>=Chunk)
				{
					FillBufferCurrent(sizeof(double) * Chunk);
					for (int i=0;i<Chunk;i++) *val++ = cr64_dep(m_buffer + i * sizeof(double));
					len -= Chunk;
				}
				if (len > 0)
				{
					FillBufferCurrent(sizeof(double) * len);
					for (int i=0;i<len;i++) *val++ = cr64_dep(m_buffer + i * sizeof(double));
				}
			}
			else
#endif
			{
				while (len>=Chunk)
				{
					FillBufferCurrent(sizeof(double) * Chunk);
					for (int i=0;i<Chunk;i++) *val++ = cr64_le(m_buffer + i * sizeof(double));
					len -= Chunk;
				}
				if (len > 0)
				{
					FillBufferCurrent(sizeof(double) * len);
					for (int i=0;i<len;i++) *val++ = cr64_le(m_buffer + i * sizeof(double));
				}
			}
		}

		void TaggedDataReader::_GetEntryVector2(const Entry* ent, Vector2& vec)
		{
			FillBuffer(*ent, sizeof(float)*2);
#if !LITTLE_ENDIAN
			if (m_endianDependent)
			{
				v2x(vec) = cr32_dep(m_buffer);
				v2y(vec) = cr32_dep(m_buffer + sizeof(float));
			}
			else
#endif
			{
				v2x(vec) = cr32_le(m_buffer);
				v2y(vec) = cr32_le(m_buffer + sizeof(float));
			}
		}
		void TaggedDataReader::_GetEntryVector3(const Entry* ent, Vector3& vec)
		{
			FillBuffer(*ent, sizeof(float)*3);
#if !LITTLE_ENDIAN
			if (m_endianDependent)
			{
				v3x(vec) = cr32_dep(m_buffer);
				v3y(vec) = cr32_dep(m_buffer + sizeof(float));
				v3z(vec) = cr32_dep(m_buffer + sizeof(float) * 2);
			}
			else
#endif
			{
				v3x(vec) = cr32_le(m_buffer);
				v3y(vec) = cr32_le(m_buffer + sizeof(float));
				v3z(vec) = cr32_le(m_buffer + sizeof(float) * 2);
			}
		}
		void TaggedDataReader::_GetEntryVector4(const Entry* ent, Vector4& vec)
		{
			FillBuffer(*ent, sizeof(float)*4);
#if !LITTLE_ENDIAN
			if (m_endianDependent)
			{
				v4x(vec) = cr32_dep(m_buffer);
				v4y(vec) = cr32_dep(m_buffer + sizeof(float));
				v4z(vec) = cr32_dep(m_buffer + sizeof(float) * 2);
				v4w(vec) = cr32_dep(m_buffer + sizeof(float) * 3);
			}
			else
#endif
			{
				v4x(vec) = cr32_le(m_buffer);
				v4y(vec) = cr32_le(m_buffer + sizeof(float));
				v4z(vec) = cr32_le(m_buffer + sizeof(float) * 2);
				v4w(vec) = cr32_le(m_buffer + sizeof(float) * 3);
			}
		}
		void TaggedDataReader::_GetEntryColor4(const Entry* ent, Color4& clr)
		{
			FillBuffer(*ent, sizeof(float)*4);
#if !LITTLE_ENDIAN
			if (m_endianDependent)
			{
				clr.Red = cr32_dep(m_buffer);
				clr.Green = cr32_dep(m_buffer + sizeof(float));
				clr.Blue = cr32_dep(m_buffer + sizeof(float) * 2);
				clr.Alpha = cr32_dep(m_buffer + sizeof(float) * 3);
			}
			else
#endif
			{
				clr.Red = cr32_le(m_buffer);
				clr.Green = cr32_le(m_buffer + sizeof(float));
				clr.Blue = cr32_le(m_buffer + sizeof(float) * 2);
				clr.Alpha = cr32_le(m_buffer + sizeof(float) * 3);
			}
		}
		void TaggedDataReader::_GetEntryMatrix(const Entry* ent, Matrix& mat)
		{
#if !LITTLE_ENDIAN
			if (m_endianDependent)
			{
				FillBuffer(*ent, sizeof(float)*8);
				mat.M11 = cr32_dep(m_buffer);
				mat.M12 = cr32_dep(m_buffer + sizeof(float));
				mat.M13 = cr32_dep(m_buffer + sizeof(float) * 2);
				mat.M14 = cr32_dep(m_buffer + sizeof(float) * 3);
				mat.M21 = cr32_dep(m_buffer + sizeof(float) * 4);
				mat.M22 = cr32_dep(m_buffer + sizeof(float) * 5);
				mat.M23 = cr32_dep(m_buffer + sizeof(float) * 6);
				mat.M24 = cr32_dep(m_buffer + sizeof(float) * 7);

				FillBufferCurrent(sizeof(float)*8);
				mat.M31 = cr32_dep(m_buffer);
				mat.M32 = cr32_dep(m_buffer + sizeof(float));
				mat.M33 = cr32_dep(m_buffer + sizeof(float) * 2);
				mat.M34 = cr32_dep(m_buffer + sizeof(float) * 3);
				mat.M41 = cr32_dep(m_buffer + sizeof(float) * 4);
				mat.M42 = cr32_dep(m_buffer + sizeof(float) * 5);
				mat.M43 = cr32_dep(m_buffer + sizeof(float) * 6);
				mat.M44 = cr32_dep(m_buffer + sizeof(float) * 7);
			}
			else
#endif
			{
				FillBuffer(*ent, sizeof(float)*8);
				mat.M11 = cr32_le(m_buffer);
				mat.M12 = cr32_le(m_buffer + sizeof(float));
				mat.M13 = cr32_le(m_buffer + sizeof(float) * 2);
				mat.M14 = cr32_le(m_buffer + sizeof(float) * 3);
				mat.M21 = cr32_le(m_buffer + sizeof(float) * 4);
				mat.M22 = cr32_le(m_buffer + sizeof(float) * 5);
				mat.M23 = cr32_le(m_buffer + sizeof(float) * 6);
				mat.M24 = cr32_le(m_buffer + sizeof(float) * 7);

				FillBufferCurrent(sizeof(float)*8);
				mat.M31 = cr32_le(m_buffer);
				mat.M32 = cr32_le(m_buffer + sizeof(float));
				mat.M33 = cr32_le(m_buffer + sizeof(float) * 2);
				mat.M34 = cr32_le(m_buffer + sizeof(float) * 3);
				mat.M41 = cr32_le(m_buffer + sizeof(float) * 4);
				mat.M42 = cr32_le(m_buffer + sizeof(float) * 5);
				mat.M43 = cr32_le(m_buffer + sizeof(float) * 6);
				mat.M44 = cr32_le(m_buffer + sizeof(float) * 7);
			}
		}
		void TaggedDataReader::_GetEntryString(const Entry* ent, String& str)
		{
			VirtualStream vStrm(m_stream, ent->Offset, ent->Size);
			BinaryReader br(&vStrm);
			br.SuspendStreamRelease();

			str = br.ReadString();

			br.Close();
		}
		void TaggedDataReader::_GetEntryPlane(const Entry* ent, Plane& plane)
		{
			FillBuffer(*ent, sizeof(float)*4);
#if !LITTLE_ENDIAN
			if (m_endianDependent)
			{
				plane.X = cr32_dep(m_buffer);
				plane.Y = cr32_dep(m_buffer + sizeof(float));
				plane.Z = cr32_dep(m_buffer + sizeof(float) * 2);
				plane.D = cr32_dep(m_buffer + sizeof(float) * 3);
			}
			else
#endif
			{
				plane.X = cr32_le(m_buffer);
				plane.Y = cr32_le(m_buffer + sizeof(float));
				plane.Z = cr32_le(m_buffer + sizeof(float) * 2);
				plane.D = cr32_le(m_buffer + sizeof(float) * 3);
			}
		}
		void TaggedDataReader::_GetEntryQuaternion(const Entry* e, Quaternion& quat)
		{
			FillBuffer(*e, sizeof(float)*4);
#if !LITTLE_ENDIAN
			if (m_endianDependent)
			{
				quat.X = cr32_dep(m_buffer);
				quat.Y = cr32_dep(m_buffer + sizeof(float));
				quat.Z = cr32_dep(m_buffer + sizeof(float) * 2);
				quat.W = cr32_dep(m_buffer + sizeof(float) * 3);
			}
			else
#endif
			{
				quat.X = cr32_le(m_buffer);
				quat.Y = cr32_le(m_buffer + sizeof(float));
				quat.Z = cr32_le(m_buffer + sizeof(float) * 2);
				quat.W = cr32_le(m_buffer + sizeof(float) * 3);
			}
		}
		void TaggedDataReader::_GetEntryRectangle(const Entry* e, Rectangle& rect)
		{
			FillBuffer(*e, sizeof(int32)*4);
#if !LITTLE_ENDIAN
			if (m_endianDependent)
			{
				rect.X = ci32_dep(m_buffer);
				rect.Y = ci32_dep(m_buffer + sizeof(int32));
				rect.Width = ci32_dep(m_buffer + sizeof(int32)*2);
				rect.Height = ci32_dep(m_buffer + sizeof(int32)*3);
			}
			else
#endif
			{
				rect.X = ci32_le(m_buffer);
				rect.Y = ci32_le(m_buffer + sizeof(int32));
				rect.Width = ci32_le(m_buffer + sizeof(int32)*2);
				rect.Height = ci32_le(m_buffer + sizeof(int32)*3);
			}
		}
		void TaggedDataReader::_GetEntryRectangleF(const Entry* e, RectangleF& rect)
		{
			FillBuffer(*e, sizeof(float)*4);
#if !LITTLE_ENDIAN
			if (m_endianDependent)
			{
				rect.X = cr32_dep(m_buffer);
				rect.Y = cr32_dep(m_buffer + sizeof(float));
				rect.Width = cr32_dep(m_buffer + sizeof(float)*2);
				rect.Height = cr32_dep(m_buffer + sizeof(float)*3);
			}
			else
#endif
			{
				rect.X = cr32_le(m_buffer);
				rect.Y = cr32_le(m_buffer + sizeof(float));
				rect.Width = cr32_le(m_buffer + sizeof(float)*2);
				rect.Height = cr32_le(m_buffer + sizeof(float)*3);
			}
		}
		void TaggedDataReader::_GetEntryPoint(const Entry* e, Point& pt)
		{
			FillBuffer(*e, sizeof(int32)*2);
#if !LITTLE_ENDIAN
			if (m_endianDependent)
			{
				pt.X = ci32_dep(m_buffer);
				pt.Y = ci32_dep(m_buffer + sizeof(int32));
			}
			else
#endif
			{
				pt.X = ci32_le(m_buffer);
				pt.Y = ci32_le(m_buffer + sizeof(int32));
			}
		}
		void TaggedDataReader::_GetEntryPointF(const Entry* e, PointF& pt)
		{
			FillBuffer(*e, sizeof(float)*2);
#if !LITTLE_ENDIAN
			if (m_endianDependent)
			{
				pt.X = cr32_dep(m_buffer);
				pt.Y = cr32_dep(m_buffer + sizeof(float));
			}
			else
#endif
			{
				pt.X = cr32_le(m_buffer);
				pt.Y = cr32_le(m_buffer + sizeof(float));
			}
		}
		void TaggedDataReader::_GetEntryMathSize(const Entry* e, Apoc3D::Math::Size& sz)
		{
			FillBuffer(*e, sizeof(int32)*2);
#if !LITTLE_ENDIAN
			if (m_endianDependent)
			{
				sz.Width = ci32_dep(m_buffer);
				sz.Height = ci32_dep(m_buffer + sizeof(int32));
			}
			else
#endif
			{
				sz.Width = ci32_le(m_buffer);
				sz.Height = ci32_le(m_buffer + sizeof(int32));
			}
		}
		void TaggedDataReader::_GetEntryBoundingBox(const Entry* e, BoundingBox& bb)
		{
#if !LITTLE_ENDIAN
			if (m_endianDependent)
			{
				FillBuffer(*e, sizeof(float)*6);
				v3x(bb.Minimum) = cr32_dep(m_buffer);
				v3y(bb.Minimum) = cr32_dep(m_buffer + sizeof(float));
				v3z(bb.Minimum) = cr32_dep(m_buffer + sizeof(float) * 2);

				v3x(bb.Maximum) = cr32_dep(m_buffer + sizeof(float) * 3);
				v3y(bb.Maximum) = cr32_dep(m_buffer + sizeof(float) * 4);
				v3z(bb.Maximum) = cr32_dep(m_buffer + sizeof(float) * 5);
			}
			else
#endif
			{
				FillBuffer(*e, sizeof(float)*6);
				v3x(bb.Minimum) = cr32_le(m_buffer);
				v3y(bb.Minimum) = cr32_le(m_buffer + sizeof(float));
				v3z(bb.Minimum) = cr32_le(m_buffer + sizeof(float) * 2);

				v3x(bb.Maximum) = cr32_le(m_buffer + sizeof(float) * 3);
				v3y(bb.Maximum) = cr32_le(m_buffer + sizeof(float) * 4);
				v3z(bb.Maximum) = cr32_le(m_buffer + sizeof(float) * 5);
			}
		}
		void TaggedDataReader::_GetEntryBoundingSphere(const Entry* e, BoundingSphere& bs)
		{
#if !LITTLE_ENDIAN
			if (m_endianDependent)
			{
				FillBuffer(*e, sizeof(float)*4);
				v3x(bs.Center) = cr32_dep(m_buffer);
				v3y(bs.Center) = cr32_dep(m_buffer + sizeof(float));
				v3z(bs.Center) = cr32_dep(m_buffer + sizeof(float) * 2);
				bs.Radius = cr32_dep(m_buffer + sizeof(float) * 3);
			}
			else
#endif
			{
				FillBuffer(*e, sizeof(float)*4);
				v3x(bs.Center) = cr32_le(m_buffer);
				v3y(bs.Center) = cr32_le(m_buffer + sizeof(float));
				v3z(bs.Center) = cr32_le(m_buffer + sizeof(float) * 2);
				bs.Radius = cr32_le(m_buffer + sizeof(float) * 3);
			}			
		}
		void TaggedDataReader::_GetEntryRay(const Entry* e, Ray& r)
		{
#if !LITTLE_ENDIAN
			if (m_endianDependent)
			{
				FillBuffer(*e, sizeof(float)*6);
				v3x(r.Position) = cr32_dep(m_buffer);
				v3y(r.Position) = cr32_dep(m_buffer + sizeof(float));
				v3z(r.Position) = cr32_deps(m_buffer + sizeof(float) * 2);

				v3x(r.Direction) = cr32_dep(m_buffer + sizeof(float) * 3);
				v3y(r.Direction) = cr32_dep(m_buffer + sizeof(float) * 4);
				v3z(r.Direction) = cr32_dep(m_buffer + sizeof(float) * 5);
			}
			else
#endif
			{
				FillBuffer(*e, sizeof(float)*6);
				v3x(r.Position) = cr32_le(m_buffer);
				v3y(r.Position) = cr32_le(m_buffer + sizeof(float));
				v3z(r.Position) = cr32_le(m_buffer + sizeof(float) * 2);

				v3x(r.Direction) = cr32_le(m_buffer + sizeof(float) * 3);
				v3y(r.Direction) = cr32_le(m_buffer + sizeof(float) * 4);
				v3z(r.Direction) = cr32_le(m_buffer + sizeof(float) * 5);
			}
		}
		void TaggedDataReader::_GetEntryViewport(const Entry* e, Viewport& vp)
		{
#if !LITTLE_ENDIAN
			if (m_endianDependent)
			{
				FillBuffer(*e, sizeof(int32)*4);
				vp.X = ci32_dep(m_buffer);
				vp.Y = ci32_dep(m_buffer + sizeof(int32));
				vp.Width = ci32_dep(m_buffer + sizeof(int32)*2);
				vp.Height = ci32_dep(m_buffer + sizeof(int32)*3);

				FillBufferCurrent(sizeof(float)*2);
				vp.MinZ = cr32_dep(m_buffer);
				vp.MaxZ = cr32_dep(m_buffer + sizeof(float));
			}
			else
#endif
			{
				FillBuffer(*e, sizeof(int32)*4);
				vp.X = ci32_le(m_buffer);
				vp.Y = ci32_le(m_buffer + sizeof(int32));
				vp.Width = ci32_le(m_buffer + sizeof(int32)*2);
				vp.Height = ci32_le(m_buffer + sizeof(int32)*3);

				FillBufferCurrent(sizeof(float)*2);
				vp.MinZ = cr32_le(m_buffer);
				vp.MaxZ = cr32_le(m_buffer + sizeof(float));
			}
		}

		void TaggedDataReader::_GetEntryVector2(const Entry* ent, Vector2* value, int len)
		{
			if (len>0) _GetEntryVector2(ent, value[0]);
#if !LITTLE_ENDIAN
			if (m_endianDependent)
			{
				for (int i=1;i<len;i++)
				{
					FillBufferCurrent(sizeof(float)*2);
					v2x(value[i]) = cr32_dep(m_buffer);
					v2y(value[i]) = cr32_dep(m_buffer + sizeof(float));
				}
			}
			else
#endif
			{
				for (int i=1;i<len;i++)
				{
					FillBufferCurrent(sizeof(float)*2);
					v2x(value[i]) = cr32_le(m_buffer);
					v2y(value[i]) = cr32_le(m_buffer + sizeof(float));
				}
			}
		}
		void TaggedDataReader::_GetEntryVector3(const Entry* ent, Vector3* value, int len)
		{
			if (len>0) _GetEntryVector3(ent, value[0]);
#if !LITTLE_ENDIAN
			if (m_endianDependent)
			{
				for (int i=1;i<len;i++)
				{
					Vector3& vec = value[i];
					FillBufferCurrent(sizeof(float)*3);
					v3x(vec) = cr32_dep(m_buffer);
					v3y(vec) = cr32_dep(m_buffer + sizeof(float));
					v3z(vec) = cr32_dep(m_buffer + sizeof(float) * 2);
				}
			}
			else
#endif
			{
				for (int i=1;i<len;i++)
				{
					Vector3& vec = value[i];
					FillBufferCurrent(sizeof(float)*3);
					v3x(vec) = cr32_le(m_buffer);
					v3y(vec) = cr32_le(m_buffer + sizeof(float));
					v3z(vec) = cr32_le(m_buffer + sizeof(float) * 2);
				}
			}
		}
		void TaggedDataReader::_GetEntryVector4(const Entry* ent, Vector4* value, int len)
		{
			if (len>0) _GetEntryVector4(ent, value[0]);
#if !LITTLE_ENDIAN
			if (m_endianDependent)
			{
				for (int i=1;i<len;i++)
				{
					Vector4& vec = value[i];
					FillBufferCurrent(sizeof(float)*4);
					v4x(vec) = cr32_dep(m_buffer);
					v4y(vec) = cr32_dep(m_buffer + sizeof(float));
					v4z(vec) = cr32_dep(m_buffer + sizeof(float) * 2);
					v4w(vec) = cr32_dep(m_buffer + sizeof(float) * 3);
				}
			}
			else
#endif
			{
				for (int i=1;i<len;i++)
				{
					Vector4& vec = value[i];
					FillBufferCurrent(sizeof(float)*4);
					v4x(vec) = cr32_le(m_buffer);
					v4y(vec) = cr32_le(m_buffer + sizeof(float));
					v4z(vec) = cr32_le(m_buffer + sizeof(float) * 2);
					v4w(vec) = cr32_le(m_buffer + sizeof(float) * 3);
				}
			}
		}
		void TaggedDataReader::_GetEntryMatrix(const Entry* ent, Matrix* value, int len)
		{
			if (len>0) _GetEntryMatrix(ent, value[0]);
#if !LITTLE_ENDIAN
			if (m_endianDependent)
			{
				for (int i=1;i<len;i++)
				{
					Matrix& mat = value[i];

					FillBufferCurrent(sizeof(float)*8);
					mat.M11 = cr32_dep(m_buffer);
					mat.M12 = cr32_dep(m_buffer + sizeof(float));
					mat.M13 = cr32_dep(m_buffer + sizeof(float) * 2);
					mat.M14 = cr32_dep(m_buffer + sizeof(float) * 3);
					mat.M21 = cr32_dep(m_buffer + sizeof(float) * 4);
					mat.M22 = cr32_dep(m_buffer + sizeof(float) * 5);
					mat.M23 = cr32_dep(m_buffer + sizeof(float) * 6);
					mat.M24 = cr32_dep(m_buffer + sizeof(float) * 7);

					FillBufferCurrent(sizeof(float)*8);
					mat.M31 = cr32_dep(m_buffer);
					mat.M32 = cr32_dep(m_buffer + sizeof(float));
					mat.M33 = cr32_dep(m_buffer + sizeof(float) * 2);
					mat.M34 = cr32_dep(m_buffer + sizeof(float) * 3);
					mat.M41 = cr32_dep(m_buffer + sizeof(float) * 4);
					mat.M42 = cr32_dep(m_buffer + sizeof(float) * 5);
					mat.M43 = cr32_dep(m_buffer + sizeof(float) * 6);
					mat.M44 = cr32_dep(m_buffer + sizeof(float) * 7);
				}
			}
			else
#endif
			{
				for (int i=1;i<len;i++)
				{
					Matrix& mat = value[i];

					FillBufferCurrent(sizeof(float)*8);
					mat.M11 = cr32_le(m_buffer);
					mat.M12 = cr32_le(m_buffer + sizeof(float));
					mat.M13 = cr32_le(m_buffer + sizeof(float) * 2);
					mat.M14 = cr32_le(m_buffer + sizeof(float) * 3);
					mat.M21 = cr32_le(m_buffer + sizeof(float) * 4);
					mat.M22 = cr32_le(m_buffer + sizeof(float) * 5);
					mat.M23 = cr32_le(m_buffer + sizeof(float) * 6);
					mat.M24 = cr32_le(m_buffer + sizeof(float) * 7);

					FillBufferCurrent(sizeof(float)*8);
					mat.M31 = cr32_le(m_buffer);
					mat.M32 = cr32_le(m_buffer + sizeof(float));
					mat.M33 = cr32_le(m_buffer + sizeof(float) * 2);
					mat.M34 = cr32_le(m_buffer + sizeof(float) * 3);
					mat.M41 = cr32_le(m_buffer + sizeof(float) * 4);
					mat.M42 = cr32_le(m_buffer + sizeof(float) * 5);
					mat.M43 = cr32_le(m_buffer + sizeof(float) * 6);
					mat.M44 = cr32_le(m_buffer + sizeof(float) * 7);
				}
			}
		}
		void TaggedDataReader::_GetEntryColor4(const Entry* ent, Color4* value, int len)
		{
			if (len>0) _GetEntryColor4(ent, value[0]);
#if !LITTLE_ENDIAN
			if (m_endianDependent)
			{
				for (int i=1;i<len;i++)
				{
					Color4& clr = value[i];
					FillBufferCurrent(sizeof(float)*4);
					clr.Red = cr32_dep(m_buffer);
					clr.Green = cr32_dep(m_buffer + sizeof(float));
					clr.Blue = cr32_dep(m_buffer + sizeof(float) * 2);
					clr.Alpha = cr32_dep(m_buffer + sizeof(float) * 3);
				}
			}
			else
#endif
			{
				for (int i=1;i<len;i++)
				{
					Color4& clr = value[i];
					FillBufferCurrent(sizeof(float)*4);
					clr.Red = cr32_le(m_buffer);
					clr.Green = cr32_le(m_buffer + sizeof(float));
					clr.Blue = cr32_le(m_buffer + sizeof(float) * 2);
					clr.Alpha = cr32_le(m_buffer + sizeof(float) * 3);
				}
			}
		}
		void TaggedDataReader::_GetEntryString(const Entry* ent, String* value, int len)
		{
			VirtualStream vStrm(m_stream, ent->Offset, ent->Size);
			BinaryReader br(&vStrm);
			br.SuspendStreamRelease();

			for (int32 i=0;i<len;i++)
				value[i] = br.ReadString();

			br.Close();
		}
		void TaggedDataReader::_GetEntryPlane(const Entry* ent, Plane* plane, int len)
		{
			if (len>0) _GetEntryPlane(ent, plane[0]);
#if !LITTLE_ENDIAN
			if (m_endianDependent)
			{
				for (int i=1;i<len;i++)
				{
					Plane& pl = plane[i];
					FillBufferCurrent(sizeof(float)*4);
					pl.X = cr32_dep(m_buffer);
					pl.Y = cr32_dep(m_buffer + sizeof(float));
					pl.Z = cr32_dep(m_buffer + sizeof(float) * 2);
					pl.D = cr32_dep(m_buffer + sizeof(float) * 3);
				}
			}
			else
#endif
			{
				for (int i=1;i<len;i++)
				{
					Plane& pl = plane[i];
					FillBufferCurrent(sizeof(float)*4);
					pl.X = cr32_le(m_buffer);
					pl.Y = cr32_le(m_buffer + sizeof(float));
					pl.Z = cr32_le(m_buffer + sizeof(float) * 2);
					pl.D = cr32_le(m_buffer + sizeof(float) * 3);
				}
			}
		}
		void TaggedDataReader::_GetEntryQuaternion(const Entry* ent, Quaternion* quat, int len)
		{
			if (len>0) _GetEntryQuaternion(ent, quat[0]);
#if !LITTLE_ENDIAN
			if (m_endianDependent)
			{
				for (int i=1;i<len;i++)
				{
					Quaternion& q = quat[i];
					FillBufferCurrent(sizeof(float)*4);
					q.X = cr32_dep(m_buffer);
					q.Y = cr32_dep(m_buffer + sizeof(float));
					q.Z = cr32_dep(m_buffer + sizeof(float) * 2);
					q.W = cr32_dep(m_buffer + sizeof(float) * 3);
				}
			}
			else
#endif
			{
				for (int i=1;i<len;i++)
				{
					Quaternion& q = quat[i];
					FillBufferCurrent(sizeof(float)*4);
					q.X = cr32_le(m_buffer);
					q.Y = cr32_le(m_buffer + sizeof(float));
					q.Z = cr32_le(m_buffer + sizeof(float) * 2);
					q.W = cr32_le(m_buffer + sizeof(float) * 3);
				}
			}
		}
		void TaggedDataReader::_GetEntryRectangle(const Entry* ent, Rectangle* rect, int len)
		{
			if (len>0) _GetEntryRectangle(ent, rect[0]);
#if !LITTLE_ENDIAN
			if (m_endianDependent)
			{
				for (int i=1;i<len;i++)
				{
					Rectangle& r = rect[i];
					FillBufferCurrent(sizeof(int32)*4);
					r.X = ci32_dep(m_buffer);
					r.Y = ci32_dep(m_buffer + sizeof(int32));
					r.Width = ci32_dep(m_buffer + sizeof(int32)*2);
					r.Height = ci32_dep(m_buffer + sizeof(int32)*3);
				}
			}
			else
#endif
			{
				for (int i=1;i<len;i++)
				{
					Rectangle& r = rect[i];
					FillBufferCurrent(sizeof(int32)*4);
					r.X = ci32_le(m_buffer);
					r.Y = ci32_le(m_buffer + sizeof(int32));
					r.Width = ci32_le(m_buffer + sizeof(int32)*2);
					r.Height = ci32_le(m_buffer + sizeof(int32)*3);
				}
			}
		}
		void TaggedDataReader::_GetEntryRectangleF(const Entry* ent, RectangleF* rect, int len)
		{
			if (len>0) _GetEntryRectangleF(ent, rect[0]);
#if !LITTLE_ENDIAN
			if (m_endianDependent)
			{
				for (int i=1;i<len;i++)
				{
					RectangleF& r = rect[i];
					FillBufferCurrent(sizeof(float)*4);
					r.X = cr32_dep(m_buffer);
					r.Y = cr32_dep(m_buffer + sizeof(float));
					r.Width = cr32_dep(m_buffer + sizeof(float)*2);
					r.Height = cr32_dep(m_buffer + sizeof(float)*3);
				}
			}
			else
#endif
			{
				for (int i=1;i<len;i++)
				{
					RectangleF& r = rect[i];
					FillBufferCurrent(sizeof(float)*4);
					r.X = cr32_le(m_buffer);
					r.Y = cr32_le(m_buffer + sizeof(float));
					r.Width = cr32_le(m_buffer + sizeof(float)*2);
					r.Height = cr32_le(m_buffer + sizeof(float)*3);
				}
			}
		}
		void TaggedDataReader::_GetEntryPoint(const Entry* ent, Point* pt, int len)
		{
			if (len>0) _GetEntryPoint(ent, pt[0]);
#if !LITTLE_ENDIAN
			if (m_endianDependent)
			{
				for (int i=1;i<len;i++)
				{
					Point& p = pt[i];
					FillBufferCurrent(sizeof(int32)*2);
					p.X = ci32_dep(m_buffer);
					p.Y = ci32_dep(m_buffer + sizeof(int32));
				}
			}
			else
#endif
			{
				for (int i=1;i<len;i++)
				{
					Point& p = pt[i];
					FillBufferCurrent(sizeof(int32)*2);
					p.X = ci32_le(m_buffer);
					p.Y = ci32_le(m_buffer + sizeof(int32));
				}
			}
		}
		void TaggedDataReader::_GetEntryPointF(const Entry* ent, PointF* pt, int len)
		{
			if (len>0) _GetEntryPointF(ent, pt[0]);
#if !LITTLE_ENDIAN
			if (m_endianDependent)
			{
				for (int i=1;i<len;i++)
				{
					PointF& p = pt[i];
					FillBufferCurrent(sizeof(float)*2);
					p.X = cr32_dep(m_buffer);
					p.Y = cr32_dep(m_buffer + sizeof(float));
				}
			}
			else
#endif
			{
				for (int i=1;i<len;i++)
				{
					PointF& p = pt[i];
					FillBufferCurrent(sizeof(float)*2);
					p.X = cr32_le(m_buffer);
					p.Y = cr32_le(m_buffer + sizeof(float));
				}
			}
		}
		void TaggedDataReader::_GetEntryMathSize(const Entry* ent, Apoc3D::Math::Size* sz, int len)
		{
			if (len>0) _GetEntryMathSize(ent, sz[0]);
#if !LITTLE_ENDIAN
			if (m_endianDependent)
			{
				for (int i=1;i<len;i++)
				{
					Apoc3D::Math::Size& s = sz[i];
					FillBufferCurrent(sizeof(float)*2);
					s.Width = ci32_dep(m_buffer);
					s.Height = ci32_dep(m_buffer + sizeof(int32));
				}
			}
			else
#endif
			{
				for (int i=1;i<len;i++)
				{
					Apoc3D::Math::Size& s = sz[i];
					FillBufferCurrent(sizeof(float)*2);
					s.Width = ci32_le(m_buffer);
					s.Height = ci32_le(m_buffer + sizeof(int32));
				}
			}
		}
		void TaggedDataReader::_GetEntryBoundingBox(const Entry* ent, BoundingBox* bb, int len)
		{
			if (len>0) _GetEntryBoundingBox(ent, bb[0]);
#if !LITTLE_ENDIAN
			if (m_endianDependent)
			{
				for (int i=1;i<len;i++)
				{
					BoundingBox& b = bb[i];
					FillBufferCurrent(sizeof(float)*3);
					v3x(b.Minimum) = cr32_dep(m_buffer);
					v3y(b.Minimum) = cr32_dep(m_buffer + sizeof(float));
					v3z(b.Minimum) = cr32_dep(m_buffer + sizeof(float) * 2);

					FillBufferCurrent(sizeof(float)*3);
					v3x(b.Maximum) = cr32_dep(m_buffer);
					v3y(b.Maximum) = cr32_dep(m_buffer + sizeof(float));
					v3z(b.Maximum) = cr32_dep(m_buffer + sizeof(float) * 2);
				}
			}
			else
#endif
			{
				for (int i=1;i<len;i++)
				{
					BoundingBox& b = bb[i];
					FillBufferCurrent(sizeof(float)*3);
					v3x(b.Minimum) = cr32_le(m_buffer);
					v3y(b.Minimum) = cr32_le(m_buffer + sizeof(float));
					v3z(b.Minimum) = cr32_le(m_buffer + sizeof(float) * 2);

					FillBufferCurrent(sizeof(float)*3);
					v3x(b.Maximum) = cr32_le(m_buffer);
					v3y(b.Maximum) = cr32_le(m_buffer + sizeof(float));
					v3z(b.Maximum) = cr32_le(m_buffer + sizeof(float) * 2);
				}
			}
		}
		void TaggedDataReader::_GetEntryBoundingSphere(const Entry* ent, BoundingSphere* bs, int len)
		{
			if (len>0) _GetEntryBoundingSphere(ent, bs[0]);
#if !LITTLE_ENDIAN
			if (m_endianDependent)
			{
				for (int i=1;i<len;i++)
				{
					BoundingSphere& b = bs[i];
					FillBufferCurrent(sizeof(float)*4);
					v3x(b.Center) = cr32_dep(m_buffer);
					v3y(b.Center) = cr32_dep(m_buffer + sizeof(float));
					v3z(b.Center) = cr32_dep(m_buffer + sizeof(float) * 2);
					b.Radius = cr32_dep(m_buffer + sizeof(float) * 3);
				}
			}
			else
#endif
			{
				for (int i=1;i<len;i++)
				{
					BoundingSphere& b = bs[i];
					FillBufferCurrent(sizeof(float)*4);
					v3x(b.Center) = cr32_le(m_buffer);
					v3y(b.Center) = cr32_le(m_buffer + sizeof(float));
					v3z(b.Center) = cr32_le(m_buffer + sizeof(float) * 2);
					b.Radius = cr32_le(m_buffer + sizeof(float) * 3);
				}
			}
		}
		void TaggedDataReader::_GetEntryRay(const Entry* ent, Ray* r, int len)
		{
			if (len>0) _GetEntryRay(ent, r[0]);
#if !LITTLE_ENDIAN
			if (m_endianDependent)
			{
				for (int i=1;i<len;i++)
				{
					Ray& cr = r[i];
					FillBufferCurrent( sizeof(float)*6);
					v3x(cr.Position) = cr32_dep(m_buffer);
					v3y(cr.Position) = cr32_dep(m_buffer + sizeof(float));
					v3z(cr.Position) = cr32_dep(m_buffer + sizeof(float) * 2);

					v3x(cr.Direction) = cr32_dep(m_buffer + sizeof(float) * 3);
					v3y(cr.Direction) = cr32_dep(m_buffer + sizeof(float) * 4);
					v3z(cr.Direction) = cr32_dep(m_buffer + sizeof(float) * 5);
				}
			}
			else
#endif
			{
				for (int i=1;i<len;i++)
				{
					Ray& cr = r[i];
					FillBufferCurrent( sizeof(float)*3);
					v3x(cr.Position) = cr32_le(m_buffer);
					v3y(cr.Position) = cr32_le(m_buffer + sizeof(float));
					v3z(cr.Position) = cr32_le(m_buffer + sizeof(float) * 2);

					v3x(cr.Direction) = cr32_le(m_buffer + sizeof(float) * 3);
					v3y(cr.Direction) = cr32_le(m_buffer + sizeof(float) * 4);
					v3z(cr.Direction) = cr32_le(m_buffer + sizeof(float) * 5);
				}
			}
		}
		void TaggedDataReader::_GetEntryViewport(const Entry* ent, Viewport* vp, int len)
		{
			if (len>0) _GetEntryViewport(ent, vp[0]);
#if !LITTLE_ENDIAN
			if (m_endianDependent)
			{
				for (int i=1;i<len;i++)
				{
					Viewport& v = vp[i];
					FillBufferCurrent(sizeof(int32)*4);
					v.X = ci32_dep(m_buffer);
					v.Y = ci32_dep(m_buffer + sizeof(int32));
					v.Width = ci32_dep(m_buffer + sizeof(int32)*2);
					v.Height = ci32_dep(m_buffer + sizeof(int32)*3);

					FillBufferCurrent(sizeof(float)*2);
					v.MinZ = cr32_dep(m_buffer);
					v.MaxZ = cr32_deps(m_buffer + sizeof(float));
				}
			}
			else
#endif
			{
				for (int i=1;i<len;i++)
				{
					Viewport& v = vp[i];
					FillBufferCurrent(sizeof(int32)*4);
					v.X = ci32_le(m_buffer);
					v.Y = ci32_le(m_buffer + sizeof(int32));
					v.Width = ci32_le(m_buffer + sizeof(int32)*2);
					v.Height = ci32_le(m_buffer + sizeof(int32)*3);

					FillBufferCurrent(sizeof(float)*2);
					v.MinZ = cr32_le(m_buffer);
					v.MaxZ = cr32_le(m_buffer + sizeof(float));
				}
			}
		}

		//////////////////////////////////////////////////////////////////////////
		//////////////////////////////////////////////////////////////////////////
		//////////////////////////////////////////////////////////////////////////

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

		void TaggedDataWriter::Entry::ResetWritePosition() const { Buffer->setPosition(0); }

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
		ConfigurationSection* TaggedDataWriter::MakeDigest(const String& name) const
		{
			ConfigurationSection* sect = new ConfigurationSection(name);
			for (SectionTable::Enumerator e = m_positions.GetEnumerator(); e.MoveNext();)
			{
				Entry* ent = e.getCurrentValue();
				MemoryOutStream* memBlock = ent->Buffer;

				String text;

				if (memBlock->getLength()>8)
				{
					text = StringUtils::ToString(memBlock->getLength()) + L" bytes";
				}
				else
				{
					const byte* data = reinterpret_cast<const byte*>(memBlock->getPointer());

					for (int64 i=0;i<memBlock->getLength();i++)
					{
						std::wostringstream stream;
						stream.width(2);
						stream.fill('0');
						stream.imbue(std::locale::classic());
						stream.setf ( std::ios::hex, std::ios::basefield );       // set hex as the basefield
						stream << data[i];

						text.append(stream.str());
					}
				}

				sect->AddStringValue(*e.getCurrentKey(), text);
			}
			return sect;
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

		bool TaggedDataWriter::Contains(const String& name) const { return !!FindEntry(name); }

		BinaryWriter* TaggedDataWriter::GetData(const String& name)
		{
			const Entry* ent = FindEntry(name);
			return new BinaryWriter(new VirtualStream(ent->Buffer, 0));
		}

#define TAGW_NEW_ENTRY(name, value, setvalue) Entry ent = Entry(name); m_positions.Add(name, ent); setvalue(ent, value);
#define TAGW_NEW_ENTRY_ARR(name, value, count, setvalue) Entry ent = Entry(name); m_positions.Add(name, ent); setvalue(ent, value, count);

		void TaggedDataWriter::AddEntryInt64(const String& name, int64 value)	{ TAGW_NEW_ENTRY(name, value, _SetEntryDataInt64);  }
		void TaggedDataWriter::AddEntryUInt64(const String& name, uint64 value)	{ TAGW_NEW_ENTRY(name, value, _SetEntryDataUInt64); }
		void TaggedDataWriter::AddEntryInt32(const String& name, int32 value)	{ TAGW_NEW_ENTRY(name, value, _SetEntryDataInt32);  }
		void TaggedDataWriter::AddEntryUInt32(const String& name, uint32 value)	{ TAGW_NEW_ENTRY(name, value, _SetEntryDataUInt32); }
		void TaggedDataWriter::AddEntryInt16(const String& name, int16 value)	{ TAGW_NEW_ENTRY(name, value, _SetEntryDataInt16);  }
		void TaggedDataWriter::AddEntryUInt16(const String& name, uint16 value)	{ TAGW_NEW_ENTRY(name, value, _SetEntryDataUInt16); }
		void TaggedDataWriter::AddEntrySingle(const String& name, float value)	{ TAGW_NEW_ENTRY(name, value, _SetEntryDataSingle); }
		void TaggedDataWriter::AddEntryDouble(const String& name, double value)	{ TAGW_NEW_ENTRY(name, value, _SetEntryDataDouble); }
		void TaggedDataWriter::AddEntryBool(const String& name, bool value)		{ TAGW_NEW_ENTRY(name, value, _SetEntryDataBool); }

		void TaggedDataWriter::AddEntryVector2(const String& name, const Vector2& value)	{ TAGW_NEW_ENTRY(name, value, _SetEntryDataVector2); }
		void TaggedDataWriter::AddEntryVector3(const String& name, const Vector3& value)	{ TAGW_NEW_ENTRY(name, value, _SetEntryDataVector3); }
		void TaggedDataWriter::AddEntryVector4(const String& name, const Vector4& value)	{ TAGW_NEW_ENTRY(name, value, _SetEntryDataVector4); }
		void TaggedDataWriter::AddEntryMatrix(const String& name, const Matrix& value)		{ TAGW_NEW_ENTRY(name, value, _SetEntryDataMatrix); }
		void TaggedDataWriter::AddEntryColor4(const String& name, const Color4& value)		{ TAGW_NEW_ENTRY(name, value, _SetEntryDataColor4); }
		void TaggedDataWriter::AddEntryString(const String& name, const String& value)		{ TAGW_NEW_ENTRY(name, value, _SetEntryDataString); }
		void TaggedDataWriter::AddEntryPlane(const String& name, const Plane& value)		{ TAGW_NEW_ENTRY(name, value, _SetEntryDataPlane); }
		void TaggedDataWriter::AddEntryQuaternion(const String& name, const Quaternion& value)		{ TAGW_NEW_ENTRY(name, value, _SetEntryDataQuaternion); }
		void TaggedDataWriter::AddEntryRectangle(const String& name,  const Rectangle& value)		{ TAGW_NEW_ENTRY(name, value, _SetEntryDataRectangle); }
		void TaggedDataWriter::AddEntryRectangleF(const String& name, const RectangleF& value)		{ TAGW_NEW_ENTRY(name, value, _SetEntryDataRectangleF); }
		void TaggedDataWriter::AddEntryPoint(const String& name, const Point& value)					{ TAGW_NEW_ENTRY(name, value, _SetEntryDataPoint); }
		void TaggedDataWriter::AddEntryPointF(const String& name, const PointF& value)					{ TAGW_NEW_ENTRY(name, value, _SetEntryDataPointF); }
		void TaggedDataWriter::AddEntryMathSize(const String& name, const Apoc3D::Math::Size& value)	{ TAGW_NEW_ENTRY(name, value, _SetEntryDataMathSize); }
		void TaggedDataWriter::AddEntryBoundingBox(const String& name, const BoundingBox& value)		{ TAGW_NEW_ENTRY(name, value, _SetEntryDataBoundingBox); }
		void TaggedDataWriter::AddEntryBoundingSphere(const String& name, const BoundingSphere& value)	{ TAGW_NEW_ENTRY(name, value, _SetEntryDataBoundingSphere); }
		void TaggedDataWriter::AddEntryRay(const String& name, const Ray& value)						{ TAGW_NEW_ENTRY(name, value, _SetEntryDataRay); }
		void TaggedDataWriter::AddEntryViewport(const String& name, const Viewport& value)				{ TAGW_NEW_ENTRY(name, value, _SetEntryDataViewport); }


		void TaggedDataWriter::AddEntryInt64(const String& name, const int64* value, int32 count)	{ TAGW_NEW_ENTRY_ARR(name, value, count, _SetEntryDataInt64); }
		void TaggedDataWriter::AddEntryUInt64(const String& name, const uint64* value, int32 count){ TAGW_NEW_ENTRY_ARR(name, value, count, _SetEntryDataUInt64); }
		void TaggedDataWriter::AddEntryInt32(const String& name, const int32* value, int32 count)	{ TAGW_NEW_ENTRY_ARR(name, value, count, _SetEntryDataInt32); }
		void TaggedDataWriter::AddEntryUInt32(const String& name, const uint32* value, int32 count){ TAGW_NEW_ENTRY_ARR(name, value, count, _SetEntryDataUInt32); }
		void TaggedDataWriter::AddEntryInt16(const String& name, const int16* value, int32 count)	{ TAGW_NEW_ENTRY_ARR(name, value, count, _SetEntryDataInt16); }
		void TaggedDataWriter::AddEntryUInt16(const String& name, const uint16* value, int32 count){ TAGW_NEW_ENTRY_ARR(name, value, count, _SetEntryDataUInt16); }
		void TaggedDataWriter::AddEntrySingle(const String& name, const float* value, int32 count)	{ TAGW_NEW_ENTRY_ARR(name, value, count, _SetEntryDataSingle); }
		void TaggedDataWriter::AddEntryDouble(const String& name, const double* value, int32 count){ TAGW_NEW_ENTRY_ARR(name, value, count, _SetEntryDataDouble); }
		void TaggedDataWriter::AddEntryBool(const String& name, const bool* value, int32 count)	{ TAGW_NEW_ENTRY_ARR(name, value, count, _SetEntryDataBool); }

		void TaggedDataWriter::AddEntryVector2(const String& name, const Vector2* vec, int32 count)	{ TAGW_NEW_ENTRY_ARR(name, vec, count, _SetEntryDataVector2); }
		void TaggedDataWriter::AddEntryVector3(const String& name, const Vector3* vec, int32 count)	{ TAGW_NEW_ENTRY_ARR(name, vec, count, _SetEntryDataVector3); }
		void TaggedDataWriter::AddEntryVector4(const String& name, const Vector4* vec, int32 count)	{ TAGW_NEW_ENTRY_ARR(name, vec, count, _SetEntryDataVector4); }
		void TaggedDataWriter::AddEntryMatrix(const String& name, const Matrix* mat, int32 count)		{ TAGW_NEW_ENTRY_ARR(name, mat, count, _SetEntryDataMatrix); }
		void TaggedDataWriter::AddEntryColor4(const String& name, const Color4* clr, int32 count)		{ TAGW_NEW_ENTRY_ARR(name, clr, count, _SetEntryDataColor4); }
		void TaggedDataWriter::AddEntryString(const String& name, const String* str, int32 count)		{ TAGW_NEW_ENTRY_ARR(name, str, count, _SetEntryDataString); }
		void TaggedDataWriter::AddEntryPlane(const String& name, const Plane* plane, int32 count)		{ TAGW_NEW_ENTRY_ARR(name, plane, count, _SetEntryDataPlane); }
		void TaggedDataWriter::AddEntryQuaternion(const String& name, const Quaternion* quat, int32 count)		{ TAGW_NEW_ENTRY_ARR(name, quat, count, _SetEntryDataQuaternion); }
		void TaggedDataWriter::AddEntryRectangle(const String& name, const Rectangle* rect, int32 count)			{ TAGW_NEW_ENTRY_ARR(name, rect, count, _SetEntryDataRectangle); }
		void TaggedDataWriter::AddEntryRectangleF(const String& name, const RectangleF* rect, int32 count)			{ TAGW_NEW_ENTRY_ARR(name, rect, count, _SetEntryDataRectangleF); }
		void TaggedDataWriter::AddEntryPoint(const String& name, const Point* pt, int32 count)						{ TAGW_NEW_ENTRY_ARR(name, pt, count, _SetEntryDataPoint); }
		void TaggedDataWriter::AddEntryPointF(const String& name, const PointF* pt, int32 count)					{ TAGW_NEW_ENTRY_ARR(name, pt, count, _SetEntryDataPointF); }
		void TaggedDataWriter::AddEntryMathSize(const String& name, const Apoc3D::Math::Size* sz, int32 count)		{ TAGW_NEW_ENTRY_ARR(name, sz, count, _SetEntryDataMathSize); }
		void TaggedDataWriter::AddEntryBoundingBox(const String& name, const BoundingBox* bb, int32 count)			{ TAGW_NEW_ENTRY_ARR(name, bb, count, _SetEntryDataBoundingBox); }
		void TaggedDataWriter::AddEntryBoundingSphere(const String& name, const BoundingSphere* bs, int32 count)	{ TAGW_NEW_ENTRY_ARR(name, bs, count, _SetEntryDataBoundingSphere); }
		void TaggedDataWriter::AddEntryRay(const String& name, const Ray* r, int32 count)							{ TAGW_NEW_ENTRY_ARR(name, r, count, _SetEntryDataRay); }
		void TaggedDataWriter::AddEntryViewport(const String& name, const Viewport* vp, int32 count)				{ TAGW_NEW_ENTRY_ARR(name, vp, count, _SetEntryDataViewport); }

#define TAGW_SETDATA(name, value, setter) const Entry* ent = FindEntry(name); assert(ent); ent->ResetWritePosition(); setter(*ent, value);
#define TAGW_SETDATA_ARR(name, value, count, setter) const Entry* ent = FindEntry(name); assert(ent); ent->ResetWritePosition(); setter(*ent, value, count);

		void TaggedDataWriter::SetData(const String& name, int64 value)		{ TAGW_SETDATA(name, value, _SetEntryDataInt64); }
		void TaggedDataWriter::SetData(const String& name, uint64 value)	{ TAGW_SETDATA(name, value, _SetEntryDataUInt64); }
		void TaggedDataWriter::SetData(const String& name, int32 value)		{ TAGW_SETDATA(name, value, _SetEntryDataInt32); }
		void TaggedDataWriter::SetData(const String& name, uint32 value)	{ TAGW_SETDATA(name, value, _SetEntryDataUInt32); }
		void TaggedDataWriter::SetData(const String& name, int16 value)		{ TAGW_SETDATA(name, value, _SetEntryDataInt16); }
		void TaggedDataWriter::SetData(const String& name, uint16 value)	{ TAGW_SETDATA(name, value, _SetEntryDataUInt16); }
		void TaggedDataWriter::SetData(const String& name, float value)		{ TAGW_SETDATA(name, value, _SetEntryDataSingle); }
		void TaggedDataWriter::SetData(const String& name, double value)	{ TAGW_SETDATA(name, value, _SetEntryDataDouble); }
		void TaggedDataWriter::SetData(const String& name, bool value)		{ TAGW_SETDATA(name, value, _SetEntryDataBool); }
		
		void TaggedDataWriter::SetDataInt64(const String& name, const int64* value, int32 count)	{ TAGW_SETDATA_ARR(name, value, count, _SetEntryDataInt64); }
		void TaggedDataWriter::SetDataUInt64(const String& name, const uint64* value, int32 count)	{ TAGW_SETDATA_ARR(name, value, count, _SetEntryDataUInt64); }
		void TaggedDataWriter::SetDataInt32(const String& name, const int32* value, int32 count)	{ TAGW_SETDATA_ARR(name, value, count, _SetEntryDataInt32); }
		void TaggedDataWriter::SetDataUInt32(const String& name, const uint32* value, int32 count)	{ TAGW_SETDATA_ARR(name, value, count, _SetEntryDataUInt32); }
		void TaggedDataWriter::SetDataInt16(const String& name, const int16* value, int32 count)	{ TAGW_SETDATA_ARR(name, value, count, _SetEntryDataInt16); }
		void TaggedDataWriter::SetDataUInt16(const String& name, const uint16* value, int32 count)	{ TAGW_SETDATA_ARR(name, value, count, _SetEntryDataUInt16); }
		void TaggedDataWriter::SetDataSingle(const String& name, const float* value, int32 count)	{ TAGW_SETDATA_ARR(name, value, count, _SetEntryDataSingle); }
		void TaggedDataWriter::SetDataDouble(const String& name, const double* value, int32 count)	{ TAGW_SETDATA_ARR(name, value, count, _SetEntryDataDouble); }
		void TaggedDataWriter::SetDataBool(const String& name, const bool* value, int32 count)		{ TAGW_SETDATA_ARR(name, value, count, _SetEntryDataBool); }

		void TaggedDataWriter::SetDataVector2(const String& name, const Vector2& value)		{ TAGW_SETDATA(name, value, _SetEntryDataVector2); }
		void TaggedDataWriter::SetDataVector3(const String& name, const Vector3& value)		{ TAGW_SETDATA(name, value, _SetEntryDataVector3); }
		void TaggedDataWriter::SetDataVector4(const String& name, const Vector4& value)		{ TAGW_SETDATA(name, value, _SetEntryDataVector4); }
		void TaggedDataWriter::SetDataMatrix(const String& name, const Matrix& value)		{ TAGW_SETDATA(name, value, _SetEntryDataMatrix); }
		void TaggedDataWriter::SetDataColor4(const String& name, const Color4& value)		{ TAGW_SETDATA(name, value, _SetEntryDataColor4); }
		void TaggedDataWriter::SetDataString(const String& name, const String& value)		{ TAGW_SETDATA(name, value, _SetEntryDataString); }
		void TaggedDataWriter::SetDataPlane(const String& name, const Plane& value)			{ TAGW_SETDATA(name, value, _SetEntryDataPlane); }
		void TaggedDataWriter::SetDataQuaternion(const String& name, const Quaternion& value)	{ TAGW_SETDATA(name, value, _SetEntryDataQuaternion); }
		void TaggedDataWriter::SetDataRectangle(const String& name,  const Rectangle& value)	{ TAGW_SETDATA(name, value, _SetEntryDataRectangle); }
		void TaggedDataWriter::SetDataRectangleF(const String& name, const RectangleF& value)	{ TAGW_SETDATA(name, value, _SetEntryDataRectangleF); }
		void TaggedDataWriter::SetDataPoint(const String& name, const Point& value)				{ TAGW_SETDATA(name, value, _SetEntryDataPoint); }
		void TaggedDataWriter::SetDataPointF(const String& name, const PointF& value)			{ TAGW_SETDATA(name, value, _SetEntryDataPointF); }
		void TaggedDataWriter::SetDataMathSize(const String& name, const Apoc3D::Math::Size& value)		{ TAGW_SETDATA(name, value, _SetEntryDataMathSize); }
		void TaggedDataWriter::SetDataBoundingBox(const String& name, const BoundingBox& value)			{ TAGW_SETDATA(name, value, _SetEntryDataBoundingBox); }
		void TaggedDataWriter::SetDataBoundingSphere(const String& name, const BoundingSphere& value)	{ TAGW_SETDATA(name, value, _SetEntryDataBoundingSphere); }
		void TaggedDataWriter::SetDataRay(const String& name, const Ray& value)							{ TAGW_SETDATA(name, value, _SetEntryDataRay); }		
		void TaggedDataWriter::SetDataViewport(const String& name, const Viewport& value)				{ TAGW_SETDATA(name, value, _SetEntryDataViewport); }

		void TaggedDataWriter::SetDataVector2(const String& name, const Vector2* vec, int32 count)		{ TAGW_SETDATA_ARR(name, vec, count, _SetEntryDataVector2); }
		void TaggedDataWriter::SetDataVector3(const String& name, const Vector3* vec, int32 count)		{ TAGW_SETDATA_ARR(name, vec, count, _SetEntryDataVector3); }
		void TaggedDataWriter::SetDataVector4(const String& name, const Vector4* vec, int32 count)		{ TAGW_SETDATA_ARR(name, vec, count, _SetEntryDataVector4); }
		void TaggedDataWriter::SetDataMatrix(const String& name, const Matrix* mat, int32 count)		{ TAGW_SETDATA_ARR(name, mat, count, _SetEntryDataMatrix); }
		void TaggedDataWriter::SetDataColor4(const String& name, const Color4* clr, int32 count)		{ TAGW_SETDATA_ARR(name, clr, count, _SetEntryDataColor4); }
		void TaggedDataWriter::SetDataString(const String& name, const String* str, int32 count)		{ TAGW_SETDATA_ARR(name, str, count, _SetEntryDataString); }
		void TaggedDataWriter::SetDataPlane(const String& name, const Plane* plane, int32 count)		{ TAGW_SETDATA_ARR(name, plane, count, _SetEntryDataPlane); }
		void TaggedDataWriter::SetDataQuaternion(const String& name, const Quaternion* quad, int32 count)	{ TAGW_SETDATA_ARR(name, quad, count, _SetEntryDataQuaternion); }
		void TaggedDataWriter::SetDataRectangle(const String& name, const Rectangle* rect, int32 count)		{ TAGW_SETDATA_ARR(name, rect, count, _SetEntryDataRectangle); }
		void TaggedDataWriter::SetDataRectangleF(const String& name, const RectangleF* rect, int32 count)		{ TAGW_SETDATA_ARR(name, rect, count, _SetEntryDataRectangleF); }
		void TaggedDataWriter::SetDataPoint(const String& name, const Point* pt, int32 count)					{ TAGW_SETDATA_ARR(name, pt, count, _SetEntryDataPoint); }
		void TaggedDataWriter::SetDataPointF(const String& name, const PointF* pt, int32 count)				{ TAGW_SETDATA_ARR(name, pt, count, _SetEntryDataPointF); }
		void TaggedDataWriter::SetDataMathSize(const String& name, const Apoc3D::Math::Size* sz, int32 count)	{ TAGW_SETDATA_ARR(name, sz, count, _SetEntryDataMathSize); }
		void TaggedDataWriter::SetDataBoundingBox(const String& name, const BoundingBox* bb, int32 count)		{ TAGW_SETDATA_ARR(name, bb, count, _SetEntryDataBoundingBox); }
		void TaggedDataWriter::SetDataBoundingSphere(const String& name, const BoundingSphere* bs, int32 count)	{ TAGW_SETDATA_ARR(name, bs, count, _SetEntryDataBoundingSphere); }
		void TaggedDataWriter::SetDataRay(const String& name, const Ray* r, int32 count)							{ TAGW_SETDATA_ARR(name, r, count, _SetEntryDataRay); }
		void TaggedDataWriter::SetDataViewport(const String& name, const Viewport* vp, int32 count)				{ TAGW_SETDATA_ARR(name, vp, count, _SetEntryDataViewport); }

		void TaggedDataWriter::_SetEntryDataInt64(const Entry& ent, int64 value)
		{
#if !LITTLE_ENDIAN
			if (m_endianDependent)
				i64tomb_dep(value, m_buffer);
			else
#endif
				i64tomb_le(value, m_buffer);

			ent.Buffer->Write(m_buffer, sizeof(value));
		}
		void TaggedDataWriter::_SetEntryDataUInt64(const Entry& ent, uint64 value)
		{
#if !LITTLE_ENDIAN
			if (m_endianDependent)
				ui64tomb_dep(value, m_buffer);
			else
#endif
				ui64tomb_le(value, m_buffer);

			ent.Buffer->Write(m_buffer, sizeof(value));
		}
		void TaggedDataWriter::_SetEntryDataInt32(const Entry& ent, int32 value)
		{
#if !LITTLE_ENDIAN
			if (m_endianDependent)
				i32tomb_dep(value, m_buffer);
			else
#endif
				i32tomb_le(value, m_buffer);

			ent.Buffer->Write(m_buffer, sizeof(value));
		}
		void TaggedDataWriter::_SetEntryDataUInt32(const Entry& ent, uint32 value)
		{
#if !LITTLE_ENDIAN
			if (m_endianDependent)
				ui32tomb_dep(value, m_buffer);
			else
#endif
				ui32tomb_le(value, m_buffer);

			ent.Buffer->Write(m_buffer, sizeof(value));
		}
		void TaggedDataWriter::_SetEntryDataInt16(const Entry& ent, int16 value)
		{
#if !LITTLE_ENDIAN
			if (m_endianDependent)
				i16tomb_dep(value, m_buffer);
			else
#endif
				i16tomb_le(value, m_buffer);

			ent.Buffer->Write(m_buffer, sizeof(value));
		}
		void TaggedDataWriter::_SetEntryDataUInt16(const Entry& ent, uint16 value)
		{
#if !LITTLE_ENDIAN
			if (m_endianDependent)
				ui16tomb_dep(value, m_buffer);
			else
#endif
				ui16tomb_le(value, m_buffer);

			ent.Buffer->Write(m_buffer, sizeof(value));
		}
		void TaggedDataWriter::_SetEntryDataSingle(const Entry& ent, float value)
		{
#if !LITTLE_ENDIAN
			if (m_endianDependent)
				r32tomb_dep(value, m_buffer);
			else
#endif
				r32tomb_le(value, m_buffer);

			ent.Buffer->Write(m_buffer, sizeof(value));
		}
		void TaggedDataWriter::_SetEntryDataDouble(const Entry& ent, double value)
		{
#if !LITTLE_ENDIAN
			if (m_endianDependent)
				r64tomb_dep(value, m_buffer);
			else
#endif
				r64tomb_le(value, m_buffer);

			ent.Buffer->Write(m_buffer, sizeof(value));
		}
		void TaggedDataWriter::_SetEntryDataBool(const Entry& ent, bool value)
		{
			m_buffer[0] = value ? 1 : 0;

			ent.Buffer->Write(m_buffer, sizeof(value));
		}


		void TaggedDataWriter::_SetEntryDataInt64(const Entry& ent, const int64* value, int32 count)		{ for (int32 i=0;i<count;i++) _SetEntryDataInt64(ent, value[i]); }
		void TaggedDataWriter::_SetEntryDataUInt64(const Entry& ent, const uint64* value, int32 count)		{ for (int32 i=0;i<count;i++) _SetEntryDataUInt64(ent, value[i]); }
		void TaggedDataWriter::_SetEntryDataInt32(const Entry& ent, const int32* value, int32 count)		{ for (int32 i=0;i<count;i++) _SetEntryDataInt32(ent, value[i]); }
		void TaggedDataWriter::_SetEntryDataUInt32(const Entry& ent, const uint32* value, int32 count)		{ for (int32 i=0;i<count;i++) _SetEntryDataUInt32(ent, value[i]); }
		void TaggedDataWriter::_SetEntryDataInt16(const Entry& ent, const int16* value, int32 count)		{ for (int32 i=0;i<count;i++) _SetEntryDataInt16(ent, value[i]); }
		void TaggedDataWriter::_SetEntryDataUInt16(const Entry& ent, const uint16* value, int32 count)		{ for (int32 i=0;i<count;i++) _SetEntryDataUInt16(ent, value[i]); }
		void TaggedDataWriter::_SetEntryDataSingle(const Entry& ent, const float* value, int32 count)		{ for (int32 i=0;i<count;i++) _SetEntryDataSingle(ent, value[i]); }
		void TaggedDataWriter::_SetEntryDataDouble(const Entry& ent, const double* value, int32 count)		{ for (int32 i=0;i<count;i++) _SetEntryDataDouble(ent, value[i]); }
		void TaggedDataWriter::_SetEntryDataBool(const Entry& ent, const bool* value, int32 count)			{ for (int32 i=0;i<count;i++) _SetEntryDataBool(ent, value[i]); }



		void TaggedDataWriter::_SetEntryDataVector2(const Entry& ent, const Vector2& vec)
		{
#if !LITTLE_ENDIAN
			if (m_endianDependent)
			{
				r32tomb_dep(v2x(vec), m_buffer);
				r32tomb_dep(v2y(vec), m_buffer + sizeof(float));
			}
			else
#endif
			{
				r32tomb_le(v2x(vec), m_buffer);
				r32tomb_le(v2y(vec), m_buffer + sizeof(float));
			}
			ent.Buffer->Write(m_buffer, sizeof(float) * 2);
		}
		void TaggedDataWriter::_SetEntryDataVector3(const Entry& ent, const Vector3& vec)
		{
#if !LITTLE_ENDIAN
			if (m_endianDependent)
			{
				r32tomb_dep(v3x(vec), m_buffer);
				r32tomb_dep(v3y(vec), m_buffer + sizeof(float));
				r32tomb_dep(v3z(vec), m_buffer + sizeof(float) * 2);
			}
			else
#endif
			{
				r32tomb_le(v3x(vec), m_buffer);
				r32tomb_le(v3y(vec), m_buffer + sizeof(float));
				r32tomb_le(v3z(vec), m_buffer + sizeof(float) * 2);
			}
			ent.Buffer->Write(m_buffer, sizeof(float) * 3);
		}
		void TaggedDataWriter::_SetEntryDataVector4(const Entry& ent, const Vector4& vec)
		{
#if !LITTLE_ENDIAN
			if (m_endianDependent)
			{
				r32tomb_dep(v4x(vec), m_buffer);
				r32tomb_dep(v4y(vec), m_buffer + sizeof(float));
				r32tomb_dep(v4z(vec), m_buffer + sizeof(float) * 2);
				r32tomb_dep(v4w(vec), m_buffer + sizeof(float) * 3);
			}
			else
#endif
			{
				r32tomb_le(v4x(vec), m_buffer);
				r32tomb_le(v4y(vec), m_buffer + sizeof(float));
				r32tomb_le(v4z(vec), m_buffer + sizeof(float) * 2);
				r32tomb_le(v4w(vec), m_buffer + sizeof(float) * 3);
			}
			ent.Buffer->Write(m_buffer, sizeof(float) * 4);
		}
		void TaggedDataWriter::_SetEntryDataMatrix(const Entry& ent, const Matrix& mat)
		{
#if !LITTLE_ENDIAN
			if (m_endianDependent)
			{
				r32tomb_dep(mat.M11, m_buffer);
				r32tomb_dep(mat.M12, m_buffer + sizeof(float));
				r32tomb_dep(mat.M13, m_buffer + sizeof(float) * 2);
				r32tomb_dep(mat.M14, m_buffer + sizeof(float) * 3);
				r32tomb_dep(mat.M21, m_buffer + sizeof(float) * 4);
				r32tomb_dep(mat.M22, m_buffer + sizeof(float) * 5);
				r32tomb_dep(mat.M23, m_buffer + sizeof(float) * 6);
				r32tomb_dep(mat.M24, m_buffer + sizeof(float) * 7);
				ent.Buffer->Write(m_buffer, sizeof(float) * 8);

				r32tomb_dep(mat.M31, m_buffer);
				r32tomb_dep(mat.M32, m_buffer + sizeof(float));
				r32tomb_dep(mat.M33, m_buffer + sizeof(float) * 2);
				r32tomb_dep(mat.M34, m_buffer + sizeof(float) * 3);
				r32tomb_dep(mat.M41, m_buffer + sizeof(float) * 4);
				r32tomb_dep(mat.M42, m_buffer + sizeof(float) * 5);
				r32tomb_dep(mat.M43, m_buffer + sizeof(float) * 6);
				r32tomb_dep(mat.M44, m_buffer + sizeof(float) * 7);
				ent.Buffer->Write(m_buffer, sizeof(float) * 8);
			}
			else
#endif
			{
				r32tomb_le(mat.M11, m_buffer);
				r32tomb_le(mat.M12, m_buffer + sizeof(float));
				r32tomb_le(mat.M13, m_buffer + sizeof(float) * 2);
				r32tomb_le(mat.M14, m_buffer + sizeof(float) * 3);
				r32tomb_le(mat.M21, m_buffer + sizeof(float) * 4);
				r32tomb_le(mat.M22, m_buffer + sizeof(float) * 5);
				r32tomb_le(mat.M23, m_buffer + sizeof(float) * 6);
				r32tomb_le(mat.M24, m_buffer + sizeof(float) * 7);
				ent.Buffer->Write(m_buffer, sizeof(float) * 8);

				r32tomb_le(mat.M31, m_buffer);
				r32tomb_le(mat.M32, m_buffer + sizeof(float));
				r32tomb_le(mat.M33, m_buffer + sizeof(float) * 2);
				r32tomb_le(mat.M34, m_buffer + sizeof(float) * 3);
				r32tomb_le(mat.M41, m_buffer + sizeof(float) * 4);
				r32tomb_le(mat.M42, m_buffer + sizeof(float) * 5);
				r32tomb_le(mat.M43, m_buffer + sizeof(float) * 6);
				r32tomb_le(mat.M44, m_buffer + sizeof(float) * 7);
				ent.Buffer->Write(m_buffer, sizeof(float) * 8);
			}
		}
		void TaggedDataWriter::_SetEntryDataColor4(const Entry& ent, const Color4& clr)
		{
#if !LITTLE_ENDIAN
			if (m_endianDependent)
			{
				r32tomb_dep(clr.Red, m_buffer);
				r32tomb_dep(clr.Green, m_buffer + sizeof(float));
				r32tomb_dep(clr.Blue, m_buffer + sizeof(float) * 2);
				r32tomb_dep(clr.Alpha, m_buffer + sizeof(float) * 3);
			}
			else
#endif
			{
				r32tomb_le(clr.Red, m_buffer);
				r32tomb_le(clr.Green, m_buffer + sizeof(float));
				r32tomb_le(clr.Blue, m_buffer + sizeof(float) * 2);
				r32tomb_le(clr.Alpha, m_buffer + sizeof(float) * 3);
			}
			ent.Buffer->Write(m_buffer, sizeof(float) * 4);
		}
		void TaggedDataWriter::_SetEntryDataString(const Entry& ent, const String& str)
		{
			VirtualStream vStrm(ent.Buffer, 0);
			BinaryWriter bw(&vStrm);
			bw.SuspendStreamRelease();

			bw.Write(str);

			bw.Close();
		}
		void TaggedDataWriter::_SetEntryDataPlane(const Entry& ent, const Plane& plane)
		{
#if !LITTLE_ENDIAN
			if (m_endianDependent)
			{
				r32tomb_dep(plane.X, m_buffer);
				r32tomb_dep(plane.Y, m_buffer + sizeof(float));
				r32tomb_dep(plane.Z, m_buffer + sizeof(float) * 2);
				r32tomb_dep(plane.D, m_buffer + sizeof(float) * 3);
			}
			else
#endif
			{
				r32tomb_le(plane.X, m_buffer);
				r32tomb_le(plane.Y, m_buffer + sizeof(float));
				r32tomb_le(plane.Z, m_buffer + sizeof(float) * 2);
				r32tomb_le(plane.D, m_buffer + sizeof(float) * 3);
			}
			ent.Buffer->Write(m_buffer, sizeof(float) * 4);
		}
		void TaggedDataWriter::_SetEntryDataQuaternion(const Entry& ent, const Quaternion& quat)
		{
#if !LITTLE_ENDIAN
			if (m_endianDependent)
			{
				r32tomb_dep(quat.X, m_buffer);
				r32tomb_dep(quat.Y, m_buffer + sizeof(float));
				r32tomb_dep(quat.Z, m_buffer + sizeof(float) * 2);
				r32tomb_dep(quat.W, m_buffer + sizeof(float) * 3);
			}
			else
#endif
			{
				r32tomb_le(quat.X, m_buffer);
				r32tomb_le(quat.Y, m_buffer + sizeof(float));
				r32tomb_le(quat.Z, m_buffer + sizeof(float) * 2);
				r32tomb_le(quat.W, m_buffer + sizeof(float) * 3);
			}
			ent.Buffer->Write(m_buffer, sizeof(float) * 4);
		}
		void TaggedDataWriter::_SetEntryDataRectangle(const Entry& ent, const Rectangle& rect)
		{
#if !LITTLE_ENDIAN
			if (m_endianDependent)
			{
				i32tomb_dep(rect.X, m_buffer);
				i32tomb_dep(rect.Y, m_buffer + sizeof(int32));
				i32tomb_dep(rect.Width, m_buffer + sizeof(int32) * 2);
				i32tomb_dep(rect.Height, m_buffer + sizeof(int32) * 3);
			}
			else
#endif
			{
				i32tomb_le(rect.X, m_buffer);
				i32tomb_le(rect.Y, m_buffer + sizeof(int32));
				i32tomb_le(rect.Width, m_buffer + sizeof(int32) * 2);
				i32tomb_le(rect.Height, m_buffer + sizeof(int32) * 3);
			}
			ent.Buffer->Write(m_buffer, sizeof(int32) * 4);
		}
		void TaggedDataWriter::_SetEntryDataRectangleF(const Entry& ent, const RectangleF& rect)
		{
#if !LITTLE_ENDIAN
			if (m_endianDependent)
			{
				r32tomb_dep(rect.X, m_buffer);
				r32tomb_dep(rect.Y, m_buffer + sizeof(float));
				r32tomb_dep(rect.Width, m_buffer + sizeof(float) * 2);
				r32tomb_dep(rect.Height, m_buffer + sizeof(float) * 3);
			}
			else
#endif
			{
				r32tomb_le(rect.X, m_buffer);
				r32tomb_le(rect.Y, m_buffer + sizeof(float));
				r32tomb_le(rect.Width, m_buffer + sizeof(float) * 2);
				r32tomb_le(rect.Height, m_buffer + sizeof(float) * 3);
			}
			ent.Buffer->Write(m_buffer, sizeof(float) * 4);
		}
		void TaggedDataWriter::_SetEntryDataPoint(const Entry& ent, const Point& pt)
		{
#if !LITTLE_ENDIAN
			if (m_endianDependent)
			{
				i32tomb_dep(pt.X, m_buffer);
				i32tomb_dep(pt.Y, m_buffer + sizeof(int32));
			}
			else
#endif
			{
				i32tomb_le(pt.X, m_buffer);
				i32tomb_le(pt.Y, m_buffer + sizeof(int32));
			}
			ent.Buffer->Write(m_buffer, sizeof(int32) * 2);
		}
		void TaggedDataWriter::_SetEntryDataPointF(const Entry& ent, const PointF& pt)
		{
#if !LITTLE_ENDIAN
			if (m_endianDependent)
			{
				r32tomb_dep(pt.X, m_buffer);
				r32tomb_dep(pt.Y, m_buffer + sizeof(float));
			}
			else
#endif
			{
				r32tomb_le(pt.X, m_buffer);
				r32tomb_le(pt.Y, m_buffer + sizeof(float));
			}
			ent.Buffer->Write(m_buffer, sizeof(float) * 2);
		}
		void TaggedDataWriter::_SetEntryDataMathSize(const Entry& ent, const Apoc3D::Math::Size& sz)
		{
#if !LITTLE_ENDIAN
			if (m_endianDependent)
			{
				i32tomb_dep(sz.Width, m_buffer);
				i32tomb_dep(sz.Height, m_buffer + sizeof(int32));
			}
			else
#endif
			{
				i32tomb_le(sz.Width, m_buffer);
				i32tomb_le(sz.Height, m_buffer + sizeof(int32));
			}
			ent.Buffer->Write(m_buffer, sizeof(int32) * 2);
		}
		void TaggedDataWriter::_SetEntryDataBoundingBox(const Entry& ent, const BoundingBox& bb)
		{
#if !LITTLE_ENDIAN
			if (m_endianDependent)
			{
				r32tomb_dep(v3x(bb.Minimum), m_buffer);
				r32tomb_dep(v3y(bb.Minimum), m_buffer + sizeof(float));
				r32tomb_dep(v3z(bb.Minimum), m_buffer + sizeof(float) * 2);

				r32tomb_dep(v3x(bb.Maximum), m_buffer + sizeof(float) * 3);
				r32tomb_dep(v3y(bb.Maximum), m_buffer + sizeof(float) * 4);
				r32tomb_dep(v3z(bb.Maximum), m_buffer + sizeof(float) * 5);
				ent.Buffer->Write(m_buffer, sizeof(float) * 6);
			}
			else
#endif
			{
				r32tomb_le(v3x(bb.Minimum), m_buffer);
				r32tomb_le(v3y(bb.Minimum), m_buffer + sizeof(float));
				r32tomb_le(v3z(bb.Minimum), m_buffer + sizeof(float) * 2);

				r32tomb_le(v3x(bb.Maximum), m_buffer + sizeof(float) * 3);
				r32tomb_le(v3y(bb.Maximum), m_buffer + sizeof(float) * 4);
				r32tomb_le(v3z(bb.Maximum), m_buffer + sizeof(float) * 5);
				ent.Buffer->Write(m_buffer, sizeof(float) * 6);
			}
		}
		void TaggedDataWriter::_SetEntryDataBoundingSphere(const Entry& ent, const BoundingSphere& bs)
		{
#if !LITTLE_ENDIAN
			if (m_endianDependent)
			{
				r32tomb_dep(v3x(bs.Center), m_buffer);
				r32tomb_dep(v3y(bs.Center), m_buffer + sizeof(float));
				r32tomb_dep(v3z(bs.Center), m_buffer + sizeof(float) * 2);
				r32tomb_dep(bs.Radius, m_buffer + sizeof(float) * 3);
			}
			else
#endif
			{
				r32tomb_le(v3x(bs.Center), m_buffer);
				r32tomb_le(v3y(bs.Center), m_buffer + sizeof(float));
				r32tomb_le(v3z(bs.Center), m_buffer + sizeof(float) * 2);
				r32tomb_le(bs.Radius, m_buffer + sizeof(float) * 3);
			}
			ent.Buffer->Write(m_buffer, sizeof(float) * 4);
		}
		void TaggedDataWriter::_SetEntryDataRay(const Entry& ent, const Ray& r)
		{
#if !LITTLE_ENDIAN
			if (m_endianDependent)
			{
				r32tomb_dep(v3x(r.Position), m_buffer);
				r32tomb_dep(v3y(r.Position), m_buffer + sizeof(float));
				r32tomb_dep(v3z(r.Position), m_buffer + sizeof(float) * 2);
				
				r32tomb_dep(v3x(r.Direction), m_buffer + sizeof(float) * 3);
				r32tomb_dep(v3y(r.Direction), m_buffer + sizeof(float) * 4);
				r32tomb_dep(v3z(r.Direction), m_buffer + sizeof(float) * 5);
				ent.Buffer->Write(m_buffer, sizeof(float) * 6);
			}
			else
#endif
			{
				r32tomb_le(v3x(r.Position), m_buffer);
				r32tomb_le(v3y(r.Position), m_buffer + sizeof(float));
				r32tomb_le(v3z(r.Position), m_buffer + sizeof(float) * 2);
				
				r32tomb_le(v3x(r.Direction), m_buffer + sizeof(float) * 3);
				r32tomb_le(v3y(r.Direction), m_buffer + sizeof(float) * 4);
				r32tomb_le(v3z(r.Direction), m_buffer + sizeof(float) * 5);
				ent.Buffer->Write(m_buffer, sizeof(float) * 6);
			}
		}
		void TaggedDataWriter::_SetEntryDataViewport(const Entry& ent, const Viewport& vp)
		{
#if !LITTLE_ENDIAN
			if (m_endianDependent)
			{
				i32tomb_dep(vp.X, m_buffer);
				i32tomb_dep(vp.Y, m_buffer + sizeof(int32));
				i32tomb_dep(vp.Width, m_buffer + sizeof(int32) * 2);
				i32tomb_dep(vp.Height, m_buffer + sizeof(int32) * 3);
				ent.Buffer->Write(m_buffer, sizeof(int32) * 4);

				r32tomb_dep(vp.MinZ, m_buffer);
				r32tomb_dep(vp.MaxZ, m_buffer + sizeof(float));
				ent.Buffer->Write(m_buffer, sizeof(float) * 2);
			}
			else
#endif
			{
				i32tomb_le(vp.X, m_buffer);
				i32tomb_le(vp.Y, m_buffer + sizeof(int32));
				i32tomb_le(vp.Width, m_buffer + sizeof(int32) * 2);
				i32tomb_le(vp.Height, m_buffer + sizeof(int32) * 3);
				ent.Buffer->Write(m_buffer, sizeof(int32) * 4);

				r32tomb_le(vp.MinZ, m_buffer);
				r32tomb_le(vp.MaxZ, m_buffer + sizeof(float));
				ent.Buffer->Write(m_buffer, sizeof(float) * 2);
			}
		}


		void TaggedDataWriter::_SetEntryDataVector2(const Entry& ent, const Vector2* vec, int32 count)		{ for (int32 i=0;i<count;i++) _SetEntryDataVector2(ent, vec[i]); }
		void TaggedDataWriter::_SetEntryDataVector3(const Entry& ent, const Vector3* vec, int32 count)		{ for (int32 i=0;i<count;i++) _SetEntryDataVector3(ent, vec[i]); }
		void TaggedDataWriter::_SetEntryDataVector4(const Entry& ent, const Vector4* vec, int32 count)		{ for (int32 i=0;i<count;i++) _SetEntryDataVector4(ent, vec[i]); }
		void TaggedDataWriter::_SetEntryDataMatrix(const Entry& ent, const Matrix* mat, int32 count)		{ for (int32 i=0;i<count;i++) _SetEntryDataMatrix(ent, mat[i]); }
		void TaggedDataWriter::_SetEntryDataColor4(const Entry& ent, const Color4* clr, int32 count)		{ for (int32 i=0;i<count;i++) _SetEntryDataColor4(ent, clr[i]); }
		void TaggedDataWriter::_SetEntryDataString(const Entry& ent, const String* str, int32 count)
		{
			VirtualStream vStrm(ent.Buffer, 0);
			BinaryWriter bw(&vStrm);
			bw.SuspendStreamRelease();

			for (int32 i=0;i<count;i++)
				bw.Write(str[i]);

			bw.Close();
		}
		void TaggedDataWriter::_SetEntryDataPlane(const Entry& ent, const Plane* plane, int32 count)					{ for (int32 i=0;i<count;i++) _SetEntryDataPlane(ent, plane[i]); }
		void TaggedDataWriter::_SetEntryDataQuaternion(const Entry& ent, const Quaternion* quat, int32 count)			{ for (int32 i=0;i<count;i++) _SetEntryDataQuaternion(ent, quat[i]); }
		void TaggedDataWriter::_SetEntryDataRectangle(const Entry& ent, const Rectangle* rect, int32 count)			{ for (int32 i=0;i<count;i++) _SetEntryDataRectangle(ent, rect[i]); }
		void TaggedDataWriter::_SetEntryDataRectangleF(const Entry& ent, const RectangleF* rect, int32 count)			{ for (int32 i=0;i<count;i++) _SetEntryDataRectangleF(ent, rect[i]); }
		void TaggedDataWriter::_SetEntryDataPoint(const Entry& ent, const Point* pt, int32 count)						{ for (int32 i=0;i<count;i++) _SetEntryDataPoint(ent, pt[i]); }
		void TaggedDataWriter::_SetEntryDataPointF(const Entry& ent, const PointF* pt, int32 count)					{ for (int32 i=0;i<count;i++) _SetEntryDataPointF(ent, pt[i]); }
		void TaggedDataWriter::_SetEntryDataMathSize(const Entry& ent, const Apoc3D::Math::Size* sz, int32 count)		{ for (int32 i=0;i<count;i++) _SetEntryDataMathSize(ent, sz[i]); }
		void TaggedDataWriter::_SetEntryDataBoundingBox(const Entry& ent, const BoundingBox* bb, int32 count)			{ for (int32 i=0;i<count;i++) _SetEntryDataBoundingBox(ent, bb[i]); }
		void TaggedDataWriter::_SetEntryDataBoundingSphere(const Entry& ent, const BoundingSphere* bs, int32 count)	{ for (int32 i=0;i<count;i++) _SetEntryDataBoundingSphere(ent, bs[i]); }
		void TaggedDataWriter::_SetEntryDataRay(const Entry& ent, const Ray* r, int32 count)							{ for (int32 i=0;i<count;i++) _SetEntryDataRay(ent, r[i]); }
		void TaggedDataWriter::_SetEntryDataViewport(const Entry& ent, const Viewport* vp, int32 count)				{ for (int32 i=0;i<count;i++) _SetEntryDataViewport(ent, vp[i]); }

	}
}
