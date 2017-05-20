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

#include "BinaryReader.h"

#include "Streams.h"
#include "TaggedData.h"
#include "IOUtils.h"

#include "apoc3d/Exception.h"
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
		BinaryReader::BinaryReader(Stream* baseStream, bool releaseStream)
			: m_baseStream(baseStream), m_shouldDeleteStream(releaseStream)
		{
			m_isEndianIndependent = baseStream->IsReadEndianIndependent();
		}

		BinaryReader::BinaryReader(const ResourceLocation& rsloc)
			: m_shouldDeleteStream(true)
		{
			m_baseStream = rsloc.GetReadStream();
			m_isEndianIndependent = m_baseStream->IsReadEndianIndependent();
		}
		BinaryReader::~BinaryReader()
		{
			if (m_shouldDeleteStream)
				delete m_baseStream;
			m_baseStream = nullptr;
		}


		void BinaryReader::FillBuffer(int32 len)
		{
			int64 result = m_baseStream->Read(&m_buffer[0], len); 
			if (len != result)
			{
				throwEndofStreamException();
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
#ifdef BIG_ENDIAN
			if (!m_isEndianIndependent)
			{
				return cr64_dep(m_buffer);
			}
#endif
			return cr64_le(m_buffer);
		}
		float BinaryReader::ReadSingle()
		{
			FillBuffer(sizeof(float));
#ifdef BIG_ENDIAN
			if (!m_isEndianIndependent)
			{
				return cr32_dep(m_buffer);
			}
#endif
			return cr32_le(m_buffer);
		}

		template <typename T, typename ET>
		void BinaryReader::FillString(T& str, int32 len)
		{
			int32 idx = 0;

			const int32 BufferStringCharCount = sizeof(m_buffer) / sizeof(ET);

			while (len >= BufferStringCharCount)
			{
				m_baseStream->Read(m_buffer, sizeof(m_buffer));

				if (sizeof(ET) == 1)
				{
					for (int32 i = 0; i < BufferStringCharCount; i++)
						str[idx++] = m_buffer[i];
				}
				else
				{
					for (int32 i = 0; i < BufferStringCharCount; i++)
						str[idx++] = static_cast<T::value_type>(InterpretInt16(m_buffer + i * sizeof(ET)));
				}

				len -= BufferStringCharCount;
			}

			if (len > 0)
			{
				m_baseStream->Read(m_buffer, len * sizeof(ET));

				if (sizeof(ET) == 1)
				{
					for (int32 i = 0; i < len; i++)
						str[idx++] = m_buffer[i];
				}
				else
				{
					for (int32 i = 0; i < len; i++)
						str[idx++] = static_cast<T::value_type>(InterpretInt16(m_buffer + i * sizeof(ET)));
				}
			}
		}

		int16 BinaryReader::InterpretInt16(const char* data) const
		{
#ifdef BIG_ENDIAN
			if (!m_isEndianIndependent)
			{
				return ci16_dep(data);
			}
#endif
			return ci16_le(data);
		}

		String BinaryReader::ReadString()
		{
			uint32 len = ReadUInt32();

			if ((len & 0x80000000U) == 0x80000000U)
			{
				len &= 0x7FFFFFFFU;

				String str(len, ' ');
				
				FillString<String, char>(str, len);
				return str;
			}

			String str(len, ' ');
			FillString<String, int16>(str, len);
			return str;
		}

		std::string BinaryReader::ReadMBString()
		{
			uint32 len = ReadUInt32();

			std::string str(len, ' ');
			FillString<std::string, char>(str, len);

			return str;
		}

		int16 BinaryReader::ReadInt16() 
		{
			FillBuffer(sizeof(int16));
			return InterpretInt16(m_buffer);
		}
		int32 BinaryReader::ReadInt32() 
		{
			FillBuffer(sizeof(int32));
#ifdef BIG_ENDIAN
			if (!m_isEndianIndependent)
			{
				return ci32_dep(m_buffer);
			}
#endif
			return ci32_le(m_buffer);
		}
		int64 BinaryReader::ReadInt64() 
		{
			FillBuffer(sizeof(int64));
#ifdef BIG_ENDIAN
			if (!m_isEndianIndependent)
			{
				return ci64_dep(m_buffer);
			}
#endif
			return ci64_le(m_buffer);
		}

		uint16 BinaryReader::ReadUInt16() 
		{
			FillBuffer(sizeof(uint16));
#ifdef BIG_ENDIAN
			if (!m_isEndianIndependent)
			{
				return cui16_dep(m_buffer);
			}
#endif
			return cui16_le(m_buffer);
		}
		uint32 BinaryReader::ReadUInt32() 
		{
			FillBuffer(sizeof(uint32));
#ifdef BIG_ENDIAN
			if (!m_isEndianIndependent)
			{
				return cui32_dep(m_buffer);
			}
#endif
			return cui32_le(m_buffer);
		}
		uint64 BinaryReader::ReadUInt64() 
		{
			FillBuffer(sizeof(uint64));
#ifdef BIG_ENDIAN
			if (!m_isEndianIndependent)
			{
				return cui64_dep(m_buffer);
			}
#endif
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
				 throwEndofStreamException();
			}
			return reinterpret_cast<const char&>(r);
		}

		void BinaryReader::ReadMatrix(Matrix& matrix)
		{
			FillBuffer(sizeof(float)*8);
#ifdef BIG_ENDIAN
			if (!m_isEndianIndependent)
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
#endif
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
#ifdef BIG_ENDIAN
			if (!m_isEndianIndependent)
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
#endif
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
#ifdef BIG_ENDIAN
			if (!m_isEndianIndependent)
			{
				vec.X = cr32_dep(&m_buffer[0]);
				vec.Y = cr32_dep(&m_buffer[sizeof(float)]);
			}
			else
#endif
			{
				vec.X = cr32_le(&m_buffer[0]);
				vec.Y = cr32_le(&m_buffer[sizeof(float)]);
			}
		}
		void BinaryReader::ReadVector3(Vector3& vec)
		{
			FillBuffer(sizeof(float)*3);
#ifdef BIG_ENDIAN
			if (!m_isEndianIndependent)
			{
				vec.X = cr32_dep(&m_buffer[0]);
				vec.Y = cr32_dep(&m_buffer[sizeof(float)]);
				vec.Z = cr32_dep(&m_buffer[sizeof(float)*2]);
			}
			else
#endif
			{
				vec.X = cr32_le(&m_buffer[0]);
				vec.Y = cr32_le(&m_buffer[sizeof(float)]);
				vec.Z = cr32_le(&m_buffer[sizeof(float)*2]);
			}

		}
		void BinaryReader::ReadVector4(Vector4& vec)
		{
			FillBuffer(sizeof(float)*4);
#ifdef BIG_ENDIAN
			if (!m_isEndianIndependent)
			{
				vec.X = cr32_dep(&m_buffer[0]);
				vec.Y = cr32_dep(&m_buffer[sizeof(float)]);
				vec.Z = cr32_dep(&m_buffer[sizeof(float)*2]);
				vec.W = cr32_dep(&m_buffer[sizeof(float)*3]);
			}
			else
#endif
			{
				vec.X = cr32_le(&m_buffer[0]);
				vec.Y = cr32_le(&m_buffer[sizeof(float)]);
				vec.Z = cr32_le(&m_buffer[sizeof(float)*2]);
				vec.W = cr32_le(&m_buffer[sizeof(float)*3]);
			}
		}

		void BinaryReader::ReadColor4(Color4& color)
		{
			FillBuffer(sizeof(float)*4);
#ifdef BIG_ENDIAN
			if (!m_isEndianIndependent)
			{
				color.Red = cr32_dep(&m_buffer[0]);
				color.Green = cr32_dep(&m_buffer[sizeof(float)]);
				color.Blue = cr32_dep(&m_buffer[sizeof(float)*2]);
				color.Alpha = cr32_dep(&m_buffer[sizeof(float)*3]);
			}
			else
#endif
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
#ifdef BIG_ENDIAN
			if (!m_isEndianIndependent)
			{
				plane.X = cr32_dep(&m_buffer[0]);
				plane.Y = cr32_dep(&m_buffer[sizeof(float)]);
				plane.Z = cr32_dep(&m_buffer[sizeof(float)*2]);
				plane.D = cr32_dep(&m_buffer[sizeof(float)*3]);
			}
			else
#endif
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
#ifdef BIG_ENDIAN
			if (!m_isEndianIndependent)
			{
				quat.X = cr32_dep(&m_buffer[0]);
				quat.Y = cr32_dep(&m_buffer[sizeof(float)]);
				quat.Z = cr32_dep(&m_buffer[sizeof(float)*2]);
				quat.W = cr32_dep(&m_buffer[sizeof(float)*3]);
			}
			else
#endif
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
#ifdef BIG_ENDIAN
			if (!m_isEndianIndependent)
			{
				point.X = ci32_dep(&m_buffer[0]);
				point.Y = ci32_dep(&m_buffer[sizeof(int32)]);
			}
			else
#endif
			{
				point.X = ci32_le(&m_buffer[0]);
				point.Y = ci32_le(&m_buffer[sizeof(int32)]);
			}
		}
		void BinaryReader::ReadSize(Size& size)
		{
			FillBuffer(sizeof(int32)*2);
#ifdef BIG_ENDIAN
			if (!m_isEndianIndependent)
			{
				size.Width = ci32_dep(&m_buffer[0]);
				size.Height = ci32_dep(&m_buffer[sizeof(int32)]);
			}
			else
#endif
			{
				size.Width = ci32_le(&m_buffer[0]);
				size.Height = ci32_le(&m_buffer[sizeof(int32)]);
			}
		}
		void BinaryReader::ReadRectangle(Rectangle& rect)
		{
			FillBuffer(sizeof(int32)*4);
#ifdef BIG_ENDIAN
			if (!m_isEndianIndependent)
			{
				rect.X = ci32_dep(&m_buffer[0]);
				rect.Y = ci32_dep(&m_buffer[sizeof(int32)]);
				rect.Width = ci32_dep(&m_buffer[sizeof(int32)*2]);
				rect.Height = ci32_dep(&m_buffer[sizeof(int32)*3]);
			}
			else
#endif
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
#ifdef BIG_ENDIAN
			if (!m_isEndianIndependent)
			{
				rect.X = cr32_dep(&m_buffer[0]);
				rect.Y = cr32_dep(&m_buffer[sizeof(float)]);
				rect.Width = cr32_dep(&m_buffer[sizeof(float)*2]);
				rect.Height = cr32_dep(&m_buffer[sizeof(float)*3]);
			}
			else
#endif
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
#ifdef BIG_ENDIAN
			if (!m_isEndianIndependent)
			{
				float x = cr32_dep(&m_buffer[0]);
				float y = cr32_dep(&m_buffer[sizeof(float)]);
				float z = cr32_dep(&m_buffer[sizeof(float)*2]);
				ray.Position = Vector3(x,y,z);
				x = cr32_dep(&m_buffer[0]);
				y = cr32_dep(&m_buffer[sizeof(float)]);
				z = cr32_dep(&m_buffer[sizeof(float)*2]);
				ray.Direction = Vector3(x,y,z);
			}
			else
#endif
			{
				float x = cr32_le(&m_buffer[0]);
				float y = cr32_le(&m_buffer[sizeof(float)]);
				float z = cr32_le(&m_buffer[sizeof(float)*2]);
				ray.Position = Vector3(x,y,z);
				x = cr32_le(&m_buffer[0]);
				y = cr32_le(&m_buffer[sizeof(float)]);
				z = cr32_le(&m_buffer[sizeof(float)*2]);
				ray.Direction = Vector3(x,y,z);

			}
			
		}
		void BinaryReader::ReadBoundingBox(BoundingBox& box)
		{
			FillBuffer(sizeof(float)*6);
#ifdef BIG_ENDIAN
			if (!m_isEndianIndependent)
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
#endif
			{
				float x = cr32_le(&m_buffer[0]);
				float y = cr32_le(&m_buffer[sizeof(float)]);
				float z = cr32_le(&m_buffer[sizeof(float)*2]);
				box.Minimum = Vector3(x,y,z);
				x = cr32_le(&m_buffer[sizeof(float)*3]);
				y = cr32_le(&m_buffer[sizeof(float)*4]);
				z = cr32_le(&m_buffer[sizeof(float)*5]);
				box.Maximum = Vector3(x,y,z);
			}
		}
		void BinaryReader::ReadBoundingSphere(BoundingSphere& sphere)
		{
			FillBuffer(sizeof(float)*4);
#ifdef BIG_ENDIAN
			if (!m_isEndianIndependent)
			{
				float x = cr32_dep(&m_buffer[0]);
				float y = cr32_dep(&m_buffer[sizeof(float)]);
				float z = cr32_dep(&m_buffer[sizeof(float)*2]);
				sphere.Center = Vector3(x,y,z);
				sphere.Radius = cr32_dep(&m_buffer[sizeof(float)*3]);
			}
			else
#endif
			{
				float x = cr32_le(&m_buffer[0]);
				float y = cr32_le(&m_buffer[sizeof(float)]);
				float z = cr32_le(&m_buffer[sizeof(float)*2]);
				sphere.Center = Vector3(x,y,z);
				sphere.Radius = cr32_le(&m_buffer[sizeof(float)*3]);
			}
		}

		void BinaryReader::ReadSingle(float* data, int32 count)	 { for (int32 i = 0; i < count; i++) data[i] = ReadSingle(); }
		void BinaryReader::ReadDouble(double* data, int32 count) { for (int32 i = 0; i < count; i++) data[i] = ReadDouble(); }
		void BinaryReader::ReadInt32(int32* data, int32 count)	 { for (int32 i = 0; i < count; i++) data[i] = ReadInt32(); }
		void BinaryReader::ReadUInt32(uint32* data, int32 count) { for (int32 i = 0; i < count; i++) data[i] = ReadUInt32(); }
		void BinaryReader::ReadBoolean(bool* data, int32 count)	 { for (int32 i = 0; i < count; i++) data[i] = ReadBoolean(); }

		void BinaryReader::ReadBooleanBits(bool* arr, int32 count)
		{
			for (int32 i = 0; i < count; i += 8)
			{
				byte bits = ReadByte();

				for (int32 j = 0; j < 8 && i + j < count; j++)
					arr[i + j] = (bits & (1 << j)) != 0;
			}
		}



		TaggedDataReader* BinaryReader::ReadTaggedDataBlock()
		{
			uint32 size = ReadUInt32();

			VirtualStream* vs = new VirtualStream(m_baseStream, m_baseStream->getPosition(), size);
			return new TaggedDataReader(vs);
		}

		void BinaryReader::ReadTaggedDataBlock(FunctorReference<void(TaggedDataReader*)> f, bool continuous)
		{
			uint32 size = ReadUInt32();

			VirtualStream vs(m_baseStream, m_baseStream->getPosition(), size);
			{
				TaggedDataReader data(&vs);
				//TaggedDataReader data(m_baseStream);
				data.SuspendStreamRelease();

				f(&data);

				data.Close(continuous);
			}
		}


		void BinaryReader::throwEndofStreamException()
		{
			throw AP_EXCEPTION(ExceptID::EndOfStream, L"");
		}
	}
}