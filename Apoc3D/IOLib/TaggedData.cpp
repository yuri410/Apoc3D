/* -----------------------------------------------------------------------
 * This source file is part of Apoc3D Framework
 * 
 * Copyright (c) 2011-2017 Tao Xin
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

#include "TaggedData.h"
#include "BinaryReader.h"
#include "BinaryWriter.h"
#include "Streams.h"

#include "apoc3d/Config/ConfigurationSection.h"
#include "apoc3d/Config/ConfigurationManager.h"
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
		TaggedDataKey::TaggedDataKey(uint32 hash, const std::string& name)
			: m_hash(hash)
		{
			size_t nameSize = name.size() + 1;
			
			//assert(nameSize <= StringMax);

			nameSize = Math::Min((size_t)StringMax, nameSize);
			m_nameLen = (byte)nameSize - 1;

			memcpy(m_nameLocal, name.c_str(), m_nameLen);
			m_nameLocal[m_nameLen] = 0;
		}

		void TaggedDataKey::Read(BinaryReader& br)
		{
			m_hash = br.ReadUInt32();

			uint32 nameLen = br.ReadByte();
			uint32 nameSize = nameLen + 1;

			int64 seekForward = 0;
			if (nameSize > StringMax)
			{
				seekForward = StringMax - nameSize;

				nameSize = StringMax;
				nameLen = StringMax - 1;
			}

			br.ReadBytes(m_nameLocal, nameLen);
			m_nameLocal[nameLen] = 0;
			m_nameLen = (byte)nameLen;

			if (seekForward)
				br.getBaseStream()->Seek(seekForward, SeekMode::Current);
		}

		void TaggedDataKey::Write(BinaryWriter& bw) const
		{
			bw.WriteUInt32(m_hash);
			
			const char* str = getString();
			uint32 nameLen = Math::Min((uint32)StringMax - 1, m_nameLen);
			bw.WriteByte((byte)nameLen);
			bw.WriteBytes(str, nameLen);
		}

		TaggedDataKey TaggedDataKey::CreateAppended(const char* str, uint32 len) const
		{
			TaggedDataKey r = *this;

			// extend hash
			r.m_hash = Utility::FNVHash32(m_hash).Accumulate(str, len).getResult();

			//assert(m_nameLen + len <= StringMax - 1);
			uint32 copiableSize = Math::Min(StringMax - 1 - m_nameLen, len);

			if (r.m_nameConst)
			{
				memcpy(r.m_nameLocal, r.m_nameConst, m_nameLen);
				r.m_nameConst = nullptr;
			}

			memcpy(r.m_nameLocal + m_nameLen, str, copiableSize);

			r.m_nameLen = (byte)(r.m_nameLen + copiableSize);
			r.m_nameLocal[r.m_nameLen] = 0;

			return r;
		}

		TaggedDataKey TaggedDataKey::operator+(const std::string& o) const
		{
			return CreateAppended(o.c_str(), (uint32)o.size());
		}

		uint32 _uitoa(uint32 value, char (&str)[16])
		{
			char tmp[16];
			char *tp = tmp;
			uint32 v = value;

			while (v || tp == tmp)
			{
				uint32 i = v % 10;
				v /= 10;

				*tp++ = i + '0';
			}

			uint32 len = (uint32)(tp - tmp);

			char* sp = str;
			while (tp > tmp)
				*sp++ = *--tp;

			*sp = 0;
			return len;
		}

		TaggedDataKey TaggedDataKey::operator+(uint32 o) const
		{
			char str[16];
			uint32 len = _uitoa(o, str);

			return CreateAppended(str, len);
		}

		//////////////////////////////////////////////////////////////////////////
		//////////////////////////////////////////////////////////////////////////
		//////////////////////////////////////////////////////////////////////////

		/************************************************************************/
		/* TaggedDataReader                                                     */
		/************************************************************************/

		enum
		{
			TF_None = 0,
			TF_NarrowKeyFormat = 1,
			TF_HashKeyFormat = 4, // fnv1-a w/o null terminator

			TF_64Bit = 16
		};

		TaggedDataReader::TaggedDataReader(Stream* strm)
			: m_stream(strm)
		{
			m_endBlockPosition = strm->getPosition() + strm->getLength();

			m_endianIndependent = strm->IsReadEndianIndependent();

			BinaryReader br(strm, false);

			uint32 firstInt = br.ReadUInt32();
			if ((firstInt & 0x80000000U) == 0x80000000U)
			{
				// format ver 1.1, firstInt is flag
				uint32 flags = firstInt & 0x7fffffffU;

				bool narrowKeyFormat = (flags & TF_NarrowKeyFormat) == TF_NarrowKeyFormat;
				bool hashKeyFormat = (flags & TF_HashKeyFormat) == TF_HashKeyFormat;
				bool use64Bit = (flags & TF_64Bit) == TF_64Bit;

				m_sectCount = br.ReadInt32();

				if (hashKeyFormat)
				{
					for (int32 i = 0; i < m_sectCount; i++)
					{
						KeyType key;
						key.Read(br);

						m_positions.Add(key, Entry());
					}
				}
				else if (narrowKeyFormat)
				{
					for (int32 i = 0; i < m_sectCount; i++)
					{
						std::string name = br.ReadMBString();
						m_positions.Add({ name }, Entry());
					}
				}
				else
				{
					for (int32 i = 0; i < m_sectCount; i++)
					{
						std::string name = StringUtils::UTF16toUTF8(br.ReadString());
						m_positions.Add({ name }, Entry());
					}
				}

				if (use64Bit)
				{
					for (Entry& ent : m_positions.getValueAccessor())
					{
						ent.Offset = br.ReadUInt64();
						ent.Size = br.ReadUInt64();
					}
				}
				else
				{
					for (Entry& ent : m_positions.getValueAccessor())
					{
						ent.Offset = br.ReadUInt32();
						ent.Size = br.ReadUInt32();
					}
				}
			}
			else
			{
				// original format
				m_sectCount = (int32)firstInt;

				for (int32 i = 0; i < m_sectCount; i++)
				{
					std::string name = StringUtils::UTF16toUTF8(br.ReadString());
					uint size = br.ReadUInt32();

					m_positions.Add({ name }, Entry(strm->getPosition(), size));
					strm->Seek(size, SeekMode::Current);
				}
			}
		}
		TaggedDataReader::~TaggedDataReader()
		{
			if (!m_suspendStreamRelease)
				delete m_stream;
			m_stream = nullptr;
		}

		int64 TaggedDataReader::GetChunkOffset(const KeyType& name) const
		{
			const Entry* ent = FindEntry(name);
			if (!ent)
			{
				KeynotFoundError(name);
				return 0;
			}

			return ent->Offset;
		}
		void TaggedDataReader::Close(bool seekToEnd)
		{
			if (seekToEnd)
				m_stream->setPosition(m_endBlockPosition);
		}

		void TaggedDataReader::FillTagList(List<std::string>& nameTags) const
		{
			for (const KeyType& key : m_positions.getKeyAccessor())
			{
				nameTags.Add(key.getString());
			}
		}
		void TaggedDataReader::FillTagList(List<const char*>& nameTags) const
		{
			for (const KeyType& key : m_positions.getKeyAccessor())
			{
				nameTags.Add(key.getString());
			}
		}


		void TaggedDataReader::FillBuffer(const KeyType& name, uint32 len)
		{
			assert(len <= sizeof(m_buffer));

			const Entry* ent = FindEntry(name);
			m_stream->setPosition(ent->Offset);
			m_stream->Read(m_buffer, len);
		}
		void TaggedDataReader::FillBuffer(const Entry& ent, uint32 len)
		{
			assert(len <= sizeof(m_buffer));

			m_stream->setPosition(ent.Offset);
			m_stream->Read(m_buffer, len);
		}
		void TaggedDataReader::FillBufferCurrent(uint32 len)
		{
			assert(len <= sizeof(m_buffer));
			m_stream->Read(m_buffer, len);
		}
		bool TaggedDataReader::TryFillBuffer(const KeyType& name, uint32 len)
		{
			const Entry* ent = FindEntry(name);
			if (!ent) return false;

			assert(len <= sizeof(m_buffer));
			m_stream->setPosition(ent->Offset);
			m_stream->Read(m_buffer, len);
			return true;
		}

		BinaryReader* TaggedDataReader::TryGetData(const KeyType& name) const
		{
			const Entry* ent = FindEntry(name);
			if (ent)
				return new BinaryReader(new VirtualStream(m_stream, ent->Offset, ent->Size), true);
			return nullptr;
		}
		BinaryReader* TaggedDataReader::GetData(const KeyType& name) const
		{
			const Entry* ent = FindEntry(name);
			if (ent)
				return new BinaryReader(new VirtualStream(m_stream, ent->Offset, ent->Size), true);
			KeynotFoundError(name);
			return nullptr;
		}


		bool TaggedDataReader::TryProcessData(const KeyType& name, FunctorReference<void(BinaryReader*)> func) const
		{
			const Entry* ent = FindEntry(name);

			if (ent)
			{
				VirtualStream vs(m_stream, ent->Offset, ent->Size);
				BinaryReader br(&vs, false);
				func(&br);
				return true;
			}
			return false;
		}

		void TaggedDataReader::ProcessData(const KeyType& name, FunctorReference<void(BinaryReader*)> func) const
		{
			const Entry* ent = FindEntry(name);
			VirtualStream vs(m_stream, ent->Offset, ent->Size);
			{
				BinaryReader br(&vs, false);
				func(&br);
			}
		}

		bool TaggedDataReader::TryProcessDataSection(const KeyType& name, FunctorReference<void(TaggedDataReader*)> f) const
		{
			return TryProcessData(name, [f](BinaryReader* br) 
			{
				br->ReadTaggedDataBlock(f, false); 
			});
		}
		void TaggedDataReader::ProcessDataSection(const KeyType& name, FunctorReference<void(TaggedDataReader*)> f) const
		{
			ProcessData(name, [f](BinaryReader* br) 
			{
				br->ReadTaggedDataBlock(f, false); 
			});
		}


		Stream* TaggedDataReader::GetDataStream(const KeyType& name) const
		{
			const Entry* ent = FindEntry(name);
			if (ent)
				return new VirtualStream(m_stream, ent->Offset, ent->Size);
			KeynotFoundError(name);
			return nullptr;
		}


		void TaggedDataReader::GetAuto(const KeyType& name, int64& value)	{ value = GetInt64(name); }
		void TaggedDataReader::GetAuto(const KeyType& name, int32& value)	{ value = GetInt32(name); }
		void TaggedDataReader::GetAuto(const KeyType& name, int16& value)	{ value = GetInt16(name); }
		void TaggedDataReader::GetAuto(const KeyType& name, uint64& value)	{ value = GetUInt64(name); }
		void TaggedDataReader::GetAuto(const KeyType& name, uint32& value)	{ value = GetUInt32(name); }
		void TaggedDataReader::GetAuto(const KeyType& name, uint16& value)	{ value = GetUInt16(name); }
		void TaggedDataReader::GetAuto(const KeyType& name, float& value)	{ value = GetSingle(name); }
		void TaggedDataReader::GetAuto(const KeyType& name, double& value)	{ value = GetDouble(name); }
		void TaggedDataReader::GetAuto(const KeyType& name, bool& value)	{ value = GetBool(name); }
		void TaggedDataReader::GetAuto(const KeyType& name, Vector2& vec)	{ GetVector2(name, vec); }
		void TaggedDataReader::GetAuto(const KeyType& name, Vector3& vec)	{ GetVector3(name, vec); }
		void TaggedDataReader::GetAuto(const KeyType& name, Vector4& vec)	{ GetVector4(name, vec); }
		void TaggedDataReader::GetAuto(const KeyType& name, Matrix& mat)	{ GetMatrix(name, mat); }
		void TaggedDataReader::GetAuto(const KeyType& name, Quaternion& quad)	{ GetQuaternion(name, quad); }
		void TaggedDataReader::GetAuto(const KeyType& name, Color4& clr)		{ GetColor4(name, clr); }
		void TaggedDataReader::GetAuto(const KeyType& name, Plane& plane)		{ GetPlane(name, plane); }
		void TaggedDataReader::GetAuto(const KeyType& name, Apoc3D::Math::Rectangle& rect)	{ GetRectangle(name, rect); }
		void TaggedDataReader::GetAuto(const KeyType& name, Apoc3D::Math::RectangleF& rect)	{ GetRectangleF(name, rect); }
		void TaggedDataReader::GetAuto(const KeyType& name, Point& pt)				{ GetPoint(name, pt); }
		void TaggedDataReader::GetAuto(const KeyType& name, PointF& pt)				{ GetPointF(name, pt); }
		void TaggedDataReader::GetAuto(const KeyType& name, Apoc3D::Math::Size& sz)	{ GetMathSize(name, sz); }
		void TaggedDataReader::GetAuto(const KeyType& name, BoundingBox& bb)		{ GetBoundingBox(name, bb); }
		void TaggedDataReader::GetAuto(const KeyType& name, BoundingSphere& bs)		{ GetBoundingSphere(name, bs); }
		void TaggedDataReader::GetAuto(const KeyType& name, Ray& r)					{ GetRay(name, r); }
		void TaggedDataReader::GetAuto(const KeyType& name, Viewport& vp)			{ GetViewport(name, vp); }
		void TaggedDataReader::GetAuto(const KeyType& name, String& str)			{ GetString(name, str); }

		void TaggedDataReader::GetAuto(const KeyType& name, int64* value, int32 count)	{ GetInt64(name, value, count); }
		void TaggedDataReader::GetAuto(const KeyType& name, int32* value, int32 count)	{ GetInt32(name, value, count); }
		void TaggedDataReader::GetAuto(const KeyType& name, int16* value, int32 count)	{ GetInt16(name, value, count); }
		void TaggedDataReader::GetAuto(const KeyType& name, uint64* value, int32 count)	{ GetUInt64(name, value, count); }
		void TaggedDataReader::GetAuto(const KeyType& name, uint32* value, int32 count)	{ GetUInt32(name, value, count); }
		void TaggedDataReader::GetAuto(const KeyType& name, uint16* value, int32 count)	{ GetUInt16(name, value, count); }
		void TaggedDataReader::GetAuto(const KeyType& name, float* value, int32 count)	{ GetSingle(name, value, count); }
		void TaggedDataReader::GetAuto(const KeyType& name, double* value, int32 count)	{ GetDouble(name, value, count); }
		void TaggedDataReader::GetAuto(const KeyType& name, bool* value, int32 count)	{ GetBool(name, value, count); }
		void TaggedDataReader::GetAuto(const KeyType& name, Vector2* vec, int32 count)	{ GetVector2(name, vec, count); }
		void TaggedDataReader::GetAuto(const KeyType& name, Vector3* vec, int32 count)	{ GetVector3(name, vec, count); }
		void TaggedDataReader::GetAuto(const KeyType& name, Vector4* vec, int32 count)	{ GetVector4(name, vec, count); }
		void TaggedDataReader::GetAuto(const KeyType& name, Matrix* mat, int32 count)	{ GetMatrix(name, mat, count); }
		void TaggedDataReader::GetAuto(const KeyType& name, Quaternion* quad, int32 count)	{ GetQuaternion(name, quad, count); }
		void TaggedDataReader::GetAuto(const KeyType& name, Color4* clr, int32 count)		{ GetColor4(name, clr, count); }
		void TaggedDataReader::GetAuto(const KeyType& name, Plane* plane, int32 count)		{ GetPlane(name, plane, count); }
		void TaggedDataReader::GetAuto(const KeyType& name, Apoc3D::Math::Rectangle* rect, int32 count)	{ GetRectangle(name, rect, count); }
		void TaggedDataReader::GetAuto(const KeyType& name, RectangleF* rect, int32 count)				{ GetRectangleF(name, rect, count); }
		void TaggedDataReader::GetAuto(const KeyType& name, Point* pt, int32 count)						{ GetPoint(name, pt, count); }
		void TaggedDataReader::GetAuto(const KeyType& name, PointF* pt, int32 count)					{ GetPointF(name, pt, count); }
		void TaggedDataReader::GetAuto(const KeyType& name, Apoc3D::Math::Size* sz, int32 count)		{ GetMathSize(name, sz, count); }
		void TaggedDataReader::GetAuto(const KeyType& name, BoundingBox* bb, int32 count)				{ GetBoundingBox(name, bb, count); }
		void TaggedDataReader::GetAuto(const KeyType& name, BoundingSphere* bs, int32 count)			{ GetBoundingSphere(name, bs, count); }
		void TaggedDataReader::GetAuto(const KeyType& name, Ray* r, int32 count)						{ GetRay(name, r, count); }
		void TaggedDataReader::GetAuto(const KeyType& name, Viewport* vp, int32 count)					{ GetViewport(name, vp, count); }
		void TaggedDataReader::GetAuto(const KeyType& name, String* str, int32 count)					{ GetString(name, str, count); }

		bool TaggedDataReader::TryGetAuto(const KeyType& name, int64& value)	{ return TryGetInt64(name, value); }
		bool TaggedDataReader::TryGetAuto(const KeyType& name, int32& value)	{ return TryGetInt32(name, value); }
		bool TaggedDataReader::TryGetAuto(const KeyType& name, int16& value)	{ return TryGetInt16(name, value); }
		bool TaggedDataReader::TryGetAuto(const KeyType& name, uint64& value)	{ return TryGetUInt64(name, value); }
		bool TaggedDataReader::TryGetAuto(const KeyType& name, uint32& value)	{ return TryGetUInt32(name, value); }
		bool TaggedDataReader::TryGetAuto(const KeyType& name, uint16& value)	{ return TryGetUInt16(name, value); }
		bool TaggedDataReader::TryGetAuto(const KeyType& name, float& value)	{ return TryGetSingle(name, value); }
		bool TaggedDataReader::TryGetAuto(const KeyType& name, double& value)	{ return TryGetDouble(name, value); }
		bool TaggedDataReader::TryGetAuto(const KeyType& name, bool& value)		{ return TryGetBool(name, value); }
		bool TaggedDataReader::TryGetAuto(const KeyType& name, Vector2& vec)	{ return TryGetVector2(name, vec); }
		bool TaggedDataReader::TryGetAuto(const KeyType& name, Vector3& vec)	{ return TryGetVector3(name, vec); }
		bool TaggedDataReader::TryGetAuto(const KeyType& name, Vector4& vec)	{ return TryGetVector4(name, vec); }
		bool TaggedDataReader::TryGetAuto(const KeyType& name, Matrix& mat)		{ return TryGetMatrix(name, mat); }
		bool TaggedDataReader::TryGetAuto(const KeyType& name, Quaternion& quad)				{ return TryGetQuaternion(name, quad); }
		bool TaggedDataReader::TryGetAuto(const KeyType& name, Color4& clr)						{ return TryGetColor4(name, clr); }
		bool TaggedDataReader::TryGetAuto(const KeyType& name, Plane& plane)					{ return TryGetPlane(name, plane); }
		bool TaggedDataReader::TryGetAuto(const KeyType& name, Apoc3D::Math::Rectangle& rect)	{ return TryGetRectangle(name, rect); }
		bool TaggedDataReader::TryGetAuto(const KeyType& name, RectangleF& rect)				{ return TryGetRectangleF(name, rect); }
		bool TaggedDataReader::TryGetAuto(const KeyType& name, Point& pt)						{ return TryGetPoint(name, pt); }
		bool TaggedDataReader::TryGetAuto(const KeyType& name, PointF& pt)						{ return TryGetPointF(name, pt); }
		bool TaggedDataReader::TryGetAuto(const KeyType& name, Apoc3D::Math::Size& sz)			{ return TryGetSize(name, sz); }
		bool TaggedDataReader::TryGetAuto(const KeyType& name, BoundingBox& bb)					{ return TryGetBoundingBox(name, bb); }
		bool TaggedDataReader::TryGetAuto(const KeyType& name, BoundingSphere& bs)				{ return TryGetBoundingSphere(name, bs); }
		bool TaggedDataReader::TryGetAuto(const KeyType& name, Ray& r)							{ return TryGetRay(name, r); }
		bool TaggedDataReader::TryGetAuto(const KeyType& name, Viewport& vp)					{ return TryGetViewport(name, vp); }
		bool TaggedDataReader::TryGetAuto(const KeyType& name, String& str)						{ return TryGetString(name, str); }

		bool TaggedDataReader::TryGetAuto(const KeyType& name, int64* value, int32 count)		{ return TryGetInt64(name, value, count); }
		bool TaggedDataReader::TryGetAuto(const KeyType& name, int32* value, int32 count)		{ return TryGetInt32(name, value, count); }
		bool TaggedDataReader::TryGetAuto(const KeyType& name, int16* value, int32 count)		{ return TryGetInt16(name, value, count); }
		bool TaggedDataReader::TryGetAuto(const KeyType& name, uint64* value, int32 count)		{ return TryGetUInt64(name, value, count); }
		bool TaggedDataReader::TryGetAuto(const KeyType& name, uint32* value, int32 count)		{ return TryGetUInt32(name, value, count); }
		bool TaggedDataReader::TryGetAuto(const KeyType& name, uint16* value, int32 count)		{ return TryGetUInt16(name, value, count); }
		bool TaggedDataReader::TryGetAuto(const KeyType& name, float* value, int32 count)		{ return TryGetSingle(name, value, count); }
		bool TaggedDataReader::TryGetAuto(const KeyType& name, double* value, int32 count)		{ return TryGetDouble(name, value, count); }
		bool TaggedDataReader::TryGetAuto(const KeyType& name, bool* value, int32 count)		{ return TryGetBool(name, value, count); }
		bool TaggedDataReader::TryGetAuto(const KeyType& name, Vector2* vec, int32 count)		{ return TryGetVector2(name, vec, count); }
		bool TaggedDataReader::TryGetAuto(const KeyType& name, Vector3* vec, int32 count)		{ return TryGetVector3(name, vec, count); }
		bool TaggedDataReader::TryGetAuto(const KeyType& name, Vector4* vec, int32 count)		{ return TryGetVector4(name, vec, count); }
		bool TaggedDataReader::TryGetAuto(const KeyType& name, Matrix* mat, int32 count)		{ return TryGetMatrix(name, mat, count); }
		bool TaggedDataReader::TryGetAuto(const KeyType& name, Quaternion* quad, int32 count)	{ return TryGetQuaternion(name, quad, count); }
		bool TaggedDataReader::TryGetAuto(const KeyType& name, Color4* clr, int32 count)		{ return TryGetColor4(name, clr, count); }
		bool TaggedDataReader::TryGetAuto(const KeyType& name, Plane* plane, int32 count)		{ return TryGetPlane(name, plane, count); }
		bool TaggedDataReader::TryGetAuto(const KeyType& name, Apoc3D::Math::Rectangle* rect, int32 count) { return TryGetRectangle(name, rect, count); }
		bool TaggedDataReader::TryGetAuto(const KeyType& name, RectangleF* rect, int32 count)		{ return TryGetRectangleF(name, rect, count); }
		bool TaggedDataReader::TryGetAuto(const KeyType& name, Point* pt, int32 count)				{ return TryGetPoint(name, pt, count); }
		bool TaggedDataReader::TryGetAuto(const KeyType& name, PointF* pt, int32 count)				{ return TryGetPointF(name, pt, count); }
		bool TaggedDataReader::TryGetAuto(const KeyType& name, Apoc3D::Math::Size* sz, int32 count)	{ return TryGetSize(name, sz, count); }
		bool TaggedDataReader::TryGetAuto(const KeyType& name, BoundingBox* bb, int32 count)		{ return TryGetBoundingBox(name, bb, count); }
		bool TaggedDataReader::TryGetAuto(const KeyType& name, BoundingSphere* bs, int32 count)		{ return TryGetBoundingSphere(name, bs, count); }
		bool TaggedDataReader::TryGetAuto(const KeyType& name, Ray* r, int32 count)					{ return TryGetRay(name, r, count); }
		bool TaggedDataReader::TryGetAuto(const KeyType& name, Viewport* vp, int32 count)			{ return TryGetViewport(name, vp, count); }
		bool TaggedDataReader::TryGetAuto(const KeyType& name, String* str, int32 count)			{ return TryGetString(name, str, count); }

