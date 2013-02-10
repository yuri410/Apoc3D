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

#ifndef APOC3D_BINARYREADER_H
#define APOC3D_BINARYREADER_H

#include "Common.h"

using namespace Apoc3D::Math;
using namespace Apoc3D::VFS;

namespace Apoc3D
{
	namespace IO
	{
		class APAPI BinaryReader
		{
		public:
			BinaryReader(Stream* baseStream);
			BinaryReader(const ResourceLocation* baseStream);

			~BinaryReader();

			int64 ReadBytes(char* dest, int64 count) const;
			char ReadByte() const;

			bool ReadBoolean();

			double ReadDouble();
			float ReadSingle();
			String ReadString();
			std::string ReadMBString();

			int16 ReadInt16();
			int32 ReadInt32();
			int64 ReadInt64();

			uint16 ReadUInt16();
			uint32 ReadUInt32();
			uint64 ReadUInt64();
			void ReadColor4(Color4& color);

			void ReadPlane(Plane& plane);

			void ReadMatrix(Matrix& matrix);
			void ReadQuaternion(Quaternion& quat);

			void ReadPoint(Point& point);
			void ReadSize(Size& size);

			void ReadRectangle(Apoc3D::Math::Rectangle& rect);
			void ReadRectangleF(Apoc3D::Math::RectangleF& rect);

			void ReadRay(Ray& ray);

			void ReadBoundingSphere(BoundingSphere& sphere);
			void ReadBoundingBox(BoundingBox& box);

			void ReadVector4(float* buffer);
			void ReadVector3(float* buffer);
			void ReadVector2(float* buffer);


			void ReadSingle(float* data, int32 count) { for (int32 i=0;i<count;i++) data[i] = ReadSingle(); }
			void ReadDouble(double* data, int32 count) { for (int32 i=0;i<count;i++) data[i] = ReadDouble(); }
			void ReadInt32(int32* data, int32 count) { for (int32 i=0;i<count;i++) data[i] = ReadInt32(); }
			void ReadBoolean(bool* data, int32 count) { for (int32 i=0;i<count;i++) data[i] = ReadBoolean(); }



			TaggedDataReader* ReadTaggedDataBlock();

			void Close() const;

			Stream* getBaseStream() const { return m_baseStream; }

			/**
			 * Ask the BinaryWriter not to delete the base stream upon release
			 */
			void SuspendStreamRelease() { m_shouldDeleteStream = false; }
		private:
			inline void FillBuffer(int32 len);

			bool m_isEndianDependent;
			bool m_shouldDeleteStream;

			Stream* m_baseStream;

			char m_buffer[32];

		};
	};
};

#endif