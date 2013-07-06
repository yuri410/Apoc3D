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
#include "BinaryWriter.h"

#include "Streams.h"
#include "TaggedData.h"
#include "IOUtils.h"

#include "apoc3d/Math/Ray.h"
#include "apoc3d/Math/BoundingBox.h"
#include "apoc3d/Math/BoundingSphere.h"
#include "apoc3d/Math/Rectangle.h"
#include "apoc3d/Math/Quaternion.h"
#include "apoc3d/Math/Color.h"

namespace Apoc3D
{
	namespace IO
	{
		BinaryWriter::BinaryWriter(Stream* baseStream)
			: m_baseStream(baseStream), m_shouldDeleteStream(true)
		{
			m_endianDependent = baseStream->IsWriteEndianDependent();
		}
		BinaryWriter::~BinaryWriter()
		{
			if (m_shouldDeleteStream)
				delete m_baseStream;
		}

		void BinaryWriter::Write(const char* bytes, int64 count) const
		{
			m_baseStream->Write(bytes, count);
		}

		void BinaryWriter::WriteByte(char byte) const
		{
			m_baseStream->Write(&byte, 1);
		}
		void BinaryWriter::WriteBoolean(bool value) const
		{
			char buffer[sizeof(bool)];
			buffer[0] = value ? 1 : 0;
			m_baseStream->Write(buffer, sizeof(bool));
		}

		void BinaryWriter::WriteDouble(double value) const
		{
			char buffer[sizeof(double)];
#if !LITTLE_ENDIAN
			if (m_endianDependent)
			{
				r64tomb_dep(value, buffer);
			}
			else
#endif
			{
				r64tomb_le(value, buffer);				
			}
			m_baseStream->Write(buffer, sizeof(double));
		}
		void BinaryWriter::WriteSingle(float value) const
		{
			char buffer[sizeof(float)];
#if !LITTLE_ENDIAN
			if (m_endianDependent)
			{
				r32tomb_dep(value, buffer);
			}
			else
#endif
			{
				r32tomb_le(value, buffer);				
			}
			m_baseStream->Write(buffer, sizeof(float));
		}
		void BinaryWriter::WriteString(const String& value) const
		{
			bool isMB = true;

			for (size_t i=0;i<value.size();i++)
			{
				if (value[i]>0xff)
				{
					isMB = false;
				}
			}

			uint32 sizeBits = static_cast<uint32>( value.size());
			assert(sizeBits<0x80000000U);

			if (isMB)
			{
				sizeBits |= 0x80000000U;
			}

			WriteUInt32(sizeBits);

			if (isMB)
			{
				for (size_t i=0;i<value.size();i++)
				{
					Write(reinterpret_cast<const char&>(value[i]));
				}
			}
			else
			{
				for (size_t i=0;i<value.size();i++)
				{
					Write(reinterpret_cast<const int16&>(value[i]));
				}
			}
		}
		void BinaryWriter::WriteMBString(const std::string& value) const
		{
			WriteUInt32(static_cast<uint32>( value.size()));

			Write(value.c_str(), value.size());
		}

		void BinaryWriter::WriteInt16(int16 value) const
		{
			char buffer[sizeof(int16)];
#if !LITTLE_ENDIAN
			if (m_endianDependent)
			{
				i16tomb_dep(value, buffer);
			}
			else
#endif
			{
				i16tomb_le(value, buffer);				
			}
			m_baseStream->Write(buffer, sizeof(int16));
		}
		void BinaryWriter::WriteInt32(int32 value) const
		{
			char buffer[sizeof(int32)];
#if !LITTLE_ENDIAN
			if (m_endianDependent)
			{
				i32tomb_dep(value, buffer);
			}
			else
#endif
			{
				i32tomb_le(value, buffer);				
			}
			m_baseStream->Write(buffer, sizeof(int32));
		}
		void BinaryWriter::WriteInt64(int64 value) const
		{
			char buffer[sizeof(int64)];
#if !LITTLE_ENDIAN
			if (m_endianDependent)
			{
				i64tomb_dep(value, buffer);
			}
			else
#endif
			{
				i64tomb_le(value, buffer);				
			}
			m_baseStream->Write(buffer, sizeof(int64));
		}
		void BinaryWriter::WriteUInt16(uint16 value) const
		{
			char buffer[sizeof(uint16)];
#if !LITTLE_ENDIAN
			if (m_endianDependent)
			{
				ui16tomb_dep(value, buffer);
			}
			else
#endif
			{
				ui16tomb_le(value, buffer);				
			}
			m_baseStream->Write(buffer, sizeof(uint16));
		}
		void BinaryWriter::WriteUInt32(uint32 value) const
		{
			char buffer[sizeof(uint32)];
#if !LITTLE_ENDIAN
			if (m_endianDependent)
			{
				ui32tomb_dep(value, buffer);
			}
			else
#endif
			{
				ui32tomb_le(value, buffer);				
			}
			m_baseStream->Write(buffer, sizeof(uint32));
		}
		void BinaryWriter::WriteUInt64(uint64 value) const
		{
			char buffer[sizeof(uint64)];
#if !LITTLE_ENDIAN
			if (m_endianDependent)
			{
				ui64tomb_dep(value, buffer);
			}
			else
#endif
			{
				ui64tomb_le(value, buffer);				
			}
			m_baseStream->Write(buffer, sizeof(uint64));
		}