#define TDR_GETDATA(name, value, getter) const Entry* ent = FindEntry(name); assert(ent); getter(ent, value);
#define TDR_GETDATA_ARR(name, value, count, getter) const Entry* ent = FindEntry(name); assert(ent); getter(ent, value, count);

		int64 TaggedDataReader::GetInt64(const KeyType& name)	{ int64 r; TDR_GETDATA(name, r, _GetEntryInt64); return r; }
		int32 TaggedDataReader::GetInt32(const KeyType& name)	{ int32 r; TDR_GETDATA(name, r, _GetEntryInt32); return r; }
		int16 TaggedDataReader::GetInt16(const KeyType& name)	{ int16 r; TDR_GETDATA(name, r, _GetEntryInt16); return r; }
		uint64 TaggedDataReader::GetUInt64(const KeyType& name)	{ uint64 r; TDR_GETDATA(name, r, _GetEntryUInt64); return r; }
		uint32 TaggedDataReader::GetUInt32(const KeyType& name)	{ uint32 r; TDR_GETDATA(name, r, _GetEntryUInt32); return r; }
		uint16 TaggedDataReader::GetUInt16(const KeyType& name)	{ uint16 r; TDR_GETDATA(name, r, _GetEntryUInt16); return r; }
		bool TaggedDataReader::GetBool(const KeyType& name)		{ bool r; TDR_GETDATA(name, r, _GetEntryBool); return r; }
		float TaggedDataReader::GetSingle(const KeyType& name)	{ float r; TDR_GETDATA(name, r, _GetEntrySingle); return r; }
		double TaggedDataReader::GetDouble(const KeyType& name)	{ double r; TDR_GETDATA(name, r, _GetEntryDouble); return r; }

		void TaggedDataReader::GetInt64(const KeyType& name, int64* value, int32 count)		{ TDR_GETDATA_ARR(name, value, count, _GetEntryInt64); }
		void TaggedDataReader::GetInt32(const KeyType& name, int32* value, int32 count)		{ TDR_GETDATA_ARR(name, value, count, _GetEntryInt32); }
		void TaggedDataReader::GetInt16(const KeyType& name, int16* value, int32 count)		{ TDR_GETDATA_ARR(name, value, count, _GetEntryInt16); }
		void TaggedDataReader::GetUInt64(const KeyType& name, uint64* value, int32 count)	{ TDR_GETDATA_ARR(name, value, count, _GetEntryUInt64); }
		void TaggedDataReader::GetUInt32(const KeyType& name, uint32* value, int32 count)	{ TDR_GETDATA_ARR(name, value, count, _GetEntryUInt32); }
		void TaggedDataReader::GetUInt16(const KeyType& name, uint16* value, int32 count)	{ TDR_GETDATA_ARR(name, value, count, _GetEntryUInt16); }
		void TaggedDataReader::GetSingle(const KeyType& name, float* value, int32 count)	{ TDR_GETDATA_ARR(name, value, count, _GetEntrySingle); }
		void TaggedDataReader::GetDouble(const KeyType& name, double* value, int32 count)	{ TDR_GETDATA_ARR(name, value, count, _GetEntryDouble); }
		void TaggedDataReader::GetBool(const KeyType& name, bool* value, int32 count)		{ TDR_GETDATA_ARR(name, value, count, _GetEntryBool); }
		
		void TaggedDataReader::GetVector2(const KeyType& name, Vector2& vec)	{ TDR_GETDATA(name, vec, _GetEntryVector2); }
		void TaggedDataReader::GetVector3(const KeyType& name, Vector3& vec)	{ TDR_GETDATA(name, vec, _GetEntryVector3); }
		void TaggedDataReader::GetVector4(const KeyType& name, Vector4& vec)	{ TDR_GETDATA(name, vec, _GetEntryVector4); }
		void TaggedDataReader::GetColor4(const KeyType& name, Color4& clr)		{ TDR_GETDATA(name, clr, _GetEntryColor4); }
		void TaggedDataReader::GetMatrix(const KeyType& name, Matrix& mat)		{ TDR_GETDATA(name, mat, _GetEntryMatrix); }
		void TaggedDataReader::GetPlane(const KeyType& name, Plane& plane)		{ TDR_GETDATA(name, plane, _GetEntryPlane); }
		void TaggedDataReader::GetQuaternion(const KeyType& name, Quaternion& quat) { TDR_GETDATA(name, quat, _GetEntryQuaternion); }
		void TaggedDataReader::GetString(const KeyType& name, String& str)		{ TDR_GETDATA(name, str, _GetEntryString); }
		void TaggedDataReader::GetRectangle(const KeyType& name, Rectangle& rect)			{ TDR_GETDATA(name, rect, _GetEntryRectangle); }
		void TaggedDataReader::GetRectangleF(const KeyType& name, RectangleF& rect)			{ TDR_GETDATA(name, rect, _GetEntryRectangleF); }
		void TaggedDataReader::GetPoint(const KeyType& name, Point& pt)						{ TDR_GETDATA(name, pt, _GetEntryPoint); }
		void TaggedDataReader::GetPointF(const KeyType& name, PointF& pt)					{ TDR_GETDATA(name, pt, _GetEntryPointF); }
		void TaggedDataReader::GetMathSize(const KeyType& name, Apoc3D::Math::Size& sz)		{ TDR_GETDATA(name, sz, _GetEntryMathSize); }
		void TaggedDataReader::GetBoundingBox(const KeyType& name, BoundingBox& bb)			{ TDR_GETDATA(name, bb, _GetEntryBoundingBox); }
		void TaggedDataReader::GetBoundingSphere(const KeyType& name, BoundingSphere& bs)	{ TDR_GETDATA(name, bs, _GetEntryBoundingSphere); }
		void TaggedDataReader::GetRay(const KeyType& name, Ray& r)							{ TDR_GETDATA(name, r, _GetEntryRay); }
		void TaggedDataReader::GetViewport(const KeyType& name, Viewport& vp)				{ TDR_GETDATA(name, vp, _GetEntryViewport); }

		void TaggedDataReader::GetVector2(const KeyType& name, Vector2* vec, int32 count)	{ TDR_GETDATA_ARR(name, vec, count, _GetEntryVector2); }
		void TaggedDataReader::GetVector3(const KeyType& name, Vector3* vec, int32 count)	{ TDR_GETDATA_ARR(name, vec, count, _GetEntryVector3); }
		void TaggedDataReader::GetVector4(const KeyType& name, Vector4* vec, int32 count)	{ TDR_GETDATA_ARR(name, vec, count, _GetEntryVector4); }
		void TaggedDataReader::GetColor4(const KeyType& name, Color4* clr, int32 count)		{ TDR_GETDATA_ARR(name, clr, count, _GetEntryColor4); }
		void TaggedDataReader::GetMatrix(const KeyType& name, Matrix* mat, int32 count)		{ TDR_GETDATA_ARR(name, mat, count, _GetEntryMatrix); }
		void TaggedDataReader::GetPlane(const KeyType& name, Plane* str, int32 count)		{ TDR_GETDATA_ARR(name, str, count, _GetEntryPlane); }
		void TaggedDataReader::GetQuaternion(const KeyType& name, Quaternion* quat, int32 count) { TDR_GETDATA_ARR(name, quat, count, _GetEntryQuaternion); }
		void TaggedDataReader::GetString(const KeyType& name, String* str, int32 count)		{ TDR_GETDATA_ARR(name, str, count, _GetEntryString); }
		void TaggedDataReader::GetRectangle(const KeyType& name, Rectangle* rect, int32 count)			{ TDR_GETDATA_ARR(name, rect, count, _GetEntryRectangle); }
		void TaggedDataReader::GetRectangleF(const KeyType& name, RectangleF* rect, int32 count)		{ TDR_GETDATA_ARR(name, rect, count, _GetEntryRectangleF); }
		void TaggedDataReader::GetPoint(const KeyType& name, Point* pt, int32 count)					{ TDR_GETDATA_ARR(name, pt, count, _GetEntryPoint); }
		void TaggedDataReader::GetPointF(const KeyType& name, PointF* pt, int32 count)					{ TDR_GETDATA_ARR(name, pt, count, _GetEntryPointF); }
		void TaggedDataReader::GetMathSize(const KeyType& name, Apoc3D::Math::Size* sz, int32 count)	{ TDR_GETDATA_ARR(name, sz, count, _GetEntryMathSize); }
		void TaggedDataReader::GetBoundingBox(const KeyType& name, BoundingBox* bb, int32 count)		{ TDR_GETDATA_ARR(name, bb, count, _GetEntryBoundingBox); }
		void TaggedDataReader::GetBoundingSphere(const KeyType& name, BoundingSphere* bs, int32 count)	{ TDR_GETDATA_ARR(name, bs, count, _GetEntryBoundingSphere); }
		void TaggedDataReader::GetRay(const KeyType& name, Ray* r, int32 count)							{ TDR_GETDATA_ARR(name, r, count, _GetEntryRay); }
		void TaggedDataReader::GetViewport(const KeyType& name, Viewport* vp, int32 count)				{ TDR_GETDATA_ARR(name, vp, count, _GetEntryViewport); }

