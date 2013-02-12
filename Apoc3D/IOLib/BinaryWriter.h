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

#ifndef APOC3D_BINARYWRITER_H
#define APOC3D_BINARYWRITER_H

#include "apoc3d/Common.h"

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
			void Write(char byte) const;

			void Write(bool value) const;

			void Write(double value) const;
			void Write(float value) const;
			void Write(const String& value) const;
			void Write(const std::string& value) const;

			void Write(int16 value) const;
			void Write(int32 value) const;
			void Write(int64 value) const;
			void Write(uint16 value) const;
			void Write(uint32 value) const;
			void Write(uint64 value) const;

			void Write(const Color4& value) const;
			void Write(const Plane& plane) const;
			void Write(const Matrix& matrix) const;
			void Write(const Quaternion& quat) const;
			void Write(const Point& point) const;
			void Write(const Size& size) const;
			void Write(const Apoc3D::Math::Rectangle& rect) const;
			void Write(const Apoc3D::Math::RectangleF& rect) const;

			void Write(const Ray& ray) const;
			void Write(const BoundingBox& sphere) const;
			void Write(const BoundingSphere& box) const;

			void WriteVector2(const float* buffer) const;
			void WriteVector3(const float* buffer) const;
			void WriteVector4(const float* buffer) const;

			void Write(const TaggedDataWriter* data) const;

			void Write(const float* data, int32 count) const { for (int32 i=0;i<count;i++) Write(data[i]); }
			void Write(const double* data, int32 count) const { for (int32 i=0;i<count;i++) Write(data[i]); }
			void Write(const int32* data, int32 count) const { for (int32 i=0;i<count;i++) Write(data[i]); }
			void Write(const bool* data, int32 count) const { for (int32 i=0;i<count;i++) Write(data[i]); }

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