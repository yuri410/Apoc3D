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


#include "BinaryReader.h"

#include "Streams.h"
#include "TaggedData.h"
#include "IOUtils.h"

#include "apoc3d/Apoc3DException.h"
#include "apoc3d/Math/Ray.h"
#include "apoc3d/Math/BoundingBox.h"
#include "apoc3d/Math/BoundingSphere.h"
#include "apoc3d/Math/Rectangle.h"
#include "apoc3d/Math/Quaternion.h"
#include "apoc3d/Math/Color.h"
#include "apoc3d/Vfs/ResourceLocation.h"

namespace Apoc3D
{
	namespace IO
	{
		BinaryReader::BinaryReader(Stream* baseStream)
			: m_baseStream(baseStream), m_shouldDeleteStream(true)
		{
			m_isEndianDependent = baseStream->IsReadEndianDependent();
		}
		BinaryReader::BinaryReader(const ResourceLocation* rl)
			: m_shouldDeleteStream(true)
		{
			m_baseStream = rl->GetReadStream();
			m_isEndianDependent = m_baseStream->IsReadEndianDependent();
		}
		BinaryReader::~BinaryReader()
		{
			if (m_shouldDeleteStream)
				delete m_baseStream;
		}
		void BinaryReader::FillBuffer(int32 len)
		{
			int64 result = m_baseStream->Read(&m_buffer[0], len); 
			if (len != result)
			{
				throw Apoc3DException::createException(EX_EndOfStream, L"");
			}
		}

		bool BinaryReader::ReadBoolean()
		{
			FillBuffer(sizeof(bool));
			return !!m_buffer[0];
		}
		double BinaryReader::ReadDouble()
		{
			FillBuffer(sizeof(double));
			if (m_isEndianDependent)
			{
				return cr64_dep(m_buffer);
			}
			return cr64_le(m_buffer);
		}
		float BinaryReader::ReadSingle()
		{
			FillBuffer(sizeof(float));
			if (m_isEndianDependent)
			{
				return cr32_dep(m_buffer);
			}
			return cr32_le(m_buffer);
		}
		String BinaryReader::ReadString()
		{
			uint32 len = ReadUInt32();

			if ((len & 0x80000000U) == 0x80000000U)
			{
				len &= 0x7FFFFFFFU;

				String str(len, ' ');
				for (size_t i=0;i<len;i++)
				{
					str[i] = ReadByte();
				}
				return str;
			}

			String str(len, ' ');
			for (size_t i=0;i<len;i++)
			{
				str[i] = ReadInt16();
			}

			return str;
		}

		std::string BinaryReader::ReadMBString()
		{
			uint32 len = ReadUInt32();

			std::string str(len, ' ');
			for (size_t i=0;i<len;i++)
			{
				str[i] = ReadByte();
			}
			return str;
		}

		int16 BinaryReader::ReadInt16() 
		{
			FillBuffer(sizeof(int16));
			if (m_isEndianDependent)
			{
				return ci16_dep(m_buffer);
			}
			return ci16_le(m_buffer);
		}
		int32 BinaryReader::ReadInt32() 
		{
			FillBuffer(sizeof(int32));
			if (m_isEndianDependent)
			{
				return ci32_dep(m_buffer);
			}
			return ci32_le(m_buffer);
		}
		int64 BinaryReader::ReadInt64() 
		{
			FillBuffer(sizeof(int64));
			if (m_isEndianDependent)
			{
				return ci64_dep(m_buffer);
			}
			return ci64_le(m_buffer);
		}

		uint16 BinaryReader::ReadUInt16() 
		{
			FillBuffer(sizeof(uint16));
			if (m_isEndianDependent)
			{
				return cui16_dep(m_buffer);
			}
			return cui16_le(m_buffer);
		}
		uint32 BinaryReader::ReadUInt32() 
		{
			FillBuffer(sizeof(uint32));
			if (m_isEndianDependent)
			{
				return cui32_dep(m_buffer);
			}
			return cui32_le(m_buffer);
		}
		uint64 BinaryReader::ReadUInt64() 
		{
			FillBuffer(sizeof(uint64));
			if (m_isEndianDependent)
			{
				return cui64_dep(m_buffer);
			}
			return cui64_le(m_buffer);
		}


		int64 BinaryReader::ReadBytes(char* dest, int64 count) const
		{
			return m_baseStream->Read(dest, count);
		}
		char BinaryReader::ReadByte() const
		{
			int r = m_baseStream->ReadByte();
			if (r == -1)
			{
				throw Apoc3DException::createException(EX_EndOfStream, L"");
			}
			return reinterpret_cast<const char&>(r);
		}