#define TDR_TRYGETDATA(name, value, getter) const Entry* ent = FindEntry(name); if (ent) { getter(ent, value); return true; } return false;

		bool TaggedDataReader::TryGetInt64(const KeyType& name, int64& v)	{ TDR_TRYGETDATA(name, v, _GetEntryInt64); }
		bool TaggedDataReader::TryGetInt32(const KeyType& name, int32& v)	{ TDR_TRYGETDATA(name, v, _GetEntryInt32); }
		bool TaggedDataReader::TryGetInt16(const KeyType& name, int16& v)	{ TDR_TRYGETDATA(name, v, _GetEntryInt16); }
		bool TaggedDataReader::TryGetUInt64(const KeyType& name, uint64& v)	{ TDR_TRYGETDATA(name, v, _GetEntryUInt64); }
		bool TaggedDataReader::TryGetUInt32(const KeyType& name, uint32& v)	{ TDR_TRYGETDATA(name, v, _GetEntryUInt32); }
		bool TaggedDataReader::TryGetUInt16(const KeyType& name, uint16& v)	{ TDR_TRYGETDATA(name, v, _GetEntryUInt16); }
		bool TaggedDataReader::TryGetBool(const KeyType& name, bool& v)		{ TDR_TRYGETDATA(name, v, _GetEntryBool); }
		bool TaggedDataReader::TryGetSingle(const KeyType& name, float& v)	{ TDR_TRYGETDATA(name, v, _GetEntrySingle); }
		bool TaggedDataReader::TryGetDouble(const KeyType& name, double& v)	{ TDR_TRYGETDATA(name, v, _GetEntryDouble); }

		bool TaggedDataReader::TryGetVector2(const KeyType& name, Vector2& vec)			{ TDR_TRYGETDATA(name, vec, _GetEntryVector2); }
		bool TaggedDataReader::TryGetVector3(const KeyType& name, Vector3& vec)			{ TDR_TRYGETDATA(name, vec, _GetEntryVector3); }
		bool TaggedDataReader::TryGetVector4(const KeyType& name, Vector4& vec)			{ TDR_TRYGETDATA(name, vec, _GetEntryVector4); }
		bool TaggedDataReader::TryGetMatrix(const KeyType& name, Matrix& mat)			{ TDR_TRYGETDATA(name, mat, _GetEntryMatrix); }
		bool TaggedDataReader::TryGetQuaternion(const KeyType& name, Quaternion& quat)  { TDR_TRYGETDATA(name, quat, _GetEntryQuaternion); }
		bool TaggedDataReader::TryGetColor4(const KeyType& name, Color4& clr)			{ TDR_TRYGETDATA(name, clr, _GetEntryColor4); }
		bool TaggedDataReader::TryGetPlane(const KeyType& name, Plane& plane)			{ TDR_TRYGETDATA(name, plane, _GetEntryPlane); }
		bool TaggedDataReader::TryGetRectangle(const KeyType& name, Apoc3D::Math::Rectangle& rect)			{ TDR_TRYGETDATA(name, rect, _GetEntryRectangle); }
		bool TaggedDataReader::TryGetRectangleF(const KeyType& name, Apoc3D::Math::RectangleF& rect)		{ TDR_TRYGETDATA(name, rect, _GetEntryRectangleF); }
		bool TaggedDataReader::TryGetPoint(const KeyType& name, Point& pt)						{ TDR_TRYGETDATA(name, pt, _GetEntryPoint); }
		bool TaggedDataReader::TryGetPointF(const KeyType& name, PointF& pt)					{ TDR_TRYGETDATA(name, pt, _GetEntryPointF); }
		bool TaggedDataReader::TryGetSize(const KeyType& name, Apoc3D::Math::Size& sz)			{ TDR_TRYGETDATA(name, sz, _GetEntryMathSize); }
		bool TaggedDataReader::TryGetBoundingBox(const KeyType& name, BoundingBox& bb)			{ TDR_TRYGETDATA(name, bb, _GetEntryBoundingBox); }
		bool TaggedDataReader::TryGetBoundingSphere(const KeyType& name, BoundingSphere& bs)	{ TDR_TRYGETDATA(name, bs, _GetEntryBoundingSphere); }
		bool TaggedDataReader::TryGetRay(const KeyType& name, Ray& r)							{ TDR_TRYGETDATA(name, r, _GetEntryRay); }
		bool TaggedDataReader::TryGetViewport(const KeyType& name, Viewport& vp)				{ TDR_TRYGETDATA(name, vp, _GetEntryViewport); }
		bool TaggedDataReader::TryGetString(const KeyType& name, String& str)					{ TDR_TRYGETDATA(name, str, _GetEntryString); }

		
#define TDR_TRYGETDATA_ARR(name, value, count, getter) const Entry* ent = FindEntry(name); if (ent) { getter(ent, value, count); return true; } return false;

		bool TaggedDataReader::TryGetInt64(const KeyType& name, int64* value, int32 count)		{ TDR_TRYGETDATA_ARR(name, value, count, _GetEntryInt64); }
		bool TaggedDataReader::TryGetInt32(const KeyType& name, int32* value, int32 count)		{ TDR_TRYGETDATA_ARR(name, value, count, _GetEntryInt32); }
		bool TaggedDataReader::TryGetInt16(const KeyType& name, int16* value, int32 count)		{ TDR_TRYGETDATA_ARR(name, value, count, _GetEntryInt16); }
		bool TaggedDataReader::TryGetUInt64(const KeyType& name, uint64* value, int32 count)	{ TDR_TRYGETDATA_ARR(name, value, count, _GetEntryUInt64); }
		bool TaggedDataReader::TryGetUInt32(const KeyType& name, uint32* value, int32 count)	{ TDR_TRYGETDATA_ARR(name, value, count, _GetEntryUInt32); }
		bool TaggedDataReader::TryGetUInt16(const KeyType& name, uint16* value, int32 count)	{ TDR_TRYGETDATA_ARR(name, value, count, _GetEntryUInt16); }
		bool TaggedDataReader::TryGetBool(const KeyType& name, bool* value, int32 count)		{ TDR_TRYGETDATA_ARR(name, value, count, _GetEntryBool); }
		bool TaggedDataReader::TryGetSingle(const KeyType& name, float* value, int32 count)		{ TDR_TRYGETDATA_ARR(name, value, count, _GetEntrySingle); }
		bool TaggedDataReader::TryGetDouble(const KeyType& name, double* value, int32 count)	{ TDR_TRYGETDATA_ARR(name, value, count, _GetEntryDouble); }

		bool TaggedDataReader::TryGetVector2(const KeyType& name, Vector2* vec, int32 count)		{ TDR_TRYGETDATA_ARR(name, vec, count, _GetEntryVector2); }
		bool TaggedDataReader::TryGetVector3(const KeyType& name, Vector3* vec, int32 count)		{ TDR_TRYGETDATA_ARR(name, vec, count, _GetEntryVector3); }
		bool TaggedDataReader::TryGetVector4(const KeyType& name, Vector4* vec, int32 count)		{ TDR_TRYGETDATA_ARR(name, vec, count, _GetEntryVector4); }
		bool TaggedDataReader::TryGetMatrix(const KeyType& name, Matrix* mat, int32 count)			{ TDR_TRYGETDATA_ARR(name, mat, count, _GetEntryMatrix); }
		bool TaggedDataReader::TryGetQuaternion(const KeyType& name, Quaternion* quat, int32 count)	{ TDR_TRYGETDATA_ARR(name, quat, count, _GetEntryQuaternion); }
		bool TaggedDataReader::TryGetColor4(const KeyType& name, Color4* clr, int32 count)			{ TDR_TRYGETDATA_ARR(name, clr, count, _GetEntryColor4); }
		bool TaggedDataReader::TryGetPlane(const KeyType& name, Plane* plane, int32 count)			{ TDR_TRYGETDATA_ARR(name, plane, count, _GetEntryPlane); }
		bool TaggedDataReader::TryGetRectangle(const KeyType& name, Apoc3D::Math::Rectangle* rect, int32 count)			{ TDR_TRYGETDATA_ARR(name, rect, count, _GetEntryRectangle); }
		bool TaggedDataReader::TryGetRectangleF(const KeyType& name, Apoc3D::Math::RectangleF* rect, int32 count)		{ TDR_TRYGETDATA_ARR(name, rect, count, _GetEntryRectangleF); }
		bool TaggedDataReader::TryGetPoint(const KeyType& name, Point* pt, int32 count)						{ TDR_TRYGETDATA_ARR(name, pt, count, _GetEntryPoint); }
		bool TaggedDataReader::TryGetPointF(const KeyType& name, PointF* pt, int32 count)					{ TDR_TRYGETDATA_ARR(name, pt, count, _GetEntryPointF); }
		bool TaggedDataReader::TryGetSize(const KeyType& name, Apoc3D::Math::Size* sz, int32 count)			{ TDR_TRYGETDATA_ARR(name, sz, count, _GetEntryMathSize); }
		bool TaggedDataReader::TryGetBoundingBox(const KeyType& name, BoundingBox* bb, int32 count)			{ TDR_TRYGETDATA_ARR(name, bb, count, _GetEntryBoundingBox); }
		bool TaggedDataReader::TryGetBoundingSphere(const KeyType& name, BoundingSphere* bs, int32 count)	{ TDR_TRYGETDATA_ARR(name, bs, count, _GetEntryBoundingSphere); }
		bool TaggedDataReader::TryGetRay(const KeyType& name, Ray* r, int32 count)							{ TDR_TRYGETDATA_ARR(name, r, count, _GetEntryRay); }
		bool TaggedDataReader::TryGetViewport(const KeyType& name, Viewport* vp, int32 count)				{ TDR_TRYGETDATA_ARR(name, vp, count, _GetEntryViewport); }
		bool TaggedDataReader::TryGetString(const KeyType& name, String* str, int32 count)			{ TDR_TRYGETDATA_ARR(name, str, count, _GetEntryString); }


		//////////////////////////////////////////////////////////////////////////
		//////////////////////////////////////////////////////////////////////////

#ifndef BIG_ENDIAN
		void TaggedDataReader::_GetEntryInt64(const Entry* e, int64& val) { FillBuffer(*e, sizeof(int64)); val = mb_i64_le(m_buffer); }
		void TaggedDataReader::_GetEntryInt32(const Entry* e, int32& val) { FillBuffer(*e, sizeof(int32)); val = mb_i32_le(m_buffer); }
		void TaggedDataReader::_GetEntryInt16(const Entry* e, int16& val) { FillBuffer(*e, sizeof(int16)); val = mb_i16_le(m_buffer); }
		void TaggedDataReader::_GetEntryUInt64(const Entry* e, uint64& val) { FillBuffer(*e, sizeof(uint64)); val = mb_u64_le(m_buffer); }
		void TaggedDataReader::_GetEntryUInt32(const Entry* e, uint32& val) { FillBuffer(*e, sizeof(uint32)); val = mb_u32_le(m_buffer);}
		void TaggedDataReader::_GetEntryUInt16(const Entry* e, uint16& val) { FillBuffer(*e, sizeof(int16)); val = mb_u16_le(m_buffer); }
		void TaggedDataReader::_GetEntryBool(const Entry* e, bool& val) { FillBuffer(*e, sizeof(bool)); val = !!m_buffer[0]; }
		void TaggedDataReader::_GetEntrySingle(const Entry* e, float& val) { FillBuffer(*e, sizeof(float)); val = mb_f32_le(m_buffer); }
		void TaggedDataReader::_GetEntryDouble(const Entry* e, double& val) { FillBuffer(*e, sizeof(double)); val = mb_f64_le(m_buffer); }
#else
		void TaggedDataReader::_GetEntryInt64(const Entry* e, int64& val) { FillBuffer(*e, sizeof(int64)); val = !m_endianIndependent ? ci64_dep(m_buffer) : mb_i64_le(m_buffer); }
		void TaggedDataReader::_GetEntryInt32(const Entry* e, int32& val) { FillBuffer(*e, sizeof(int32)); val = !m_endianIndependent ? ci32_dep(m_buffer) : mb_i32_le(m_buffer); }
		void TaggedDataReader::_GetEntryInt16(const Entry* e, int16& val) { FillBuffer(*e, sizeof(int16)); val = !m_endianIndependent ? ci16_dep(m_buffer) : mb_i16_le(m_buffer); }
		void TaggedDataReader::_GetEntryUInt64(const Entry* e, uint64& val) { FillBuffer(*e, sizeof(uint64)); val = !m_endianIndependent ? cui64_dep(m_buffer) : mb_u64_le(m_buffer); }
		void TaggedDataReader::_GetEntryUInt32(const Entry* e, uint32& val) { FillBuffer(*e, sizeof(uint32)); val = !m_endianIndependent ? cui32_dep(m_buffer) : mb_u32_le(m_buffer);}
		void TaggedDataReader::_GetEntryUInt16(const Entry* e, uint16& val) { FillBuffer(*e, sizeof(int16)); val = !m_endianIndependent ? ci16_dep(m_buffer) : mb_i16_le(m_buffer); }
		void TaggedDataReader::_GetEntryBool(const Entry* e, bool& val) { FillBuffer(*e, sizeof(bool)); val = !!m_buffer[0]; }
		void TaggedDataReader::_GetEntrySingle(const Entry* e, float& val) { FillBuffer(*e, sizeof(float)); val = !m_endianIndependent ? cr32_dep(m_buffer) : mb_f32_le(m_buffer); }
		void TaggedDataReader::_GetEntryDouble(const Entry* e, double& val) { FillBuffer(*e, sizeof(double)); val = !m_endianIndependent ? cr64_dep(m_buffer) : mb_f64_le(m_buffer); }
