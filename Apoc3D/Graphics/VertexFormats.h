#pragma once
#ifndef APOC3D_VERTEXFORMATS_H
#define APOC3D_VERTEXFORMATS_H

/* -----------------------------------------------------------------------
 * This source file is part of Apoc3D Framework
 * 
 * Copyright (c) 2009-2018 Tao Xin
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

#include "RenderSystem/VertexElement.h"
#include "apoc3d/Math/Vector.h"
#include "apoc3d/Collections/List.h"

using namespace Apoc3D::Math;
using namespace Apoc3D::Core;
using namespace Apoc3D::Collections;
using namespace Apoc3D::Graphics;
using namespace Apoc3D::Graphics::RenderSystem;

namespace Apoc3D
{
	namespace Graphics
	{
		struct APAPI VertexPIBNT1
		{
		private:
			static List<VertexElement> GetElements();
		public:
			static const List<VertexElement> Elements;

			Vector3 Position;
			Vector4 BlendIndices;
			Vector4 BlendWeight;
			Vector3 Normal;
			Vector2 TexCoord1;

			uint64 GetHashCode() const
			{
				return Position.GetHashCode() ^ Normal.GetHashCode() ^ TexCoord1.GetHashCode() ^
					BlendWeight.GetHashCode() ^ BlendIndices.GetHashCode();
			}
			String GetHashString() const;
		};

		struct APAPI VertexPN
		{
		private:
			static List<VertexElement> GetElements();
		public:
			static const List<VertexElement> Elements;

			Vector3 Position;
			Vector3 Normal;

			uint64 GetHashCode() const
			{
				return Position.GetHashCode() ^ Normal.GetHashCode();
			}
			String GetHashString() const;
		};

		struct APAPI VertexPNT1
		{
		private:
			static List<VertexElement> GetElements();
		public:
			static const List<VertexElement> Elements;

			Vector3 Position;
			Vector3 Normal;
			Vector2 TexCoord1;


			uint64 GetHashCode() const
			{
				return Position.GetHashCode() ^ Normal.GetHashCode() ^ TexCoord1.GetHashCode();
			}
			String GetHashString() const;
		};

		struct APAPI VertexPNT2
		{
		private:
			static List<VertexElement> GetElements();
		public:
			static const List<VertexElement> Elements;

			Vector3 Position;
			Vector3 Normal;
			Vector2 TexCoord1;
			Vector2 TexCoord2;

			uint64 GetHashCode() const
			{
				return Position.GetHashCode() ^ Normal.GetHashCode() ^ 
					TexCoord1.GetHashCode() ^ TexCoord2.GetHashCode();
			}
			String GetHashString() const;
		};

		struct APAPI VertexPT1
		{
		private:
			static List<VertexElement> GetElements();
		public:
			static const List<VertexElement> Elements;

			Vector3 Position;
			Vector2 TexCoord1;

			uint64 GetHashCode() const
			{
				return Position.GetHashCode() ^ TexCoord1.GetHashCode();
			}
			String GetHashString() const;
		};
		struct APAPI VertexPT2
		{
		private:
			static List<VertexElement> GetElements();
		public:
			static const List<VertexElement> Elements;

			Vector3 Position;
			Vector2 TexCoord1;
			Vector2 TexCoord2;

			uint64 GetHashCode() const
			{
				return Position.GetHashCode() ^ TexCoord1.GetHashCode() ^ TexCoord2.GetHashCode();
			}
			String GetHashString() const;
		};

	}
}

#endif