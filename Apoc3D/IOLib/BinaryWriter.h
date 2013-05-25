#pragma once
#ifndef APOC3D_BINARYWRITER_H
#define APOC3D_BINARYWRITER_H

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
#include "apoc3d/Math/Vector.h"

using namespace Apoc3D::Math;

namespace Apoc3D
{
	namespace IO
	{
		class APAPI BinaryWriter
		{
		public:
			BinaryWriter(Stream* baseStream);
			~BinaryWriter();

			void Write(const char* bytes, int64 count) const;
			void Write(char byte) const		{ WriteByte(byte); }

			void Write(bool value) const	{ WriteBoolean(value); }

			void Write(double value) const	{ WriteDouble(value); }
			void Write(float value) const	{ WriteSingle(value); }
			void Write(const String& value) const		{ WriteString(value); }
			void Write(const std::string& value) const	{ WriteMBString(value); }

			void Write(int16 value) const	{ WriteInt16(value); }
			void Write(int32 value) const	{ WriteInt32(value); }
			void Write(int64 value) const	{ WriteInt64(value); }
			void Write(uint16 value) const	{ WriteUInt16(value); }
			void Write(uint32 value) const	{ WriteUInt32(value); }
			void Write(uint64 value) const	{ WriteUInt64(value); }

			void Write(const Color4& value) const	{ WriteColor4(value); }
			void Write(const Plane& plane) const	{ WritePlane(plane); }
			void Write(const Matrix& matrix) const	{ WriteMatrix(matrix); }
			void Write(const Quaternion& quat) const	{ WriteQuaternion(quat); }
			void Write(const Point& point) const		{ WritePoint(point); }
			void Write(const Size& size) const			{ WriteSize(size); }
			void Write(const Apoc3D::Math::Rectangle& rect) const	{ WriteRectangle(rect); }
			void Write(const Apoc3D::Math::RectangleF& rect) const	{ WriteRectangleF(rect); }

			void Write(const Ray& ray) const				{ WriteRay(ray); }
			void Write(const BoundingBox& box) const		{ WriteBoundingBox(box); }
			void Write(const BoundingSphere& sphere) const { WriteBoundingSphere(sphere); }

			void Write(const TaggedDataWriter* data) const	{ WriteTaggedDataBlock(data); }

			void Write(const float* data, int32 count) const { WriteSingle(data, count); }
			void Write(const double* data, int32 count) const { WriteDouble(data, count); }
			void Write(const int32* data, int32 count) const { WriteInt32(data, count); }
			void Write(const bool* data, int32 count) const { WriteBoolean(data, count); }


			void WriteByte(char byte) const;
			void WriteBoolean(bool value) const;

			void WriteDouble(double value) const;
			void WriteSingle(float value) const;
			void WriteString(const String& value) const;
			void WriteMBString(const std::string& value) const;

			void WriteInt16(int16 value) const;
			void WriteInt32(int32 value) const;
			void WriteInt64(int64 value) const;
			void WriteUInt16(uint16 value) const;
			void WriteUInt32(uint32 value) const;
			void WriteUInt64(uint64 value) const;

			void WriteColor4(const Color4& value) const;
			void WritePlane(const Plane& plane) const;
			void WriteMatrix(const Matrix& matrix) const;
			void WriteQuaternion(const Quaternion& quat) const;
			void WritePoint(const Point& point) const;
			void WriteSize(const Size& size) const;
			void WriteRectangle(const Apoc3D::Math::Rectangle& rect) const;
			void WriteRectangleF(const Apoc3D::Math::RectangleF& rect) const;

			void WriteRay(const Ray& ray) const;
			void WriteBoundingBox(const BoundingBox& box) const;
			void WriteBoundingSphere(const BoundingSphere& sphere) const;

			void WriteTaggedDataBlock(const TaggedDataWriter* data) const;

			void WriteVector2(const Vector2& vec) const;
			void WriteVector3(const Vector3& vec) const;
			void WriteVector4(const Vector4& vec) const;

			void WriteSingle(const float* data, int32 count) const;
			void WriteDouble(const double* data, int32 count) const;
			void WriteInt32(const int32* data, int32 count) const;
			void WriteBoolean(const bool* data, int32 count) const;

			void Close() const;

			/**
			 * Ask the BinaryWriter not to delete the base stream upon release
			 */
			void SuspendStreamRelease() { m_shouldDeleteStream = false; }

			Stream* getBaseStream() const { return m_baseStream; }

		private:
			bool m_endianDependent;
			bool m_shouldDeleteStream;

			Stream* m_baseStream;
		};
	}
}
#endif