#endif
		void TaggedDataReader::_GetEntryBool(const Entry* ent, bool* val, int32 len)
		{
			m_stream->setPosition(ent->Offset);
			while (len >= sizeof(m_buffer))
			{
				FillBufferCurrent(sizeof(m_buffer));
				for (char c : m_buffer)
					*val++ = c != 0;
				len -= sizeof(m_buffer);
			}
			if (len > 0)
			{
				FillBufferCurrent(len);
				for (int32 i = 0; i < len; i++)
					*val++ = m_buffer[i] != 0;
			}
		}
		void TaggedDataReader::_GetEntryInt64(const Entry* ent, int64* val, int32 len)
		{
			const int32 Chunk = 4;
			m_stream->setPosition(ent->Offset);
#ifdef BIG_ENDIAN
			if (!m_endianIndependent)
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
				while (len >= Chunk)
				{
					FillBufferCurrent(sizeof(int64) * Chunk);
					for (int i = 0; i < Chunk; i++)
						*val++ = mb_i64_le(m_buffer + i * sizeof(int64));
					len -= Chunk;
				}
				if (len > 0)
				{
					FillBufferCurrent(sizeof(int64) * len);
					for (int i = 0; i < len; i++)
						*val++ = mb_i64_le(m_buffer + i * sizeof(int64));
				}
			}
		}
		void TaggedDataReader::_GetEntryInt32(const Entry* ent, int32* val, int32 len)
		{
			const int32 Chunk = 8;
			m_stream->setPosition(ent->Offset);
#ifdef BIG_ENDIAN
			if (!m_endianIndependent)
			{
				while (len>=Chunk)
				{
					FillBufferCurrent(sizeof(int32) * Chunk);
					for (int i=0;i<Chunk;i++) 
						*val++ = ci32_dep(m_buffer + i * sizeof(int32));
					len -= Chunk;
				}
				if (len > 0)
				{
					FillBufferCurrent(sizeof(int32) * len);
					for (int i=0;i<len;i++)
						*val++ = ci32_dep(m_buffer + i * sizeof(int32));
				}
			}
			else
#endif
			{
				while (len >= Chunk)
				{
					FillBufferCurrent(sizeof(int32) * Chunk);
					for (int i = 0; i < Chunk; i++)
						*val++ = mb_i32_le(m_buffer + i * sizeof(int32));
					len -= Chunk;
				}
				if (len > 0)
				{
					FillBufferCurrent(sizeof(int32) * len);
					for (int i = 0; i < len; i++)
						*val++ = mb_i32_le(m_buffer + i * sizeof(int32));
				}
			}
		}
		void TaggedDataReader::_GetEntryInt16(const Entry* ent, int16* val, int32 len)
		{
			const int32 Chunk = 16;
			m_stream->setPosition(ent->Offset);
#ifdef BIG_ENDIAN
			if (!m_endianIndependent)
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
				while (len >= Chunk)
				{
					FillBufferCurrent(sizeof(int16) * Chunk);
					for (int i = 0; i < Chunk; i++)
						*val++ = mb_i16_le(m_buffer + i * sizeof(int16));
					len -= Chunk;
				}
				if (len > 0)
				{
					FillBufferCurrent(sizeof(int16) * len);
					for (int i = 0; i < len; i++)
						*val++ = mb_i16_le(m_buffer + i * sizeof(int16));
				}
			}
		}
		
		void TaggedDataReader::_GetEntryUInt64(const Entry* ent, uint64* val, int32 len)
		{
			const int32 Chunk = 4;
			m_stream->setPosition(ent->Offset);
#ifdef BIG_ENDIAN
			if (!m_endianIndependent)
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
				while (len >= Chunk)
				{
					FillBufferCurrent(sizeof(uint64) * Chunk);
					for (int i = 0; i < Chunk; i++)
						*val++ = mb_u64_le(m_buffer + i * sizeof(uint64));
					len -= Chunk;
				}
				if (len > 0)
				{
					FillBufferCurrent(sizeof(uint64) * len);
					for (int i = 0; i < len; i++)
						*val++ = mb_u64_le(m_buffer + i * sizeof(uint64));
				}
			}
		}
		void TaggedDataReader::_GetEntryUInt32(const Entry* ent, uint32* val, int32 len)
		{
			const int32 Chunk = 8;
			m_stream->setPosition(ent->Offset);
#ifdef BIG_ENDIAN
			if (!m_endianIndependent)
			{
				while (len >= Chunk)
				{
					FillBufferCurrent(sizeof(uint32) * Chunk);
					for (int i = 0; i < Chunk; i++) *val++ = cui32_dep(m_buffer + i * sizeof(uint32));
					len -= Chunk;
				}
				if (len > 0)
				{
					FillBufferCurrent(sizeof(uint32) * len);
					for (int i = 0; i < len; i++) *val++ = cui32_dep(m_buffer + i * sizeof(uint32));
				}
			}
			else
#endif
			{
				while (len >= Chunk)
				{
					FillBufferCurrent(sizeof(uint32) * Chunk);
					for (int i = 0; i < Chunk; i++)
						*val++ = mb_u32_le(m_buffer + i * sizeof(uint32));
					len -= Chunk;
				}
				if (len > 0)
				{
					FillBufferCurrent(sizeof(uint32) * len);
					for (int i = 0; i < len; i++)
						*val++ = mb_u32_le(m_buffer + i * sizeof(uint32));
				}
			}
		}
		void TaggedDataReader::_GetEntryUInt16(const Entry* ent, uint16* val, int32 len)
		{
			const int32 Chunk = 16;
			m_stream->setPosition(ent->Offset);
#ifdef BIG_ENDIAN
			if (!m_endianIndependent)
			{
				while (len >= Chunk)
				{
					FillBufferCurrent(sizeof(uint16) * Chunk);
					for (int i = 0; i < Chunk; i++) *val++ = cui16_dep(m_buffer + i * sizeof(uint16));
					len -= Chunk;
				}
				if (len > 0)
				{
					FillBufferCurrent(sizeof(uint16) * len);
					for (int i = 0; i < len; i++) *val++ = cui16_dep(m_buffer + i * sizeof(uint16));
				}
			}
			else
#endif
			{
				while (len >= Chunk)
				{
					FillBufferCurrent(sizeof(uint16) * Chunk);
					for (int i = 0; i < Chunk; i++) 
						*val++ = mb_u16_le(m_buffer + i * sizeof(uint16));
					len -= Chunk;
				}
				if (len > 0)
				{
					FillBufferCurrent(sizeof(uint16) * len);
					for (int i = 0; i < len; i++) 
						*val++ = mb_u16_le(m_buffer + i * sizeof(uint16));
				}
			}
		}

		void TaggedDataReader::_GetEntrySingle(const Entry* ent, float* val, int32 len)
		{
			const int32 Chunk = 8;
			m_stream->setPosition(ent->Offset);
#ifdef BIG_ENDIAN
			if (!m_endianIndependent)
			{
				while (len>=Chunk)
				{
					FillBufferCurrent(sizeof(float) * Chunk);
					for (int i=0;i<Chunk;i++)
						*val++ = cr32_dep(m_buffer + i * sizeof(float));
					len -= Chunk;
				}
				if (len > 0)
				{
					FillBufferCurrent(sizeof(float) * len);
					for (int i=0;i<len;i++)
						*val++ = cr32_dep(m_buffer + i * sizeof(float));
				}
			}
			else
#endif
			{
				while (len >= Chunk)
				{
					FillBufferCurrent(sizeof(float) * Chunk);
					for (int i = 0; i < Chunk; i++)
						*val++ = mb_f32_le(m_buffer + i * sizeof(float));
					len -= Chunk;
				}
				if (len > 0)
				{
					FillBufferCurrent(sizeof(float) * len);
					for (int i = 0; i < len; i++)
						*val++ = mb_f32_le(m_buffer + i * sizeof(float));
				}
			}
		}
		void TaggedDataReader::_GetEntryDouble(const Entry* ent, double* val, int32 len)
		{
			const int32 Chunk = 4;
			m_stream->setPosition(ent->Offset);
#ifdef BIG_ENDIAN
			if (!m_endianIndependent)
			{
				while (len>=Chunk)
				{
					FillBufferCurrent(sizeof(double) * Chunk);
					for (int i=0;i<Chunk;i++) 
						*val++ = cr64_dep(m_buffer + i * sizeof(double));
					len -= Chunk;
				}
				if (len > 0)
				{
					FillBufferCurrent(sizeof(double) * len);
					for (int i=0;i<len;i++) 
						*val++ = cr64_dep(m_buffer + i * sizeof(double));
				}
			}
			else
#endif
			{
				while (len >= Chunk)
				{
					FillBufferCurrent(sizeof(double) * Chunk);
					for (int i = 0; i < Chunk; i++)
						*val++ = mb_f64_le(m_buffer + i * sizeof(double));
					len -= Chunk;
				}
				if (len > 0)
				{
					FillBufferCurrent(sizeof(double) * len);
					for (int i = 0; i < len; i++)
						*val++ = mb_f64_le(m_buffer + i * sizeof(double));
				}
			}
		}

		void TaggedDataReader::_GetEntryVector2(const Entry* ent, Vector2& vec)
		{
			FillBuffer(*ent, sizeof(float)*2);
#ifdef BIG_ENDIAN
			if (!m_endianIndependent)
			{
				vec.X = cr32_dep(m_buffer);
				vec.Y = cr32_dep(m_buffer + sizeof(float));
			}
			else
#endif
			{
				vec.X = mb_f32_le(m_buffer);
				vec.Y = mb_f32_le(m_buffer + sizeof(float));
			}
		}
		void TaggedDataReader::_GetEntryVector3(const Entry* ent, Vector3& vec)
		{
			FillBuffer(*ent, sizeof(float)*3);
#ifdef BIG_ENDIAN
			if (!m_endianIndependent)
			{
				vec.X = cr32_dep(m_buffer);
				vec.Y = cr32_dep(m_buffer + sizeof(float));
				vec.Z = cr32_dep(m_buffer + sizeof(float) * 2);
			}
			else
#endif
			{
				vec.X = mb_f32_le(m_buffer);
				vec.Y = mb_f32_le(m_buffer + sizeof(float));
				vec.Z = mb_f32_le(m_buffer + sizeof(float) * 2);
			}
		}
		void TaggedDataReader::_GetEntryVector4(const Entry* ent, Vector4& vec)
		{
			FillBuffer(*ent, sizeof(float)*4);
#ifdef BIG_ENDIAN
			if (!m_endianIndependent)
			{
				vec.X = cr32_dep(m_buffer);
				vec.Y = cr32_dep(m_buffer + sizeof(float));
				vec.Z = cr32_dep(m_buffer + sizeof(float) * 2);
				vec.W = cr32_dep(m_buffer + sizeof(float) * 3);
			}
			else
#endif
			{
				vec.X = mb_f32_le(m_buffer);
				vec.Y = mb_f32_le(m_buffer + sizeof(float));
				vec.Z = mb_f32_le(m_buffer + sizeof(float) * 2);
				vec.W = mb_f32_le(m_buffer + sizeof(float) * 3);
			}
		}
		void TaggedDataReader::_GetEntryColor4(const Entry* ent, Color4& clr)
		{
			FillBuffer(*ent, sizeof(float)*4);
#ifdef BIG_ENDIAN
			if (!m_endianIndependent)
			{
				clr.Red = cr32_dep(m_buffer);
				clr.Green = cr32_dep(m_buffer + sizeof(float));
				clr.Blue = cr32_dep(m_buffer + sizeof(float) * 2);
				clr.Alpha = cr32_dep(m_buffer + sizeof(float) * 3);
			}
			else
#endif
			{
				clr.Red = mb_f32_le(m_buffer);
				clr.Green = mb_f32_le(m_buffer + sizeof(float));
				clr.Blue = mb_f32_le(m_buffer + sizeof(float) * 2);
				clr.Alpha = mb_f32_le(m_buffer + sizeof(float) * 3);
			}
		}
		void TaggedDataReader::_GetEntryMatrix(const Entry* ent, Matrix& mat)
		{
#ifdef BIG_ENDIAN
			if (!m_endianIndependent)
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
				mat.M11 = mb_f32_le(m_buffer);
				mat.M12 = mb_f32_le(m_buffer + sizeof(float));
				mat.M13 = mb_f32_le(m_buffer + sizeof(float) * 2);
				mat.M14 = mb_f32_le(m_buffer + sizeof(float) * 3);
				mat.M21 = mb_f32_le(m_buffer + sizeof(float) * 4);
				mat.M22 = mb_f32_le(m_buffer + sizeof(float) * 5);
				mat.M23 = mb_f32_le(m_buffer + sizeof(float) * 6);
				mat.M24 = mb_f32_le(m_buffer + sizeof(float) * 7);

				FillBufferCurrent(sizeof(float)*8);
				mat.M31 = mb_f32_le(m_buffer);
				mat.M32 = mb_f32_le(m_buffer + sizeof(float));
				mat.M33 = mb_f32_le(m_buffer + sizeof(float) * 2);
				mat.M34 = mb_f32_le(m_buffer + sizeof(float) * 3);
				mat.M41 = mb_f32_le(m_buffer + sizeof(float) * 4);
				mat.M42 = mb_f32_le(m_buffer + sizeof(float) * 5);
				mat.M43 = mb_f32_le(m_buffer + sizeof(float) * 6);
				mat.M44 = mb_f32_le(m_buffer + sizeof(float) * 7);
			}
		}
		void TaggedDataReader::_GetEntryString(const Entry* ent, String& str)
		{
			VirtualStream vStrm(m_stream, ent->Offset, ent->Size);
			BinaryReader br(&vStrm, false);

			str = br.ReadString();
		}
		void TaggedDataReader::_GetEntryPlane(const Entry* ent, Plane& plane)
		{
			FillBuffer(*ent, sizeof(float)*4);
#ifdef BIG_ENDIAN
			if (!m_endianIndependent)
			{
				plane.X = cr32_dep(m_buffer);
				plane.Y = cr32_dep(m_buffer + sizeof(float));
				plane.Z = cr32_dep(m_buffer + sizeof(float) * 2);
				plane.D = cr32_dep(m_buffer + sizeof(float) * 3);
			}
			else
#endif
			{
				plane.X = mb_f32_le(m_buffer);
				plane.Y = mb_f32_le(m_buffer + sizeof(float));
				plane.Z = mb_f32_le(m_buffer + sizeof(float) * 2);
				plane.D = mb_f32_le(m_buffer + sizeof(float) * 3);
			}
		}
		void TaggedDataReader::_GetEntryQuaternion(const Entry* e, Quaternion& quat)
		{
			FillBuffer(*e, sizeof(float)*4);
#ifdef BIG_ENDIAN
			if (!m_endianIndependent)
			{
				quat.X = cr32_dep(m_buffer);
				quat.Y = cr32_dep(m_buffer + sizeof(float));
				quat.Z = cr32_dep(m_buffer + sizeof(float) * 2);
				quat.W = cr32_dep(m_buffer + sizeof(float) * 3);
			}
			else
#endif
			{
				quat.X = mb_f32_le(m_buffer);
				quat.Y = mb_f32_le(m_buffer + sizeof(float));
				quat.Z = mb_f32_le(m_buffer + sizeof(float) * 2);
				quat.W = mb_f32_le(m_buffer + sizeof(float) * 3);
			}
		}
		void TaggedDataReader::_GetEntryRectangle(const Entry* e, Rectangle& rect)
		{
			FillBuffer(*e, sizeof(int32)*4);
#ifdef BIG_ENDIAN
			if (!m_endianIndependent)
			{
				rect.X = ci32_dep(m_buffer);
				rect.Y = ci32_dep(m_buffer + sizeof(int32));
				rect.Width = ci32_dep(m_buffer + sizeof(int32)*2);
				rect.Height = ci32_dep(m_buffer + sizeof(int32)*3);
			}
			else
#endif
			{
				rect.X = mb_i32_le(m_buffer);
				rect.Y = mb_i32_le(m_buffer + sizeof(int32));
				rect.Width = mb_i32_le(m_buffer + sizeof(int32)*2);
				rect.Height = mb_i32_le(m_buffer + sizeof(int32)*3);
			}
		}
		void TaggedDataReader::_GetEntryRectangleF(const Entry* e, RectangleF& rect)
		{
			FillBuffer(*e, sizeof(float)*4);
#ifdef BIG_ENDIAN
			if (!m_endianIndependent)
			{
				rect.X = cr32_dep(m_buffer);
				rect.Y = cr32_dep(m_buffer + sizeof(float));
				rect.Width = cr32_dep(m_buffer + sizeof(float)*2);
				rect.Height = cr32_dep(m_buffer + sizeof(float)*3);
			}
			else
#endif
			{
				rect.X = mb_f32_le(m_buffer);
				rect.Y = mb_f32_le(m_buffer + sizeof(float));
				rect.Width = mb_f32_le(m_buffer + sizeof(float)*2);
				rect.Height = mb_f32_le(m_buffer + sizeof(float)*3);
			}
		}
		void TaggedDataReader::_GetEntryPoint(const Entry* e, Point& pt)
		{
			FillBuffer(*e, sizeof(int32)*2);
#ifdef BIG_ENDIAN
			if (!m_endianIndependent)
			{
				pt.X = ci32_dep(m_buffer);
				pt.Y = ci32_dep(m_buffer + sizeof(int32));
			}
			else
#endif
			{
				pt.X = mb_i32_le(m_buffer);
				pt.Y = mb_i32_le(m_buffer + sizeof(int32));
			}
		}
		void TaggedDataReader::_GetEntryPointF(const Entry* e, PointF& pt)
		{
			FillBuffer(*e, sizeof(float)*2);
#ifdef BIG_ENDIAN
			if (!m_endianIndependent)
			{
				pt.X = cr32_dep(m_buffer);
				pt.Y = cr32_dep(m_buffer + sizeof(float));
			}
			else
#endif
			{
				pt.X = mb_f32_le(m_buffer);
				pt.Y = mb_f32_le(m_buffer + sizeof(float));
			}
		}
		void TaggedDataReader::_GetEntryMathSize(const Entry* e, Apoc3D::Math::Size& sz)
		{
			FillBuffer(*e, sizeof(int32)*2);
#ifdef BIG_ENDIAN
			if (!m_endianIndependent)
			{
				sz.Width = ci32_dep(m_buffer);
				sz.Height = ci32_dep(m_buffer + sizeof(int32));
			}
			else
#endif
			{
				sz.Width = mb_i32_le(m_buffer);
				sz.Height = mb_i32_le(m_buffer + sizeof(int32));
			}
			}
		void TaggedDataReader::_GetEntryBoundingBox(const Entry* e, BoundingBox& bb)
		{
#ifdef BIG_ENDIAN
			if (!m_endianIndependent)
			{
				FillBuffer(*e, sizeof(float) * 6);
				bb.Minimum.X = cr32_dep(m_buffer);
				bb.Minimum.Y = cr32_dep(m_buffer + sizeof(float));
				bb.Minimum.Z = cr32_dep(m_buffer + sizeof(float) * 2);

				bb.Maximum.X = cr32_dep(m_buffer + sizeof(float) * 3);
				bb.Maximum.Y = cr32_dep(m_buffer + sizeof(float) * 4);
				bb.Maximum.Z = cr32_dep(m_buffer + sizeof(float) * 5);
			}
			else
#endif
			{
				FillBuffer(*e, sizeof(float) * 6);
				bb.Minimum.X = mb_f32_le(m_buffer);
				bb.Minimum.Y = mb_f32_le(m_buffer + sizeof(float));
				bb.Minimum.Z = mb_f32_le(m_buffer + sizeof(float) * 2);

				bb.Maximum.X = mb_f32_le(m_buffer + sizeof(float) * 3);
				bb.Maximum.Y = mb_f32_le(m_buffer + sizeof(float) * 4);
				bb.Maximum.Z = mb_f32_le(m_buffer + sizeof(float) * 5);
			}
		}
		void TaggedDataReader::_GetEntryBoundingSphere(const Entry* e, BoundingSphere& bs)
		{
#ifdef BIG_ENDIAN
			if (!m_endianIndependent)
			{
				FillBuffer(*e, sizeof(float)*4);
				bs.Center.X = cr32_dep(m_buffer);
				bs.Center.Y = cr32_dep(m_buffer + sizeof(float));
				bs.Center.Z = cr32_dep(m_buffer + sizeof(float) * 2);
				bs.Radius = cr32_dep(m_buffer + sizeof(float) * 3);
			}
			else
#endif
			{
				FillBuffer(*e, sizeof(float) * 4);
				bs.Center.X = mb_f32_le(m_buffer);
				bs.Center.Y = mb_f32_le(m_buffer + sizeof(float));
				bs.Center.Z = mb_f32_le(m_buffer + sizeof(float) * 2);
				bs.Radius = mb_f32_le(m_buffer + sizeof(float) * 3);
			}
		}
		void TaggedDataReader::_GetEntryRay(const Entry* e, Ray& r)
		{
#ifdef BIG_ENDIAN
			if (!m_endianIndependent)
			{
				FillBuffer(*e, sizeof(float) * 6);
				r.Position.X = cr32_dep(m_buffer);
				r.Position.Y = cr32_dep(m_buffer + sizeof(float));
				r.Position.Z = cr32_dep(m_buffer + sizeof(float) * 2);

				r.Direction.X = cr32_dep(m_buffer + sizeof(float) * 3);
				r.Direction.Y = cr32_dep(m_buffer + sizeof(float) * 4);
				r.Direction.Z = cr32_dep(m_buffer + sizeof(float) * 5);
			}
			else
#endif
			{
				FillBuffer(*e, sizeof(float) * 6);
				r.Position.X = mb_f32_le(m_buffer);
				r.Position.Y = mb_f32_le(m_buffer + sizeof(float));
				r.Position.Z = mb_f32_le(m_buffer + sizeof(float) * 2);

				r.Direction.X = mb_f32_le(m_buffer + sizeof(float) * 3);
				r.Direction.Y = mb_f32_le(m_buffer + sizeof(float) * 4);
				r.Direction.Z = mb_f32_le(m_buffer + sizeof(float) * 5);
			}
		}
		void TaggedDataReader::_GetEntryViewport(const Entry* e, Viewport& vp)
		{
#ifdef BIG_ENDIAN
			if (!m_endianIndependent)
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
				FillBuffer(*e, sizeof(int32) * 4);
				vp.X = mb_i32_le(m_buffer);
				vp.Y = mb_i32_le(m_buffer + sizeof(int32));
				vp.Width = mb_i32_le(m_buffer + sizeof(int32) * 2);
				vp.Height = mb_i32_le(m_buffer + sizeof(int32) * 3);

				FillBufferCurrent(sizeof(float) * 2);
				vp.MinZ = mb_f32_le(m_buffer);
				vp.MaxZ = mb_f32_le(m_buffer + sizeof(float));
			}
		}

		void TaggedDataReader::_GetEntryVector2(const Entry* ent, Vector2* value, int len)
		{
			if (len>0) _GetEntryVector2(ent, value[0]);
#ifdef BIG_ENDIAN
			if (!m_endianIndependent)
			{
				for (int i=1;i<len;i++)
				{
					FillBufferCurrent(sizeof(float)*2);
					value[i].X = cr32_dep(m_buffer);
					value[i].Y = cr32_dep(m_buffer + sizeof(float));
				}
			}
			else
#endif
			{
				for (int i = 1; i < len; i++)
				{
					FillBufferCurrent(sizeof(float) * 2);
					value[i].X = mb_f32_le(m_buffer);
					value[i].Y = mb_f32_le(m_buffer + sizeof(float));
				}
			}
		}
		void TaggedDataReader::_GetEntryVector3(const Entry* ent, Vector3* value, int len)
		{
			if (len>0) _GetEntryVector3(ent, value[0]);
#ifdef BIG_ENDIAN
			if (!m_endianIndependent)
			{
				for (int i=1;i<len;i++)
				{
					Vector3& vec = value[i];
					FillBufferCurrent(sizeof(float)*3);
					vec.X = cr32_dep(m_buffer);
					vec.Y = cr32_dep(m_buffer + sizeof(float));
					vec.Z = cr32_dep(m_buffer + sizeof(float) * 2);
				}
			}
			else
#endif
			{
				for (int i = 1; i < len; i++)
				{
					Vector3& vec = value[i];
					FillBufferCurrent(sizeof(float) * 3);
					vec.X = mb_f32_le(m_buffer);
					vec.Y = mb_f32_le(m_buffer + sizeof(float));
					vec.Z = mb_f32_le(m_buffer + sizeof(float) * 2);
				}
			}
		}
		void TaggedDataReader::_GetEntryVector4(const Entry* ent, Vector4* value, int len)
		{
			if (len>0) _GetEntryVector4(ent, value[0]);
#ifdef BIG_ENDIAN
			if (!m_endianIndependent)
			{
				for (int i=1;i<len;i++)
				{
					Vector4& vec = value[i];
					FillBufferCurrent(sizeof(float)*4);
					vec.X = cr32_dep(m_buffer);
					vec.Y = cr32_dep(m_buffer + sizeof(float));
					vec.Z = cr32_dep(m_buffer + sizeof(float) * 2);
					vec.W = cr32_dep(m_buffer + sizeof(float) * 3);
				}
			}
			else
#endif
			{
				for (int i = 1; i < len; i++)
				{
					Vector4& vec = value[i];
					FillBufferCurrent(sizeof(float) * 4);
					vec.X = mb_f32_le(m_buffer);
					vec.Y = mb_f32_le(m_buffer + sizeof(float));
					vec.Z = mb_f32_le(m_buffer + sizeof(float) * 2);
					vec.W = mb_f32_le(m_buffer + sizeof(float) * 3);
				}
			}
		}
		void TaggedDataReader::_GetEntryMatrix(const Entry* ent, Matrix* value, int len)
		{
			if (len>0) _GetEntryMatrix(ent, value[0]);
#ifdef BIG_ENDIAN
			if (!m_endianIndependent)
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
				for (int i = 1; i < len; i++)
				{
					Matrix& mat = value[i];

					FillBufferCurrent(sizeof(float) * 8);
					mat.M11 = mb_f32_le(m_buffer);
					mat.M12 = mb_f32_le(m_buffer + sizeof(float));
					mat.M13 = mb_f32_le(m_buffer + sizeof(float) * 2);
					mat.M14 = mb_f32_le(m_buffer + sizeof(float) * 3);
					mat.M21 = mb_f32_le(m_buffer + sizeof(float) * 4);
					mat.M22 = mb_f32_le(m_buffer + sizeof(float) * 5);
					mat.M23 = mb_f32_le(m_buffer + sizeof(float) * 6);
					mat.M24 = mb_f32_le(m_buffer + sizeof(float) * 7);

					FillBufferCurrent(sizeof(float) * 8);
					mat.M31 = mb_f32_le(m_buffer);
					mat.M32 = mb_f32_le(m_buffer + sizeof(float));
					mat.M33 = mb_f32_le(m_buffer + sizeof(float) * 2);
					mat.M34 = mb_f32_le(m_buffer + sizeof(float) * 3);
					mat.M41 = mb_f32_le(m_buffer + sizeof(float) * 4);
					mat.M42 = mb_f32_le(m_buffer + sizeof(float) * 5);
					mat.M43 = mb_f32_le(m_buffer + sizeof(float) * 6);
					mat.M44 = mb_f32_le(m_buffer + sizeof(float) * 7);
				}
			}
		}
		void TaggedDataReader::_GetEntryColor4(const Entry* ent, Color4* value, int len)
		{
			if (len>0) _GetEntryColor4(ent, value[0]);
#ifdef BIG_ENDIAN
			if (!m_endianIndependent)
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
				for (int i = 1; i < len; i++)
				{
					Color4& clr = value[i];
					FillBufferCurrent(sizeof(float) * 4);
					clr.Red = mb_f32_le(m_buffer);
					clr.Green = mb_f32_le(m_buffer + sizeof(float));
					clr.Blue = mb_f32_le(m_buffer + sizeof(float) * 2);
					clr.Alpha = mb_f32_le(m_buffer + sizeof(float) * 3);
				}
			}
		}
		void TaggedDataReader::_GetEntryString(const Entry* ent, String* value, int len)
		{
			VirtualStream vStrm(m_stream, ent->Offset, ent->Size);
			BinaryReader br(&vStrm, false);

			for (int32 i = 0; i < len; i++)
				value[i] = br.ReadString();
		}
		void TaggedDataReader::_GetEntryPlane(const Entry* ent, Plane* plane, int len)
		{
			if (len>0) _GetEntryPlane(ent, plane[0]);
#ifdef BIG_ENDIAN
			if (!m_endianIndependent)
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
				for (int i = 1; i < len; i++)
				{
					Plane& pl = plane[i];
					FillBufferCurrent(sizeof(float) * 4);
					pl.X = mb_f32_le(m_buffer);
					pl.Y = mb_f32_le(m_buffer + sizeof(float));
					pl.Z = mb_f32_le(m_buffer + sizeof(float) * 2);
					pl.D = mb_f32_le(m_buffer + sizeof(float) * 3);
				}
			}
		}
		void TaggedDataReader::_GetEntryQuaternion(const Entry* ent, Quaternion* quat, int len)
		{
			if (len>0) _GetEntryQuaternion(ent, quat[0]);
#ifdef BIG_ENDIAN
			if (!m_endianIndependent)
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
				for (int i = 1; i < len; i++)
				{
					Quaternion& q = quat[i];
					FillBufferCurrent(sizeof(float) * 4);
					q.X = mb_f32_le(m_buffer);
					q.Y = mb_f32_le(m_buffer + sizeof(float));
					q.Z = mb_f32_le(m_buffer + sizeof(float) * 2);
					q.W = mb_f32_le(m_buffer + sizeof(float) * 3);
				}
			}
		}
		void TaggedDataReader::_GetEntryRectangle(const Entry* ent, Rectangle* rect, int len)
		{
			if (len>0) _GetEntryRectangle(ent, rect[0]);
#ifdef BIG_ENDIAN
			if (!m_endianIndependent)
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
				for (int i = 1; i < len; i++)
				{
					Rectangle& r = rect[i];
					FillBufferCurrent(sizeof(int32) * 4);
					r.X = mb_i32_le(m_buffer);
					r.Y = mb_i32_le(m_buffer + sizeof(int32));
					r.Width = mb_i32_le(m_buffer + sizeof(int32) * 2);
					r.Height = mb_i32_le(m_buffer + sizeof(int32) * 3);
				}
			}
		}
		void TaggedDataReader::_GetEntryRectangleF(const Entry* ent, RectangleF* rect, int len)
		{
			if (len>0) _GetEntryRectangleF(ent, rect[0]);
#ifdef BIG_ENDIAN
			if (!m_endianIndependent)
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
				for (int i = 1; i < len; i++)
				{
					RectangleF& r = rect[i];
					FillBufferCurrent(sizeof(float) * 4);
					r.X = mb_f32_le(m_buffer);
					r.Y = mb_f32_le(m_buffer + sizeof(float));
					r.Width = mb_f32_le(m_buffer + sizeof(float) * 2);
					r.Height = mb_f32_le(m_buffer + sizeof(float) * 3);
				}
			}
		}
		void TaggedDataReader::_GetEntryPoint(const Entry* ent, Point* pt, int len)
		{
			if (len>0) _GetEntryPoint(ent, pt[0]);
#ifdef BIG_ENDIAN
			if (!m_endianIndependent)
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
				for (int i = 1; i < len; i++)
				{
					Point& p = pt[i];
					FillBufferCurrent(sizeof(int32) * 2);
					p.X = mb_i32_le(m_buffer);
					p.Y = mb_i32_le(m_buffer + sizeof(int32));
				}
			}
		}
		void TaggedDataReader::_GetEntryPointF(const Entry* ent, PointF* pt, int len)
		{
			if (len>0) _GetEntryPointF(ent, pt[0]);
#ifdef BIG_ENDIAN
			if (!m_endianIndependent)
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
				for (int i = 1; i < len; i++)
				{
					PointF& p = pt[i];
					FillBufferCurrent(sizeof(float) * 2);
					p.X = mb_f32_le(m_buffer);
					p.Y = mb_f32_le(m_buffer + sizeof(float));
				}
			}
		}
		void TaggedDataReader::_GetEntryMathSize(const Entry* ent, Apoc3D::Math::Size* sz, int len)
		{
			if (len>0) _GetEntryMathSize(ent, sz[0]);
#ifdef BIG_ENDIAN
			if (!m_endianIndependent)
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
				for (int i = 1; i < len; i++)
				{
					Apoc3D::Math::Size& s = sz[i];
					FillBufferCurrent(sizeof(float) * 2);
					s.Width = mb_i32_le(m_buffer);
					s.Height = mb_i32_le(m_buffer + sizeof(int32));
				}
			}
		}
		void TaggedDataReader::_GetEntryBoundingBox(const Entry* ent, BoundingBox* bb, int len)
		{
			if (len>0) _GetEntryBoundingBox(ent, bb[0]);
#ifdef BIG_ENDIAN
			if (!m_endianIndependent)
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
				for (int i = 1; i < len; i++)
				{
					BoundingBox& b = bb[i];
					FillBufferCurrent(sizeof(float) * 3);
					b.Minimum.X = mb_f32_le(m_buffer);
					b.Minimum.Y = mb_f32_le(m_buffer + sizeof(float));
					b.Minimum.Z = mb_f32_le(m_buffer + sizeof(float) * 2);

					FillBufferCurrent(sizeof(float) * 3);
					b.Maximum.X = mb_f32_le(m_buffer);
					b.Maximum.Y = mb_f32_le(m_buffer + sizeof(float));
					b.Maximum.Z = mb_f32_le(m_buffer + sizeof(float) * 2);
				}
			}
		}
		void TaggedDataReader::_GetEntryBoundingSphere(const Entry* ent, BoundingSphere* bs, int len)
		{
			if (len>0) _GetEntryBoundingSphere(ent, bs[0]);
#ifdef BIG_ENDIAN
			if (!m_endianIndependent)
			{
				for (int i=1;i<len;i++)
				{
					BoundingSphere& b = bs[i];
					FillBufferCurrent(sizeof(float)*4);
					b.Center.X = cr32_dep(m_buffer);
					b.Center.Y = cr32_dep(m_buffer + sizeof(float));
					b.Center.Z = cr32_dep(m_buffer + sizeof(float) * 2);
					b.Radius = cr32_dep(m_buffer + sizeof(float) * 3);
				}
			}
			else
#endif
			{
				for (int i = 1; i < len; i++)
				{
					BoundingSphere& b = bs[i];
					FillBufferCurrent(sizeof(float) * 4);
					b.Center.X = mb_f32_le(m_buffer);
					b.Center.Y = mb_f32_le(m_buffer + sizeof(float));
					b.Center.Z = mb_f32_le(m_buffer + sizeof(float) * 2);
					b.Radius = mb_f32_le(m_buffer + sizeof(float) * 3);
				}
			}
		}
		void TaggedDataReader::_GetEntryRay(const Entry* ent, Ray* r, int len)
		{
			if (len>0) _GetEntryRay(ent, r[0]);
#ifdef BIG_ENDIAN
			if (!m_endianIndependent)
			{
				for (int i=1;i<len;i++)
				{
					Ray& cr = r[i];
					FillBufferCurrent( sizeof(float)*6);
					cr.Position.X = cr32_dep(m_buffer);
					cr.Position.Y = cr32_dep(m_buffer + sizeof(float));
					cr.Position.Z = cr32_dep(m_buffer + sizeof(float) * 2);

					cr.Direction.X = cr32_dep(m_buffer + sizeof(float) * 3);
					cr.Direction.Y = cr32_dep(m_buffer + sizeof(float) * 4);
					cr.Direction.Z = cr32_dep(m_buffer + sizeof(float) * 5);
				}
			}
			else
#endif
			{
				for (int i = 1; i < len; i++)
				{
					Ray& cr = r[i];
					FillBufferCurrent(sizeof(float) * 3);
					cr.Position.X = mb_f32_le(m_buffer);
					cr.Position.Y = mb_f32_le(m_buffer + sizeof(float));
					cr.Position.Z = mb_f32_le(m_buffer + sizeof(float) * 2);

					cr.Direction.X = mb_f32_le(m_buffer + sizeof(float) * 3);
					cr.Direction.Y = mb_f32_le(m_buffer + sizeof(float) * 4);
					cr.Direction.Z = mb_f32_le(m_buffer + sizeof(float) * 5);
				}
			}
		}
		void TaggedDataReader::_GetEntryViewport(const Entry* ent, Viewport* vp, int len)
		{
			if (len>0) _GetEntryViewport(ent, vp[0]);
#ifdef BIG_ENDIAN
			if (!m_endianIndependent)
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
				for (int i = 1; i < len; i++)
				{
					Viewport& v = vp[i];
					FillBufferCurrent(sizeof(int32) * 4);
					v.X = mb_i32_le(m_buffer);
					v.Y = mb_i32_le(m_buffer + sizeof(int32));
					v.Width = mb_i32_le(m_buffer + sizeof(int32) * 2);
					v.Height = mb_i32_le(m_buffer + sizeof(int32) * 3);

					FillBufferCurrent(sizeof(float) * 2);
					v.MinZ = mb_f32_le(m_buffer);
					v.MaxZ = mb_f32_le(m_buffer + sizeof(float));
				}
			}
		}

		void TaggedDataReader::KeynotFoundError(const KeyType& name)
		{
			AP_EXCEPTION(ErrorID::KeyNotFound, StringUtils::UTF8toUTF16(name.getString()));
		}


		//////////////////////////////////////////////////////////////////////////
		//////////////////////////////////////////////////////////////////////////
		//////////////////////////////////////////////////////////////////////////

		/************************************************************************/
		/* TaggedDataWriter                                                     */
		/************************************************************************/
		
		TaggedDataWriter::TaggedDataWriter(bool isEndianIndependent)
			: m_endianIndependent(isEndianIndependent)
		{ }

		TaggedDataWriter::~TaggedDataWriter()
		{
			for (Entry& ent : m_positions.getValueAccessor())
			{
				delete ent.Buffer;
			}
			m_positions.Clear();
		}

		TaggedDataWriter::Entry::Entry(const KeyType& name)
			: Name(name)
		{
			Buffer = new MemoryOutStream(1024);
		}

		void TaggedDataWriter::Entry::ResetWritePosition() const { Buffer->setPosition(0); }

		void TaggedDataWriter::Save(Stream& stream) const
		{
			int64 totalSize = 0;
			for (const Entry& e : m_positions.getValueAccessor())
			{
				totalSize += e.Buffer->getLength();
			}

			bool shouldUse64Bit = totalSize > 0xffffffffu;

			BinaryWriter bw(&stream, false);

			// always write in the latest format

			int64 startPos = stream.getPosition();

			bw.WriteUInt32(0x80000000U | TF_HashKeyFormat | (shouldUse64Bit ? TF_64Bit : 0));
			bw.WriteInt32(m_positions.getCount());

			for (const KeyType& key : m_positions.getKeyAccessor())
			{
				key.Write(bw);
			}

			const int64 offsetSize = shouldUse64Bit ? sizeof(int64) : sizeof(uint32);

			int64 baseOffset = stream.getPosition() - startPos + offsetSize * 2 * m_positions.getCount();
			int64 offset = 0;
			for (const Entry& e : m_positions.getValueAccessor())
			{
				MemoryOutStream* memBlock = e.Buffer;
				int64 blockSize = memBlock->getLength();

				if (shouldUse64Bit)
				{
					bw.WriteInt64(offset + baseOffset);
					bw.WriteInt64(blockSize);
				}
				else
				{
					bw.WriteUInt32((uint32)(offset + baseOffset));
					bw.WriteUInt32((uint32)blockSize);
				}

				offset += blockSize;
			}

			for (const Entry& e : m_positions.getValueAccessor())
			{
				MemoryOutStream* memBlock = e.Buffer;
				bw.WriteBytes(memBlock->getDataPointer(), memBlock->getLength());
			}

		}
		ConfigurationSection* TaggedDataWriter::MakeDigest(const KeyType& name) const
		{
			ConfigurationSection* sect = new ConfigurationSection(StringUtils::UTF8toUTF16(name.getString()));
			for (const Entry& e : m_positions.getValueAccessor())
			{
				MemoryOutStream* memBlock = e.Buffer;

				String text;

				if (memBlock->getLength()>8)
				{
					text = StringUtils::IntToString(memBlock->getLength()) + L" bytes";
				}
				else
				{
					const byte* data = reinterpret_cast<const byte*>(memBlock->getDataPointer());

					for (int64 i = 0; i < memBlock->getLength(); i++)
					{
						text.append(StringUtils::UIntToStringHex((uint32)data[i], StrFmt::a<2, '0'>::val));
					}
				}

				sect->AddStringValue(StringUtils::UTF8toUTF16(e.Name.getString()), text);
			}
			return sect;
		}


		BinaryWriter* TaggedDataWriter::AddEntry(const KeyType& name)
		{
			Entry ent = Entry(name);
			m_positions.Add(name, ent);
			return new BinaryWriter(new VirtualStream(ent.Buffer, 0), true);
		}
		Stream* TaggedDataWriter::AddEntryStream(const KeyType& name)
		{
			Entry ent = Entry(name);
			m_positions.Add(name, ent);
			return new VirtualStream(ent.Buffer, 0);
		}

		void TaggedDataWriter::AddEntry(const KeyType& name, FunctorReference<void(BinaryWriter*)> func)
		{
			Entry ent = Entry(name);
			m_positions.Add(name, ent);

			//VirtualStream vs(ent.Buffer, 0);
			{
				BinaryWriter bw(ent.Buffer, false);
				func(&bw);
			}
		}

		void TaggedDataWriter::AddEntryStream(const KeyType& name, FunctorReference<void(Stream*)> func)
		{
			Entry ent = Entry(name);
			m_positions.Add(name, ent);

			//VirtualStream vs(ent.Buffer, 0);
			func(ent.Buffer);
		}

		void TaggedDataWriter::AddEntryDataSection(const KeyType& name, FunctorReference<void(TaggedDataWriter*)> func)
		{
			AddEntry(name, [func](BinaryWriter* bw) 
			{
				bw->WriteTaggedDataBlock(func);
			});
		}


		bool TaggedDataWriter::Contains(const KeyType& name) const { return FindEntry(name) != nullptr; }

		BinaryWriter* TaggedDataWriter::GetData(const KeyType& name)
		{
			const Entry* ent = FindEntry(name);
			return new BinaryWriter(new VirtualStream(ent->Buffer, 0), true);
		}


		void TaggedDataWriter::AddAuto(const KeyType& name, int64 value)	{ AddInt64(name, value); }
		void TaggedDataWriter::AddAuto(const KeyType& name, int32 value)	{ AddInt32(name, value); }
		void TaggedDataWriter::AddAuto(const KeyType& name, int16 value)	{ AddInt16(name, value); }
		void TaggedDataWriter::AddAuto(const KeyType& name, uint64 value)	{ AddUInt64(name, value); }
		void TaggedDataWriter::AddAuto(const KeyType& name, uint32 value)	{ AddUInt32(name, value); }
		void TaggedDataWriter::AddAuto(const KeyType& name, uint16 value)	{ AddUInt16(name, value); }
		void TaggedDataWriter::AddAuto(const KeyType& name, float value)	{ AddSingle(name, value); }
		void TaggedDataWriter::AddAuto(const KeyType& name, double value)	{ AddDouble(name, value); }
		void TaggedDataWriter::AddAuto(const KeyType& name, bool value)		{ AddBool(name, value); }
		void TaggedDataWriter::AddAuto(const KeyType& name, const Vector2& vec)	{ AddVector2(name, vec); }
		void TaggedDataWriter::AddAuto(const KeyType& name, const Vector3& vec)	{ AddVector3(name, vec); }
		void TaggedDataWriter::AddAuto(const KeyType& name, const Vector4& vec)	{ AddVector4(name, vec); }
		void TaggedDataWriter::AddAuto(const KeyType& name, const Matrix& mat)	{ AddMatrix(name, mat); }
		void TaggedDataWriter::AddAuto(const KeyType& name, const Color4& clr)	{ AddColor4(name, clr); }
		void TaggedDataWriter::AddAuto(const KeyType& name, const Plane& plane)	{ AddPlane(name, plane); }
		void TaggedDataWriter::AddAuto(const KeyType& name, const Quaternion& quad)					{ AddQuaternion(name, quad); }
		void TaggedDataWriter::AddAuto(const KeyType& name, const String& str)						{ AddString(name, str); }
		void TaggedDataWriter::AddAuto(const KeyType& name, const Apoc3D::Math::Rectangle& rect)	{ AddRectangle(name, rect); }
		void TaggedDataWriter::AddAuto(const KeyType& name, const RectangleF& rect)					{ AddRectangleF(name, rect); }
		void TaggedDataWriter::AddAuto(const KeyType& name, const Point& pt)						{ AddPoint(name, pt); }
		void TaggedDataWriter::AddAuto(const KeyType& name, const PointF& pt)						{ AddPointF(name, pt); }
		void TaggedDataWriter::AddAuto(const KeyType& name, const Apoc3D::Math::Size& sz)			{ AddMathSize(name, sz); }
		void TaggedDataWriter::AddAuto(const KeyType& name, const BoundingBox& bb)					{ AddBoundingBox(name, bb); }
		void TaggedDataWriter::AddAuto(const KeyType& name, const BoundingSphere& bs)				{ AddBoundingSphere(name, bs); }
		void TaggedDataWriter::AddAuto(const KeyType& name, const Ray& r)							{ AddRay(name, r); }
		void TaggedDataWriter::AddAuto(const KeyType& name, const Viewport& vp)						{ AddViewport(name, vp); }

		void TaggedDataWriter::AddAuto(const KeyType& name, const int64* value, int32 count)		{ AddInt64(name, value, count); }
		void TaggedDataWriter::AddAuto(const KeyType& name, const int32* value, int32 count)		{ AddInt32(name, value, count); }
		void TaggedDataWriter::AddAuto(const KeyType& name, const int16* value, int32 count)		{ AddInt16(name, value, count); }
		void TaggedDataWriter::AddAuto(const KeyType& name, const uint64* value, int32 count)		{ AddUInt64(name, value, count); }
		void TaggedDataWriter::AddAuto(const KeyType& name, const uint32* value, int32 count)		{ AddUInt32(name, value, count); }
		void TaggedDataWriter::AddAuto(const KeyType& name, const uint16* value, int32 count)		{ AddUInt16(name, value, count); }
		void TaggedDataWriter::AddAuto(const KeyType& name, const float* value, int32 count)		{ AddSingle(name, value, count); }
		void TaggedDataWriter::AddAuto(const KeyType& name, const double* value, int32 count)		{ AddDouble(name, value, count); }
		void TaggedDataWriter::AddAuto(const KeyType& name, const bool* value, int32 count)			{ AddBool(name, value, count); }
		void TaggedDataWriter::AddAuto(const KeyType& name, const Vector2* vec, int32 count)		{ AddVector2(name, vec, count); }
		void TaggedDataWriter::AddAuto(const KeyType& name, const Vector3* vec, int32 count)		{ AddVector3(name, vec, count); }
		void TaggedDataWriter::AddAuto(const KeyType& name, const Vector4* vec, int32 count)		{ AddVector4(name, vec, count); }
		void TaggedDataWriter::AddAuto(const KeyType& name, const Matrix* mat, int32 count)			{ AddMatrix(name, mat, count); }
		void TaggedDataWriter::AddAuto(const KeyType& name, const Color4* clr, int32 count)			{ AddColor4(name, clr, count); }
		void TaggedDataWriter::AddAuto(const KeyType& name, const Plane* plane, int32 count)		{ AddPlane(name, plane, count); }
		void TaggedDataWriter::AddAuto(const KeyType& name, const Quaternion* quad, int32 count)	{ AddQuaternion(name, quad, count); }
		void TaggedDataWriter::AddAuto(const KeyType& name, const String* str, int32 count)			{ AddString(name, str, count); }
		void TaggedDataWriter::AddAuto(const KeyType& name, const Apoc3D::Math::Rectangle* rect, int32 count)	{ AddRectangle(name, rect, count); }
		void TaggedDataWriter::AddAuto(const KeyType& name, const RectangleF* rect, int32 count)				{ AddRectangleF(name, rect, count); }
		void TaggedDataWriter::AddAuto(const KeyType& name, const Point* pt, int32 count)						{ AddPoint(name, pt, count); }
		void TaggedDataWriter::AddAuto(const KeyType& name, const PointF* pt, int32 count)						{ AddPointF(name, pt, count); }
		void TaggedDataWriter::AddAuto(const KeyType& name, const Apoc3D::Math::Size* sz, int32 count)			{ AddMathSize(name, sz, count); }
		void TaggedDataWriter::AddAuto(const KeyType& name, const BoundingBox* bb, int32 count)					{ AddBoundingBox(name, bb, count); }
		void TaggedDataWriter::AddAuto(const KeyType& name, const BoundingSphere* bs, int32 count)				{ AddBoundingSphere(name, bs, count); }
		void TaggedDataWriter::AddAuto(const KeyType& name, const Ray* r, int32 count)							{ AddRay(name, r, count); }
		void TaggedDataWriter::AddAuto(const KeyType& name, const Viewport* vp, int32 count)					{ AddViewport(name, vp, count); }