		void BinaryWriter::WriteColor4(const Color4& value) const
		{
			WriteSingle(static_cast<float>(value.Red));
			WriteSingle(static_cast<float>(value.Green));
			WriteSingle(static_cast<float>(value.Blue));
			WriteSingle(static_cast<float>(value.Alpha));
		}
		void BinaryWriter::WritePlane(const Plane& plane) const
		{
			WriteSingle(static_cast<float>(plane.X));
			WriteSingle(static_cast<float>(plane.Y));
			WriteSingle(static_cast<float>(plane.Z));
			WriteSingle(static_cast<float>(plane.D));
		}
		void BinaryWriter::WriteMatrix(const Matrix& matrix) const
		{
			WriteSingle(static_cast<float>(matrix.M11));
			WriteSingle(static_cast<float>(matrix.M12));
			WriteSingle(static_cast<float>(matrix.M13));
			WriteSingle(static_cast<float>(matrix.M14));

			WriteSingle(static_cast<float>(matrix.M21));
			WriteSingle(static_cast<float>(matrix.M22));
			WriteSingle(static_cast<float>(matrix.M23));
			WriteSingle(static_cast<float>(matrix.M24));

			WriteSingle(static_cast<float>(matrix.M31));
			WriteSingle(static_cast<float>(matrix.M32));
			WriteSingle(static_cast<float>(matrix.M33));
			WriteSingle(static_cast<float>(matrix.M34));

			WriteSingle(static_cast<float>(matrix.M41));
			WriteSingle(static_cast<float>(matrix.M42));
			WriteSingle(static_cast<float>(matrix.M43));
			WriteSingle(static_cast<float>(matrix.M44));
		}
		void BinaryWriter::WriteQuaternion(const Quaternion& quat) const
		{
			WriteSingle(static_cast<float>(quat.X));
			WriteSingle(static_cast<float>(quat.Y));
			WriteSingle(static_cast<float>(quat.Z));
			WriteSingle(static_cast<float>(quat.W));
		}
		void BinaryWriter::WritePoint(const Point& point) const
		{
			WriteInt32(static_cast<int32>(point.X));
			WriteInt32(static_cast<int32>(point.Y));
		}
		void BinaryWriter::WriteSize(const Size& size) const
		{
			WriteInt32(static_cast<int32>(size.Width));
			WriteInt32(static_cast<int32>(size.Height));
		}
		void BinaryWriter::WriteRectangle(const Apoc3D::Math::Rectangle& rect) const
		{
			WriteInt32(static_cast<int32>(rect.X));
			WriteInt32(static_cast<int32>(rect.Y));
			WriteInt32(static_cast<int32>(rect.Width));
			WriteInt32(static_cast<int32>(rect.Height));
		}
		void BinaryWriter::WriteRectangleF(const Apoc3D::Math::RectangleF& rect) const
		{
			WriteSingle(static_cast<float>(rect.X));
			WriteSingle(static_cast<float>(rect.Y));
			WriteSingle(static_cast<float>(rect.Width));
			WriteSingle(static_cast<float>(rect.Height));
		}

