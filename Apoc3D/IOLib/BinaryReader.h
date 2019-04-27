#pragma once
#ifndef APOC3D_BINARYREADER_H
#define APOC3D_BINARYREADER_H

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

#include "apoc3d/ApocCommon.h"
#include "apoc3d/Math/Vector.h"

using namespace Apoc3D::Math;
using namespace Apoc3D::VFS;

namespace Apoc3D
{
	namespace IO
	{
		class APAPI BinaryReader
		{
		public:
			BinaryReader(Stream* baseStream, bool releaseStream);
			BinaryReader(const ResourceLocation& rsloc);

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

			void ReadVector4(Vector4& vec);
			void ReadVector3(Vector3& vec);
			void ReadVector2(Vector2& vec);


			void ReadSingle(float* data, int32 count);
			void ReadDouble(double* data, int32 count);
			void ReadInt32(int32* data, int32 count);
			void ReadUInt32(uint32* data, int32 count);
			void ReadBoolean(bool* data, int32 count);

			void ReadBooleanBits(bool* arr, int32 count);

			TaggedDataReader* ReadTaggedDataBlock();
			void ReadTaggedDataBlock(FunctorReference<void(TaggedDataReader*)> f, bool continuous = true);

			


			template <int32 N> void ReadSingle(float(&data)[N]) { return ReadSingle(data, N); }
			template <int32 N> void ReadDouble(double (&data)[N]) { return ReadDouble(data, N); }
			template <int32 N> void ReadInt32(int32 (&data)[N]) { return ReadInt32(data, N); }
			template <int32 N> void ReadUInt32(uint32 (&data)[N]) { return ReadUInt32(data, N); }
			template <int32 N> void ReadBoolean(bool (&data)[N]) { return ReadBoolean(data, N); }
			template <int32 N> void ReadBooleanBits(bool (&data)[N]) { return ReadBooleanBits(data, N); }

			Stream* getBaseStream() const { return m_baseStream; }

			/** Ask the BinaryWriter not to delete the base stream upon release */
			void SuspendStreamRelease() { m_shouldDeleteStream = false; }
		
		private:
			void FillBuffer(int32 len);

			NO_INLINE static void EndofStreamError();

			template <typename T, typename ET>
			void FillString(T& str, int32 len);

			int16 InterpretInt16(const char* data) const;

			bool m_isEndianIndependent;
			bool m_shouldDeleteStream;

			Stream* m_baseStream;

			char m_buffer[32];

		};
	};
};

#endif