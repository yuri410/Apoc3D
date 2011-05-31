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

#ifndef BINARYREADER_H
#define BINARYREADER_H

#include "Common.h"

using namespace Apoc3D::Math;
using namespace Apoc3D::VFS;

namespace Apoc3D
{
	namespace IO
	{
		class APAPI BinaryReader
		{
		private:
			bool m_isEndianDependent;
			Stream* m_baseStream;

			char m_buffer[32];

			inline void FillBuffer(int32 len);

		public:

			Stream* getBaseStream() const { return m_baseStream; }

			BinaryReader(Stream* baseStream);
			BinaryReader(const ResourceLocation* baseStream);

			~BinaryReader();

			inline int64 ReadBytes(char* dest, int64 count) const;
			inline char ReadByte() const;

			bool ReadBoolean();

			double ReadDouble();
			float ReadSingle();
			String ReadString();

			int16 ReadInt16();
			int32 ReadInt32();
			int64 ReadInt64();

			uint16 ReadUInt16();
			uint32 ReadUInt32();
			uint64 ReadUInt64();
			inline void ReadColor4(Color4& color);

			inline void ReadPlane(Plane& plane);

			inline void ReadMatrix(Matrix& matrix);
			inline void ReadQuaternion(Quaternion& quat);

			inline void ReadPoint(Point& point);
			inline void ReadSize(Size& size);

			inline void ReadRectangle(Apoc3D::Math::Rectangle& rect);
			inline void ReadRectangleF(Apoc3D::Math::RectangleF& rect);

			inline void ReadRay(Ray& ray);

			inline void ReadBoundingSphere(BoundingSphere& sphere);
			inline void ReadBoundingBox(BoundingBox& box);

			inline void ReadVector4(float* buffer);
			inline void ReadVector3(float* buffer);
			inline void ReadVector2(float* buffer);

			TaggedDataReader* ReadTaggedDataBlock();

			inline void Close() const;
		};
	};
};

#endif