		void BinaryWriter::WriteRay(const Ray& ray) const
		{
			WriteSingle(ray.Position.X);
			WriteSingle(ray.Position.Y);
			WriteSingle(ray.Position.Z);
			WriteSingle(ray.Direction.X);
			WriteSingle(ray.Direction.Y);
			WriteSingle(ray.Direction.Z);
		}
		void BinaryWriter::WriteBoundingBox(const BoundingBox& box) const
		{
			WriteSingle(box.Minimum.X);
			WriteSingle(box.Minimum.Y);
			WriteSingle(box.Minimum.Z);
			WriteSingle(box.Maximum.X);
			WriteSingle(box.Maximum.Y);
			WriteSingle(box.Maximum.Z);
		}
		void BinaryWriter::WriteBoundingSphere(const BoundingSphere& sphere) const
		{
			WriteSingle(sphere.Center.X);
			WriteSingle(sphere.Center.Y);
			WriteSingle(sphere.Center.Z);
			WriteSingle(sphere.Radius);
		}

		void BinaryWriter::WriteTaggedDataBlock(const TaggedDataWriter* data) const
		{
			const uint32 zeroSize = 0;

			// temp placeholder
			WriteUInt32(zeroSize);

			int64 start = m_baseStream->getPosition();
			data->Save(new VirtualStream(m_baseStream, m_baseStream->getPosition()));

			int64 end = m_baseStream->getPosition();

			uint32 size = static_cast<uint32>(end - start);
			m_baseStream->setPosition(start-sizeof(uint32));

			WriteUInt32(size);

			m_baseStream->setPosition(end);

		}

		void BinaryWriter::WriteVector2(const Vector2& vec) const
		{
			char writeBuffer[sizeof(float) * 2];
#if !LITTLE_ENDIAN
			if (m_endianDependent)
			{
				r32tomb_dep(vec.X, writeBuffer);
				r32tomb_dep(vec.Y, writeBuffer + sizeof(float));
			}
			else
#endif
			{
				r32tomb_le(vec.X, writeBuffer);
				r32tomb_le(vec.Y, writeBuffer + sizeof(float));
			}
			m_baseStream->Write(writeBuffer, sizeof(float)*2);
		}
		void BinaryWriter::WriteVector3(const Vector3& vec) const
		{
			char writeBuffer[sizeof(float) * 3];
#if !LITTLE_ENDIAN
			if (m_endianDependent)
			{
				r32tomb_dep(vec.X, writeBuffer);
				r32tomb_dep(vec.Y, writeBuffer + sizeof(float));
				r32tomb_dep(vec.Z, writeBuffer + sizeof(float) * 2);
			}
			else
#endif
			{
				r32tomb_le(vec.X, writeBuffer);
				r32tomb_le(vec.Y, writeBuffer + sizeof(float));
				r32tomb_le(vec.Z, writeBuffer + sizeof(float) * 2);
			}
			m_baseStream->Write(writeBuffer, sizeof(float)*3);
		}
		void BinaryWriter::WriteVector4(const Vector4& vec) const
		{
			char writeBuffer[sizeof(float) * 4];
#if !LITTLE_ENDIAN
			if (m_endianDependent)
			{
				r32tomb_dep(vec.X, writeBuffer);
				r32tomb_dep(vec.Y, writeBuffer + sizeof(float));
				r32tomb_dep(vec.Z, writeBuffer + sizeof(float) * 2);
				r32tomb_dep(vec.W, writeBuffer + sizeof(float) * 3);
			}
			else
#endif
			{
				r32tomb_le(vec.X, writeBuffer);
				r32tomb_le(vec.Y, writeBuffer + sizeof(float));
				r32tomb_le(vec.Z, writeBuffer + sizeof(float) * 2);
				r32tomb_le(vec.W, writeBuffer + sizeof(float) * 3);
			}
			m_baseStream->Write(writeBuffer, sizeof(float)*4);
		}

		void BinaryWriter::WriteSingle(const float* data, int32 count) const		{ for (int32 i=0;i<count;i++) WriteSingle(data[i]); }
		void BinaryWriter::WriteDouble(const double* data, int32 count) const		{ for (int32 i=0;i<count;i++) WriteDouble(data[i]); }
		void BinaryWriter::WriteInt32(const int32* data, int32 count) const		{ for (int32 i=0;i<count;i++) WriteInt32(data[i]); }
		void BinaryWriter::WriteBoolean(const bool* data, int32 count) const		{ for (int32 i=0;i<count;i++) WriteBoolean(data[i]); }

		void BinaryWriter::Close() const
		{
			m_baseStream->Close();
		}


	}
}