		void BinaryReader::ReadMatrix(Matrix& matrix)
		{
			FillBuffer(sizeof(float)*8);
			if (m_isEndianDependent)
			{
				matrix.M11 = cr32_dep(&m_buffer[0]);
				matrix.M12 = cr32_dep(&m_buffer[sizeof(float)]);
				matrix.M13 = cr32_dep(&m_buffer[sizeof(float)*2]);
				matrix.M14 = cr32_dep(&m_buffer[sizeof(float)*3]);
				matrix.M21 = cr32_dep(&m_buffer[sizeof(float)*4]);
				matrix.M22 = cr32_dep(&m_buffer[sizeof(float)*5]);
				matrix.M23 = cr32_dep(&m_buffer[sizeof(float)*6]);
				matrix.M24 = cr32_dep(&m_buffer[sizeof(float)*7]);
			}
			else
			{
				matrix.M11 = cr32_le(&m_buffer[0]);
				matrix.M12 = cr32_le(&m_buffer[sizeof(float)]);
				matrix.M13 = cr32_le(&m_buffer[sizeof(float)*2]);
				matrix.M14 = cr32_le(&m_buffer[sizeof(float)*3]);
				matrix.M21 = cr32_le(&m_buffer[sizeof(float)*4]);
				matrix.M22 = cr32_le(&m_buffer[sizeof(float)*5]);
				matrix.M23 = cr32_le(&m_buffer[sizeof(float)*6]);
				matrix.M24 = cr32_le(&m_buffer[sizeof(float)*7]);
			}
			
			FillBuffer(sizeof(float)*8);
			if (m_isEndianDependent)
			{
				matrix.M31 = cr32_dep(&m_buffer[0]);
				matrix.M32 = cr32_dep(&m_buffer[sizeof(float)]);
				matrix.M33 = cr32_dep(&m_buffer[sizeof(float)*2]);
				matrix.M34 = cr32_dep(&m_buffer[sizeof(float)*3]);
				matrix.M41 = cr32_dep(&m_buffer[sizeof(float)*4]);
				matrix.M42 = cr32_dep(&m_buffer[sizeof(float)*5]);
				matrix.M43 = cr32_dep(&m_buffer[sizeof(float)*6]);
				matrix.M44 = cr32_dep(&m_buffer[sizeof(float)*7]);
			}
			else
			{
				matrix.M31 = cr32_le(&m_buffer[0]);
				matrix.M32 = cr32_le(&m_buffer[sizeof(float)]);
				matrix.M33 = cr32_le(&m_buffer[sizeof(float)*2]);
				matrix.M34 = cr32_le(&m_buffer[sizeof(float)*3]);
				matrix.M41 = cr32_le(&m_buffer[sizeof(float)*4]);
				matrix.M42 = cr32_le(&m_buffer[sizeof(float)*5]);
				matrix.M43 = cr32_le(&m_buffer[sizeof(float)*6]);
				matrix.M44 = cr32_le(&m_buffer[sizeof(float)*7]);
			}
			
		}

		void BinaryReader::ReadVector2(Vector2& vec)
		{
			FillBuffer(sizeof(float)*2);
			if (m_isEndianDependent)
			{
				v2x(vec) = cr32_dep(&m_buffer[0]);
				v2y(vec) = cr32_dep(&m_buffer[sizeof(float)]);
			}
			else
			{
				v2x(vec) = cr32_le(&m_buffer[0]);
				v2y(vec) = cr32_le(&m_buffer[sizeof(float)]);
			}
		}
		void BinaryReader::ReadVector3(Vector3& vec)
		{
			FillBuffer(sizeof(float)*2);
			if (m_isEndianDependent)
			{
				v3x(vec) = cr32_dep(&m_buffer[0]);
				v3y(vec) = cr32_dep(&m_buffer[sizeof(float)]);
				v3z(vec) = cr32_dep(&m_buffer[sizeof(float)*2]);
			}
			else
			{
				v3x(vec) = cr32_le(&m_buffer[0]);
				v3y(vec) = cr32_le(&m_buffer[sizeof(float)]);
				v3z(vec) = cr32_le(&m_buffer[sizeof(float)*2]);
			}

		}
		void BinaryReader::ReadVector4(Vector4& vec)
		{
			FillBuffer(sizeof(float)*2);
			if (m_isEndianDependent)
			{
				v4x(vec) = cr32_dep(&m_buffer[0]);
				v4y(vec) = cr32_dep(&m_buffer[sizeof(float)]);
				v4z(vec) = cr32_dep(&m_buffer[sizeof(float)*2]);
				v4w(vec) = cr32_dep(&m_buffer[sizeof(float)*3]);
			}
			else
			{
				v4x(vec) = cr32_le(&m_buffer[0]);
				v4y(vec) = cr32_le(&m_buffer[sizeof(float)]);
				v4z(vec) = cr32_le(&m_buffer[sizeof(float)*2]);
				v4w(vec) = cr32_le(&m_buffer[sizeof(float)*3]);
			}
		}

