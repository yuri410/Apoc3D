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

#ifndef BINARYWRITER_H
#define BINARYWRITER_H

#include "Common.h"

using namespace Apoc3D::Math;

namespace Apoc3D
{
	namespace IO
	{
		class APAPI BinaryWriter
		{
		private:
			bool m_endianDependent;
			Stream* m_baseStream;

		public:
			Stream* getBaseStream() const { return m_baseStream; }

			BinaryWriter(Stream* baseStream);
			~BinaryWriter();

			void Write(const char* bytes, int64 count) const;
			void Write(char byte) const;

			void Write(bool value) const;

			void Write(double value) const;
			void Write(float value) const;
			void Write(const String& value) const;			
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
			void Close() const;
		};
	}
}
#endif