#define TAGW_NEW_ENTRY(name, value, setvalue) Entry ent = Entry(name); m_positions.Add(name, ent); setvalue(ent, value);
#define TAGW_NEW_ENTRY_ARR(name, value, count, setvalue) Entry ent = Entry(name); m_positions.Add(name, ent); setvalue(ent, value, count);

		void TaggedDataWriter::AddInt64(const KeyType& name, int64 value)	{ TAGW_NEW_ENTRY(name, value, _SetEntryDataInt64); }
		void TaggedDataWriter::AddInt32(const KeyType& name, int32 value)	{ TAGW_NEW_ENTRY(name, value, _SetEntryDataInt32); }
		void TaggedDataWriter::AddInt16(const KeyType& name, int16 value)	{ TAGW_NEW_ENTRY(name, value, _SetEntryDataInt16); }
		void TaggedDataWriter::AddUInt64(const KeyType& name, uint64 value)	{ TAGW_NEW_ENTRY(name, value, _SetEntryDataUInt64); }
		void TaggedDataWriter::AddUInt32(const KeyType& name, uint32 value)	{ TAGW_NEW_ENTRY(name, value, _SetEntryDataUInt32); }
		void TaggedDataWriter::AddUInt16(const KeyType& name, uint16 value)	{ TAGW_NEW_ENTRY(name, value, _SetEntryDataUInt16); }
		void TaggedDataWriter::AddSingle(const KeyType& name, float value)	{ TAGW_NEW_ENTRY(name, value, _SetEntryDataSingle); }
		void TaggedDataWriter::AddDouble(const KeyType& name, double value)	{ TAGW_NEW_ENTRY(name, value, _SetEntryDataDouble); }
		void TaggedDataWriter::AddBool(const KeyType& name, bool value)		{ TAGW_NEW_ENTRY(name, value, _SetEntryDataBool); }

		void TaggedDataWriter::AddVector2(const KeyType& name, const Vector2& value)	{ TAGW_NEW_ENTRY(name, value, _SetEntryDataVector2); }
		void TaggedDataWriter::AddVector3(const KeyType& name, const Vector3& value)	{ TAGW_NEW_ENTRY(name, value, _SetEntryDataVector3); }
		void TaggedDataWriter::AddVector4(const KeyType& name, const Vector4& value)	{ TAGW_NEW_ENTRY(name, value, _SetEntryDataVector4); }
		void TaggedDataWriter::AddMatrix(const KeyType& name, const Matrix& value)		{ TAGW_NEW_ENTRY(name, value, _SetEntryDataMatrix); }
		void TaggedDataWriter::AddColor4(const KeyType& name, const Color4& value)		{ TAGW_NEW_ENTRY(name, value, _SetEntryDataColor4); }
		void TaggedDataWriter::AddString(const KeyType& name, const String& value)		{ TAGW_NEW_ENTRY(name, value, _SetEntryDataString); }
		void TaggedDataWriter::AddPlane(const KeyType& name, const Plane& value)		{ TAGW_NEW_ENTRY(name, value, _SetEntryDataPlane); }
		void TaggedDataWriter::AddQuaternion(const KeyType& name, const Quaternion& value)		{ TAGW_NEW_ENTRY(name, value, _SetEntryDataQuaternion); }
		void TaggedDataWriter::AddRectangle(const KeyType& name, const Rectangle& value)		{ TAGW_NEW_ENTRY(name, value, _SetEntryDataRectangle); }
		void TaggedDataWriter::AddRectangleF(const KeyType& name, const RectangleF& value)		{ TAGW_NEW_ENTRY(name, value, _SetEntryDataRectangleF); }
		void TaggedDataWriter::AddPoint(const KeyType& name, const Point& value)					{ TAGW_NEW_ENTRY(name, value, _SetEntryDataPoint); }
		void TaggedDataWriter::AddPointF(const KeyType& name, const PointF& value)					{ TAGW_NEW_ENTRY(name, value, _SetEntryDataPointF); }
		void TaggedDataWriter::AddMathSize(const KeyType& name, const Apoc3D::Math::Size& value)	{ TAGW_NEW_ENTRY(name, value, _SetEntryDataMathSize); }
		void TaggedDataWriter::AddBoundingBox(const KeyType& name, const BoundingBox& value)		{ TAGW_NEW_ENTRY(name, value, _SetEntryDataBoundingBox); }
		void TaggedDataWriter::AddBoundingSphere(const KeyType& name, const BoundingSphere& value)	{ TAGW_NEW_ENTRY(name, value, _SetEntryDataBoundingSphere); }
		void TaggedDataWriter::AddRay(const KeyType& name, const Ray& value)						{ TAGW_NEW_ENTRY(name, value, _SetEntryDataRay); }
		void TaggedDataWriter::AddViewport(const KeyType& name, const Viewport& value)				{ TAGW_NEW_ENTRY(name, value, _SetEntryDataViewport); }


		void TaggedDataWriter::AddInt64(const KeyType& name, const int64* value, int32 count)	{ TAGW_NEW_ENTRY_ARR(name, value, count, _SetEntryDataInt64); }
		void TaggedDataWriter::AddInt32(const KeyType& name, const int32* value, int32 count)	{ TAGW_NEW_ENTRY_ARR(name, value, count, _SetEntryDataInt32); }
		void TaggedDataWriter::AddInt16(const KeyType& name, const int16* value, int32 count)	{ TAGW_NEW_ENTRY_ARR(name, value, count, _SetEntryDataInt16); }
		void TaggedDataWriter::AddUInt64(const KeyType& name, const uint64* value, int32 count)	{ TAGW_NEW_ENTRY_ARR(name, value, count, _SetEntryDataUInt64); }
		void TaggedDataWriter::AddUInt32(const KeyType& name, const uint32* value, int32 count)	{ TAGW_NEW_ENTRY_ARR(name, value, count, _SetEntryDataUInt32); }
		void TaggedDataWriter::AddUInt16(const KeyType& name, const uint16* value, int32 count)	{ TAGW_NEW_ENTRY_ARR(name, value, count, _SetEntryDataUInt16); }
		void TaggedDataWriter::AddSingle(const KeyType& name, const float* value, int32 count)	{ TAGW_NEW_ENTRY_ARR(name, value, count, _SetEntryDataSingle); }
		void TaggedDataWriter::AddDouble(const KeyType& name, const double* value, int32 count)	{ TAGW_NEW_ENTRY_ARR(name, value, count, _SetEntryDataDouble); }
		void TaggedDataWriter::AddBool(const KeyType& name, const bool* value, int32 count)		{ TAGW_NEW_ENTRY_ARR(name, value, count, _SetEntryDataBool); }

		void TaggedDataWriter::AddVector2(const KeyType& name, const Vector2* vec, int32 count)	{ TAGW_NEW_ENTRY_ARR(name, vec, count, _SetEntryDataVector2); }
		void TaggedDataWriter::AddVector3(const KeyType& name, const Vector3* vec, int32 count)	{ TAGW_NEW_ENTRY_ARR(name, vec, count, _SetEntryDataVector3); }
		void TaggedDataWriter::AddVector4(const KeyType& name, const Vector4* vec, int32 count)	{ TAGW_NEW_ENTRY_ARR(name, vec, count, _SetEntryDataVector4); }
		void TaggedDataWriter::AddMatrix(const KeyType& name, const Matrix* mat, int32 count)		{ TAGW_NEW_ENTRY_ARR(name, mat, count, _SetEntryDataMatrix); }
		void TaggedDataWriter::AddColor4(const KeyType& name, const Color4* clr, int32 count)		{ TAGW_NEW_ENTRY_ARR(name, clr, count, _SetEntryDataColor4); }
		void TaggedDataWriter::AddString(const KeyType& name, const String* str, int32 count)		{ TAGW_NEW_ENTRY_ARR(name, str, count, _SetEntryDataString); }
		void TaggedDataWriter::AddPlane(const KeyType& name, const Plane* plane, int32 count)		{ TAGW_NEW_ENTRY_ARR(name, plane, count, _SetEntryDataPlane); }
		void TaggedDataWriter::AddQuaternion(const KeyType& name, const Quaternion* quat, int32 count)		{ TAGW_NEW_ENTRY_ARR(name, quat, count, _SetEntryDataQuaternion); }
		void TaggedDataWriter::AddRectangle(const KeyType& name, const Rectangle* rect, int32 count)			{ TAGW_NEW_ENTRY_ARR(name, rect, count, _SetEntryDataRectangle); }
		void TaggedDataWriter::AddRectangleF(const KeyType& name, const RectangleF* rect, int32 count)			{ TAGW_NEW_ENTRY_ARR(name, rect, count, _SetEntryDataRectangleF); }
		void TaggedDataWriter::AddPoint(const KeyType& name, const Point* pt, int32 count)						{ TAGW_NEW_ENTRY_ARR(name, pt, count, _SetEntryDataPoint); }
		void TaggedDataWriter::AddPointF(const KeyType& name, const PointF* pt, int32 count)					{ TAGW_NEW_ENTRY_ARR(name, pt, count, _SetEntryDataPointF); }
		void TaggedDataWriter::AddMathSize(const KeyType& name, const Apoc3D::Math::Size* sz, int32 count)		{ TAGW_NEW_ENTRY_ARR(name, sz, count, _SetEntryDataMathSize); }
		void TaggedDataWriter::AddBoundingBox(const KeyType& name, const BoundingBox* bb, int32 count)			{ TAGW_NEW_ENTRY_ARR(name, bb, count, _SetEntryDataBoundingBox); }
		void TaggedDataWriter::AddBoundingSphere(const KeyType& name, const BoundingSphere* bs, int32 count)	{ TAGW_NEW_ENTRY_ARR(name, bs, count, _SetEntryDataBoundingSphere); }
		void TaggedDataWriter::AddRay(const KeyType& name, const Ray* r, int32 count)							{ TAGW_NEW_ENTRY_ARR(name, r, count, _SetEntryDataRay); }
		void TaggedDataWriter::AddViewport(const KeyType& name, const Viewport* vp, int32 count)				{ TAGW_NEW_ENTRY_ARR(name, vp, count, _SetEntryDataViewport); }

