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
#include "BinaryWriter.h"

#include "Streams.h"
#include "Math/Math.h"
#include "Math/BoundingBox.h"
#include "Math/BoundingSphere.h"
#include "TaggedData.h"

#include "IOUtils.h"

namespace Apoc3D
{
	namespace IO
	{
		BinaryWriter::BinaryWriter(Stream* baseStream)
			: m_baseStream(baseStream)
		{
			m_endianDependent = baseStream->IsWriteEndianDependent();
		}
		BinaryWriter::~BinaryWriter()
		{
			delete m_baseStream;
		}

		void BinaryWriter::Write(const char* bytes, int64 count) const
		{
			m_baseStream->Write(bytes, count);
		}
		void BinaryWriter::Write(char byte) const
		{
			m_baseStream->Write(&byte, 1);
		}

		void BinaryWriter::Write(const TaggedDataWriter* data) const
		{
			const uint32 zeroSize = 0;

			// temp placeholder
			Write(zeroSize);

			int64 start = m_baseStream->getPosition();
			data->Save(new VirtualStream(m_baseStream, m_baseStream->getPosition()));

			int64 end = m_baseStream->getPosition();

			uint32 size = static_cast<uint32>(end - start);
			m_baseStream->setPosition(start-sizeof(uint32));

			Write(size);

			m_baseStream->setPosition(end);

		}
		void BinaryWriter::Write(const BoundingSphere& sphere) const
		{
			Write(static_cast<float>(_V3X(sphere.Center)));
			Write(static_cast<float>(_V3Y(sphere.Center)));
			Write(static_cast<float>(_V3Z(sphere.Center)));
			Write(static_cast<float>(sphere.Radius));
		}
		void BinaryWriter::Write(const BoundingBox& box) const
		{
			Write(static_cast<float>(_V3X(box.Minimum)));
			Write(static_cast<float>(_V3Y(box.Minimum)));
			Write(static_cast<float>(_V3Z(box.Minimum)));
			Write(static_cast<float>(_V3X(box.Maximum)));
			Write(static_cast<float>(_V3Y(box.Maximum)));
			Write(static_cast<float>(_V3Z(box.Maximum)));
		}
		void BinaryWriter::Write(const Ray& ray) const
		{
			Write(static_cast<float>(_V3X(ray.Position)));
			Write(static_cast<float>(_V3Y(ray.Position)));
			Write(static_cast<float>(_V3Z(ray.Position)));
			Write(static_cast<float>(_V3X(ray.Direction)));
			Write(static_cast<float>(_V3Y(ray.Direction)));
			Write(static_cast<float>(_V3Z(ray.Direction)));
		}
		void BinaryWriter::Write(const RectangleF& rect) const
		{
			Write(static_cast<float>(rect.X));
			Write(static_cast<float>(rect.Y));
			Write(static_cast<float>(rect.Width));
			Write(static_cast<float>(rect.Height));
		}
		void BinaryWriter::Write(const Rectangle& rect) const
		{
			Write(static_cast<int32>(rect.X));
			Write(static_cast<int32>(rect.Y));
			Write(static_cast<int32>(rect.Width));
			Write(static_cast<int32>(rect.Height));
		}
		void BinaryWriter::Write(const Size& size) const
		{
			Write(static_cast<int32>(size.Width));
			Write(static_cast<int32>(size.Height));
		}
		void BinaryWriter::Write(const Point& point) const
		{
			Write(static_cast<int32>(point.X));
			Write(static_cast<int32>(point.Y));
		}
		void BinaryWriter::Write(const Quaternion& quat) const
		{
			Write(static_cast<float>(quat.X));
			Write(static_cast<float>(quat.Y));
			Write(static_cast<float>(quat.Z));
			Write(static_cast<float>(quat.W));
		}
		void BinaryWriter::Write(const Matrix& matrix) const
		{
			Write(static_cast<float>(matrix.M11));
			Write(static_cast<float>(matrix.M12));
			Write(static_cast<float>(matrix.M13));
			Write(static_cast<float>(matrix.M14));

			Write(static_cast<float>(matrix.M21));
			Write(static_cast<float>(matrix.M22));
			Write(static_cast<float>(matrix.M23));
			Write(static_cast<float>(matrix.M24));

			Write(static_cast<float>(matrix.M31));
			Write(static_cast<float>(matrix.M32));
			Write(static_cast<float>(matrix.M33));
			Write(static_cast<float>(matrix.M34));

			Write(static_cast<float>(matrix.M41));
			Write(static_cast<float>(matrix.M42));
			Write(static_cast<float>(matrix.M43));
			Write(static_cast<float>(matrix.M44));

		}
		void BinaryWriter::Write(const Plane& plane) const
		{
			Write(static_cast<float>(plane.X));
			Write(static_cast<float>(plane.Y));
			Write(static_cast<float>(plane.Z));
			Write(static_cast<float>(plane.D));

		}
		void BinaryWriter::Write(const Color4& value) const
		{
			Write(static_cast<float>(value.Red));
			Write(static_cast<float>(value.Green));
			Write(static_cast<float>(value.Blue));
			Write(static_cast<float>(value.Alpha));
		}
		void BinaryWriter::Write(bool value) const
		{
			char buffer[sizeof(bool)];
			buffer[0] = value ? 1 : 0;
			m_baseStream->Write(buffer, sizeof(bool));
		}
		void BinaryWriter::Write(uint64 value) const
		{
			char buffer[sizeof(uint64)];
			if (m_endianDependent)
			{
				ui64tomb_dep(value, buffer);
			}
			else
			{
				ui64tomb_le(value, buffer);				
			}
			m_baseStream->Write(buffer, sizeof(uint64));
		}
		void BinaryWriter::Write(uint32 value) const
		{
			char buffer[sizeof(uint32)];
			if (m_endianDependent)
			{
				ui32tomb_dep(value, buffer);
			}
			else
			{
				ui32tomb_le(value, buffer);				
			}
			m_baseStream->Write(buffer, sizeof(uint32));
		}
		void BinaryWriter::Write(uint16 value) const
		{
			char buffer[sizeof(uint16)];
			if (m_endianDependent)
			{
				ui16tomb_dep(value, buffer);
			}
			else
			{
				ui16tomb_le(value, buffer);				
			}
			m_baseStream->Write(buffer, sizeof(uint16));
		}
		void BinaryWriter::Write(int64 value) const
		{
			char buffer[sizeof(int64)];
			if (m_endianDependent)
			{
				i64tomb_dep(value, buffer);
			}
			else
			{
				i64tomb_le(value, buffer);				
			}
			m_baseStream->Write(buffer, sizeof(int64));
		}
		void BinaryWriter::Write(int32 value) const
		{
			char buffer[sizeof(int32)];
			if (m_endianDependent)
			{
				i32tomb_dep(value, buffer);
			}
			else
			{
				i32tomb_le(value, buffer);				
			}
			m_baseStream->Write(buffer, sizeof(int32));
		}
		void BinaryWriter::Write(int16 value) const
		{
			char buffer[sizeof(int16)];
			if (m_endianDependent)
			{
				i16tomb_dep(value, buffer);
			}
			else
			{
				i16tomb_le(value, buffer);				
			}
			m_baseStream->Write(buffer, sizeof(int16));
		}
		void BinaryWriter::Write(float value) const
		{
			char buffer[sizeof(float)];
			if (m_endianDependent)
			{
				r32tomb_dep(value, buffer);
			}
			else
			{
				r32tomb_le(value, buffer);				
			}
			m_baseStream->Write(buffer, sizeof(float));
		}
		void BinaryWriter::Write(double value) const
		{
			char buffer[sizeof(double)];
			if (m_endianDependent)
			{
				r64tomb_dep(value, buffer);
			}
			else
			{
				r64tomb_le(value, buffer);				
			}
			m_baseStream->Write(buffer, sizeof(double));
		}
		void BinaryWriter::Write(const String& value) const
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

			Write(sizeBits);

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
		void BinaryWriter::Write(const std::string& value) const
		{
			Write(static_cast<uint32>( value.size()));

			Write(value.c_str(), value.size());
		}

		void BinaryWriter::Close() const
		{
			m_baseStream->Close();
		}


	}
}