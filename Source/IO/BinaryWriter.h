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

			inline void Write(const char* bytes, int64 count) const;
			inline void Write(char byte) const;

			inline void Write(bool value) const;

			inline void Write(double value) const;
			inline void Write(float value) const;
			inline void Write(const String& value) const;			
			inline void Write(int16 value) const;
			inline void Write(int32 value) const;
			inline void Write(int64 value) const;
			inline void Write(uint16 value) const;
			inline void Write(uint32 value) const;
			inline void Write(uint64 value) const;

			inline void Write(const Color4& value) const;
			inline void Write(const Plane& plane) const;
			inline void Write(const Matrix& matrix) const;
			inline void Write(const Quaternion& quat) const;
			inline void Write(const Point& point) const;
			inline void Write(const Size& size) const;
			inline void Write(const Rectangle& rect) const;
			inline void Write(const RectangleF& rect) const;

			inline void Write(const Ray& ray) const;
			inline void Write(const BoundingBox& sphere) const;
			inline void Write(const BoundingSphere& box) const;

			inline void WriteVector2(const float* buffer) const;
			inline void WriteVector3(const float* buffer) const;
			inline void WriteVector4(const float* buffer) const;

			void Write(const TaggedDataWriter* data) const;
			inline void Close() const;
		};
	}
}
#endif