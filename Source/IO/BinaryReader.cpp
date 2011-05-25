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


#include "BinaryReader.h"

#include "Streams.h"
#include "Math/Math.h"
#include "Math/BoundingBox.h"
#include "Math/BoundingSphere.h"
#include "TaggedData.h"


namespace Apoc3D
{
	namespace IO
	{
		BinaryReader::BinaryReader(Stream* baseStream)
			: m_baseStream(baseStream)
		{
			m_isEndianDependent = baseStream->IsReadEndianDependent();
		}
		BinaryReader::~BinaryReader()
		{
			delete m_baseStream;
		}
		void BinaryReader::FillBuffer(int32 len)
		{
			m_baseStream->Read(&m_buffer[0], len); 
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

		void BinaryReader::ReadVector2(float* buffer)
		{
			FillBuffer(sizeof(float)*2);
			if (m_isEndianDependent)
			{
				buffer[0] = cr32_dep(&m_buffer[0]);
				buffer[1] = cr32_dep(&m_buffer[sizeof(float)]);
			}
			else
			{
				buffer[0] = cr32_le(&m_buffer[0]);
				buffer[1] = cr32_le(&m_buffer[sizeof(float)]);
			}
		}
		void BinaryReader::ReadVector3(float* buffer)
		{
			FillBuffer(sizeof(float)*2);
			if (m_isEndianDependent)
			{
				buffer[0] = cr32_dep(&m_buffer[0]);
				buffer[1] = cr32_dep(&m_buffer[sizeof(float)]);
				buffer[2] = cr32_dep(&m_buffer[sizeof(float)*2]);
			}
			else
			{
				buffer[0] = cr32_le(&m_buffer[0]);
				buffer[1] = cr32_le(&m_buffer[sizeof(float)]);
				buffer[2] = cr32_le(&m_buffer[sizeof(float)*2]);
			}

		}
		void BinaryReader::ReadVector4(float* buffer)
		{
			FillBuffer(sizeof(float)*2);
			if (m_isEndianDependent)
			{
				buffer[0] = cr32_dep(&m_buffer[0]);
				buffer[1] = cr32_dep(&m_buffer[sizeof(float)]);
				buffer[2] = cr32_dep(&m_buffer[sizeof(float)*2]);
				buffer[3] = cr32_dep(&m_buffer[sizeof(float)*3]);
			}
			else
			{
				buffer[0] = cr32_le(&m_buffer[0]);
				buffer[1] = cr32_le(&m_buffer[sizeof(float)]);
				buffer[2] = cr32_le(&m_buffer[sizeof(float)*2]);
				buffer[3] = cr32_le(&m_buffer[sizeof(float)*3]);
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

		const TaggedDataReader* BinaryReader::ReadTaggedDataBlock()
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