#define TAGW_SETDATA(name, value, setter) const Entry* ent = FindEntry(name); assert(ent); ent->ResetWritePosition(); setter(*ent, value);
#define TAGW_SETDATA_ARR(name, value, count, setter) const Entry* ent = FindEntry(name); assert(ent); ent->ResetWritePosition(); setter(*ent, value, count);

		void TaggedDataWriter::SetInt64(const KeyType& name, int64 value)	{ TAGW_SETDATA(name, value, _SetEntryDataInt64); }
		void TaggedDataWriter::SetInt32(const KeyType& name, int32 value)	{ TAGW_SETDATA(name, value, _SetEntryDataInt32); }
		void TaggedDataWriter::SetInt16(const KeyType& name, int16 value)	{ TAGW_SETDATA(name, value, _SetEntryDataInt16); }
		void TaggedDataWriter::SetUInt64(const KeyType& name, uint64 value)	{ TAGW_SETDATA(name, value, _SetEntryDataUInt64); }
		void TaggedDataWriter::SetUInt32(const KeyType& name, uint32 value)	{ TAGW_SETDATA(name, value, _SetEntryDataUInt32); }
		void TaggedDataWriter::SetUInt16(const KeyType& name, uint16 value)	{ TAGW_SETDATA(name, value, _SetEntryDataUInt16); }
		void TaggedDataWriter::SetSingle(const KeyType& name, float value)	{ TAGW_SETDATA(name, value, _SetEntryDataSingle); }
		void TaggedDataWriter::SetDouble(const KeyType& name, double value)	{ TAGW_SETDATA(name, value, _SetEntryDataDouble); }
		void TaggedDataWriter::SetBool(const KeyType& name, bool value)		{ TAGW_SETDATA(name, value, _SetEntryDataBool); }
		
		void TaggedDataWriter::SetInt64(const KeyType& name, const int64* value, int32 count)	{ TAGW_SETDATA_ARR(name, value, count, _SetEntryDataInt64); }
		void TaggedDataWriter::SetInt32(const KeyType& name, const int32* value, int32 count)	{ TAGW_SETDATA_ARR(name, value, count, _SetEntryDataInt32); }
		void TaggedDataWriter::SetInt16(const KeyType& name, const int16* value, int32 count)	{ TAGW_SETDATA_ARR(name, value, count, _SetEntryDataInt16); }
		void TaggedDataWriter::SetUInt64(const KeyType& name, const uint64* value, int32 count)	{ TAGW_SETDATA_ARR(name, value, count, _SetEntryDataUInt64); }
		void TaggedDataWriter::SetUInt32(const KeyType& name, const uint32* value, int32 count)	{ TAGW_SETDATA_ARR(name, value, count, _SetEntryDataUInt32); }
		void TaggedDataWriter::SetUInt16(const KeyType& name, const uint16* value, int32 count)	{ TAGW_SETDATA_ARR(name, value, count, _SetEntryDataUInt16); }
		void TaggedDataWriter::SetSingle(const KeyType& name, const float* value, int32 count)	{ TAGW_SETDATA_ARR(name, value, count, _SetEntryDataSingle); }
		void TaggedDataWriter::SetDouble(const KeyType& name, const double* value, int32 count)	{ TAGW_SETDATA_ARR(name, value, count, _SetEntryDataDouble); }
		void TaggedDataWriter::SetBool(const KeyType& name, const bool* value, int32 count)		{ TAGW_SETDATA_ARR(name, value, count, _SetEntryDataBool); }

		void TaggedDataWriter::SetVector2(const KeyType& name, const Vector2& value)		{ TAGW_SETDATA(name, value, _SetEntryDataVector2); }
		void TaggedDataWriter::SetVector3(const KeyType& name, const Vector3& value)		{ TAGW_SETDATA(name, value, _SetEntryDataVector3); }
		void TaggedDataWriter::SetVector4(const KeyType& name, const Vector4& value)		{ TAGW_SETDATA(name, value, _SetEntryDataVector4); }
		void TaggedDataWriter::SetMatrix(const KeyType& name, const Matrix& value)			{ TAGW_SETDATA(name, value, _SetEntryDataMatrix); }
		void TaggedDataWriter::SetColor4(const KeyType& name, const Color4& value)			{ TAGW_SETDATA(name, value, _SetEntryDataColor4); }
		void TaggedDataWriter::SetString(const KeyType& name, const String& value)			{ TAGW_SETDATA(name, value, _SetEntryDataString); }
		void TaggedDataWriter::SetPlane(const KeyType& name, const Plane& value)			{ TAGW_SETDATA(name, value, _SetEntryDataPlane); }
		void TaggedDataWriter::SetQuaternion(const KeyType& name, const Quaternion& value)	{ TAGW_SETDATA(name, value, _SetEntryDataQuaternion); }
		void TaggedDataWriter::SetRectangle(const KeyType& name, const Rectangle& value)	{ TAGW_SETDATA(name, value, _SetEntryDataRectangle); }
		void TaggedDataWriter::SetRectangleF(const KeyType& name, const RectangleF& value)	{ TAGW_SETDATA(name, value, _SetEntryDataRectangleF); }
		void TaggedDataWriter::SetPoint(const KeyType& name, const Point& value)			{ TAGW_SETDATA(name, value, _SetEntryDataPoint); }
		void TaggedDataWriter::SetPointF(const KeyType& name, const PointF& value)			{ TAGW_SETDATA(name, value, _SetEntryDataPointF); }
		void TaggedDataWriter::SetMathSize(const KeyType& name, const Apoc3D::Math::Size& value)	{ TAGW_SETDATA(name, value, _SetEntryDataMathSize); }
		void TaggedDataWriter::SetBoundingBox(const KeyType& name, const BoundingBox& value)		{ TAGW_SETDATA(name, value, _SetEntryDataBoundingBox); }
		void TaggedDataWriter::SetBoundingSphere(const KeyType& name, const BoundingSphere& value)	{ TAGW_SETDATA(name, value, _SetEntryDataBoundingSphere); }
		void TaggedDataWriter::SetRay(const KeyType& name, const Ray& value)						{ TAGW_SETDATA(name, value, _SetEntryDataRay); }
		void TaggedDataWriter::SetViewport(const KeyType& name, const Viewport& value)				{ TAGW_SETDATA(name, value, _SetEntryDataViewport); }

		void TaggedDataWriter::SetVector2(const KeyType& name, const Vector2* vec, int32 count)		{ TAGW_SETDATA_ARR(name, vec, count, _SetEntryDataVector2); }
		void TaggedDataWriter::SetVector3(const KeyType& name, const Vector3* vec, int32 count)		{ TAGW_SETDATA_ARR(name, vec, count, _SetEntryDataVector3); }
		void TaggedDataWriter::SetVector4(const KeyType& name, const Vector4* vec, int32 count)		{ TAGW_SETDATA_ARR(name, vec, count, _SetEntryDataVector4); }
		void TaggedDataWriter::SetMatrix(const KeyType& name, const Matrix* mat, int32 count)		{ TAGW_SETDATA_ARR(name, mat, count, _SetEntryDataMatrix); }
		void TaggedDataWriter::SetColor4(const KeyType& name, const Color4* clr, int32 count)		{ TAGW_SETDATA_ARR(name, clr, count, _SetEntryDataColor4); }
		void TaggedDataWriter::SetString(const KeyType& name, const String* str, int32 count)		{ TAGW_SETDATA_ARR(name, str, count, _SetEntryDataString); }
		void TaggedDataWriter::SetPlane(const KeyType& name, const Plane* plane, int32 count)		{ TAGW_SETDATA_ARR(name, plane, count, _SetEntryDataPlane); }
		void TaggedDataWriter::SetQuaternion(const KeyType& name, const Quaternion* quad, int32 count)		{ TAGW_SETDATA_ARR(name, quad, count, _SetEntryDataQuaternion); }
		void TaggedDataWriter::SetRectangle(const KeyType& name, const Rectangle* rect, int32 count)		{ TAGW_SETDATA_ARR(name, rect, count, _SetEntryDataRectangle); }
		void TaggedDataWriter::SetRectangleF(const KeyType& name, const RectangleF* rect, int32 count)		{ TAGW_SETDATA_ARR(name, rect, count, _SetEntryDataRectangleF); }
		void TaggedDataWriter::SetPoint(const KeyType& name, const Point* pt, int32 count)					{ TAGW_SETDATA_ARR(name, pt, count, _SetEntryDataPoint); }
		void TaggedDataWriter::SetPointF(const KeyType& name, const PointF* pt, int32 count)				{ TAGW_SETDATA_ARR(name, pt, count, _SetEntryDataPointF); }
		void TaggedDataWriter::SetMathSize(const KeyType& name, const Apoc3D::Math::Size* sz, int32 count)	{ TAGW_SETDATA_ARR(name, sz, count, _SetEntryDataMathSize); }
		void TaggedDataWriter::SetBoundingBox(const KeyType& name, const BoundingBox* bb, int32 count)		{ TAGW_SETDATA_ARR(name, bb, count, _SetEntryDataBoundingBox); }
		void TaggedDataWriter::SetBoundingSphere(const KeyType& name, const BoundingSphere* bs, int32 count){ TAGW_SETDATA_ARR(name, bs, count, _SetEntryDataBoundingSphere); }
		void TaggedDataWriter::SetRay(const KeyType& name, const Ray* r, int32 count)						{ TAGW_SETDATA_ARR(name, r, count, _SetEntryDataRay); }
		void TaggedDataWriter::SetViewport(const KeyType& name, const Viewport* vp, int32 count)			{ TAGW_SETDATA_ARR(name, vp, count, _SetEntryDataViewport); }

		void TaggedDataWriter::_SetEntryDataInt64(const Entry& ent, int64 value)
		{
#ifdef BIG_ENDIAN
			if (!m_endianIndependent)
				i64_mb_dep(value, m_buffer);
			else
#endif
				i64_mb_le(value, m_buffer);

			ent.Buffer->Write(m_buffer, sizeof(value));
		}
		void TaggedDataWriter::_SetEntryDataInt32(const Entry& ent, int32 value)
		{
#ifdef BIG_ENDIAN
			if (!m_endianIndependent)
				i32_mb_dep(value, m_buffer);
			else
#endif
				i32_mb_le(value, m_buffer);

			ent.Buffer->Write(m_buffer, sizeof(value));
		}
		void TaggedDataWriter::_SetEntryDataInt16(const Entry& ent, int16 value)
		{
#ifdef BIG_ENDIAN
			if (!m_endianIndependent)
				i16_mb_dep(value, m_buffer);
			else
#endif
				i16_mb_le(value, m_buffer);

			ent.Buffer->Write(m_buffer, sizeof(value));
		}

		void TaggedDataWriter::_SetEntryDataUInt64(const Entry& ent, uint64 value)
		{
#ifdef BIG_ENDIAN
			if (!m_endianIndependent)
				u64_mb_dep(value, m_buffer);
			else
#endif
				u64_mb_le(value, m_buffer);

			ent.Buffer->Write(m_buffer, sizeof(value));
		}
		void TaggedDataWriter::_SetEntryDataUInt32(const Entry& ent, uint32 value)
		{
#ifdef BIG_ENDIAN
			if (!m_endianIndependent)
				u32_mb_dep(value, m_buffer);
			else
#endif
				u32_mb_le(value, m_buffer);

			ent.Buffer->Write(m_buffer, sizeof(value));
		}
		void TaggedDataWriter::_SetEntryDataUInt16(const Entry& ent, uint16 value)
		{
#ifdef BIG_ENDIAN
			if (!m_endianIndependent)
				u16_mb_dep(value, m_buffer);
			else
#endif
				u16_mb_le(value, m_buffer);

			ent.Buffer->Write(m_buffer, sizeof(value));
		}

		void TaggedDataWriter::_SetEntryDataSingle(const Entry& ent, float value)
		{
#ifdef BIG_ENDIAN
			if (!m_endianIndependent)
				f32_mb_dep(value, m_buffer);
			else
#endif
				f32_mb_le(value, m_buffer);

			ent.Buffer->Write(m_buffer, sizeof(value));
		}
		void TaggedDataWriter::_SetEntryDataDouble(const Entry& ent, double value)
		{
#ifdef BIG_ENDIAN
			if (!m_endianIndependent)
				f64_mb_dep(value, m_buffer);
			else
#endif
				f64_mb_le(value, m_buffer);

			ent.Buffer->Write(m_buffer, sizeof(value));
		}
		void TaggedDataWriter::_SetEntryDataBool(const Entry& ent, bool value)
		{
			m_buffer[0] = value ? 1 : 0;

			ent.Buffer->Write(m_buffer, sizeof(value));
		}


		void TaggedDataWriter::_SetEntryDataInt64(const Entry& ent, const int64* value, int32 count)		{ for (int32 i = 0; i < count; i++) _SetEntryDataInt64(ent, value[i]); }
		void TaggedDataWriter::_SetEntryDataInt32(const Entry& ent, const int32* value, int32 count)		{ for (int32 i = 0; i < count; i++) _SetEntryDataInt32(ent, value[i]); }
		void TaggedDataWriter::_SetEntryDataInt16(const Entry& ent, const int16* value, int32 count)		{ for (int32 i = 0; i < count; i++) _SetEntryDataInt16(ent, value[i]); }
		void TaggedDataWriter::_SetEntryDataUInt64(const Entry& ent, const uint64* value, int32 count)		{ for (int32 i = 0; i < count; i++) _SetEntryDataUInt64(ent, value[i]); }
		void TaggedDataWriter::_SetEntryDataUInt32(const Entry& ent, const uint32* value, int32 count)		{ for (int32 i = 0; i < count; i++) _SetEntryDataUInt32(ent, value[i]); }
		void TaggedDataWriter::_SetEntryDataUInt16(const Entry& ent, const uint16* value, int32 count)		{ for (int32 i = 0; i < count; i++) _SetEntryDataUInt16(ent, value[i]); }
		void TaggedDataWriter::_SetEntryDataSingle(const Entry& ent, const float* value, int32 count)		{ for (int32 i = 0; i < count; i++) _SetEntryDataSingle(ent, value[i]); }
		void TaggedDataWriter::_SetEntryDataDouble(const Entry& ent, const double* value, int32 count)		{ for (int32 i = 0; i < count; i++) _SetEntryDataDouble(ent, value[i]); }
		void TaggedDataWriter::_SetEntryDataBool(const Entry& ent, const bool* value, int32 count)			{ for (int32 i = 0; i < count; i++) _SetEntryDataBool(ent, value[i]); }



		void TaggedDataWriter::_SetEntryDataVector2(const Entry& ent, const Vector2& vec)
		{
#ifdef BIG_ENDIAN
			if (!m_endianIndependent)
			{
				f32_mb_dep(vec.X, m_buffer);
				f32_mb_dep(vec.Y, m_buffer + sizeof(float));
			}
			else
#endif
			{
				f32_mb_le(vec.X, m_buffer);
				f32_mb_le(vec.Y, m_buffer + sizeof(float));
			}
			ent.Buffer->Write(m_buffer, sizeof(float) * 2);
		}
		void TaggedDataWriter::_SetEntryDataVector3(const Entry& ent, const Vector3& vec)
		{
#ifdef BIG_ENDIAN
			if (!m_endianIndependent)
			{
				f32_mb_dep(vec.X, m_buffer);
				f32_mb_dep(vec.Y, m_buffer + sizeof(float));
				f32_mb_dep(vec.Z, m_buffer + sizeof(float) * 2);
			}
			else
#endif
			{
				f32_mb_le(vec.X, m_buffer);
				f32_mb_le(vec.Y, m_buffer + sizeof(float));
				f32_mb_le(vec.Z, m_buffer + sizeof(float) * 2);
			}
			ent.Buffer->Write(m_buffer, sizeof(float) * 3);
		}
		void TaggedDataWriter::_SetEntryDataVector4(const Entry& ent, const Vector4& vec)
		{
#ifdef BIG_ENDIAN
			if (!m_endianIndependent)
			{
				f32_mb_dep(vec.X, m_buffer);
				f32_mb_dep(vec.Y, m_buffer + sizeof(float));
				f32_mb_dep(vec.Z, m_buffer + sizeof(float) * 2);
				f32_mb_dep(vec.W, m_buffer + sizeof(float) * 3);
			}
			else
#endif
			{
				f32_mb_le(vec.X, m_buffer);
				f32_mb_le(vec.Y, m_buffer + sizeof(float));
				f32_mb_le(vec.Z, m_buffer + sizeof(float) * 2);
				f32_mb_le(vec.W, m_buffer + sizeof(float) * 3);
			}
			ent.Buffer->Write(m_buffer, sizeof(float) * 4);
		}
		void TaggedDataWriter::_SetEntryDataMatrix(const Entry& ent, const Matrix& mat)
		{
#ifdef BIG_ENDIAN
			if (!m_endianIndependent)
			{
				f32_mb_dep(mat.M11, m_buffer);
				f32_mb_dep(mat.M12, m_buffer + sizeof(float));
				f32_mb_dep(mat.M13, m_buffer + sizeof(float) * 2);
				f32_mb_dep(mat.M14, m_buffer + sizeof(float) * 3);
				f32_mb_dep(mat.M21, m_buffer + sizeof(float) * 4);
				f32_mb_dep(mat.M22, m_buffer + sizeof(float) * 5);
				f32_mb_dep(mat.M23, m_buffer + sizeof(float) * 6);
				f32_mb_dep(mat.M24, m_buffer + sizeof(float) * 7);
				ent.Buffer->Write(m_buffer, sizeof(float) * 8);

				f32_mb_dep(mat.M31, m_buffer);
				f32_mb_dep(mat.M32, m_buffer + sizeof(float));
				f32_mb_dep(mat.M33, m_buffer + sizeof(float) * 2);
				f32_mb_dep(mat.M34, m_buffer + sizeof(float) * 3);
				f32_mb_dep(mat.M41, m_buffer + sizeof(float) * 4);
				f32_mb_dep(mat.M42, m_buffer + sizeof(float) * 5);
				f32_mb_dep(mat.M43, m_buffer + sizeof(float) * 6);
				f32_mb_dep(mat.M44, m_buffer + sizeof(float) * 7);
				ent.Buffer->Write(m_buffer, sizeof(float) * 8);
			}
			else
#endif
			{
				f32_mb_le(mat.M11, m_buffer);
				f32_mb_le(mat.M12, m_buffer + sizeof(float));
				f32_mb_le(mat.M13, m_buffer + sizeof(float) * 2);
				f32_mb_le(mat.M14, m_buffer + sizeof(float) * 3);
				f32_mb_le(mat.M21, m_buffer + sizeof(float) * 4);
				f32_mb_le(mat.M22, m_buffer + sizeof(float) * 5);
				f32_mb_le(mat.M23, m_buffer + sizeof(float) * 6);
				f32_mb_le(mat.M24, m_buffer + sizeof(float) * 7);
				ent.Buffer->Write(m_buffer, sizeof(float) * 8);

				f32_mb_le(mat.M31, m_buffer);
				f32_mb_le(mat.M32, m_buffer + sizeof(float));
				f32_mb_le(mat.M33, m_buffer + sizeof(float) * 2);
				f32_mb_le(mat.M34, m_buffer + sizeof(float) * 3);
				f32_mb_le(mat.M41, m_buffer + sizeof(float) * 4);
				f32_mb_le(mat.M42, m_buffer + sizeof(float) * 5);
				f32_mb_le(mat.M43, m_buffer + sizeof(float) * 6);
				f32_mb_le(mat.M44, m_buffer + sizeof(float) * 7);
				ent.Buffer->Write(m_buffer, sizeof(float) * 8);
			}
		}
		void TaggedDataWriter::_SetEntryDataColor4(const Entry& ent, const Color4& clr)
		{
#ifdef BIG_ENDIAN
			if (!m_endianIndependent)
			{
				f32_mb_dep(clr.Red, m_buffer);
				f32_mb_dep(clr.Green, m_buffer + sizeof(float));
				f32_mb_dep(clr.Blue, m_buffer + sizeof(float) * 2);
				f32_mb_dep(clr.Alpha, m_buffer + sizeof(float) * 3);
			}
			else
#endif
			{
				f32_mb_le(clr.Red, m_buffer);
				f32_mb_le(clr.Green, m_buffer + sizeof(float));
				f32_mb_le(clr.Blue, m_buffer + sizeof(float) * 2);
				f32_mb_le(clr.Alpha, m_buffer + sizeof(float) * 3);
			}
			ent.Buffer->Write(m_buffer, sizeof(float) * 4);
		}
		void TaggedDataWriter::_SetEntryDataString(const Entry& ent, const String& str)
		{
			//VirtualStream vs(ent.Buffer, 0);
			BinaryWriter bw(ent.Buffer, false);

			bw.WriteString(str);
		}
		void TaggedDataWriter::_SetEntryDataPlane(const Entry& ent, const Plane& plane)
		{
#ifdef BIG_ENDIAN
			if (!m_endianIndependent)
			{
				f32_mb_dep(plane.X, m_buffer);
				f32_mb_dep(plane.Y, m_buffer + sizeof(float));
				f32_mb_dep(plane.Z, m_buffer + sizeof(float) * 2);
				f32_mb_dep(plane.D, m_buffer + sizeof(float) * 3);
			}
			else
#endif
			{
				f32_mb_le(plane.X, m_buffer);
				f32_mb_le(plane.Y, m_buffer + sizeof(float));
				f32_mb_le(plane.Z, m_buffer + sizeof(float) * 2);
				f32_mb_le(plane.D, m_buffer + sizeof(float) * 3);
			}
			ent.Buffer->Write(m_buffer, sizeof(float) * 4);
		}
		void TaggedDataWriter::_SetEntryDataQuaternion(const Entry& ent, const Quaternion& quat)
		{
#ifdef BIG_ENDIAN
			if (!m_endianIndependent)
			{
				f32_mb_dep(quat.X, m_buffer);
				f32_mb_dep(quat.Y, m_buffer + sizeof(float));
				f32_mb_dep(quat.Z, m_buffer + sizeof(float) * 2);
				f32_mb_dep(quat.W, m_buffer + sizeof(float) * 3);
			}
			else
#endif
			{
				f32_mb_le(quat.X, m_buffer);
				f32_mb_le(quat.Y, m_buffer + sizeof(float));
				f32_mb_le(quat.Z, m_buffer + sizeof(float) * 2);
				f32_mb_le(quat.W, m_buffer + sizeof(float) * 3);
			}
			ent.Buffer->Write(m_buffer, sizeof(float) * 4);
		}
		void TaggedDataWriter::_SetEntryDataRectangle(const Entry& ent, const Rectangle& rect)
		{
#ifdef BIG_ENDIAN
			if (!m_endianIndependent)
			{
				i32_mb_dep(rect.X, m_buffer);
				i32_mb_dep(rect.Y, m_buffer + sizeof(int32));
				i32_mb_dep(rect.Width, m_buffer + sizeof(int32) * 2);
				i32_mb_dep(rect.Height, m_buffer + sizeof(int32) * 3);
			}
			else
#endif
			{
				i32_mb_le(rect.X, m_buffer);
				i32_mb_le(rect.Y, m_buffer + sizeof(int32));
				i32_mb_le(rect.Width, m_buffer + sizeof(int32) * 2);
				i32_mb_le(rect.Height, m_buffer + sizeof(int32) * 3);
			}
			ent.Buffer->Write(m_buffer, sizeof(int32) * 4);
		}
		void TaggedDataWriter::_SetEntryDataRectangleF(const Entry& ent, const RectangleF& rect)
		{
#ifdef BIG_ENDIAN
			if (!m_endianIndependent)
			{
				f32_mb_dep(rect.X, m_buffer);
				f32_mb_dep(rect.Y, m_buffer + sizeof(float));
				f32_mb_dep(rect.Width, m_buffer + sizeof(float) * 2);
				f32_mb_dep(rect.Height, m_buffer + sizeof(float) * 3);
			}
			else
#endif
			{
				f32_mb_le(rect.X, m_buffer);
				f32_mb_le(rect.Y, m_buffer + sizeof(float));
				f32_mb_le(rect.Width, m_buffer + sizeof(float) * 2);
				f32_mb_le(rect.Height, m_buffer + sizeof(float) * 3);
			}
			ent.Buffer->Write(m_buffer, sizeof(float) * 4);
		}
		void TaggedDataWriter::_SetEntryDataPoint(const Entry& ent, const Point& pt)
		{
#ifdef BIG_ENDIAN
			if (!m_endianIndependent)
			{
				i32_mb_dep(pt.X, m_buffer);
				i32_mb_dep(pt.Y, m_buffer + sizeof(int32));
			}
			else
#endif
			{
				i32_mb_le(pt.X, m_buffer);
				i32_mb_le(pt.Y, m_buffer + sizeof(int32));
			}
			ent.Buffer->Write(m_buffer, sizeof(int32) * 2);
		}
		void TaggedDataWriter::_SetEntryDataPointF(const Entry& ent, const PointF& pt)
		{
#ifdef BIG_ENDIAN
			if (!m_endianIndependent)
			{
				f32_mb_dep(pt.X, m_buffer);
				f32_mb_dep(pt.Y, m_buffer + sizeof(float));
			}
			else
#endif
			{
				f32_mb_le(pt.X, m_buffer);
				f32_mb_le(pt.Y, m_buffer + sizeof(float));
			}
			ent.Buffer->Write(m_buffer, sizeof(float) * 2);
		}
		void TaggedDataWriter::_SetEntryDataMathSize(const Entry& ent, const Apoc3D::Math::Size& sz)
		{
#ifdef BIG_ENDIAN
			if (!m_endianIndependent)
			{
				i32_mb_dep(sz.Width, m_buffer);
				i32_mb_dep(sz.Height, m_buffer + sizeof(int32));
			}
			else
#endif
			{
				i32_mb_le(sz.Width, m_buffer);
				i32_mb_le(sz.Height, m_buffer + sizeof(int32));
			}
			ent.Buffer->Write(m_buffer, sizeof(int32) * 2);
		}
		void TaggedDataWriter::_SetEntryDataBoundingBox(const Entry& ent, const BoundingBox& bb)
		{
#ifdef BIG_ENDIAN
			if (!m_endianIndependent)
			{
				f32_mb_dep(bb.Minimum.X, m_buffer);
				f32_mb_dep(bb.Minimum.Y, m_buffer + sizeof(float));
				f32_mb_dep(bb.Minimum.Z, m_buffer + sizeof(float) * 2);

				f32_mb_dep(bb.Maximum.X, m_buffer + sizeof(float) * 3);
				f32_mb_dep(bb.Maximum.Y, m_buffer + sizeof(float) * 4);
				f32_mb_dep(bb.Maximum.Z, m_buffer + sizeof(float) * 5);
				ent.Buffer->Write(m_buffer, sizeof(float) * 6);
			}
			else
#endif
			{
				f32_mb_le(bb.Minimum.X, m_buffer);
				f32_mb_le(bb.Minimum.Y, m_buffer + sizeof(float));
				f32_mb_le(bb.Minimum.Z, m_buffer + sizeof(float) * 2);

				f32_mb_le(bb.Maximum.X, m_buffer + sizeof(float) * 3);
				f32_mb_le(bb.Maximum.Y, m_buffer + sizeof(float) * 4);
				f32_mb_le(bb.Maximum.Z, m_buffer + sizeof(float) * 5);
				ent.Buffer->Write(m_buffer, sizeof(float) * 6);
			}
		}
		void TaggedDataWriter::_SetEntryDataBoundingSphere(const Entry& ent, const BoundingSphere& bs)
		{
#ifdef BIG_ENDIAN
			if (!m_endianIndependent)
			{
				f32_mb_dep(bs.Center.X, m_buffer);
				f32_mb_dep(bs.Center.Y, m_buffer + sizeof(float));
				f32_mb_dep(bs.Center.Z, m_buffer + sizeof(float) * 2);
				f32_mb_dep(bs.Radius, m_buffer + sizeof(float) * 3);
			}
			else
#endif
			{
				f32_mb_le(bs.Center.X, m_buffer);
				f32_mb_le(bs.Center.Y, m_buffer + sizeof(float));
				f32_mb_le(bs.Center.Z, m_buffer + sizeof(float) * 2);
				f32_mb_le(bs.Radius, m_buffer + sizeof(float) * 3);
			}
			ent.Buffer->Write(m_buffer, sizeof(float) * 4);
		}
		void TaggedDataWriter::_SetEntryDataRay(const Entry& ent, const Ray& r)
		{
#ifdef BIG_ENDIAN
			if (!m_endianIndependent)
			{
				f32_mb_dep(r.Position.X, m_buffer);
				f32_mb_dep(r.Position.Y, m_buffer + sizeof(float));
				f32_mb_dep(r.Position.Z, m_buffer + sizeof(float) * 2);
				
				f32_mb_dep(r.Direction.X, m_buffer + sizeof(float) * 3);
				f32_mb_dep(r.Direction.Y, m_buffer + sizeof(float) * 4);
				f32_mb_dep(r.Direction.Z, m_buffer + sizeof(float) * 5);
				ent.Buffer->Write(m_buffer, sizeof(float) * 6);
			}
			else
#endif
			{
				f32_mb_le(r.Position.X, m_buffer);
				f32_mb_le(r.Position.Y, m_buffer + sizeof(float));
				f32_mb_le(r.Position.Z, m_buffer + sizeof(float) * 2);
				
				f32_mb_le(r.Direction.X, m_buffer + sizeof(float) * 3);
				f32_mb_le(r.Direction.Y, m_buffer + sizeof(float) * 4);
				f32_mb_le(r.Direction.Z, m_buffer + sizeof(float) * 5);
				ent.Buffer->Write(m_buffer, sizeof(float) * 6);
			}
		}
		void TaggedDataWriter::_SetEntryDataViewport(const Entry& ent, const Viewport& vp)
		{
#ifdef BIG_ENDIAN
			if (!m_endianIndependent)
			{
				i32_mb_dep(vp.X, m_buffer);
				i32_mb_dep(vp.Y, m_buffer + sizeof(int32));
				i32_mb_dep(vp.Width, m_buffer + sizeof(int32) * 2);
				i32_mb_dep(vp.Height, m_buffer + sizeof(int32) * 3);
				ent.Buffer->Write(m_buffer, sizeof(int32) * 4);

				f32_mb_dep(vp.MinZ, m_buffer);
				f32_mb_dep(vp.MaxZ, m_buffer + sizeof(float));
				ent.Buffer->Write(m_buffer, sizeof(float) * 2);
			}
			else
#endif
			{
				i32_mb_le(vp.X, m_buffer);
				i32_mb_le(vp.Y, m_buffer + sizeof(int32));
				i32_mb_le(vp.Width, m_buffer + sizeof(int32) * 2);
				i32_mb_le(vp.Height, m_buffer + sizeof(int32) * 3);
				ent.Buffer->Write(m_buffer, sizeof(int32) * 4);

				f32_mb_le(vp.MinZ, m_buffer);
				f32_mb_le(vp.MaxZ, m_buffer + sizeof(float));
				ent.Buffer->Write(m_buffer, sizeof(float) * 2);
			}
		}


		void TaggedDataWriter::_SetEntryDataVector2(const Entry& ent, const Vector2* vec, int32 count)		{ for (int32 i = 0; i < count; i++) _SetEntryDataVector2(ent, vec[i]); }
		void TaggedDataWriter::_SetEntryDataVector3(const Entry& ent, const Vector3* vec, int32 count)		{ for (int32 i = 0; i < count; i++) _SetEntryDataVector3(ent, vec[i]); }
		void TaggedDataWriter::_SetEntryDataVector4(const Entry& ent, const Vector4* vec, int32 count)		{ for (int32 i = 0; i < count; i++) _SetEntryDataVector4(ent, vec[i]); }
		void TaggedDataWriter::_SetEntryDataMatrix(const Entry& ent, const Matrix* mat, int32 count)		{ for (int32 i = 0; i < count; i++) _SetEntryDataMatrix(ent, mat[i]); }
		void TaggedDataWriter::_SetEntryDataColor4(const Entry& ent, const Color4* clr, int32 count)		{ for (int32 i = 0; i < count; i++) _SetEntryDataColor4(ent, clr[i]); }
		void TaggedDataWriter::_SetEntryDataString(const Entry& ent, const String* str, int32 count)
		{
			//VirtualStream vs(ent.Buffer, 0);
			BinaryWriter bw(ent.Buffer, false);

			for (int32 i = 0; i < count; i++)
				bw.WriteString(str[i]);
		}
		void TaggedDataWriter::_SetEntryDataPlane(const Entry& ent, const Plane* plane, int32 count)				{ for (int32 i = 0; i < count; i++) _SetEntryDataPlane(ent, plane[i]); }
		void TaggedDataWriter::_SetEntryDataQuaternion(const Entry& ent, const Quaternion* quat, int32 count)		{ for (int32 i = 0; i < count; i++) _SetEntryDataQuaternion(ent, quat[i]); }
		void TaggedDataWriter::_SetEntryDataRectangle(const Entry& ent, const Rectangle* rect, int32 count)			{ for (int32 i = 0; i < count; i++) _SetEntryDataRectangle(ent, rect[i]); }
		void TaggedDataWriter::_SetEntryDataRectangleF(const Entry& ent, const RectangleF* rect, int32 count)		{ for (int32 i = 0; i < count; i++) _SetEntryDataRectangleF(ent, rect[i]); }
		void TaggedDataWriter::_SetEntryDataPoint(const Entry& ent, const Point* pt, int32 count)					{ for (int32 i = 0; i < count; i++) _SetEntryDataPoint(ent, pt[i]); }
		void TaggedDataWriter::_SetEntryDataPointF(const Entry& ent, const PointF* pt, int32 count)					{ for (int32 i = 0; i < count; i++) _SetEntryDataPointF(ent, pt[i]); }
		void TaggedDataWriter::_SetEntryDataMathSize(const Entry& ent, const Apoc3D::Math::Size* sz, int32 count)	{ for (int32 i = 0; i < count; i++) _SetEntryDataMathSize(ent, sz[i]); }
		void TaggedDataWriter::_SetEntryDataBoundingBox(const Entry& ent, const BoundingBox* bb, int32 count)		{ for (int32 i = 0; i < count; i++) _SetEntryDataBoundingBox(ent, bb[i]); }
		void TaggedDataWriter::_SetEntryDataBoundingSphere(const Entry& ent, const BoundingSphere* bs, int32 count)	{ for (int32 i = 0; i < count; i++) _SetEntryDataBoundingSphere(ent, bs[i]); }
		void TaggedDataWriter::_SetEntryDataRay(const Entry& ent, const Ray* r, int32 count)						{ for (int32 i = 0; i < count; i++) _SetEntryDataRay(ent, r[i]); }
		void TaggedDataWriter::_SetEntryDataViewport(const Entry& ent, const Viewport* vp, int32 count)				{ for (int32 i = 0; i < count; i++) _SetEntryDataViewport(ent, vp[i]); }

	}
}
