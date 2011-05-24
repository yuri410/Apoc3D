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

namespace Apoc3D
{
	namespace IO
	{
		class APAPI BinaryReader
		{
		private:
			Stream* m_baseStream;

			char m_buffer[32];

			inline void FillBuffer(int32 len);

		public:

			Stream* getBaseStream() const { return m_baseStream; }

			BinaryReader(Stream* baseStream)
				: m_baseStream(baseStream)
			{
			}
			~BinaryReader();
			double ReadDouble()
			{
				FillBuffer(sizeof(double));
				return cr64_le(m_buffer);
			}
			float ReadSingle()
			{
				FillBuffer(sizeof(float));
				return cr32_le(m_buffer);
			}
			String ReadString()
			{
				int32 len = ReadInt32();
				
				wchar_t* const chars = new wchar_t[len];

				for (int i=0;i<len;i++)
				{
					chars[i] = ReadInt16();
				}
				String str = String(chars, len);
				
				delete chars;
				return str;
			}

			int16 ReadInt16() 
			{
				FillBuffer(sizeof(int16));
				return ci16_le(m_buffer);
			}
			int32 ReadInt32() 
			{
				FillBuffer(sizeof(int32));
				return ci32_le(m_buffer);
			}
			int64 ReadInt64() 
			{
				FillBuffer(sizeof(int64));
				return ci64_le(m_buffer);
			}

			uint16 ReadUInt16() 
			{
				FillBuffer(sizeof(uint16));
				return cui16_le(m_buffer);
			}
			uint32 ReadUInt32() 
			{
				FillBuffer(sizeof(uint32));
				return cui32_le(m_buffer);
			}
			uint64 ReadUInt64() 
			{
				FillBuffer(sizeof(uint64));
				return cui64_le(m_buffer);
			}
			inline void ReadColor4(Color4& color);

			inline void ReadPlane(Plane& plane);

			inline void ReadMatrix(Matrix& matrix);
			inline void ReadQuaternion(Quaternion& quat);

			inline void ReadPoint(Point& point);
			inline void ReadSize(Size& size);

			inline void ReadRectangle(Rectangle& rect);
			inline void ReadRectangleF(RectangleF& rect);

			inline void ReadRay(Ray& ray);

			inline void ReadBoundingSphere(BoundingSphere& sphere);
			inline void ReadBoundingBox(BoundingBox& box);

			inline void ReadVector4(float* buffer);
			inline void ReadVector3(float* buffer);
			inline void ReadVector2(float* buffer);

			const TaggedDataReader* ReadTaggedDataBlock();

			inline void Close();
		};
	};
};

#endif