		void BinaryReader::ReadColor4(Color4& color)
		{
			FillBuffer(sizeof(float)*4);
			if (m_isEndianDependent)
			{
				color.Red = cr32_dep(&m_buffer[0]);
				color.Green = cr32_dep(&m_buffer[sizeof(float)]);
				color.Blue = cr32_dep(&m_buffer[sizeof(float)*2]);
				color.Alpha = cr32_dep(&m_buffer[sizeof(float)*3]);
			}
			else
			{
				color.Red = cr32_le(&m_buffer[0]);
				color.Green = cr32_le(&m_buffer[sizeof(float)]);
				color.Blue = cr32_le(&m_buffer[sizeof(float)*2]);
				color.Alpha = cr32_le(&m_buffer[sizeof(float)*3]);
			}
		}
		void BinaryReader::ReadPlane(Plane& plane)
		{
			FillBuffer(sizeof(float)*4);
			if (m_isEndianDependent)
			{
				plane.X = cr32_dep(&m_buffer[0]);
				plane.Y = cr32_dep(&m_buffer[sizeof(float)]);
				plane.Z = cr32_dep(&m_buffer[sizeof(float)*2]);
				plane.D = cr32_dep(&m_buffer[sizeof(float)*3]);
			}
			else
			{
				plane.X = cr32_le(&m_buffer[0]);
				plane.Y = cr32_le(&m_buffer[sizeof(float)]);
				plane.Z = cr32_le(&m_buffer[sizeof(float)*2]);
				plane.D = cr32_le(&m_buffer[sizeof(float)*3]);
			}
		}
		void BinaryReader::ReadQuaternion(Quaternion& quat)
		{
			FillBuffer(sizeof(float)*4);
			if (m_isEndianDependent)
			{
				quat.X = cr32_dep(&m_buffer[0]);
				quat.Y = cr32_dep(&m_buffer[sizeof(float)]);
				quat.Z = cr32_dep(&m_buffer[sizeof(float)*2]);
				quat.W = cr32_dep(&m_buffer[sizeof(float)*3]);
			}
			else
			{
				quat.X = cr32_le(&m_buffer[0]);
				quat.Y = cr32_le(&m_buffer[sizeof(float)]);
				quat.Z = cr32_le(&m_buffer[sizeof(float)*2]);
				quat.W = cr32_le(&m_buffer[sizeof(float)*3]);
			}
		}
		void BinaryReader::ReadPoint(Point& point)
		{
			FillBuffer(sizeof(int32)*2);
			if (m_isEndianDependent)
			{
				point.X = ci32_dep(&m_buffer[0]);
				point.Y = ci32_dep(&m_buffer[sizeof(int32)]);
			}
			else
			{
				point.X = ci32_le(&m_buffer[0]);
				point.Y = ci32_le(&m_buffer[sizeof(int32)]);
			}
		}
		void BinaryReader::ReadSize(Size& size)
		{
			FillBuffer(sizeof(int32)*2);
			if (m_isEndianDependent)
			{
				size.Width = ci32_dep(&m_buffer[0]);
				size.Height = ci32_dep(&m_buffer[sizeof(int32)]);
			}
			else
			{
				size.Width = ci32_le(&m_buffer[0]);
				size.Height = ci32_le(&m_buffer[sizeof(int32)]);
			}
		}
		void BinaryReader::ReadRectangle(Rectangle& rect)
		{
			FillBuffer(sizeof(int32)*4);
			if (m_isEndianDependent)
			{
				rect.X = ci32_dep(&m_buffer[0]);
				rect.Y = ci32_dep(&m_buffer[sizeof(int32)]);
				rect.Width = ci32_dep(&m_buffer[sizeof(int32)*2]);
				rect.Height = ci32_dep(&m_buffer[sizeof(int32)*3]);
			}
			else
			{
				rect.X = ci32_le(&m_buffer[0]);
				rect.Y = ci32_le(&m_buffer[sizeof(int32)]);
				rect.Width = ci32_le(&m_buffer[sizeof(int32)*2]);
				rect.Height = ci32_le(&m_buffer[sizeof(int32)*3]);
			}
		}
		void BinaryReader::ReadRectangleF(RectangleF& rect)
		{
			FillBuffer(sizeof(float)*4);
			if (m_isEndianDependent)
			{
				rect.X = cr32_dep(&m_buffer[0]);
				rect.Y = cr32_dep(&m_buffer[sizeof(float)]);
				rect.Width = cr32_dep(&m_buffer[sizeof(float)*2]);
				rect.Height = cr32_dep(&m_buffer[sizeof(float)*3]);
			}
			else
			{
				rect.X = cr32_le(&m_buffer[0]);
				rect.Y = cr32_le(&m_buffer[sizeof(float)]);
				rect.Width = cr32_le(&m_buffer[sizeof(float)*2]);
				rect.Height = cr32_le(&m_buffer[sizeof(float)*3]);
			}			
		}
		void BinaryReader::ReadRay(Ray& ray)
		{
			FillBuffer(sizeof(float)*6);

			if (m_isEndianDependent)
			{
				float x = cr32_dep(&m_buffer[0]);
				float y = cr32_dep(&m_buffer[sizeof(float)]);
				float z = cr32_dep(&m_buffer[sizeof(float)*2]);
				ray.Position = Vector3Utils::LDVector(x,y,z);
				x = cr32_dep(&m_buffer[0]);
				y = cr32_dep(&m_buffer[sizeof(float)]);
				z = cr32_dep(&m_buffer[sizeof(float)*2]);
				ray.Direction = Vector3Utils::LDVector(x,y,z);

			}
			else
			{
				float x = cr32_le(&m_buffer[0]);
				float y = cr32_le(&m_buffer[sizeof(float)]);
				float z = cr32_le(&m_buffer[sizeof(float)*2]);
				ray.Position = Vector3Utils::LDVector(x,y,z);
				x = cr32_le(&m_buffer[0]);
				y = cr32_le(&m_buffer[sizeof(float)]);
				z = cr32_le(&m_buffer[sizeof(float)*2]);
				ray.Direction = Vector3Utils::LDVector(x,y,z);

			}
			
		}
		void BinaryReader::ReadBoundingBox(BoundingBox& box)
		{
			FillBuffer(sizeof(float)*6);

			if (m_isEndianDependent)
			{
				float x = cr32_dep(&m_buffer[0]);
				float y = cr32_dep(&m_buffer[sizeof(float)]);
				float z = cr32_dep(&m_buffer[sizeof(float)*2]);
				box.Minimum = Vector3Utils::LDVector(x,y,z);
				x = cr32_dep(&m_buffer[sizeof(float)*3]);
				y = cr32_dep(&m_buffer[sizeof(float)*4]);
				z = cr32_dep(&m_buffer[sizeof(float)*5]);
				box.Maximum = Vector3Utils::LDVector(x,y,z);
			}
			else
			{
				float x = cr32_le(&m_buffer[0]);
				float y = cr32_le(&m_buffer[sizeof(float)]);
				float z = cr32_le(&m_buffer[sizeof(float)*2]);
				box.Minimum = Vector3Utils::LDVector(x,y,z);
				x = cr32_le(&m_buffer[sizeof(float)*3]);
				y = cr32_le(&m_buffer[sizeof(float)*4]);
				z = cr32_le(&m_buffer[sizeof(float)*5]);
				box.Maximum = Vector3Utils::LDVector(x,y,z);
			}
		}
		void BinaryReader::ReadBoundingSphere(BoundingSphere& sphere)
		{
			FillBuffer(sizeof(float)*4);

			if (m_isEndianDependent)
			{
				float x = cr32_dep(&m_buffer[0]);
				float y = cr32_dep(&m_buffer[sizeof(float)]);
				float z = cr32_dep(&m_buffer[sizeof(float)*2]);
				sphere.Center = Vector3Utils::LDVector(x,y,z);
				sphere.Radius = cr32_dep(&m_buffer[sizeof(float)*3]);
			}
			else
			{
				float x = cr32_le(&m_buffer[0]);
				float y = cr32_le(&m_buffer[sizeof(float)]);
				float z = cr32_le(&m_buffer[sizeof(float)*2]);
				sphere.Center = Vector3Utils::LDVector(x,y,z);
				sphere.Radius = cr32_le(&m_buffer[sizeof(float)*3]);
			}
		}

		TaggedDataReader* BinaryReader::ReadTaggedDataBlock()
		{
			uint32 size = ReadUInt32();

			VirtualStream* vs = new VirtualStream(m_baseStream, m_baseStream->getPosition(), size);
			return new TaggedDataReader(vs);
		}

		void BinaryReader::Close() const
		{
			m_baseStream->Close();
		}
	}
}